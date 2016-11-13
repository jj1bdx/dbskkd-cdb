# SKK server-client protocol for dbskkd-cdb (unofficial)

* Written by Kenji Rikitake
* Content last update 13-NOV-2016

## Notes

* This document is for dbskkd-cdb only
* This document does *not* describe the sub-protocols for the server completion,which is unsupported on dbskkd-cdb

## Notation

* `0` is an ASCII letter (0x30)
* `LF` is ASCII 0x0a
* `space` is ASCII 0x20
* `abc` is a string

## Size limits

* Maximum request size: 1024 bytes
* Maximum result size: 4096 bytes

## Request codes and actions

### CLIENT_END

* Request to server: `0` + `space` + `LF`
* Server terminates and disconnects after receiving the request

### CLIENT_REQUEST

* Request to server: `1` + `dictionary_key` + `space` + `LF`
* Answer if found: `1` + (`/` + `candidate`) * (number of candidates) + `/` + `LF`
* Answer if not found: `4` + `dictionary_key` + `space` + `LF`
* The dictionary keys and candidates are all variable-length strings
* The dictionary keys and candidates have the same character encoding
* The primary encoding set of SKK is ASCII + euc-jp (note: UTF-8 can also be used in some implementations)

### CLIENT_VERSION

* Request to server: `2` + `space` + `LF`
* Answer: string including server version + space, e.g., `dbskkd-cdb-2.00 `
* Note: no known client parses this string
* Implementation on dbskkd-cdb: returns the version string

### CLIENT_HOST

* Request to server: `3` + `space` + `LF`
* Answer: string including host information + space, e.g., `localhost:127.0.0.1: `
* Note: no known client parses this string 
* Implementation on dbskkd-cdb: returns dummy string `novalue: `

## Unexpected shutting down on errors

* Server disconnects the link and shuts down when received an unknown request
* Implementation on dbskkd-cdb: an internal error will cause shutdown such as that invoked by `exit(1)`
