all: dirs links build
SUBPRO=disco wxqDocView LavaBase LavaPE_UI LavaExecs LavaGUI LavaPE Lava_UI Interpreter Lava
include src/common.mk
#
dirs:
	if [ ! -e bin ] ; then mkdir bin; fi; \
	if [ ! -e lib ] ; then mkdir lib; fi; \
	cd src; \
	if [ ! -e lib ] ; then mkdir lib; fi; \
	cd Lava; if [ ! -e Generated ]; then mkdir Generated; fi ; \
	cd ../LavaBase; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaExecs; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaGUI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../Lava_UI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE_UI; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../LavaPE; if [ ! -e Generated ]; then mkdir Generated; fi; \
	cd ../wxqDocView; if [ ! -e Generated ]; then mkdir Generated; fi;
#
links:
	cd bin; if [ ! -h std.lava ]; then ln -s ../src/LavaPE/std.lava .; fi; \
	cd ../src/Interpreter; \
	rm -f Constructs.ph; ln -s ../LavaExecs/Constructs.ph .; \
	rm -f Constructs.h; ln -s ../LavaExecs/Constructs.h .; \
	rm -f Constructs.cpp; ln -s ../LavaExecs/Constructs.cpp .; \
	rm -f ConstructsG.cpp; ln -s ../LavaExecs/ConstructsG.cpp .; \
	rm -f Tokens.ph; ln -s ../LavaExecs/Tokens.ph .; \
	rm -f Tokens.h; ln -s ../LavaExecs/Tokens.h .; \
	rm -f Tokens.cpp; ln -s ../LavaExecs/Tokens.cpp .; \
	rm -f TokensG.cpp; ln -s ../LavaExecs/TokensG.cpp .; \
	rm -f Check.cpp; ln -s ../LavaExecs/Check.cpp .; \
	rm -f MakeTable.cpp; ln -s ../LavaExecs/MakeTable.cpp .; \
#
build:
	cd src/Precompiler && $(MAKE); \
	cd ../LavaPE && $(MAKE); \
	cd ../Lava && $(MAKE);
