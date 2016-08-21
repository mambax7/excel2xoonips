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
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cole.h"
#include "libsl4.h"
#include "cexcel.h"
#include "cexcel_internal.h"

static sl4_string_t *_cexcel_cell_get_address( unsigned int row,
                                               unsigned int col );

cexcel_cell_t *cexcel_cell_new( const char *text, unsigned int row,
                                unsigned int col )
{
  cexcel_cell_t *cell;
  cell = ( cexcel_cell_t * ) malloc( sizeof( cexcel_cell_t ) );
  if ( cell == NULL )
    return NULL;
  cell->row = row;
  cell->col = col;
  cell->text = sl4_string_new( text );
  cell->address = _cexcel_cell_get_address( row, col );
  if ( cell->text == NULL || cell->address == NULL ) {
    cexcel_cell_delete( cell );
    return NULL;
  }
  return cell;
}

int cexcel_cell_delete( cexcel_cell_t * cell )
{
  if ( cell == NULL )
    return -1;
  if ( cell->text )
    sl4_string_delete( cell->text );
  if ( cell->address )
    sl4_string_delete( cell->address );
  free( cell );
  return 0;
}

const char *cexcel_cell_get_text( cexcel_cell_t * cell )
{
  return sl4_string_get( cell->text );
}

unsigned int cexcel_cell_get_row( cexcel_cell_t * cell )
{
  return cell->row;
}

unsigned int cexcel_cell_get_col( cexcel_cell_t * cell )
{
  return cell->col;
}

const char *cexcel_cell_get_address( cexcel_cell_t * cell )
{
  return sl4_string_get( cell->address );
}

static sl4_string_t *_cexcel_cell_get_address( unsigned int row,
                                               unsigned int col )
{
  sl4_string_t *ret;
  unsigned int tmp;
  char c[2];
  ret = sl4_string_new( NULL );
  if ( ret == NULL )
    return NULL;
  if ( sl4_string_sprintf( ret, "%u", row + 1 ) != 0 ) {
    sl4_string_delete( ret );
    return NULL;
  }
  c[1] = '\0';
  for ( tmp = col + 1; tmp > 26; tmp /= 26 ) {
    c[0] = ( ( tmp - 1 ) % 26 ) + 'A';
    if ( sl4_string_insert( ret, 0, c ) != 0 ) {
      sl4_string_delete( ret );
      return NULL;
    }
  }
  c[0] = ( ( tmp - 1 ) % 26 ) + 'A';
  if ( sl4_string_insert( ret, 0, c ) != 0 ) {
    sl4_string_delete( ret );
    return NULL;
  }
  return ret;
}
