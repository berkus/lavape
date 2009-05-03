TEMPLATE = lib
include(../../lpc.prf)

QMAKE_OBJCXXFLAGS_PRECOMPILE =
CONFIG += warn_off \
    qt \
    debug-and-release \
    lib_bundle \
    precompile_header

SOURCES *= LavaStream.cpp
HEADERS *= LavaStream.h

INCLUDEPATH = ../disco ../wxqDocView ../LavaBase
QT += network gui

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT LAVASTREAM_EXPORT

DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaStream.framework/Versions/1/LavaStream
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaStream_all.h

links1.target = ../../bin/Components/libLavaStream.so
links1.depends = ../../lib/libLavaStream.so
links1.commands = ln -s -f ../../lib/libLavaStream.so ../../bin/Components/libLavaStream.so
QMAKE_EXTRA_TARGETS += links1
PRE_TARGETDEPS += ../../bin/Components/libLavaStream.so

links2.target = ../../bin/Components/LavaStream.lava
links2.depends = ../../samples/LavaStream.lava
links2.commands = ln -s -f ../../samples/LavaStream.lava ../../bin/Components/LavaStream.lava
QMAKE_EXTRA_TARGETS += links2
PRE_TARGETDEPS += ../../bin/Components/LavaStream.lava

