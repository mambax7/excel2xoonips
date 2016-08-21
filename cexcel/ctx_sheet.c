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

int cexcel_context_sheet_new( cexcel_context_t * ctx )
{
  ctx->sheet = cexcel_hash_new(  );
  if ( ctx->sheet == NULL ) {
    return -1;
  }
  return 0;
}

int cexcel_context_sheet_delete( cexcel_context_t * ctx )
{
  sl4_hash_table_delete( ctx->sheet, NULL, NULL );
  return 0;
}

int cexcel_context_sheet_add( cexcel_context_t * ctx, CEXCEL_DWORD offset,
                              const char *str )
{
  cexcel_sheet_t *sheet;
  sheet = cexcel_book_add_sheet( ctx->book, str );
  if ( sheet == NULL ) {
    return -1;
  }
  if ( sl4_hash_table_insert( ctx->sheet, &offset, sheet ) ) {
    return -1;
  }
  return 0;
}

int cexcel_context_sheet_get( cexcel_context_t * ctx, CEXCEL_DWORD offset )
{
  cexcel_sheet_t *sheet;
  sheet = ( cexcel_sheet_t * ) sl4_hash_table_lookup( ctx->sheet, &offset );
  if ( sheet == NULL ) {
    return -1;
  }
  ctx->worksheet = sheet;
  return 0;
}
