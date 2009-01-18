# dbskkd-cdb Makefile

CC = cc -Wall -O2 -g -I/usr/local/include
COMPAT =
CDBLIB = /usr/local/lib/libcdb.a
INSTALLDIR = /usr/local/libexec

.c.o:
	$(CC) $(COMPAT) $(PRIVATE) -c $*.c

all: 	dbskkd-cdb 

clean:
	/bin/rm -f dbskkd-cdb *.o

dbskkd-cdb: dbskkd-cdb.o 
	$(CC) $(COMPAT) $(PRIVATE) -o dbskkd-cdb \
		dbskkd-cdb.o ${CDBLIB}

dbskkd-cdb.o: dbskkd-cdb.c 
	$(CC) $(COMPAT) $(PRIVATE) -c dbskkd-cdb.c

error.o: error.c error.h

install: dbskkd-cdb 
	cp dbskkd-cdb $(INSTALLDIR); chmod 755 $(INSTALLDIR)/dbskkd-cdb

# end of makefile
