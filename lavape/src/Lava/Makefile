MOCABLES=Lava.h LavaDoc.h MainFrm.h

EXEC = Lava
SUBPRO=disco wxqDocView LavaBase LavaGUI Lava_UI Interpreter

CPP_INCLUDES = -I. -Ires -I../LavaBase/Generated \
	-I../Interpreter -I../Lava_UI -I../Lava_UI/Generated -I../LavaGUI \
	-I../wxqDocView -I../disco -I../LavaBase -I$(QTDIR)/include
CPP_FLAGS = -DINTERPRETER

include ../common.mk


LINKS = std.lava
GEN_MOCS = Lava LavaDoc MainFrm

std.lava:
		ln -s -f ../LavaPE/std.lava .

