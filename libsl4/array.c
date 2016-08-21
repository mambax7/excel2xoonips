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

/* $Id: array.c,v 1.1.1.1 2007/10/31 04:26:32 orrisroot Exp $ */

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

LIBSL4_API sl4_array_t *sl4_array_new( size_t bsize )
{
  sl4_array_t *arr;
  arr = ( sl4_array_t * ) malloc( sizeof( sl4_array_t ) );
  if ( arr == NULL )
    return NULL;
  arr->bsize = bsize;
  arr->data = NULL;
  arr->len = 0;
  return arr;
}

LIBSL4_API void sl4_array_delete( sl4_array_t *arr )
{
  if ( arr->len != 0 )
    free( arr->data );
  free( arr );
}

LIBSL4_API int sl4_array_add( sl4_array_t *arr, void *val )
{
  unsigned char *nmem, *omem;
  size_t nlen;
  nlen = arr->len + 1;
  nmem = ( unsigned char * ) malloc( arr->bsize * nlen );
  if ( nmem == NULL )
    return -1;
  omem = ( unsigned char * ) arr->data;
  if ( arr->len != 0 )
    memcpy( nmem, omem, arr->len * arr->bsize );
  memcpy( nmem + ( arr->len * arr->bsize ), val, arr->bsize );
  if ( arr->len != 0 )
    free( arr->data );
  arr->data = nmem;
  arr->len = nlen;
  return 0;
}

LIBSL4_API int sl4_array_clear( sl4_array_t *arr )
{
  if ( arr->len != 0 )
    free( arr->data );
  arr->data = NULL;
  arr->len = 0;
  return 0;
}

LIBSL4_API int sl4_array_empty( sl4_array_t *arr )
{
  return ( arr->len == 0 ) ? 1 : 0;
}

LIBSL4_API int sl4_array_erase( sl4_array_t *arr, size_t pos )
{
  unsigned char *nmem, *omem;
  size_t nlen;
  if ( arr->len <= pos )
    return -1;
  nlen = arr->len - 1;
  if ( nlen == 0 )
    return sl4_array_clear( arr );
  nmem = ( unsigned char * ) malloc( arr->bsize * nlen );
  if ( nmem == NULL )
    return -1;
  omem = ( unsigned char * ) arr->data;
  if ( pos != 0 )
    memcpy( nmem, arr->data, pos * arr->bsize );
  if ( pos != nlen )
    memcpy( nmem + pos * arr->bsize, omem + ( pos + 1 ) * arr->bsize,
            ( nlen - pos ) * arr->bsize );
  if ( arr->len != 0 )
    free( arr->data );
  arr->data = nmem;
  arr->len = nlen;
  return 0;
}

LIBSL4_API void *sl4_array_get( sl4_array_t *arr, size_t pos )
{
  unsigned char *mem;
  if ( arr->len <= pos )
    return NULL;
  mem = ( unsigned char * ) arr->data;
  return mem + pos * arr->bsize;
}

LIBSL4_API size_t sl4_array_length( sl4_array_t *arr )
{
  return arr->len;
}

LIBSL4_API int sl4_array_resize( sl4_array_t *arr, size_t len )
{
  unsigned char *nmem, *omem;
  size_t copy_len, clear_len;
  if ( len == 0 )
    return sl4_array_clear( arr );
  nmem = ( unsigned char * ) malloc( arr->bsize * len );
  if ( nmem == NULL )
    return -1;
  omem = ( unsigned char * ) arr->data;
  if ( arr->len == 0 ) {
    copy_len = 0;
  } else {
    copy_len = ( len > arr->len ) ? arr->len : len;
  }
  clear_len = len - copy_len;

  if ( copy_len != 0 )
    memcpy( nmem, arr->data, copy_len * arr->bsize );
  if ( clear_len != 0 ) {
#ifdef HAVE_MEMSET
    memset( nmem + copy_len * arr->bsize, 0, clear_len * arr->bsize );
#else
# error "memset() required"
#endif
  }

  if ( arr->len != 0 )
    free( arr->data );
  arr->data = nmem;
  arr->len = len;
  return 0;
}

LIBSL4_API int sl4_array_set( sl4_array_t *arr, size_t pos, void *val )
{
  unsigned char *mem;
  if ( arr->len <= pos )
    return -1;
  mem = ( unsigned char * ) arr->data;
  memcpy( mem + pos * arr->bsize, val, arr->bsize );
  return 0;
}
