PRJ = LavaPE

MOCABLES=Bars.h Boxes.h FindRefsBox.h LavaPE.h LavaPEFrames.h LavaPEView.h LavaPEDoc.h InclView.h TreeView.h VTView.h LavaPEWizard.h

EXEC = LavaPE

QRC = ../LavaPE_resources.qrc

SUBPRO=disco wxqDocView LavaBase LavaPE_UI LavaExecs LavaGUI

CPP_INCLUDES = -I. -Ires/TreeIcons -Ires/TOOLBUTTONS -Ires -I../LavaBase/Generated \
	-I../LavaExecs/Generated -I../LavaPE_UI -I../LavaPE_UI/Generated -I../LavaGUI -I../LavaExecs \
	-I../wxqDocView -I../disco -I../LavaBase -I$(QTDIR)/include -I$(QTDIR)/include/Qt -I$(QTDIR)/include/QtCore -I$(QTDIR)/include/QtGui -I$(QTDIR)/include/QtNetwork -I$(QTDIR)/include/Qt3Support


CPP_FLAGS = -DEXECVIEW

include ../common.mk

