TEMPLATE = lib
include(../../lpc.prf)
QMAKE_OBJCXXFLAGS_PRECOMPILE =

CONFIG += warn_off \
    qt \
    debug-and-release \
    lib_bundle \
    precompile_header

win32:SOURCES *= $$system(dir /b *.cpp)
else:SOURCES *= $$system(ls *.cpp)
SOURCES -= disco_all.cpp
win32:HEADERS *= $$system(dir /b *.h)
else:HEADERS *= $$system(ls *.h)
HEADERS -= disco_all.h

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT DISCO_EXPORT

QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -Wl,-install_name,@executable_path/../../../../lib/disco.framework/Versions/1/disco
PRECOMPILED_HEADER = disco_all.h

helpcoll.target = ../../doc/LavaPE.qhc
helpcoll.depends = ../../doc/LavaPE.qhcp
win32:helpcoll.commands = $$[QT_INSTALL_BINS]\qcollectiongenerator ../../doc/LavaPE.qhcp -o ../../doc/LavaPE.qhc
else:helpcoll.commands = $$[QT_INSTALL_BINS]/qcollectiongenerator ../../doc/LavaPE.qhcp -o ../../doc/LavaPE.qhc
QMAKE_EXTRA_TARGETS += helpcoll
PRE_TARGETDEPS += ../../doc/LavaPE.qhc

links1.target = ../../bin/LCompoImp.lcom.lnk
links1.depends = ../../samples/LCompoImpl.lcom
links1.commands = ln -s -f ../../samples/LCompoImp.lcom ../../bin/LCompoImp.lcom.lnk
QMAKE_EXTRA_TARGETS += links1
PRE_TARGETDEPS += ../../bin/LCompoImp.lcom.lnk

#message($$[QT_INSTALL_BINS])

win32:qtass.target = ..\..\bin\assistant.exe
else:qtass.target = ../../bin/assistant

macx:qtass.depends = $$[QT_INSTALL_BINS]/Assistant.app/Contents/MacOS/Assistant
else:win32:qtass.depends = $$[QT_INSTALL_BINS]\assistant.exe
else:qtass.depends = $$[QT_INSTALL_BINS]/assistant

macx:qtass.commands = cp -pf $$[QT_INSTALL_BINS]/Assistant.app/Contents/MacOS/Assistant $$qtass.target
else:win32:qtass.commands = copy  $$[QT_INSTALL_BINS]\assistant.exe $$qtass.target
else:qtass.commands = cp -pf $$[QT_INSTALL_BINS]/assistant $$qtass.target

QMAKE_EXTRA_TARGETS += qtass
win32:PRE_TARGETDEPS += ..\..\bin\assistant.exe
else:PRE_TARGETDEPS += ../../bin/assistant
