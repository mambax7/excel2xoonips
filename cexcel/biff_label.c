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

CEXCEL_RETTYPE cexcel_biff_label( cexcel_record_t * record,
                                  cexcel_context_t * ctx )
{
  CEXCEL_WORD row, col, xf;
  CEXCEL_UCHAR dummy;
  sl4_string_t *str;
  CEXCEL_SLEN slen;
  CEXCEL_STYPE stype;
  size_t offset = 0;
  if ( ctx->sheet_btype == CEXCEL_BTYPE_UNDEF ) {
    return CEXCEL_RETTYPE_UNEXPECTED;
  }
  row = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &row );
  col = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &col );
  switch ( ctx->global_btype ) {
  case CEXCEL_BTYPE_BIFF2:
    dummy = cexcel_record_data_uchar( record, ctx, &offset );
    dummy = cexcel_record_data_uchar( record, ctx, &offset );
    dummy = cexcel_record_data_uchar( record, ctx, &offset );
    stype = CEXCEL_STYPE_BYTE;
    slen = CEXCEL_SLEN_ONE;
    break;
  case CEXCEL_BTYPE_BIFF3:
  case CEXCEL_BTYPE_BIFF4:
  case CEXCEL_BTYPE_BIFF5:
    xf = cexcel_record_data_word( record, ctx, &offset );
    stype = CEXCEL_STYPE_BYTE;
    slen = CEXCEL_SLEN_TWO;
    break;
  case CEXCEL_BTYPE_BIFF8:
  default:
    xf = cexcel_record_data_word( record, ctx, &offset );
    stype = CEXCEL_STYPE_UNICODE;
    slen = CEXCEL_SLEN_TWO;
    break;
  }
  str = cexcel_record_data_str( record, ctx, &offset, stype, slen );
  if ( str == NULL ) {
    return CEXCEL_RETTYPE_OUTOFMEMORY;
  }
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x %03x LABEL - (%d,%d) (%s)\n", record->offset, record->id,
          row, col, sl4_string_get( str ) );
#endif
  cexcel_sheet_set_cell( ctx->worksheet, sl4_string_get( str ),
                         ( unsigned int ) row, ( unsigned int ) col );
  sl4_string_delete( str );
  return CEXCEL_RETTYPE_SUCCESS;
}
