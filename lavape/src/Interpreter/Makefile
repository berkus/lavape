PRJ = Interpreter

MOCABLES=Debugger.h

EXEC = Interpreter.so
SUBPRO=disco wxqDocView LavaBase LavaGUI Lava_UI
DLL = LAVAEXECS

CPP_INCLUDES = -I. -I../disco -I../LavaBase -I../LavaBase/Generated -I../LavaExecs -I../LavaExecs/Generated -I../LavaExecs/res/TOOLBUTTONS -I../wxqDocView -I../Lava_UI/Generated -I../Lava_UI -I../LavaGUI

CPP_FLAGS = -DINTERPRETER -DNEEDS_INT_DEFINED -DLAVAEXECS_EXPORT

all: interpreter_links
	$(MAKE) rec_make

interpreter_links: \
	Check.cpp \
	Check.h \
	Constructs.cpp \
	Constructs.ph \
	MakeTable.cpp \
	Tokens.cpp \
	Tokens.ph


Check.cpp: ../LavaExecs/Check.cpp
	rm -f $@; $(LN) $< $@
Check.h: ../LavaExecs/Check.h
	rm -f $@; $(LN) $< $@
Constructs.cpp: ../LavaExecs/Constructs.cpp
	rm -f $@; $(LN) $< $@
Constructs.ph: ../LavaExecs/Constructs.ph
	rm -f $@; $(LN) $< $@
MakeTable.cpp: ../LavaExecs/MakeTable.cpp
	rm -f $@; $(LN) $< $@
Tokens.cpp: ../LavaExecs/Tokens.cpp
	rm -f $@; $(LN) $< $@
Tokens.ph: ../LavaExecs/Tokens.ph
	rm -f $@; $(LN) $< $@

include ../common.mk

DLL = LAVAEXECS

