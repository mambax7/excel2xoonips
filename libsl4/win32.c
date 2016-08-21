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

/* $Id: win32.c,v 1.2 2008/01/24 14:59:41 orrisroot Exp $ */

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
# include <shlobj.h>            /* shell32.lib */
#endif

#include <stdio.h>
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
# include <string.h>
#endif

#include "libsl4.h"

/* Windows platform specific functions */

#ifdef WIN32
static int _GetSpecialFolder( char *str, DWORD mask );

LIBSL4_API int sl4_win32_has_console(  )
{
  HANDLE h;
  h = CreateFileA( "CONOUT$", GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
  if ( h != INVALID_HANDLE_VALUE ) {
    CloseHandle( h );
    return 1;
  }
  return 0;
}

LIBSL4_API int sl4_win32_path_getfull( const char *src, char *dst,
                                       size_t len )
{
  char temp[_MAX_PATH];
  LPTSTR lpFilePart;
  if ( GetLongPathName( src, temp, _MAX_PATH ) == 0 )
    return -1;
  if ( GetFullPathName( temp, ( DWORD ) len, dst, &lpFilePart ) == 0 )
    return -1;
  return 0;
}

LIBSL4_API const char *sl4_win32_path_mydocuments(  )
{
  static int init = 0;
  static char var[_MAX_PATH];
  static const char *errmes = "failed to get \"My Documents\" folder";
  if ( init == 0 ) {
    if ( _GetSpecialFolder( var, CSIDL_PERSONAL ) != 0 )
      sl4_error_fatal( errmes, __FILE__, __LINE__ );
    init = 1;
  }
  return var;
}

LIBSL4_API const char *sl4_win32_path_applicationdata(  )
{
  static int init = 0;
  static char var[_MAX_PATH];
  static const char *errmes = "failed to get \"Application Data\" folder";
  if ( init == 0 ) {
    if ( _GetSpecialFolder( var, CSIDL_APPDATA ) != 0 )
      sl4_error_fatal( errmes, __FILE__, __LINE__ );
    init = 1;
  }
  return var;
}

/* private function */
static int _GetSpecialFolder( char *str, DWORD mask )
{
  ITEMIDLIST *pIdList;
  IMalloc *pMalloc;
  int ret = -1;
  if ( NOERROR == SHGetMalloc( &pMalloc ) ) {
    if ( NOERROR ==
         SHGetSpecialFolderLocation( HWND_DESKTOP, mask, &pIdList ) ) {
      SHGetPathFromIDList( pIdList, str );
      pMalloc->lpVtbl->Free( pMalloc, pIdList );
      ret = 0;
    }
  }
  return ret;
}

#endif
