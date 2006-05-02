PRJ = Interpreter

MOCABLES=

EXEC = Interpreter.so
SUBPRO=disco wxqDocView LavaBase LavaGUI Lava_UI
DLL = LAVAEXECS

CPP_INCLUDES = -I. -I../disco -I../LavaBase -I../LavaBase/Generated -I../LavaExecs -I../LavaExecs/Generated -I../LavaExecs/res/TOOLBUTTONS -I../wxqDocView -I../Lava_UI/Generated -I../Lava_UI -I../LavaGUI

CPP_FLAGS = -DINTERPRETER -DNEEDS_INT_DEFINED -DLAVAEXECS_EXPORT

all:
	$(MAKE) rec_make

include ../common.mk

DLL = LAVAEXECS

