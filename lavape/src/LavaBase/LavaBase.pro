TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
PH_FILES = $$system(ls *.ph)
PRO_EXPORT = LAVABASE
SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaBase_all.cpp
HEADERS *= $$system(ls *.h)
HEADERS -= LavaBase_all.h
CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header \
    lpc
#message($$SOURCES:$$HEADERS)
FORMS = $$system(ls *.ui)
INCLUDEPATH = ../disco ../wxqDocView
macx:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT LAVABASE_EXPORT
QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaBase.framework/Versions/1/LavaBase
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView
else:LIBS += -ldisco -lwxqDocView
PRECOMPILED_HEADER = LavaBase_all.h
