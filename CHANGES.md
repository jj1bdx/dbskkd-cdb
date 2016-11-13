# List of changes

## 20161104

* Revise and rewrite `CHANGES` as `CHANGES.md`

## 20161003

* Change license to MIT (see `LICENSE`)

## 20131226

* Change only documents, no code change
* Revise `READMEJP` to `READMEJP.md` with UTF-8
* Delete `README` (read `README.md` instead)

## 20090206

* Version: 2.00
* Change `READMEJP` encoding to `euc-jp`
* Revise `skk-server-protocol.txt`
* Add `README` as a brief English document

## 20090131

* Add `skk-server-protocol.txt`
* Simplify and revise `READMEJP`

## 20090129

* Made CHANGES in reverse-chlonogical order

## 20090127

* makeskkcdb.sh now uses tinycdb's cdb instead of djb's cdbmake

## 20090126

* Remove ambiguity from LICENSE
* Attach LICENSE content in dbskkd-cdb.c

## 20090125

* Comment out explicit error messages before crasing
  - inetd does not handle stderr well
  - Apply the fast-fail principle
  - Setting `VERBOSE_MSG` as a compile flag reenables the messages
  - The debug messages are still useful when used with ucspi-tcp
* Insert more comments on the code
* Make DUMMYHOSTNAME shorter
* *Note: change dictionary name to* `JISYO_FILE`
  - (previously JISHO -> now *JISYO* (check ***Y***) to match the `SKK-JISYO.*` filenames
* Replace macros on dbskkd-cdb.c: `diesys(string)` -> `exit(1)`

## 20090124

* Drop functionality of supporting server request `3` with actual hostname or IP address 
  - Current syntax will be broken anyway if IPv6 should be supported
  - *No client* uses this information for authentication or anything meaningful except for showing the information to the user when optionally requested
* Replace the functionality with answering to the request with a fixed string defined in the `DUMMYHOSTNAME` macro

## 20090118

* Version: 1.99-expr (strictly *experimental*, *not* a release)
* Remove `cdb-0.75` dependencies
* Now require `tinycdb-0.76` 
* Remove chroot, setuid, setgid calls from the program (results are more like verion 1.01)
* Change Japanese docs to reflect the status

## 20090114

* Apply patches from Tatsuya Kinoshita (Debian package maintainer)
  - awk-locale.patch: Set `LC_ALL` to `C` for awk in `skktocdbm.sh`
  - datasize.patch: Increase `DATASIZE` from 2048 to 4096 so that the largest line of `SKK-JISYO.L` can be accepted
* Fix `uint32` issue of variable `datalen`, pointed out by Tatsuya

## 20090113

* Resolve BSD make dependencies of `$>` (see http://twitter.com/konosuke/status/1094925767)
* Add `cleanupcdb.sh`
* Clear gcc-4.2.1 signedness warning by quitting using unsigned chars

## 20000325

* Complete writing Japanese README
* Version: dbskkd-cdb-1.70dev

## 20000312

* List of changes from 1.01 to 1.70dev:
* Add `chroot()`s to the `SERVER_DIR`
* Include `search()` into the command loop in `main()`
* Report pid to stderr
* Use `cdb-0.75` source files instead of those of `cdb-0.55`
