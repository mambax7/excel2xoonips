#!/bin/sh

XMLLINT=xmllint

DOC_BASE_DIR="`dirname $0`"
CONFIG_INI="${DOC_BASE_DIR}/config.ini"

if test -f ${CONFIG_INI}; then
  eval `cat ${CONFIG_INI}`
  export SGML_CATALOG_FILES
  export XSL_STYLESHEETS_DIR
fi
if test -z "${SGML_CATALOG_FILES}"; then
  echo "Error: set SGML_CATALOG_FILES environment variable"
  exit 1;
fi
if test -z "${XSL_STYLESHEETS_DIR}/xhtml/chunk.xsl"; then
  echo "Error: set XSL_STYLESHEETS_DIR environment variable"
  exit 1;
fi

which "${XMLLINT}" > /dev/null
RETVAL=$?
if [ ${RETVAL} -ne 0 ]; then
  echo "Error: ${XMLLINT} command required!"
  exit 1;
fi

${XMLLINT} --xinclude --noout --nonet --postvalid --catalogs $*
RETVAL=$?
exit ${RETVAL}
