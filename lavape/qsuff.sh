#! /usr/bin/env sh

if [ -x /usr/lib/qt4/bin/moc ]; then
  echo 
elif [ -x /usr/bin/assistant-qt4 ]; then
  echo -qt4
else
  echo
fi
