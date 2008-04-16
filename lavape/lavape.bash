#!/usr/bin/env bash

#first we determine the absolute pathname of the
#directory containing this shell procedure
if [ -L $0 ]; then
  FN=`readlink $0`
  MYDIR=$(dirname $FN)
else
  V1=$(echo $0 | grep "^/")
  if [ $V1 ]; then
    MYDIR=$(dirname $0)
  else
    MYDIR=$PWD/$(dirname $0)
  fi
fi

echo dir=$MYDIR

if [ `uname` == Darwin ]; then  
  export DYLD_LIBRARY_PATH=$MYDIR/lavadir/lib
  $MYDIR/lavadir/bin/LavaPE
else
  export LD_LIBRARY_PATH=$MYDIR/../lavape/lib
  $MYDIR/../lavape/bin/LavaPE
fi
