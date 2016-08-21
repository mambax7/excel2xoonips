#!/bin/sh

XSLTPROC=xsltproc

DOC_BASE_DIR="`dirname $0`"
CONFIG_INI="${DOC_BASE_DIR}/config.ini"

TEMP_DIR=${DOC_BASE_DIR}/temp

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

which "${XSLTPROC}" > /dev/null
RETVAL=$?
if [ ${RETVAL} -ne 0 ]; then
  echo "Error: ${XSLTPROC} command required!"
  exit 1;
fi

mkdir -p ${TEMP_DIR}

xsltproc --nonet --catalogs -o ${TEMP_DIR}/xhtml-titlepage.xsl ${XSL_STYLESHEETS_DIR}/template/titlepage.xsl ${DOC_BASE_DIR}/xsl/xhtml-titlepage.xml
xsltproc --nonet --catalogs -o ${TEMP_DIR}/fo-titlepage.xsl ${XSL_STYLESHEETS_DIR}/template/titlepage.xsl ${DOC_BASE_DIR}/xsl/fo-titlepage.xml

exit 0
