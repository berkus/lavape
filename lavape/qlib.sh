#! /usr/bin/env sh

if [ -n $QTDIR -a -d $QTDIR/include/QtCore ]; then
  echo $QTDIR/lib
elif [ -e /usr/lib/qt4/lib/libQtCore.so ]; then
  echo /usr/lib/qt4/lib
elif [ -e /usr/lib/libQtCore.so ]; then
  echo /usr/lib
else
  echo
fi
