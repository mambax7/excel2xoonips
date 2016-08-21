/* 
 * Copyright (c) 2003-2007 RIKEN Japan, All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY RIKEN AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL RIKEN OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/* $Id: hash.c,v 1.2 2008/01/24 14:59:41 orrisroot Exp $ */

#define LIBSL4_EXPORTS

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef WIN32
# define WIN32_LEAN_AND_MEAN
# ifdef __MINGW32__
#  define WINVER 0x0500         /* WindowsME or Windows2000 */
#  define _WIN32_IE 0x0501      /* IE55 */
# endif
# include <windows.h>
#endif

#include <stdio.h>
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif

#include "libsl4.h"

/* dynamic resizeing range of hash bucket size */
#define HASH_BUCKETSIZE_MIN        13
#define HASH_BUCKETSIZE_MAX   5099893

static const size_t _sl4_hash_primes[] = {
  13, 23, 43, 83, 163, 317, 631,
  1259, 2503, 5003, 9973, 19937, 39869, 79699,
  159389, 318751, 637499, 1274989, 2549951, 5099893
};

static size_t _sl4_hash_create_prime( size_t nkeys );
static int _sl4_hash_table_resize( sl4_hash_table_t *table );

static void *_sl4_hash_key_copy_int( const void *key );
static void _sl4_hash_key_free_int( void *key );
static void *_sl4_hash_key_copy_str( const void *key );
static void _sl4_hash_key_free_str( void *key );

static size_t _sl4_hash_func_ptr( const void *key );
static int _sl4_hash_comp_ptr( const void *key1, const void *key2 );
static size_t _sl4_hash_func_int( const void *key );
static int _sl4_hash_comp_int( const void *key1, const void *key2 );
static size_t _sl4_hash_func_str( const void *key );
static int _sl4_hash_comp_str( const void *key1, const void *key2 );

LIBSL4_API sl4_hash_table_t *sl4_hash_table_new( sl4_hash_func_t
                                                 hash_func,
                                                 sl4_hash_comp_t
                                                 hash_comp,
                                                 sl4_hash_key_copy_t
                                                 hash_key_copy,
                                                 sl4_hash_key_free_t
                                                 hash_key_free )
{
  sl4_hash_table_t *table;
  size_t i, size;
  table = ( sl4_hash_table_t * ) malloc( sizeof( sl4_hash_table_t ) );
  if ( table == NULL )
    return NULL;
  size = HASH_BUCKETSIZE_MIN;
  table->buckets =
    ( sl4_hash_bucket_t ** ) malloc( sizeof( sl4_hash_bucket_t * ) * size );
  if ( table->buckets == NULL ) {
    free( table );
    return NULL;
  }
  for ( i = 0; i < size; i++ )
    table->buckets[i] = NULL;
  table->nbucks = size;
  if ( hash_func )
    table->hash_func = hash_func;
  else
    table->hash_func = _sl4_hash_func_ptr;
  if ( hash_comp )
    table->hash_comp = hash_comp;
  else
    table->hash_comp = _sl4_hash_comp_ptr;
  if ( hash_key_copy )
    table->hash_key_copy = hash_key_copy;
  else
    table->hash_key_copy = NULL;
  if ( hash_key_free )
    table->hash_key_free = hash_key_free;
  else
    table->hash_key_free = NULL;
  table->nkeys = 0;
  table->dyn_resize = 1;
  return table;
}

LIBSL4_API sl4_hash_table_t *sl4_hash_table_str_new(  )
{
  return sl4_hash_table_new( _sl4_hash_func_str, _sl4_hash_comp_str,
                             _sl4_hash_key_copy_str, _sl4_hash_key_free_str );
}

LIBSL4_API sl4_hash_table_t *sl4_hash_table_int_new(  )
{
  return sl4_hash_table_new( _sl4_hash_func_int, _sl4_hash_comp_int,
                             _sl4_hash_key_copy_int, _sl4_hash_key_free_int );
}

LIBSL4_API sl4_hash_table_t *sl4_hash_table_ptr_new(  )
{
  return sl4_hash_table_new( NULL, NULL, NULL, NULL );
}

LIBSL4_API void sl4_hash_table_delete( sl4_hash_table_t *table,
                                       sl4_hash_foreach_t hash_del,
                                       void *arg )
{
  size_t i;
  sl4_hash_bucket_t *buck, *next;
  next = NULL;
  for ( i = 0; i < table->nbucks; i++ ) {
    for ( buck = table->buckets[i]; buck; buck = next ) {
      next = buck->next;
      if ( hash_del )
        hash_del( buck->data, arg );
      if ( table->hash_key_free )
        table->hash_key_free( buck->key );
      free( buck );
    }
  }
  if ( table->buckets )
    free( table->buckets );
  free( table );
}

static size_t _sl4_hash_create_prime( size_t nkeys )
{
  size_t i;
  if ( nkeys < HASH_BUCKETSIZE_MIN )
    return HASH_BUCKETSIZE_MIN;
  for ( i = 1; nkeys > _sl4_hash_primes[i]; i++ )
    if ( _sl4_hash_primes[i] == HASH_BUCKETSIZE_MAX )
      return HASH_BUCKETSIZE_MAX;
  return _sl4_hash_primes[i - 1];
}

static int _sl4_hash_table_resize( sl4_hash_table_t *table )
{
  float ratio;
  size_t i, new_size;
  sl4_hash_bucket_t *buck, *next, **new_bucks;
  ratio = ( float ) table->nkeys / ( float ) table->nbucks;
  if ( ratio > 0.3 && ratio < 3.0 )
    return 0;
  new_size = _sl4_hash_create_prime( table->nkeys );
  if ( new_size == table->nbucks )
    return 0;
  new_bucks =
    ( sl4_hash_bucket_t ** ) malloc( sizeof( sl4_hash_bucket_t * ) *
                                     new_size );
  if ( new_bucks == NULL )
    return -1;
  for ( i = 0; i < new_size; i++ )
    new_bucks[i] = NULL;
  for ( i = 0; i < table->nbucks; i++ )
    for ( buck = table->buckets[i]; buck; buck = next ) {
      next = buck->next;
      buck->next = new_bucks[buck->val % new_size];
      new_bucks[buck->val % new_size] = buck;
    }
  free( table->buckets );
  table->buckets = new_bucks;
  table->nbucks = new_size;
  return 0;
}

LIBSL4_API void *sl4_hash_table_lookup( sl4_hash_table_t *table,
                                        const void *key )
{
  size_t hval;
  sl4_hash_bucket_t *buck;
  hval = ( table->hash_func( key ) % table->nbucks );
  for ( buck = table->buckets[hval]; buck; buck = buck->next )
    if ( table->hash_comp( buck->key, key ) == 0 )
      break;
  if ( buck == NULL )
    return NULL;
  return buck->data;
}

LIBSL4_API void *sl4_hash_table_remove( sl4_hash_table_t *table,
                                        const void *key )
{
  void *data;
  size_t hval;
  sl4_hash_bucket_t *buck, *last;
  hval = ( table->hash_func( key ) % table->nbucks );
  last = NULL;
  for ( buck = table->buckets[hval]; buck; buck = buck->next ) {
    if ( table->hash_comp( buck->key, key ) == 0 )
      break;
    last = buck;
  }
  if ( buck == NULL )
    return NULL;
  if ( last )
    last->next = buck->next;
  else
    table->buckets[hval] = buck->next;
  table->nkeys--;
  data = buck->data;
  if ( table->hash_key_free )
    table->hash_key_free( buck->key );
  free( buck );
  if ( table->dyn_resize )
    _sl4_hash_table_resize( table );
  return data;
}

LIBSL4_API int sl4_hash_table_insert( sl4_hash_table_t *table,
                                      const void *key, void *data )
{
  size_t hval;
  sl4_hash_bucket_t *buck;
  buck = sl4_hash_table_lookup( table, key );
  if ( buck )
    return -1;
  buck = ( sl4_hash_bucket_t * ) malloc( sizeof( sl4_hash_bucket_t ) );
  if ( buck == NULL )
    return -1;
  if ( table->hash_key_copy ) {
    buck->key = table->hash_key_copy( key );
    if ( buck->key == NULL ) {
      free( buck );
      return -1;
    }
  } else {
    buck->key = ( void * ) key;
  }
  buck->data = data;
  buck->val = table->hash_func( key );
  hval = ( buck->val % table->nbucks );
  buck->next = table->buckets[hval];
  table->buckets[hval] = buck;
  table->nkeys++;
  if ( table->dyn_resize )
    _sl4_hash_table_resize( table );
  return 0;
}

LIBSL4_API void sl4_hash_table_foreach( sl4_hash_table_t *table,
                                        sl4_hash_foreach_t hash_foreach,
                                        void *arg )
{
  size_t i;
  sl4_hash_bucket_t *buck;
  if ( table->nkeys != 0 ) {
    for ( i = 0; i < table->nbucks; i++ )
      for ( buck = table->buckets[i]; buck; buck = buck->next )
        hash_foreach( buck->data, arg );
  }
}

LIBSL4_API void sl4_hash_table_erase( sl4_hash_table_t *table,
                                      sl4_hash_foreach_t hash_del, void *arg )
{
  size_t i;
  sl4_hash_bucket_t *buck, *next;
  next = NULL;
  for ( i = 0; i < table->nbucks; i++ ) {
    for ( buck = table->buckets[i]; buck; buck = next ) {
      next = buck->next;
      if ( hash_del )
        hash_del( buck->data, arg );
      if ( table->hash_key_free )
        table->hash_key_free( buck->key );
      free( buck );
    }
    table->buckets[i] = NULL;
  }
  table->nkeys = 0;
  if ( table->dyn_resize )
    _sl4_hash_table_resize( table );
}

/* hash functions - pointer version */
static size_t _sl4_hash_func_ptr( const void *key )
{
  return ( size_t ) key;
}

static int _sl4_hash_comp_ptr( const void *key1, const void *key2 )
{
#ifdef _MSC_VER
/* Truncate 'const void *' to 'int' */
# pragma  warning(disable: 4311)
#endif
  return ( int ) ( key1 - key2 );
#ifdef _MSC_VER
# pragma  warning(default: 4311)
#endif
}

/* hash functions - int version */
static void *_sl4_hash_key_copy_int( const void *key )
{
  int *ret;
  ret = ( int * ) malloc( sizeof( int ) );
  if ( ret )
    *ret = *( int * ) key;
  return ret;
}

static void _sl4_hash_key_free_int( void *key )
{
  free( key );
}

static size_t _sl4_hash_func_int( const void *key )
{
  return ( size_t ) ( *( int * ) key );
}

static int _sl4_hash_comp_int( const void *key1, const void *key2 )
{
  return ( *( int * ) key1 ) - ( *( int * ) key2 );
}

/* hash functions - string version */

static void *_sl4_hash_key_copy_str( const void *key )
{
  char *ret, *k;
  k = ( char * ) key;
  ret = ( char * ) malloc( strlen( k ) + 1 );
  if ( ret )
    strcpy( ret, k );
  return ret;
}

static void _sl4_hash_key_free_str( void *key )
{
  free( key );
}

/*
 * Fowler/Noll/Vo hash
 *
 * The basis of this hash algorithm was token from an idea sent
 *  by email to the the IEEE POSIX P1003.2 mailing list from
 *
 *  Phong Vo (kpv@research.att.com)
 *  Glenn Flowler (gsf@research.att.com),
 *  Landon Curt Noll (chongo@toad.com) 
 *
 * later improved on their algorithm.
 *
 */

static size_t _sl4_hash_func_str( const void *key )
{
  size_t h;
  const unsigned char *k;
  k = ( const unsigned char * ) key;
  for ( h = 0; *k; k++ ) {
    h += 16777619;              /* 2^24 + 403 */
    h ^= *k;
  }
  return h;
}

static int _sl4_hash_comp_str( const void *key1, const void *key2 )
{
  return strcmp( ( const char * ) key1, ( const char * ) key2 );
}
