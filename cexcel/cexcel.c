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

static CEXCEL_RETTYPE _cexcel_proc( cexcel_file_t * file, void *info );

cexcel_book_t *cexcel_read_file( const char *filename )
{
  cexcel_file_t *file;
  CEXCEL_RETTYPE ret;
  cexcel_context_t *context;
  cexcel_book_t *book;
  file = cexcel_file_open( filename );
  if ( file == NULL ) {
    return NULL;
  }
  context = cexcel_context_new( file );
  if ( context == NULL ) {
    cexcel_file_close( file );
    return NULL;
  }
  ret = cexcel_file_parse( file, _cexcel_proc, context );
  cexcel_file_close( file );
  book = context->book;
  cexcel_context_delete( context );
  return book;
}

static CEXCEL_RETTYPE _cexcel_proc( cexcel_file_t * file, void *info )
{
  CEXCEL_RETTYPE ret;
  cexcel_record_t record;
  cexcel_context_t *context;
  ret = CEXCEL_RETTYPE_BADFILE;
  context = ( cexcel_context_t * ) info;
  while ( cexcel_record_read( file, &record ) == 0 ) {
    switch ( record.id ) {
    case CEXCEL_ID_BLANK2:     /* BLANK (BIFF2) */
    case CEXCEL_ID_BLANK3458:  /* BLANK (BIFF2) */
      ret = cexcel_biff_blank( &record, context );
      break;
    case CEXCEL_ID_BOF2:       /* BOF (BIFF2) */
    case CEXCEL_ID_BOF3:       /* BOF (BIFF3) */
    case CEXCEL_ID_BOF4:       /* BOF (BIFF4) */
    case CEXCEL_ID_BOF58:      /* BOF (BIFF5 or BIFF8) */
      ret = cexcel_biff_bof( &record, context );
      if ( ret == CEXCEL_RETTYPE_NOTIMPLEMENTED ) {
        if ( cexcel_record_goto_eof( file ) ) {
          return CEXCEL_RETTYPE_BADFILE;
        }
      }
      break;
    case CEXCEL_ID_BOOLERR2:   /* BOOLERR (BIFF2) */
    case CEXCEL_ID_BOOLERR3458:        /* BOOLERR (BIFF3-BIFF8) */
      ret = cexcel_biff_boolerr( &record, context );
      break;
    case CEXCEL_ID_BOUNDSHEET: /* BOUNDSHEET */
      ret = cexcel_biff_boundsheet( &record, context );
      break;
    case CEXCEL_ID_CODEPAGE:   /* CODEPAGE */
      ret = cexcel_biff_codepage( &record, context );
      break;
    case CEXCEL_ID_CONTINUE:   /* CONTINUE */
#ifdef _CEXCEL_DEBUG_BIFF
      printf( "%08x %03x CONTINUE - Length(%d)\n", record.offset,
              record.id, record.len );
#endif
      ret = CEXCEL_RETTYPE_SUCCESS;
      break;
    case CEXCEL_ID_EOF:        /* EOF */
      ret = cexcel_biff_eof( &record, context );
      break;
    case CEXCEL_ID_FILEPASS:   /* FILEPASS */
      ret = cexcel_biff_filepass( &record, context );
      break;
    case CEXCEL_ID_FORMAT23:   /* FORMAT (BIFF2-BIFF3) */
    case CEXCEL_ID_FORMAT458:  /* FORMAT (BIFF4-BIFF8) */
      ret = cexcel_biff_format( &record, context );
      break;
    case CEXCEL_ID_INTEGER:    /* INTEGER */
      ret = cexcel_biff_integer( &record, context );
      break;
    case CEXCEL_ID_LABEL2:     /* LABEL (BIFF2) */
    case CEXCEL_ID_LABEL3458:  /* LABEL (BIFF3-BIFF8) */
      ret = cexcel_biff_label( &record, context );
      break;
    case CEXCEL_ID_LABELSST:   /* LABELSST */
      ret = cexcel_biff_labelsst( &record, context );
      break;
    case CEXCEL_ID_MULBLANK:   /* MULBLANK */
      ret = cexcel_biff_mulblank( &record, context );
      break;
    case CEXCEL_ID_MULRK:      /* MULRK */
      ret = cexcel_biff_mulrk( &record, context );
      break;
    case CEXCEL_ID_NUMBER2:    /* NUMBER (BIFF2) */
    case CEXCEL_ID_NUMBER3458: /* NUMBER (BIFF3-BIFF8) */
      ret = cexcel_biff_number( &record, context );
      break;
    case CEXCEL_ID_RK:         /* RK */
      ret = cexcel_biff_rk( &record, context );
      break;
    case CEXCEL_ID_RSTRING:    /* RSTRING */
      ret = cexcel_biff_rstring( &record, context );
      break;
    case CEXCEL_ID_SST:        /* SST */
      ret = cexcel_biff_sst( &record, context );
      break;
    case CEXCEL_ID_XF2:        /* XF (BIFF2) */
    case CEXCEL_ID_XF3:        /* XF (BIFF3) */
    case CEXCEL_ID_XF4:        /* XF (BIFF4) */
    case CEXCEL_ID_XF58:       /* XF (BIFF5-BIFF8) */
      ret = cexcel_biff_xf( &record, context );
      break;
    default:
#ifdef _CEXCEL_DEBUG_BIFF
      printf( "%08x %03x UNKNOWN - Length(%d)\n", record.offset, record.id,
              record.len );
#endif
      ret = CEXCEL_RETTYPE_SUCCESS;
      break;
    }
    if ( ret != CEXCEL_RETTYPE_SUCCESS ) {
#ifdef _CEXCEL_DEBUG_BIFF
      printf( "Unexpected Error %d: %08x %03x\n", ret, record.offset,
              record.id );
#endif
      break;
    }
  }
  return ret;
}
