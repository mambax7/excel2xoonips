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

sl4_string_t *tmpl_rights_get( const char *text, sl4_string_t **use_cc,
                               sl4_string_t **cc_commercial_use,
                               sl4_string_t **cc_modification,
                               int is_required, char *item_xml )
{
  sl4_string_t *str;
  size_t len;
  int all_rights = 0;
  const char *cc_map[] = {
    "<!--Creative Commons License--><table><tbody><tr><td><a rel=\"license\" href=\"http://creativecommons.org/licenses/by/2.5/\"><img alt=\"Creative Commons License\" src=\"http://creativecommons.org/images/public/somerights20.png\" border=\"0\"/></a><br /></td><td>This work is licensed under a <a rel=\"license\" href=\"http://creativecommons.org/licenses/by/2.5/\">Creative Commons Attribution 2.5 License</a>.\n</td></tr></tbody></table><!--/Creative Commons License--><!-- <rdf:RDF xmlns=\"http://web.resource.org/cc/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n<Work rdf:about=\"\">\n<license rdf:resource=\"http://creativecommons.org/licenses/by/2.5/\" />\n</Work>\n<License rdf:about=\"http://creativecommons.org/licenses/by/2.5/\"><permits rdf:resource=\"http://web.resource.org/cc/Reproduction\"/><permits rdf:resource=\"http://web.resource.org/cc/Distribution\"/><requires rdf:resource=\"http://web.resource.org/cc/Notice\"/><requires rdf:resource=\"http://web.resource.org/cc/Attribution\"/><permits rdf:resource=\"http://web.resource.org/cc/DerivativeWorks\"/></License></rdf:RDF> -->",    /* CC-BY */
    "<!--Creative Commons License--><table><tbody><tr><td><a rel=\"license\" href=\"http://creativecommons.org/licenses/by-sa/2.5/\"><img alt=\"Creative Commons License\" src=\"http://creativecommons.org/images/public/somerights20.png\" border=\"0\"/></a><br />\n</td><td>This work is licensed under a <a rel=\"license\" href=\"http://creativecommons.org/licenses/by-sa/2.5/\">Creative Commons Attribution-ShareAlike 2.5 License</a>.\n</td></tr></tbody></table><!--/Creative Commons License--><!-- <rdf:RDF xmlns=\"http://web.resource.org/cc/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n<Work rdf:about=\"\">\n<license rdf:resource=\"http://creativecommons.org/licenses/by-sa/2.5/\" />\n</Work>\n<License rdf:about=\"http://creativecommons.org/licenses/by-sa/2.5/\"><permits rdf:resource=\"http://web.resource.org/cc/Reproduction\"/><permits rdf:resource=\"http://web.resource.org/cc/Distribution\"/><requires rdf:resource=\"http://web.resource.org/cc/Notice\"/><requires rdf:resource=\"http://web.resource.org/cc/Attribution\"/><permits rdf:resource=\"http://web.resource.org/cc/DerivativeWorks\"/><requires rdf:resource=\"http://web.resource.org/cc/ShareAlike\"/></License></rdf:RDF> -->", /* CC-BY-SA */
    "<!--Creative Commons License--><table><tbody><tr><td><a rel=\"license\" href=\"http://creativecommons.org/licenses/by-nd/2.5/\"><img alt=\"Creative Commons License\" src=\"http://creativecommons.org/images/public/somerights20.png\" border=\"0\"/></a><br />\n</td><td>This work is licensed under a <a rel=\"license\" href=\"http://creativecommons.org/licenses/by-nd/2.5/\">Creative Commons Attribution-NoDerivs 2.5 License</a>.\n</td></tr></tbody></table><!--/Creative Commons License--><!-- <rdf:RDF xmlns=\"http://web.resource.org/cc/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n<Work rdf:about=\"\">\n<license rdf:resource=\"http://creativecommons.org/licenses/by-nd/2.5/\" />\n</Work>\n<License rdf:about=\"http://creativecommons.org/licenses/by-nd/2.5/\"><permits rdf:resource=\"http://web.resource.org/cc/Reproduction\"/><permits rdf:resource=\"http://web.resource.org/cc/Distribution\"/><requires rdf:resource=\"http://web.resource.org/cc/Notice\"/><requires rdf:resource=\"http://web.resource.org/cc/Attribution\"/></License></rdf:RDF> -->",   /* CC-BY-ND */
    "<!--Creative Commons License--><table><tbody><tr><td><a rel=\"license\" href=\"http://creativecommons.org/licenses/by-nc/2.5/\"><img alt=\"Creative Commons License\" src=\"http://creativecommons.org/images/public/somerights20.png\" border=\"0\"/></a><br />\n</td><td>This work is licensed under a <a rel=\"license\" href=\"http://creativecommons.org/licenses/by-nc/2.5/\">Creative Commons Attribution-NonCommercial 2.5 License</a>.\n</td></tr></tbody></table><!--/Creative Commons License--><!-- <rdf:RDF xmlns=\"http://web.resource.org/cc/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n<Work rdf:about=\"\">\n<license rdf:resource=\"http://creativecommons.org/licenses/by-nc/2.5/\" />\n</Work>\n<License rdf:about=\"http://creativecommons.org/licenses/by-nc/2.5/\"><permits rdf:resource=\"http://web.resource.org/cc/Reproduction\"/><permits rdf:resource=\"http://web.resource.org/cc/Distribution\"/><requires rdf:resource=\"http://web.resource.org/cc/Notice\"/><requires rdf:resource=\"http://web.resource.org/cc/Attribution\"/><prohibits rdf:resource=\"http://web.resource.org/cc/CommercialUse\"/><permits rdf:resource=\"http://web.resource.org/cc/DerivativeWorks\"/></License></rdf:RDF> -->",  /* CC-BY-NC */
    "<!--Creative Commons License--><table><tbody><tr><td><a rel=\"license\" href=\"http://creativecommons.org/licenses/by-nc-sa/2.5/\"><img alt=\"Creative Commons License\" src=\"http://creativecommons.org/images/public/somerights20.png\" border=\"0\"/></a><br />\n</td><td>This work is licensed under a <a rel=\"license\" href=\"http://creativecommons.org/licenses/by-nc-sa/2.5/\">Creative Commons Attribution-NonCommercial-ShareAlike 2.5 License</a>.\n</td></tr></tbody></table><!--/Creative Commons License--><!-- <rdf:RDF xmlns=\"http://web.resource.org/cc/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n<Work rdf:about=\"\">\n<license rdf:resource=\"http://creativecommons.org/licenses/by-nc-sa/2.5/\" />\n</Work>\n<License rdf:about=\"http://creativecommons.org/licenses/by-nc-sa/2.5/\"><permits rdf:resource=\"http://web.resource.org/cc/Reproduction\"/><permits rdf:resource=\"http://web.resource.org/cc/Distribution\"/><requires rdf:resource=\"http://web.resource.org/cc/Notice\"/><requires rdf:resource=\"http://web.resource.org/cc/Attribution\"/><prohibits rdf:resource=\"http://web.resource.org/cc/CommercialUse\"/><permits rdf:resource=\"http://web.resource.org/cc/DerivativeWorks\"/><requires rdf:resource=\"http://web.resource.org/cc/ShareAlike\"/></License></rdf:RDF> -->", /* CC-BY-NC-SA */
    "<!--Creative Commons License--><table><tbody><tr><td><a rel=\"license\" href=\"http://creativecommons.org/licenses/by-nc-nd/2.5/\"><img alt=\"Creative Commons License\" src=\"http://creativecommons.org/images/public/somerights20.png\" border=\"0\"/></a><br />\n</td><td>This work is licensed under a <a rel=\"license\" href=\"http://creativecommons.org/licenses/by-nc-nd/2.5/\">Creative Commons Attribution-NonCommercial-NoDerivs 2.5 License</a>.\n</td></tr></tbody></table><!--/Creative Commons License--><!-- <rdf:RDF xmlns=\"http://web.resource.org/cc/\" xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n<Work rdf:about=\"\">\n<license rdf:resource=\"http://creativecommons.org/licenses/by-nc-nd/2.5/\" />\n</Work>\n<License rdf:about=\"http://creativecommons.org/licenses/by-nc-nd/2.5/\"><permits rdf:resource=\"http://web.resource.org/cc/Reproduction\"/><permits rdf:resource=\"http://web.resource.org/cc/Distribution\"/><requires rdf:resource=\"http://web.resource.org/cc/Notice\"/><requires rdf:resource=\"http://web.resource.org/cc/Attribution\"/><prohibits rdf:resource=\"http://web.resource.org/cc/CommercialUse\"/></License></rdf:RDF> -->"    /* CC-BY-NC-ND */
  };
  str = sl4_string_new( NULL );
  *use_cc = sl4_string_new( NULL );
  *cc_commercial_use = sl4_string_new( NULL );
  *cc_modification = sl4_string_new( NULL );
  if ( str == NULL || *use_cc == NULL || *cc_commercial_use == NULL
       || *cc_modification == NULL ) {
    ERROR_OUTMEM(  );
  }
  len = strlen( text );
  if ( len == 0 ) {
    if ( is_required ) {
      /* empty cell data */
      sl4_string_delete( str );
      return NULL;
    }
    if ( sl4_string_sprintf( *use_cc, "%d", 0 ) != 0 ||
         sl4_string_sprintf( *cc_commercial_use, "%d", 1 ) != 0 ||
         sl4_string_sprintf( *cc_modification, "%d", 2 ) != 0 ) {
      ERROR_OUTMEM(  );
    }
  } else {
    if ( strcmp( text, "CC-BY" ) == 0 ) {
      if ( sl4_string_set( str, cc_map[0] ) != 0 ||
           sl4_string_sprintf( *use_cc, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_commercial_use, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_modification, "%d", 2 ) != 0 ) {
        ERROR_OUTMEM(  );
      }
    } else if ( strcmp( text, "CC-BY-SA" ) == 0 ) {
      if ( sl4_string_set( str, cc_map[1] ) != 0 ||
           sl4_string_sprintf( *use_cc, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_commercial_use, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_modification, "%d", 1 ) != 0 ) {
        ERROR_OUTMEM(  );
      }
    } else if ( strcmp( text, "CC-BY-ND" ) == 0 ) {
      if ( sl4_string_set( str, cc_map[2] ) != 0 ||
           sl4_string_sprintf( *use_cc, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_commercial_use, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_modification, "%d", 0 ) != 0 ) {
        ERROR_OUTMEM(  );
      }
    } else if ( strcmp( text, "CC-BY-NC" ) == 0 ) {
      if ( sl4_string_set( str, cc_map[3] ) != 0 ||
           sl4_string_sprintf( *use_cc, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_commercial_use, "%d", 0 ) != 0 ||
           sl4_string_sprintf( *cc_modification, "%d", 2 ) != 0 ) {
        ERROR_OUTMEM(  );
      }
    } else if ( strcmp( text, "CC-BY-NC-SA" ) == 0 ) {
      if ( sl4_string_set( str, cc_map[4] ) != 0 ||
           sl4_string_sprintf( *use_cc, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_commercial_use, "%d", 0 ) != 0 ||
           sl4_string_sprintf( *cc_modification, "%d", 1 ) != 0 ) {
        ERROR_OUTMEM(  );
      }
    } else if ( strcmp( text, "CC-BY-NC-ND" ) == 0 ) {
      if ( sl4_string_set( str, cc_map[5] ) != 0 ||
           sl4_string_sprintf( *use_cc, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_commercial_use, "%d", 0 ) != 0 ||
           sl4_string_sprintf( *cc_modification, "%d", 0 ) != 0 ) {
        ERROR_OUTMEM(  );
      }
    } else {
      /* all rights reserved */
      if ( sl4_string_set( str, text ) != 0 ||
           sl4_string_sprintf( *use_cc, "%d", 0 ) != 0 ||
           sl4_string_sprintf( *cc_commercial_use, "%d", 1 ) != 0 ||
           sl4_string_sprintf( *cc_modification, "%d", 2 ) != 0 ) {
        ERROR_OUTMEM(  );
      }
      all_rights = 1;
    }
  }
  if ( !is_compat && len != 0 ) {
      if ( all_rights ) {
        sl4_string_htmlspecialchars( str );
        sl4_string_sprintf( str, "<C:%s C:type=\"extend\" C:occurrence_number=\"1\">%s%s%sUnported,%s</C:%s>", item_xml,
                 sl4_string_get(*use_cc), sl4_string_get(*cc_commercial_use), sl4_string_get(*cc_modification), sl4_string_get(str), item_xml);
      } else {
        sl4_string_sprintf( str, "<C:%s C:type=\"extend\" C:occurrence_number=\"1\">%s%s%sUnported,</C:%s>", item_xml,
                 sl4_string_get(*use_cc), sl4_string_get(*cc_commercial_use), sl4_string_get(*cc_modification), item_xml);
      }
  }
  return str;
}
