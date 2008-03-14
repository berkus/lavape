export SHELL=/bin/sh
#$(warning $(SHELL))

export LAVADIR=$(shell pwd)
export LAVADIST=$(LAVADIR)/lavadist

#ifeq ($(LD_LIBRARY_PATH),)
#  $(error LD_LIBRARY_PATH must be defined!)
#endif

QBIN = $(shell ./qbin.sh)
ifeq ($(QBIN),)
  $(error Qt tools/executables couldn't be found!)
endif
QINCL = $(shell ./qincl.sh)
QLIB = $(shell ./qlib.sh)
QSUFF = $(shell ./qsuff.sh)

SUBPRO=disco Precompiler wxqDocView LavaBase LavaPE_UI LavaExecs LavaGUI LavaPE Lava_UI Interpreter LavaStream Lava

all: dirs links $(SUBPRO)

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
  src/Interpreter/ClosedLevelVisitor.cpp \
 	bin/Components/LCompoImpl.lcom.lnk \
	bin/Components/LavaStream.lava
 
ifeq ($(OPSYS),Darwin)
links: $(ITPF) \
	bin/assistant.app \
	lib/QtAssistant lib/QtCore lib/QtGui lib/QtNetwork lib/QtXml
else
links: $(ITPF) \
	bin/assistant
endif

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
	rm -f $@; cd bin/Components; $(LN) ../../samples/LCompoImpl.lcom LCompoImpl.lcom.lnk
bin/Components/LavaStream.lava: samples/LavaStream.lava
	rm -f $@; cd bin/Components; $(LN) ../../samples/LavaStream.lava
ifeq ($(OPSYS),Darwin)
bin/assistant.app:
	cp -R $(QBIN)/assistant.app $@; \
	install_name_tool -change $(QLIB)/QtAssistant.framework/Versions/4/QtAssistant $(LAVADIR)/lib/QtAssistant \
	-change $(QLIB)/QtCore.framework/Versions/4/QtCore $(LAVADIR)/lib/QtCore \
	-change $(QLIB)/QtGui.framework/Versions/4/QtGui $(LAVADIR)/lib/QtGui \
	-change $(QLIB)/QtNetwork.framework/Versions/4/QtNetwork $(LAVADIR)/lib/QtNetwork \
	-change $(QLIB)/QtXml.framework/Versions/4/QtXml $(LAVADIR)/lib/QtXml $@/Contents/MacOS/assistant
lib/QtAssistant:
	cp -f $(QLIB)/QtAssistant.framework/Versions/4/QtAssistant $@; \
	install_name_tool -change $(QLIB)/QtAssistant.framework/Versions/4/QtAssistant $(LAVADIR)/lib/QtAssistant \
	-change $(QLIB)/QtCore.framework/Versions/4/QtCore $(LAVADIR)/lib/QtCore \
	-change $(QLIB)/QtGui.framework/Versions/4/QtGui $(LAVADIR)/lib/QtGui \
	-change $(QLIB)/QtNetwork.framework/Versions/4/QtNetwork $(LAVADIR)/lib/QtNetwork \
	-change $(QLIB)/QtXml.framework/Versions/4/QtXml $(LAVADIR)/lib/QtXml \
	-id $(LAVADIR)/lib/QtAssistant $@
lib/QtCore:
	cp -f $(QLIB)/QtCore.framework/Versions/4/QtCore $@; \
	install_name_tool -change $(QLIB)/QtAssistant.framework/Versions/4/QtAssistant $(LAVADIR)/lib/QtAssistant \
	-change $(QLIB)/QtCore.framework/Versions/4/QtCore $(LAVADIR)/lib/QtCore \
	-change $(QLIB)/QtGui.framework/Versions/4/QtGui $(LAVADIR)/lib/QtGui \
	-change $(QLIB)/QtNetwork.framework/Versions/4/QtNetwork $(LAVADIR)/lib/QtNetwork \
	-change $(QLIB)/QtXml.framework/Versions/4/QtXml $(LAVADIR)/lib/QtXml \
	-id $(LAVADIR)/lib/QtCore $@
lib/QtGui:
	cp -f $(QLIB)/QtGui.framework/Versions/4/QtGui $@; \
	install_name_tool -change $(QLIB)/QtAssistant.framework/Versions/4/QtAssistant $(LAVADIR)/lib/QtAssistant \
	-change $(QLIB)/QtCore.framework/Versions/4/QtCore $(LAVADIR)/lib/QtCore \
	-change $(QLIB)/QtGui.framework/Versions/4/QtGui $(LAVADIR)/lib/QtGui \
	-change $(QLIB)/QtNetwork.framework/Versions/4/QtNetwork $(LAVADIR)/lib/QtNetwork \
	-change $(QLIB)/QtXml.framework/Versions/4/QtXml $(LAVADIR)/lib/QtXml \
	-id $(LAVADIR)/lib/QtGui $@
lib/QtNetwork:
	cp -f $(QLIB)/QtNetwork.framework/Versions/4/QtNetwork $@; \
	install_name_tool -change $(QLIB)/QtAssistant.framework/Versions/4/QtAssistant $(LAVADIR)/lib/QtAssistant \
	-change $(QLIB)/QtCore.framework/Versions/4/QtCore $(LAVADIR)/lib/QtCore \
	-change $(QLIB)/QtGui.framework/Versions/4/QtGui $(LAVADIR)/lib/QtGui \
	-change $(QLIB)/QtNetwork.framework/Versions/4/QtNetwork $(LAVADIR)/lib/QtNetwork \
	-change $(QLIB)/QtXml.framework/Versions/4/QtXml $(LAVADIR)/lib/QtXml \
	-id $(LAVADIR)/lib/QtNetwork $@
lib/QtXml:
	cp -f $(QLIB)/QtXml.framework/Versions/4/QtXml $@; \
	install_name_tool -change $(QLIB)/QtAssistant.framework/Versions/4/QtAssistant $(LAVADIR)/lib/QtAssistant \
	-change $(QLIB)/QtCore.framework/Versions/4/QtCore $(LAVADIR)/lib/QtCore \
	-change $(QLIB)/QtGui.framework/Versions/4/QtGui $(LAVADIR)/lib/QtGui \
	-change $(QLIB)/QtNetwork.framework/Versions/4/QtNetwork $(LAVADIR)/lib/QtNetwork \
	-change $(QLIB)/QtXml.framework/Versions/4/QtXml $(LAVADIR)/lib/QtXml \
	-id $(LAVADIR)/lib/QtXml $@
else
bin/assistant$(SUFF):
	$(LN) $(QBIN)/assistant$(SUFF) $@
endif

lib/libQtCore.so.4: $(QLIB)/libQtCore.so.4
	rm -f $@; $(LN) $< $@
lib/libQtGui.so.4: $(QLIB)/libQtGui.so.4
	rm -f $@; $(LN) $< $@
lib/libQtNetwork.so.4: $(QLIB)/libQtNetwork.so.4
	rm -f $@; $(LN) $< $@
lib/libQtXml.so.4: $(QLIB)/libQtXml.so.4
	rm -f $@; $(LN) $< $@
lib/libQtAssistantClient.so.4: $(QLIB)/libQtAssistantClient.so.4
	rm -f $@; $(LN) $< $@

LavaStream: bin/Components/$(DLLPREFIX)LavaStream$(DLLSUFFIX)

ifeq ($(OPSYS),MINGW32)
bin/Components/LavaStream.dll: bin/LavaStream.dll
	rm -f $@; $(LN) $< $@
bin/LavaStream.dll:
	cd src/LavaStream; $(MAKE)
else
bin/Components/$(DLLPREFIX)LavaStream$(DLLSUFFIX): lib/$(DLLPREFIX)LavaStream$(DLLSUFFIX)
	rm -f $@; cd bin/Components; $(LN) ../../lib/$(DLLPREFIX)LavaStream$(DLLSUFFIX) $(DLLPREFIX)LavaStream$(DLLSUFFIX)
lib/$(DLLPREFIX)LavaStream$(DLLSUFFIX):
	cd src/LavaStream; $(MAKE)
endif

bin/cp_icons_std: src/LavaPE/Debug/cp_icons_std
	rm -f $@; $(LN) ../$< $@


bindist:
	rm -rf lavadist; mkdir lavadist; cp *.txt lavadist; \
	cd lavadist; mkdir bin; mkdir doc; \
	cd ..; tar --exclude=.svn -cjf /tmp/docarchive doc; \
	cd lavadist; tar xjf /tmp/docarchive; rm /tmp/docarchive; \
	mkdir lib; cp ../lib/*$(DLLSUFFIX) lib; \
	mkdir samples; cp ../samples/*.l* samples; \
	cd bin; mkdir LavaIcons; mkdir std; \
	cd ../../bin; cp LavaPE Lava ../lavadist/bin;
ifeq ($(OPSYS),Darwin)
	cp -R $(QBIN)/assistant.app lavadist/bin; \
	install_name_tool -change $(QLIB)/QtAssistant.framework/Versions/4/QtAssistant $(LAVADIST)/lib/QtAssistant \
	-change $(QLIB)/QtCore.framework/Versions/4/QtCore $(LAVADIST)/lib/QtCore \
	-change $(QLIB)/QtGui.framework/Versions/4/QtGui $(LAVADIST)/lib/QtGui \
	-change $(QLIB)/QtNetwork.framework/Versions/4/QtNetwork $(LAVADIST)/lib/QtNetwork \
	-change $(QLIB)/QtXml.framework/Versions/4/QtXml $(LAVADIST)/lib/QtXml lavadist/bin/assistant.app/Contents/MacOS/assistant
else
	cp -R $(QBIN)/assistant lavadist/bin; strip lavadist/bin/assistant;
endif
	cd lavadist/bin; strip LavaPE Lava;
	cd bin; cp LavaIcons/* ../lavadist/bin/LavaIcons; cp std/* ../lavadist/bin/std; \
	cp *.htm *.lava cp_icons_std ../lavadist/bin; \
	cp ../bin-install install; chmod a+x install;
	cd lavadist/bin; \
	mkdir Components; \
	cd Components; \
	ln -s ../../samples/LCompoImpl.lcom LCompoImpl.lcom.lnk; \
	ln -s ../../samples/LavaStream.lava .; \
	ln -s ../../lib/libLavaStream$(DLLSUFFIX) .;
ifeq ($(OPSYS),Darwin)
	cd lavadist/lib; \
	cp ../../lib/QtAssistant .; \
	cp ../../lib/QtCore .; \
	cp ../../lib/QtGui .; \
	cp ../../lib/QtNetwork .; \
	cp ../../lib/QtXml .;
else
	cp $(QLIB)/libQtCore.so.4 $(QLIB)/libQtGui.so.4 $(QLIB)/libQtNetwork.so.4 $(QLIB)/libQtXml.so.4 $(QLIB)/libQtAssistantClient.so.4 lib; \
	cd lib; strip *$(DLLSUFFIX)*;
endif
	tar cjf lavape-0.9.1-linux-bin.tar.bz2 lavadist; ls -l lavape-*;


clean: cleanall binclean libclean
distclean: cleanall binclean libclean

binclean:
	cd $(LAVADIR)/bin; rm -f LavaPE$(EXESUFFIX) Lava$(EXESUFFIX) LPC$(EXESUFFIX)

libclean:
	cd $(LAVADIR)/lib; rm -f *$(DLLSUFFIX)
