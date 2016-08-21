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

CEXCEL_RETTYPE cexcel_biff_format( cexcel_record_t * record,
                                   cexcel_context_t * ctx )
{
  CEXCEL_DWORD idx;
  CEXCEL_WORD dummy;
  sl4_string_t *str;
  CEXCEL_SLEN slen;
  CEXCEL_STYPE stype;
  size_t offset = 0;
  switch ( ctx->global_btype ) {
  case CEXCEL_BTYPE_BIFF2:
  case CEXCEL_BTYPE_BIFF3:
    idx = ( CEXCEL_DWORD ) - 1; /* zero based index */
    stype = CEXCEL_STYPE_BYTE;
    slen = CEXCEL_SLEN_ONE;
    break;
  case CEXCEL_BTYPE_BIFF4:
    dummy = cexcel_record_data_word( record, ctx, &offset );
    idx = ( CEXCEL_DWORD ) - 1; /* zero based index */
    stype = CEXCEL_STYPE_BYTE;
    slen = CEXCEL_SLEN_ONE;
    break;
  case CEXCEL_BTYPE_BIFF5:
    dummy = cexcel_record_data_word( record, ctx, &offset );
    idx = ( CEXCEL_DWORD ) dummy;
    stype = CEXCEL_STYPE_BYTE;
    slen = CEXCEL_SLEN_ONE;
    break;
  case CEXCEL_BTYPE_BIFF8:
  default:
    dummy = cexcel_record_data_word( record, ctx, &offset );
    idx = ( CEXCEL_DWORD ) dummy;
    stype = CEXCEL_STYPE_UNICODE;
    slen = CEXCEL_SLEN_TWO;
    break;
  }
  str = cexcel_record_data_str( record, ctx, &offset, stype, slen );
  if ( str == NULL ) {
    return CEXCEL_RETTYPE_OUTOFMEMORY;
  }
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x %03x FORMAT - %3d (%s)\n", record->offset, record->id,
          idx, sl4_string_get( str ) );
#endif
  cexcel_context_format_add( ctx, idx, str );
  return CEXCEL_RETTYPE_SUCCESS;
}
