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
	if [ ! -h Constructs.ph ]; then ln -s ../LavaExecs/Constructs.ph .; fi; \
	if [ ! -h Constructs.cpp ]; then ln -s ../LavaExecs/Constructs.cpp .; fi; \
	if [ ! -h Tokens.ph ]; then ln -s ../LavaExecs/Tokens.ph .; fi; \
	if [ ! -h Tokens.cpp ]; then ln -s ../LavaExecs/Tokens.cpp .; fi; \
	if [ ! -h Check.cpp ]; then ln -s ../LavaExecs/Check.cpp .; fi; \
	if [ ! -h Check.h ]; then ln -s ../LavaExecs/Check.h .; fi; \
	if [ ! -h MakeTable.cpp ]; then ln -s ../LavaExecs/MakeTable.cpp .; fi; \
	if [ ! -h MakeTable.h ]; then ln -s ../LavaExecs/MakeTable.h .; fi;
#
build:
	cd src/Precompiler && $(MAKE); \
	cd ../LavaPE && $(MAKE); \
	cd ../Lava && $(MAKE);
