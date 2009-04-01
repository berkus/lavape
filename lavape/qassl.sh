#!/usr/bin/env sh

QBIN=`./qbin.sh`
#echo QBIN=$QBIN

if [ -r $QBIN/Assistant_adp.app ]; then
  echo Assistant_adp.app
elif [ -d $QBIN/Assistant.app ]; then
  echo Assistant.app
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
