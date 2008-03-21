#!/usr/bin/env sh

if [ $QTDIR ]; then
  if [ -e $QTDIR/include/QtCore ]; then
    echo $QTDIR/lib
    exit
  fi
fi

if [ -e /usr/lib/qt4/lib/libQtCore.so ]; then
  echo /usr/lib/qt4/lib
elif [ -e /usr/lib/libQtCore.so ]; then
  echo /usr/lib
else
  echo
fi
