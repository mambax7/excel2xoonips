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

static void _cexcel_book_hash_table_delete( void *data, void *arg );

cexcel_book_t *cexcel_book_new( const char *filename )
{
  cexcel_book_t *book;
  book = ( cexcel_book_t * ) malloc( sizeof( cexcel_book_t ) );
  if ( book == NULL )
    return NULL;
  book->nums = 0;
  book->sheets = cexcel_hash_new(  );
  if ( book->sheets == NULL ) {
    free( book );
    return NULL;
  }
  book->filename = sl4_string_new( filename );
  if ( book->filename == NULL ) {
    sl4_hash_table_delete( book->sheets, _cexcel_book_hash_table_delete,
                           NULL );
    free( book );
    return NULL;
  }
  return book;
}

int cexcel_book_delete( cexcel_book_t * book )
{
  if ( book == NULL ) {
    return -1;
  }
  sl4_hash_table_delete( book->sheets, _cexcel_book_hash_table_delete, NULL );
  sl4_string_delete( book->filename );
  free( book );
  return 0;
}

cexcel_sheet_t *cexcel_book_add_sheet( cexcel_book_t * book,
                                       const char *name )
{
  cexcel_sheet_t *sheet;
  unsigned int num;
  if ( book == NULL ) {
    return NULL;
  }
  num = book->nums + 1;
  sheet = cexcel_sheet_new( name );
  if ( sheet == NULL ) {
    return NULL;
  }
  if ( sl4_hash_table_insert( book->sheets, &num, sheet ) != 0 ) {
    return NULL;
  }
  book->nums = num;
  return sheet;
}

cexcel_sheet_t *cexcel_book_get_sheet( cexcel_book_t * book,
                                       unsigned int num )
{
  if ( book == NULL )
    return NULL;
  if ( book->nums < num )
    return NULL;
  return ( cexcel_sheet_t * ) sl4_hash_table_lookup( book->sheets, &num );
}

unsigned int cexcel_book_get_nums( cexcel_book_t * book )
{
  return book->nums;
}

const char *cexcel_book_get_filename( cexcel_book_t * book )
{
  return sl4_string_get( book->filename );
}

static void _cexcel_book_hash_table_delete( void *data, void *arg )
{
  cexcel_sheet_t *sheet = ( cexcel_sheet_t * ) data;
  cexcel_sheet_delete( sheet );
}
