#! /usr/bin/env bash
cd /opt
tar -c -j \
--exclude=bin \
--exclude lib \
--exclude=LavaWWW \
--exclude='.svn' \
--exclude=Generated \
--exclude=*.cur \
--exclude=*.bmp \
--exclude=*.xml \
--exclude=*.bash \
--exclude=*.bat \
--exclude=*.phd \
--exclude=*.bz2 \
--exclude=*.gz \
--exclude=*.proj \
--exclude='*~' \
--exclude='*.o' \
--exclude='*.so' \
--exclude='*.gch' \
--exclude='*.d' \
--exclude='*.jar' \
--exclude='*.iss' \
--exclude='*.nsi' \
--exclude='.[#,a-z,A-Z]*' \
-f /windows/D/lavape-0.9.$1-src.tar.bz2 lavape
ls -l /windows/D/lavape-0.9.$1-src.tar.bz2
