# dbskkd-cdb Makefile
# $Id: Makefile,v 1.6 1998/12/12 11:24:36 kenji Exp $

# please define SERVERDIR
SERVERDIR = /usr/local/libexec
# for FreeBSD-2.x and BSD/OS (without shlib)
CC = gcc -O -g
COMPAT =
LDFLAGS = -lutil ./libcdb.a
#LDFLAGS = -L/usr/local/lib -lutil -lcdb
# enable this for DJB's ucspi-tcp (tcpserver) environment
#PRIVATE = -DUCSPI_TCP

all: 	dbskkd-cdb 

dbskkd-cdb: dbskkd-cdb.o
	$(CC) $(LDFLAGS) -o dbskkd-cdb dbskkd-cdb.o $(LDFLAGS)

dbskkd-cdb.o: dbskkd-cdb.c
	$(CC) $(COMPAT) $(PRIVATE) -DSERVER_DIR=\"$(SERVERDIR)\" -c dbskkd-cdb.c

install: dbskkd-cdb 
	cp dbskkd-cdb $(SERVERDIR); chmod 755 $(SERVERDIR)/dbskkd-cdb

clean:
	rm -f dbskkd-cdb dbskkd-cdb.o
