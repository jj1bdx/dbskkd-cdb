/* 
 * dbskkd-cdb.c
 * ucspi-tcp-style SKK Server for D. J. Bernstein's cdb
 * (cdb, daemontools and ucspi-tcp required)
 *
 * Copyright (c) 1998-2009 by Kenji Rikitake. All rights reserved.
 * See LICENSE of the distribution kit for the redistribution conditions.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "cdb.h"

/* 
 * dbskkd chroot()'ed directory
 */
#ifndef SERVER_DIR
#define SERVER_DIR	"/service/dbskkd-cdb/root"
#endif /* SERVER_DIR */

/* 
 * dbskkd dictionary file 
 * NOTE: specify chroot()'ed path here
 */
#ifndef JISHO_FILE
#define JISHO_FILE	"/SKK-JISYO.L.cdb"
#endif /* JISHO_FILE */

#define	BUFSIZE		(1024)	/* max size of a request */
#define DATASIZE	(2048)	/* max size of a result */

#define CLIENT_END	'0'
#define CLIENT_REQUEST	'1'
#define CLIENT_VERSION	'2'
#define CLIENT_HOST	'3'

#define SERVER_FOUND	'1'
#define SERVER_NOT_FOUND '4'

#define STDIN (fileno(stdin))
#define STDOUT (fileno(stdout))

/* must be terminated with space */
#define VERSION "dbskkd-cdb-1.71dev " 

/* diesys() originally from DJB's cdb-0.55, modified */
void diesys(char *why)
{ 
  fprintf(stderr, "dbskkd-cdb: pid %d ", getpid());
  perror(why); 
  exit(1); 
}

/*
 * main program
 */

int main(int argc, char *argv[])
{
  static char combuf[BUFSIZE], data[DATASIZE];
  register char *pbuf, *key, *p;
  static struct cdb diccdb;
  int id, dicfd, ex, length;
  unsigned int keylen, datalen;

  /* chroot to server directory */
  if (chdir(SERVER_DIR) == -1)
    diesys("cannot chdir() to " SERVER_DIR);
  if (chroot(".") == -1)
    diesys("cannot chroot()");
  /* set uid and gid */
  p = getenv("GID");
  if (p) {
    id = atoi(p);
    if (setgroups(1, (gid_t *)&id) == -1)
      diesys("cannot setgroups()");
    if (setgid(id) == -1)
      diesys("cannot setgid()");
    }
  p = getenv("UID");
  if (p) {
    id = atoi(p);
    if (setuid(id) == -1)
      diesys("cannot setuid()");
    }

  /* open dictionary cdb file */
  if ((dicfd = open(JISHO_FILE, O_RDONLY, S_IRUSR)) < 0) {
    diesys("cannot open() the dictionary file " JISHO_FILE);
    /* NOTREACHED */
  }
  cdb_init(&diccdb, dicfd);

  /* command loop */
  ex = 0;
  while (!ex) {
    length = read(STDIN, &combuf[0], BUFSIZE - 1);
    if (length < 0)
      diesys("read error from stdin");
    else if (length == 0) {
      fprintf(stderr, "dbskkd-cdb: pid %d end of file detected\n", getpid());
      ex = 1;
      break;
    }
    switch (combuf[0]) {
    case CLIENT_END:
      fprintf(stderr, 
	      "dbskkd-cdb: pid %d end of conversion requested\n", getpid());
      ex = 1;
      break;
    case CLIENT_VERSION:
      if (write(STDOUT, VERSION, sizeof(VERSION) - 1) < 0)
        diesys("write error (version string)"); 
      break;
    case CLIENT_HOST:
      /* use tcp-environ */
      p = getenv("TCPLOCALHOST");
      if (p) {
        if (write(STDOUT, p, strlen(p)) < 0)
          diesys("write error (TCPLOCALHOST)"); 
	}
      else {
	if (write(STDOUT, "UNKNOWN", 7) < 0)
	  diesys("write error (UNKNOWN TCPLOCALHOST)");
	}
      if (write(STDOUT, ":", 1) < 0)
	diesys("write error (colon after TCPLOCALHOST)");
      p = getenv("TCPLOCALIP");
      if (p) {
        if (write(STDOUT, p, strlen(p)) < 0)
          diesys("write error (TCPLOCALIP)"); 
	}
      else {
	if (write(STDOUT, "UNKNOWNIP", 7) < 0)
	  diesys("write error (UNKNOWN TCPLOCALIP)");
	}
      if (write(STDOUT, ": ", 2) < 0)
	diesys("write error (colon and space after TCPLOCALIP)");
      break;
    case CLIENT_REQUEST:
      /* get size of key */
      key = &combuf[1];
      for (pbuf = &combuf[1];
           *pbuf != ' ' && pbuf != &combuf[length - 1];
           pbuf++) {}
      keylen = pbuf - &combuf[1];
      if (keylen <= 0)
        diesys("invalid keysize");
      switch (cdb_find(&diccdb, key, keylen)) {
      case -1:
        diesys("fatal error on cdb_find()");
      case 1: /* found */
        if ((datalen = cdb_datalen(&diccdb)) >= DATASIZE - 2)
  	  diesys("searched entry too long");
	/* generate the answer string */
	p = data;
	*p++ = SERVER_FOUND;
        if (cdb_read(&diccdb, p, datalen, cdb_datapos(&diccdb)) < 0)
  	  diesys("read error (seeked dictionary data)");
        p += datalen;
        *p = '\n';
        /* sending found code and the result data string with LF */
        if (write(STDOUT, data, datalen + 2) < 0)
  	  diesys("write error (converted data)");
        break;
      case 0: /* NOT found */
        /* generate the answer string */
        combuf[0] = SERVER_NOT_FOUND;
        *pbuf = '\n';
        /* sending error code and the key string with LF */
        /* this action is required by skkinput */
        if (write(STDOUT, combuf, keylen + 2) < 0)
  	  diesys("write error (NOT_FOUND message)");
        break;
      default: /* unknown value */ 
        diesys("unknown return value from cdb_seek()");
      }
      break;
    default:
      fprintf(stderr, 
	      "dbskkd-cdb: pid %d unknown client request code = %d\n",
	       getpid(), combuf[0]);
      exit(1);
    }
  }

  /* close dictionary db file */
  cdb_free(&diccdb);
  if (close(dicfd) != 0)
    diesys("cannot close() the dictionary file");

  exit(0);
}

/* end of program */
