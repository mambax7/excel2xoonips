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

/* $Id: libsl4.h,v 1.2 2008/01/24 14:59:41 orrisroot Exp $ */

#ifndef __LIBSL4_H__
#define __LIBSL4_H__

#include <sys/types.h>

#if defined(LIBSL4_STATICLIB)
# define LIBSL4_API
#else
# if defined(WIN32) || defined(__CYGWIN__)
#  ifdef LIBSL4_EXPORTS
#   define LIBSL4_API __declspec(dllexport)
#  else
#   define LIBSL4_API __declspec(dllimport)
#  endif
# else
#  define LIBSL4_API
# endif
#endif

#ifdef __cplusplus
# define LIBSL4_BEGIN_DECLS extern "C" {
# define LIBSL4_END_DECLS }
#else
# define LIBSL4_BEGIN_DECLS
# define LIBSL4_END_DECLS
#endif

LIBSL4_BEGIN_DECLS
/* array.c */
  typedef struct _sl4_array_t {
  void *data;
  size_t bsize;
  size_t len;
} sl4_array_t;

LIBSL4_API sl4_array_t *sl4_array_new( size_t bsize );
LIBSL4_API void sl4_array_delete( sl4_array_t *arr );
LIBSL4_API int sl4_array_add( sl4_array_t *arr, void *val );
LIBSL4_API int sl4_array_clear( sl4_array_t *arr );
LIBSL4_API int sl4_array_empty( sl4_array_t *arr );
LIBSL4_API int sl4_array_erase( sl4_array_t *arr, size_t pos );
LIBSL4_API void *sl4_array_get( sl4_array_t *arr, size_t pos );
LIBSL4_API size_t sl4_array_length( sl4_array_t *arr );
LIBSL4_API int sl4_array_resize( sl4_array_t *arr, size_t len );
LIBSL4_API int sl4_array_set( sl4_array_t *arr, size_t pos, void *val );
#define sl4_array_index( arr, t, pos ) *(t*)sl4_array_get((arr),(pos))

/* env.c */
LIBSL4_API char *sl4_getenv( const char *env );
LIBSL4_API int sl4_unsetenv( const char *env );
LIBSL4_API int sl4_setenv( const char *env, const char *val );

/* error.c */
LIBSL4_API void sl4_error_fatal( const char *mes, const char *fname,
                                 int line );

/* file.c */
typedef enum _sl4_file_flag_enum {
  SL4_FILE_FLAG_NONE = 0x00,
  SL4_FILE_FLAG_OPEN = 0x01,
  SL4_FILE_FLAG_ROOT = 0x02,
} sl4_file_flag_enum;

typedef enum _sl4_file_seek_enum {
  SL4_FILE_SEEK_SET = 0,
  SL4_FILE_SEEK_CUR = 1,
  SL4_FILE_SEEK_END = 2
} sl4_file_seek_enum;

typedef struct _sl4_file_t {
  sl4_file_flag_enum flag;
  void *fp;
  char *path;
  const char *name;
} sl4_file_t;

LIBSL4_API sl4_file_t *sl4_file_new( const char *path );
LIBSL4_API int sl4_file_delete( sl4_file_t *file );
LIBSL4_API int sl4_file_chdir( sl4_file_t *file );
LIBSL4_API int sl4_file_close( sl4_file_t *file );
LIBSL4_API int sl4_file_eof( sl4_file_t *file );
LIBSL4_API int sl4_file_exist( sl4_file_t *file );
LIBSL4_API int sl4_file_flush( sl4_file_t *file );
LIBSL4_API int sl4_file_gets( sl4_file_t *file, char *buf, int size );
LIBSL4_API sl4_file_t *sl4_file_get_cwd(  );
LIBSL4_API const char *sl4_file_get_name( sl4_file_t *file );
LIBSL4_API const char *sl4_file_get_path( sl4_file_t *file );
LIBSL4_API off_t sl4_file_get_size( sl4_file_t *file );
LIBSL4_API const char *sl4_file_get_separator(  );
LIBSL4_API int sl4_file_is_directory( sl4_file_t *file );
LIBSL4_API int sl4_file_is_executable( sl4_file_t *file );
LIBSL4_API int sl4_file_is_file( sl4_file_t *file );
LIBSL4_API int sl4_file_is_link( sl4_file_t *file );
LIBSL4_API int sl4_file_is_readable( sl4_file_t *file );
LIBSL4_API int sl4_file_is_socket( sl4_file_t *file );
LIBSL4_API int sl4_file_is_writable( sl4_file_t *file );
LIBSL4_API int sl4_file_mkdir( sl4_file_t *file );
LIBSL4_API int sl4_file_open( sl4_file_t *file, const char *mode );
LIBSL4_API size_t sl4_file_read( sl4_file_t *file, void *buf, size_t len );
LIBSL4_API int sl4_file_rmdir( sl4_file_t *file );
LIBSL4_API int sl4_file_seek( sl4_file_t *file, off_t offset,
                              sl4_file_seek_enum whence );
LIBSL4_API long sl4_file_tell( sl4_file_t *file );
LIBSL4_API mode_t sl4_file_umask( mode_t mode );
LIBSL4_API int sl4_file_unlink( sl4_file_t *file );
LIBSL4_API size_t sl4_file_write( sl4_file_t *file, const void *buf,
                                  size_t len );

/* hash.c */
typedef void *( *sl4_hash_key_copy_t ) ( const void *key );
typedef void ( *sl4_hash_key_free_t ) ( void *key );
typedef size_t ( *sl4_hash_func_t ) ( const void *key );
typedef int ( *sl4_hash_comp_t ) ( const void *key1, const void *key2 );
typedef void ( *sl4_hash_foreach_t ) ( void *data, void *arg );

typedef struct _sl4_hash_bucket_t {
  struct _sl4_hash_bucket_t *next;
  void *key;
  size_t val;
  void *data;
} sl4_hash_bucket_t;

typedef struct _sl4_hash_table_t {
  sl4_hash_bucket_t **buckets;
  size_t nbucks;
  size_t nkeys;
  int dyn_resize;
  sl4_hash_func_t hash_func;
  sl4_hash_comp_t hash_comp;
  sl4_hash_key_copy_t hash_key_copy;
  sl4_hash_key_free_t hash_key_free;
} sl4_hash_table_t;

LIBSL4_API sl4_hash_table_t *sl4_hash_table_new( sl4_hash_func_t hash_func,
                                                 sl4_hash_comp_t hash_comp,
                                                 sl4_hash_key_copy_t
                                                 hash_key_copy,
                                                 sl4_hash_key_free_t
                                                 hash_key_free );
LIBSL4_API void sl4_hash_table_delete( sl4_hash_table_t *table,
                                       sl4_hash_foreach_t hash_del,
                                       void *arg );
LIBSL4_API sl4_hash_table_t *sl4_hash_table_str_new(  );
LIBSL4_API sl4_hash_table_t *sl4_hash_table_int_new(  );
LIBSL4_API sl4_hash_table_t *sl4_hash_table_ptr_new(  );
LIBSL4_API void *sl4_hash_table_lookup( sl4_hash_table_t *table,
                                        const void *key );
LIBSL4_API void *sl4_hash_table_remove( sl4_hash_table_t *table,
                                        const void *key );
LIBSL4_API void sl4_hash_table_erase( sl4_hash_table_t *table,
                                      sl4_hash_foreach_t hash_del,
                                      void *arg );
LIBSL4_API int sl4_hash_table_insert( sl4_hash_table_t *table,
                                      const void *key, void *data );
LIBSL4_API void sl4_hash_table_foreach( sl4_hash_table_t *table,
                                        sl4_hash_foreach_t hash_foreach,
                                        void *arg );

/* list.c */
typedef struct _sl4_list_node_t {
  void *data;
  struct _sl4_list_node_t *next;
  struct _sl4_list_node_t *prev;
} sl4_list_node_t;

typedef struct _sl4_list_t {
  sl4_list_node_t *top;
  sl4_list_node_t *tail;
  size_t size;
} sl4_list_t;

typedef struct _sl4_list_iterator_t {
  int status;
  sl4_list_t *lst;
  sl4_list_node_t *cur;
} sl4_list_iterator_t;

/* if return value is 0 then no match data, else match data */
typedef int ( *sl4_list_eq_func_t ) ( void *, void * );
/* stack type */
typedef sl4_list_t sl4_stack_t;

#define SL4_LIST_ITERATOR_STAT_FINE 0
#define SL4_LIST_ITERATOR_STAT_END  1
#define SL4_LIST_ITERATOR_STAT_ILL  2

LIBSL4_API sl4_list_t *sl4_list_new(  );
LIBSL4_API void sl4_list_delete( sl4_list_t *lst );
#define sl4_list_empty(lst) (((lst)->size==0) ? 1: 0)
#define sl4_list_size(lst)  ((lst)->size)
#define sl4_list_top(lst)   (((lst)->top==NULL) ? NULL : (lst)->top->data)

LIBSL4_API int sl4_list_push_front( sl4_list_t *lst, void *dat );
LIBSL4_API int sl4_list_push_back( sl4_list_t *lst, void *dat );
LIBSL4_API int sl4_list_pop_front( sl4_list_t *lst );
LIBSL4_API int sl4_list_pop_back( sl4_list_t *lst );
LIBSL4_API int sl4_list_erase( sl4_list_t *lst, sl4_list_iterator_t *it );
LIBSL4_API int sl4_list_begin( sl4_list_t *lst, sl4_list_iterator_t *it );
LIBSL4_API int sl4_list_end( sl4_list_t *lst, sl4_list_iterator_t *it );
LIBSL4_API int sl4_list_find( sl4_list_t *lst, void *dat,
                              sl4_list_iterator_t *it,
                              sl4_list_eq_func_t func );
LIBSL4_API int sl4_list_rfind( sl4_list_t *lst, void *dat,
                               sl4_list_iterator_t *it,
                               sl4_list_eq_func_t func );
LIBSL4_API void sl4_list_it_next( sl4_list_iterator_t *it );
LIBSL4_API void sl4_list_it_prev( sl4_list_iterator_t *it );
LIBSL4_API void *sl4_list_it_data( sl4_list_iterator_t *it );
#define sl4_list_it_is_end(it)  \
                 (((it)->status == SL4_LIST_ITERATOR_STAT_END) ? 1: 0)
#define sl4_stack_new()         sl4_list_new()
#define sl4_stack_delete(st)    sl4_list_delete((st))
#define sl4_stack_empty(st)     (((st)->size==0) ? 1: 0)
#define sl4_stack_size(st)      ((st)->size)
#define sl4_stack_push(st, dat) sl4_list_push_front(st,dat)
#define sl4_stack_pop(st)       sl4_list_pop_front(st)
#define sl4_stack_top(st)       (((st)->top==NULL) ? NULL : (st)->top->data)

/* mutex.c */
typedef struct _sl4_mutex_t {
  void *mtx;
} sl4_mutex_t;

LIBSL4_API sl4_mutex_t *sl4_mutex_new(  );
LIBSL4_API void sl4_mutex_delete( sl4_mutex_t *mutex );
LIBSL4_API void sl4_mutex_lock( sl4_mutex_t *mutex );
LIBSL4_API void sl4_mutex_unlock( sl4_mutex_t *mutex );

/* string.c */
typedef struct _sl4_string_t {
  char *mem;
  size_t len;
  size_t bsize;
} sl4_string_t;

LIBSL4_API sl4_string_t *sl4_string_new( const char *cstr );
LIBSL4_API int sl4_string_delete( sl4_string_t *str );
LIBSL4_API int sl4_string_append( sl4_string_t *dst, const char *src );
LIBSL4_API int sl4_string_append_char( sl4_string_t *dst, char c );
LIBSL4_API int sl4_string_clear( sl4_string_t *str );
LIBSL4_API int sl4_string_compare( sl4_string_t *str, const char *cstr );
LIBSL4_API int sl4_string_empty( sl4_string_t *str );
LIBSL4_API int sl4_string_erase( sl4_string_t *str, size_t pos, size_t len );
LIBSL4_API size_t sl4_string_find( sl4_string_t *str, const char *target );
LIBSL4_API size_t sl4_string_findn( sl4_string_t *str, const char *target,
                                    size_t spos );
LIBSL4_API const char *sl4_string_get( sl4_string_t *str );
LIBSL4_API char sl4_string_get_at( sl4_string_t *str, size_t pos );
LIBSL4_API int sl4_string_insert( sl4_string_t *dst, size_t pos,
                                  const char *src );
LIBSL4_API int sl4_string_insert_char( sl4_string_t *dst, size_t pos,
                                       char c );
LIBSL4_API size_t sl4_string_length( sl4_string_t *str );
LIBSL4_API int sl4_string_ltrim( sl4_string_t *str );
LIBSL4_API size_t sl4_string_rfind( sl4_string_t *str, const char *target );
LIBSL4_API size_t sl4_string_rfindn( sl4_string_t *str, const char *target,
                                     size_t spos );
LIBSL4_API int sl4_string_replace( sl4_string_t *str, size_t pos,
                                   size_t len, const char *cstr );
LIBSL4_API int sl4_string_rtrim( sl4_string_t *str );
LIBSL4_API int sl4_string_set( sl4_string_t *dst, const char *src );
LIBSL4_API int sl4_string_sprintf( sl4_string_t *str, const char *fmt, ... );
LIBSL4_API sl4_string_t *sl4_string_substr( sl4_string_t *str, size_t pos,
                                            size_t len );
LIBSL4_API sl4_string_t *sl4_string_tokenize( sl4_string_t *str,
                                              const char *sep, size_t *pos );
LIBSL4_API int sl4_string_trim( sl4_string_t *str );

/* time.c */
LIBSL4_API int sl4_time_getdate( int *year, int *month, int *day );
LIBSL4_API int sl4_time_gettime( int *hour, int *minute, int *secound );

/* user.c */
LIBSL4_API const char *sl4_user_name(  );
LIBSL4_API const char *sl4_user_home_dir(  );
LIBSL4_API const char *sl4_user_resource_dir(  );
LIBSL4_API const char *sl4_user_temp_dir(  );

/* win32.c */
#ifdef WIN32
LIBSL4_API int sl4_win32_has_console(  );
LIBSL4_API int sl4_win32_path_getfull( const char *src, char *dst,
                                       size_t len );
LIBSL4_API const char *sl4_win32_path_mydocuments(  );
LIBSL4_API const char *sl4_win32_path_applicationdata(  );
#endif

LIBSL4_END_DECLS
#endif /* __LIBSL4_H__ */
