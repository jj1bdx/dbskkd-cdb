# Configuration examples

## For daemontools

The following is an example run file for daemontools service directory.

*Note: dbskkd-cdb must be invoked from a non-privileged dedicated user account.*

    #!/bin/sh
    exec 2>&1
    exec setuidgid dbskkd softlimit -d100000 -s100000 \
      tcpserver -x ./tcprules.cdb -vDRH -l localhost 127.0.0.1 skkserv \
      /usr/local/libexec/dbskkd-cdb

## For inetd

The following is a set of example inetd entries for dbskkd-cdb.

*Note: set unprivileged and dedicated uid for execution. Define **both** IPv4 and IPv6 entries for dual-stack support.*

    skkserv	stream	tcp	nowait	dbskkd	/usr/local/libexec/dbskkd-cdb dbskkd-cdb
    skkserv	stream	tcp6	nowait	dbskkd	/usr/local/libexec/dbskkd-cdb dbskkd-cdb
