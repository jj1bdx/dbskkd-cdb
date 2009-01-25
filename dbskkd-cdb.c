/* 
 * dbskkd-cdb.c
 * SKK dictionary Server
 * with Daniel J. Bernstein's cdb database
 *
 * Copyright (c) 1998-2009 by Kenji Rikitake. All rights reserved.
 * This code is distributed under
 * Creative Commons BSD License.
 * http://creativecommons.org/licenses/BSD/
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

/* tinycdb required */
#include "cdb.h"

/* 
 * dbskkd dictionary file 
 * default value: FreeBSD port japanese/ddskk
 * note: the macro was JISHO_FILE in 1.x
 *       spelling changed from JISHO to JISYO
 */
#ifndef JISYO_FILE
#define JISYO_FILE	"/usr/local/share/skk/SKK-JISYO.L.cdb"
#endif /* JISYO_FILE */

#define	BUFSIZE		(1024)	/* max size of a request */
#define DATASIZE	(4096)	/* max size of a result */

#define CLIENT_END	'0'
#define CLIENT_REQUEST	'1'
#define CLIENT_VERSION	'2'
#define CLIENT_HOST	'3'

#define SERVER_FOUND	'1'
#define SERVER_NOT_FOUND '4'

#define STDIN (fileno(stdin))
#define STDOUT (fileno(stdout))

/* these strings must be terminated with space */
#define VERSION "dbskkd-cdb-1.99expr-20090125-2 " 
#define DUMMYHOSTNAME "novalue: "

/*
 * define VERBOSE_MSG 
 * if explicit error messages are needed
 */
/* #undef VERBOSE_MSG */

/*
 * diesys() is replaced by exit(1)
 * when VERBOSE_MSG is not defined
 */
#ifdef VERBOSE_MSG
/* 
 * diesys() originally from DJB's cdb-0.55
 * modified by Kenji Rikitake
 */
void diesys(char *why)
{ 
  fprintf(stderr, "dbskkd-cdb: pid %d ", getpid());
  perror(why); 
  exit(1); 
}
#else /* !VERBOSE_MSG */
#define diesys(dummy_string) exit(1)
#endif /* VERBOSE_MSG */

/* main program */

int main(int argc, char *argv[])
{
  char combuf[BUFSIZE], data[DATASIZE];
  char *pbuf, *key, *p;
  struct cdb diccdb;
  int dicfd, ex, length;
  unsigned int keylen, datalen;

  /* open dictionary cdb file */
  if ((dicfd = open(JISYO_FILE, O_RDONLY, S_IRUSR)) < 0) {
    diesys("cannot open() the dictionary file " JISYO_FILE);
    /* NOTREACHED */
  }
  cdb_init(&diccdb, dicfd);

  /* command loop */
  /* set ex to non-zero for exiting */
  ex = 0;
  while (!ex) {

    length = read(STDIN, &combuf[0], BUFSIZE - 1);

    if (length < 0)
      diesys("read error from stdin");
    else if (length == 0) {
#ifdef VERBOSE_MSG
      fprintf(stderr, "dbskkd-cdb: pid %d end of file detected\n", getpid());
#endif /* VERBOSE_MSG */
      ex = 1;
      break;
    }

    /* parse request code */
    switch (combuf[0]) {

    case CLIENT_END:
#ifdef VERBOSE_MSG
      fprintf(stderr, 
	      "dbskkd-cdb: pid %d end of conversion requested\n", getpid());
#endif /* VERBOSE_MSG */
      ex = 1;
      break;

    case CLIENT_VERSION:
      if (write(STDOUT, VERSION, sizeof(VERSION) - 1) < 0)
        diesys("write error (version string)"); 
      break;

    case CLIENT_HOST:
      if (write(STDOUT, DUMMYHOSTNAME, sizeof(DUMMYHOSTNAME) - 1) < 0)
          diesys("write error (dummyhostname string)"); 
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

      /* lookup the cdb database */
      switch (cdb_find(&diccdb, key, keylen)) {

      case -1: /* error */
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

      default: /* unknown cdb return value */ 
        diesys("unknown return value from cdb_seek()");
      }
      break;
      /* end CLIENT_REQUEST switch statement*/

    /* unknown request code */
    default: 
#ifdef VERBOSE_MSG
      fprintf(stderr, 
	      "dbskkd-cdb: pid %d unknown client request code = %d\n",
	       getpid(), combuf[0]);
#endif /* VERBOSE_MSG */
      exit(1);
    } /* end request code parsing */
  }

  /* close dictionary db file */
  cdb_free(&diccdb);
  if (close(dicfd) != 0)
    diesys("cannot close() the dictionary file");

  exit(0);
}

/* end of program */
