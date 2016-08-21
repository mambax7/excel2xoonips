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

static int _cexcel_sheet_is_empty_last_row( cexcel_sheet_t * sheet );
static int _cexcel_sheet_is_empty_last_col( cexcel_sheet_t * sheet );
static int _cexcel_sheet_remove_last_row( cexcel_sheet_t * sheet );
static int _cexcel_sheet_remove_last_col( cexcel_sheet_t * sheet );
static sl4_string_t *_cexcel_sheet_hash_key( unsigned int row,
                                             unsigned int col );
static void _cexcel_sheet_hash_table_delete( void *data, void *arg );

cexcel_sheet_t *cexcel_sheet_new( const char *name )
{
  cexcel_sheet_t *sheet;
  sheet = ( cexcel_sheet_t * ) malloc( sizeof( cexcel_sheet_t ) );
  if ( sheet == NULL )
    return NULL;
  sheet->rows = 0;
  sheet->cols = 0;
  sheet->cells = sl4_hash_table_str_new(  );
  if ( sheet->cells == NULL ) {
    free( sheet );
    return NULL;
  }
  sheet->name = sl4_string_new( name );
  if ( sheet->name == NULL ) {
    sl4_hash_table_delete( sheet->cells, _cexcel_sheet_hash_table_delete,
                           NULL );
    free( sheet );
    return NULL;
  }
  return sheet;
}

int cexcel_sheet_delete( cexcel_sheet_t * sheet )
{
  if ( sheet == NULL ) {
    return -1;
  }
  sl4_hash_table_delete( sheet->cells, _cexcel_sheet_hash_table_delete,
                         NULL );
  sl4_string_delete( sheet->name );
  free( sheet );
  return 0;
}

int cexcel_sheet_set_cell( cexcel_sheet_t * sheet, const char *text,
                           unsigned int row, unsigned int col )
{
  sl4_string_t *key;
  const char *key_cstr;
  cexcel_cell_t *cell, *orig;
  if ( sheet == NULL ) {
    return -1;
  }
  key = _cexcel_sheet_hash_key( row, col );
  if ( key == NULL ) {
    return -1;
  }
  key_cstr = sl4_string_get( key );
  orig = ( cexcel_cell_t * ) sl4_hash_table_lookup( sheet->cells, key_cstr );
  cell = cexcel_cell_new( text, row, col );
  if ( cell == NULL ) {
    sl4_string_delete( key );
    return -1;
  }
  if ( sl4_hash_table_insert( sheet->cells, key_cstr, cell ) != 0 ) {
    sl4_string_delete( key );
    return -1;
  }
  if ( orig != NULL ) {
    cexcel_cell_delete( orig );
  }
  if ( sheet->cols <= col )
    sheet->cols = col + 1;
  if ( sheet->rows <= row )
    sheet->rows = row + 1;
  sl4_string_delete( key );
  return 0;
}

cexcel_cell_t *cexcel_sheet_get_cell( cexcel_sheet_t * sheet,
                                      unsigned int row, unsigned int col )
{
  sl4_string_t *key;
  const char *key_cstr;
  cexcel_cell_t *cell;
  if ( sheet == NULL )
    return NULL;
  if ( sheet->rows < row || sheet->cols < col )
    return NULL;
  key = _cexcel_sheet_hash_key( row, col );
  if ( key == NULL )
    return NULL;
  key_cstr = sl4_string_get( key );
  cell = ( cexcel_cell_t * ) sl4_hash_table_lookup( sheet->cells, key_cstr );
  if ( cell == NULL ) {
    /* empty temporary add */
    if ( cexcel_sheet_set_cell( sheet, "", row, col ) != 0 ) {
      return NULL;
    }
    cell =
      ( cexcel_cell_t * ) sl4_hash_table_lookup( sheet->cells, key_cstr );
  }
  sl4_string_delete( key );
  return cell;
}

unsigned int cexcel_sheet_get_rows( cexcel_sheet_t * sheet )
{
  return sheet->rows;
}

unsigned int cexcel_sheet_get_cols( cexcel_sheet_t * sheet )
{
  return sheet->cols;
}

const char *cexcel_sheet_get_name( cexcel_sheet_t * sheet )
{
  return sl4_string_get( sheet->name );
}

int cexcel_sheet_trim_empty_cells( cexcel_sheet_t * sheet )
{
  if ( sheet->rows == 0 || sheet->cols == 0 ) {
    /* empty sheet */
    return 0;
  }

  /* trim row lines */
  while ( _cexcel_sheet_is_empty_last_row( sheet ) ) {
    _cexcel_sheet_remove_last_row( sheet );
  }
  /* trim col lines */
  while ( _cexcel_sheet_is_empty_last_col( sheet ) ) {
    _cexcel_sheet_remove_last_col( sheet );
  }

  return 0;
}

static int _cexcel_sheet_is_empty_last_row( cexcel_sheet_t * sheet )
{
  int cnt = 1;
  unsigned int row, col;
  cexcel_cell_t *cell;
  const char *text;
  if ( sheet->rows == 0 ) {
    return 0;
  }
  row = sheet->rows - 1;
  for ( col = sheet->cols - 1; 1; col-- ) {
    cell = cexcel_sheet_get_cell( sheet, row, col );
    text = cexcel_cell_get_text( cell );
    if ( *text != '\0' ) {
      cnt = 0;
      break;
    }
    if ( col == 0 ) {
      break;
    }
  }
  return cnt;
}

static int _cexcel_sheet_is_empty_last_col( cexcel_sheet_t * sheet )
{
  int cnt = 1;
  unsigned int row, col;
  cexcel_cell_t *cell;
  const char *text;
  if ( sheet->cols == 0 ) {
    return 0;
  }
  col = sheet->cols - 1;
  for ( row = sheet->rows - 1; 1; row-- ) {
    cell = cexcel_sheet_get_cell( sheet, row, col );
    text = cexcel_cell_get_text( cell );
    if ( *text != '\0' ) {
      cnt = 0;
      break;
    }
    if ( row == 0 ) {
      break;
    }
  }
  return cnt;
}

static int _cexcel_sheet_remove_last_row( cexcel_sheet_t * sheet )
{
  unsigned int row, col;
  sl4_string_t *key;
  cexcel_cell_t *cell;
  if ( sheet->rows == 0 ) {
    return -1;
  }
  row = sheet->rows - 1;
  for ( col = 0; col < sheet->cols; col++ ) {
    key = _cexcel_sheet_hash_key( row, col );
    if ( key == NULL ) {
      return -1;
    }
    cell = ( cexcel_cell_t * ) sl4_hash_table_remove( sheet->cells,
                                                      sl4_string_get( key ) );
    sl4_string_delete( key );
    cexcel_cell_delete( cell );
  }
  sheet->rows--;
  if ( sheet->rows == 0 ) {
    sheet->cols = 0;
  }
  return 0;
}

static int _cexcel_sheet_remove_last_col( cexcel_sheet_t * sheet )
{
  unsigned int row, col;
  sl4_string_t *key;
  cexcel_cell_t *cell;
  if ( sheet->cols == 0 ) {
    return -1;
  }
  col = sheet->cols - 1;
  for ( row = 0; row < sheet->rows; row++ ) {
    key = _cexcel_sheet_hash_key( row, col );
    if ( key == NULL ) {
      return -1;
    }
    cell = ( cexcel_cell_t * ) sl4_hash_table_remove( sheet->cells,
                                                      sl4_string_get( key ) );
    sl4_string_delete( key );
    cexcel_cell_delete( cell );
  }
  sheet->cols--;
  if ( sheet->cols == 0 ) {
    sheet->rows = 0;
  }
  return 0;
}

static sl4_string_t *_cexcel_sheet_hash_key( unsigned int row,
                                             unsigned int col )
{
  sl4_string_t *key = sl4_string_new( NULL );
  if ( key == NULL )
    return NULL;
  if ( sl4_string_sprintf( key, "%d:%d", row, col ) != 0 ) {
    sl4_string_delete( key );
    return NULL;
  }
  return key;
}

static void _cexcel_sheet_hash_table_delete( void *data, void *arg )
{
  cexcel_cell_t *cell = ( cexcel_cell_t * ) data;
  cexcel_cell_delete( cell );
}
