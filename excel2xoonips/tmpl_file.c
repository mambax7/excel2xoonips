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

#include "libsl4.h"
#include "cexcel.h"
#include "excel2xoonips.h"

sl4_string_t *tmpl_file_get( const char *text, strarray_t * options,
                             int item_id, const char *sname,
                             const char *address, sl4_file_t **file,
                             sl4_string_t **file_name_inzip,
                             sl4_string_t **original_file_name,
                             sl4_string_t **mime_type,
                             sl4_string_t **file_size,
                             sl4_string_t **suffix, int is_required,
                             sl4_string_t *tmpdir, char *item_xml )
{
  off_t fsize;
  sl4_file_t *_file;
  sl4_string_t *str, *file_type_name;
  sl4_string_t *_text, *file_path, *_file_name_inzip, *_mime_type,
    *_original_file_name, *_file_size, *_suffix;
  const char *file_tag_fmt =
    "<file item_id=\"%d\" file_type_name=\"%s\" original_file_name=\"%s\" file_name=\"%s\" file_size=\"%s\" mime_type=\"%s\">\n<caption></caption>\n</file>";
  sl4_string_t *tmp;
  size_t pos;
  int occurrence_no;
  char tmp_str1[128];
  const char *file_tag_fmt_v40 =
    "<C:%s C:type=\"file\" C:column_name=\"file_id\">\n"
    "<C:original_file_name>%s</C:original_file_name>\n"
    "<C:file_id>%d</C:file_id>\n"
    "<C:mime_type>%s</C:mime_type>\n"
    "<C:file_size>%s</C:file_size>\n"
    "<caption></caption>\n"
    "<C:sess_id></C:sess_id>\n"
    "<C:search_module_name></C:search_module_name>\n"
    "<C:search_module_version></C:search_module_version>\n"
    "<C:download_count>0</C:download_count>\n"
    "<C:occurrence_number>%d</C:occurrence_number>\n"
    "</C:%s>\n";

  /* file type name */
  file_type_name = strarray_get( options, 0 );

  /* result string */
  str = sl4_string_new( NULL );
  if ( str == NULL ) {
    ERROR_OUTMEM(  );
  }

  /* copy of input file name */
  _text = sl4_string_new( text );
  if ( _text == NULL ) {
    ERROR_OUTMEM(  );
  }
  sl4_string_trim( _text );

  /* requirements check */
  if ( sl4_string_empty( _text ) ) {
    sl4_string_delete( _text );
    if ( is_required ) {
      /* error */
      sl4_string_delete( str );
      return NULL;
    } else {
      /* success */
      return str;
    }
  }
  occurrence_no = 1;
  pos = 0;
  tmp = sl4_string_tokenize( _text, "\n", &pos );
  while ( tmp ) {
      sl4_string_t *tag = sl4_string_new( NULL );
      if ( tag == NULL ) {
        ERROR_OUTMEM(  );
      }

      /* physical file path */
      file_path = sl4_string_new( NULL );
      if ( file_path == NULL ) {
        ERROR_OUTMEM(  );
      }
      if ( cexcel_iconv_convert
           ( tmp, file_path, "UTF-8",
             E2X_FILENAME_CODING_SYSTEM ) != CEXCEL_ICONV_RETTYPE_SUCCESS ) {
        ERROR_UNEXPECTED(  );
      }
#ifndef WIN32
      /* convert file separator '\\' to '/' */
      {
        size_t pos = 0;
        while ( ( pos =
                  sl4_string_findn( file_path, "\\", pos ) ) != ( size_t ) -1 ) {
          if ( sl4_string_replace( file_path, pos, 1, "/" ) != 0 ) {
            break;
          }
        }
    	  }
#endif /* WIN32 */
      _file = sl4_file_new( sl4_string_get( file_path ) );
      if ( _file == NULL ) {
        ERROR_OUTMEM(  );
      }

      /* check file exists */
      if ( sl4_file_is_readable( _file ) == 0 ) {
        /* file not found */
        sl4_string_delete( str );
        sl4_string_delete( _text );
        sl4_string_delete( file_path );
        sl4_string_delete( tmp );
        sl4_file_delete( _file );
        return NULL;
      }

      fsize = sl4_file_get_size( _file );
      _file_size = sl4_string_new( NULL );
      if ( _file_size == NULL ) {
        ERROR_OUTMEM(  );
      }
      if ( sl4_string_sprintf( _file_size, "%ld", fsize ) != 0 ) {
        ERROR_OUTMEM(  );
      }
      _mime_type = sl4_file_get_mimetype( _file, &_suffix );
      _original_file_name = sl4_string_new( sl4_file_get_name( _file ) );
      if ( _original_file_name == NULL ) {
        ERROR_OUTMEM(  );
      }

      _file_name_inzip = sl4_string_new( NULL );
      if ( _file_name_inzip == NULL ) {
        ERROR_OUTMEM(  );
      }
      if (!is_compat){
        if ( sl4_string_sprintf
             ( _file_name_inzip, "%d/%s", files_count, sl4_string_get(_original_file_name)) != 0 ) {
          ERROR_OUTMEM(  );
        }
      }else{
        if ( sl4_string_sprintf
             ( _file_name_inzip, "files/%s_%s", sname, address ) != 0 ) {
          ERROR_OUTMEM(  );
        }
      }
      if ( is_compat ) {
          if ( sl4_string_sprintf
               ( tag, file_tag_fmt, item_id, sl4_string_get( file_type_name ),
                 sl4_string_get( _original_file_name ),
                 sl4_string_get( _file_name_inzip ), sl4_string_get( _file_size ),
                 sl4_string_get( _mime_type ) ) != 0 ) {
            ERROR_OUTMEM(  );
          }
      } else {
          if ( sl4_string_sprintf
               ( tag, file_tag_fmt_v40, item_xml, sl4_string_get( _original_file_name ), files_count,
                 sl4_string_get( _mime_type ), sl4_string_get( _file_size ), occurrence_no, item_xml) != 0 ) {
            ERROR_OUTMEM(  );
          }
          occurrence_no++;
      }
      if ( !is_compat ) {
        /* mkdir file temporary dicrectory */
        sprintf(tmp_str1, "/%d", files_count);
        tempdir_mkdir( tmpdir, tmp_str1 );
        files_count++;

        /* copy file to temporary directory */
        if ( tempdir_copyfile
             ( tmpdir, sl4_file_get_path( _file ),
             sl4_string_get(_file_name_inzip) ) != 0 ) {
          ERROR_UNEXPECTED(  );
        }
      }
      sl4_string_append( str, sl4_string_get( tag ) );
      sl4_string_delete( tag );
      sl4_string_delete( tmp );
      sl4_string_delete( file_path );
      tmp = sl4_string_tokenize( _text, "\n", &pos );
      if ( tmp ) {
          if ( is_compat || strcmp(item_xml, "image") != 0  || strcmp(sname, "url") == 0 ) {
              sl4_string_delete( str );
              sl4_string_delete( tmp );
              sl4_string_delete( _text );
            return NULL;
          }
          sl4_file_delete( _file );
          sl4_string_delete( _file_name_inzip );
          sl4_string_delete( _original_file_name );
          sl4_string_delete( _mime_type );
          sl4_string_delete( _file_size );
          sl4_string_delete( _suffix );
      }
  }
  /* release unused memory */
  sl4_string_delete( _text );

  *file = _file;
  *file_name_inzip = _file_name_inzip;
  *original_file_name = _original_file_name;
  *mime_type = _mime_type;
  *file_size = _file_size;
  *suffix = _suffix;
  return str;
}
