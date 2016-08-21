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

sl4_string_t *tmpl_array_get( const char *text, strarray_t * options,
                              int is_required, char *item_xml )
{
  size_t ii;
  strarray_t *arr;
  sl4_string_t *tag, *sep, *str, *stag, *etag, *tmp;
  str = sl4_string_new( text );
  if ( !is_compat && sl4_string_empty(str) ) {
    if ( is_required ) {
      sl4_string_delete( str );
      return NULL;
    } else {
      return str;
    }
  }
  tag = strarray_get( options, 0 );
  sep = strarray_get( options, 1 );
  arr = strarray_explode( sep, str );
  sl4_string_clear( str );
  if ( arr == NULL ) {
    return NULL;
  }
  stag = sl4_string_new( NULL );
  etag = sl4_string_new( NULL );
  sl4_string_sprintf( stag, "<%s>", sl4_string_get( tag ) );
  sl4_string_sprintf( etag, "</%s>", sl4_string_get( tag ) );
  for ( ii = 0; ii < strarray_length( arr ); ii++ ) {
    tmp = strarray_get( arr, ii );
    sl4_string_htmlspecialchars( tmp );
    if ( is_required && sl4_string_empty( tmp ) ) {
      sl4_string_delete( etag );
      sl4_string_delete( stag );
      strarray_delete( arr );
      sl4_string_delete( str );
      return NULL;
    }
    if (is_compat) {
      sl4_string_append( str, sl4_string_get( stag ) );
      sl4_string_append( str, sl4_string_get( tmp ) );
      sl4_string_append( str, sl4_string_get( etag ) );
    } else {
      sl4_string_t *row_tmp = sl4_string_new( NULL );
      if ( row_tmp == NULL ) {
        ERROR_OUTMEM(  );
      }
      if (strcmp(item_xml, "keyword") == 0 ) { // *.keyword
        sl4_string_sprintf( row_tmp, "<C:%s C:type=\"keyword\" C:keyword_id=\"%d\">%s</C:%s>", item_xml, ii + 1, sl4_string_get(tmp), item_xml);
      } else {
        sl4_string_sprintf( row_tmp, "<C:%s C:type=\"extend\" C:occurrence_number=\"%d\">%s</C:%s>", item_xml, ii + 1, sl4_string_get(tmp), item_xml);
      }
      if ( ii > 0 ) {
    	sl4_string_append( str, "\n" );
      }
      sl4_string_append( str, sl4_string_get( row_tmp ) );
      sl4_string_delete( row_tmp );
    }
  }
  sl4_string_delete( etag );
  sl4_string_delete( stag );
  strarray_delete( arr );
  return str;
}
