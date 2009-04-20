TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header
SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaGUI_all.cpp
HEADERS *= $$system(ls *.h)
HEADERS -= LavaGUI_all.h
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase res/TOOLBUTTONS ../LavaPE ../LavaPE_UI ../LavaPE/res ../LavaPE/res/TOOLBUTTONS ../LavaPE/res/TreeIcons
macx:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT EXECVIEW
QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaGUI.framework/Versions/1/LavaGUI
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaGUI_all.h
