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

static void _cexcel_context_xf_del_func( void *data, void *arg );

int cexcel_context_xf_new( cexcel_context_t * ctx )
{
  ctx->xf = cexcel_hash_new(  );
  if ( ctx->xf == NULL ) {
    return -1;
  }
  return 0;
}

int cexcel_context_xf_delete( cexcel_context_t * ctx )
{
  sl4_hash_table_delete( ctx->xf, _cexcel_context_xf_del_func, NULL );
  return 0;
}

int cexcel_context_xf_add( cexcel_context_t * ctx, CEXCEL_XFTYPE type,
                           CEXCEL_UCHAR attrib, CEXCEL_DWORD idx_pstyle,
                           CEXCEL_DWORD idx_font, CEXCEL_DWORD idx_format )
{
  cexcel_xf_t *xf;
  CEXCEL_DWORD nextkey = ( CEXCEL_DWORD ) ctx->xf->nkeys;
  xf = cexcel_xf_new(  );
  if ( xf == NULL ) {
    return -1;
  }
  xf->type = type;
  xf->attrib = attrib;
  xf->idx_pstyle = idx_pstyle;
  xf->idx_font = idx_font;
  xf->idx_format = idx_format;
  return sl4_hash_table_insert( ctx->xf, &nextkey, xf );
}

CEXCEL_DWORD cexcel_context_xf_get_format_index( cexcel_context_t * ctx,
                                                 CEXCEL_DWORD idx )
{
  cexcel_xf_t *xf;
  xf = ( cexcel_xf_t * ) sl4_hash_table_lookup( ctx->xf, &idx );
  if ( xf == NULL ) {
    /* xf not found */
    return ( CEXCEL_DWORD ) - 1;
  }
  switch ( xf->type ) {
  case CEXCEL_XFTYPE_UNDEF:
    return ( CEXCEL_DWORD ) - 1;
  case CEXCEL_XFTYPE_CELL:
    if ( ( xf->attrib & 0x01 ) == 0 ) {
      /* use parent style xf for number format index */
      CEXCEL_DWORD idx_pstyle;
      idx_pstyle = xf->idx_pstyle;
      xf = ( cexcel_xf_t * ) sl4_hash_table_lookup( ctx->xf, &idx_pstyle );
      if ( xf == NULL ) {
        /* parent style xf not found */
        return ( CEXCEL_DWORD ) - 1;
      }
    }
    break;
  case CEXCEL_XFTYPE_STYLE:
    break;
  }
  return xf->idx_format;
}

static void _cexcel_context_xf_del_func( void *data, void *arg )
{
  cexcel_xf_t *xf = ( cexcel_xf_t * ) data;
  cexcel_xf_delete( xf );
}
