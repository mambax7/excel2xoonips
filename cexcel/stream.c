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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "cole.h"
#include "libsl4.h"
#include "cexcel.h"
#include "cexcel_internal.h"

size_t cexcel_stream_read( cexcel_file_t * file, void *buf, size_t size )
{
  size_t ret;
  COLERRNO err;
  switch ( file->ftype ) {
  case CEXCEL_FTYPE_NORMAL:
    ret = sl4_file_read( file->file, buf, size );
    break;
  case CEXCEL_FTYPE_OLE:
    ret = cole_fread( file->ole.cf, buf, size, &err );
    break;
  case CEXCEL_FTYPE_UNDEF:
  default:
    ret = 0;
    break;
  }
  return ret;
}

size_t cexcel_stream_tell( cexcel_file_t * file )
{
  size_t ret;
  switch ( file->ftype ) {
  case CEXCEL_FTYPE_NORMAL:
    ret = ( size_t ) sl4_file_tell( file->file );
    break;
  case CEXCEL_FTYPE_OLE:
    ret = cole_ftell( file->ole.cf );
    break;
  case CEXCEL_FTYPE_UNDEF:
  default:
    ret = 0;
    break;
  }
  return ret;
}

int cexcel_stream_seek( cexcel_file_t * file, off_t offset,
                        CEXCEL_SEEK_FLAG whence )
{
  int ret;
  size_t cole_delta;
  sl4_file_seek_enum sl4_whence;
  COLE_SEEK_FLAG cole_whence;
  COLERRNO err;
  switch ( file->ftype ) {
  case CEXCEL_FTYPE_NORMAL:
    switch ( whence ) {
    case CEXCEL_SEEK_SET:
      sl4_whence = SL4_FILE_SEEK_SET;
      break;
    case CEXCEL_SEEK_END:
      sl4_whence = SL4_FILE_SEEK_END;
      break;
    case CEXCEL_SEEK_CUR:
    default:
      sl4_whence = SL4_FILE_SEEK_CUR;
      break;
    }
    ret = sl4_file_seek( file->file, offset, sl4_whence );
    break;
  case CEXCEL_FTYPE_OLE:
    switch ( whence ) {
    case CEXCEL_SEEK_SET:
      cole_whence = COLE_SEEK_SET;
      cole_delta = ( size_t ) offset;
      break;
    case CEXCEL_SEEK_END:
      cole_whence = COLE_SEEK_END;
      cole_delta = ( size_t ) ( -offset );      /* nagative value */
      break;
    case CEXCEL_SEEK_CUR:
    default:
      if ( offset > 0 ) {
        cole_whence = COLE_SEEK_FORWARD;
        cole_delta = ( size_t ) offset;
      } else {
        cole_whence = COLE_SEEK_BACKWARD;
        cole_delta = ( size_t ) ( -offset );
      }
      break;
    }
    ret = cole_fseek( file->ole.cf, cole_delta, cole_whence, &err );
    if ( ret != 0 ) {
      /* if cole_fseek() returned 1 then it's failure. */
      /* we have to override result to -1 */
      ret = -1;
    }
    break;
  case CEXCEL_FTYPE_UNDEF:
  default:
    ret = -1;
    break;
  }
  return ret;
}

int cexcel_stream_eof( cexcel_file_t * file )
{
  int ret;
  switch ( file->ftype ) {
  case CEXCEL_FTYPE_NORMAL:
    ret = sl4_file_eof( file->file );
    break;
  case CEXCEL_FTYPE_OLE:
    ret = cole_feof( file->ole.cf );
    break;
  case CEXCEL_FTYPE_UNDEF:
  default:
    ret = -1;                   /* always -1 returned */
  }
  return ret;
}
