TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
CONFIG += warn_off \
    qt \
    debug-and-release \
    lib_bundle \
    precompile_header
win32:SOURCES *= $$system(dir /b *.cpp)
else:SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaGUI_all.cpp
win32:HEADERS *= $$system(dir /b *.h)
else:HEADERS *= $$system(ls *.h)
HEADERS -= LavaGUI_all.h
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase res/TOOLBUTTONS ../LavaPE ../LavaPE_UI ../LavaPE/res ../LavaPE/res/TOOLBUTTONS ../LavaPE/res/TreeIcons
macx:DEFINES += __Darwin __UNIX__
else:win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += NEEDS_INT_DEFINED QT_THREAD_SUPPORT WXQDOCVIEW_EXPORT
QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaGUI.framework/Versions/1/LavaGUI
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaGUI_all.h
