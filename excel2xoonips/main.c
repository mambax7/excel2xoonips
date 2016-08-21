/*
 * excel2xoonips - A data convertor from Excel data to XooNIps import file
 *
 * Copyright (C) 2007  RIKEN Japan, All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <limits.h>

#include "cole.h"
#include "libsl4.h"
#include "cexcel.h"

#include "excel2xoonips.h"

#define XLS2XNI_ERROR_SUCCESS  0

static void print_usage( const char *bin );
static int proc_main( const char *bin, const char *xls_file,
                      const char *zip_file, int is_index );
static int proc_sheet( cexcel_sheet_t * sheet, sl4_string_t *tmpdir,
                       int *item_id, const char *bindir,
                       sl4_string_t *errmes, int is_index );

struct option longopts[] = {
  {"index",      no_argument,       0,  'i' },
  {"compat34",   no_argument,       0,  'c' },
  {"help",       no_argument,       0,  'h' },
  {0,            0,                 0,  0 }
};

int main( int argc, char *argv[] )
{
  int ret, is_index;
  int long_index, opt;
  const char *xls_file, *zip_file;
  is_index = 0;
  is_compat = 0;
  files_count = 1;

  long_index=0;
  while ((opt = getopt_long(argc, argv,"ich",
          longopts, &long_index )) != -1) {
    switch (opt) {
      case 'i' :
        is_index = 1;
        break;
      case 'c' :
        is_compat = 1;
        break;
      case 'h' :
      default:
        print_usage( argv[0] );
        return 1;
    }
  }

  if ( (optind + 2) == argc) {
    xls_file = argv[optind];
    zip_file = argv[optind + 1];
  } else {
    print_usage( argv[0] );
    return 1;
  }

  if (!is_compat && is_index) {
	  printf("%s don't suport index option for Xoonips 4.0\n", argv[0]);
	  return 0;
  }

  ret = proc_main( argv[0], xls_file, zip_file, is_index );
  if ( ret != 0 ) {
    return 1;
  }
  return 0;
}

static void print_usage( const char *bin )
{
  printf( "usage: %s [options] input.xls output.zip\n", bin );
  printf( "\n" );
  printf( "options:\n" );
  printf( "  -i or --index : convert index list (experimental)\n" );
  printf( "  -c or --compat34 : use xoonips v3.4 format\n" );
  printf( "  -h or --help  : print this message\n" );
}

static int proc_main( const char *bin, const char *xls_file,
                      const char *zip_file, int is_index )
{
  int item_id;
  unsigned int i, nums;
  sl4_string_t *tmpdir, *errmes;
  cexcel_book_t *book;
  cexcel_sheet_t *sheet;
  const char *bindir;
  int err = -1;
  item_id = 1;

  bindir = get_bin_dirname( bin );

  book = cexcel_read_file( xls_file );
  if ( book == NULL ) {
    printf( "ERROR: Excel File \"%s\" is Not Found or Unreadable.\n",
            xls_file );
    return 1;
  }
  nums = cexcel_book_get_nums( book );
  if ( nums == 0 ) {
    printf( "ERROR: No Readable Excel Sheets Found in \"%s\" \n", xls_file );
    cexcel_book_delete( book );
    return 1;
  }
  printf( "[info] Excel file \"%s\" was loaded.\n", xls_file );
  printf( "[info] - %u sheets found in loaded file.\n", nums );

  errmes = sl4_string_new( NULL );
  if ( errmes == NULL ) {
    ERROR_OUTMEM(  );
  }

  tmpdir = tempdir_new(  );
  if (is_compat) {
    tempdir_mkdir( tmpdir, "/files" );
  }

  for ( i = 1; i <= nums; i++ ) {
    sheet = cexcel_book_get_sheet( book, i );
    if ( sheet == NULL ) {
      ERROR_UNEXPECTED(  );
    }
    cexcel_sheet_trim_empty_cells( sheet );
    err = proc_sheet( sheet, tmpdir, &item_id, bindir, errmes, is_index );
    if ( err != 0 ) {
      printf( "ERROR: %s\n", sl4_string_get( errmes ) );
      break;
    }
  }

  cexcel_book_delete( book );
  if ( err == 0 ) {
    err = minizip_create( zip_file, sl4_string_get( tmpdir ) );
    switch ( err ) {
    case -1:
      printf( "ERROR: Failed to create XooNIps Import File \"%s\"\n",
              zip_file );
      break;
    case -2:
      printf( "ERROR: No Items Found in Excel File \"%s\"\n", xls_file );
      break;
    default:
      printf( "[info] Success to create XooNIps import file \"%s\".\n",
              zip_file );
    }
  }
  tempdir_delete( tmpdir );
  sl4_string_delete( errmes );
  return err;
}

static int proc_sheet( cexcel_sheet_t * sheet, sl4_string_t *tmpdir,
                       int *item_id, const char *bindir,
                       sl4_string_t *errmes, int is_index )
{
  int err, count;
  template_t *tmpl;
  inifile_t *ini;
  const char *sname, *cname;
  unsigned int rows, cols, i;
  sl4_string_t *tmpdir_row;
  char config_path[PATH_MAX], tmp_str1[PATH_MAX];
#ifdef WIN32
  char buf[_MAX_PATH];
  char *etc = "/etc";
  if ( strlen( bindir ) + strlen( etc ) >= _MAX_PATH ) {
    ERROR_UNEXPECTED(  );
  }
  strcpy( buf, bindir );
  strcat( buf, etc );
  strcpy(config_path, buf);
#else
  strcpy(config_path, E2X_CONFIG_PATH);
#endif

  if (is_compat) {
	  strcat(config_path, "/3.4");
  } else {
	  strcat(config_path, "/4.0");
  }

  err = XLS2XNI_ERROR_SUCCESS;

  sname = cexcel_sheet_get_name( sheet );
  rows = cexcel_sheet_get_rows( sheet );
  cols = cexcel_sheet_get_cols( sheet );

  count = *item_id;

  if ( is_index ) {
    cname = "index";
  } else {
    cname = sname;
  }

  tmpl = template_new( config_path, cname );
  if ( tmpl == NULL ) {
    sl4_string_sprintf( errmes,
                        "Failed to Open XML Template File %s.", cname );
    return -1;
  }

  ini = inifile_load( config_path, cname, errmes );
  if ( ini == NULL ) {
    template_delete( tmpl );
    return -1;
  }

  tmpdir_row = sl4_string_new( NULL );
  for ( i = 1; i < rows; i++ ) {
    if (!is_compat){
        // make a dir to a row of sheet, name as item_id
        sprintf(tmp_str1, "/%d", *item_id);
        tempdir_mkdir( tmpdir, tmp_str1 );
        sl4_string_set(tmpdir_row, sl4_string_get(tmpdir));
        sl4_string_append(tmpdir_row, tmp_str1);
        err = template_assign
               ( tmpl, sheet, ini, tmpdir_row, i, *item_id, errmes, is_index);
    } else {
        err = template_assign
               ( tmpl, sheet, ini, tmpdir, i, *item_id, errmes, is_index);
    }

    if ( err != 0 ) {
      err = -1;
      break;
    }

    if (!is_compat) {
      sprintf(tmp_str1, "%s.zip", sl4_string_get(tmpdir_row));
      err = minizip_create( tmp_str1, sl4_string_get(tmpdir_row) );
      switch ( err ) {
      case -1:
        printf( "ERROR: Failed to create XooNIps Import File \"%s\"\n", tmp_str1 );
        break;
      case -2:
        printf( "ERROR: No Items Found in item_id %d temporary dicrectory\n", *item_id );
        break;
      default:
        break;
      }
      directory_remove(sl4_string_get(tmpdir_row));
    }
    ( *item_id )++;
  }
  sl4_string_delete(tmpdir_row);
  if ( is_index ) {
    if ( template_merge_rowset( tmpl, tmpdir, count, *item_id, "index" ) !=
         0 ) {
      sl4_string_sprintf( errmes,
                          "Failed to Merge rowset XML File id:%d to id:%d.",
                          count, *item_id );
      err = -1;
    }
  }
  inifile_delete( ini );
  template_delete( tmpl );
  if ( err == 0 ) {
    printf( "[info] - %d %s found in sheet \"%s\".\n", *item_id - count,
            ( is_index ? "indexes" : "items" ), sname );
  }
  return err;
}
