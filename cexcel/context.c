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

cexcel_context_t *cexcel_context_new( cexcel_file_t * file )
{
  cexcel_context_t *ctx;
  ctx = ( cexcel_context_t * ) malloc( sizeof( cexcel_context_t ) );
  if ( ctx == NULL ) {
    return NULL;
  }
  ctx->file = file;
  ctx->has_global = 0;
  ctx->codepage = 0x16f;        /* ASCII */
  ctx->global_btype = CEXCEL_BTYPE_UNDEF;
  ctx->sheet_btype = CEXCEL_BTYPE_UNDEF;
  ctx->book = cexcel_book_new( sl4_file_get_path( file->file ) );
  ctx->worksheet = NULL;
  if ( ctx->book == NULL ) {
    free( ctx );
    return NULL;
  }
  if ( cexcel_context_format_new( ctx ) != 0 ) {
    cexcel_book_delete( ctx->book );
    free( ctx );
    return NULL;
  }
  if ( cexcel_context_sst_new( ctx ) != 0 ) {
    cexcel_book_delete( ctx->book );
    cexcel_context_format_delete( ctx );
    free( ctx );
    return NULL;
  }
  if ( cexcel_context_xf_new( ctx ) != 0 ) {
    cexcel_book_delete( ctx->book );
    cexcel_context_format_delete( ctx );
    cexcel_context_sst_delete( ctx );
    free( ctx );
    return NULL;
  }
  if ( cexcel_context_sheet_new( ctx ) != 0 ) {
    cexcel_book_delete( ctx->book );
    cexcel_context_format_delete( ctx );
    cexcel_context_sst_delete( ctx );
    cexcel_context_xf_delete( ctx );
    free( ctx );
    return NULL;
  }
  return ctx;
}

int cexcel_context_delete( cexcel_context_t * ctx )
{
  /* cexcel_book_delete( ctx->book ); */
  cexcel_context_sheet_delete( ctx );
  cexcel_context_sst_delete( ctx );
  cexcel_context_format_delete( ctx );
  cexcel_context_xf_delete( ctx );
  free( ctx );
  return 0;
}
