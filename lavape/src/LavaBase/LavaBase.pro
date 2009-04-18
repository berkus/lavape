TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
PH_FILES = $$system(ls *.ph)
CONFIG += warn_off \
    qt \
    debug \
    precompile_header \
    lpc
SOURCES = $$system(ls *.cpp)
SOURCES -= LavaBase_all.cpp
HEADERS = $$system(ls *.h)
HEADERS -= LavaBase_all.h
FORMS = $$system(ls *.ui)
INCLUDEPATH = ../disco ../wxqDocView
macx:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT
QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaBase.framework/Versions/1/LavaBase
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView
else:LIBS += -ldisco -lwxqDocView
PRECOMPILED_HEADER = LavaBase_all.h

helpcoll.target = ../../doc/LavaPE.qhc
helpcoll.depends = ../../doc/LavaPE.qhcp
helpcoll.commands = $$[QT_INSTALL_BINS]/qcollectiongenerator ../../doc/LavaPE.qhcp -o ../../doc/LavaPE.qhc
QMAKE_EXTRA_TARGETS += helpcoll
PRE_TARGETDEPS += ../../doc/LavaPE.qhc
