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

int strtomonth(int *mm, const char *str)
{
  static const char *sname[] = {
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec",
    NULL
  };    
  int i;
  for (i = 0; sname[i]; i++) {
    if (strcmp(sname[i], str) == 0) {
        *mm = i + 1;
        return 1;
    }
  }
  return 0;
}

int strtotime(time_t *time, const char *s)
{
  struct tm tm;
  int dd, mm, yy;
  int hrs, min, sec;
  char buf[20];
  if (sscanf(s, "%u-%u-%u %u:%u:%u", &yy, &mm, &dd, &hrs, &min, &sec) != 6) {
    if (sscanf(s, "%u/%u/%u %u:%u:%u", &yy, &mm, &dd, &hrs, &min, &sec) != 6) {
      if (sscanf(s, "%u-%19[a-zA-Z]-%u %u:%u:%u", &yy, buf, &dd, &hrs, &min, &sec) != 6 && strtomonth(&mm, buf) == 0) {
	hrs = 0; min = 0; sec = 0;
        if (sscanf(s, "%u-%u-%u", &yy, &mm, &dd) != 3) {
          if (sscanf(s, "%u/%u/%u", &yy, &mm, &dd) != 3) {
            if (sscanf(s, "%u-%19[a-zA-Z]-%u", &yy, buf, &dd) != 3 && strtomonth(&mm, buf) == 0) {
              return 0;
	    }
          }
        }
      }
    }
  }
  // Do some sanity checking to rule out 2015-mar-35 and such
  tm.tm_sec = sec;
  tm.tm_min = min;
  tm.tm_hour = hrs;
  tm.tm_mday = dd;
  tm.tm_mon = mm - 1;
  tm.tm_year = yy - 1900;
  *time = mktime(&tm);
  return 1;
}

sl4_string_t *tmpl_datetime_get( const char *text, int is_required, char *item_xml )
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
    time_t time;
    if (strtotime(&time, text) == 0) {
      /* invalid cell data */
      sl4_string_delete( str );
      return NULL;
    }
    if (is_compat) {
      sl4_string_sprintf(str, "%ld", time);
    } else {
      sl4_string_sprintf(str, "<C:%s C:type=\"extend\" C:occurrence_number=\"1\">%ld</C:%s>", item_xml, time, item_xml);
    }
  }
  return str;
}
