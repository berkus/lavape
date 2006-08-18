LAVADIR=$(shell pwd)
export LAVADIR
ASSISTANT=$(shell which assistant)
export ASSISTANT

all: dirs links Precompiler LavaStream build

SUBPRO=disco Precompiler wxqDocView LavaBase LavaPE_UI LavaExecs LavaGUI LavaPE Lava_UI Interpreter LavaStream Lava

include src/common.mk

disco:
	cd src/disco; $(MAKE)
wxqDocView:
	cd src/wxqDocView; $(MAKE)
Precompiler:
	cd src/Precompiler; $(MAKE)
LavaBase:
	cd src/LavaBase; $(MAKE)
LavaGUI:
	cd src/LavaGUI; $(MAKE)
LavaExecs:
	cd src/LavaExecs; $(MAKE)
LavaPE_UI:
	cd src/LavaPE_UI; $(MAKE)
LavaPE:
	cd src/LavaPE; $(MAKE)
Lava_UI:
	cd src/Lava_UI; $(MAKE)
Interpreter: interpreter_links
	cd src/Interpreter; $(MAKE)
Lava:
	cd src/Lava; $(MAKE)

dirs:
	if [ ! -e bin ] ; then mkdir bin; fi; \
	if [ ! -e lib ] ; then mkdir lib; fi; \
	cd src/Lava; if [ ! -e Generated ]; then mkdir Generated; fi ; \
	cd ../Interpreter; if [ ! -e Generated ]; then mkdir Generated; fi ; \
	cd ../LavaBase; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaExecs; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaGUI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../Lava_UI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE_UI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../wxqDocView; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../..

links: \
	bin/Components/LCompoImpl.lcom.lnk \
	bin/Components/LavaStream.lava \
	bin/assistant

bin/Components/LCompoImpl.lcom.lnk: samples/LCompoImpl.lcom
	rm -f $@; $(LN) $< $@
bin/Components/LavaStream.lava: samples/LavaStream.lava
	rm -f $@; $(LN) $< $@

LavaStream: bin/Components/$(DLLPREFIX)LavaStream$(DLLSUFFIX)

ifeq ($(OPSYS),MINGW32)
bin/Components/LavaStream.dll: bin/LavaStream.dll
	rm -f $@; $(LN) $< $@
bin/LavaStream.dll:
	cd src/LavaStream; $(MAKE)
else
bin/Components/$(DLLPREFIX)LavaStream$(DLLSUFFIX): lib/$(DLLPREFIX)LavaStream$(DLLSUFFIX)
	rm -f $@; $(LN) $< $@
lib/$(DLLPREFIX)LavaStream$(DLLSUFFIX):
	cd src/LavaStream; $(MAKE)
endif

bin/cp_icons_std: src/LavaPE/Debug/cp_icons_std
	rm -f $@; $(LN) ../$< $@
bin/assistant:
	rm -f $@; \
	if [ .$(ASSISTANT) != . ] ; then $(LN) $(ASSISTANT) $@ ; \
	else echo ; echo ++++ error: Lava online help works only if Qt assistant is in your PATH!; echo ; exit 1; fi

build:
	cd src/Precompiler && $(MAKE) \
	&& cd ../LavaPE && $(MAKE) \
	&& cd ../Lava && $(MAKE)

bin_dist:
	if [ ! -e lavape ] ; then mkdir lavape; fi; cp *.txt lavape; \
	cd lavape; if [ ! -e bin ] ; then mkdir bin; fi; \
	if [ ! -e doc ] ; then mkdir doc; fi; \
	cd ..; tar --exclude=.svn -cjf /tmp/docarchive doc; \
	cd lavape; tar xjf /tmp/docarchive; rm /tmp/docarchive; \
	if [ ! -e lib ] ; then mkdir lib; fi; \
	if [ ! -e samples ] ; then mkdir samples; fi; \
	cp ../samples/*.l* samples; \
	cd bin; if [ ! -e LavaIcons ] ; then mkdir LavaIcons; fi; \
	if [ ! -e std ] ; then mkdir std; fi; \
	cd ../../bin; cp LavaPE Lava ../lavape/bin; \
	cp LavaIcons/* ../lavape/bin/LavaIcons; cp std/* ../lavape/bin/std; \
	cd ../lavape; \
	cp ../bin/*.htm bin; cp ../bin/*.lava bin; \
	cp ../bin/cp_icons_std bin; \
	cp ../bin-install install; chmod a+x install; \
	cd bin; \
	if [ ! -e Components ] ; then mkdir Components; fi; \
	cd Components; \
	if [ ! -e LCompoImpl.lcom.lnk ]; then ln -s ../../samples/LCompoImpl.lcom LCompoImpl.lcom.lnk; fi; \
	if [ ! -e LavaStream.lava ]; then ln -s ../../samples/LavaStream.lava .; fi; \
	if [ ! -e libLavaStream.so ]; then ln -s ../../lib/libLavaStream.so .; fi; \
	cd ../..; cp ../lib/*.so lib; \
	cp $(QTDIR)/lib/libQtCore.so $(QTDIR)/lib/libQtGui.so $(QTDIR)/lib/libQtNetwork.so $(QTDIR)/lib/libQtXml.so lib; \
	cp $(QTDIR)/bin/assistant bin; \
	cd ..; tar cjf /windows/D/lavape-0.9.0-linux-bin.tar.bz2 lavape; ls -l /windows/D/lavape-*; rm -rf lavape


distclean: cleanall


