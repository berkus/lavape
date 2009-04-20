TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header \
    lpc
SOURCES *= $$system(ls *.cpp)
SOURCES -= wxqDocView_all.cpp
HEADERS *= $$system(ls *.h)
HEADERS -= wxqDocView_all.h
INCLUDEPATH = ../disco
QT += network
macx:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/wxqDocView.framework/Versions/1/wxqDocView
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco
else:LIBS += -ldisco
PRECOMPILED_HEADER = wxqDocView_all.h
