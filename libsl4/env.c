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

/* $Id: env.c,v 1.1.1.1 2007/10/31 04:26:32 orrisroot Exp $ */

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

LIBSL4_API char *sl4_getenv( const char *env )
{
  return getenv( env );
}

LIBSL4_API int sl4_unsetenv( const char *env )
{
  if ( env == NULL || *env == '\0' )
    return -1;
#ifdef HAVE_STRSTR
  if ( strstr( env, "=" ) != NULL )
    return -1;
#else
# error "strstr() required"
#endif
#ifdef WIN32
  {
    int len = strlen( env ) + 2;
    int ret;
    /* 2 means '=' and '\0' chaacters. */
    char *mem = ( char * ) malloc( len );
    sprintf( mem, "%s=", env ); /* safe */
    ret = _putenv( mem );
    free( mem );
    return ret;
  }
#else
# ifdef HAVE_SETENV
  unsetenv( env );
# else
  /* 
     In POSIX platform, if the argument of putenv() function does not
     contain an '=' character, then the variable name is removed from
     the environment. 
   */
#  ifdef HAVE_PUTENV
  putenv( env );
#  else
#   error "putenv() required"
#  endif
# endif
#endif
  return 0;
}

LIBSL4_API int sl4_setenv( const char *env, const char *val )
{
  int ret = 0;
  if ( env == NULL || *env == '\0' )
    return -1;
#ifdef HAVE_STRSTR
  if ( strstr( env, "=" ) != NULL )
    return -1;
#else
# error "strstr() required"
#endif
  if ( val == NULL )
    val = "";
#ifdef WIN32
  {
    size_t len = strlen( env ) + strlen( val ) + 2;
    /* 2 means '=' and '\0' chaacters. */
    char *mem = ( char * ) malloc( len );
    sprintf( mem, "%s=%s", env, val );  /* safe */
    ret = _putenv( mem );
    free( mem );
  }
#else
# ifdef HAVE_SETENV
  ret = setenv( env, val, 1 );  /* overwrite */
# else
  {
    size_t len = strlen( env ) + strlen( val ) + 2;
    /* 2 means '=' and '\0' chaacters. */
    char *mem = ( char * ) malloc( len );
    sprintf( mem, "%s=%s", env, val );  /* safe */
    /*
       warning :
       maybe, if putenv() specification is 4.3BSD like then it dose 
       not work.. because, the environment variable setted by putenv()
       is not influence for child process.
     */
    ret = putenv( mem );
    /* the memory of the argument in putenv() function should not be free. */
  }
# endif
#endif
  return ret;
}
