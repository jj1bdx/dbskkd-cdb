# dbskkd-cdb readme/installation note in English

for version dbskkd-cdb-2.00

by Kenji Rikitake <mailto:kenji.rikitake@acm.org>

last updated 15-AUG-2010

# Summary

dbskkd-cdb is a dictionary server for SKK Japanese Input Method system.
For building the executable file, [TinyCDB](http://www.corpit.ru/mjt/tinycdb.html)
is required.

See `skk-server-protocol.txt` for the details.

This program is tested under FreeBSD 7.3-RELEASE.

# Installation procedure

1. makeskkcdb.sh converts a plaintext SKK dictionary to the CDB format.
   The default source file name is `/usr/local/share/skk/SKK-JISYO.L`
   and the converted cdb file will be placed in the current directory
   as `SKK-JISYO.L.cdb`.  This `.cdb` file should be placed with the filename
   specified in `JISYO_FILE` macro in `dbskkd-cdb.c`.

2. do `make` to compile.  This require `cdb.h` and `libcdb.a` of TinyCDB.

3. do `make install` to copy the compile executable image to the
   executable image directory.  The default directory is `/usr/local/libexec`.

# User privilege on installation

Assign a dedicated non-privileged user for this program and invoke with
the uid.

DO NOT invoke with the root or superuser privilege.

An example of `inetd.conf` is available at `example-inetd.txt`.
An example of daemontools run file is available `at example-daemontools.txt`.

# Bug reports and comments

The source code repository and issue tracker is available in Github at
<http://github.com/jj1bdx/dbskkd-cdb>

Inquiries and comments are welcome in Japanese and English.

# Acknowledgment

Daniel J. Bernstein for his cdb, Michael Tokarev for his TinyCDB.

SKK developers and contributors including:

Makahito Sato, Mikio Nakajima, Yukiyoshi Kameyama, Masaaki Sato,
Takashi Sakamoto, Fumitoshi Ukai, Toshinori Maeno, Yasuhiro Honda,
Hayao Nakahara, Takahiro Kikuchi, Hideo Matsumoto, Hidetaka Koie,
Takao Kawamura, Hideto Kihara, Rikishi Inakazu, Makoto Matsushita,
Atsushi Yamagata, Tatsuya Kinoshita, Ryosuke Nakai.
