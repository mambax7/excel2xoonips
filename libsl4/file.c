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

/* $Id: file.c,v 1.2 2008/01/24 14:59:41 orrisroot Exp $ */

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
#include <ctype.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_DIRECT_H
# include <direct.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#include "libsl4.h"

#ifndef _MAX_PATH
# ifdef  MAXPATHLEN
#  define _MAX_PATH  MAXPATHLEN
# else
#  error definition of 'MAXPATHLEN' required
# endif
#endif

#ifdef WIN32
# define FILESEP '\\'
# define DRIVE_LETTER ':'
#else
# define FILESEP '/'
#endif

#define SL4_FILE_NOT_EXIST 0x80
#define SL4_FILE_ATTRIB_FILE 0x01
#define SL4_FILE_ATTRIB_DIRECTORY 0x02
#define SL4_FILE_ATTRIB_READABLE 0x04
#define SL4_FILE_ATTRIB_WRITABLE 0x08
#define SL4_FILE_ATTRIB_EXECUTABLE 0x10
#define SL4_FILE_ATTRIB_LINK 0x20
#define SL4_FILE_ATTRIB_SOCKET 0x40

#define SL4_FILE_EXIST( x ) \
  ( ( _sl4_file_check_attribute( x ) !=  SL4_FILE_NOT_EXIST ) ? 1 : 0 )
#define SL4_FILE_IS_FILE( x ) \
  ( ( _sl4_file_check_attribute( x ) & SL4_FILE_ATTRIB_FILE ) ? 1 : 0 )
#define SL4_FILE_IS_DIRECTORY( x ) \
  ( ( _sl4_file_check_attribute( x ) & SL4_FILE_ATTRIB_DIRECTORY ) ? 1 : 0 )
#define SL4_FILE_IS_READABLE( x ) \
  ( ( _sl4_file_check_attribute( x ) & SL4_FILE_ATTRIB_READABLE ) ? 1 : 0 )
#define SL4_FILE_IS_WRITABLE( x ) \
  ( ( _sl4_file_check_attribute( x ) & SL4_FILE_ATTRIB_WRITABLE ) ? 1 : 0 )
#define SL4_FILE_IS_EXECUTABLE( x ) \
  ( ( _sl4_file_check_attribute( x ) & SL4_FILE_ATTRIB_EXECUTABLE ) ? 1 : 0 )
#define SL4_FILE_IS_LINK( x ) \
  ( ( _sl4_file_check_attribute( x ) & SL4_FILE_ATTRIB_LINK ) ? 1 : 0 )
#define SL4_FILE_IS_SOCKET( x ) \
  ( ( _sl4_file_check_attribute( x ) & SL4_FILE_ATTRIB_SOCKET ) ? 1 : 0 )

static sl4_file_t *_sl4_file_new_sub(  );
static int _sl4_file_set_path( sl4_file_t *file, const char *path );
static int _sl4_file_set_absolute_path( sl4_file_t *file, const char *path );
static char *_sl4_file_get_cwd_sub(  );
static char *_sl4_file_get_parent_directory( const char *path );
static unsigned char _sl4_file_check_attribute( const char *path );
static int _sl4_file_is_rootdir( sl4_file_t *file );

LIBSL4_API sl4_file_t *sl4_file_new( const char *path )
{
  sl4_file_t *file;
  file = _sl4_file_new_sub(  );
  if ( file == NULL )
    return NULL;
  if ( _sl4_file_set_path( file, path ) != 0 ) {
    sl4_file_delete( file );
    return NULL;
  }
  return file;
}

LIBSL4_API int sl4_file_delete( sl4_file_t *file )
{
  if ( file->flag & SL4_FILE_FLAG_OPEN )
    sl4_file_close( file );
  if ( file->path != NULL )
    free( file->path );
  free( file );
  return 0;
}

LIBSL4_API int sl4_file_close( sl4_file_t *file )
{
  int stat;
  if ( ( file->flag & SL4_FILE_FLAG_OPEN ) == 0 )
    return -1;
  stat = fclose( ( FILE * ) file->fp );
  if ( stat != 0 )
    return -1;
  file->fp = NULL;
  file->flag &= ~SL4_FILE_FLAG_OPEN;
  return 0;
}

LIBSL4_API int sl4_file_eof( sl4_file_t *file )
{
  int stat;
  if ( ( file->flag & SL4_FILE_FLAG_OPEN ) == 0 )
    return -1;
  stat = feof( ( FILE * ) file->fp );
  if ( stat == 0 )
    return 0;
  return 1;
}

LIBSL4_API int sl4_file_exist( sl4_file_t *file )
{
  return SL4_FILE_EXIST( file->path );
}

LIBSL4_API int sl4_file_flush( sl4_file_t *file )
{
  int stat;
  if ( ( file->flag & SL4_FILE_FLAG_OPEN ) == 0 )
    return -1;
  stat = fflush( ( FILE * ) file->fp );
  if ( stat != 0 )
    return -1;
  return 0;
}

LIBSL4_API int sl4_file_gets( sl4_file_t *file, char *buf, int size )
{
  char *ret;
  if ( ( file->flag & SL4_FILE_FLAG_OPEN ) == 0 )
    return -1;
  ret = fgets( buf, size, ( FILE * ) file->fp );
  if ( ret == NULL )
    return -1;
  return 0;
}

LIBSL4_API sl4_file_t *sl4_file_get_cwd(  )
{
  sl4_file_t *file;
  char *path;
  path = _sl4_file_get_cwd_sub(  );
  if ( path == NULL )
    return NULL;
  file = sl4_file_new( path );
  free( path );
  if ( file == NULL )
    return NULL;
  return file;
}

LIBSL4_API const char *sl4_file_get_name( sl4_file_t *file )
{
  return file->name;
}

LIBSL4_API const char *sl4_file_get_path( sl4_file_t *file )
{
  return file->path;
}

LIBSL4_API off_t sl4_file_get_size( sl4_file_t *file )
{
  off_t ret;
#ifdef WIN32
  HANDLE h;
  DWORD low, high;
#else
  struct stat sb;
#endif
#ifdef WIN32
  h = CreateFile( file->path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
  if ( h == INVALID_HANDLE_VALUE ) {
    return ( off_t ) -1;
  }
  low = GetFileSize( h, &high );
  ret = ( off_t ) high *( 2 ^ 32 ) + ( off_t ) low;
  CloseHandle( h );
#else
  if ( lstat( file->path, &sb ) != 0 ) {
    /* usually file not found */
    return ( off_t ) -1;
  }
  if ( S_ISREG( sb.st_mode ) ) {
    ret = sb.st_size;
  } else {
    /* file is not regular file */
    return ( off_t ) -1;
  }
#endif
  return ret;
}

LIBSL4_API const char *sl4_file_get_separator(  )
{
  static char sep[2];
  static int init = 0;
  if ( init == 0 ) {
    sep[0] = FILESEP;
    sep[1] = '\0';
    init = 1;
  }
  return sep;
}

LIBSL4_API int sl4_file_is_directory( sl4_file_t *file )
{
  return SL4_FILE_IS_DIRECTORY( file->path );
}

LIBSL4_API int sl4_file_is_executable( sl4_file_t *file )
{
  return SL4_FILE_IS_EXECUTABLE( file->path );
}

LIBSL4_API int sl4_file_is_file( sl4_file_t *file )
{
  return SL4_FILE_IS_FILE( file->path );
}

LIBSL4_API int sl4_file_is_link( sl4_file_t *file )
{
  return SL4_FILE_IS_LINK( file->path );
}

LIBSL4_API int sl4_file_is_readable( sl4_file_t *file )
{
  return SL4_FILE_IS_READABLE( file->path );
}

LIBSL4_API int sl4_file_is_socket( sl4_file_t *file )
{
  return SL4_FILE_IS_SOCKET( file->path );
}

LIBSL4_API int sl4_file_is_writable( sl4_file_t *file )
{
  return SL4_FILE_IS_WRITABLE( file->path );
}

LIBSL4_API int sl4_file_mkdir( sl4_file_t *file )
{
  char *parent;
  unsigned char attr;
  if ( SL4_FILE_EXIST( file->path ) != 0 )
    return -1;
  if ( _sl4_file_is_rootdir( file ) != 0 )
    return -1;
  /* check parent directory */
  parent = _sl4_file_get_parent_directory( file->path );
  if ( parent == NULL )
    return -1;
  attr = _sl4_file_check_attribute( parent );
  free( parent );
  if ( ( attr & SL4_FILE_ATTRIB_DIRECTORY ) == 0 ) {
    /* parent directory is not available */
    return -1;
  }
#ifdef WIN32
  if ( _mkdir( file->path ) != 0 )
    return -1;
#else
# ifdef HAVE_MKDIR
  if ( mkdir( file->path, 0755 ) != 0 )
    return -1;
# else
#  error "mkdir() required"
# endif
#endif

  return 0;
}

LIBSL4_API int sl4_file_open( sl4_file_t *file, const char *mode )
{
  FILE *fp;

#ifdef WIN32
  char *m = NULL;
  const char *p;
  int has_binary = 0;
#endif

  if ( file->path == NULL )
    return -1;
  if ( SL4_FILE_IS_DIRECTORY( file->path ) )
    return -1;

#ifdef WIN32
  for ( p = mode; *p != '\0'; p++ )
    if ( *p == 'b' )
      has_binary = 1;
  if ( has_binary == 0 ) {
    m = ( char * ) malloc( strlen( mode ) + 2 );
    if ( m == NULL )
      return -1;
    strcpy( m, mode );
    strcat( m, "b" );
    mode = m;
  }
#endif

  fp = fopen( file->path, mode );

#ifdef WIN32
  if ( has_binary == 0 )
    free( m );
#endif

  if ( fp == NULL )
    return -1;
  file->flag |= SL4_FILE_FLAG_OPEN;
  file->fp = fp;
  return 0;
}

LIBSL4_API size_t sl4_file_read( sl4_file_t *file, void *buf, size_t len )
{
  if ( ( file->flag & SL4_FILE_FLAG_OPEN ) == 0 )
    return 0;
  return fread( buf, 1, len, ( FILE * ) file->fp );
}

LIBSL4_API int sl4_file_rmdir( sl4_file_t *file )
{
  unsigned char attr;
  int stat;
  attr = _sl4_file_check_attribute( file->path );
  if ( attr == SL4_FILE_NOT_EXIST )
    return -1;
  if ( ( attr & SL4_FILE_ATTRIB_DIRECTORY ) == 0 )
    return -1;
  if ( _sl4_file_is_rootdir( file ) != 0 )
    return -1;
  if ( ( attr & SL4_FILE_ATTRIB_WRITABLE ) == 0 )
    return -1;

#ifdef WIN32
  stat = _rmdir( file->path );
#else
# ifdef HAVE_RMDIR
  stat = rmdir( file->path );
# else
#  error "rmdir() required"
# endif
#endif

  if ( stat != 0 )
    return -1;
  return 0;
}

LIBSL4_API int sl4_file_seek( sl4_file_t *file, off_t offset,
                              sl4_file_seek_enum whence )
{
  static int seek[] = { SEEK_SET, SEEK_CUR, SEEK_END };
  if ( ( file->flag & SL4_FILE_FLAG_OPEN ) == 0 )
    return 0;
  return fseek( ( FILE * ) file->fp, offset, seek[whence] );
}

LIBSL4_API long sl4_file_tell( sl4_file_t *file )
{
  if ( ( file->flag & SL4_FILE_FLAG_OPEN ) == 0 )
    return 0;
  return ftell( ( FILE * ) file->fp );
}

LIBSL4_API int sl4_file_unlink( sl4_file_t *file )
{
  unsigned char attr;
  int stat;
  attr = _sl4_file_check_attribute( file->path );
  if ( attr == SL4_FILE_NOT_EXIST )
    return -1;
  if ( ( attr & SL4_FILE_ATTRIB_FILE ) == 0 )
    return -1;
  if ( ( attr & SL4_FILE_ATTRIB_WRITABLE ) == 0 )
    return -1;

#ifdef WIN32
  stat = _unlink( file->path );
#else
  stat = unlink( file->path );
#endif

  if ( stat != 0 )
    return -1;
  return 0;
}

LIBSL4_API size_t sl4_file_write( sl4_file_t *file, const void *buf,
                                  size_t len )
{
  if ( ( file->flag & SL4_FILE_FLAG_OPEN ) == 0 )
    return 0;
  return fwrite( buf, 1, len, ( FILE * ) file->fp );
}

LIBSL4_API mode_t sl4_file_umask( mode_t mode )
{
#ifdef WIN32
  return mode;
#else
  return umask( mode );
#endif
}

LIBSL4_API int sl4_file_chdir( sl4_file_t *file )
{
  if ( file->path == NULL )
    return -1;
  if ( SL4_FILE_IS_DIRECTORY( file->path ) == 0 )
    return -1;
  if ( chdir( file->path ) == -1 )
    return -1;
  sl4_setenv( "PWD", file->path );
  return 0;
}

/* private functions */
static sl4_file_t *_sl4_file_new_sub(  )
{
  sl4_file_t *file;
  file = ( sl4_file_t * ) malloc( sizeof( sl4_file_t ) );
  if ( file == NULL )
    return NULL;
  file->flag = SL4_FILE_FLAG_NONE;
  file->fp = NULL;
  file->path = NULL;
  file->name = NULL;
  return file;
}

static int _sl4_file_set_path( sl4_file_t *file, const char *path )
{
  if ( file->flag & SL4_FILE_FLAG_OPEN )
    sl4_file_close( file );
  if ( file->path != NULL ) {
    free( file->path );
    file->path = NULL;
  }
  if ( path == NULL || *path == '\0' )
    return -1;
  return _sl4_file_set_absolute_path( file, path );
}

static int _sl4_file_set_absolute_path( sl4_file_t *file, const char *path )
{
  size_t i, len;
  char *ret;
  int is_root_directory = 0;

#ifdef WIN32
  char buf[_MAX_PATH];
  char *filepart;

  if ( GetFullPathName( path, _MAX_PATH, buf, &filepart ) == 0 )
    return -1;
  /* use lower case for drive letter */
  if ( isupper( buf[0] ) != 0 )
    buf[0] = tolower( buf[0] );
  if ( islower( buf[0] ) ) {
    len = strlen( buf );
    /* local path : 'c:\pathname', 'c:\pathname\' */
    if ( len < 3 || buf[1] != DRIVE_LETTER || buf[2] != FILESEP )
      return -1;
    if ( len == 3 ) {
      /* root directory */
      is_root_directory = 1;
    } else if ( buf[len - 1] == FILESEP )
      buf[len - 1] = '\0';      /* trim the last file separator */
  } else {
    /* UNC path   : '\\hostname\sharename', '\\hostname\sharename\' */
    len = strlen( buf );
    /* must be '\\' + hostname + '\' + sharename */
    if ( len < 5 )
      return -1;
    if ( buf[0] != FILESEP || buf[1] != FILESEP )
      return -1;
    /* skip hostname */
    for ( i = 2; i != len && buf[i] != FILESEP; i++ )
      if ( i == len )
        return -1;
    /* skip sharename */
    /* initialize i : file separator -> next char */
    for ( i++; i != len && buf[i] != FILESEP; i++ )
      if ( i == len ) {
        /* append file separator */
        if ( i == _MAX_PATH )
          return -1;
        buf[i++] = FILESEP;
        buf[i] = '\0';
        len++;
      } else {
        i++;                    /* i: file separator -> next char */
      }
    if ( i == len ) {
      /* root directory */
      is_root_directory = 1;
    }
  }
#else
  char buf[_MAX_PATH];
  char *p;
  /* copy to buffer */
  len = strlen( path );
  if ( *path == FILESEP ) {
    /* absolute directory */
    if ( len >= _MAX_PATH )
      return -1;
    strcpy( buf, path );        /* safe */
  } else {
    /* relative directory */
    char *cwd = _sl4_file_get_cwd_sub(  );
    const char *sep = sl4_file_get_separator(  );
    if ( cwd == NULL )
      return -1;
    if ( strlen( cwd ) + len + strlen( sep ) >= _MAX_PATH ) {
      free( cwd );
      return -1;
    }
    strcpy( buf, cwd );
    strcat( buf, sl4_file_get_separator(  ) );
    strcat( buf, path );
    free( cwd );
  }
  for ( p = buf; *p != '\0'; /* noting */  ) {
    /* *p must be FILESEP */
    char *p1 = p + 1;
    char *p2 = p + 2;
    char *p3 = p + 3;
    if ( *p1 == '\0' ) {
      /* "/" */
      break;                    /* exit loop */
    } else if ( *p1 == FILESEP ) {
      /* "//" -> / */
#ifdef HAVE_MEMMOVE
      memmove( p, p1, strlen( p1 ) + 1 );
#else
# error "memmove() required"
#endif
    } else if ( *p1 == '.' && *p2 == FILESEP ) {
      /* "/./" -> / */
#ifdef HAVE_MEMMOVE
      memmove( p, p2, strlen( p2 ) + 1 );
#else
# error "memmove() required"
#endif
    } else if ( *p1 == '.' && *p2 == '\0' ) {
      /* "/." -> / */
      *p1 = '\0';
      break;                    /* exit loop */
    } else if ( *p1 == '.' && *p2 == '.' && *p3 == '\0' ) {
      /* "/../" -> (parent)/ */
      /* seek char pointer to previous file separator */
      if ( p != buf )
        for ( p--; p != buf && *p != FILESEP; p-- );
      /* after finished seeking, *p will file separator */
      p[1] = '\0';
      break;                    /* exit loop */
    } else if ( *p1 == '.' && *p2 == '.' && *p3 == FILESEP ) {
      /* "/../" -> (parent)/ */
      if ( p != buf )
        for ( p--; p != buf && *p != FILESEP; p-- );
      /* after exit loop, *p will file separator */
#ifdef HAVE_MEMMOVE
      memmove( p, p3, strlen( p3 ) + 1 );
#else
# error "memmove() required"
#endif
    } else {
      /* seek char pointer to next file separator */
      for ( p++; *p != '\0' && *p != FILESEP; p++ );
    }
  }
  len = strlen( buf );
  if ( len == 1 && buf[0] == FILESEP ) {
    /* root directory */
    is_root_directory = 1;
  }
#endif

  /* trim last file separator */
  if ( is_root_directory == 0 ) {
    len = strlen( buf );
    if ( buf[len - 1] == FILESEP ) {
      buf[len - 1] = '\0';
    }
  }
  len = strlen( buf );
  ret = ( char * ) malloc( len + 1 );
  if ( ret == NULL )
    return -1;
  strcpy( ret, buf );
  if ( is_root_directory ) {
    i = len - 1;
  } else {
    for ( i = len - 1; i > 0; i-- )
      if ( ret[i] == FILESEP )
        break;
    /* now, i is separator */
    i++;
  }
  file->path = ret;
  file->name = &ret[i];
  if ( is_root_directory )
    file->flag |= SL4_FILE_FLAG_ROOT;
  return 0;
}

static char *_sl4_file_get_cwd_sub(  )
{
  char pwd[_MAX_PATH];
  char *ret;
  size_t len;

#ifdef HAVE_GETCWD
  if ( getcwd( pwd, _MAX_PATH ) == NULL )
    return NULL;
#else
# error "getcwd() required"
#endif

  len = strlen( pwd );
  ret = ( char * ) malloc( len + 1 );
  if ( ret == NULL )
    return NULL;
  strcpy( ret, pwd );
  return ret;
}

static char *_sl4_file_get_parent_directory( const char *path )
{
  size_t i, len;
  char *parent;
  len = strlen( path );
  for ( i = len - 1; i != 0 && path[i] != FILESEP; i-- );
  if ( i <= 1 )
    return NULL;
  parent = ( char * ) malloc( i );
  if ( parent == NULL )
    return NULL;
  memcpy( parent, path, i );
  parent[i] = '\0';
  return parent;
}

static unsigned char _sl4_file_check_attribute( const char *path )
{
  unsigned char retcode = 0;

#ifdef WIN32
  DWORD attr;
  char ext[_MAX_EXT];
#else
  struct stat sb;
#endif

  if ( path == NULL )
    return SL4_FILE_NOT_EXIST;

#ifdef WIN32
  attr = GetFileAttributes( path );
  /* existing check */
  if ( attr == -1 ) {
    return SL4_FILE_NOT_EXIST;
  }
  /* file / directory check */
  if ( ( attr & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY ) {
    retcode |= SL4_FILE_ATTRIB_DIRECTORY;
  } else {
    retcode |= SL4_FILE_ATTRIB_FILE;
  }
  /* readable / writable check */
  retcode |= SL4_FILE_ATTRIB_READABLE;
  if ( ( attr & FILE_ATTRIBUTE_READONLY ) == 0 )
    retcode |= SL4_FILE_ATTRIB_WRITABLE;
  /* executable check  */
  if ( retcode & SL4_FILE_ATTRIB_DIRECTORY ) {
    retcode |= SL4_FILE_ATTRIB_EXECUTABLE;
  } else {
    int i, is_executable;
    char tmp[_MAX_EXT];
    char *env;
    _splitpath( path, NULL, NULL, NULL, ext );
    is_executable = 0;
    env = getenv( "PATHEXT" );
    if ( env == NULL )
      env = ".COM;.EXE;.BAT";
    while ( *env != '\0' ) {
      for ( i = 0; *env != '\0' && *env != ';'; i++, env++ )
        tmp[i] = *env;
      tmp[i] = '\0';
      if ( stricmp( ext, tmp ) == 0 ) {
        is_executable = 1;
        break;
      }
      if ( *env == ';' )
        env++;
    }
    if ( is_executable != 0 )
      retcode |= SL4_FILE_ATTRIB_EXECUTABLE;
    /* link check */
    if ( stricmp( ext, ".LNK" ) == 0 )
      retcode |= SL4_FILE_ATTRIB_LINK;
  }
  /* socket check - nothing to do */
#else
  /* existing check */
  if ( lstat( path, &sb ) != 0 )
    return SL4_FILE_NOT_EXIST;
  /* file / directory check */
  if ( ( sb.st_mode & S_IFDIR ) == S_IFDIR )
    retcode |= SL4_FILE_ATTRIB_DIRECTORY;
  else if ( ( sb.st_mode & S_IFREG ) == S_IFREG )
    retcode |= SL4_FILE_ATTRIB_FILE;
  /* readable / writable check */
  if ( ( sb.st_mode & S_IRUSR ) == S_IRUSR )
    retcode |= SL4_FILE_ATTRIB_READABLE;
  if ( ( sb.st_mode & S_IWUSR ) == S_IWUSR )
    retcode |= SL4_FILE_ATTRIB_WRITABLE;
  /* executable check */
  if ( ( sb.st_mode & S_IXUSR ) == S_IXUSR )
    retcode |= SL4_FILE_ATTRIB_EXECUTABLE;
  /* link check */
  if ( ( sb.st_mode & S_IFLNK ) == S_IFLNK )
    retcode |= SL4_FILE_ATTRIB_LINK;
  /* socket check */
  if ( ( sb.st_mode & S_IFSOCK ) == S_IFSOCK )
    retcode |= SL4_FILE_ATTRIB_SOCKET;
#endif

  return retcode;
}

static int _sl4_file_is_rootdir( sl4_file_t *file )
{
  size_t len;
  len = strlen( file->path );
  if ( file->path[len - 1] == FILESEP )
    return 1;
  return 0;
}
