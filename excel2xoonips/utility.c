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
#include <sys/param.h>

#ifdef WIN32
# include <windows.h>
#endif

#include "libsl4.h"
#include "cexcel.h"
#include "excel2xoonips.h"

#ifndef WIN32
# ifdef  MAXPATHLEN
#  define _MAX_PATH MAXPATHLEN
# else
#  error MAXPATHLEN not defined
# endif
#endif

const char *get_bin_dirname( const char *bin )
{
  static char var[_MAX_PATH];
  static int is_init = 0;
  size_t i, len;
  if ( is_init != 0 ) {
    return var;
  }
#ifdef WIN32
  if ( GetModuleFileName( NULL, var, _MAX_PATH ) == 0 )
    return NULL;
  len = strlen( var );
  for ( i = 0; i < len; i++ ) {
    if ( var[i] == '\\' ) {
      var[i] = '/';
    }
  }
#else
  len = strlen( bin );
  if ( len >= _MAX_PATH ) {
    return NULL;
  }
  strcpy( var, bin );
#endif
  for ( i = len - 1; i > 0; i-- ) {
    if ( var[i] == '/' ) {
      var[i] = '\0';
      break;
    }
  }
  is_init = 1;
  return var;
}

int sl4_string_htmlspecialchars( sl4_string_t *str )
{
  size_t pos;
  for ( pos = 0; pos < str->len; pos++ ) {
    switch ( str->mem[pos] ) {
    case '&':
      if ( sl4_string_replace( str, pos, 1, "&amp;" ) != 0 ) {
        ERROR_OUTMEM(  );
      }
      pos += 4;
      break;
    case '>':
      if ( sl4_string_replace( str, pos, 1, "&gt;" ) != 0 ) {
        ERROR_OUTMEM(  );
      }
      pos += 3;
      break;
    case '<':
      if ( sl4_string_replace( str, pos, 1, "&lt;" ) != 0 ) {
        ERROR_OUTMEM(  );
      }
      pos += 3;
      break;
    case '"':
      if ( sl4_string_replace( str, pos, 1, "&quot;" ) != 0 ) {
        ERROR_OUTMEM(  );
      }
      pos += 5;
      break;
    case '\'':
      if ( sl4_string_replace( str, pos, 1, "&#039;" ) != 0 ) {
        ERROR_OUTMEM(  );
      }
      pos += 4;
      break;
    }
  }
  return 0;
}

int sl4_string_tolower( sl4_string_t *str )
{
  size_t i;
  for ( i = 0; i < str->len; i++ ) {
    if ( str->mem[i] >= 'A' && str->mem[i] <= 'Z' ) {
      str->mem[i] += 'a' - 'A';
    }
  }
  return 0;
}

int sl4_string_replace2( sl4_string_t *str, const char *ptn, const char *rep )
{
  size_t pos, next, plen;
  pos = 0;
  plen = strlen( ptn );
  for ( next = sl4_string_findn( str, ptn, pos );
        next != ( size_t ) -1; next = sl4_string_findn( str, ptn, pos ) ) {
    if ( sl4_string_replace( str, next, plen, rep ) != 0 ) {
      return -1;
    }
    pos = next + plen;
  }
  return 0;
}

int sl4_string_convert_format( sl4_string_t *str )
{
  int i;
  const char *ptns[] =
    { "\\n", "\\t", "\\\\", "\\\'", "\\\"", "\\0", "%", NULL };
  const char *reps[] =
    { "\x0a", "\x09", "\\", "\x2c", "\x22", " ", "%%", NULL };
  for ( i = 0; ptns[i] != NULL; i++ ) {
    sl4_string_replace2( str, ptns[i], reps[i] );
  }
  return sl4_string_sprintf( str, sl4_string_get( str ) );
}

sl4_string_t *sl4_file_get_mimetype( sl4_file_t *file, sl4_string_t **suffix )
{
  sl4_string_t *str, *_suffix;
  size_t i, len, pos;
  const char *ext, *type;
  const char *unknown_type = "application/octet-stream";
  const char *map_ext[] = {
    "hqx", "application/mac-binhex40",
    "doc", "application/msword",
    "dot", "application/msword",
    "bin", "application/octet-stream",
    "lha", "application/octet-stream",
    "lzh", "application/octet-stream",
    "exe", "application/octet-stream",
    "class", "application/octet-stream",
    "so", "application/octet-stream",
    "dll", "application/octet-stream",
    "pdf", "application/pdf",
    "ai", "application/postscript",
    "eps", "application/postscript",
    "ps", "application/postscript",
    "smi", "application/smil",
    "smil", "application/smil",
    "wbxml", "application/vnd.wap.wbxml",
    "wmlc", "application/vnd.wap.wmlc",
    "wmlsc", "application/vnd.wap.wmlscriptc",
    "xla", "application/vnd.ms-excel",
    "xls", "application/vnd.ms-excel",
    "xlt", "application/vnd.ms-excel",
    "ppt", "application/vnd.ms-powerpoint",
    "csh", "application/x-csh",
    "dcr", "application/x-director",
    "dir", "application/x-director",
    "dxr", "application/x-director",
    "spl", "application/x-futuresplash",
    "gtar", "application/x-gtar",
    "php", "application/x-httpd-php",
    "php3", "application/x-httpd-php",
    "php5", "application/x-httpd-php",
    "phtml", "application/x-httpd-php",
    "js", "application/x-javascript",
    "sh", "application/x-sh",
    "swf", "application/x-shockwave-flash",
    "sit", "application/x-stuffit",
    "tar", "application/x-tar",
    "tcl", "application/x-tcl",
    "xhtml", "application/xhtml+xml",
    "xht", "application/xhtml+xml",
    "xhtml", "application/xml",
    "ent", "application/xml-external-parsed-entity",
    "dtd", "application/xml-dtd",
    "mod", "application/xml-dtd",
    "gz", "application/x-gzip",
    "zip", "application/zip",
    "au", "audio/basic",
    "snd", "audio/basic",
    "mid", "audio/midi",
    "midi", "audio/midi",
    "kar", "audio/midi",
    "mp1", "audio/mpeg",
    "mp2", "audio/mpeg",
    "mp3", "audio/mpeg",
    "aif", "audio/x-aiff",
    "aiff", "audio/x-aiff",
    "m3u", "audio/x-mpegurl",
    "ram", "audio/x-pn-realaudio",
    "rm", "audio/x-pn-realaudio",
    "rpm", "audio/x-pn-realaudio-plugin",
    "ra", "audio/x-realaudio",
    "wav", "audio/x-wav",
    "bmp", "image/bmp",
    "gif", "image/gif",
    "jpeg", "image/jpeg",
    "jpg", "image/jpeg",
    "jpe", "image/jpeg",
    "png", "image/png",
    "tiff", "image/tiff",
    "tif", "image/tif",
    "wbmp", "image/vnd.wap.wbmp",
    "pnm", "image/x-portable-anymap",
    "pbm", "image/x-portable-bitmap",
    "pgm", "image/x-portable-graymap",
    "ppm", "image/x-portable-pixmap",
    "xbm", "image/x-xbitmap",
    "xpm", "image/x-xpixmap",
    "ics", "text/calendar",
    "ifb", "text/calendar",
    "css", "text/css",
    "html", "text/html",
    "htm", "text/html",
    "asc", "text/plain",
    "txt", "text/plain",
    "rtf", "text/rtf",
    "sgml", "text/x-sgml",
    "sgm", "text/x-sgml",
    "tsv", "text/tab-seperated-values",
    "wml", "text/vnd.wap.wml",
    "wmls", "text/vnd.wap.wmlscript",
    "xsl", "text/xml",
    "mpeg", "video/mpeg",
    "mpg", "video/mpeg",
    "mpe", "video/mpeg",
    "qt", "video/quicktime",
    "mov", "video/quicktime",
    "avi", "video/x-msvideo",
    NULL, NULL
  };
  _suffix = sl4_string_new( file->name );
  str = sl4_string_new( NULL );
  if ( _suffix == NULL || str == NULL ) {
    ERROR_OUTMEM(  );
  }
  len = sl4_string_length( _suffix );
  pos = sl4_string_rfind( _suffix, "." );
  if ( pos == ( size_t ) -1 || pos == len ) {
    if ( sl4_string_set( str, unknown_type ) != 0 ) {
      ERROR_OUTMEM(  );
    }
    sl4_string_set( _suffix, "" );
  } else {
    sl4_string_t *_lower;
    if ( sl4_string_erase( _suffix, 0, pos + 1 ) != 0 ) {
      ERROR_UNEXPECTED(  );
    }
    _lower = sl4_string_new( sl4_string_get( _suffix ) );
    sl4_string_tolower( _lower );
    type = NULL;
    ext = NULL;
    for ( i = 0; map_ext[i] != NULL; i += 2 ) {
      ext = map_ext[i];
      type = map_ext[i + 1];
      if ( sl4_string_compare( _lower, ext ) == 0 ) {
        if ( sl4_string_set( str, type ) != 0 ) {
          ERROR_OUTMEM(  );
        }
        break;
      }
    }
    sl4_string_delete( _lower );
    ext = map_ext[i];
    type = map_ext[i + 1];
    if ( type == NULL ) {
      if ( sl4_string_set( str, unknown_type ) != 0 ) {
        ERROR_OUTMEM(  );
      }
    }
  }
  if ( suffix != NULL ) {
    *suffix = _suffix;
  } else {
    sl4_string_delete( _suffix );
  }
  return str;
}
