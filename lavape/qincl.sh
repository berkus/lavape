#! /usr/bin/env sh

if [ -n $QTDIR -a -d $QTDIR/include/QtCore ]; then
  echo $QTDIR/include
elif [ -d /usr/include/QtCore ]; then
  echo /usr/include
elif [ -d /usr/lib/qt4/include/QtCore ]; then
  echo /usr/lib/qt4/include
elif [ -d /usr/include/qt4/QtCore ]; then
  echo /usr/include/qt4
else
  echo
fi
