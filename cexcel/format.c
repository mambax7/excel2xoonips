/**
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

/* #define CEXCEL_FORMAT_DEBUG 1 */

static void _cexcel_format_general( sl4_string_t *str, double data );

sl4_string_t *cexcel_format_double( double data, sl4_string_t *format )
{
  sl4_string_t *str;
  str = sl4_string_new( NULL );
  if ( str == NULL ) {
    return NULL;
  }
  _cexcel_format_general( str, data );
  if ( sl4_string_compare( format, "General" ) != 0 &&
       sl4_string_compare( format, "@" ) != 0 ) {
    printf( "[warn] Unsupported Cell Number Format Found : %s\n",
            sl4_string_get( format ) );
    printf
      ( "[warn] - Maybe unexpected string (%s) appear in converted data\n",
        sl4_string_get( str ) );
  }
#ifdef CEXCEL_FORMAT_DEBUG
  printf( "<STR> FORMAT: %s, DATA: %.9f, STR: %s\n",
          sl4_string_get( format ), data, sl4_string_get( str ) );
#endif
  return str;
}

static void _cexcel_format_general( sl4_string_t *str, double data )
{
  size_t len;
  sl4_string_sprintf( str, "%.9f", data );
  for ( len = sl4_string_length( str );
        len > 0 && sl4_string_get_at( str, len - 1 ) == '0'; len-- ) {
    sl4_string_erase( str, len - 1, 1 );
  }
  if ( len > 0 && sl4_string_get_at( str, len - 1 ) == '.' ) {
    sl4_string_erase( str, len - 1, 1 );
  }
}
