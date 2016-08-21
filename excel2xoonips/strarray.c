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
#include "excel2xoonips.h"

strarray_t *strarray_new(  )
{
  sl4_array_t *arr;
  arr = sl4_array_new( sizeof( sl4_string_t * ) );
  return arr;
}

int strarray_delete( strarray_t * arr )
{
  strarray_clear( arr );
  sl4_array_delete( arr );
  return 0;
}

int strarray_add( strarray_t * arr, sl4_string_t *str )
{
  return sl4_array_add( arr, &str );
}

sl4_string_t *strarray_get( strarray_t * arr, size_t pos )
{
  size_t len;
  len = sl4_array_length( arr );
  if ( len <= pos ) {
    return NULL;
  }
  return *( sl4_string_t ** ) sl4_array_get( arr, pos );
}

size_t strarray_length( strarray_t * arr )
{
  return sl4_array_length( arr );
}

int strarray_clear( strarray_t * arr )
{
  size_t i, len;
  sl4_string_t *str;
  len = strarray_length( arr );
  for ( i = 0; i < len; i++ ) {
    str = strarray_get( arr, i );
    if ( str != NULL ) {
      sl4_string_delete( str );
    }
  }
  sl4_array_clear( arr );
  return 0;
}

strarray_t *strarray_explode( sl4_string_t *sep, sl4_string_t *str )
{
  size_t pos, next, seplen;
  sl4_string_t *tmp;
  strarray_t *arr;
  seplen = sl4_string_length( sep );
  if ( seplen == 0 ) {
    return NULL;
  }
  arr = strarray_new(  );
  if ( arr == NULL ) {
    return NULL;
  }
  pos = 0;
  for ( next = sl4_string_findn( str, sl4_string_get( sep ), pos );
        next != ( size_t ) -1;
        next = sl4_string_findn( str, sl4_string_get( sep ), pos ) ) {
    if ( next - pos == 0 ) {
      tmp = sl4_string_new( NULL );
    } else {
      tmp = sl4_string_substr( str, pos, next - pos );
    }
    strarray_add( arr, tmp );
    pos = next + seplen;
  }
  tmp = sl4_string_substr( str, pos, 0 );
  strarray_add( arr, tmp );
  return arr;
}
