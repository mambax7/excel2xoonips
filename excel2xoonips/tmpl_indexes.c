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

sl4_string_t *tmpl_indexes_get( const char *text, int is_required, char *item_xml )
{
  sl4_string_t *str, *ret, *tmp;
  size_t pos;
  int has_public, has_private;
  has_public = 0;
  has_private = 0;
  ret = sl4_string_new( NULL );
  str = sl4_string_new( text );
  if ( ret == NULL || str == NULL ) {
    ERROR_OUTMEM(  );
  }
  pos = 0;
  tmp = sl4_string_tokenize( str, "\n", &pos );
  while ( tmp ) {
    int is_public = 0;
    sl4_string_t *tag = sl4_string_new( NULL );
    if ( tag == NULL ) {
      ERROR_OUTMEM(  );
    }
    sl4_string_trim( tmp );
    if ( myonig_match( "/^\\/Public(.*)$/", sl4_string_get( tmp ), NULL ) ) {
      is_public = 1;
      has_public = 1;
    } else
      if ( myonig_match
           ( "/^\\/Private(.*)$/", sl4_string_get( tmp ), NULL ) ) {
      is_public = 0;
      has_private = 1;
    } else {
      sl4_string_delete( ret );
      sl4_string_delete( str );
      sl4_string_delete( tmp );
      return NULL;
    }
    if ( is_compat ) {
      sl4_string_sprintf( tag, "<index open_level=\"%s\">%s</index>\n",
                            ( is_public ) ? "public" : "private",
                            sl4_string_get( tmp ) );
    } else {
      sl4_string_sprintf( tag, "<C:index C:type=\"index_item_link\">\n<C:index_title>%s</C:index_title>\n</C:index>\n",
              sl4_string_get( tmp ) );
    }
    sl4_string_append( ret, sl4_string_get( tag ) );
    sl4_string_delete( tag );
    sl4_string_delete( tmp );
    tmp = sl4_string_tokenize( str, "\n", &pos );
  }
  sl4_string_delete( str );
  if ( has_private == 0 && has_public == 0 ) {
    if ( is_required ) {
      /* index is empty */
      sl4_string_delete( ret );
      return NULL;
    }
    if ( is_compat ) {
      if ( sl4_string_set( ret, "<index></index>" ) != 0 ) {
        ERROR_OUTMEM(  );
      }
    } else {
        if ( sl4_string_set( ret, "" ) != 0 ) {
          ERROR_OUTMEM(  );
        }
    }
  }
  if ( has_private == 0 && has_public != 0 ) {
    /* private index not found */
    sl4_string_delete( ret );
    return NULL;
  }
  return ret;
}
