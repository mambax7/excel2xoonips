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

#include "libsl4.h"
#include "cexcel.h"
#include "excel2xoonips.h"

static inifile_t *_inifile_new( const char *inifile_path );
static inifile_column_t *_inifile_column_new( sl4_string_t *fields,
                                              sl4_string_t *type,
                                              sl4_string_t *optional,
                                              const char *file, int line,
                                              sl4_string_t *errmes );
static int _inifile_column_delete( inifile_column_t * col );

inifile_t *inifile_load( const char *inidir, const char *itemtype,
                         sl4_string_t *errmes )
{
  int line;
  sl4_string_t *inifile_path;
  sl4_file_t *inifile;
  strarray_t *matches;
  inifile_t *ini;
  char buf[INIFILE_MAX_LINE];
  inifile_path = sl4_string_new( inidir );
  if ( inifile_path == NULL ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_string_append( inifile_path, "/" ) != 0 ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_string_append( inifile_path, itemtype ) != 0 ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_string_append( inifile_path, ".ini" ) != 0 ) {
    ERROR_OUTMEM(  );
  }
  inifile = sl4_file_new( sl4_string_get( inifile_path ) );
  if ( sl4_file_open( inifile, "r" ) != 0 ) {
    /* failed to open file */
    sl4_string_sprintf( errmes,
                        "Failed to Open INI File for XooNIps Item Type %s.",
                        itemtype );
    return NULL;
  }
  line = 0;
  ini = _inifile_new( sl4_string_get( inifile_path ) );
  matches = strarray_new(  );
  if ( matches == NULL ) {
    ERROR_OUTMEM(  );
  }
  while ( !sl4_file_eof( inifile ) ) {
    line++;
    if ( sl4_file_gets( inifile, buf, INIFILE_MAX_LINE ) == 0 ) {
      sl4_string_t *tmp;
      tmp = sl4_string_new( buf );
      if ( tmp == NULL ) {
        ERROR_OUTMEM(  );
      }
      sl4_string_trim( tmp );
      if ( myonig_match( "/^[#;].*$/", sl4_string_get( tmp ), NULL ) ) {
        /* comment line */
        sl4_string_delete( tmp );
        continue;
      }
      if ( myonig_match
           ( "/^\\s*([^=]+)\\s*=\\s*([^:]+)\\s*:\\s*([^#]+)(#.*)?\\s*$/",
             sl4_string_get( tmp ), matches ) ) {
        sl4_string_t *m[5];
        m[0] = strarray_get( matches, 0 );      /* all */
        m[1] = strarray_get( matches, 1 );      /* fields */
        m[2] = strarray_get( matches, 2 );      /* type */
        m[3] = strarray_get( matches, 3 );      /* optional */
        m[4] = strarray_get( matches, 4 );      /* comment */
        ini->col[ini->ncol] =
          _inifile_column_new( m[1], m[2], m[3],
                               sl4_string_get( inifile_path ), line, errmes );
        if ( ini->col[ini->ncol] == NULL ) {
          sl4_string_delete( tmp );
          sl4_file_close( inifile );
          strarray_delete( matches );
          sl4_string_delete( inifile_path );
          inifile_delete( ini );
          return NULL;
        }
        ini->ncol++;
      } else {
        /* invalid format */
        sl4_string_delete( tmp );
        sl4_file_close( inifile );
        strarray_delete( matches );
        sl4_string_delete( inifile_path );
        inifile_delete( ini );
        sl4_string_sprintf( errmes,
                            "Invalid format line found in item type %s line %d.",
                            itemtype, line );
        return NULL;
      }
      strarray_clear( matches );
      sl4_string_delete( tmp );
    }
  }
  sl4_file_close( inifile );
  strarray_delete( matches );
  sl4_string_delete( inifile_path );
  return ini;
}

int inifile_delete( inifile_t * ini )
{
  int i;
  sl4_string_delete( ini->inifile_path );
  for ( i = 0; i < ini->ncol; i++ ) {
    _inifile_column_delete( ini->col[i] );
  }
  free( ini );
  return 0;
}

static inifile_t *_inifile_new( const char *inifile_path )
{
  inifile_t *ini;
  ini = ( inifile_t * ) malloc( sizeof( inifile_t ) );
  if ( ini == NULL ) {
    ERROR_OUTMEM(  );
  }
  ini->inifile_path = sl4_string_new( inifile_path );
  if ( ini->inifile_path == NULL ) {
    ERROR_OUTMEM(  );
  }
  ini->ncol = 0;
  return ini;
}

static inifile_column_t *_inifile_column_new( sl4_string_t *fields,
                                              sl4_string_t *type,
                                              sl4_string_t *optional,
                                              const char *itemtype,
                                              int line, sl4_string_t *errmes )
{
  inifile_column_t *col;
  sl4_string_t *tmp;
  size_t pos;
  /* trim arguments */
  sl4_string_trim( fields );
  sl4_string_trim( type );
  sl4_string_trim( optional );

  /* allocate memory */
  col = ( inifile_column_t * ) malloc( sizeof( inifile_column_t ) );
  if ( col == NULL ) {
    ERROR_OUTMEM(  );
  }
  col->field = strarray_new(  );
  if ( col->field == NULL ) {
    ERROR_OUTMEM(  );
  }
  col->type = INIFILE_TYPE_UNDEF;
  col->options = NULL;

  /* fields */
  pos = 0;
  tmp = sl4_string_tokenize( fields, ",", &pos );
  if ( tmp == NULL ) {
    ERROR_UNEXPECTED(  );
  }
  while ( tmp ) {
    sl4_string_trim( tmp );
    if ( sl4_string_empty( tmp ) ) {
      sl4_string_delete( tmp );
      _inifile_column_delete( col );
      sl4_string_sprintf( errmes, "Empty field found in %s.ini line %d.",
                          itemtype, line );
      return NULL;
    }
    strarray_add( col->field, tmp );
    tmp = sl4_string_tokenize( fields, ",", &pos );
  }
  /* type */
  if ( sl4_string_compare( type, "string" ) == 0 ) {
    col->type = INIFILE_TYPE_STRING;
  } else if ( sl4_string_compare( type, "int" ) == 0 ) {
    col->type = INIFILE_TYPE_STRING;
  } else if ( sl4_string_compare( type, "datetime" ) == 0 ) {
    col->type = INIFILE_TYPE_DATETIME;
  } else if ( sl4_string_compare( type, "indexes" ) == 0 ) {
    col->type = INIFILE_TYPE_INDEXES;
  } else if ( sl4_string_compare( type, "lang" ) == 0 ) {
    int i;
    const char *lang[] = {
      "eng", "jpn", "fra", "deu", "esl", "ita", "dut",
      "sve", "nor", "dan", "fin", "por", "chi", "kor",
      NULL
    };
    col->type = INIFILE_TYPE_SELECT;
    col->options = strarray_new(  );
    if ( col->options == NULL ) {
      ERROR_OUTMEM(  );
    }
    for ( i = 0; lang[i] != NULL; i++ ) {
      tmp = sl4_string_new( lang[i] );
      if ( tmp == NULL ) {
        ERROR_OUTMEM(  );
      }
      strarray_add( col->options, tmp );
    }
  } else if ( sl4_string_compare( type, "bool" ) == 0 ) {
    col->type = INIFILE_TYPE_BOOL;
  } else if ( sl4_string_compare( type, "rights" ) == 0 ) {
    col->type = INIFILE_TYPE_RIGHTS;
  } else {
    strarray_t *matches = strarray_new(  );
    if ( myonig_match
         ( "/select\\(\\s*(.+)\\s*\\)/", sl4_string_get( type ), matches ) ) {
      sl4_string_t *m = strarray_get( matches, 1 );
      if ( m == NULL ) {
        ERROR_UNEXPECTED(  );
      }
      sl4_string_trim( m );
      col->type = INIFILE_TYPE_SELECT;
      col->options = strarray_new(  );
      if ( col->options == NULL ) {
        ERROR_OUTMEM(  );
      }
      pos = 0;
      tmp = sl4_string_tokenize( m, ",", &pos );
      while ( tmp ) {
        sl4_string_trim( tmp );
        strarray_add( col->options, tmp );
        tmp = sl4_string_tokenize( m, ",", &pos );
      }
    } else
      if ( myonig_match
           ( "/array\\(\\s*([^,]+)\\s*(,\\s*'(.+)')?\\s*\\)/",
             sl4_string_get( type ), matches ) ) {
      sl4_string_t *tag, *sep;
      tag = strarray_get( matches, 1 );
      if ( tag == NULL ) {
        ERROR_UNEXPECTED(  );
      }
      sep = strarray_get( matches, 3 );
      col->type = INIFILE_TYPE_ARRAY;
      col->options = strarray_new(  );
      if ( col->options == NULL ) {
        ERROR_OUTMEM(  );
      }
      sl4_string_trim( tag );
      tmp = sl4_string_new( sl4_string_get( tag ) );
      if ( tmp == NULL ) {
        ERROR_OUTMEM(  );
      }
      strarray_add( col->options, tmp );
      if ( sep == NULL ) {
        tmp = sl4_string_new( "\\n" );
      } else {
        tmp = sl4_string_new( sl4_string_get( sep ) );
      }
      if ( tmp == NULL ) {
        ERROR_OUTMEM(  );
      }
      sl4_string_convert_format( tmp );
      strarray_add( col->options, tmp );
    } else
      if ( myonig_match
           ( "/dataset\\(\\s*([^,]+)\\s*(,\\s*'(.+)')?\\s*\\)/",
             sl4_string_get( type ), matches ) ) {
      sl4_string_t *tag, *sep;
      tag = strarray_get( matches, 1 );
      if ( tag == NULL ) {
        ERROR_UNEXPECTED(  );
      }
      sep = strarray_get( matches, 3 );
      col->type = INIFILE_TYPE_DATASET;
      col->options = strarray_new(  );
      if ( col->options == NULL ) {
        ERROR_OUTMEM(  );
      }
      sl4_string_trim( tag );
      tmp = sl4_string_new( sl4_string_get( tag ) );
      if ( tmp == NULL ) {
        ERROR_OUTMEM(  );
      }
      strarray_add( col->options, tmp );
      if ( sep == NULL ) {
        tmp = sl4_string_new( "\\n" );
      } else {
        tmp = sl4_string_new( sl4_string_get( sep ) );
      }
      if ( tmp == NULL ) {
        ERROR_OUTMEM(  );
      }
      sl4_string_convert_format( tmp );
      strarray_add( col->options, tmp );
    } else
      if ( myonig_match
           ( "/file\\(\\s*(.+)\\s*\\)/", sl4_string_get( type ), matches ) ) {
      sl4_string_t *m = strarray_get( matches, 1 );
      if ( m == NULL ) {
        ERROR_UNEXPECTED(  );
      }
      sl4_string_trim( m );
      col->type = INIFILE_TYPE_FILE;
      col->options = strarray_new(  );
      if ( col->options == NULL ) {
        ERROR_OUTMEM(  );
      }
      tmp = sl4_string_new( sl4_string_get( m ) );
      if ( tmp == NULL ) {
        ERROR_OUTMEM(  );
      }
      strarray_add( col->options, tmp );
    } else {
      strarray_delete( matches );
      _inifile_column_delete( col );
      sl4_string_sprintf( errmes,
                          "Invalid data type found in %s.ini line %d.",
                          itemtype, line );
      return NULL;
    }
    strarray_delete( matches );
  }
  /* is_required */
  sl4_string_trim( optional );
  if ( sl4_string_compare( optional, "required" ) == 0 ) {
    col->is_required = 1;
  } else if ( sl4_string_compare( optional, "optional" ) == 0 ) {
    col->is_required = 0;
  } else {
    _inifile_column_delete( col );
    sl4_string_sprintf( errmes,
                        "Unknown requirement option found in %s.ini line %d.",
                        itemtype, line );
    return NULL;
  }
  return col;
}

static int _inifile_column_delete( inifile_column_t * col )
{
  strarray_delete( col->field );
  if ( col->options != NULL ) {
    strarray_delete( col->options );
  }
  free( col );
  return 0;
}
