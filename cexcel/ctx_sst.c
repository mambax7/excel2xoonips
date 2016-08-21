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

static void _cexcel_context_sst_del_func( void *data, void *arg );

int cexcel_context_sst_new( cexcel_context_t * ctx )
{
  ctx->sst = cexcel_hash_new(  );
  if ( ctx->sst == NULL ) {
    return -1;
  }
  return 0;
}

int cexcel_context_sst_delete( cexcel_context_t * ctx )
{
  sl4_hash_table_delete( ctx->sst, _cexcel_context_sst_del_func, NULL );
  return 0;
}

int cexcel_context_sst_add( cexcel_context_t * ctx, sl4_string_t *str )
{
  CEXCEL_DWORD nextkey = ( CEXCEL_DWORD ) ctx->sst->nkeys;
  return sl4_hash_table_insert( ctx->sst, &nextkey, str );
}

sl4_string_t *cexcel_context_sst_get( cexcel_context_t * ctx,
                                      CEXCEL_DWORD idx )
{
  return ( sl4_string_t * ) sl4_hash_table_lookup( ctx->sst, &idx );
}

static void _cexcel_context_sst_del_func( void *data, void *arg )
{
  sl4_string_t *str = ( sl4_string_t * ) data;
  sl4_string_delete( str );
}
