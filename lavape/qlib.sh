#! /usr/bin/env sh

if [ -d $QTDIR ]; then
  if [ -d $QTDIR/include/QtCore ]; then
    echo $QTDIR/lib
    exit
  fi
fi

if [ -e /usr/lib/qt4/lib/libQtCore.so ]; then
  echo /usr/lib/qt4/lib
elif [ -e /usr/lib/libQtCore.so ]; then
  echo /usr/lib
elif [ -d /Library/Frameworks/QtCore.framework ]; then
  echo /Library/Frameworks
else
  echo
fi
