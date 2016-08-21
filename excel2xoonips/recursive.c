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

#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif

#ifdef WIN32
# include "win32_dirent.h"
#else
# include "dirent.h"
#endif

#include "libsl4.h"
#include "cexcel.h"
#include "excel2xoonips.h"

#ifndef _MAX_PATH
# ifdef  MAXPATHLEN
#  define _MAX_PATH  MAXPATHLEN
# else
#  error definition of 'MAXPATHLEN' required
# endif
#endif

int recursive_proc( const char *path, recursive_func_t func, void *data )
{
  char buf[_MAX_PATH];
  DIR *dp;
  struct dirent *entry;
  if ( ( dp = opendir( path ) ) == NULL ) {
    return -1;
  }
  while ( ( entry = readdir( dp ) ) != NULL ) {
    sl4_file_t *file;
    int is_directory = 0;
    if ( !strcmp( entry->d_name, "." ) || !strcmp( entry->d_name, ".." ) )
      continue;
    if ( strlen( path ) + strlen( entry->d_name ) + 2 > _MAX_PATH ) {
      /* 2 means '/' + '\0' */
      closedir( dp );
      return -1;
    }
    strcpy( buf, path );
    strcat( buf, "/" );
    strcat( buf, entry->d_name );
    file = sl4_file_new( buf );
    if ( file != NULL ) {
      is_directory = sl4_file_is_directory( file );
      sl4_file_delete( file );
    }
    if ( is_directory ) {
      /* directory */
      if ( recursive_proc( buf, func, data ) != 0 ) {
        closedir( dp );         /* error */
        return -1;              /* error */
      }
    } else {
      /* file */
      if ( func( buf, 0, data ) != 0 ) {
        closedir( dp );         /* error */
        return -1;
      }
    }
  }
  closedir( dp );
  return func( path, 1, data );
}
