LAVADIR=$(shell pwd)
export LAVADIR

all: lavadir dirs links build
lavadir:
  echo setting LAVADIR=$(LAVADIR)
SUBPRO=Precompiler SFLsockets disco wxqDocView LavaBase LavaPE_UI LavaExecs LavaGUI LavaPE Lava_UI Interpreter LavaStream Lava
include src/common.mk
#
dirs:
	if [ ! -e bin ] ; then mkdir bin; fi; \
	if [ ! -e lib ] ; then mkdir lib; fi; \
	cd bin; \
	if [ ! -e Components ] ; then mkdir Components; fi; \
	cd Components; \
	if [ ! -e LCompoImpl.lcom.lnk ]; then ln -s ../../samples/LCompoImpl.lcom LCompoImpl.lcom.lnk; fi; \
	if [ ! -e LavaStream.lava ]; then ln -s ../../samples/LavaStream.lava .; fi; \
	if [ ! -e libLavaStream.so ]; then ln -s ../../lib/libLavaStream.so .; fi; \
	cd ../../src/Lava; if [ ! -e Generated ]; then mkdir Generated; fi ; \
	cd ../LavaBase; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaExecs; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaGUI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../Lava_UI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE_UI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../wxqDocView; if [ ! -e Generated ]; then mkdir Generated; fi;
#
links:
	cd bin; \
	if [ ! -e std.lava ]; then ln -s ../src/LavaPE/Debug/std.lava .; fi; \
	if [ ! -e std ]; then ln -s ../src/LavaPE/Debug/std .; fi; \
	if [ ! -e LavaIcons ]; then ln -s ../src/LavaPE/Debug/LavaIcons .; fi; \
	if [ ! -e std_1.htm ]; then ln -s ../src/LavaPE/Debug/std_1.htm .; fi; \
	if [ ! -e std_m.htm ]; then ln -s ../src/LavaPE/Debug/std_m.htm .; fi; \
	if [ ! -e std.htm ]; then ln -s ../src/LavaPE/Debug/std.htm .; fi; \
	if [ ! -e cp_icons_std ]; then ln -s ../src/LavaPE/Debug/cp_icons_std .; fi; \
	cd ../src/Interpreter; \
	if [ ! -e Constructs.ph ]; then ln -s ../LavaExecs/Constructs.ph .; fi; \
	if [ ! -e Constructs.cpp ]; then ln -s ../LavaExecs/Constructs.cpp .; fi; \
	if [ ! -e Tokens.ph ]; then ln -s ../LavaExecs/Tokens.ph .; fi; \
	if [ ! -e Tokens.cpp ]; then ln -s ../LavaExecs/Tokens.cpp .; fi; \
	if [ ! -e Check.cpp ]; then ln -s ../LavaExecs/Check.cpp .; fi; \
	if [ ! -e Check.h ]; then ln -s ../LavaExecs/Check.h .; fi; \
	if [ ! -e MakeTable.cpp ]; then ln -s ../LavaExecs/MakeTable.cpp .; fi
#
build:
	cd src/Precompiler && $(MAKE) \
	&& cd ../LavaPE && $(MAKE) \
	&& cd ../Lava && $(MAKE);
#
bin_dist: build
	if [ ! -e lavape ] ; then mkdir lavape; fi; cp *.txt lavape; \
	cd lavape; if [ ! -e bin ] ; then mkdir bin; fi; \
	if [ ! -e doc ] ; then mkdir doc; fi; \
  cd ..; tar --exclude=CVS -cjf /tmp/docarchive doc; \
  cd lavape; tar xjf /tmp/docarchive; rm /tmp/docarchive; \
	if [ ! -e lib ] ; then mkdir lib; fi; \
	if [ ! -e samples ] ; then mkdir samples; fi; \
  cp ../samples/*.l* samples; \
  cd bin; if [ ! -e LavaIcons ] ; then mkdir LavaIcons; fi; \
  if [ ! -e std ] ; then mkdir std; fi; \
  cd ../../bin; cp LavaPE Lava ../lavape/bin; strip ../lavape/bin/LavaPE; strip  ../lavape/bin/Lava; \
  cp LavaIcons/* ../lavape/bin/LavaIcons; cp std/* ../lavape/bin/std; \
  cd ../lavape; \
  cp ../src/LavaPE/Debug/*.htm bin; cp ../src/LavaPE/Debug/*.lava bin; \
  cp ../src/LavaPE/Debug/cp_icons_std bin; \
  cp ../bin-install install; chmod a+x install; \
	cd bin; \
	if [ ! -e Components ] ; then mkdir Components; fi; \
	cd Components; \
	if [ ! -e LCompoImpl.lcom.lnk ]; then ln -s ../../samples/LCompoImpl.lcom LCompoImpl.lcom.lnk; fi; \
	if [ ! -e LavaStream.lava ]; then ln -s ../../samples/LavaStream.lava .; fi; \
	if [ ! -e libLavaStream.so ]; then ln -s ../../lib/libLavaStream.so .; fi; \
  cd ../..; cp ../lib/*.so lib; cd lib; strip *.so; cd ..; \
  cp $(QTDIR)/lib/libqt-mt.so lib; strip lib/libqt-mt.so; \
  cp $(QTDIR)/bin/assistant bin; strip bin/assistant;\
  cd ..; tar cjf lavape-0.8.4-linux-bin.tar.bz2 lavape; ls -l lavape-*
#
#:
distclean: cleanall


