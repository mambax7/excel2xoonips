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

#include "cole.h"
#include "libsl4.h"
#include "cexcel.h"
#include "cexcel_internal.h"

/*
#define CEXCEL_CONTEXT_FORMAT_DEBUG 1
*/

static void _cexcel_context_format_del_func( void *data, void *arg );

#ifdef CEXCEL_CONTEXT_FORMAT_DEBUG
static void _cexcel_context_format_dump( cexcel_context_t * ctx );
#endif

int cexcel_context_format_new( cexcel_context_t * ctx )
{
  ctx->format = cexcel_hash_new(  );
  if ( ctx->format == NULL ) {
    return -1;
  }
  return 0;
}

int cexcel_context_format_delete( cexcel_context_t * ctx )
{
#ifdef CEXCEL_CONTEXT_FORMAT_DEBUG
  _cexcel_context_format_dump( ctx );
#endif
  sl4_hash_table_delete( ctx->format, _cexcel_context_format_del_func, NULL );
  return 0;
}

int cexcel_context_format_init( cexcel_context_t * ctx )
{
  /* initialize built-in formats */
  struct _builtin_format {
    CEXCEL_DWORD idx;
    const char *type;
    const char *str;
  } builtin_format[] = {
    {
    0, "General", "General"}, {
    1, "Decimal", "0"}, {
    2, "Decimal", "0.00"}, {
    3, "Decimal", "#,##0"}, {
    4, "Decimal", "#,##0.00"}, {
      /* override 5-8 */
    5, "Currency", "\"$\"#,##0_);(\"$\"#,##0)"}, {
    6, "Currency", "\"$\"#,##0_);[Red](\"$\"#,##0)"}, {
    7, "Currency", "\"$\"#,##0.00_);(\"$\"#,##0.00)"}, {
    8, "Currency", "\"$\"#,##0.00_);[Red](\"$\"#,##0.00)"}, {
    9, "Percent", "0%"}, {
    10, "Percent", "0.00%"}, {
    11, "Scientific", "0.00E+00"}, {
    12, "Fraction", "#\\ \?/\?"}, {
    13, "Fraction", "#\\ \?\?/\?\?"}, {
      /* adjust for windows locale 14 */
    14, "Date", "m/d/yy"}, {
    15, "Date", "d-mmm-yy"}, {
    16, "Date", "d-mmm"}, {
    17, "Date", "mmm-yy"}, {
    18, "Time", "h:mm AM/PM"}, {
    19, "Time", "h:mm:ss AM/PM"}, {
    20, "Time", "h:mm"}, {
    21, "Time", "h:mm:ss"}, {
    22, "Date/Time", "m/d/yy h:mm"}, {
    27, "Date", "[$-411]ge\\.m\\.d"}, {
    28, "Date",
        "[$-411]ggge\"\xe5\xb9\xb4\"m\"\xe6\x9c\x88\"d\"\xe6\x97\xa5\""}, {
    29, "Date",
        "[$-411]ggge\"\xe5\xb9\xb4\"m\"\xe6\x9c\x88\"d\"\xe6\x97\xa5\""}, {
    30, "Date", "[$-411]m/d/yy"}, {
    31, "Date",
        "[$-411]yyyy\"\xe5\xb9\xb4\"m\"\xe6\x9c\x88\"d\"\xe6\x97\xa5\""}, {
    32, "Time", "[$-411]h\"\xe6\x99\x82\"mm\"\xe5\x88\x86\""}, {
    33, "Time",
        "[$-0411]h\"\xe6\x99\x82\"mm\"\xe5\x88\x86\"ss\"\xe7\xa7\x92\""}, {
    34, "Date", "[$-411]yyyy\"\xe5\xb9\xb4\"m\"\xe6\x9c\x88\""}, {
    35, "Date", "[$-411]m\"\xe6\x9c\x88\"d\"\xe6\x97\xa5\""}, {
    36, "Date", "[$-411]ge\\.m\\.d"}, {
    37, "Account.", "_(#,##0_);(#,##0)"}, {
    38, "Account.", "_(#,##0_);[Red]_(\\-#,##0_)"}, {
    39, "Account.", "_(#,##0.00_);(#,##0.00)"}, {
    40, "Account.", "_(#,##0.00_);[Red](#,##0.00)"}, {
      /* override 41-43 */
    41, "Currency",
        "_(\"$\"* #,##0_);_(\"$\"* (#,##0);_(\"$\"* \"-\"_);_(@_)"}, {
    42, "Currency", "_(* #,##0_);_(* (#,##0);_(* \"-\"_);_(@_)"}, {
    43, "Currency",
        "_(\"$\"* #,##0.00_);_(\"$\"* (#,##0.00);_(\"$\"* \"-\"??_);_(@_)"}, {
    44, "Currency", "_(* #,##0.00_);_(* (#,##0.00);_(* \"-\"??_);_(@_)"}, {
    45, "Time", "mm:ss"}, {
    46, "Time", "[h]:mm:ss"}, {
    47, "Time", "mm:ss.0"}, {
    48, "Time", "##0.0E+0"}, {
    49, "Time", "@"}, {
    50, "Date", "[$-0411]ge\\.m\\.d"}, {
    51, "Date",
        "[$-0411]ggge\"\xe5\xb9\xb4\"m\"\xe6\x9c\x88\"d\"\xe6\x97\xa5\""}, {
    52, "Date", "[$-0411]yyyy\"\xe5\xb9\xb4\"m\"\xe6\x9c\x88\""}, {
    53, "Date", "[$-0411]m\"\xe6\x9c\x88\"d\"\xe6\x97\xa5\""}, {
    54, "Date",
        "[$-0411]ggge\"\xe5\xb9\xb4\"m\"\xe6\x9c\x88\"d\"\xe6\x97\xa5\""}, {
    55, "Date", "[$-0411]yyyy\"\xe5\xb9\xb4\"m\"\xe6\x9c\x88\""}, {
    56, "Date", "[$-0411]m\"\xe6\x9c\x88\"d\"\xe6\x97\xa5\""}, {
    57, "Date", "[$-0411]ge\\.m\\.d"}, {
    58, "Date",
        "[$-0411]ggge\"\xe5\xb9\xb4\"m\"\xe6\x9c\x88\"d\"\xe6\x97\xa5\""}, {
  0xFFFF, NULL, NULL},};
  CEXCEL_DWORD i;
  sl4_string_t *str;
  for ( i = 0; builtin_format[i].str != NULL; i++ ) {
    str = cexcel_context_format_get( ctx, builtin_format[i].idx );
    if ( str != NULL ) {
      /* format information already exists in excel file */
      /* skip built-in formats */
      continue;
    }
    str = sl4_string_new( builtin_format[i].str );
    if ( str == NULL ) {
      return -1;
    }
    if ( cexcel_context_format_add( ctx, builtin_format[i].idx, str ) != 0 ) {
      return -1;
    }
  }
  return 0;
}

int cexcel_context_format_add( cexcel_context_t * ctx, CEXCEL_DWORD idx,
                               sl4_string_t *str )
{
  if ( idx == ( CEXCEL_DWORD ) - 1 ) {
    /* zero based index */
    idx = ( CEXCEL_DWORD ) ctx->format->nkeys;
  }
  return sl4_hash_table_insert( ctx->format, &idx, str );
}

sl4_string_t *cexcel_context_format_get( cexcel_context_t * ctx,
                                         CEXCEL_DWORD idx )
{
  return ( sl4_string_t * ) sl4_hash_table_lookup( ctx->format, &idx );
}

static void _cexcel_context_format_del_func( void *data, void *arg )
{
  sl4_string_t *str = ( sl4_string_t * ) data;
  sl4_string_delete( str );
}

#ifdef CEXCEL_CONTEXT_FORMAT_DEBUG
static void _cexcel_context_format_dump( cexcel_context_t * ctx )
{
  size_t i;
  sl4_hash_bucket_t *buck;
  sl4_string_t *str;
  CEXCEL_DWORD *key;
  if ( ctx->format->nkeys != 0 ) {
    for ( i = 0; i < ctx->format->nbucks; i++ )
      for ( buck = ctx->format->buckets[i]; buck; buck = buck->next ) {
        key = ( CEXCEL_DWORD * ) buck->key;
        str = ( sl4_string_t * ) buck->data;
        printf( "%4d : %s\n", *key, sl4_string_get( str ) );
      }
  }
}
#endif
