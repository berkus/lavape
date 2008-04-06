export

LAVADIR=$(shell pwd)

QBIN = $(shell ./qbin.sh)
ifeq ($(QBIN),)
  $(error Qt tools/executables couldn't be found!)
  $(error QTDIR=$(QTDIR))
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
LavaStream: bin/Components/$(DLLPREFIX)LavaStream$(DLLSUFFIX)
	cd src/LavaStream; $(MAKE)
Lava:
	cd src/Lava; $(MAKE)

dirs:
	if [ ! -d lib ] ; then mkdir lib; fi; \
	cd src/Lava; if [ ! -d Generated ]; then mkdir Generated; fi ; \
	cd ../Interpreter; if [ ! -d Generated ]; then mkdir Generated; fi ; \
	cd ../LavaBase; if [ ! -d Generated ]; then mkdir Generated; fi; \
	cd ../LavaExecs; if [ ! -d Generated ]; then mkdir Generated; fi; \
	cd ../LavaGUI; if [ ! -d Generated ]; then mkdir Generated; fi; \
	cd ../Lava_UI; if [ ! -d Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE_UI; if [ ! -d Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE; if [ ! -d Generated ]; then mkdir Generated; fi; \
	cd ../wxqDocView; if [ ! -d Generated ]; then mkdir Generated; fi;

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
links: $(ITPF) bin/assistant.app
else
links: $(ITPF) bin/assistant
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
	ln -s -f $(QBIN)/assistant.app $@
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

#LavaStream: bin/Components/$(DLLPREFIX)LavaStream$(DLLSUFFIX)

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
	rm -rf $(HOME)/lavadistro; mkdir $(HOME)/lavadistro $(HOME)/lavadistro/lavadir; cp *.txt $(HOME)/lavadistro/lavadir; \
	cd $(HOME)/lavadistro/lavadir; mkdir bin; mkdir doc; \
	cd $(LAVADIR); tar --exclude=.svn -cjf /tmp/docarchive doc; \
	cd $(HOME)/lavadistro/lavadir; tar xjf /tmp/docarchive; rm /tmp/docarchive; \
	mkdir lib; cp $(LAVADIR)/lib/*$(DLLSUFFIX) lib; \
	mkdir samples; cp $(LAVADIR)/samples/*.l* samples; \
	cd bin; mkdir LavaIcons; mkdir std; \
	cd $(LAVADIR)/bin; cp LavaPE Lava ../colrm $(HOME)/lavadistro/lavadir/bin;
ifeq ($(OPSYS),Darwin)
	cp -R $(QBIN)/assistant.app $(HOME)/lavadistro/lavadir/bin;
else
	cp -R $(QBIN)/assistant $(HOME)/lavadistro/lavadir/bin; strip $(HOME)/lavadistro/lavadir/bin/assistant; \
	cp /usr/local/bin/chrpath $(HOME)/lavadistro/lavadir/bin;
endif
	cp bin-install $(HOME)/lavadistro/install.bash; chmod a+x $(HOME)/lavadistro/install.bash;
	cd bin; cp LavaIcons/* $(HOME)/lavadistro/lavadir/bin/LavaIcons; cp std/* $(HOME)/lavadistro/lavadir/bin/std; \
	cp *.htm *.lava cp_icons_std $(HOME)/lavadistro/lavadir/bin; \
	cd $(HOME)/lavadistro/lavadir/bin; \
	mkdir Components; \
	cd Components; \
	ln -s ../../samples/LCompoImpl.lcom LCompoImpl.lcom.lnk; \
	ln -s ../../samples/LavaStream.lava .; \
	ln -s ../../lib/libLavaStream$(DLLSUFFIX) .;
ifeq ($(OPSYS),Darwin)
	cd $(HOME)/lavadistro/lavadir/lib; \
	cp $(QLIB)/QtAssistant.framework/Versions/4/QtAssistant \
	$(QLIB)/QtCore.framework/Versions/4/QtCore \
	$(QLIB)/QtGui.framework/Versions/4/QtGui \
	$(QLIB)/QtNetwork.framework/Versions/4/QtNetwork .; \
	cd $(HOME); rm -f Desktop/lavape-*-macos*; tar cjf Desktop/lavape-0.9.1-macos-bin.tar.bz2 lavadistro; ls -l Desktop/lavape-*;
else
	cd $(HOME)/lavadistro/lavadir/lib; strip lib*;
ifeq ($(OPSYS),SunOS)
	cd $(HOME)/lavadistro/lavadir/lib; \
	cp $(QLIB)/libQtCore.so.4 $(QLIB)/libQtGui.so.4 $(QLIB)/libQtNetwork.so.4 $(QLIB)/libQtAssistantClient.so.4 .; \
	cd $(HOME); rm -f Desktop/lavape-*-bin.*; tar cjf Desktop/lavape-0.9.1-sunos-bin.tar.bz2 lavadistro; ls -l Desktop/lavape-*;
else
	cd $(HOME)/lavadistro/lavadir/lib; \
	cp $(QLIB)/libQtCore.so.4 $(QLIB)/libQtGui.so.4 $(QLIB)/libQtNetwork.so.4 $(QLIB)/libQtAssistantClient.so.4 .; \
	cd $(HOME); rm -f Desktop/lavape-*-bin.*; tar cjf Desktop/lavape-0.9.1-linux-bin.tar.bz2 lavadistro; ls -l Desktop/lavape-*;
endif
endif


clean: cleanall binclean libclean
distclean: cleanall binclean libclean

binclean:
	cd $(LAVADIR)/bin; rm -f LavaPE$(EXESUFFIX) Lava$(EXESUFFIX) LPC$(EXESUFFIX)

libclean:
	cd $(LAVADIR)/lib; rm -f lib*
