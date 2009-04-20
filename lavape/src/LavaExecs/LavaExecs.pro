TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
PRO_EXPORT = LAVAEXECS
CONFIG += warn_off \
    lpc \
    qt \
    debug \
    lib_bundle \
    precompile_header
PH_FILES = $$system(ls *.ph)
SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaExecs_all.cpp
HEADERS *= $$system(ls *.h)
HEADERS -= LavaExecs_all.h
#XXX = $$SOURCES
FORMS = $$system(ls *.ui)
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase res/TOOLBUTTONS ../LavaPE ../LavaPE_UI ../LavaPE/res ../LavaPE/res/TOOLBUTTONS ../LavaPE/res/TreeIcons
macx:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT EXECVIEW LAVAEXECS_EXPORT
QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaExecs.framework/Versions/1/LavaExecs
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaExecs_all.h
