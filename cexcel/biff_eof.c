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

CEXCEL_RETTYPE cexcel_biff_eof( cexcel_record_t * record,
                                cexcel_context_t * ctx )
{
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x EOF (%03x)\n", record->offset, record->id );
#endif
  if ( ctx->sheet_btype != CEXCEL_BTYPE_UNDEF ) {
    ctx->sheet_btype = CEXCEL_BTYPE_UNDEF;
    ctx->worksheet = NULL;
  } else if ( ctx->global_btype != CEXCEL_BTYPE_UNDEF ) {
    /* ctx->global_btype = CEXCEL_BTYPE_UNDEF; */
  } else {
    return CEXCEL_RETTYPE_BADFILE;
  }
  return CEXCEL_RETTYPE_SUCCESS;
}
