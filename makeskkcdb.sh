#!/bin/sh
./skktocdbm.sh < /usr/local/share/skk/SKK-JISYO.L | \
	cdbmake SKK-JISYO.L.cdb SKK-JISYO.L.tmp
