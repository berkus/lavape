TEMPLATE = lib
include(../../lpc.prf)

QMAKE_OBJCXXFLAGS_PRECOMPILE =
PRO_EXPORT = LAVAEXECS
win32:SOURCES *= $$system(dir /b *.cpp)
else:SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaExecs_all.cpp
win32:HEADERS *= $$system(dir /b *.h)
else:HEADERS *= $$system(ls *.h)
HEADERS -= LavaExecs_all.h
include(../../lpc.prf)
CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header
win32:PH_FILES = $$system(dir /b *.ph)
else:PH_FILES = $$system(ls *.ph)
SOURCES *= ConstructsG.cpp TokensG.cpp
#message($$SOURCES:$$PH_FILES)

win32:FORMS = $$system(ls *.ui)
else:FORMS = $$system(ls *.ui)
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase res/TOOLBUTTONS ../LavaPE ../LavaPE_UI ../LavaPE/res ../LavaPE/res/TOOLBUTTONS ../LavaPE/res/TreeIcons

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT LAVAEXECS_EXPORT EXECVIEW

QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaExecs.framework/Versions/1/LavaExecs
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaExecs_all.h
