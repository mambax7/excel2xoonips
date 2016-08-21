/*
 * cexcel - A library to read data from Excel spread sheets
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

#ifndef __CEXCEL_INTERNAL_H__
#define __CEXCEL_INTERNAL_H__

CEXCEL_BEGIN_DECLS
/* for biff format debugging */
/* #define _CEXCEL_DEBUG_BIFF */
/* datasize */
#if SIZEOF_UNSIGNED_CHAR == 1
typedef unsigned char CEXCEL_UCHAR;
#else
# error 1 byte length type not found
#endif
#if SIZEOF_UNSIGNED_INT == 2
typedef unsigned int CEXCEL_WORD;
#elif SIZEOF_UNSIGNED_SHORT == 2
typedef unsigned short CEXCEL_WORD;
#elif SIZEOF_UNSIGNED_LONG == 2
typedef unsigned long CEXCEL_WORD;
#else
# error 2 byte length type not found
#endif
#if SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long CEXCEL_DWORD;
#elif SIZEOF_UNSIGNED_INT == 4
typedef unsigned int CEXCEL_DWORD;
#elif SIZEOF_UNSIGNED_LONG_LONG == 4
typedef unsigned long long CEXCEL_DWORD;
#else
# error 4 byte length type not found
#endif
#if SIZEOF_DOUBLE == 8
typedef double CEXCEL_DOUBLE;
#elif SIZEOF_FLOAT == 8
typedef float CEXCEL_DOUBLE;
#elif SIZEOF_LONG_DOULBE == 8
typedef long double CEXCEL_DOUBLE;
#else
# error 8 byte length float type not found
#endif
/* id */
#define CEXCEL_ID_BLANK2      0x0001
#define CEXCEL_ID_BLANK3458   0x0201
#define CEXCEL_ID_BOF2        0x0009
#define CEXCEL_ID_BOF3        0x0209
#define CEXCEL_ID_BOF4        0x0409
#define CEXCEL_ID_BOF58       0x0809
#define CEXCEL_ID_BOOLERR2    0x0005
#define CEXCEL_ID_BOOLERR3458 0x0205
#define CEXCEL_ID_BOUNDSHEET  0x0085
#define CEXCEL_ID_CODEPAGE    0x0042
#define CEXCEL_ID_CONTINUE    0x003c
#define CEXCEL_ID_EOF         0x000a
#define CEXCEL_ID_FILEPASS    0x002f
#define CEXCEL_ID_FORMAT23    0x001e
#define CEXCEL_ID_FORMAT458   0x041e
#define CEXCEL_ID_INTEGER     0x0002
#define CEXCEL_ID_LABEL2      0x0004
#define CEXCEL_ID_LABEL3458   0x0204
#define CEXCEL_ID_LABELSST    0x00fd
#define CEXCEL_ID_MULBLANK    0x00be
#define CEXCEL_ID_MULRK       0x00bd
#define CEXCEL_ID_NUMBER2     0x0003
#define CEXCEL_ID_NUMBER3458  0x0203
#define CEXCEL_ID_RK          0x027e
#define CEXCEL_ID_RSTRING     0x00d6
#define CEXCEL_ID_SST         0x00fc
#define CEXCEL_ID_XF2         0x0043
#define CEXCEL_ID_XF3         0x0243
#define CEXCEL_ID_XF4         0x0443
#define CEXCEL_ID_XF58        0x00e0
/* iconv.c */
const char *cexcel_iconv_codepage2charset( CEXCEL_WORD codepage );
void cexcel_iconv_macja2cp932( sl4_string_t *str );

/* hash.c */
sl4_hash_table_t *cexcel_hash_new(  );

/* file.c */
typedef enum _CEXCEL_FTYPE {
  CEXCEL_FTYPE_UNDEF,
  CEXCEL_FTYPE_NORMAL,
  CEXCEL_FTYPE_OLE
} CEXCEL_FTYPE;

typedef enum _CEXCEL_RETTYPE {
  CEXCEL_RETTYPE_SUCCESS,
  CEXCEL_RETTYPE_UNEXPECTED,
  CEXCEL_RETTYPE_OUTOFMEMORY,
  CEXCEL_RETTYPE_BADFILE,
  CEXCEL_RETTYPE_NOTIMPLEMENTED,
} CEXCEL_RETTYPE;

typedef struct _cexcel_file_ole_t {
  COLEFS *cfs;
  COLEFILE *cf;
} cexcel_file_ole_t;

typedef struct _cexcel_file_t {
  cexcel_file_ole_t ole;
  sl4_file_t *file;
  CEXCEL_FTYPE ftype;
} cexcel_file_t;

typedef CEXCEL_RETTYPE( *cexcel_file_func_t ) ( cexcel_file_t * file,
                                                void *info );

cexcel_file_t *cexcel_file_open( const char *filename );
int cexcel_file_close( cexcel_file_t * file );
CEXCEL_RETTYPE cexcel_file_parse( cexcel_file_t * file,
                                  cexcel_file_func_t func, void *info );

/* stream.c */
typedef enum _CEXCEL_SEEK_FLAG {
  CEXCEL_SEEK_SET,
  CEXCEL_SEEK_CUR,
  CEXCEL_SEEK_END,
} CEXCEL_SEEK_FLAG;

size_t cexcel_stream_read( cexcel_file_t * file, void *buf, size_t size );
size_t cexcel_stream_tell( cexcel_file_t * file );
int cexcel_stream_seek( cexcel_file_t * file, off_t offset,
                        CEXCEL_SEEK_FLAG whence );
int cexcel_stream_eof( cexcel_file_t * file );

/* xf.c */
typedef enum _CEXCEL_XFTYPE {
  CEXCEL_XFTYPE_UNDEF = -1,
  CEXCEL_XFTYPE_CELL = 0,
  CEXCEL_XFTYPE_STYLE = 1
} CEXCEL_XFTYPE;

typedef struct _cexcel_xf_t {
  CEXCEL_XFTYPE type;           /* xf type */
  CEXCEL_UCHAR attrib;          /* attibutes used from parent style xf */
  CEXCEL_DWORD idx_pstyle;      /* index of parent style xf */
  CEXCEL_DWORD idx_font;        /* index of font */
  CEXCEL_DWORD idx_format;      /* index of format */
} cexcel_xf_t;

cexcel_xf_t *cexcel_xf_new(  );
int cexcel_xf_delete( cexcel_xf_t * xf );

/* format.c */
sl4_string_t *cexcel_format_double( double data, sl4_string_t *format );

/* context.c */
typedef enum _CEXCEL_BTYPE {
  CEXCEL_BTYPE_UNDEF,
  CEXCEL_BTYPE_BIFF2 = 2,
  CEXCEL_BTYPE_BIFF3,
  CEXCEL_BTYPE_BIFF4,
  CEXCEL_BTYPE_BIFF5,
  CEXCEL_BTYPE_BIFF8 = 8
} CEXCEL_BTYPE;

typedef struct _cexcel_context_t {
  CEXCEL_BTYPE global_btype;
  CEXCEL_BTYPE sheet_btype;
  CEXCEL_WORD codepage;
  cexcel_file_t *file;
  int has_global;
  sl4_hash_table_t *format;
  sl4_hash_table_t *sst;
  sl4_hash_table_t *xf;
  sl4_hash_table_t *sheet;
  cexcel_book_t *book;
  cexcel_sheet_t *worksheet;
} cexcel_context_t;

cexcel_context_t *cexcel_context_new( cexcel_file_t * file );
int cexcel_context_delete( cexcel_context_t * ctx );

/* ctx_format.c */
int cexcel_context_format_new( cexcel_context_t * ctx );
int cexcel_context_format_delete( cexcel_context_t * ctx );
int cexcel_context_format_init( cexcel_context_t * ctx );
int cexcel_context_format_add( cexcel_context_t * ctx, CEXCEL_DWORD idx,
                               sl4_string_t *str );
sl4_string_t *cexcel_context_format_get( cexcel_context_t * ctx,
                                         CEXCEL_DWORD idx );

/* ctx_sst.c */
int cexcel_context_sst_new( cexcel_context_t * ctx );
int cexcel_context_sst_delete( cexcel_context_t * ctx );
int cexcel_context_sst_add( cexcel_context_t * ctx, sl4_string_t *str );
sl4_string_t *cexcel_context_sst_get( cexcel_context_t * ctx,
                                      CEXCEL_DWORD idx );

/* ctx_sheet.c */
int cexcel_context_sheet_new( cexcel_context_t * ctx );
int cexcel_context_sheet_delete( cexcel_context_t * ctx );
int cexcel_context_sheet_add( cexcel_context_t * ctx, CEXCEL_DWORD position,
                              const char *str );
int cexcel_context_sheet_get( cexcel_context_t * ctx, CEXCEL_DWORD position );

/* ctx_xf.c */
int cexcel_context_xf_new( cexcel_context_t * ctx );
int cexcel_context_xf_delete( cexcel_context_t * ctx );
int cexcel_context_xf_add( cexcel_context_t * ctx, CEXCEL_XFTYPE type,
                           CEXCEL_UCHAR attrib, CEXCEL_DWORD idx_pstyle,
                           CEXCEL_DWORD idx_font, CEXCEL_DWORD idx_format );
CEXCEL_DWORD cexcel_context_xf_get_format_index( cexcel_context_t * ctx,
                                                 CEXCEL_DWORD idx );

/* record.c */
typedef enum _CEXCEL_STYPE {
  CEXCEL_STYPE_BYTE = 8,        /* for BIFF2-BIFF5 */
  CEXCEL_STYPE_UNICODE = 16     /* for BIFF8 */
} CEXCEL_STYPE;

typedef enum _CEXCEL_SLEN {
  CEXCEL_SLEN_ONE = 1,
  CEXCEL_SLEN_TWO = 2
} CEXCEL_SLEN;

typedef struct _cexcel_record_t {
  /* maximum record length of BIFF8 is 8228 byte */
  size_t offset;
  CEXCEL_WORD id;
  CEXCEL_WORD len;
  CEXCEL_UCHAR data[8224];
} cexcel_record_t;

int cexcel_record_read( cexcel_file_t * file, cexcel_record_t * record );
void cexcel_record_dump( cexcel_record_t * record );
int cexcel_record_goto_eof( cexcel_file_t * file );
CEXCEL_UCHAR cexcel_record_data_uchar( cexcel_record_t * record,
                                       cexcel_context_t * ctx,
                                       size_t *offset );
CEXCEL_WORD cexcel_record_data_word( cexcel_record_t * record,
                                     cexcel_context_t * ctx, size_t *offset );
CEXCEL_DWORD cexcel_record_data_dword( cexcel_record_t * record,
                                       cexcel_context_t * ctx,
                                       size_t *offset );
sl4_string_t *cexcel_record_data_str( cexcel_record_t * record,
                                      cexcel_context_t * ctx,
                                      size_t *offset, CEXCEL_STYPE stype,
                                      CEXCEL_SLEN slen );
sl4_string_t *cexcel_record_data_rk( cexcel_record_t * record,
                                     cexcel_context_t * ctx, size_t *offset,
                                     CEXCEL_DWORD idx_xf );
void cexcel_record_align_word( CEXCEL_WORD * data );
void cexcel_record_align_dword( CEXCEL_DWORD * data );
int cexcel_record_word2int( CEXCEL_WORD data );
int cexcel_record_dword2int( CEXCEL_DWORD data );
double cexcel_record_dword2double( CEXCEL_DWORD high, CEXCEL_DWORD low );

/* biff_blank.c */
CEXCEL_RETTYPE cexcel_biff_blank( cexcel_record_t * record,
                                  cexcel_context_t * ctx );
/* biff_bof.c */
CEXCEL_RETTYPE cexcel_biff_bof( cexcel_record_t * record,
                                cexcel_context_t * ctx );
/* biff_boolerr.c */
CEXCEL_RETTYPE cexcel_biff_boolerr( cexcel_record_t * record,
                                    cexcel_context_t * ctx );
/* biff_boundsheete.c */
CEXCEL_RETTYPE cexcel_biff_boundsheet( cexcel_record_t * record,
                                       cexcel_context_t * ctx );
/* biff_codepage.c */
CEXCEL_RETTYPE cexcel_biff_codepage( cexcel_record_t * record,
                                     cexcel_context_t * ctx );
/* biff_eof.c */
CEXCEL_RETTYPE cexcel_biff_eof( cexcel_record_t * record,
                                cexcel_context_t * ctx );
/* biff_filepass.c */
CEXCEL_RETTYPE cexcel_biff_filepass( cexcel_record_t * record,
                                     cexcel_context_t * ctx );
/* biff_format.c */
CEXCEL_RETTYPE cexcel_biff_format( cexcel_record_t * record,
                                   cexcel_context_t * ctx );
/* biff_integer.c */
CEXCEL_RETTYPE cexcel_biff_integer( cexcel_record_t * record,
                                    cexcel_context_t * ctx );
/* biff_label.c */
CEXCEL_RETTYPE cexcel_biff_label( cexcel_record_t * record,
                                  cexcel_context_t * ctx );
/* biff_labelsst.c */
CEXCEL_RETTYPE cexcel_biff_labelsst( cexcel_record_t * record,
                                     cexcel_context_t * ctx );
/* biff_mulblank.c */
CEXCEL_RETTYPE cexcel_biff_mulblank( cexcel_record_t * record,
                                     cexcel_context_t * ctx );
/* biff_mulrk.c */
CEXCEL_RETTYPE cexcel_biff_mulrk( cexcel_record_t * record,
                                  cexcel_context_t * ctx );
/* biff_number.c */
CEXCEL_RETTYPE cexcel_biff_number( cexcel_record_t * record,
                                   cexcel_context_t * ctx );
/* biff_rk.c */
CEXCEL_RETTYPE cexcel_biff_rk( cexcel_record_t * record,
                               cexcel_context_t * ctx );
/* biff_rstring.c */
CEXCEL_RETTYPE cexcel_biff_rstring( cexcel_record_t * record,
                                    cexcel_context_t * ctx );
/* biff_sst.c */
CEXCEL_RETTYPE cexcel_biff_sst( cexcel_record_t * record,
                                cexcel_context_t * ctx );
/* biff_xf.c */
CEXCEL_RETTYPE cexcel_biff_xf( cexcel_record_t * record,
                               cexcel_context_t * ctx );

CEXCEL_END_DECLS
#endif /* __CEXCEL_INTERNAL_H__ */
