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

void error_internal( ERROR_CODE errcode, const char *file, int line )
{
  const char *mes;
  switch ( errcode ) {
  case ERROR_CODE_OUTMEM:
    mes = "out of memory";
    break;
  case ERROR_CODE_UNEXPECTED:
    mes = "unexpected error occurred";
    break;
  default:
    mes = "";
  }
  fprintf( stderr, "Internal Error : %s in %s on line %d\n", mes, file,
           line );
  exit( 1 );
}
