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

CEXCEL_RETTYPE cexcel_biff_sst( cexcel_record_t * record,
                                cexcel_context_t * ctx )
{
  CEXCEL_DWORD total, nm, i;
  size_t offset = 0;
  sl4_string_t *str;
  total = cexcel_record_data_dword( record, ctx, &offset );
  cexcel_record_align_dword( &total );
  nm = cexcel_record_data_dword( record, ctx, &offset );
  cexcel_record_align_dword( &nm );
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x %03x SST - Total(%ld), Num(%ld)\n", record->offset,
          record->id, total, nm );
#endif
  for ( i = 0; i < nm; i++ ) {
    str =
      cexcel_record_data_str( record, ctx, &offset, CEXCEL_STYPE_UNICODE,
                              CEXCEL_SLEN_TWO );
    if ( str == NULL ) {
      return CEXCEL_RETTYPE_OUTOFMEMORY;
    }
    cexcel_context_sst_add( ctx, str );
  }
  return CEXCEL_RETTYPE_SUCCESS;
}
