QMAKE_OBJCXXFLAGS_PRECOMPILE =
SOURCES = $$system(ls *.cpp)
SOURCES -= LavaPE_all.cpp
HEADERS = $$system(ls *.h)
HEADERS -= LavaPE_all.h
QT += network gui
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase ../LavaExecs ../LavaGUI ../LavaPE_UI ../LavaPE_UI/res/TOOLBUTTONS res res/TOOLBUTTONS res/TreeIcons
macx:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT EXECVIEW
DESTDIR = ../../bin
macx:QMAKE_LFLAGS += -F../../lib
else:QMAKE_LFLAGS += -L../../lib -Wl,-rpath,../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase -framework LavaGUI -framework LavaPE_UI -framework LavaExecs
else:LIBS += -ldisco -lwxqDocView -lLavaBase -lLavaGUI -lLavaPE_UI -lLavaExecs
CONFIG += warn_off qt qtestlib precompile_header
PRECOMPILED_HEADER = LavaPE_all.h
RESOURCES = ../LavaPE_res.qrc
macx:ICON = res/LavaPE.icns