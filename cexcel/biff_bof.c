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

CEXCEL_RETTYPE cexcel_biff_bof( cexcel_record_t * record,
                                cexcel_context_t * ctx )
{
  CEXCEL_BTYPE btype = CEXCEL_BTYPE_UNDEF;
  CEXCEL_WORD ver, stream;
  size_t offset = 0;
  int is_global;
  /* biff version */
  ver = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &ver );
  switch ( record->id ) {
  case CEXCEL_ID_BOF58:
    switch ( ver ) {
    case 0x000:                /* BIFF5 */
      btype = CEXCEL_BTYPE_BIFF5;
      break;
    case 0x200:                /* BIFF2 */
      btype = CEXCEL_BTYPE_BIFF2;
      break;
    case 0x300:                /* BIFF3 */
      btype = CEXCEL_BTYPE_BIFF3;
      break;
    case 0x400:                /* BIFF4 */
      btype = CEXCEL_BTYPE_BIFF4;
      break;
    case 0x500:                /* BIFF5 */
      btype = CEXCEL_BTYPE_BIFF5;
      break;
    case 0x600:                /* BIFF8 */
      btype = CEXCEL_BTYPE_BIFF8;
      break;
    default:
      return CEXCEL_RETTYPE_BADFILE;
    }
    break;
  case CEXCEL_ID_BOF4:
    btype = CEXCEL_BTYPE_BIFF4;
    break;
  case CEXCEL_ID_BOF3:
    btype = CEXCEL_BTYPE_BIFF3;
    break;
  case CEXCEL_ID_BOF2:
    btype = CEXCEL_BTYPE_BIFF2;
    break;
  }
  /* type of the data */
  stream = cexcel_record_data_word( record, ctx, &offset );
  cexcel_record_align_word( &stream );
  switch ( stream ) {
  case 0x0005:                 /* workbook global */
    if ( btype != CEXCEL_BTYPE_BIFF5 && btype != CEXCEL_BTYPE_BIFF8 ) {
      return CEXCEL_RETTYPE_BADFILE;
    }
    is_global = 1;
    break;
  case 0x0010:                 /* sheet or dialogue */
    is_global = 0;
    break;
  case 0x0100:                 /* workspace */
    if ( btype == CEXCEL_BTYPE_BIFF2 ) {
      return CEXCEL_RETTYPE_BADFILE;
    }
    is_global = 1;
    break;
  default:
    /* need to skip to eof */
    return CEXCEL_RETTYPE_NOTIMPLEMENTED;
  }
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08x %03x BOF - BIFF%d, version(%03x), stream(%05x)\n",
          record->offset, record->id, btype, ver, stream );
#endif
  if ( ctx->sheet_btype != CEXCEL_BTYPE_UNDEF ) {
    return CEXCEL_RETTYPE_BADFILE;
  }
  if ( is_global ) {
    if ( ctx->global_btype != CEXCEL_BTYPE_UNDEF ) {
      return CEXCEL_RETTYPE_BADFILE;
    }
    ctx->global_btype = btype;
    ctx->has_global = 1;
  } else {
    /* usually this is old format of Excel data */
    ctx->sheet_btype = btype;
    if ( ctx->has_global == 0 ) {
      sl4_string_t *sname;
      size_t pos;
      sname = sl4_string_new( sl4_file_get_name( ctx->file->file ) );
      if ( sname == NULL ) {
        return CEXCEL_RETTYPE_OUTOFMEMORY;
      }
      pos = sl4_string_rfind( sname, "." );
      if ( pos != ( size_t ) -1 ) {
        sl4_string_erase( sname, pos, sl4_string_length( sname ) - pos );
      }
      ctx->global_btype = btype;
      cexcel_context_sheet_add( ctx, record->offset,
                                sl4_string_get( sname ) );
      sl4_string_delete( sname );
    }
    if ( cexcel_context_sheet_get( ctx, record->offset ) ) {
      return CEXCEL_RETTYPE_BADFILE;
    }
  }
  if ( ctx->sheet_btype == CEXCEL_BTYPE_BIFF5 ||
       ctx->sheet_btype == CEXCEL_BTYPE_BIFF8 ) {
    cexcel_context_format_init( ctx );
  }
  return CEXCEL_RETTYPE_SUCCESS;
}
