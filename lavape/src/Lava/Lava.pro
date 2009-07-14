QMAKE_OBJCXXFLAGS_PRECOMPILE =

include(../../lpc.prf)

SOURCES *= Lava.cpp LavaDoc.cpp MainFrm.cpp
HEADERS *= Lava.h LavaDoc.h MainFrm.h

QT += network gui
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase ../LavaExecs ../LavaGUI ../Lava_UI res/TOOLBUTTONS res ../Interpreter

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT

DESTDIR = ../../bin
macx:QMAKE_LFLAGS += -F../../lib
else:QMAKE_LFLAGS += -L../../lib  -Wl,-rpath,/usr/local/lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase -framework LavaGUI -framework Lava_UI -framework Interpreter -framework LavaStream
else:LIBS += -ldisco -lwxqDocView -lLavaBase -lLavaGUI -lLava_UI -lInterpreter -lLavaStream
CONFIG += warn_off qt qtestlib precompile_header
PRECOMPILED_HEADER = Lava_all.h
RESOURCES = ../Lava_res.qrc
macx:ICON = res/Lava.icns
