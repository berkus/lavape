PRJ = Interpreter

MOCABLES=

EXEC = Interpreter.so
SUBPRO=SFLsockets disco wxqDocView LavaBase Lava_UI LavaExecs

CPP_INCLUDES = -I. -I../disco -I../LavaBase -I../LavaExecs -I../LavaExecs/Generated -I../wxqDocView -I../Lava_UI/Generated -I../Lava_UI -I../SFLsockets -I$(QTDIR)/include

CPP_FLAGS = -DINTERPRETER -DNEEDS_INT_DEFINED

LINKS = Tokens.ph Tokens.cpp Constructs.ph Check.cpp Check.h MakeTable.cpp

all: 
	$(MAKE) links; $(MAKE) rec_make

links: $(LINKS)

include ../common.mk

DLL = LAVAEXECS

Constructs.ph: ../LavaExecs/Constructs.ph Tokens.ph
	rm -f $@; $(LN) $< $@
	
Constructs.cpp: ../LavaExecs/Constructs.cpp
	rm -f $@; $(LN) $< $@
	
Check.cpp: ../LavaExecs/Check.cpp
	rm -f $@; $(LN) $< $@
Check.h: ../LavaExecs/Check.h
	rm -f $@; $(LN) $< $@
	
Tokens.ph: ../LavaExecs/Tokens.ph
	rm -f $@; $(LN) $< $@
Tokens.cpp: ../LavaExecs/Tokens.cpp
	rm -f $@; $(LN) $< $@
	
MakeTable.cpp: ../LavaExecs/MakeTable.cpp
	rm -f $@; $(LN) $< $@

