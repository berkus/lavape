include(../../lpc.prf)

QMAKE_OBJCXXFLAGS_PRECOMPILE =

SOURCES *= Bars.cpp Boxes.cpp DebuggerPE.cpp ExecTree.cpp FindRefsBox.cpp GenHTML.cpp InclView.cpp \
    LavaPE.cpp LavaPEDoc.cpp LavaPEFrames.cpp LavaPEStringInit.cpp LavaPEView.cpp LavaPEWizard.cpp \
    TreeView.cpp VTView.cpp
HEADERS *= Bars.h Boxes.h DebuggerPE.h ExecTree.h FindRefsBox.h InclView.h \
    LavaPE.h LavaPEDoc.h LavaPEFrames.h LavaPEStringInit.h LavaPEView.h LavaPEWizard.h \
    Resource.h TreeView.h VTView.h

QT += core network gui
contains(QT_MAJOR_VERSION,5):QT += widgets

INCLUDEPATH = ../disco ../wxqDocView ../LavaBase ../LavaExecs ../LavaGUI ../LavaPE_UI ../LavaPE_UI/res/TOOLBUTTONS res res/TOOLBUTTONS res/TreeIcons

win32:DEFINES += WIN32
macx:DEFINES += __Darwin __UNIX__
linux-g++:DEFINES += __LINUX __UNIX__
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT EXECVIEW QTMV=$$QT_MAJOR_VERSION

DESTDIR = ../../bin
macx:QMAKE_LFLAGS += -F../../lib
else:QMAKE_LFLAGS += -L../../lib -Wl,-rpath,../lib -Wl,-rpath,/usr/local/lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase -framework LavaGUI -framework LavaPE_UI -framework LavaExecs
else:LIBS += -ldisco -lwxqDocView -lLavaBase -lLavaGUI -lLavaPE_UI -lLavaExecs
CONFIG += warn_off qt testlib
PRECOMPILED_HEADER = LavaPE_all.h
RESOURCES = ../LavaPE_res.qrc
macx:ICON = res/LavaPE.icns
