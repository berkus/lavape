LAVADIR=$(shell pwd)
export LAVADIR

all: lavadir dirs links build
lavadir:
  echo setting LAVADIR=$(LAVADIR)
SUBPRO=lavape/src/disco lavape/src/wxqDocView lavape/src/LavaBase lavape/src/LavaPE_UI lavape/src/LavaExecs lavape/src/LavaGUI lavape/src/LavaPE lavape/src/Lava_UI lavape/src/Interpreter lavape/src/Lava
include src/common.mk
#
dirs:
	if [ ! -e bin ] ; then mkdir bin; fi; \
	if [ ! -e lib ] ; then mkdir lib; fi; \
	cd bin; \
	if [ ! -e Components ] ; then \
	  mkdir Components; cd Components; \
		ln -s $(LAVADIR)/samples/LCompoImpl.lcom LCompoImpl.lcom.lnk; \
		ln -s $(LAVADIR)/samples/LCompoSmall.lcom LCompoSmall.lcom.lnk; \
    cd ../..; \
  else \
    cd ..; \
	fi; \
	cd src/Lava; if [ ! -e Generated ]; then mkdir Generated; fi ; \
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
	if [ ! -e std ]; then ln -s ../src/LavaPE/std .; fi; \
	if [ ! -e LavaIcons ]; then ln -s ../src/LavaPE/LavaIcons .; fi; \
	if [ ! -h std_1.htm ]; then ln -s ../src/LavaPE/std_1.htm .; fi; \
	if [ ! -h std_m.htm ]; then ln -s ../src/LavaPE/std_m.htm .; fi; \
	if [ ! -h std.htm ]; then ln -s ../src/LavaPE/std.htm .; fi; \
	if [ ! -h cp_icons_std ]; then ln -s ../src/LavaPE/cp_icons_std .; fi; \
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

