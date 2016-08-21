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

/* $Id: win32_dirent.c,v 1.2 2008/01/24 15:04:27 orrisroot Exp $ */
#ifdef WIN32

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "win32_dirent.h"

#ifdef __cplusplus
extern "C" {
#endif

  DLLEXPORT DIR *opendir( const char *pDirName ) {
    struct stat sb;
    DIR *pDir;
    char *pEndDirName;
    size_t nBufferLen;

    /* sanity checks */
    if ( !pDirName ) {
      errno = EINVAL;
      return NULL;
    }
    if ( stat( pDirName, &sb ) != 0 ) {
      errno = ENOENT;
      return NULL;
    }
    if ( ( sb.st_mode & S_IFMT ) != S_IFDIR ) {
      errno = ENOTDIR;
      return NULL;
    }

    /* allocate a DIR structure to return */
    pDir = ( DIR * ) malloc( sizeof( DIR ) );

    if ( !pDir )
      return NULL;

    /* input directory name length */
    nBufferLen = strlen( pDirName );

    /* copy input directory name to DIR buffer */
    strcpy( pDir->dir_pDirectoryName, pDirName );

    /* point to end of the copied directory name */
    pEndDirName = &pDir->dir_pDirectoryName[nBufferLen - 1];

    /* if directory name did not end in '/' or '\', add '/' */
    if ( ( *pEndDirName != '/' ) && ( *pEndDirName != '\\' ) ) {
      pEndDirName++;
      *pEndDirName = '/';
    }

    /* now append the wildcard character to the buffer */
    pEndDirName++;
    *pEndDirName = '*';
    pEndDirName++;
    *pEndDirName = '\0';

    /* other values defaulted */
    pDir->dir_nNumFiles = 0;
    pDir->dir_hDirHandle = INVALID_HANDLE_VALUE;
    pDir->dir_ulCookie = __DIRENT_COOKIE;

    return pDir;
  }

  DLLEXPORT void closedir( DIR * pDir ) {
    /* got a valid pointer? */
    if ( !pDir ) {
      errno = EINVAL;
      return;
    }

    /* sanity check that this is a DIR pointer */
    if ( pDir->dir_ulCookie != __DIRENT_COOKIE ) {
      errno = EINVAL;
      return;
    }

    /* close the WIN32 directory handle */
    if ( pDir->dir_hDirHandle != INVALID_HANDLE_VALUE )
      FindClose( pDir->dir_hDirHandle );

    free( pDir );

    return;
  }

  DLLEXPORT struct dirent *readdir( DIR * pDir ) {
    WIN32_FIND_DATA wfdFindData;

    if ( !pDir ) {
      errno = EINVAL;
      return NULL;
    }

    /* sanity check that this is a DIR pointer */
    if ( pDir->dir_ulCookie != __DIRENT_COOKIE ) {
      errno = EINVAL;
      return NULL;
    }

    if ( pDir->dir_nNumFiles == 0 ) {
      pDir->dir_hDirHandle = FindFirstFile( pDir->dir_pDirectoryName,
                                            &wfdFindData );
      if ( pDir->dir_hDirHandle == INVALID_HANDLE_VALUE )
        return NULL;
    } else if ( !FindNextFile( pDir->dir_hDirHandle, &wfdFindData ) )
      return NULL;

    /* bump count for next call to readdir() or telldir() */
    pDir->dir_nNumFiles++;

    /* fill in struct dirent values */
    pDir->dir_sdReturn.d_ino = -1;
    strcpy( pDir->dir_sdReturn.d_name, wfdFindData.cFileName );

    return &pDir->dir_sdReturn;
  }

  DLLEXPORT void rewinddir( DIR * pDir ) {
    if ( !pDir ) {
      errno = EINVAL;
      return;
    }

    /* sanity check that this is a DIR pointer */
    if ( pDir->dir_ulCookie != __DIRENT_COOKIE ) {
      errno = EINVAL;
      return;
    }

    /* close the WIN32 directory handle */
    if ( pDir->dir_hDirHandle != INVALID_HANDLE_VALUE )
      if ( !FindClose( pDir->dir_hDirHandle ) )
        errno = EBADF;

    /* reset members which control readdir() */
    pDir->dir_hDirHandle = INVALID_HANDLE_VALUE;
    pDir->dir_nNumFiles = 0;

    return;
  }

  DLLEXPORT int telldir( DIR * pDir ) {
    if ( !pDir ) {
      errno = EINVAL;
      return -1;
    }

    /* sanity check that this is a DIR pointer */
    if ( pDir->dir_ulCookie != __DIRENT_COOKIE ) {
      errno = EINVAL;
      return -1;
    }

    /* return number of times readdir() called */
    return pDir->dir_nNumFiles;
  }

  DLLEXPORT void seekdir( DIR * pDir, long nPosition ) {
    if ( !pDir )
      return;

    /* sanity check that this is a DIR pointer */
    if ( pDir->dir_ulCookie != __DIRENT_COOKIE )
      return;

    /* go back to beginning of directory */
    rewinddir( pDir );

    /* loop until we have found position we care about */
    for ( --nPosition; nPosition && readdir( pDir ); nPosition-- );

    /* flag invalid nPosition value */
    if ( nPosition )
      errno = EINVAL;

    return;
  }
#ifdef __cplusplus
}
#endif

#endif /* WIN32 */
