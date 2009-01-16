#!/bin/sh

CDBDIR=~/src/cdb-0.75
CP=/bin/cp

${CP} ${CDBDIR}/byte.h .
${CP} ${CDBDIR}/byte_copy.c .
${CP} ${CDBDIR}/byte_diff.c .
${CP} ${CDBDIR}/cdb.c .
${CP} ${CDBDIR}/cdb.h .
${CP} ${CDBDIR}/cdb_hash.c .
${CP} ${CDBDIR}/error.c .
${CP} ${CDBDIR}/error.h .
${CP} ${CDBDIR}/readwrite.h .
${CP} ${CDBDIR}/seek.h .
${CP} ${CDBDIR}/seek_set.c .
${CP} ${CDBDIR}/uint32.h .
${CP} ${CDBDIR}/uint32_unpack.c .

