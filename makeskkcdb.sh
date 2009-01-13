#!/bin/sh
#$Id: makeskkcdb.sh,v 1.1 1998/12/12 11:01:36 kenji Exp $
./skktocdbm.sh < /usr/local/share/skk/SKK-JISYO.L | \
	cdbmake SKK-JISYO.L.cdb SKK-JISYO.L.tmp
