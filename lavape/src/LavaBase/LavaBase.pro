TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
PRO_EXPORT = LAVABASE
win32:SOURCES *= $$system(dir /b *.cpp)
else:SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaBase_all.cpp
win32:HEADERS *= $$system(dir /b *.h)
else:HEADERS *= $$system(ls *.h)
HEADERS -= LavaBase_all.h
include(../../lpc.prf)
CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header
win32:PH_FILES = $$system(dir /b *.ph)
else:PH_FILES = $$system(ls *.ph)
SOURCES *= SyntaxG.cpp
#message($$SOURCES:$$PH_FILES)
win32:FORMS = $$system(ls *.ui)
else:FORMS = $$system(ls *.ui)
INCLUDEPATH = ../disco ../wxqDocView
macx:DEFINES += __Darwin __UNIX__
else:win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += NEEDS_INT_DEFINED QT_THREAD_SUPPORT WXQDOCVIEW_EXPORT
QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaBase.framework/Versions/1/LavaBase
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView
else:LIBS += -ldisco -lwxqDocView
PRECOMPILED_HEADER = LavaBase_all.h
