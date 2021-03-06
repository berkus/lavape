TEMPLATE = lib
include(../../lpc.prf)

QMAKE_OBJCXXFLAGS_PRECOMPILE =

CONFIG += warn_off \
    qt \
    lib_bundle

SOURCES *= docview.cpp mdiframes.cpp
HEADERS *= defs.h docview.h mdiframes.h setup.h wxExport.h

INCLUDEPATH = ../disco
QT += core network
contains(QT_MAJOR_VERSION,5):QT += widgets

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT WXQDOCVIEW_EXPORT

DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/wxqDocView.framework/Versions/1/wxqDocView
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco
else:LIBS += -ldisco
PRECOMPILED_HEADER = wxqDocView_all.h
