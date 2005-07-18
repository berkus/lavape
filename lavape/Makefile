LAVADIR=$(shell pwd)
export LAVADIR
ASSISTANT=$(shell which assistant)
export ASSISTANT

all: dirs links build

SUBPRO=Precompiler SFLsockets disco wxqDocView LavaBase LavaPE_UI LavaExecs LavaGUI LavaPE Lava_UI Interpreter LavaStream Lava

include src/common.mk
#
dirs:
	if [ ! -e bin ] ; then mkdir bin; fi; \
	if [ ! -e lib ] ; then mkdir lib; fi; \
	cd bin; \
	if [ ! -e Components ] ; then mkdir Components; fi; \
	if [ ! -e std ]; then $(LN) ../src/LavaPE/Debug/std .; fi; \
	if [ ! -e LavaIcons ]; then $(LN) ../src/LavaPE/Debug/LavaIcons .; fi; \
	cd ../src/Lava; if [ ! -e Generated ]; then mkdir Generated; fi ; \
	cd ../LavaBase; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaExecs; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaGUI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../Lava_UI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE_UI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../wxqDocView; if [ ! -e Generated ]; then mkdir Generated; fi;
#
links: bin/std.lava bin/std_1.htm bin/std_m.htm bin/std.htm bin/cp_icons_std \
  bin/Components/LCompoImpl.lcom.lnk \
	bin/Components/LavaStream.lava \
	bin/Components/libLavaStream.so \
	bin/assistant

bin/Components/LCompoImpl.lcom.lnk: samples/LCompoImpl.lcom
	rm -f $@; $(LN) ../../$< $@
bin/Components/LavaStream.lava: samples/LavaStream.lava
	rm -f $@; $(LN) ../../$< $@
bin/Components/libLavaStream.so: lib/libLavaStream.so
	rm -f $@; $(LN) ../../$< $@
lib/libLavaStream.so:
	cd src/LavaStream; $(MAKE)
bin/std.lava: src/LavaPE/Debug/std.lava
	rm -f $@; $(LN) ../$< $@
bin/std_1.htm: src/LavaPE/Debug/std_1.htm
	rm -f $@; $(LN) ../$< $@
bin/std_m.htm: src/LavaPE/Debug/std_m.htm
	rm -f $@; $(LN) ../$< $@
bin/std.htm: src/LavaPE/Debug/std.htm
	rm -f $@; $(LN) ../$< $@
bin/cp_icons_std: src/LavaPE/Debug/cp_icons_std
	rm -f $@; $(LN) ../$< $@
bin/assistant:
	rm -f $@; \
	if [ .$(ASSISTANT) != . ] ; then $(LN) $(ASSISTANT) $@ ; \
	else echo ; echo ++++ error: Lava online help works only if Qt assistant is in your PATH!; echo ; exit 1; fi

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
  cd ..; tar cjf /windows/d/lavape-0.8.4-linux-bin.tar.bz2 lavape; ls -l /windows/d/lavape-*; rm -rf lavape
#
#:
distclean: cleanall


