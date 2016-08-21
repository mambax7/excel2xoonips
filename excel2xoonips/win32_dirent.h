/* 
 * Copyright (c) 2003 RIKEN (The Institute of Physical and Chemical Research)
 * All rights reserved.
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

/* $Id: win32_dirent.h,v 1.1.1.1 2007/10/31 04:26:33 orrisroot Exp $ */

#ifdef WIN32

#ifndef _DIRENT_H
#define _DIRENT_H

#ifdef DECLSPEC
# undef DECLSPEC
#endif
#ifdef DLLEXPORT
# undef DLLEXPORT
#endif

#define DECLSPEC
#define DLLEXPORT

#include <stdlib.h>
#include <windows.h>
#include <limits.h>
#include <sys/types.h>

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#define __DIRENT_COOKIE 0xfefeabab

#ifdef __cplusplus
extern "C" {
#endif

  struct dirent {
    ino_t d_ino;                /* unused - no equivalent on WIN32 */
    char d_name[NAME_MAX + 1];
  };

  DECLSPEC typedef struct dir_struct {
    ULONG dir_ulCookie;
    HANDLE dir_hDirHandle;
    DWORD dir_nNumFiles;
    char dir_pDirectoryName[NAME_MAX + 1];
    struct dirent dir_sdReturn;
  } DIR;

  DECLSPEC DIR *opendir( const char * );
  DECLSPEC struct dirent *readdir( DIR * );
  DECLSPEC void rewinddir( DIR * );
  DECLSPEC void closedir( DIR * );
  DECLSPEC int telldir( DIR * );
  DECLSPEC void seekdir( DIR *, long );

#ifdef __cplusplus
}
#endif
#endif
#endif                          /* WIN32 */
