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

#ifdef HAVE_CONFIG
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "libsl4.h"
#include "cexcel.h"
#include "excel2xoonips.h"

#ifdef WIN32
# define mkdir( x, y ) _mkdir( x )
#endif

#ifndef WIN32
# ifdef  MAXPATHLEN
#  define _MAX_PATH MAXPATHLEN
# else
#  error MAXPATHLEN not defined
# endif
#endif

#define PREFIX  "excel2xoonips"
#define BUFSIZE   8192

static sl4_string_t *_tempdir_getpath( sl4_string_t *tempdir,
                                       const char *vpath );

sl4_string_t *tempdir_new(  )
{
  const char *temp;
  char path[_MAX_PATH];
  sl4_string_t *ret;

  temp = sl4_getenv( "TEMP" );
  if ( temp == NULL ) {
    temp = sl4_getenv( "TMP" );
  }
  if ( temp == NULL ) {
    temp = "/tmp";
  }
#ifdef WIN32
  /* expand path name */
  if ( sl4_win32_path_getfull( temp, path, _MAX_PATH ) != 0 )
    ERROR_UNEXPECTED(  );
#else
  if ( strlen( temp ) >= _MAX_PATH )
    ERROR_UNEXPECTED(  );
  strcpy( path, temp );         /* safe */
#endif
  ret = sl4_string_new( NULL );
  if ( ret == NULL ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_string_sprintf( ret, "%s/%s-XXXXXX", path, PREFIX ) != 0 ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_string_length( ret ) >= _MAX_PATH ) {
    ERROR_UNEXPECTED(  );
  }
  strcpy( path, sl4_string_get( ret ) );        /* safe */
  if ( bsd_mkdtemp( path ) == NULL ) {
    ERROR_UNEXPECTED(  );
  }
  if ( sl4_string_set( ret, path ) != 0 ) {
    ERROR_OUTMEM(  );
  }
  return ret;
}

int tempdir_delete( sl4_string_t *tempdir )
{
  int ret;
  ret = directory_remove( sl4_string_get( tempdir ) );
  sl4_string_delete( tempdir );
  return ret;
}

int tempdir_mkdir( sl4_string_t *tempdir, const char *dir )
{
  sl4_string_t *path;
  path = _tempdir_getpath( tempdir, dir );
  if ( mkdir( sl4_string_get( path ), 0755 ) != 0 ) {
    ERROR_UNEXPECTED(  );
  }
  sl4_string_delete( path );
  return 0;
}

sl4_file_t *tempdir_fopen( sl4_string_t *tempdir, const char *vpath,
                           const char *mode )
{
  sl4_string_t *path;
  sl4_file_t *file;
  path = _tempdir_getpath( tempdir, vpath );
  file = sl4_file_new( sl4_string_get( path ) );
  if ( file == NULL ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_file_open( file, mode ) != 0 ) {
    ERROR_UNEXPECTED(  );
  }
  return file;
}

int tempdir_fclose( sl4_file_t *file )
{
  int ret;
  ret = sl4_file_close( file );
  sl4_file_delete( file );
  return ret;
}

int tempdir_copyfile( sl4_string_t *tempdir, const char *spath,
                      const char *dvpath )
{
  sl4_file_t *src, *dst;
  char buf[BUFSIZE];
  size_t len;
  src = sl4_file_new( spath );
  if ( src == NULL ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_file_open( src, "r" ) != 0 ) {
    ERROR_UNEXPECTED(  );
  }
  dst = tempdir_fopen( tempdir, dvpath, "w" );
  while ( sl4_file_eof( src ) == 0 ) {
    len = sl4_file_read( src, buf, BUFSIZE );
    if ( len > 0 ) {
      sl4_file_write( dst, buf, len );
    }
  }
  tempdir_fclose( dst );
  sl4_file_close( src );
  sl4_file_delete( src );
  return 0;
}

static sl4_string_t *_tempdir_getpath( sl4_string_t *tempdir,
                                       const char *vpath )
{
  sl4_string_t *path;
  path = sl4_string_new( sl4_string_get( tempdir ) );
  if ( path == NULL ) {
    ERROR_OUTMEM(  );
  }
  if ( vpath[0] != '/' ) {
    if ( sl4_string_append( path, "/" ) ) {
      ERROR_OUTMEM(  );
    }
  }
  if ( sl4_string_append( path, vpath ) ) {
    ERROR_OUTMEM(  );
  }
  return path;
}
