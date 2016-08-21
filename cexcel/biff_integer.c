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

CEXCEL_RETTYPE cexcel_biff_integer( cexcel_record_t * record,
                                    cexcel_context_t * ctx )
{
  CEXCEL_WORD row, col, value;
  CEXCEL_UCHAR dummy;
  sl4_string_t *str;
  int data;
  size_t offset = 0;
  if ( ctx->sheet_btype != CEXCEL_BTYPE_BIFF2 ) {
    return CEXCEL_RETTYPE_BADFILE;
  }
  row = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &row );
  col = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &col );
  dummy = cexcel_record_data_uchar( record, ctx, &offset );
  dummy = cexcel_record_data_uchar( record, ctx, &offset );
  dummy = cexcel_record_data_uchar( record, ctx, &offset );
  value = cexcel_record_data_word( record, ctx, &offset );
  data = cexcel_record_word2int( value );
  str = sl4_string_new( NULL );
  if ( str == NULL ) {
    return CEXCEL_RETTYPE_OUTOFMEMORY;
  }
  sl4_string_sprintf( str, "%d", data );
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x %03x INTEGER - (%d,%d) (%s)\n", record->offset, record->id,
          row, col, sl4_string_get( str ) );
#endif
  cexcel_sheet_set_cell( ctx->worksheet, sl4_string_get( str ),
                         ( unsigned int ) row, ( unsigned int ) col );
  sl4_string_delete( str );
  return CEXCEL_RETTYPE_SUCCESS;
}
