#!/usr/bin/env sh

QBIN=`./qbin.sh`
#echo QBIN=$QBIN

if [ -d $QBIN/Assistant.app ]; then
  echo Assistant.app
elif [ -x $QBIN/assistant ]; then
  echo assistant
elif [ -x $QBIN/assistant.exe ]; then
  echo assistant.exe
else
  echo
fi
