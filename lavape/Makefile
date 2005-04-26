LAVADIR=$(shell pwd)
export LAVADIR

all: lavadir dirs links build
lavadir:
  echo setting LAVADIR=$(LAVADIR)
SUBPRO=Precompiler SFLsockets disco wxqDocView LavaBase LavaPE_UI LavaExecs LavaGUI LavaPE Lava_UI Interpreter Lava LavaStream
include src/common.mk
#
dirs:
	if [ ! -e bin ] ; then mkdir bin; fi; \
	if [ ! -e lib ] ; then mkdir lib; fi; \
	cd bin; \
	if [ ! -e Components ] ; then mkdir Components; fi; \
	cd Components; \
	if [ ! -e LCompoImpl.lcom.lnk ]; then ln -s $(LAVADIR)/samples/LCompoImpl.lcom LCompoImpl.lcom.lnk; fi; \
	if [ ! -e LavaStream.lava ]; then ln -s $(LAVADIR)/samples/LavaStream.lava .; fi; \
	if [ ! -e libLavaStream.so ]; then ln -s $(LAVADIR)/lib/libLavaStream.so .; fi; \
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
	if [ ! -h std_1.htm ]; then ln -s ../src/LavaPE/Debug/std_1.htm .; fi; \
	if [ ! -h std_m.htm ]; then ln -s ../src/LavaPE/Debug/std_m.htm .; fi; \
	if [ ! -h std.htm ]; then ln -s ../src/LavaPE/Debug/std.htm .; fi; \
	if [ ! -h cp_icons_std ]; then ln -s ../src/LavaPE/Debug/cp_icons_std .; fi; \
	cd ../src/Interpreter; \
	if [ ! -h Constructs.ph ]; then ln -s ../LavaExecs/Constructs.ph .; fi; \
	if [ ! -h Constructs.cpp ]; then ln -s ../LavaExecs/Constructs.cpp .; fi; \
	if [ ! -h Tokens.ph ]; then ln -s ../LavaExecs/Tokens.ph .; fi; \
	if [ ! -h Tokens.cpp ]; then ln -s ../LavaExecs/Tokens.cpp .; fi; \
	if [ ! -h Check.cpp ]; then ln -s ../LavaExecs/Check.cpp .; fi; \
	if [ ! -h Check.h ]; then ln -s ../LavaExecs/Check.h .; fi; \
	if [ ! -h MakeTable.cpp ]; then ln -s ../LavaExecs/MakeTable.cpp .; fi
#
build:
	cd src/Precompiler && $(MAKE); \
	cd ../LavaPE && $(MAKE); \
	cd ../Lava && $(MAKE);
#
#:
distclean: $(clean_subpro) clean


