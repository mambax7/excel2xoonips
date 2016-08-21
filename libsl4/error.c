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

/* $Id: error.c,v 1.1.1.1 2007/10/31 04:26:32 orrisroot Exp $ */

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

#define  MAXERRORMESSAGE  1024

LIBSL4_API void sl4_error_fatal( const char *mes, const char *fname,
                                 int line )
{
  char str[MAXERRORMESSAGE];
#ifdef HAVE_SNPRINTF
  snprintf( str, MAXERRORMESSAGE, "Fatal Error : %s in \"%s\" on line %d",
            mes, fname, line );
#else
# error "snprintf() required"
#endif
  str[MAXERRORMESSAGE - 1] = '\0';
#ifdef WIN32
  if ( sl4_win32_has_console(  ) ) {
    fprintf( stderr, "%s\n", str );
  } else {
    MessageBoxA( NULL, str, "Fatal Error",
                 MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND |
                 MB_TOPMOST );
  }
#else
  fprintf( stderr, "%s\n", str );
#endif
  exit( 1 );
}
