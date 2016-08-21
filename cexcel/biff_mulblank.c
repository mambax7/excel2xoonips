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

CEXCEL_RETTYPE cexcel_biff_mulblank( cexcel_record_t * record,
                                     cexcel_context_t * ctx )
{
  CEXCEL_WORD i, row, fc, lc, xf;
  sl4_string_t *str;
  size_t save_offset, offset = 0;
  if ( ctx->sheet_btype != CEXCEL_BTYPE_BIFF5
       && ctx->sheet_btype != CEXCEL_BTYPE_BIFF8 ) {
    return CEXCEL_RETTYPE_BADFILE;
  }
  row = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &row );
  fc = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &fc );
  save_offset = offset;
  offset = record->len - 2;
  lc = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &lc );
  offset = save_offset;
  str = sl4_string_new( NULL );
  if ( str == NULL ) {
    return CEXCEL_RETTYPE_OUTOFMEMORY;
  }
  for ( i = fc; i <= lc; i++ ) {
    xf = cexcel_record_data_word( record, ctx, &offset );
    cexcel_sheet_set_cell( ctx->worksheet, sl4_string_get( str ),
                           ( unsigned int ) row, ( unsigned int ) i );
  }
  sl4_string_delete( str );
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x %03x MULBRANK - (%d,%d-%d)\n", record->offset, record->id,
          row, fc, lc );
#endif
  return CEXCEL_RETTYPE_SUCCESS;
}
