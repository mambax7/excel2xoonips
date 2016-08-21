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

#ifndef __EXCEL2XOONIPS_H__
#define __EXCEL2XOONIPS_H__

/* error.c */
typedef enum {
  ERROR_CODE_OUTMEM,
  ERROR_CODE_UNEXPECTED,
} ERROR_CODE;
void error_internal( ERROR_CODE errcode, const char *file, int line );
#define ERROR_OUTMEM()    error_internal( ERROR_CODE_OUTMEM, __FILE__, __LINE__ )
#define ERROR_UNEXPECTED()    error_internal( ERROR_CODE_UNEXPECTED, __FILE__, __LINE__ )

int is_compat;
int files_count;

/* strarray.c */
typedef sl4_array_t strarray_t;
strarray_t *strarray_new(  );
int strarray_delete( strarray_t * arr );
sl4_string_t *strarray_get( strarray_t * arr, size_t pos );
int strarray_add( strarray_t * arr, sl4_string_t *str );
size_t strarray_length( strarray_t * arr );
int strarray_clear( strarray_t * arr );
strarray_t *strarray_explode( sl4_string_t *sep, sl4_string_t *str );

/* recursive.c */
typedef int ( *recursive_func_t ) ( const char *, int, void * );
int recursive_proc( const char *path, recursive_func_t func, void *data );

/* directory.c */
int directory_remove( const char *path );
sl4_list_t *directory_filelist( const char *path );

/* minizip.c */
int minizip_create( const char *filename, const char *rootdir );

/* onig.c */
int myonig_match( const char *pattern, const char *str,
                  strarray_t * matches );
sl4_string_t *myonig_replace( const char *pattern, const char *replacement,
                              const char *str );

/* inifile.c */
#define INIFILE_MAX_LINE 8192
#define INIFILE_MAX_COLUMNS 128
typedef enum {
  INIFILE_TYPE_UNDEF,
  INIFILE_TYPE_BOOL,
  INIFILE_TYPE_INTEGER,
  INIFILE_TYPE_STRING,
  INIFILE_TYPE_ARRAY,
  INIFILE_TYPE_DATASET,
  INIFILE_TYPE_FILE,
  INIFILE_TYPE_SELECT,
  INIFILE_TYPE_RIGHTS,
  INIFILE_TYPE_INDEXES,
} INIFILE_TYPE;
typedef struct _inifile_column_t {
  strarray_t *field;
  INIFILE_TYPE type;
  int is_required;
  strarray_t *options;
} inifile_column_t;
typedef struct _inifile_t {
  sl4_string_t *inifile_path;
  int ncol;
  inifile_column_t *col[INIFILE_MAX_COLUMNS];
} inifile_t;

inifile_t *inifile_load( const char *inidir, const char *itemtype,
                         sl4_string_t *errmes );
int inifile_delete( inifile_t * ini );

/* bsd_mktemp.c */
int bsd_mkstemp( char *path );
char *bsd_mkdtemp( char *path );

/* tempdir.c */
sl4_string_t *tempdir_new(  );
int tempdir_delete( sl4_string_t *tempdir );
int tempdir_mkdir( sl4_string_t *tempdir, const char *dir );
sl4_file_t *tempdir_fopen( sl4_string_t *tempdir, const char *vpath,
                           const char *mode );
int tempdir_fclose( sl4_file_t *file );
int tempdir_copyfile( sl4_string_t *tempdir, const char *spath,
                      const char *dvpath );

/* utilitiy.c */
const char *get_bin_dirname( const char *bin );
int sl4_string_htmlspecialchars( sl4_string_t *str );
int sl4_string_tolower( sl4_string_t *str );
int sl4_string_replace2( sl4_string_t *str, const char *ptn,
                         const char *rep );
int sl4_string_convert_format( sl4_string_t *str );
sl4_string_t *sl4_file_get_mimetype( sl4_file_t *file,
                                     sl4_string_t **suffix );

/* tmpl_bool.c */
sl4_string_t *tmpl_bool_get( const char *text, int is_required, char *item_xml );

/* tmpl_integer.c */
sl4_string_t *tmpl_integer_get( const char *text, int is_required, char *item_xml );

/* tmpl_string.c */
sl4_string_t *tmpl_string_get( const char *text, int is_required, char *item_xml );

/* tmpl_select.c */
sl4_string_t *tmpl_select_get( const char *text, strarray_t * options,
                               int is_required, char *item_xml );

/* tmpl_array.c */
sl4_string_t *tmpl_array_get( const char *text, strarray_t * options,
                              int is_required, char *item_xml );

/* tmpl_dataset.c */
strarray_t *tmpl_dataset_get( const char *text, strarray_t * options,
                              int is_required );

/* tmpl_file.c */
sl4_string_t *tmpl_file_get( const char *text, strarray_t * options,
                             int item_id, const char *sname,
                             const char *address, sl4_file_t **file,
                             sl4_string_t **file_name_inzip,
                             sl4_string_t **original_file_name,
                             sl4_string_t **mime_type,
                             sl4_string_t **file_size,
                             sl4_string_t **suffux, int is_required,
                             sl4_string_t *tmpdir, char *item_xml );

/* tmpl_rights.c */
sl4_string_t *tmpl_rights_get( const char *text, sl4_string_t **use_cc,
                               sl4_string_t **cc_commercial_use,
                               sl4_string_t **cc_modification,
                               int is_required, char *item_xml );

/* tmpl_indexes.c */
sl4_string_t *tmpl_indexes_get( const char *text, int is_required, char *item_xml );

/* template.c */
typedef struct _template_t {
  sl4_string_t *itemtype;
  sl4_string_t *data;
} template_t;
template_t *template_new( const char *tmpldir, const char *itemtype );
int template_delete( template_t * tmpl );
int template_assign( template_t * tmpl, cexcel_sheet_t * sheet,
                     inifile_t * ini, sl4_string_t *tmpdir,
                     unsigned int row, int item_id, sl4_string_t *errmes,
                     int is_rowset );
int template_merge_rowset( template_t *tmpl, sl4_string_t *tempdir, int sid, int eid, const char *name );


#endif /* __EXCEL2XOONIPS_H__ */
