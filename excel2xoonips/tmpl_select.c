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

sl4_string_t *tmpl_select_get( const char *text, strarray_t * options,
                               int is_required, char *item_xml )
{
  sl4_string_t *str;
  size_t len;
  str = sl4_string_new( NULL );
  if ( str == NULL ) {
    ERROR_OUTMEM(  );
  }
  len = strlen( text );
  if ( len == 0 ) {
    if ( is_required ) {
      /* empty cell data */
      sl4_string_delete( str );
      return NULL;
    }
  } else {
    size_t pos, olen;
    sl4_string_t *sel;
    olen = strarray_length( options );
    for ( pos = 0; pos < olen; pos++ ) {
      sel = strarray_get( options, pos );
      if ( sl4_string_compare( sel, text ) == 0 ) {
        if ( sl4_string_set( str, text ) != 0 ) {
          ERROR_OUTMEM(  );
        }
        break;
      }
    }
    if ( pos >= olen ) {
      /* invalid cell data */
      sl4_string_delete( str );
      return NULL;
    }
  }
  if ( !is_compat && len != 0 ) {
      sl4_string_htmlspecialchars( str );
	  sl4_string_sprintf( str, "<C:%s C:type=\"extend\" C:occurrence_number=\"1\">%s</C:%s>", item_xml, sl4_string_get(str), item_xml);
  }
  return str;
}
