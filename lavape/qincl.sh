#!/usr/bin/env sh

if [ -d $QTDIR ]; then
  if [ -d $QTDIR/include/QtCore ]; then
    echo $QTDIR/include
    exit
  fi
fi

if [ -d /usr/include/QtCore ]; then
  echo /usr/include
elif [ -d /usr/lib/qt4/include/QtCore ]; then
  echo /usr/lib/qt4/include
elif [ -d /usr/include/qt4/QtCore ]; then
  echo /usr/include/qt4
elif [ -d /usr/local/include/qt4/QtCore ]; then
  echo /usr/local/include/qt4
elif [ -d /Library/Frameworks/QtCore.framework ]; then
  echo /Library/Frameworks
else
  echo
fi
