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

/* $Id: user.c,v 1.2 2008/01/24 14:59:41 orrisroot Exp $ */

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

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_PWD_H
# include <pwd.h>
#endif

#include "libsl4.h"

#ifndef WIN32
# ifdef  MAXPATHLEN
#  define _MAX_PATH MAXPATHLEN
# else
#  error MAXPATHLEN not defined
# endif
#endif

#define MAXUSERNAME 256

#define SATELLITE_TEMP_PREFIX              "SATELLITE-Lang"

#ifdef WIN32
# define SATELLITE_WIN32_HOME              "My SATELLITE"
# define SATELLITE_RESOURCE_PATH           "SATELLITE4"
#else
# define SATELLITE_RESOURCE_PATH           ".sl4rc"
#endif

LIBSL4_API const char *sl4_user_name(  )
{
  static int init = 0;
  static char var[MAXUSERNAME];
  if ( init == 0 ) {
#ifdef WIN32
    DWORD usiz = MAXUSERNAME;
    if ( GetUserName( var, &usiz ) == 0 )
      sl4_error_fatal( "Failed to get user name", __FILE__, __LINE__ );
#else
    uid_t uid;
    struct passwd *pass;
    uid = getuid(  );
    pass = getpwuid( uid );
    if ( strlen( pass->pw_name ) + 1 > MAXUSERNAME ) {
# ifdef HAVE_ENDPWENT
      endpwent(  );
# endif
      sl4_error_fatal( "Failed to get user name", __FILE__, __LINE__ );
    }
    strcpy( var, pass->pw_name );       /* safe */
# ifdef HAVE_ENDPWENT
    endpwent(  );
# endif
#endif
    init = 1;
  }
  return var;
}

LIBSL4_API const char *sl4_user_home_dir(  )
{
  static int is_first = 1;
  static char var[_MAX_PATH];
  static const char *errmes = "failed to get home directory";
  if ( is_first ) {
    const char *home;
    home = sl4_getenv( "SL_HOME" );
    if ( home == NULL )
      home = sl4_getenv( "HOME" );
    if ( home != NULL ) {
      if ( strlen( home ) >= _MAX_PATH )
        return 0;
      strcpy( var, home );
    }
#ifdef WIN32
    if ( home == NULL ) {
      const char *mydoc = sl4_win32_path_mydocuments(  );
      /* MyDoc + '/' + "My SATELLITE" */
      if ( strlen( mydoc ) + strlen( SATELLITE_WIN32_HOME ) + 1 >= _MAX_PATH )
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      sprintf( "%s\\%s", mydoc, SATELLITE_WIN32_HOME ); /* safe */
      {
        /* try to create directory */
        sl4_file_t *file;
        file = sl4_file_new( var );
        if ( file == NULL )
          sl4_error_fatal( errmes, __FILE__, __LINE__ );
        if ( sl4_file_is_directory( file ) == 0 )
          if ( sl4_file_mkdir( file ) != 0 )
            sl4_error_fatal( errmes, __FILE__, __LINE__ );
        sl4_file_delete( file );
      }
    }
#endif
    {
      /* check file permission */
      sl4_file_t *file;
      file = sl4_file_new( var );
      if ( file == NULL )
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      if ( sl4_file_is_directory( file ) == 0 ) {
        sl4_file_delete( file );
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      }
      if ( sl4_file_is_readable( file ) == 0 ||
           sl4_file_is_writable( file ) == 0 ||
           sl4_file_is_executable( file ) == 0 ) {
        sl4_file_delete( file );
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      }
      sl4_file_delete( file );
    }
    is_first = 0;
  }
  return var;
}

LIBSL4_API const char *sl4_user_resource_dir(  )
{
  static int is_first = 1;
  static char var[_MAX_PATH];
  static const char *errmes = "failed to get user resource directory";
  if ( is_first ) {
    const char *basedir, *sep;
    sl4_file_t *file;
#ifdef WIN32
    basedir = sl4_win32_path_applicationdata(  );
#else
    basedir = sl4_user_home_dir(  );
#endif
    sep = sl4_file_get_separator(  );
    if ( strlen( basedir ) + strlen( sep ) + strlen( SATELLITE_RESOURCE_PATH )
         >= _MAX_PATH )
      sl4_error_fatal( errmes, __FILE__, __LINE__ );
#ifdef HAVE_SNPRINTF
    snprintf( var, _MAX_PATH, "%s%s%s", basedir, sep,
              SATELLITE_RESOURCE_PATH );
#else
# error "snprintf() required"
#endif
    /* try to create directory */
    file = sl4_file_new( var );
    if ( file == NULL )
      sl4_error_fatal( errmes, __FILE__, __LINE__ );
    if ( sl4_file_is_directory( file ) == 0 )
      if ( sl4_file_mkdir( file ) != 0 )
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
    /* check file permission */
    if ( sl4_file_is_readable( file ) == 0 ||
         sl4_file_is_writable( file ) == 0 ||
         sl4_file_is_executable( file ) == 0 ) {
      sl4_file_delete( file );
      sl4_error_fatal( errmes, __FILE__, __LINE__ );
    }
    sl4_file_delete( file );
    is_first = 0;
  }
  return var;
}

LIBSL4_API const char *sl4_user_temp_dir(  )
{
  static int is_first = 1;
  static char var[_MAX_PATH];
  static const char *errmes = "failed to get temp directory";
  if ( is_first ) {
    const char *temp;
    temp = sl4_getenv( "SL_TEMP" );
    if ( temp == NULL )
      temp = sl4_getenv( "TEMP" );
    if ( temp == NULL )
      temp = sl4_getenv( "TMP" );
#ifndef WIN32
    if ( temp == NULL )
      temp = "/var/tmp";
#endif
    if ( temp == NULL )
      sl4_error_fatal( errmes, __FILE__, __LINE__ );
#ifdef WIN32
    /* expand path name */
    if ( sl4_win32_path_getfull( temp, var, _MAX_PATH ) != 0 )
      sl4_error_fatal( errmes, __FILE__, __LINE__ );
#else
    if ( strlen( temp ) >= _MAX_PATH )
      sl4_error_fatal( errmes, __FILE__, __LINE__ );
    strcpy( var, temp );        /* safe */
#endif
    {
      /* check file permission */
      sl4_file_t *file;
      file = sl4_file_new( var );
      if ( file == NULL )
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      if ( sl4_file_is_directory( file ) == 0 ) {
        sl4_file_delete( file );
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      }
      if ( sl4_file_is_readable( file ) == 0 ||
           sl4_file_is_writable( file ) == 0 ||
           sl4_file_is_executable( file ) == 0 ) {
        sl4_file_delete( file );
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      }
      sl4_file_delete( file );
    }
    {
      /* create temporary directory */
      /*  - ENV[temp] + / + SATELLITE_TEMP_PREFIX + PID */
      sl4_file_t *file;
      const char *sep = sl4_file_get_separator(  );
      char pid_buf[16];
      pid_t pid;
#ifdef WIN32
      pid = ( pid_t ) GetCurrentProcessId(  );
#else
      pid = getpid(  );
#endif
#ifdef HAVE_SNPRINTF
      snprintf( pid_buf, 16, "%d", ( int ) pid );
#else
# error "snprintf() required"
#endif
      if ( strlen( var ) + strlen( sep ) + strlen( SATELLITE_TEMP_PREFIX )
           + strlen( pid_buf ) >= _MAX_PATH )
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      strcat( var, sep );
      strcat( var, SATELLITE_TEMP_PREFIX );
      strcat( var, pid_buf );
      /*  - check access permissions */
      file = sl4_file_new( var );
      if ( file == NULL )
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      if ( sl4_file_is_directory( file ) == 0 ) {
        if ( sl4_file_mkdir( file ) != 0 ) {
          sl4_file_delete( file );
          sl4_error_fatal( errmes, __FILE__, __LINE__ );
        }
      }
      if ( sl4_file_is_readable( file ) == 0 ||
           sl4_file_is_writable( file ) == 0 ||
           sl4_file_is_executable( file ) == 0 ) {
        sl4_file_delete( file );
        sl4_error_fatal( errmes, __FILE__, __LINE__ );
      }
      sl4_file_delete( file );
    }
    is_first = 0;
  }
  return var;
}
