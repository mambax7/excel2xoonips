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

CEXCEL_RETTYPE cexcel_biff_labelsst( cexcel_record_t * record,
                                     cexcel_context_t * ctx )
{
  CEXCEL_WORD row, col, xf;
  CEXCEL_DWORD sst;
  sl4_string_t *str;
  size_t offset = 0;
  if ( ctx->sheet_btype != CEXCEL_BTYPE_BIFF8 ) {
    return CEXCEL_RETTYPE_UNEXPECTED;
  }
  row = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &row );
  col = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &col );
  xf = cexcel_record_data_word( record, ctx, &offset );
  sst = cexcel_record_data_dword( record, ctx, &offset );
  cexcel_record_align_dword( &sst );
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x %03x LABELSST - (%d,%d) SST(%ld)\n", record->offset,
          record->id, row, col, sst );
#endif
  str = cexcel_context_sst_get( ctx, sst );
  if ( str == NULL ) {
    return CEXCEL_RETTYPE_UNEXPECTED;
  }
  cexcel_sheet_set_cell( ctx->worksheet, sl4_string_get( str ),
                         ( unsigned int ) row, ( unsigned int ) col );
  return CEXCEL_RETTYPE_SUCCESS;
}
