/*
 * excel2xoonips - A data convertor from Excel data to XooNIps import file
 *
 * Copyright (C) 2007  RIKEN Japan, All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifndef WIN32
# include <unistd.h>
# include <utime.h>
# include <sys/types.h>
# include <sys/stat.h>
#else
# include <direct.h>
# include <io.h>
#endif

#include "minizip/zip.h"

#ifdef WIN32
#include <windows.h>
/*
#define USEWIN32IOAPI
#include "iowin32.h"
*/
#endif

#include "libsl4.h"
#include "cexcel.h"
#include "excel2xoonips.h"

#define WRITEBUFFERSIZE 16384

static uLong filetime( const char *f, tm_zip * tmzip, uLong * dt );

int minizip_create( const char *filename, const char *rootdir )
{
  int err;
  zipFile zf;
  FILE *fin;
  zip_fileinfo zi;
  int size_buf, size_read;
  void *buf = NULL;
  sl4_list_t *filelist;

  /* get file list */
  filelist = directory_filelist( rootdir );
  if ( sl4_list_empty( filelist ) ) {
    /* no files found */
    sl4_list_delete( filelist );
    return -2;
  }

  /* allocate memory */
  size_buf = WRITEBUFFERSIZE;
  buf = ( void * ) malloc( size_buf );
  if ( buf == NULL ) {
    ERROR_OUTMEM(  );
  }

  /* open zip file */
  zf = zipOpen( filename, APPEND_STATUS_CREATE );
  if ( zf == NULL ) {
    /* failed to open zip */
    free( buf );
    return -1;
  }
  err = 0;
  while ( sl4_list_empty( filelist ) == 0 ) {
    sl4_string_t *realpath_inzip, *virtualpath_inzip;
    virtualpath_inzip = ( sl4_string_t * ) sl4_list_top( filelist );
    sl4_list_pop_front( filelist );
    realpath_inzip = sl4_string_new( rootdir );
    if ( realpath_inzip == NULL ) {
      ERROR_OUTMEM(  );
    }
    sl4_string_append( realpath_inzip, "/" );
    sl4_string_append( realpath_inzip, sl4_string_get( virtualpath_inzip ) );

    zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = 0;
    zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = 0;
    filetime( sl4_string_get( realpath_inzip ), &zi.tmz_date, &zi.dosDate );

    /* open file in zip */
    err =
      zipOpenNewFileInZip3( zf, sl4_string_get( virtualpath_inzip ), &zi,
                            NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9, 0,
                            -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                            NULL, 0 );
    if ( err != ZIP_OK ) {
      ERROR_UNEXPECTED(  );
    }

    /* open physical file */
    fin = fopen( sl4_string_get( realpath_inzip ), "rb" );
    if ( fin == NULL ) {
      /* failed to open file */
      err = ZIP_ERRNO;
      zipCloseFileInZip( zf );
      ERROR_UNEXPECTED(  );
      break;
    }

    do {
      err = ZIP_OK;
      size_read = ( int ) fread( buf, 1, size_buf, fin );
      if ( size_read < size_buf ) {
        if ( feof( fin ) == 0 ) {
          err = ZIP_ERRNO;
        }
      }
      if ( size_read > 0 ) {
        err = zipWriteInFileInZip( zf, buf, size_read );
      }
    } while ( ( err == ZIP_OK ) && ( size_read > 0 ) );

    /* close physical file */
    fclose( fin );

    /* close file in zip */
    err = zipCloseFileInZip( zf );
    if ( err != ZIP_OK ) {
      ERROR_UNEXPECTED(  );
    }

    sl4_string_delete( realpath_inzip );
    sl4_string_delete( virtualpath_inzip );
  }

  free( buf );
  /* close zip file */
  err = zipClose( zf, NULL );

  if ( err != ZIP_OK ) {
    ERROR_UNEXPECTED(  );
  }

  sl4_list_delete( filelist );
  return 0;
}

static uLong filetime( const char *f, tm_zip * tmzip, uLong * dt )
{
#ifdef WIN32
  int ret = 0;
  FILETIME ftLocal;
  HANDLE hFind;
  WIN32_FIND_DATA ff32;

  hFind = FindFirstFile( f, &ff32 );
  if ( hFind != INVALID_HANDLE_VALUE ) {
    FileTimeToLocalFileTime( &( ff32.ftLastWriteTime ), &ftLocal );
    FileTimeToDosDateTime( &ftLocal, ( ( LPWORD ) dt ) + 1,
                           ( ( LPWORD ) dt ) + 0 );
    FindClose( hFind );
    ret = 1;
  }
  return ret;
#else
  int ret = 0;
  struct stat s;                /* results of stat() */
  struct tm *filedate;
  time_t tm_t = 0;

  if ( stat( f, &s ) == 0 ) {
    tm_t = s.st_mtime;
    ret = 1;
  }
  filedate = gmtime( &tm_t );

  tmzip->tm_sec = filedate->tm_sec;
  tmzip->tm_min = filedate->tm_min;
  tmzip->tm_hour = filedate->tm_hour;
  tmzip->tm_mday = filedate->tm_mday;
  tmzip->tm_mon = filedate->tm_mon;
  tmzip->tm_year = filedate->tm_year;

  return ret;
#endif
}
