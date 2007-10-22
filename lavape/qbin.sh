#! /usr/bin/env sh

if [ $QTDIR. != 0. -a -d $QTDIR/include/QtCore ]; then
  echo $QTDIR/bin
elif [ -x /usr/lib/qt4/bin/moc ]; then
  echo /usr/lib/qt4/bin
elif [ -x /usr/bin/moc-qt4 ]; then
  echo /usr/bin
elif [ -x /usr/bin/moc ]; then
  echo /usr/bin
else
  echo
fi
