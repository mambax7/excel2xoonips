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

static void *_cexcel_hash_key_copy( const void *key );
static void _cexcel_hash_key_free( void *key );
static size_t _cexcel_hash_func( const void *key );
static int _cexcel_hash_comp( const void *key1, const void *key2 );

sl4_hash_table_t *cexcel_hash_new(  )
{
  return sl4_hash_table_new( _cexcel_hash_func, _cexcel_hash_comp,
                             _cexcel_hash_key_copy, _cexcel_hash_key_free );
}

/* hash functions - CEXCEL_DWORD version */
static void *_cexcel_hash_key_copy( const void *key )
{
  CEXCEL_DWORD *ret;
  ret = ( CEXCEL_DWORD * ) malloc( sizeof( CEXCEL_DWORD ) );
  if ( ret )
    *ret = *( CEXCEL_DWORD * ) key;
  return ret;
}

static void _cexcel_hash_key_free( void *key )
{
  free( key );
}

static size_t _cexcel_hash_func( const void *key )
{
  return ( size_t ) ( *( CEXCEL_DWORD * ) key );
}

static int _cexcel_hash_comp( const void *key1, const void *key2 )
{
  return ( *( CEXCEL_DWORD * ) key1 ) - ( *( CEXCEL_DWORD * ) key2 );
}
