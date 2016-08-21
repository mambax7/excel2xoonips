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

#include "libsl4.h"
#include "cexcel.h"
#include "excel2xoonips.h"

#define BUFSIZE 8192

/* TODO : filename coding system */
/* TODO : cleanup */

static int _template_assign_fields( sl4_hash_table_t *hash,
                                    sl4_array_t *field, sl4_string_t *str,
                                    int do_escape, const char *postfix );

template_t *template_new( const char *tmpldir, const char *itemtype )
{
  template_t *tmpl;
  sl4_string_t *filepath;
  sl4_file_t *file;
  char buf[BUFSIZE];
  size_t len;

  tmpl = ( template_t * ) malloc( sizeof( template_t ) );
  if ( tmpl == NULL ) {
    ERROR_OUTMEM(  );
  }
  tmpl->itemtype = sl4_string_new( itemtype );
  if ( tmpl->itemtype == NULL ) {
    ERROR_OUTMEM(  );
  }
  tmpl->data = sl4_string_new( NULL );
  if ( tmpl->data == NULL ) {
    ERROR_OUTMEM(  );
  }
  // create template file path
  filepath = sl4_string_new( tmpldir );
  if ( filepath == NULL ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_string_append( filepath, "/" ) != 0 ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_string_append( filepath, itemtype ) != 0 ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_string_append( filepath, ".tmpl" ) != 0 ) {
    ERROR_OUTMEM(  );
  }
  file = sl4_file_new( sl4_string_get( filepath ) );
  if ( file == NULL ) {
    ERROR_OUTMEM(  );
  }
  if ( sl4_file_open( file, "r" ) != 0 ) {
    sl4_file_delete( file );
    return NULL;
  }
  while ( !sl4_file_eof( file ) ) {
    len = sl4_file_read( file, buf, BUFSIZE - 1 );
    if ( len != 0 ) {
      buf[len] = '\0';
      if ( sl4_string_append( tmpl->data, buf ) != 0 ) {
        ERROR_OUTMEM(  );
      }
    }
  }
  sl4_file_close( file );
  sl4_file_delete( file );
  return tmpl;
}

int template_delete( template_t * tmpl )
{
  sl4_string_delete( tmpl->itemtype );
  sl4_string_delete( tmpl->data );
  free( tmpl );
  return 0;
}

int template_assign( template_t * tmpl, cexcel_sheet_t * sheet,
                     inifile_t * ini, sl4_string_t *tmpdir,
                     unsigned int row, int item_id, sl4_string_t *errmes,
                     int is_rowset )
{
  unsigned int col;
  cexcel_cell_t *cell;
  const char *address, *text, *sname;
  sl4_hash_table_t *rep_hash, *dataset_hash;
  inifile_column_t *inicol;
  char tmp_str1[128];

  char *item_xml;

  sname = cexcel_sheet_get_name( sheet );

  rep_hash = sl4_hash_table_str_new(  );
  dataset_hash = sl4_hash_table_str_new(  );
  if ( rep_hash == NULL || dataset_hash == NULL ) {
    ERROR_OUTMEM(  );
  }

  /* system.item_id */
  {
    sl4_string_t *itemid_str;
    itemid_str = sl4_string_new( NULL );
    if ( itemid_str == NULL ) {
      ERROR_OUTMEM(  );
    }
    if ( sl4_string_sprintf( itemid_str, "%d", item_id ) != 0 ) {
      ERROR_OUTMEM(  );
    }
    sl4_hash_table_insert( rep_hash, "system.item_id", itemid_str );
  }
  /* system.now */
  {
    sl4_string_t *now_str;
    char outstr[32];
    time_t t;
    struct tm *tmp;
    t = time( NULL );
    tmp = gmtime( &t );
    strftime( outstr, 32, "%Y-%m-%dT%H:%M:%SZ", tmp );
    now_str = sl4_string_new( outstr );
    if ( now_str == NULL ) {
      ERROR_OUTMEM(  );
    }
    sl4_hash_table_insert( rep_hash, "system.now", now_str );
  }
  /* system.itemtype */
  {
    sl4_string_t *itemtype_str;
    if ( !is_compat ) {
      strcpy(tmp_str1, sname);
      tmp_str1[0] = toupper(tmp_str1[0]);
      itemtype_str = sl4_string_new( tmp_str1 );
    } else {
      itemtype_str = sl4_string_new( sname );
    }
    if ( itemtype_str == NULL ) {
      ERROR_OUTMEM(  );
    }
    sl4_hash_table_insert( rep_hash, "system.itemtype", itemtype_str );
  }
  for ( col = 0; col < ini->ncol; col++ ) {
    cell = cexcel_sheet_get_cell( sheet, row, col );
    if ( cell == NULL ) {
      sl4_string_sprintf( errmes, "Cell(%d,%d) Not Found in Sheet \"%s\"",
                          row, col, sname );
      return -1;
    }
    inicol = ini->col[col];
    text = cexcel_cell_get_text( cell );
    address = cexcel_cell_get_address( cell );
    if ( inicol->is_required ) {
      if ( sl4_string_empty( cell->text ) ) {
        sl4_string_sprintf( errmes,
                            "Empty Cell(%s) Found in Sheet \"%s\". This is Requirement Field.",
                            address, sname );
        return -1;
      }
    }
    if ( !is_compat ) {
        item_xml = strchr(sl4_string_get(strarray_get(inicol->field, 0)), '.');
        if (item_xml == NULL) {
            item_xml = sl4_string_get(strarray_get(inicol->field, 0));
        } else {
            item_xml++;
        }
    }
    switch ( inicol->type ) {
    case INIFILE_TYPE_BOOL:
      {
        sl4_string_t *bool_str;
        bool_str = tmpl_bool_get( text, inicol->is_required, item_xml );
        if ( bool_str == NULL ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". Data Type is Not Boolean.",
                              address, sname );
          return -1;
        }
        _template_assign_fields( rep_hash, inicol->field, bool_str, 0, NULL );
        sl4_string_delete( bool_str );
      }
      break;
    case INIFILE_TYPE_INTEGER:
      {
        sl4_string_t *integer_str;
        integer_str = tmpl_integer_get( text, inicol->is_required, item_xml );
        if ( integer_str == NULL ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". Data Type is Not Integer.",
                              address, sname );
          return -1;
        }
        _template_assign_fields( rep_hash, inicol->field, integer_str, 0,
                                 NULL );
        sl4_string_delete( integer_str );
      }
      break;
    case INIFILE_TYPE_STRING:
      {
        sl4_string_t *string_str;
        string_str = tmpl_string_get( text, inicol->is_required, item_xml );
        if ( string_str == NULL ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". Data is Not Selectable Name.",
                              address, sname );
          return -1;
        }
        if ( is_compat ) {
            _template_assign_fields( rep_hash, inicol->field, string_str, 1,
                                     NULL );
        } else {
            _template_assign_fields( rep_hash, inicol->field, string_str, 0,
                                     NULL );
        }

        sl4_string_delete( string_str );
      }
      break;
    case INIFILE_TYPE_FILE:
      {
        sl4_string_t *file_str;
        sl4_file_t *file;
        sl4_string_t *file_name_inzip, *original_file_name, *mime_type,
          *file_size, *suffix;
        file_str =
          tmpl_file_get( text, inicol->options, item_id, sname, address,
                         &file, &file_name_inzip, &original_file_name,
                         &mime_type, &file_size, &suffix,
                         inicol->is_required, tmpdir, item_xml );

        if ( file_str == NULL ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". File Not Found or not allow duplicate.",
                              address, sname );
          return -1;
        }

        if ( sl4_string_empty( file_str ) == 0 ) {
          if ( is_compat ) {
              /* copy file to temporary directory */
              if ( tempdir_copyfile
                   ( tmpdir, sl4_file_get_path( file ),
                     sl4_string_get( file_name_inzip ) ) != 0 ) {
                ERROR_UNEXPECTED(  );
              }
          }
          sl4_string_delete( file_name_inzip );
          sl4_file_delete( file );
        } else {
          original_file_name = sl4_string_new( NULL );
          mime_type = sl4_string_new( NULL );
          file_size = sl4_string_new( NULL );
          suffix = sl4_string_new( NULL );
          if ( original_file_name == NULL || mime_type == NULL
               || file_size == NULL || suffix == NULL ) {
            ERROR_OUTMEM(  );
          }
        }
        _template_assign_fields( rep_hash, inicol->field, file_str, 0, NULL );
        _template_assign_fields( rep_hash, inicol->field, original_file_name,
                                 0, ".original_file_name" );
        _template_assign_fields( rep_hash, inicol->field, mime_type, 0,
                                 ".mime_type" );
        _template_assign_fields( rep_hash, inicol->field, file_size, 0,
                                 ".file_size" );
        _template_assign_fields( rep_hash, inicol->field, suffix, 0,
                                 ".suffix" );
        sl4_string_delete( file_str );
        sl4_string_delete( original_file_name );
        sl4_string_delete( mime_type );
        sl4_string_delete( file_size );
        sl4_string_delete( suffix );
      }
      break;
    case INIFILE_TYPE_SELECT:
      {
        sl4_string_t *select_str;
        select_str =
          tmpl_select_get( text, inicol->options, inicol->is_required, item_xml );
        if ( select_str == NULL ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". Data Type Mismatch.",
                              address, sname );
          return -1;
        }
        if ( is_compat ) {
            _template_assign_fields( rep_hash, inicol->field, select_str, 1,
                                     NULL );
        } else {
            _template_assign_fields( rep_hash, inicol->field, select_str, 0,
                                     NULL );
        }
        sl4_string_delete( select_str );
      }
      break;
    case INIFILE_TYPE_ARRAY:
      {
        sl4_string_t *array_str;
        array_str =
          tmpl_array_get( text, inicol->options, inicol->is_required, item_xml );
        if ( array_str == NULL ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". Data Type Mismatch.",
                              address, sname );
          return -1;
        }
        _template_assign_fields( rep_hash, inicol->field, array_str, 0,
                                 NULL );
        sl4_string_delete( array_str );
      }
      break;
    case INIFILE_TYPE_DATASET:
      {
        strarray_t *dataset_data, *dataset_field;
        sl4_string_t *dataset_name, *tmp;
        size_t *dataset_size;
        dataset_field = strarray_new(  );
        dataset_name = strarray_get( inicol->options, 0 );
        tmp = sl4_string_new( sl4_string_get( dataset_name ) );
        if ( dataset_field == NULL || tmp == NULL ) {
          ERROR_OUTMEM(  );
        }
        strarray_add( dataset_field, tmp );
        dataset_data =
          tmpl_dataset_get( text, inicol->options, inicol->is_required );
        if ( dataset_data == NULL ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". Data Type Mismatch.",
                              address, sname );
          return -1;
        }
        // if dataset hash does not exists, create a new content.
        dataset_size =
          ( size_t * ) sl4_hash_table_lookup( dataset_hash,
                                              sl4_string_get
                                              ( dataset_name ) );
        if ( dataset_size == NULL ) {
          dataset_size = ( size_t * ) malloc( sizeof( size_t ) );
          if ( dataset_size == NULL ) {
            ERROR_OUTMEM(  );
          }
          *dataset_size = strarray_length( dataset_data );
          sl4_hash_table_insert( dataset_hash, sl4_string_get( dataset_name ),
                                 dataset_size );
        }
        // compare number of content in dataset array.
        if ( *dataset_size != strarray_length( dataset_data ) ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". Array Size Mismatch.",
                              address, sname );
          return -1;
        }
        {
          size_t ii, jj;
          sl4_string_t *postfix = sl4_string_new( NULL );
          if ( postfix == NULL ) {
            ERROR_OUTMEM(  );
          }
          for ( ii = 0; ii < strarray_length( dataset_data ); ii++ ) {
            tmp = strarray_get( dataset_data, ii );
            for ( jj = 0; jj < strarray_length( inicol->field ); jj++ ) {
              sl4_string_sprintf( postfix, ".%s#%d",
                                  sl4_string_get( strarray_get
                                                  ( inicol->field, jj ) ),
                                  ii );
              _template_assign_fields( rep_hash, dataset_field, tmp, 1,
                                       sl4_string_get( postfix ) );
            }
          }
          sl4_string_delete( postfix );
        }
        strarray_delete( dataset_field );
        strarray_delete( dataset_data );
      }
      break;
    case INIFILE_TYPE_RIGHTS:
      {
        sl4_string_t *rights, *use_cc, *cc_commercial_use, *cc_modification;
        rights =
          tmpl_rights_get( text, &use_cc, &cc_commercial_use,
                           &cc_modification, inicol->is_required, item_xml );
        if ( rights == NULL ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". Data Type Mismatch.",
                              address, sname );
          return -1;
        }
        if ( is_compat ) {
          _template_assign_fields( rep_hash, inicol->field, rights, 1, NULL );
        } else {
          _template_assign_fields( rep_hash, inicol->field, rights, 0, NULL );
        }
        _template_assign_fields( rep_hash, inicol->field, use_cc, 0,
                                 ".use_cc" );
        _template_assign_fields( rep_hash, inicol->field, cc_commercial_use,
                                 0, ".cc_commercial_use" );
        _template_assign_fields( rep_hash, inicol->field, cc_modification, 0,
                                 ".cc_modification" );
        sl4_string_delete( rights );
        sl4_string_delete( use_cc );
        sl4_string_delete( cc_commercial_use );
        sl4_string_delete( cc_modification );
      }
      break;
    case INIFILE_TYPE_INDEXES:
      {
        sl4_string_t *indexes_str;
        indexes_str = tmpl_indexes_get( text, inicol->is_required, item_xml );
        if ( indexes_str == NULL ) {
          sl4_string_sprintf( errmes,
                              "Invalid Cell(%s) Found in Sheet \"%s\". Data Type Mismatch.",
                              address, sname );
          return -1;
        }
        _template_assign_fields( rep_hash, inicol->field, indexes_str, 0,
                                 NULL );
        sl4_string_delete( indexes_str );
      }
      break;
    default:
      ERROR_UNEXPECTED(  );
      break;
    }
  }
  {
    /* dump replacing hash data */
    size_t bi;
    sl4_hash_bucket_t *buck;
    sl4_string_t *mydata, *mypattern, *tmp, *data;
    const char *mykey;
    mypattern = sl4_string_new( NULL );
    if ( mypattern == NULL ) {
      ERROR_OUTMEM(  );
    }
    data = sl4_string_new( sl4_string_get( tmpl->data ) );
    if ( data == NULL ) {
      ERROR_OUTMEM(  );
    }
    /* preprocess for rowset */
    if ( is_rowset ) {
      strarray_t *matches = strarray_new(  );
      if ( myonig_match
           ( "/<{\\s*rowset\\s*}>(.*?)<{\\s*\\/rowset\\s*}>/s",
             sl4_string_get( data ), matches ) != 0 ) {
        sl4_string_t *body = strarray_get( matches, 1 );
        sl4_string_set( data, sl4_string_get( body ) );
      }
      strarray_delete( matches );
    }
    /* preprocess for dataset */
    {
      strarray_t *matches = strarray_new(  );
      while ( myonig_match
              ( "/<{\\s*dataset\\s*\\$([^}]+)\\s*}>(.*?)<{\\s*\\/dataset\\s*}>/s",
                sl4_string_get( data ), matches ) != 0 ) {
        sl4_string_t *all, *name, *body, *rep_body;
        size_t ii, pos, *dataset_size;
        all = strarray_get( matches, 0 );
        name = strarray_get( matches, 1 );
        body = strarray_get( matches, 2 );
        rep_body = sl4_string_new( NULL );
        dataset_size =
          ( size_t * ) sl4_hash_table_lookup( dataset_hash,
                                              sl4_string_get( name ) );
        for ( ii = 0; ii < *dataset_size; ii++ ) {
          sl4_string_t *rep_name, *tmp_body1, *tmp_body2;
          rep_name = sl4_string_new( NULL );
          sl4_string_sprintf( rep_name, "<{@@%s.\\1#%d}>",
                              sl4_string_get( name ), ii );
          tmp_body1 =
            myonig_replace( "/<{\\s*\\$(.+?)\\s*}>/",
                            sl4_string_get( rep_name ),
                            sl4_string_get( body ) );
          if ( tmp_body1 == NULL ) {
            ERROR_UNEXPECTED(  );
          }
          tmp_body2 =
            myonig_replace( "/<{@@(.+?)}>/", "<{\\$\\1}>",
                            sl4_string_get( tmp_body1 ) );
          if ( tmp_body2 == NULL ) {
            ERROR_UNEXPECTED(  );
          }
          sl4_string_append( rep_body, sl4_string_get( tmp_body2 ) );
          sl4_string_delete( tmp_body1 );
          sl4_string_delete( tmp_body2 );
          sl4_string_delete( rep_name );
        }
        pos = sl4_string_find( data, sl4_string_get( all ) );
        sl4_string_replace( data, pos, sl4_string_length( all ),
                            sl4_string_get( rep_body ) );
      }
      strarray_delete( matches );
    }
    /* replace */
    if ( rep_hash->nkeys != 0 ) {
      for ( bi = 0; bi < rep_hash->nbucks; bi++ ) {
        for ( buck = rep_hash->buckets[bi]; buck; buck = buck->next ) {
          mydata = ( sl4_string_t * ) buck->data;
          mykey = ( const char * ) buck->key;
          sl4_string_sprintf( mypattern, "/<{\\$%s}>/", mykey );
          tmp =
            myonig_replace( sl4_string_get( mypattern ),
                            sl4_string_get( mydata ),
                            sl4_string_get( data ) );
          if ( tmp == NULL ) {
            ERROR_UNEXPECTED(  );
          }
          sl4_string_delete( data );
          data = tmp;
          /* free hash data */
          sl4_string_delete( mydata );
        }
      }
    }
    {
      /* output xml file */
      sl4_file_t *xml_file;
      sl4_string_t *xml_fname;
      xml_fname = sl4_string_new( NULL );
      sl4_string_sprintf( xml_fname, "/%d.xml", item_id );
      xml_file = tempdir_fopen( tmpdir, sl4_string_get( xml_fname ), "w" );
      sl4_file_write( xml_file, sl4_string_get( data ),
                      sl4_string_length( data ) );
      sl4_file_close( xml_file );
      sl4_string_delete( xml_fname );
      sl4_file_delete( xml_file );
    }
    sl4_string_delete( data );
  }
  /* free hash box */
  sl4_hash_table_delete( rep_hash, NULL, NULL );
  {
    size_t bi, *mem;
    sl4_hash_bucket_t *buck;
    if ( dataset_hash->nkeys != 0 ) {
      for ( bi = 0; bi < dataset_hash->nbucks; bi++ ) {
        for ( buck = dataset_hash->buckets[bi]; buck; buck = buck->next ) {
          mem = ( size_t * ) buck->data;
          free( mem );
        }
      }
    }
  }
  sl4_hash_table_delete( dataset_hash, NULL, NULL );
  return 0;
}

int template_merge_rowset( template_t * tmpl, sl4_string_t *tempdir, int sid,
                           int eid, const char *name )
{
  sl4_string_t *xml_fname, *body, *data;
  sl4_file_t *xml_file;
  size_t siz, blen;
  int i;
  char buf[8196];
  blen = 8195;
  if ( sid > eid ) {
    return -1;
  }
  body = sl4_string_new( NULL );
  xml_fname = sl4_string_new( NULL );
  for ( i = sid; i < eid; i++ ) {
    sl4_string_sprintf( xml_fname, "/%d.xml", i );
    xml_file = tempdir_fopen( tempdir, sl4_string_get( xml_fname ), "r" );
    while ( sl4_file_eof( xml_file ) == 0 ) {
      siz = sl4_file_read( xml_file, buf, blen );
      if ( siz > 0 ) {
        buf[siz] = '\0';
        sl4_string_append( body, buf );
      }
    }
    sl4_file_close( xml_file );
    sl4_file_unlink( xml_file );
    sl4_file_delete( xml_file );
  }
  data = myonig_replace( "/<{\\s*rowset\\s*}>.*?<{\\s*\\/rowset\\s*}>/s",
                         sl4_string_get( body ),
                         sl4_string_get( tmpl->data ) );

  /* output xml file */
  sl4_string_sprintf( xml_fname, "/%s.xml", name );
  xml_file = tempdir_fopen( tempdir, sl4_string_get( xml_fname ), "w" );
  sl4_file_write( xml_file, sl4_string_get( data ),
                  sl4_string_length( data ) );
  sl4_file_close( xml_file );
  sl4_string_delete( xml_fname );
  sl4_file_delete( xml_file );
  return 0;
}

static int _template_assign_fields( sl4_hash_table_t *hash,
                                    sl4_array_t *field, sl4_string_t *str,
                                    int do_escape, const char *postfix )
{
  size_t pos, flen;
  sl4_string_t *key, *tmp_key, *tmp_str;
  flen = strarray_length( field );
  for ( pos = 0; pos < flen; pos++ ) {
    tmp_str = sl4_string_new( sl4_string_get( str ) );
    if ( tmp_str == NULL ) {
      ERROR_OUTMEM(  );
    }
    if ( do_escape != 0 ) {
      sl4_string_htmlspecialchars( tmp_str );
    }
    key = strarray_get( field, pos );
    if ( postfix != NULL ) {
      tmp_key = sl4_string_new( sl4_string_get( key ) );
      if ( tmp_key == NULL ) {
        ERROR_OUTMEM(  );
      }
      if ( sl4_string_append( tmp_key, postfix ) != 0 ) {
        ERROR_OUTMEM(  );
      }
      sl4_hash_table_insert( hash, sl4_string_get( tmp_key ), tmp_str );
      sl4_string_delete( tmp_key );
    } else {
      sl4_hash_table_insert( hash, sl4_string_get( key ), tmp_str );
    }
  }
  return 0;
}
