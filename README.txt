excel2xoonips 2.0.0 : A data convertor from Excel data to XooNIps import file.

Copyright (C) 2007-2016 RIKEN, Japan All rights reserved.

DESCRIPTION:
  excel2xoonips is a utility to covert Excel data to XooNIps import file.

LICENSE:
  This software was provided under GPL (The GNU General Public License)
  version 2. See COPYING.txt file for more detail.

Usage:
 % excel2xoonips Excel.xls XooNIpsImport.zip
   - Excel.xls         : input file
   - XooNIpsImport.zip : output file

Import Excel data to XooNIps:
 1) convert Excel data to XooNIps Import file
    % excel2xoonips Excel.xls XooNIps.zip
 2) import converted "XooNIps import file" on XooNIps using web browser
    
Change Logs:
  2016-08-25  2.0.0
   * supported xoonips version 4.x
  2009-11-26  1.0.2
   * fixed bug for single quote ' handling.
   * fixed bug for missing 'lang' element of Article item type.
  2008-12-12  1.0.1
   * supported xoonips version 3.41
   * supported to work on the MS-Vista.
   * supported dataset cell type.
   * supported user defined array's delimiter.
   * changed default delimiter of Article item type configs and samples
   * experimental supported for index tree mode.
  2008-1-21   1.0.0
   * initial release from RIKEN BSI Neuroinformatics Japan Center.

