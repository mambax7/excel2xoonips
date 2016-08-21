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

/* $Id: time.c,v 1.1.1.1 2007/10/31 04:26:32 orrisroot Exp $ */

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

#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#include "libsl4.h"

/* get local date */
LIBSL4_API int sl4_time_getdate( int *year, int *month, int *day )
{
  struct tm *times;
  time_t gtime;
  time( &gtime );
  times = localtime( &gtime );
  if ( year != NULL )
    *year = times->tm_year;
  if ( month != NULL )
    *month = times->tm_mon + 1;
  if ( day != NULL )
    *day = times->tm_mday;
  return 0;
}

/* get local time*/
LIBSL4_API int sl4_time_gettime( int *hour, int *minute, int *second )
{
  struct tm *times;
  time_t gtime;
  time( &gtime );
  times = localtime( &gtime );
  if ( hour != NULL )
    *hour = times->tm_hour;
  if ( minute != NULL )
    *minute = times->tm_min;
  if ( second != NULL )
    *second = times->tm_sec;
  return 0;
}
