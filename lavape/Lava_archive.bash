#!/bin/bash
cd /opt
tar -c -j \
--exclude=bin \
--exclude lib \
--exclude=LavaWWW \
--exclude='CVS*' \
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
--exclude='*.d' \
--exclude='*.jar' \
--exclude='*.iss' \
--exclude='.[#,a-z,A-Z]*' \
-f /windows/d/lavape-0.8.$1-src.tar.bz2 lavape
ls -l /windows/d/lavape-0.8.$1-src.tar.bz2
