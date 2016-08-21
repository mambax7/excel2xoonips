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

CEXCEL_RETTYPE cexcel_biff_xf( cexcel_record_t * record,
                               cexcel_context_t * ctx )
{
  CEXCEL_XFTYPE type = CEXCEL_XFTYPE_STYLE;
  CEXCEL_UCHAR attrib = 0;
  CEXCEL_DWORD idx_pstyle = ( CEXCEL_DWORD ) - 1;
  CEXCEL_DWORD idx_font = ( CEXCEL_DWORD ) - 1;
  CEXCEL_DWORD idx_format = ( CEXCEL_DWORD ) - 1;
  CEXCEL_UCHAR dummy1;
  CEXCEL_WORD dummy2;
  CEXCEL_DWORD dummy4;
  size_t offset = 0;
  switch ( ctx->global_btype ) {
  case CEXCEL_BTYPE_BIFF2:
    /* index to font record */
    idx_font = ( CEXCEL_DWORD ) cexcel_record_data_uchar( record, ctx,
                                                          &offset );
    /* not used */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    /* number format and cell flags */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    idx_format = ( CEXCEL_DWORD ) ( dummy1 & 0x3f );
    /* horizontal alignment, border style and background */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    break;
  case CEXCEL_BTYPE_BIFF3:
    /* index to font record */
    idx_font = ( CEXCEL_DWORD ) cexcel_record_data_uchar( record, ctx,
                                                          &offset );
    /* index to format record */
    idx_format = ( CEXCEL_DWORD ) cexcel_record_data_uchar( record, ctx,
                                                            &offset );
    /* XF_TYPE_PROTO - XF type and cell protection */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    if ( dummy1 & 0x04 ) {
      type = CEXCEL_XFTYPE_STYLE;
    } else {
      type = CEXCEL_XFTYPE_CELL;
    }
    /* flags for used attribute groups */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    attrib = ( dummy1 & 0xfc ) >> 2;
    /* horizontal alignment, text break and parent style xf */
    dummy2 = cexcel_record_data_word( record, ctx, &offset );
    idx_pstyle = ( dummy2 & 0xfff0 >> 4 );
    /* XF_AREA_34 - cell background area */
    dummy2 = cexcel_record_data_word( record, ctx, &offset );
    /* XF_BORDER_34 - cell border lines */
    dummy4 = cexcel_record_data_word( record, ctx, &offset );
    break;
  case CEXCEL_BTYPE_BIFF4:
    /* index to font record */
    idx_font = ( CEXCEL_DWORD ) cexcel_record_data_uchar( record, ctx,
                                                          &offset );
    /* index to format record */
    idx_format = ( CEXCEL_DWORD ) cexcel_record_data_uchar( record, ctx,
                                                            &offset );
    /* xf type, cell protection and parent style xf */
    dummy2 = cexcel_record_data_word( record, ctx, &offset );
    if ( dummy2 & 0x04 ) {
      type = CEXCEL_XFTYPE_STYLE;
    } else {
      type = CEXCEL_XFTYPE_CELL;
    }
    idx_pstyle = ( dummy2 & 0xfff0 >> 4 );
    /* alignment, text break and text orientation */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    /* flags for used attribute groups */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    attrib = ( dummy1 & 0xfc ) >> 2;
    /* XF_AREA_34 - cell background area */
    dummy2 = cexcel_record_data_word( record, ctx, &offset );
    /* XF_BORDER_34 - cell border lines */
    dummy4 = cexcel_record_data_word( record, ctx, &offset );
    break;
  case CEXCEL_BTYPE_BIFF5:
    /* index to font record */
    idx_font = ( CEXCEL_DWORD ) cexcel_record_data_word( record, ctx,
                                                         &offset );
    /* index to format record */
    idx_format = ( CEXCEL_DWORD ) cexcel_record_data_word( record, ctx,
                                                           &offset );
    /* xf type, cell protection and parent style xf */
    dummy2 = cexcel_record_data_word( record, ctx, &offset );
    if ( dummy2 & 0x04 ) {
      type = CEXCEL_XFTYPE_STYLE;
    } else {
      type = CEXCEL_XFTYPE_CELL;
    }
    idx_pstyle = ( dummy2 & 0xfff0 >> 4 );
    /* alignment and text break */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    /* text orientation and flags for used attribute groups */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    attrib = ( dummy1 & 0xfc ) >> 2;
    /* cell border lines and background area */
    dummy4 = cexcel_record_data_dword( record, ctx, &offset );
    dummy4 = cexcel_record_data_dword( record, ctx, &offset );
    break;
  case CEXCEL_BTYPE_BIFF8:
    /* index to font record */
    idx_font = ( CEXCEL_DWORD ) cexcel_record_data_word( record, ctx,
                                                         &offset );
    /* index to format record */
    idx_format = ( CEXCEL_DWORD ) cexcel_record_data_word( record, ctx,
                                                           &offset );
    /* xf type, cell protection and parent style xf */
    dummy2 = cexcel_record_data_word( record, ctx, &offset );
    if ( dummy2 & 0x04 ) {
      type = CEXCEL_XFTYPE_STYLE;
    } else {
      type = CEXCEL_XFTYPE_CELL;
    }
    idx_pstyle = ( dummy2 & 0xfff0 >> 4 );
    /* alignment and text break */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    /* XF_ROTATION - text rotation angle */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    /* indentation, shrink to cell size and text direction */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    /* flags for used attribute groups */
    dummy1 = cexcel_record_data_uchar( record, ctx, &offset );
    attrib = ( dummy1 & 0xfc ) >> 2;
    /* cell border lines and background area */
    dummy4 = cexcel_record_data_dword( record, ctx, &offset );
    dummy4 = cexcel_record_data_dword( record, ctx, &offset );
    dummy2 = cexcel_record_data_word( record, ctx, &offset );
  default:
    break;
  }
#ifdef _CEXCEL_DEBUG_BIFF
  printf
    ( "%08x %03x XF - type:%1d attrib:%04x font:%3d format:%3d parent:%08lx\n",
      record->offset, record->id, type, attrib, idx_font, idx_format,
      idx_pstyle );
#endif
  cexcel_context_xf_add( ctx, type, attrib, idx_pstyle, idx_font,
                         idx_format );
  return CEXCEL_RETTYPE_SUCCESS;
}
