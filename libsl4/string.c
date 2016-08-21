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

/* $Id: string.c,v 1.2 2008/01/24 14:59:41 orrisroot Exp $ */

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
#include <stdarg.h>

#include "libsl4.h"

/* #define USE_VSPRINTF */

#define STRING_BLOCKSIZE 32
#define STRING_BUFFSIZE  1024

#define TRIM_MODE_RIGHT 0x01
#define TRIM_MODE_LEFT  0x02
#define TRIM_MODE_ALL   0x03

static sl4_string_t *_sl4_string_new_sub(  );
static int _sl4_string_insert_sub( sl4_string_t *dst, size_t pos,
                                   const char *src );
static int _sl4_string_resize( sl4_string_t *str, size_t len );
static int _sl4_string_trim_sub( sl4_string_t *str, int mode );

LIBSL4_API sl4_string_t *sl4_string_new( const char *cstr )
{
  sl4_string_t *ret;
  ret = _sl4_string_new_sub(  );
  if ( ret == NULL )
    return NULL;
  if ( sl4_string_set( ret, cstr ) != 0 ) {
    sl4_string_delete( ret );
    return NULL;
  }
  return ret;
}

LIBSL4_API int sl4_string_delete( sl4_string_t *str )
{
  if ( str->mem != NULL )
    free( str->mem );
  free( str );
  return 0;
}

LIBSL4_API int sl4_string_append( sl4_string_t *dst, const char *src )
{
  return _sl4_string_insert_sub( dst, dst->len, src );
}

LIBSL4_API int sl4_string_append_char( sl4_string_t *dst, char c )
{
  return sl4_string_insert_char( dst, dst->len, c );
}

LIBSL4_API int sl4_string_clear( sl4_string_t *str )
{
  return sl4_string_set( str, NULL );
}

LIBSL4_API int sl4_string_compare( sl4_string_t *str, const char *cstr )
{
  const char *cp = sl4_string_get( str );
  if ( cstr == NULL )
    cstr = "";
  return strcmp( cp, cstr );
}

LIBSL4_API size_t sl4_string_find( sl4_string_t *str, const char *target )
{
  return sl4_string_findn( str, target, 0 );
}

LIBSL4_API size_t sl4_string_findn( sl4_string_t *str, const char *target,
                                    size_t spos )
{
  size_t i;
  const char *t, *p;
  const char *base = sl4_string_get( str );
  size_t len = sl4_string_length( str );
  if ( len == 0 || spos > len || target == NULL || *target == '\0' )
    return ( size_t ) -1;
  for ( i = spos; i < len; i++ ) {
    p = base + i;
    t = target;
    while ( *p == *t ) {
      p++;
      t++;
      if ( *t == '\0' ) {       /* found ! */
        return i;
      }
    }
  }
  return ( size_t ) -1;
}

LIBSL4_API int sl4_string_empty( sl4_string_t *str )
{
  return ( str->len == 0 );
}

LIBSL4_API int sl4_string_erase( sl4_string_t *str, size_t pos, size_t len )
{
  size_t nlen;
  if ( pos > str->len )
    return -1;
  if ( len == 0 || len > str->len - pos )
    len = str->len - pos;
  if ( pos + len > str->len )
    return -1;
  if ( str->len == 0 || len == 0 )
    return 0;                   /* success */
  nlen = str->len - len;
  if ( nlen == 0 )
    return sl4_string_set( str, NULL );
  /* shift */
#ifdef HAVE_MEMMOVE
  memmove( str->mem + pos, str->mem + pos + len, nlen - pos );
#else
# error "memmove() required"
#endif
  /* null terminate */
  str->mem[nlen] = '\0';
  /* set new string length */
  str->len = nlen;
  /* resize memory block */
  _sl4_string_resize( str, nlen );      /* if an error occured then no problem */
  return 0;
}

LIBSL4_API const char *sl4_string_get( sl4_string_t *str )
{
  if ( str->mem )
    return str->mem;
  return "";
}

LIBSL4_API char sl4_string_get_at( sl4_string_t *str, size_t pos )
{
  if ( str->len > pos )
    return str->mem[pos];
  return '\0';
}

LIBSL4_API int sl4_string_insert( sl4_string_t *dst, size_t pos,
                                  const char *src )
{
  return _sl4_string_insert_sub( dst, pos, src );
}

LIBSL4_API int sl4_string_insert_char( sl4_string_t *dst, size_t pos, char c )
{
  char cstr[2];
  cstr[0] = c;
  cstr[1] = '\0';
  return _sl4_string_insert_sub( dst, pos, cstr );
}

LIBSL4_API size_t sl4_string_length( sl4_string_t *str )
{
  return str->len;
}

LIBSL4_API int sl4_string_ltrim( sl4_string_t *str )
{
  return _sl4_string_trim_sub( str, TRIM_MODE_LEFT );
}

LIBSL4_API size_t sl4_string_rfind( sl4_string_t *str, const char *target )
{
  if ( str->len == 0 )
    return ( size_t ) -1;
  return sl4_string_rfindn( str, target, str->len - 1 );
}

LIBSL4_API size_t sl4_string_rfindn( sl4_string_t *str, const char *target,
                                     size_t spos )
{
  size_t i;
  const char *t, *p;
  const char *base = sl4_string_get( str );
  size_t len = str->len;
  if ( len == 0 || spos > len || target == NULL || *target == '\0' )
    return ( size_t ) -1;
  for ( i = spos;; i-- ) {
    p = base + i;
    t = target;
    while ( *p == *t ) {
      p++;
      t++;
      if ( *t == '\0' ) {
        /* found ! */
        return i;
      }
    }
    if ( i == 0 )
      break;
  }
  return ( size_t ) -1;
}

LIBSL4_API int sl4_string_replace( sl4_string_t *str, size_t pos,
                                   size_t len, const char *cstr )
{
  int stat;
  sl4_string_t *tmpstr;
  /* copy string */
  tmpstr = sl4_string_new( str->mem );
  if ( tmpstr == NULL )
    return -1;
  /* replace - erase & insert */
  stat = sl4_string_erase( tmpstr, pos, len );
  if ( stat != 0 ) {
    sl4_string_delete( tmpstr );
    return -1;
  }
  stat = sl4_string_insert( tmpstr, pos, cstr );
  if ( stat != 0 ) {
    sl4_string_delete( tmpstr );
    return -1;
  }
  /* set result */
  stat = sl4_string_set( str, tmpstr->mem );
  sl4_string_delete( tmpstr );
  return stat;
}

LIBSL4_API int sl4_string_rtrim( sl4_string_t *str )
{
  return _sl4_string_trim_sub( str, TRIM_MODE_RIGHT );
}

LIBSL4_API int sl4_string_set( sl4_string_t *dst, const char *src )
{
  size_t len;
  if ( src == NULL )
    src = "";
  len = strlen( src );
  if ( _sl4_string_resize( dst, len ) != 0 )
    return -1;
  if ( len != 0 ) {
    strcpy( dst->mem, src );
    dst->len = len;
  }
  return 0;
}

LIBSL4_API int sl4_string_sprintf( sl4_string_t *str, const char *fmt, ... )
{
#ifdef WIN32
  double r6002;                 /* for C Run-Time Error R6002 */
#endif

#ifndef HAVE_VASPRINTF
  int len;
#endif

  va_list args;
  int stat;
  char *mem;

  va_start( args, fmt );

#ifdef WIN32
  r6002 = 0.0;                  /* for C Run-Time Error R6002 */
#endif

#ifdef HAVE_VASPRINTF
  stat = vasprintf( &mem, fmt, args );
#else
# ifdef HAVE__VSCPRINTF
  len = _vscprintf( fmt, args ) + 1;
# else
  len = STRING_BUFFSIZE;
# endif

  if ( len <= 0 )
    return -1;
  mem = ( char * ) malloc( len );
  if ( mem == NULL )
    return -1;

# ifdef HAVE_VSNPRINTF
  stat = vsnprintf( mem, len, fmt, args );
# else
#  ifdef HAVE_WVSPRINTF
  stat = wvsprintf( mem, fmt, args );
#  else
#   ifdef USE_VSPRINTF
  stat = vsprintf( mem, fmt, args );
#   else
#    error "vsprintf() is unsafe function"
#   endif
#  endif
# endif
#endif

  if ( stat <= 0 ) {
    free( mem );
    return -1;
  }
  stat = sl4_string_set( str, mem );
  free( mem );
  if ( stat != 0 )
    return -1;
  return stat;
}

LIBSL4_API sl4_string_t *sl4_string_substr( sl4_string_t *str, size_t pos,
                                            size_t len )
{
  sl4_string_t *ret;
  if ( pos > str->len )
    return NULL;
  if ( len == 0 ) {
    len = str->len - pos;
  }
  if ( pos + len > str->len )
    return NULL;
  /* allocate new string */
  ret = sl4_string_new( NULL );
  if ( ret == NULL )
    return NULL;
  /* resize memory of new string */
  if ( _sl4_string_resize( ret, len ) != 0 ) {
    sl4_string_delete( ret );
    return NULL;
  }
  if ( len != 0 ) {
    memcpy( ret->mem, str->mem + pos, len );
    ret->mem[len] = '\0';
    ret->len = len;
  }
  return ret;
}

LIBSL4_API sl4_string_t *sl4_string_tokenize( sl4_string_t *str,
                                              const char *sep, size_t *pos )
{
  sl4_string_t *ret;
  size_t i, start, end, next;
  size_t seplen, sublen;
  /* check end of position */
  if ( *pos == str->len ) {
    return NULL;
  }
  seplen = strlen( sep );
  /* seek to start char */
  for ( start = *pos; start < str->len; start++ ) {
    int is_sep = 0;
    for ( i = 0; i < seplen; i++ ) {
      if ( str->mem[start] == sep[i] ) {
        is_sep = 1;
        break;
      }
    }
    if ( is_sep == 0 )
      break;
  }
  /* seek to next separator */
  end = str->len;
  for ( i = 0; i < seplen; i++ ) {
    size_t j;
    for ( j = start; j < str->len; j++ )
      if ( str->mem[j] == sep[i] )
        break;
    if ( j < end )
      end = j;
  }
  sublen = end - start;
  ret = sl4_string_substr( str, start, sublen );
  /* seek to next not separator */
  for ( next = end; next < str->len; next++ ) {
    int is_sep = 0;
    for ( i = 0; i < seplen; i++ ) {
      if ( str->mem[next] == sep[i] ) {
        is_sep = 1;
        break;
      }
    }
    if ( is_sep == 0 )
      break;
  }
  *pos = next;
  return ret;
}

LIBSL4_API int sl4_string_trim( sl4_string_t *str )
{
  return _sl4_string_trim_sub( str, TRIM_MODE_ALL );
}

/* private functions */
static sl4_string_t *_sl4_string_new_sub(  )
{
  sl4_string_t *str;
  str = ( sl4_string_t * ) malloc( sizeof( sl4_string_t ) );
  if ( str == NULL )
    return NULL;
  str->bsize = 0;
  str->len = 0;
  str->mem = NULL;
  return str;
}

static int _sl4_string_insert_sub( sl4_string_t *dst, size_t pos,
                                   const char *src )
{
  size_t alen, nlen, shiftlen;
  if ( dst->len < pos )
    return -1;
  if ( src == NULL )
    return 0;
  alen = strlen( src );
  /* if src string length is zero then return as success */
  if ( alen == 0 )
    return 0;
  nlen = dst->len + alen;
  if ( _sl4_string_resize( dst, nlen ) != 0 )
    return -1;
  /* shift memory */
  shiftlen = dst->len - pos;
#ifdef HAVE_MEMMOVE
  if ( shiftlen != 0 )
    memmove( dst->mem + pos + alen, dst->mem + pos, shiftlen );
#else
# error "memmove() required"
#endif
  /* insert memory */
  memcpy( dst->mem + pos, src, alen );
  /* null terminate */
  dst->mem[nlen] = '\0';
  /* set new string length */
  dst->len = nlen;
  return 0;
}

static int _sl4_string_resize( sl4_string_t *str, size_t len )
{
  size_t nlen, nbsi, olen, obsi;
  char *nmem, *omem;
  omem = str->mem;
  olen = str->len;
  obsi = str->bsize;
  nbsi = ( len == 0 ) ? 0 : len / STRING_BLOCKSIZE + 1;
  /* if new block size is same size then return as success */
  if ( nbsi == obsi )
    return 0;
  /* allocate new memory */
  if ( nbsi == 0 ) {
    nmem = NULL;
  } else {
    nmem = ( char * ) malloc( sizeof( char ) * STRING_BLOCKSIZE * nbsi );
    if ( nmem == NULL )
      return -1;
  }
  nlen = ( len < olen ) ? len : olen;
  /* copy memory */
  if ( nlen != 0 ) {
    memcpy( nmem, omem, nlen );
    /* null terminate new memory */
    nmem[nlen] = '\0';
  }
  /* assgin new variables to sl4_string_t */
  str->mem = nmem;
  str->len = nlen;
  str->bsize = nbsi;
  /* free old memory */
  if ( omem != NULL )
    free( omem );
  /* success */
  return 0;
}

static int _sl4_string_trim_sub( sl4_string_t *str, int mode )
{
  static const char *trimchar = " \t\n\r\0\x0b";
  static int trimchar_num = 6;
  static int first = 1;
  static char mask[256];
  size_t i, len;
  int j;
  size_t alltrim, ltrim = 0, rtrim = 0;
  if ( str->mem == NULL )
    return 0;
  len = str->len;

  /* set trimming character map */
  if ( first ) {
#ifdef HAVE_MEMSET
    memset( mask, 0, 256 );
#else
# error "memset() required"
#endif
    for ( j = 0; j < trimchar_num; j++ ) {
      mask[( ( unsigned char * ) trimchar )[j]] = 1;
    }
    first = 0;
  }
  if ( mode & TRIM_MODE_LEFT ) {        /* trim left of string */
    for ( i = 0; i < len; i++ ) {
      if ( mask[( ( unsigned char ) str->mem[i] )] )
        ltrim++;
      else
        break;
    }
  }
  if ( mode & TRIM_MODE_RIGHT ) {       /* trim right of string */
    for ( i = len - 1; i >= 0; i-- ) {
      if ( mask[( ( unsigned char ) str->mem[i] )] )
        rtrim++;
      else
        break;
    }
  }
  alltrim = ltrim + rtrim;
  if ( alltrim > len ) {
    len = 0;
  } else {
    len -= alltrim;
  }
  if ( len == 0 )
    return _sl4_string_resize( str, len );
  for ( i = 0; i < len; i++ ) {
    str->mem[i] = str->mem[ltrim + i];
  }
  str->mem[len] = '\0';
  str->len = len;
  /* success */
  return 0;
}
