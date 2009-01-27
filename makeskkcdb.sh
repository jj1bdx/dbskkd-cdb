#!/bin/sh
./skktocdbm.sh < /usr/local/share/skk/SKK-JISYO.L | \
	cdb -c -t - SKK-JISYO.L.cdb
