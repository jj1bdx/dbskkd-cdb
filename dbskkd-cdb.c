/* 
 * dbskkd-cdb.c
 * inetd-style SKK Server for D. J. Bernstein's cdb
 * $Id: dbskkd-cdb.c,v 1.12 1999/09/28 12:14:43 kenji Exp $
 */

/* 
 * Copyright (c) 1998-1999 by Kenji Rikitake. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by Kenji Rikitake
 *    for use in the dbskkd project."
 *
 * 4. The names "dbskkd" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission by Kenji Rikitake.
 *
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by Kenji Rikitake
 *    for use in the dbskkd project."
 *
 * THIS SOFTWARE IS PROVIDED BY KENJI RIKITAKE ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL KENJI RIKITAKE OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by 
 * Kenji Rikitake and many individuals. 
 * This software was originally based on the protocols mentioned in
 * SKK Server Version 3.9.3, by Yukiyoshi Kameyama and
 * other contributors.
 * 
 * For more information on the dbskkd project, please refer to 
 * <http://www.k2r.org/kenji/software/jp-dbskkd.html>.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <limits.h>
#include <syslog.h>
#include "cdb.h"

/* architectural dependencies */

#if defined(__FreeBSD__)
#include <libutil.h>
#endif /* FreeBSD */

#if defined(__FreeBSD__) || defined(__bsdi__)
#define USE_SETPROCTITLE
#endif /* __FreeBSD__ || __bsdi__ */

/* SERVER_DIR must be explicitly defined in the compile option */

/* 
 * dbskkd dictionary file 
 * The default pathnane is for FreeBSD skk-jisyo package
 */
#ifndef JISHO_FILE
#define JISHO_FILE	"/usr/local/share/skk/SKK-JISYO.L.cdb"
#endif /* JISHO_FILE */

#define	BUFSIZE		(1024)	/* max size of a request */
#define DATASIZE	(2048)	/* max size of a result */

#define CLIENT_END	'0'
#define CLIENT_REQUEST	'1'
#define CLIENT_VERSION	'2'
#define CLIENT_HOST	'3'

#define SERVER_FOUND	'1'
#define SERVER_NOT_FOUND '4'

#define STDIN	(fileno(stdin))
#define STDOUT	(fileno(stdout))

#ifdef USE_SETPROCTITLE
void setproctitle __P((const char *, ...));
#endif /* USE_SETPROCTITLE */

/* version number (always ends with space) */
char	pgmver[] = "dbskkd-cdb-1.01 ";

char	hname[BUFSIZE];	/* hostname and IP address */
int	jisho_fd;	/* file descriptor for SKK dictionary */

/*
 * error dump and exit
 */

void errorexit(reason)
char *reason;
{
  syslog(LOG_ERR, "%s: %m", reason);
  exit(1);
}

void errorexitval(descrip, value)
char *descrip;
int value;
{
  syslog(LOG_ERR, "%s=%d", descrip, value);
  exit(1);
}

/*
 * main server loop:
 * DB-style hashed database search 
 */

int search()
{	
  unsigned char	combuf[BUFSIZE];
  char data[DATASIZE];
  register unsigned char *pbuf, *key, *p;
  int length, errcode, get_status;
  unsigned int keylen;
  uint32 datalen;

  length = read(STDIN, &combuf[0], BUFSIZE - 1);
  if (length < 0) {
    errorexit("read error from stdin");
    /* NOTREACHED */
  } else if (length == 0) {
    syslog(LOG_NOTICE, "end of file detected");
    return -1;
  }

  switch (combuf[0]) {

  case CLIENT_END:
    syslog(LOG_NOTICE, "end of conversion requested");
    return -1;
    /* NOTREACHED */
    break; /* foolproof */

  case CLIENT_VERSION:
    if (write(STDOUT, pgmver, strlen(pgmver)) < 0) {
      errorexit("write error (version string)"); 
      /* NOTREACHED */
    }
    return 0;
    /* NOTREACHED */
    break; /* foolproof */

  case CLIENT_HOST:
    if (write(STDOUT, hname, strlen(hname)) < 0) {
      errorexit("write error (host info)");
      /* NOTREACHED */
    }
    return 0;
    /* NOTREACHED */
    break; /* foolproof */

  case CLIENT_REQUEST:
    /* get size of key */
    key = &combuf[1];
    for (pbuf = &combuf[1];
         *pbuf != ' ' && pbuf != &combuf[length - 1];
         pbuf++) {}
    keylen = pbuf - &combuf[1];
	
    if (keylen <= 0) {
      errorexitval("invalid keysize", keylen);
      /* NOTREACHED */
    }
    else {
      get_status = cdb_seek(jisho_fd, key, keylen, &datalen);
    }
    switch (get_status) {
    case -1:
      errorexit("fatal error on cdb_seek()");
      /* NOTREACHED */
      break; /* foolproof */
    case 1: /* found */
      if (datalen >= DATASIZE - 2) {
	errorexit("searched entry too long");
	/* NOTREACHED */
	}
      /* generate the answer string */
      data[0] = SERVER_FOUND;
      p = data + 1;
      if ((errcode = read(jisho_fd, (void *)p, datalen)) != datalen) {
	if (errcode < 0) {
	  errorexit("read error (seeked dictionary data)");
	  /* NOTREACHED */
	  }
	}
      p += datalen;
      *p = '\n';
      /* sending found code and the result data string with LF */
      if ((errcode = write(STDOUT, data, datalen + 2)) < 0) {
	errorexit("write error (converted data)");
	/* NOTREACHED */
        }
      return 0;
      /* NOTREACHED */
      break; /* foolproof */
    case 0: /* NOT found */
      /* generate the answer string */
      combuf[0] = SERVER_NOT_FOUND;
      *pbuf = '\n';
      /* sending error code and the key string with LF */
      /* this action is required by skkinput */
      if ((errcode = write(STDOUT, combuf, keylen + 2)) < 0) {
	errorexit("write error (NOT_FOUND message)");
	/* NOTREACHED */
        }
      return 0;
      /* NOTREACHED */
      break; /* foolproof */
    default: /* unknown value */ 
      errorexitval("unknown return value from cdb_seek()", get_status);
      /* NOTREACHED */
      break; /* foolproof */
    }
    break;
  default:
    errorexitval("unknown client request code", (int)combuf[0]);
    /* NOTREACHED */
    break; /* foolproof */
  }
  /* NOTREACHED */
}

/*
 * main program
 */

int main(argc, argv)
int argc;
char *argv[];
{
  struct hostent *hp;
  char *p, *lp;
  struct in_addr addr;
  struct sockaddr_in sin;
  int sval;
  int sinlen = sizeof sin;
  
  /* open syslog */
  openlog("dbskkd-cdb", LOG_PID | LOG_CONS, LOG_DAEMON);

  /* chdir to server directory */
  if (chdir(SERVER_DIR) != 0) {
    errorexit("cannot chdir() to " SERVER_DIR);
    }
  /* set umask to g-w, o-rwx */
  (void)umask(027);

  /* open dictionary cdb file */
  if ((jisho_fd = open(JISHO_FILE, O_RDONLY, S_IRUSR)) < 0) {
    errorexit("cannot open() the dictionary file " JISHO_FILE);
    /* NOTREACHED */
  }

#ifdef UCSPI_TCP
  if ((p = getenv("TCPLOCALHOST")) != NULL) {
    strncpy(hname, p, BUFSIZE - 20);
    }
  else {
    strncpy(hname, "UNKNOWN", BUFSIZE - 20);
    }
  strncat(hname, ":", 1);
  if ((p = getenv("TCPLOCALIP")) != NULL) {
    strncat(hname, p, 15);
    }
  else {
    strncat(hname, "UNKNOWN_IP", 15);
    }
#else /* UCSPI_TCP */
  /* get host info */
  if (getsockname(STDIN, (struct sockaddr*)&sin, &sinlen) < 0)
    strncpy(hname, "ERROR_GETSOCKNAME:UNKNOWN_IP", BUFSIZE - 20);
  else if (sinlen != sizeof sin || sin.sin_family != AF_INET)
    strncpy(hname, "ERROR_NOT_IPv4:UNKNOWN_IP", BUFSIZE - 20);
  else {
    hp = gethostbyaddr((const char*)&sin.sin_addr, sizeof sin.sin_addr, AF_INET);
    if (hp)
      strncpy(hname, hp->h_name, BUFSIZE - 20);
    else
      strncpy(hname, "UNKNOWN", BUFSIZE - 20);
    strncat(hname, ":", 1);
    strncat(hname, inet_ntoa(sin.sin_addr), 15);
  }
#endif /* UCSPI_TCP */

  strncat(hname, ": ", 2);

#ifdef UCSPI_TCP
  /* get peer info */
  if ((lp = getenv("TCPREMOTEHOST")) == NULL) {
    if ((lp = getenv("TCPREMOTEIP")) == NULL) {
      lp = "UNKNOWN_IP";
      }
    }
#else /* UCSPI_TCP */
  /* get peer info */
  sval = sizeof(sin);
  if (getpeername(0, (struct sockaddr *)&sin, &sval) < 0) {
    lp = "UNKNOWN_IP";
    }
  else {
    if (hp = gethostbyaddr((char *)&sin.sin_addr.s_addr,
             sizeof(sin.sin_addr.s_addr), AF_INET))
      lp = hp->h_name;
    else 
      lp = inet_ntoa(sin.sin_addr);
    }
#endif /* UCSPI_TCP */

#ifdef LOG_PEERINFO
  /* log peer info */
  syslog(LOG_NOTICE, "connected from %s", lp);
#endif /* LOG_PEERINFO */
#ifdef USE_SETPROCTITLE
  /* set process status string */
  setproctitle("serving %s", lp);
#endif /* USE_SETPROCTITLE */

  /* command loop */
  while (search() == 0) {}

  /* close dictionary db file */
  if (close(jisho_fd) != 0) 
    errorexit("cannot close() the dictionary file");
    /* NOTREACHED */

  exit(0);
}

/* end of program */
