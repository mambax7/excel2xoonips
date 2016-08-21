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

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "libsl4.h"
#include "cexcel.h"
#include "excel2xoonips.h"

typedef struct _myfilelist_t {
  sl4_string_t *basedir;
  sl4_list_t *filelist;
} myfilelist_t;

static int rmdir_proc( const char *path, int is_dir, void *data );
static int filelist_proc( const char *path, int is_dir, void *data );

int directory_remove( const char *path )
{
  return recursive_proc( path, rmdir_proc, NULL );
}

sl4_list_t *directory_filelist( const char *path )
{
  myfilelist_t mylist;
  mylist.basedir = sl4_string_new( path );
  if ( mylist.basedir == NULL ) {
    ERROR_OUTMEM(  );
  }
  mylist.filelist = sl4_list_new(  );
  if ( mylist.filelist == NULL ) {
    ERROR_OUTMEM(  );
  }

  if ( recursive_proc( path, filelist_proc, &mylist ) != 0 ) {
    ERROR_UNEXPECTED(  );
  }

  sl4_string_delete( mylist.basedir );
  return mylist.filelist;
}

static int rmdir_proc( const char *path, int is_dir, void *data )
{
  int ret = 0;
  if ( is_dir != 0 ) {
    /* directory */
#ifdef HAVE_RMDIR
    rmdir( path );
#else
# error rmdir() required
#endif
  } else {
    /* file */
    unlink( path );
  }
  return ret;
}

static int filelist_proc( const char *path, int is_dir, void *data )
{
  int ret = 0;
  if ( is_dir != 0 ) {
    /* directory */
  } else {
    /* file */
    myfilelist_t *mylist;
    sl4_string_t *str;
    size_t baselen;
    mylist = ( myfilelist_t * ) data;
    baselen = sl4_string_length( mylist->basedir );

    str = sl4_string_new( path );
    if ( str == NULL ) {
      ERROR_OUTMEM(  );
    }
    if ( sl4_string_erase( str, 0, baselen + 1 ) != 0 ) {
      ERROR_UNEXPECTED(  );
    }

    if ( sl4_list_push_back( mylist->filelist, str ) != 0 ) {
      ERROR_UNEXPECTED(  );
    }
  }
  return ret;
}
