export

QBIN = $(shell ./qbin.sh)
ifeq ($(QBIN),)
  $(error Qt tools/executables couldn't be found!)
#else
#  $(error QBIN=$(QBIN))
endif

SUBPRO=disco Precompiler wxqDocView LavaBase LavaPE_UI LavaExecs LavaGUI LavaPE Lava_UI Interpreter LavaStream Lava

all: qhelp dirs links $(SUBPRO)

LAVADIR=$(shell pwd)
OPSYS = $(shell uname)

ifeq ($(OPSYS),MINGW32_NT-5.1)
  OPSYS = MINGW32
  OSCAT = WIN32
  LN = cp -f
else
  OSCAT = __UNIX__
  LN = ln -s -f
endif

QINCL = $(shell ./qincl.sh)
QLIB = $(shell ./qlib.sh)
QSUFF = $(shell ./qsuff.sh)
QASSLINK = $(shell ./qassl.sh)
QASS = $(QBIN)/$(QASSLINK)
ifeq ($(OPSYS),Darwin)
  QASSCALL = \".\"
else
  ifeq ($(OSCAT),WIN32)
    QASSCALL = \"assistant.exe\"
  else
    QASSCALL = \"$(QBIN)/$(QASSLINK)\"
	endif
endif

#$(error QINCL=$(QINCL), QASS=$(QASS), QASSLINK=$(QASSLINK))

include src/common.mk

ifeq ($(OSCAT),WIN32)
  EXESUFFIX=.exe
else
  EXESUFFIX=
endif

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

links: $(ITPF) bin/assistant

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
bin/assistant:
	cp $(QASS)/Contents/MacOS/Assistant bin/assistant
else
bin/assistant:
	cp $(QASS) $@
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

qhelp:
	cd doc; $(MAKE)

bindist:
	rm -rf $(HOME)/lavadistro; mkdir $(HOME)/lavadistro $(HOME)/lavadistro/lavadir; cp *.txt $(HOME)/lavadistro/lavadir; \
	cd $(HOME)/lavadistro/lavadir; mkdir bin; mkdir doc; \
	cd $(LAVADIR); tar --exclude=.svn -cjf /tmp/docarchive doc; \
	cd $(HOME)/lavadistro/lavadir; tar xjf /tmp/docarchive; rm /tmp/docarchive; \
	mkdir lib; cp $(LAVADIR)/lib/*$(DLLSUFFIX) lib; \
	mkdir samples; cp $(LAVADIR)/samples/*.l* samples; chmod -R a+rw samples; \
	cd bin; mkdir LavaIcons; mkdir std; \
	cd $(LAVADIR)/bin; cp LavaPE Lava $(HOME)/lavadistro/lavadir/bin;
ifeq ($(OPSYS),Darwin)
	cp -R $(QBIN)/assistant.app $(HOME)/lavadistro/lavadir/bin;
#	cp bin-install-mac $(HOME)/lavadistro/install.bash
else
	cp $(QASS) $(HOME)/lavadistro/lavadir/bin/assistant;
endif
	cp lava.bash $(HOME)/lavadistro/lava; cp lavape.bash $(HOME)/lavadistro/lavape; \
	cd $(HOME)/lavadistro; ln -s lavadir/ReleaseNotes.txt lavadir/SamplesReadme.txt lavadir/samples .; \
	cd $(LAVADIR)/bin; cp LavaIcons/* $(HOME)/lavadistro/lavadir/bin/LavaIcons; cp std/* $(HOME)/lavadistro/lavadir/bin/std; \
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
	$(QLIB)/QtNetwork.framework/Versions/4/QtNetwork \
	$(QLIB)/QtXml.framework/Versions/4/QtXml .; \
	cd $(HOME); rm -f Desktop/lavape-*-macos*; tar cjf Desktop/lavape-0.9.1-mac-bin.tar.bz2 lavadistro; ls -l Desktop/lavape-*
#	cd $(HOME); rm -f Desktop/lavape-*-macos*; /usr/local/makeself-2.1.5/makeself.sh --notemp --bzip2 --nomd5 lavadistro Desktop/lavape-0.9.1-macos-bin.bz2.run "the Lava Programming Environment"; ls -l Desktop/lavape-*;
else
	cd $(HOME)/lavadistro/lavadir/lib; \
	cp $(QLIB)/libQtCore.so.4 $(QLIB)/libQtDBus.so.4 $(QLIB)/libQtGui.so.4 $(QLIB)/libQtNetwork.so.4 $(QLIB)/libQtXml.so.4 $(QLIB)/libQtAssistantClient.so.4 .; \
	cd $(HOME); rm -f Desktop/lavape-*-bin.*; tar cjf Desktop/lavape-0.9.1-linux-bin.tar.bz2 lavadistro; ls -l Desktop/lavape-*;
endif


clean: cleanall binclean docclean libclean
distclean: cleanall binclean docclean libclean

ifeq ($(OPSYS),Darwin)
binclean:
	cd $(LAVADIR)/bin; rm -f LavaPE$(EXESUFFIX) Lava$(EXESUFFIX) LPC$(EXESUFFIX); rm -rf $(QASSLINK)
else
binclean:
	cd $(LAVADIR)/bin; rm -f LavaPE$(EXESUFFIX) Lava$(EXESUFFIX) LPC$(EXESUFFIX) $(QASSLINK)
endif

docclean:
	cd $(LAVADIR)/doc; rm -f LavaPE.qhc LavaPE.qch
	
libclean:
	cd $(LAVADIR)/lib; rm -f lib*
