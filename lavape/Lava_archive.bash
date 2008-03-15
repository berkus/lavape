#! /usr/bin/env bash
cd bin
mv LavaPE LavaPE.exe
mv Lava Lava.exe
mv LPC LPC.exe
rm -f assistant

cd ../..
tar -c -j \
--exclude=lib \
--exclude=*.app \
--exclude=*.build \
--exclude=LavaWWW \
--exclude=*.kdevelop \
--exclude=*.filelist \
--exclude=*.pcs \
--exclude=*.kdevses \
--exclude=LavaWWW \
--exclude=*.svn \
--exclude=*.DS_Store \
--exclude=Generated \
--exclude=*.cur \
--exclude=*.bmp \
--exclude=*.xml \
--exclude=*.bash \
--exclude=*.bat \
--exclude=*.phd \
--exclude='*.bz2' \
--exclude=*.gz \
--exclude=*.proj \
--exclude=linux* \
--exclude='*~' \
--exclude='*.o' \
--exclude='*.so' \
--exclude='*.dylib' \
--exclude='*.exe' \
--exclude='*.gch' \
--exclude='*.d' \
--exclude='*.jar' \
--exclude='*.iss' \
--exclude='*.nsi' \
--exclude=*.xcodeproj \
--exclude='.[#,a-z,A-Z]*' \
-f Downloads/lavape-0.9.$1-src.tar.bz2 lavape
ls -l Downloads/lavape-0.9.$1-src.tar.bz2
cd lavape/bin
mv LavaPE.exe LavaPE
mv Lava.exe Lava
mv LPC.exe LPC
