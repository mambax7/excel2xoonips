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

CEXCEL_RETTYPE cexcel_biff_number( cexcel_record_t * record,
                                   cexcel_context_t * ctx )
{
  CEXCEL_WORD row, col;
  CEXCEL_DWORD high, low, idx_xf, idx_format;
  CEXCEL_UCHAR dummy;
  sl4_string_t *str, *format;
  double data = 0.0;
  size_t offset = 0;
  if ( ctx->sheet_btype == CEXCEL_BTYPE_UNDEF ) {
    return CEXCEL_RETTYPE_BADFILE;
  }
  row = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &row );
  col = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &col );
  if ( ctx->sheet_btype == CEXCEL_BTYPE_BIFF2 ) {
    dummy = cexcel_record_data_uchar( record, ctx, &offset );
    idx_xf = dummy & 0x3f;
    if ( idx_xf == 0x3f ) {
      /* need to use IXFE record */
      return CEXCEL_RETTYPE_NOTIMPLEMENTED;
    }
    dummy = cexcel_record_data_uchar( record, ctx, &offset );
    dummy = cexcel_record_data_uchar( record, ctx, &offset );
  } else {
    idx_xf = ( CEXCEL_DWORD ) cexcel_record_data_word( record, ctx, &offset );
  }
  idx_format = cexcel_context_xf_get_format_index( ctx, idx_xf );
  format = cexcel_context_format_get( ctx, idx_format );
  low = cexcel_record_data_dword( record, ctx, &offset );
  high = cexcel_record_data_dword( record, ctx, &offset );
  data = cexcel_record_dword2double( high, low );
  str = cexcel_format_double( data, format );
  if ( str == NULL ) {
    return CEXCEL_RETTYPE_OUTOFMEMORY;
  }
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x %03x NUMBER - (%d,%d) (%s)\n", record->offset, record->id,
          row, col, sl4_string_get( str ) );
#endif
  cexcel_sheet_set_cell( ctx->worksheet, sl4_string_get( str ),
                         ( unsigned int ) row, ( unsigned int ) col );
  sl4_string_delete( str );
  return CEXCEL_RETTYPE_SUCCESS;
}
