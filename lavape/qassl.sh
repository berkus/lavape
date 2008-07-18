#!/usr/bin/env sh

QBIN=`./qbin.sh`

if [ -d $QBIN/Assistant_adp.app ]; then
  echo Assistant_adp.app
elif [ -x $QBIN/assistant.app ]; then
  echo assistant.app
elif [ -x $QBIN/assistant_adp ]; then
  echo assistant_adp
elif [ -x $QBIN/assistant ]; then
  echo assistant
elif [ -x $QBIN/assistant_adp.exe ]; then
  echo assistant_adp.exe
elif [ -x $QBIN/assistant.exe ]; then
  echo assistant.exe
else
  echo
fi
