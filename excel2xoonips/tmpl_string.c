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
#include <time.h>

#include "libsl4.h"
#include "cexcel.h"
#include "excel2xoonips.h"

sl4_string_t *tmpl_string_get( const char *text, int is_required, char *item_xml)
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
    if ( sl4_string_set( str, text ) != 0 ) {
      ERROR_OUTMEM(  );
    }
  }
  if ( !is_compat && len != 0 ) {
    if ( strcmp(item_xml, "essential_date") == 0 // *.essential_date
            || strcmp(item_xml, "date") == 0 ) { // date.date
      struct tm tm;
      time_t epoch;
      int year, month, day;

      memset(&tm, 0, sizeof(struct tm));
      if ( sscanf(text, "%d/%d/%d", &year, &month, &day) == 3 ) {
        if ( year < 1970 || year >= 2038  || month > 12 || month < 1 || day < 1 || day > 31) {
            sl4_string_delete( str );
            return NULL;
        }
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;

   	    epoch = mktime(&tm);
        if ( epoch == -1 ) {
            sl4_string_delete( str );
            return NULL;
        }
        if ( sl4_string_sprintf(str, "%ld", epoch) != 0 ) {
          ERROR_OUTMEM(  );
        }
      } else {
        sl4_string_delete( str );
        return NULL;
      }
    }
    sl4_string_htmlspecialchars( str );
    if ( strcmp(item_xml, "essential_title") == 0 // *.essential_title
            || strcmp(item_xml, "ctitle") == 0 // ctitle.ctitle
            || strcmp(item_xml, "title") == 0 ) { // title.title
      sl4_string_sprintf( str, "<C:%s C:type=\"title\" C:title_id=\"1\">%s</C:%s>", item_xml, sl4_string_get(str), item_xml);
    } else if ( strcmp(item_xml, "doi") == 0 ) {
      sl4_string_sprintf( str, "<C:%s C:type=\"item\" C:column_name=\"%s\">%s</C:%s>", item_xml, item_xml, sl4_string_get(str), item_xml);
    } else {
      sl4_string_sprintf( str, "<C:%s C:type=\"extend\" C:occurrence_number=\"1\">%s</C:%s>", item_xml, sl4_string_get(str), item_xml);
    }
  }

  return str;
}
