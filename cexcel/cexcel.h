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

#ifndef __CEXCEL_H__
#define __CEXCEL_H__

#ifdef __cplusplus
# define CEXCEL_BEGIN_DECLS extern "C" {
# define CEXCEL_END_DECLS }
#else
# define CEXCEL_BEGIN_DECLS
# define CEXCEL_END_DECLS
#endif

CEXCEL_BEGIN_DECLS
/* iconv.c */
  typedef enum _CEXCEL_ICONV_RETTYPE {
  CEXCEL_ICONV_RETTYPE_SUCCESS,
  CEXCEL_ICONV_RETTYPE_WRONGCHARSET,
  CEXCEL_ICONV_RETTYPE_ILLCHAR,
  CEXCEL_ICONV_RETTYPE_ILLSEQ,
  CEXCEL_ICONV_RETTYPE_OUTMEM,
  CEXCEL_ICONV_RETTYPE_UNKNOWN,
} CEXCEL_ICONV_RETTYPE;

CEXCEL_ICONV_RETTYPE cexcel_iconv_convert( sl4_string_t *in,
                                           sl4_string_t *out,
                                           const char *in_charset,
                                           const char *out_charset );

/* cell.c */
typedef struct _cexcel_cell_t {
  unsigned int row;
  unsigned int col;
  sl4_string_t *text;
  sl4_string_t *address;
} cexcel_cell_t;

cexcel_cell_t *cexcel_cell_new( const char *text, unsigned int row,
                                unsigned int col );
int cexcel_cell_delete( cexcel_cell_t * cell );
const char *cexcel_cell_get_text( cexcel_cell_t * cell );
unsigned int cexcel_cell_get_col( cexcel_cell_t * cell );
unsigned int cexcel_cell_get_row( cexcel_cell_t * cell );
const char *cexcel_cell_get_address( cexcel_cell_t * cell );

/* sheet.c */
typedef struct _cexcel_sheet_t {
  unsigned int rows;
  unsigned int cols;
  sl4_hash_table_t *cells;
  sl4_string_t *name;
} cexcel_sheet_t;

cexcel_sheet_t *cexcel_sheet_new( const char *name );
int cexcel_sheet_delete( cexcel_sheet_t * sheet );
int cexcel_sheet_set_cell( cexcel_sheet_t * sheet, const char *text,
                           unsigned int row, unsigned int col );
cexcel_cell_t *cexcel_sheet_get_cell( cexcel_sheet_t * sheet,
                                      unsigned int row, unsigned int col );
unsigned int cexcel_sheet_get_rows( cexcel_sheet_t * sheet );
unsigned int cexcel_sheet_get_cols( cexcel_sheet_t * sheet );
const char *cexcel_sheet_get_name( cexcel_sheet_t * sheet );
int cexcel_sheet_trim_empty_cells( cexcel_sheet_t * sheet );

/* book.c */
typedef struct _cexcel_book_t {
  unsigned int nums;
  sl4_hash_table_t *sheets;
  sl4_string_t *filename;
} cexcel_book_t;

cexcel_book_t *cexcel_book_new( const char *filename );
int cexcel_book_delete( cexcel_book_t * book );
cexcel_sheet_t *cexcel_book_add_sheet( cexcel_book_t * book,
                                       const char *name );
cexcel_sheet_t *cexcel_book_get_sheet( cexcel_book_t * book,
                                       unsigned int num );
unsigned int cexcel_book_get_nums( cexcel_book_t * book );
const char *cexcel_book_get_filename( cexcel_book_t * book );

/* cexcel.c */
cexcel_book_t *cexcel_read_file( const char *filename );

CEXCEL_END_DECLS
#endif /* __CEXCEL_H__ */
