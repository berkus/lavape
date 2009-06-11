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

macx:links1.target = ../../bin/Components/lib$${TARGET}.dylib
else:links1.target = ../../bin/Components/lib$${TARGET}.so
macx:links1.commands = ln -s -f $${DESTDIR}/LavaStream.framework/Versions/1/$${TARGET} ../../bin/Components/lib$${TARGET}.dylib
else:links1.commands = ln -s -f $${DESTDIR}/lib/lib$${TARGET}.so ../../bin/Components/lib$${TARGET}.so
QMAKE_EXTRA_TARGETS += links1
macx:POST_TARGETDEPS += ../../bin/Components/lib$${TARGET}.dylib
else:POST_TARGETDEPS += ../../bin/Components/lib$${TARGET}.so

links2.target = ../../bin/Components/LavaStream.lava
#links2.depends = ../../samples/LavaStream.lava
links2.commands = ln -s -f ../../samples/LavaStream.lava ../../bin/Components/LavaStream.lava
QMAKE_EXTRA_TARGETS += links2
POST_TARGETDEPS += ../../bin/Components/LavaStream.lava

