TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header
SOURCES *= $$system(ls *.cpp)
SOURCES -= LavaPE_UI_all.cpp
HEADERS *= $$system(ls *.h)
HEADERS -= LavaPE_UI_all.h
FORMS = $$system(ls *.ui)
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase
macx:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT EXECVIEW
QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaPE_UI.framework/Versions/1/LavaPE_UI
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaPE_UI_all.h
