/*
 * cexcel - A library to read data from Excel spread sheets
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

#include "cole.h"
#include "libsl4.h"
#include "cexcel.h"
#include "cexcel_internal.h"

typedef struct _cexcel_file_context_t {
  cexcel_file_t *file;
  CEXCEL_RETTYPE ret;
  void *info;
  cexcel_file_func_t func;
} cexcel_file_context_t;

static cexcel_file_t *_cexcel_file_new( const char *filename );
static void _cexcel_file_func_wrapper( COLEDIRENT * cde, void *_context );

cexcel_file_t *cexcel_file_open( const char *filename )
{
  cexcel_file_t *file;
  COLERRNO err;
  file = _cexcel_file_new( filename );
  if ( file == NULL ) {
    return NULL;
  }
  file->ole.cfs = cole_mount( sl4_file_get_path( file->file ), &err );
  if ( file->ole.cfs == NULL ) {
    /* not ole file */
    file->ftype = CEXCEL_FTYPE_NORMAL;
    if ( sl4_file_open( file->file, "rb" ) ) {
      /* failed to open file */
      cexcel_file_close( file );
      return NULL;
    }
  } else {
    /* ole file */
    file->ftype = CEXCEL_FTYPE_OLE;
  }
  return file;
}

int cexcel_file_close( cexcel_file_t * file )
{
  COLERRNO err;
  if ( file->ole.cfs ) {
    cole_umount( file->ole.cfs, &err );
  }
  sl4_file_delete( file->file );
  free( file );
  return 0;
}

CEXCEL_RETTYPE cexcel_file_parse( cexcel_file_t * file,
                                  cexcel_file_func_t func, void *info )
{
  COLERRNO err;
  CEXCEL_RETTYPE ret;
  cexcel_file_context_t context;        /* for OLE document */
  /* call functions */
  switch ( file->ftype ) {
  case CEXCEL_FTYPE_NORMAL:
    ret = func( file, info );
    break;
  case CEXCEL_FTYPE_OLE:
    /* initialize context */
    context.file = file;
    context.ret = CEXCEL_RETTYPE_SUCCESS;
    context.info = info;
    context.func = func;
    /* try BIFF8 */
    if ( cole_locate_filename
         ( file->ole.cfs, "/Workbook", &context, _cexcel_file_func_wrapper,
           &err ) ) {
      /* try BIFF5 */
      if ( cole_locate_filename
           ( file->ole.cfs, "/Book", &context, _cexcel_file_func_wrapper,
             &err ) ) {
        context.ret = CEXCEL_RETTYPE_BADFILE;
      }
    }
    ret = context.ret;
    break;
  case CEXCEL_FTYPE_UNDEF:
  default:
    ret = CEXCEL_RETTYPE_UNEXPECTED;
  }
  return ret;
}

static cexcel_file_t *_cexcel_file_new( const char *filename )
{
  cexcel_file_t *file;
  sl4_file_t *sl4file;
  /* allocate memory : sl4_file_t */
  sl4file = sl4_file_new( filename );
  if ( sl4file == NULL ) {
    return NULL;
  }
  /* allocate memory : excel_file_t */
  file = ( cexcel_file_t * ) malloc( sizeof( cexcel_file_t ) );
  if ( file == NULL ) {
    sl4_file_delete( sl4file );
    return NULL;
  }
  file->ole.cfs = NULL;
  file->ole.cf = NULL;
  file->file = sl4file;
  file->ftype = CEXCEL_FTYPE_UNDEF;
  return file;
}

static void _cexcel_file_func_wrapper( COLEDIRENT * cde, void *_context )
{
  COLERRNO err;
  cexcel_file_context_t *context;
  cexcel_file_func_t func;
  cexcel_file_t *file;
  void *info;
  /* dump info */
  context = ( cexcel_file_context_t * ) _context;
  func = context->func;
  file = context->file;
  info = context->info;
  /* open stream */
  file->ole.cf = cole_fopen_direntry( cde, &err );
  if ( file->ole.cf == NULL ) {
    context->ret = CEXCEL_RETTYPE_BADFILE;
    return;
  }
  /* call user defined function */
  context->ret = func( file, info );
  /* close stream */
  cole_fclose( file->ole.cf, &err );
}
