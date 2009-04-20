TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header
SOURCES *= $$system(ls *.cpp)
SOURCES -= disco_all.cpp
HEADERS *= $$system(ls *.h)
HEADERS -= disco_all.h
macx:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT DISCO_EXPORT
QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -Wl,-install_name,@executable_path/../../../../lib/disco.framework/Versions/1/disco
PRECOMPILED_HEADER = disco_all.h

helpcoll.target = ../../doc/LavaPE.qhc
helpcoll.depends = ../../doc/LavaPE.qhcp
helpcoll.commands = $$[QT_INSTALL_BINS]/qcollectiongenerator ../../doc/LavaPE.qhcp -o ../../doc/LavaPE.qhc
QMAKE_EXTRA_TARGETS += helpcoll
PRE_TARGETDEPS += ../../doc/LavaPE.qhc

qtass.target = ../../bin/assistant
macx:qtass.depends = $$[QT_INSTALL_BINS]/Assistant.app/Contents/MacOS/Assistant
else:qtass.depends = $$[QT_INSTALL_BINS]/assistant
macx:qtass.commands = cp -pf $$[QT_INSTALL_BINS]/Assistant.app/Contents/MacOS/Assistant $$qtass.target
else:qtass.commands = cp -pf $$[QT_INSTALL_BINS]/assistant $$qtass.target
QMAKE_EXTRA_TARGETS += qtass
PRE_TARGETDEPS += ../../bin/assistant
