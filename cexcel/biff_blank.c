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

CEXCEL_RETTYPE cexcel_biff_blank( cexcel_record_t * record,
                                  cexcel_context_t * ctx )
{
  CEXCEL_WORD row, col, xf;
  CEXCEL_UCHAR dummy;
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
    break;
  case CEXCEL_BTYPE_BIFF3:
  case CEXCEL_BTYPE_BIFF4:
  case CEXCEL_BTYPE_BIFF5:
  case CEXCEL_BTYPE_BIFF8:
  default:
    xf = cexcel_record_data_word( record, ctx, &offset );
    break;
  }
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x %03x BLANK - (%d,%d)\n", record->offset, record->id, row,
          col );
#endif
  cexcel_sheet_set_cell( ctx->worksheet, "", ( unsigned int ) row,
                         ( unsigned int ) col );
  return CEXCEL_RETTYPE_SUCCESS;
}
