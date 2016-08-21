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

CEXCEL_RETTYPE cexcel_biff_boundsheet( cexcel_record_t * record,
                                       cexcel_context_t * ctx )
{
  CEXCEL_STYPE stype;
  CEXCEL_DWORD position;
  CEXCEL_UCHAR visibility, sheet_type;
  sl4_string_t *str;
  size_t offset = 0;
  position = cexcel_record_data_dword( record, ctx, &offset );
  cexcel_record_align_dword( &position );
  visibility = cexcel_record_data_uchar( record, ctx, &offset );
  sheet_type = cexcel_record_data_uchar( record, ctx, &offset );
  switch ( ctx->global_btype ) {
  case CEXCEL_BTYPE_BIFF5:
    stype = CEXCEL_STYPE_BYTE;
    break;
  case CEXCEL_BTYPE_BIFF8:
    stype = CEXCEL_STYPE_UNICODE;
    break;
  default:
    return CEXCEL_RETTYPE_BADFILE;
  }
  str =
    cexcel_record_data_str( record, ctx, &offset, stype, CEXCEL_SLEN_ONE );
  if ( str == NULL ) {
    return CEXCEL_RETTYPE_OUTOFMEMORY;
  }
#ifdef _CEXCEL_DEBUG_BIFF
  printf
    ( "%08x %03x BOUNDSHEET - Position(%08lx), Visibility(%x), SheetType(%x), SheetName(%s)\n",
      record->offset, record->id, position, visibility, sheet_type,
      sl4_string_get( str ) );
#endif
  if ( visibility == 0x00 && sheet_type == 0x00 ) {
    if ( cexcel_context_sheet_add( ctx, position, sl4_string_get( str ) ) ) {
      sl4_string_delete( str );
      return CEXCEL_RETTYPE_OUTOFMEMORY;
    }
  }
  sl4_string_delete( str );
  return CEXCEL_RETTYPE_SUCCESS;
}
