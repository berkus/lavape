TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
CONFIG += warn_off \
    qt \
    debug-and-release \
    lib_bundle \
    precompile_header
win32:SOURCES *= $$system(dir /b *.cpp)
else:SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaStream_all.cpp
win32:HEADERS *= $$system(dir /b *.h)
else:HEADERS *= $$system(ls *.h)
HEADERS -= LavaStream_all.h
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase
QT += network gui
macx:DEFINES += __Darwin __UNIX__
else:win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += NEEDS_INT_DEFINED QT_THREAD_SUPPORT WXQDOCVIEW_EXPORT
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaStream.framework/Versions/1/LavaStream
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaStream_all.h
