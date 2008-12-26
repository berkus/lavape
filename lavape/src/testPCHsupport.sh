#! /usr/bin/env sh

if [ $PCH. != . ]; then
  echo yes
  exit
fi

echo //>precomp_header.h

PRECOMP_SUPPORT=no
if gcc -x c-header precomp_header.h >/dev/null 2>&1; then
   gcc -x c++-header precomp_header.h && PRECOMP_SUPPORT=yes
fi
rm -f precomp_header.h precomp_header.h.gch

echo $PRECOMP_SUPPORT
