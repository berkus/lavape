#!/bin/bash
#cd /opt
tar -c -j \
--exclude=bin/*.app \
--exclude=bin/assistant \
--exclude lib \
--exclude doc/LavaPE.qch \
--exclude doc/LavaPE.qhc \
--exclude=doc/ftv2* \
--exclude=doc/ftiens4.js \
--exclude=doc/diff** \
--exclude *.gch \
--exclude=LavaWWW \
--exclude='CVS*' \
--exclude=Generated \
--exclude=Makefile \
--exclude=moc_* \
--exclude=*.xcodeproj \
--exclude=*.cur \
--exclude=*.qhc \
--exclude=*.qch \
--exclude=qtc-* \
--exclude=*.bmp \
--exclude=*.xml \
--exclude=*.ldat \
--exclude=*.bash \
--exclude=*.bat \
--exclude=*.phd \
--exclude=*.bz2 \
--exclude=*.gz \
--exclude=*.proj \
--exclude='*~' \
--exclude='*.o' \
--exclude='*.so' \
--exclude='*.dylib' \
--exclude='*.d' \
--exclude='*.jar' \
--exclude='*.class' \
--exclude='*.iss' \
--exclude='.[#,a-z,A-Z]*' \
-f lavape-0.9.1-src.tar.bz2 lavape
ls -l lavape-0.9.1-src.tar.bz2
  