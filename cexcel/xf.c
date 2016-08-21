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

cexcel_xf_t *cexcel_xf_new(  )
{
  cexcel_xf_t *xf;
  xf = ( cexcel_xf_t * ) malloc( sizeof( cexcel_xf_t ) );
  if ( xf == NULL ) {
    return NULL;
  }
  xf->type = CEXCEL_XFTYPE_UNDEF;
  xf->idx_pstyle = ( CEXCEL_DWORD ) - 1;
  xf->idx_font = ( CEXCEL_DWORD ) - 1;
  xf->idx_format = ( CEXCEL_DWORD ) - 1;
  return xf;
}

int cexcel_xf_delete( cexcel_xf_t * xf )
{
  if ( xf == NULL ) {
    return -1;
  }
  free( xf );
  return 0;
}
