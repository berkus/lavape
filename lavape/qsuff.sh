#! /usr/bin/env sh

if [ -x /usr/lib/qt4/bin/moc-qt4 ]; then
  echo -qt4
elif [ -x /usr/bin/moc-qt4 ]; then
  echo -qt4
elif [ -x /usr/local/bin/moc-qt4 ]; then
  echo -qt4
else
  echo
fi
