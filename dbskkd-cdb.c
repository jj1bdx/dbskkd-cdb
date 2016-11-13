/*
 * dbskkd-cdb.c
 * SKK dictionary Server
 * with Daniel J. Bernstein's cdb database
 */
/*
 * MIT License (MIT)
 * Copyright © 2016 Kenji Rikitake <kenji.rikitake@acm.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sysexits.h>

/* tinycdb required */
#include "cdb.h"

/* dbskkd dictionary file name */
/* default value is for FreeBSD port japanese/ddskk */
#ifndef JISYO_FILE
#define JISYO_FILE "/usr/local/share/skk/SKK-JISYO.L.cdb"
#endif /* JISYO_FILE */

/* max size of a request */
#define BUFSIZE (1024)
/* max size of a result */
#define DATASIZE (4096)

#define CLIENT_END '0'
#define CLIENT_REQUEST '1'
#define CLIENT_VERSION '2'
#define CLIENT_HOST '3'

#define SERVER_FOUND '1'
#define SERVER_NOT_FOUND '4'

#define STDIN (fileno(stdin))
#define STDOUT (fileno(stdout))

/* these strings must be terminated with space */
#define VERSION "dbskkd-cdb-3.00 "
#define DUMMYHOSTNAME "novalue: "

/* main program */
int main(int argc, char *argv[]) {
  char combuf[BUFSIZE], data[DATASIZE];
  char *pbuf, *key, *p;
  struct cdb diccdb;
  int dicfd, ex, length;
  unsigned int keylen, datalen;

  /* open dictionary cdb file */
  if ((dicfd = open(JISYO_FILE, O_RDONLY, S_IRUSR)) < 0) {
    exit(EX_CONFIG);
  }
  /* init cdb */
  cdb_init(&diccdb, dicfd);
  /* To exit, set ex to non-zero */
  ex = 0;
  /* command loop */
  while (!ex) {
    /* Read from stdin */
    length = read(STDIN, &combuf[0], BUFSIZE - 1);
    if (length < 0) {
      exit(EX_NOINPUT);
    } else if (length == 0) {
      /* EOF detected */
      /* exit from the while loop */
      ex = 1;
      break;
    }
    /* parse request code */
    switch (combuf[0]) {
    case CLIENT_END:
      /* End of conversion requested*/
      /* exit from the while loop */
      ex = 1;
      break;
    case CLIENT_VERSION:
      if (write(STDOUT, VERSION, sizeof(VERSION) - 1) < 0) {
        exit(EX_IOERR);
      }
      break;
    case CLIENT_HOST:
      if (write(STDOUT, DUMMYHOSTNAME, sizeof(DUMMYHOSTNAME) - 1) < 0) {
        exit(EX_IOERR);
      }
      break;
    case CLIENT_REQUEST:
      /* get size of key */
      key = &combuf[1];
      for (pbuf = &combuf[1]; *pbuf != ' ' && pbuf != &combuf[length - 1];
           pbuf++) {
      }
      keylen = pbuf - &combuf[1];
      if (keylen <= 0) { /* invalid keysize */
        exit(EX_PROTOCOL);
      }
      /* lookup the cdb database */
      switch (cdb_find(&diccdb, key, keylen)) {
      case -1: /* fatal error on cdb_find() */
        exit(EX_SOFTWARE);
        break;
      case 1: /* found */
        if ((datalen = cdb_datalen(&diccdb)) >= DATASIZE - 2) {
          exit(EX_PROTOCOL);
        }
        /* generate the answer string */
        p = data;
        *p++ = SERVER_FOUND;
        if (cdb_read(&diccdb, p, datalen, cdb_datapos(&diccdb)) < 0) {
          exit(EX_SOFTWARE);
        }
        p += datalen;
        *p = '\n';
        /* sending found code and the result data string with LF */
        if (write(STDOUT, data, datalen + 2) < 0) {
          exit(EX_IOERR);
        }
        break;
      case 0: /* NOT found */
        /* generate the answer string */
        combuf[0] = SERVER_NOT_FOUND;
        *pbuf = '\n';
        /* sending error code and the key string with LF */
        /* this action is required by skkinput */
        if (write(STDOUT, combuf, keylen + 2) < 0) {
          exit(EX_IOERR);
        }
        break;
      default: /* unknown cdb return value */
        exit(EX_SOFTWARE);
        break;
      }
      break;
    /* end CLIENT_REQUEST switch clause */
    /* unknown request code */
    default:
      exit(EX_PROTOCOL);
    }
    /* end request code parsing */
  }
  /* end while loop */
  /* close dictionary db file */
  cdb_free(&diccdb);
  if (close(dicfd) != 0) {
    exit(EX_IOERR);
  }
  /* normal exit, finally */
  exit(EX_OK);
}

/* end of program */
