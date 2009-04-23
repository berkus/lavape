TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
CONFIG += warn_off \
    qt \
    debug-and-release \
    lib_bundle \
    precompile_header
win32:SOURCES *= $$system(dir /b *.cpp)
else:SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaPE_UI_all.cpp
win32:HEADERS *= $$system(dir /b *.h)
else:HEADERS *= $$system(ls *.h)
HEADERS -= LavaPE_UI_all.h
win32:FORMS = $$system(ls *.ui)
else:FORMS = $$system(ls *.ui)
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase
macx:DEFINES += __Darwin __UNIX__
else:win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += NEEDS_INT_DEFINED QT_THREAD_SUPPORT WXQDOCVIEW_EXPORT
QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaPE_UI.framework/Versions/1/LavaPE_UI
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaPE_UI_all.h
