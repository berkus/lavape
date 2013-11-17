#!/bin/bash

LAVADIR=$(pwd)
#echo $LAVADIR

rm -rf $HOME/lavape; mkdir $HOME/lavape; cp *.txt $HOME/lavape
cd $HOME/lavape; mkdir bin; mkdir doc; mkdir lib
cp -R $LAVADIR/lib/*.framework lib;
mkdir samples; cp $LAVADIR/samples/*.l* samples; chmod -R a+rw samples;
cd bin; mkdir LavaIcons; mkdir std;
cd $LAVADIR/bin; cp -R LavaPE.app Lava.app $HOME/lavape/bin;
ln -s /Developer/Applications/Qt5.1.1/5.1.1/clang_64/bin/Assistant.app/Contents/MacOS/Assistant $HOME/lavape/bin/Lava-Help;
cd $LAVADIR/bin; cp LavaIcons/* $HOME/lavape/bin/LavaIcons; cp std/* $HOME/lavape/bin/std;
cp *.htm *.lava cp_icons_std $HOME/lavape/bin;
cd $LAVADIR/doc; cp LavaPE.qch LavaPE.qhc $HOME/lavape/doc;
cd $HOME/lavape/bin; \
mkdir Components; \
cd Components; \
ln -s ../../samples/LCompoImpl.lcom LCompoImpl.lcom.lnk
ln -s ../../samples/LavaStream.lava .
ln -s ../../lib/LavaStream.framework/Versions/1/LavaStream libLavaStream.dylib
cd $HOME/lavape
ln -s bin/LavaPE.app bin/Lava.app .
mkdir data
chmod a+rwx data
