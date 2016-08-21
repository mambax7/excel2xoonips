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
#include <string.h>

#include "cole.h"
#include "libsl4.h"
#include "cexcel.h"
#include "cexcel_internal.h"

static void _cexcel_record_align_endian( CEXCEL_UCHAR * data, int len );
static sl4_string_t *_cexcel_record_data_str_to_utf8( sl4_string_t *str,
                                                      CEXCEL_WORD codepage );
static int _cexcel_record_read_continue( cexcel_record_t * record,
                                         cexcel_context_t * ctx,
                                         size_t *offset );

int cexcel_record_read( cexcel_file_t * file, cexcel_record_t * record )
{
  size_t len;
  record->offset = cexcel_stream_tell( file );
  if ( cexcel_stream_eof( file ) ) {
    return -1;                  /* eof */
  }
  /* id */
  len = cexcel_stream_read( file, &record->id, 2 );
  cexcel_record_align_word( &record->id );
  if ( len != 2 ) {
    return -1;                  /* unexpected error */
  }
  /* data length */
  len = cexcel_stream_read( file, &record->len, 2 );
  cexcel_record_align_word( &record->len );
  if ( len != 2 ) {
    return -1;                  /* unexpected error */
  }
  /* check data size */
  if ( record->len > 8224 ) {
    return -1;                  /* unexpected error */
  }
  if ( record->len > 0 ) {
    if ( cexcel_stream_read( file, record->data, record->len ) !=
         record->len ) {
      return -1;                /* unexpected error */
    }
  }
  return 0;
}

void cexcel_record_dump( cexcel_record_t * record )
{
  CEXCEL_WORD i;
  printf( "RECORD: OFFSET(%08lx), ID(%04x), LEN(%04d)", record->offset,
          record->id, record->len );
  for ( i = 0; i < record->len; i++ ) {
    if ( i % 16 == 0 ) {
      printf( "\n%08lx ", record->offset + i );
    }
    printf( " %02x", record->data[i] );
  }
  printf( "\n" );
}

int cexcel_record_goto_eof( cexcel_file_t * file )
{
  size_t len;
  CEXCEL_WORD id, dlen;
  for ( ;; ) {
    if ( cexcel_stream_eof( file ) ) {
      return -1;                /* eof */
    }
    /* id */
    len = cexcel_stream_read( file, &id, 2 );
    cexcel_record_align_word( &id );
    if ( len != 2 ) {
      return -1;                /* unexpected error */
    }
    /* data length */
    len = cexcel_stream_read( file, &dlen, 2 );
    cexcel_record_align_word( &dlen );
    if ( len != 2 ) {
      return -1;                /* unexpected error */
    }
    /* check data size */
    if ( dlen > 8220 ) {
      return -1;                /* unexpected error */
    }
    if ( dlen > 0 ) {
      if ( cexcel_stream_seek( file, ( off_t ) dlen, CEXCEL_SEEK_CUR ) ) {
        return -1;              /* unexpected error */
      }
    }
    if ( id == CEXCEL_ID_EOF ) {
      break;
    }
  }
  return 0;
}

CEXCEL_UCHAR cexcel_record_data_uchar( cexcel_record_t * record,
                                       cexcel_context_t * ctx,
                                       size_t *offset )
{
  CEXCEL_UCHAR ret;
  static size_t dsize = 1;
  if ( record->len == *offset ) {
    /* try continue */
    if ( _cexcel_record_read_continue( record, ctx, offset ) ) {
      return 0;                 /* unexpected error */
    }
    if ( record->len == 0 ) {
      return 0;                 /* unexpected error */
    }
  }
  ret = record->data[*offset];
  *offset += dsize;
  return ret;
}

CEXCEL_WORD cexcel_record_data_word( cexcel_record_t * record,
                                     cexcel_context_t * ctx, size_t *offset )
{
  CEXCEL_WORD ret;
  CEXCEL_WORD *ret_ref;
  CEXCEL_UCHAR c[2];
  c[0] = cexcel_record_data_uchar( record, ctx, offset );
  c[1] = cexcel_record_data_uchar( record, ctx, offset );
  ret_ref = ( ( CEXCEL_WORD * ) & c );
  ret = *ret_ref;
  return ret;
}

CEXCEL_DWORD cexcel_record_data_dword( cexcel_record_t * record,
                                       cexcel_context_t * ctx,
                                       size_t *offset )
{
  CEXCEL_DWORD ret;
  CEXCEL_DWORD *ret_ref;
  CEXCEL_UCHAR c[4];
  c[0] = cexcel_record_data_uchar( record, ctx, offset );
  c[1] = cexcel_record_data_uchar( record, ctx, offset );
  c[2] = cexcel_record_data_uchar( record, ctx, offset );
  c[3] = cexcel_record_data_uchar( record, ctx, offset );
  ret_ref = ( ( CEXCEL_DWORD * ) & c);
  ret = *ret_ref;
  return ret;
}

sl4_string_t *cexcel_record_data_str( cexcel_record_t * record,
                                      cexcel_context_t * ctx,
                                      size_t *offset, CEXCEL_STYPE stype,
                                      CEXCEL_SLEN slen )
{
  sl4_string_t *str, *ret;
  CEXCEL_WORD clen;
  CEXCEL_WORD i;
  /* character count */
  switch ( slen ) {
  case CEXCEL_SLEN_ONE:
    clen = cexcel_record_data_uchar( record, ctx, offset );
    break;
  case CEXCEL_SLEN_TWO:
    clen = cexcel_record_data_word( record, ctx, offset );
    cexcel_record_align_word( &clen );
    break;
  default:
    return NULL;                /* unexpected error */
  }
  /* allocate memory */
  str = sl4_string_new( NULL );
  if ( str == NULL ) {
    return NULL;
  }
  /* read strings */
  if ( stype == CEXCEL_STYPE_BYTE ) {
    CEXCEL_UCHAR ch;
    for ( i = 0; i < clen; i++ ) {
      ch = cexcel_record_data_uchar( record, ctx, offset );
      sl4_string_append_char( str, ( char ) ch );
    }
    /* convert to utf8 */
    ret = _cexcel_record_data_str_to_utf8( str, ctx->codepage );
    sl4_string_delete( str );
  } else {                      /* CEXCEL_STYPE_UNICODE */
    CEXCEL_UCHAR grbit;
    int ccompr, phonetic, richtext;
    CEXCEL_WORD rt, ch;
    CEXCEL_DWORD sz;
    rt = sz = 0;
    grbit = cexcel_record_data_uchar( record, ctx, offset );
    ccompr = ( grbit & 0x01 );
    phonetic = ( grbit & 0x04 );
    richtext = ( grbit & 0x08 );
    if ( richtext ) {
      rt = cexcel_record_data_word( record, ctx, offset );
      cexcel_record_align_word( &rt );
    }
    if ( phonetic ) {
      sz = cexcel_record_data_dword( record, ctx, offset );
      cexcel_record_align_dword( &sz );
    }
    /* string */
    for ( i = 0; i < clen; i++ ) {
      unsigned char utf8[4];
      if ( record->len == *offset ) {
        CEXCEL_UCHAR tmp_grbit;
        tmp_grbit = cexcel_record_data_uchar( record, ctx, offset );
        ccompr = ( tmp_grbit & 0x01 );
      }
      if ( ccompr ) {
        /* 16-bit */
        ch = cexcel_record_data_word( record, ctx, offset );
        cexcel_record_align_word( &ch );
      } else {
        /* 8-bit */
        ch = cexcel_record_data_uchar( record, ctx, offset );
      }
      /* encode to utf8 */
      if ( ch < 0x80 ) {
        utf8[0] = ( char ) ch;
        utf8[1] = '\0';
      } else if ( ch < 0x800 ) {
        utf8[0] = ( 0xc0 | ( ( ch >> 6 ) & 0x1f ) );
        utf8[1] = ( 0x80 | ( ch & 0x3f ) );
        utf8[2] = '\0';
      } else {
        utf8[0] = ( 0xe0 | ( ( ch >> 12 ) & 0x0f ) );
        utf8[1] = ( 0x80 | ( ( ch >> 6 ) & 0x3f ) );
        utf8[2] = ( 0x80 | ( ch & 0x3f ) );
        utf8[3] = '\0';
      }
      if ( sl4_string_append( str, ( char * ) utf8 ) ) {
        sl4_string_delete( str );
        return NULL;            /* unexpected error */
      }
    }
    if ( richtext ) {
      CEXCEL_DWORD dummy;
      for ( i = 0; i < rt; i++ ) {
        dummy = cexcel_record_data_dword( record, ctx, offset );
      }
    }
    if ( phonetic ) {
      CEXCEL_UCHAR dummy;
      for ( i = 0; i < sz; i++ ) {
        dummy = cexcel_record_data_uchar( record, ctx, offset );
      }
    }
    ret = str;
  }
  return ret;
}

sl4_string_t *cexcel_record_data_rk( cexcel_record_t * record,
                                     cexcel_context_t * ctx, size_t *offset,
                                     CEXCEL_DWORD idx_xf )
{
  CEXCEL_DWORD value, idx_format;
  sl4_string_t *format;
  double data = 0.0;
  value = cexcel_record_data_dword( record, ctx, offset );
  cexcel_record_align_dword( &value );
  idx_format = cexcel_context_xf_get_format_index( ctx, idx_xf );
  format = cexcel_context_format_get( ctx, idx_format );
  if ( value & 0x00000001 ) {
    /* encoded value is multiplied by 100 */
    if ( value & 0x00000002 ) {
      /* signed integer */
      cexcel_record_align_dword( &value );
      data = ( double ) cexcel_record_dword2int( value >> 2 ) * 0.01;
    } else {
      /* floting point value */
      CEXCEL_DWORD high, low;
      cexcel_record_align_dword( &value );
      high = value & 0xfffffffc;
      low = 0;
      data = cexcel_record_dword2double( high, low ) * 0.01;
    }
  } else {
    /* value not chanegd */
    if ( value & 0x00000002 ) {
      /* signed integer */
      cexcel_record_align_dword( &value );
      data = ( double ) cexcel_record_dword2int( value >> 2 );
    } else {
      /* floting point value */
      CEXCEL_DWORD high, low;
      cexcel_record_align_dword( &value );
      high = value & 0xfffffffc;
      low = 0;
      data = cexcel_record_dword2double( high, low );
    }
  }
  return cexcel_format_double( data, format );
}

void cexcel_record_align_word( CEXCEL_WORD * data )
{
  _cexcel_record_align_endian( ( CEXCEL_UCHAR * ) data, 2 );
}

void cexcel_record_align_dword( CEXCEL_DWORD * data )
{
  _cexcel_record_align_endian( ( CEXCEL_UCHAR * ) data, 4 );
}

int cexcel_record_word2int( CEXCEL_WORD data )
{
  CEXCEL_DWORD ret;
  ret = data;
  _cexcel_record_align_endian( ( CEXCEL_UCHAR * ) & ret, 2 );
  return ( int ) ret;
}

int cexcel_record_dword2int( CEXCEL_DWORD data )
{
  CEXCEL_DWORD ret;
  ret = data;
  _cexcel_record_align_endian( ( CEXCEL_UCHAR * ) & ret, 4 );
  return ( int ) ret;
}

double cexcel_record_dword2double( CEXCEL_DWORD high, CEXCEL_DWORD low )
{
  union {
    CEXCEL_DOUBLE ddata;
    CEXCEL_UCHAR udata[8];
  } ret;
  memcpy( &ret.udata[0], &low, 4 );
  memcpy( &ret.udata[4], &high, 4 );
  _cexcel_record_align_endian( ret.udata, 8 );
  return ( double ) ret.ddata;
}

static void _cexcel_record_align_endian( CEXCEL_UCHAR * data, int len )
{
#ifdef WORDS_BIGENDIAN
  int i, j, half;
  CEXCEL_UCHAR c;
  half = len / 2;
  j = len - 1;
  for ( i = 0; i < half; i++ ) {
    c = data[i];
    data[i] = data[j - i];
    data[j - i] = c;
  }
#endif
  return;
}

static sl4_string_t *_cexcel_record_data_str_to_utf8( sl4_string_t *str,
                                                      CEXCEL_WORD codepage )
{
  sl4_string_t *ret;
  CEXCEL_ICONV_RETTYPE err;
  const char *in_charset, *out_charset;
  in_charset = cexcel_iconv_codepage2charset( codepage );
  if ( in_charset == NULL ) {
    return NULL;
  }
  if ( codepage == 0x2711 ) {
    /* 10001 = Apple Japanese */
    cexcel_iconv_macja2cp932( str );
  }
  ret = sl4_string_new( NULL );
  if ( ret == NULL ) {
    return NULL;
  }
  out_charset = "UTF-8";
  if ( codepage == 0x4b0 ) {
    /* if codepage is UTF-16 then it already set UTF-8 */
    if ( sl4_string_set( ret, sl4_string_get( str ) ) ) {
      sl4_string_delete( ret );
      return NULL;
    }
  } else {
    err = cexcel_iconv_convert( str, ret, in_charset, out_charset );
    if ( err != CEXCEL_ICONV_RETTYPE_SUCCESS ) {
      sl4_string_delete( ret );
      return NULL;
    }
  }
  return ret;
}

static int _cexcel_record_read_continue( cexcel_record_t * record,
                                         cexcel_context_t * ctx,
                                         size_t *offset )
{
  if ( cexcel_record_read( ctx->file, record ) == 0 ) {
    if ( record->id != CEXCEL_ID_CONTINUE ) {
      return -1;
    }
    *offset = 0;
  }
#ifdef _CEXCEL_DEBUG_BIFF
  printf( "%08lx %03x CONTINUE - Length(%d)\n", record->offset, record->id,
          record->len );
#endif
  return 0;
}
