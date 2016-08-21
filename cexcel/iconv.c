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

#ifndef HAVE_ICONV
# error iconv() required
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <iconv.h>

#include "cole.h"
#include "libsl4.h"
#include "cexcel.h"
#include "cexcel_internal.h"

CEXCEL_ICONV_RETTYPE cexcel_iconv_convert( sl4_string_t *in,
                                           sl4_string_t *out,
                                           const char *in_charset,
                                           const char *out_charset )
{
  size_t in_len, in_left, out_size, out_left;
  char *in_buf, *in_p, *out_buf, *out_p, *tmp_buf;
  size_t bsz, result = 0;
  iconv_t cd;

  in_p = in_buf = ( char * ) sl4_string_get( in );
  in_left = in_len = sl4_string_length( in );
  out_left = in_len + 32;
  out_size = 0;
  bsz = out_left;

  if ( in_left == 0 ) {
    return CEXCEL_ICONV_RETTYPE_SUCCESS;
  }

  cd = iconv_open( out_charset, in_charset );
  if ( cd == ( iconv_t ) ( -1 ) ) {
    if ( errno == EINVAL ) {
      /* wrong charset */
      return CEXCEL_ICONV_RETTYPE_WRONGCHARSET;
    } else {
      /* unknown errro */
      return CEXCEL_ICONV_RETTYPE_UNKNOWN;
    }
  }
  /* allocate output buffer */
  out_buf = ( char * ) malloc( bsz + 1 );
  if ( out_buf == NULL ) {
    iconv_close( cd );
    return CEXCEL_ICONV_RETTYPE_OUTMEM;
  }
  out_p = out_buf;
  /* start charset conversion */
  for ( ;; ) {
    result =
      iconv( cd, ( ICONV_CONST char ** ) &in_p, &in_left,
             ( char ** ) &out_p, &out_left );
    if ( result == ( size_t ) ( -1 ) ) {
      if ( errno == E2BIG ) {
        /* converted string is longer than out buffer */
        bsz += in_len;
        tmp_buf = ( char * ) realloc( out_buf, bsz + 1 );
        if ( tmp_buf == NULL ) {
          free( out_buf );
          iconv_close( cd );
          return CEXCEL_ICONV_RETTYPE_OUTMEM;
        }
        out_p = out_buf = tmp_buf;
        out_p += out_size;
        out_left = bsz - out_size;
        continue;
      }
      break;
    }
    break;
  }
  /* flush the shift-out sequences */
  if ( result != ( size_t ) ( -1 ) ) {
    for ( ;; ) {
      result = iconv( cd, NULL, NULL, ( char ** ) &out_p, &out_left );
      out_size = bsz - out_left;
      if ( result == ( size_t ) ( -1 ) ) {
        if ( errno == E2BIG ) {
          bsz += 16;
          tmp_buf = ( char * ) realloc( out_buf, bsz );
          if ( tmp_buf == NULL ) {
            free( out_buf );
            iconv_close( cd );
            return CEXCEL_ICONV_RETTYPE_OUTMEM;
          }
          out_p = out_buf = tmp_buf;
          out_p += out_size;
          out_left = bsz - out_size;
          continue;
        }
        break;
      }
      break;
    }
  }
  /* close */
  iconv_close( cd );
  /* check conversion results */
  if ( result == ( size_t ) ( -1 ) ) {
    free( out_buf );
    switch ( errno ) {
    case EINVAL:
      return CEXCEL_ICONV_RETTYPE_ILLCHAR;
      break;
    case EILSEQ:
      return CEXCEL_ICONV_RETTYPE_ILLSEQ;
      break;
    default:
      return CEXCEL_ICONV_RETTYPE_UNKNOWN;
      break;
    }
  }
  /* copy output buffer */
  *out_p = '\0';
  if ( sl4_string_set( out, out_buf ) ) {
    free( out_buf );
    return CEXCEL_ICONV_RETTYPE_OUTMEM;
  }
  free( out_buf );
  /* finish */
  return CEXCEL_ICONV_RETTYPE_SUCCESS;
}

const char *cexcel_iconv_codepage2charset( CEXCEL_WORD codepage )
{
  const char *in_charset;
  /* charset map */
  switch ( codepage ) {
  case 0x16f:                  /* 367 = ASCII */
    in_charset = "ASCII";
    break;
  case 0x1b5:                  /* 437 = IBM PC CP-437 (US) */
    in_charset = "CP437";
    break;
  case 0x2d0:                  /* 720 = IBM PC CP-720 (OEM Arabic) */
    in_charset = "CP720";
    return NULL;                /* does not work on iconv() */
    break;
  case 0x2e1:                  /* 737 = IBM PC CP-737 (Greek) */
    in_charset = "CP737";
    break;
  case 0x307:                  /* 775 = IBM PC CP-775 (Baltic) */
    in_charset = "CP775";
    break;
  case 0x352:                  /* 850 = IBM PC CP-850 (Latin I) */
    in_charset = "CP850";
    break;
  case 0x354:                  /* 852 = IBM PC CP-852 (Latin II (Central European)) */
    in_charset = "CP852";
    break;
  case 0x357:                  /* 855 = IBM PC CP-855 (Cyrillic) */
    in_charset = "CP855";
    break;
  case 0x359:                  /* 857 = IBM PC CP-857 (Turkish) */
    in_charset = "CP857";
    break;
  case 0x35a:                  /* 858 = IBM PC CP-858 (Multilingual Latin I with Euro) */
    in_charset = "CP857";
    break;
  case 0x35c:                  /* 860 = IBM PC CP-860 (Portuguese) */
    in_charset = "CP860";
    break;
  case 0x35d:                  /* 861 = IBM PC CP-861 (Icelandic) */
    in_charset = "CP861";
    break;
  case 0x35e:                  /* 862 = IBM PC CP-862 (Hebrew) */
    in_charset = "CP862";
    break;
  case 0x35f:                  /* 863 = IBM PC CP-863 (Canadian (French)) */
    in_charset = "CP863";
    break;
  case 0x360:                  /* 864 = IBM PC CP-864 (Arabic) */
    in_charset = "CP864";
    break;
  case 0x361:                  /* 865 = IBM PC CP-865 (Nordic) */
    in_charset = "CP865";
    break;
  case 0x362:                  /* 866 = IBM PC CP-866 (Cyrillic (Russian)) */
    in_charset = "CP866";
    break;
  case 0x365:                  /* 869 = IBM PC CP-869 (Greek (Modern)) */
    in_charset = "CP869";
    break;
  case 0x36a:                  /* 874 = Windows CP-874 (Thai) */
    in_charset = "CP874";
    break;
  case 0x3a4:                  /* 932 = Windows CP-932 (Japanese Shift-JIS) */
    in_charset = "CP932";
    break;
  case 0x3a8:                  /* 936 = Windows CP-936 (Chinese Simplified GBK) */
    in_charset = "CP936";
    break;
  case 0x3b5:                  /* 949 = Windows CP-949 (Korean(Wansung)) */
    in_charset = "CP949";
    break;
  case 0x3b6:                  /* 950 = Windows CP-950 (Chinese Traditional BIG5) */
    in_charset = "CP950";
    break;
  case 0x4b0:                  /* 1200 = UTF-16 (BIFF8) */
    in_charset = "UTF-16LE";
    break;
  case 0x4e2:                  /* 1250 = Windows CP-1250 (Latin II (Central European)) */
    in_charset = "CP1250";
    break;
  case 0x4e3:                  /* 1251 = Windows CP-1251 (Cyrillic) */
    in_charset = "CP1251";
    break;
  case 0x4e4:                  /* 1252 = Windows CP-1252 (Latin I) (BIFF4-BIFF5) */
    in_charset = "CP1252";
    break;
  case 0x4e5:                  /* 1253 = Windows CP-1253 (Greek) */
    in_charset = "CP1253";
    break;
  case 0x4e6:                  /* 1254 = Windows CP-1254 (Turkish) */
    in_charset = "CP1254";
    break;
  case 0x4e7:                  /* 1255 = Windows CP-1255 (Hebrew) */
    in_charset = "CP1255";
    break;
  case 0x4e8:                  /* 1256 = Windows CP-1256 (Arabic) */
    in_charset = "CP1256";
    break;
  case 0x4e9:                  /* 1257 = Windows CP-1257 (Baltic) */
    in_charset = "CP1257";
    break;
  case 0x4ea:                  /* 1258 = Windows CP-1258 (Vietnamese) */
    in_charset = "CP1258";
    break;
  case 0x551:                  /* 1361 = Windows CP-1361 (Korean (Johab)) */
    in_charset = "CP1361";
    break;
  case 0x2710:                 /* 10000 = Apple Roman */
    in_charset = "MACINTOSH";
    break;
  case 0x2711:                 /* 10001 = Apple Japanese */
    in_charset = "CP932";
    break;
  case 0x8000:                 /* 32768 = Apple Roman */
    in_charset = "MACINTOSH";
    break;
  case 0x8001:                 /* 32769 = Windows CP-1252 (Latin I) (BIFF2-BIFF3) */
    in_charset = "CP1252";
    break;
  default:
    return NULL;                /* does not supported */
  }
  return in_charset;
}

void cexcel_iconv_macja2cp932( sl4_string_t *str )
{
  size_t i, j, len;
  CEXCEL_UCHAR uchar, *ch;
  CEXCEL_WORD word;
  sl4_string_t *tmp;
  CEXCEL_WORD from_map_word2str[] = {
    /* PARENTHESIZED DIGIT & NUMBER */
    0x855e, 0x855f, 0x8560, 0x8561, 0x8562,
    0x8563, 0x8564, 0x8565, 0x8566, 0x8567,
    0x8568, 0x8569, 0x856a, 0x856b, 0x856c,
    0x856d, 0x856e, 0x856f, 0x8570, 0x8571,
    /* DIGIT FULL STOP */
    0x8591, 0x8592, 0x8593, 0x8594, 0x8595,
    0x8596, 0x8597, 0x8598, 0x8599, 0x859a,
    /* ROMAN NUMERICAL (11 - 15 ) */
    0x85a9, 0x85aa, 0x85ab, 0x85ac, 0x85ad,
    /* SMALL ROMAN NUMERICAL (11 - 15 ) */
    0x85bd, 0x85be, 0x85bf, 0x85c0, 0x85c1,
    /* PARENTHESIZED LATIN SMALL LETTER */
    0x85db, 0x85dc, 0x85dd, 0x85de, 0x85df,
    0x85e0, 0x85e1, 0x85e2, 0x85e3, 0x85e4,
    0x85e5, 0x85e6, 0x85e7, 0x85e8, 0x85e9,
    0x85ea, 0x85eb, 0x85ec, 0x85ed, 0x85ee,
    0x85ef, 0x85f0, 0x85f1, 0x85f2, 0x85f3,
    0x85f4,
    /* ENDMARK */
    0xffff,
  };
  const char *to_map_word2str[] = {
    /* PARENTHESIZED DIGIT & NUMBER */
    "(1)", "(2)", "(3)", "(4)", "(5)",
    "(6)", "(7)", "(8)", "(9)", "(10)",
    "(11)", "(12)", "(13)", "(14)", "(15)",
    "(16)", "(17)", "(18)", "(19)", "(20)",
    /* DIGIT FULL STOP */
    "0.", "1.", "2.", "3.", "4.",
    "5.", "6.", "7.", "8.", "9.",
    /* ROMAN NUMERICAL (11 - 15) */
    "XI", "XII", "XIII", "XIV", "XV",
    /* SMALL ROMAN NUMERICAL (11 - 15 ) */
    "xi", "xii", "xiii", "xiv", "xv",
    /* PARENTHESIZED LATIN SMALL LETTER */
    "(a)", "(b)", "(c)", "(d)", "(e)",
    "(f)", "(g)", "(h)", "(i)", "(j)",
    "(k)", "(l)", "(m)", "(n)", "(o)",
    "(p)", "(q)", "(r)", "(s)", "(t)",
    "(u)", "(v)", "(w)", "(x)", "(y)",
    "(x)"
  };
  CEXCEL_WORD from_map_word2word[] = {
    /* CIRCLED DIGIT & NUMBER */
    0x8540, 0x8541, 0x8542, 0x8543, 0x8544,
    0x8545, 0x8546, 0x8547, 0x8548, 0x8549,
    0x854a, 0x854b, 0x854c, 0x854d, 0x854e,
    0x854f, 0x8550, 0x8551, 0x8552, 0x8553,
    /* DINGBAT NEGATIVE CIRCLED DIGIT */
    0x857c, 0x857d, 0x857e, 0x8580, 0x8581,
    0x8582, 0x8583, 0x8584, 0x8585,
    /* ROMAN NUMERICAL (1 - 10) */
    0x859f, 0x85a0, 0x85a1, 0x85a2, 0x85a3,
    0x85a4, 0x85a5, 0x85a6, 0x85a7, 0x85a8,
    /* SMALL ROMAN NUMERICAL (1 - 10) */
    0x85b3, 0x85b4, 0x85b5, 0x85b6, 0x85b7,
    0x85b8, 0x85b9, 0x85ba, 0x85bb, 0x85bc,
    /* ENDMARK */
    0xffff,
  };
  CEXCEL_WORD to_map_word2word[] = {
    /* CIRCLED DIGIT & NUMBER */
    0x8740, 0x8741, 0x8742, 0x8743, 0x8744,
    0x8745, 0x8746, 0x8747, 0x8748, 0x8749,
    0x874a, 0x874b, 0x874c, 0x874d, 0x874e,
    0x874f, 0x8750, 0x8751, 0x8752, 0x8753,
    /* DINGBAT NEGATIVE CIRCLED DIGIT */
    0x8740, 0x8741, 0x8742, 0x8743, 0x8744,
    0x8745, 0x8746, 0x8747, 0x8748,
    /* ROMAN NUMERICAL (1 - 10) */
    0x8754, 0x8755, 0x8756, 0x8757, 0x8758,
    0x8759, 0x875a, 0x875b, 0x875c, 0x875d,
    /* SMALL ROMAN NUMERICAL (1 - 10) */
    0xeeef, 0xeef0, 0xeef1, 0xeef2, 0xeef3,
    0xeef4, 0xeef5, 0xeef6, 0xeef7, 0xeef8,
  };
  len = sl4_string_length( str );
  if ( len == 0 ) {
    return;
  }
  tmp = sl4_string_new( NULL );
  if ( tmp == NULL ) {
    return;
  }
  ch = ( CEXCEL_UCHAR * ) sl4_string_get( str );
  for ( i = 0; i < len; i++ ) {
    if ( ch[i] == 0x80 ) {
      /* reverse solidus -> reverse solidus (ASCII) */
      sl4_string_append_char( tmp, 0x5c );
    } else if ( ch[i] == 0xa0 ) {
      /* no-break space -> " " */
      sl4_string_append_char( tmp, ' ' );
    } else if ( ch[i] == 0xfd ) {
      /* copyright sign -> "(C)" */
      sl4_string_append( tmp, "(C)" );
    } else if ( ch[i] == 0xfe ) {
      /* trade mark sign -> "TM" */
      sl4_string_append( tmp, "TM" );
    } else if ( ch[i] == 0xff ) {
      /* half width horizontal ellipsis -> full width */
      sl4_string_append_char( tmp, 0x81 );
      sl4_string_append_char( tmp, 0x63 );
    } else if ( ch[i] >= 0x81 && ch[i] <= 0x9f ) {
      /* dbcs lead byte */
      word = ( ( CEXCEL_WORD ) ch[i] << 8 ) + ch[i + 1];
      if ( word >= 0x8540 && word <= 0x886d ) {
        int next = 1;
        for ( j = 0; from_map_word2word[j] != 0xffff; j++ ) {
          if ( word == from_map_word2word[j] ) {
            uchar = ( CEXCEL_UCHAR ) ( ( to_map_word2word[j] >> 8 ) & 0xff );
            sl4_string_append_char( tmp, ( char ) uchar );
            uchar = ( CEXCEL_UCHAR ) ( to_map_word2word[j] & 0xff );
            sl4_string_append_char( tmp, ( char ) uchar );
            next = 0;
            break;
          }
        }
        if ( next ) {
          for ( j = 0; from_map_word2str[j] != 0xffff; j++ ) {
            if ( word == from_map_word2str[j] ) {
              sl4_string_append( tmp, to_map_word2str[j] );
              next = 0;
              break;
            }
          }
        }
        if ( next ) {
          /* give up */
          sl4_string_append( tmp, "??" );
        }
      } else {
        sl4_string_append_char( tmp, ch[i] );
        sl4_string_append_char( tmp, ch[i + 1] );
      }
      i++;
    } else if ( ch[i] >= 0xe0 && ch[i] <= 0xfc ) {
      /* cjk unified/compatibility ideograph */
      word = ( ( CEXCEL_WORD ) ch[i] << 8 ) + ch[i + 1];
      if ( word >= 0xeb41 && word <= 0xed96 ) {
        sl4_string_append( tmp, "??" );
      } else {
        sl4_string_append_char( tmp, ch[i] );
        sl4_string_append_char( tmp, ch[i + 1] );
      }
      i++;
    } else {
      sl4_string_append_char( tmp, ch[i] );
    }
  }
  sl4_string_set( str, sl4_string_get( tmp ) );
  sl4_string_delete( tmp );
}
