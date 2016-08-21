/*
 * excel2xoonips - A data convertor from Excel data to XooNIps import file
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

#include "libsl4.h"
#include "cexcel.h"
#include "oniguruma.h"
#include "excel2xoonips.h"

static int _myonig_match_body( const char *pattern, const char *str,
                               OnigRegion * resion, OnigOptionType option );
static sl4_string_t *_myonig_parse_pattern( const char *pattern,
                                            OnigOptionType * option );
static sl4_string_t *_myonig_replacement_pattern( const char *replacement,
                                                  const char *str,
                                                  OnigRegion * resion );

int myonig_match( const char *pattern, const char *str, strarray_t * matches )
{
  int ret = 0;
  OnigRegion *region;
  OnigOptionType option;
  sl4_string_t *pat, *tmp, *match;
  tmp = sl4_string_new( str );
  if ( tmp == NULL ) {
    return 0;
  }
  pat = _myonig_parse_pattern( pattern, &option );
  if ( pat == NULL ) {
    sl4_string_delete( tmp );
    return 0;
  }
  region = onig_region_new(  );
  if ( _myonig_match_body( sl4_string_get( pat ), str, region, option ) == 0 ) {
    int i;
    ret = region->num_regs + 1;
    if ( matches != NULL ) {
      strarray_clear( matches );
      for ( i = 0; i < region->num_regs; i++ ) {
        if ( region->beg[i] == -1 || region->beg[i] == -1 ) {
          match = NULL;
        } else {
          match =
            sl4_string_substr( tmp, region->beg[i],
                               region->end[i] - region->beg[i] );
        }
        strarray_add( matches, match );
      }
    }
  }
  onig_region_free( region, 1 );
  onig_end(  );
  sl4_string_delete( pat );
  sl4_string_delete( tmp );
  return ret;
}

sl4_string_t *myonig_replace( const char *pattern, const char *replacement,
                              const char *str )
{
  OnigRegion *region;
  OnigOptionType option;
  sl4_string_t *pat, *ret;
  int cont;
  ret = sl4_string_new( str );
  pat = _myonig_parse_pattern( pattern, &option );
  if ( pat == NULL ) {
    return NULL;
  }
  cont = 1;
  while ( cont ) {
    region = onig_region_new(  );
    if ( _myonig_match_body
         ( sl4_string_get( pat ), sl4_string_get( ret ), region,
           option ) == 0 ) {
      sl4_string_t *rep;
      size_t len;
      rep =
        _myonig_replacement_pattern( replacement, sl4_string_get( ret ),
                                     region );
      len = region->end[0] - region->beg[0];
      sl4_string_replace( ret, region->beg[0], len, sl4_string_get( rep ) );
      sl4_string_delete( rep );
    } else {
      cont = 0;
    }
    onig_region_free( region, 1 );
  }
  onig_end(  );
  sl4_string_delete( pat );
  return ret;
}

static int _myonig_match_body( const char *pattern, const char *str,
                               OnigRegion * region, OnigOptionType option )
{
  int r, res;
  regex_t *reg;
  OnigErrorInfo einfo;
  unsigned char *start, *range, *end;

  r =
    onig_new( &reg, ( UChar * ) pattern,
              ( UChar * ) pattern + strlen( pattern ), option,
              ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL, &einfo );
  if ( r != ONIG_NORMAL ) {
    ERROR_UNEXPECTED(  );
  }

  end = ( unsigned char * ) str + strlen( str );
  start = ( unsigned char * ) str;
  range = end;

  r =
    onig_search( reg, ( const UChar * ) str, end, start, range, region,
                 ONIG_OPTION_NONE );
  res = 0;
  if ( r >= 0 ) {
    /* found */
  } else if ( r == ONIG_MISMATCH ) {
    res = -1;
  } else {
    /* error */
    ERROR_UNEXPECTED(  );
  }

  onig_free( reg );
  return res;
}

static sl4_string_t *_myonig_parse_pattern( const char *pattern,
                                            OnigOptionType * option )
{
  sl4_string_t *str;
  char sep[2], prev, ch, is_escape, is_option;
  size_t i, len, pos;
  OnigOptionType op = ONIG_OPTION_NONE;
  str = sl4_string_new( pattern );
  if ( str == NULL ) {
    return NULL;
  }
  len = sl4_string_length( str );
  if ( len < 3 ) {              /* 3 = BEGIN('/') + BODY + END('/') */
    sl4_string_delete( str );
    return NULL;
  }
  sep[0] = sl4_string_get_at( str, 0 );
  sep[1] = '\0';
  pos = sl4_string_rfind( str, sep );
  if ( pos == 0 ) {
    /* separator not found */
    sl4_string_delete( str );
    return NULL;
  }
  sl4_string_erase( str, 0, 1 );
  len--;
  prev = sep[0];
  is_escape = 0;
  is_option = 0;
  for ( i = 0; i < len; i++ ) {
    ch = sl4_string_get_at( str, i );
    if ( is_option ) {
      switch ( ch ) {
      case 'i':
        op |= ONIG_OPTION_IGNORECASE;
        break;
      case 's':
        op |= ONIG_OPTION_MULTILINE;
        break;
      case 'x':
        op |= ONIG_OPTION_EXTEND;
        break;
      }
      sl4_string_erase( str, i, 1 );
      len--;
      i--;
    } else {
      if ( is_escape ) {
        is_escape = 0;
        if ( ch == sep[0] ) {
          /* escaped separator */
          sl4_string_erase( str, i - 1, 1 );
          len--;
          i--;
        }
      } else {
        if ( ch == '\\' ) {
          is_escape = 1;
        } else {
          if ( ch == sep[0] ) {
            /* tail part of separator found */
            is_option = 1;
            sl4_string_erase( str, i, 1 );
            len--;
            i--;
          }
        }
      }
    }
  }
  if ( is_option == 0 ) {
    sl4_string_delete( str );
    return NULL;
  }
  *option = op;
  return str;
}

static sl4_string_t *_myonig_replacement_pattern( const char *replacement,
                                                  const char *str,
                                                  OnigRegion * region )
{
  sl4_string_t *ret;
  size_t i, len, j;
  char ch;
  int num, in_escape = 0;
  ret = sl4_string_new( NULL );
  len = strlen( replacement );
  for ( i = 0; i < len; i++ ) {
    ch = replacement[i];
    if ( in_escape ) {
      if ( ch >= '0' && ch <= '9' ) {
        num = ch - '0';
        if ( region->num_regs >= num ) {
          for ( j = region->beg[num]; j != region->end[num]; j++ ) {
            sl4_string_append_char( ret, str[j] );
          }
        }
      } else {
        sl4_string_append_char( ret, ch );
      }
      in_escape = 0;
    } else {
      if ( ch == '\\' ) {
        in_escape = 1;
      } else {
        sl4_string_append_char( ret, ch );
      }
    }
  }
  return ret;
}
