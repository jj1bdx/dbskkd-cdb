#!/bin/sh
# $Id: skktocdbm.sh,v 1.2 1998/12/12 11:11:30 kenji Exp $
# Converting SKK Dictionaty to cdbmake-acceptable form
# Originally by D. J. Bernstein's 12tocdbm.sh
# modified by Kenji Rikitake
# bugfix of truncated strings by Hideto Kihara
# bugfix of locale by Tatsuya Kinoshita
LC_ALL=C awk '
  /^[^;]/ {
    s = substr($0, index($0, " ") + 1)
    print "+" length($1) "," length(s) ":" $1 "->" s
  }
  END {
    print ""
  }
'
