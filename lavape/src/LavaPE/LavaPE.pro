include(../../lpc.prf)

QMAKE_OBJCXXFLAGS_PRECOMPILE =
win32:SOURCES *= $$system(dir /b *.cpp)
else:SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaPE_all.cpp
win32:HEADERS *= $$system(dir /b *.h)
else:HEADERS *= $$system(ls *.h)
HEADERS -= LavaPE_all.h
QT += network gui
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase ../LavaExecs ../LavaGUI ../LavaPE_UI ../LavaPE_UI/res/TOOLBUTTONS res res/TOOLBUTTONS res/TreeIcons

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT EXECVIEW

DESTDIR = ../../bin
macx:QMAKE_LFLAGS += -F../../lib
else:QMAKE_LFLAGS += -L../../lib -Wl,-rpath,../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase -framework LavaGUI -framework LavaPE_UI -framework LavaExecs
else:LIBS += -ldisco -lwxqDocView -lLavaBase -lLavaGUI -lLavaPE_UI -lLavaExecs
CONFIG += warn_off qt debug qtestlib precompile_header
PRECOMPILED_HEADER = LavaPE_all.h
RESOURCES = ../LavaPE_res.qrc
macx:ICON = res/LavaPE.icns
