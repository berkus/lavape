export SHELL=/bin/sh
#$(warning $(SHELL))

export LAVADIR=$(shell pwd)

ifeq ($(LD_LIBRARY_PATH),)
  $(error LD_LIBRARY_PATH must be defined!)
endif
ifeq ($(QTDIR),)
QTDIR = /usr
endif

all: dirs links build
#all: dirs links Precompiler LavaStream build

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
Interpreter:
	cd src/Interpreter; $(MAKE)
LavaStream:
	cd src/LavaStream; $(MAKE)
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

ITPF = src/Interpreter/Constructs.ph \
  src/Interpreter/Constructs.cpp \
  src/Interpreter/Check.cpp \
  src/Interpreter/Check.h \
  src/Interpreter/Tokens.ph \
  src/Interpreter/Tokens.cpp \
  src/Interpreter/VisitorTraversal.cpp \
  src/Interpreter/TableVisitor.cpp \
  src/Interpreter/ClosedLevelVisitor.cpp

links: $(ITPF) \
	bin/Components/LCompoImpl.lcom.lnk \
	bin/Components/LavaStream.lava \
	bin/assistant \
	lib/libQtCore.so.4 \
	lib/libQtGui.so.4 \
	lib/libQtNetwork.so.4 \
	lib/libQtXml.so.4 \
	lib/libQtAssistantClient.so.4

src/Interpreter/Constructs.ph: src/LavaExecs/Constructs.ph
	cd src/Interpreter; $(LN) ../LavaExecs/Constructs.ph .
src/Interpreter/Constructs.cpp: src/LavaExecs/Constructs.cpp
	cd src/Interpreter; $(LN) ../LavaExecs/Constructs.cpp .
src/Interpreter/Check.cpp: src/LavaExecs/Check.cpp
	cd src/Interpreter; $(LN) ../LavaExecs/Check.cpp .
src/Interpreter/Check.h: src/LavaExecs/Check.h
	cd src/Interpreter; $(LN) ../LavaExecs/Check.h .
src/Interpreter/Tokens.ph: src/LavaExecs/Tokens.ph
	cd src/Interpreter; $(LN) ../LavaExecs/Tokens.ph .
src/Interpreter/Tokens.cpp: src/LavaExecs/Tokens.cpp
	cd src/Interpreter; $(LN) ../LavaExecs/Tokens.cpp .
src/Interpreter/VisitorTraversal.cpp: src/LavaExecs/VisitorTraversal.cpp
	cd src/Interpreter; $(LN) ../LavaExecs/VisitorTraversal.cpp .
src/Interpreter/TableVisitor.cpp: src/LavaExecs/TableVisitor.cpp
	cd src/Interpreter; $(LN) ../LavaExecs/TableVisitor.cpp .
src/Interpreter/ClosedLevelVisitor.cpp: src/LavaExecs/ClosedLevelVisitor.cpp
	cd src/Interpreter; $(LN) ../LavaExecs/ClosedLevelVisitor.cpp .
	
bin/Components/LCompoImpl.lcom.lnk: samples/LCompoImpl.lcom
	rm -f $@; $(LN) $< $@
bin/Components/LavaStream.lava: samples/LavaStream.lava
	rm -f $@; $(LN) $< $@
bin/assistant:
	rm -f $@; \
	$(LN) $(QASSISTANT) $@ 
#	else echo ; echo ++++ error: Lava online help works only if QTDIR is defined!; echo ; exit 1; fi

lib/libQtCore.so.4: $(QTDIR)/lib/libQtCore.so.4
	rm -f $@; $(LN) $< $@
lib/libQtGui.so.4: $(QTDIR)/lib/libQtGui.so.4
	rm -f $@; $(LN) $< $@
lib/libQtNetwork.so.4: $(QTDIR)/lib/libQtNetwork.so.4
	rm -f $@; $(LN) $< $@
lib/libQtXml.so.4: $(QTDIR)/lib/libQtXml.so.4
	rm -f $@; $(LN) $< $@
lib/libQtAssistantClient.so.4: $(QTDIR)/lib/libQtAssistantClient.so.4
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

build:  $(SUBPRO)
#	cd src/disco && $(MAKE) \
#	&& cd ../LavaPE && $(MAKE) \
#	&& cd ../Lava && $(MAKE)

bindist:
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
	cd ../../bin; cp LavaPE Lava ../lavape/bin; cp $(QTDIR)/bin/assistant ../lavape/bin; \
	cd ../lavape/bin; strip LavaPE Lava assistant; \
	cd ../../bin; cp LavaIcons/* ../lavape/bin/LavaIcons; cp std/* ../lavape/bin/std; \
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
	cp $(QTDIR)/lib/libQtCore.so.4 $(QTDIR)/lib/libQtGui.so.4 $(QTDIR)/lib/libQtNetwork.so.4 $(QTDIR)/lib/libQtXml.so.4 $(QTDIR)/lib/libQtAssistantClient.so.4 lib; \
	cd lib; strip *.so*; \
	cd ../..; tar cjf /windows/D/lavape-0.9.0-linux-bin.tar.bz2 lavape; ls -l /windows/D/lavape-*; rm -rf lavape


clean: cleanall binclean libclean
distclean: cleanall binclean libclean

binclean:
	cd $(LAVADIR)/bin; rm -f LavaPE$(EXESUFFIX) Lava$(EXESUFFIX) LPC$(EXESUFFIX)

libclean:
	cd $(LAVADIR)/lib; rm -f *$(DLLSUFFIX)

