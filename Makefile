# dbskkd-cdb Makefile

# please define SERVERDIR
SERVERDIR = /service/dbskkd-cdb/root
#
CC = gcc -Wall -O2 -g
COMPAT =
INSTALLDIR = /usr/local/libexec

.c.o:
	$(CC) $(COMPAT) $(PRIVATE) -c $*.c

all: 	getsrc dbskkd-cdb 

byte_copy.o: byte_copy.c byte.h

byte_diff.o: byte_diff.c byte.h

cdb.o: cdb.c readwrite.h error.h seek.h byte.h cdb.h uint32.h

cdb_hash.o: cdb_hash.c cdb.h uint32.h

clean:
	/bin/rm -f dbskkd-cdb *.o
	./deletecdb.sh

dbskkd-cdb: byte_copy.o byte_diff.o cdb.o cdb_hash.o dbskkd-cdb.o \
		error.o seek_set.o uint32_unpack.o
	$(CC) $(COMPAT) $(PRIVATE) -o dbskkd-cdb \
		byte_copy.o byte_diff.o cdb.o cdb_hash.o dbskkd-cdb.o \
		error.o seek_set.o uint32_unpack.o

dbskkd-cdb.o: dbskkd-cdb.c cdb.h uint32.h
	$(CC) $(COMPAT) $(PRIVATE) -DSERVER_DIR=\"$(SERVERDIR)\" -c dbskkd-cdb.c

error.o: error.c error.h

getsrc:
	./copyfromcdb.sh
	
install: dbskkd-cdb 
	cp dbskkd-cdb $(INSTALLDIR); chmod 755 $(INSTALLDIR)/dbskkd-cdb

seek_set.o: seek_set.c seek.h

uint32_unpack.o: uint32_unpack.c uint32.h
