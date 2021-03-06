#!/usr/bin/env sh

if [ -d $QTDIR ]; then
  if [ -d $QTDIR/include/QtCore ]; then
    echo $QTDIR/bin
    exit
  fi
fi

if [ -x /usr/lib/qt4/bin/moc-qt4 ]; then
  echo /usr/lib/qt4/bin
elif [ -x /usr/lib/qt4/bin/moc ]; then
  echo /usr/lib/qt4/bin
elif [ -x /usr/bin/moc-qt4 ]; then
  echo /usr/bin
elif [ -x /usr/local/bin/moc-qt4 ]; then
  echo /usr/local/bin
elif [ -d /Developer/Tools/Qt ]; then
  echo /Developer/Tools/Qt
elif [ -x /usr/bin/moc ]; then
  echo /usr/bin
else
  echo
fi
