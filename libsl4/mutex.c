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

/* $Id: mutex.c,v 1.1.1.1 2007/10/31 04:26:32 orrisroot Exp $ */

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

#ifdef HAVE_PTHREAD_H
# include <pthread.h>
#else
# ifndef WIN32
#  error "pthread.h header required"
# endif
#endif

#include "libsl4.h"

LIBSL4_API sl4_mutex_t *sl4_mutex_new(  )
{
  sl4_mutex_t *mutex;
  int status = 0;
  mutex = ( sl4_mutex_t * ) malloc( sizeof( sl4_mutex_t ) );
  if ( mutex == NULL )
    return NULL;
  mutex->mtx = NULL;

#ifdef WIN32
  mutex->mtx = ( void * ) CreateMutex( NULL, FALSE, NULL );
  if ( mutex->mtx == NULL )
    status = -1;
#else
  mutex->mtx = malloc( sizeof( pthread_mutex_t ) );
  if ( mutex->mtx == NULL ) {
    free( mutex );
    return NULL;
  }
  status = pthread_mutex_init( ( pthread_mutex_t * ) mutex->mtx, NULL );
#endif

  if ( status != 0 ) {
    free( mutex );
    return NULL;
  }
  return mutex;
}

LIBSL4_API void sl4_mutex_delete( sl4_mutex_t *mutex )
{
  if ( mutex == NULL )
    return;
  if ( mutex->mtx != NULL ) {
#ifdef WIN32
    CloseHandle( ( HANDLE ) mutex->mtx );
#else
    pthread_mutex_destroy( ( pthread_mutex_t * ) mutex->mtx );
    free( mutex->mtx );
#endif
  }
  free( mutex );
}

LIBSL4_API void sl4_mutex_lock( sl4_mutex_t *mutex )
{
  if ( mutex == NULL )
    return;
  if ( mutex->mtx != NULL ) {
#ifdef WIN32
    WaitForSingleObject( ( HANDLE ) mutex->mtx, INFINITE );
#else
    pthread_mutex_lock( ( pthread_mutex_t * ) mutex->mtx );
#endif
  }
}

LIBSL4_API void sl4_mutex_unlock( sl4_mutex_t *mutex )
{
  if ( mutex == NULL )
    return;
  if ( mutex->mtx != NULL ) {
#ifdef WIN32
    ReleaseMutex( ( HANDLE ) mutex->mtx );
#else
    pthread_mutex_unlock( ( pthread_mutex_t * ) mutex->mtx );
#endif
  }
}
