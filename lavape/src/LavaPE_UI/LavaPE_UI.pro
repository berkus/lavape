TEMPLATE = lib
include(../../lpc.prf)

QMAKE_OBJCXXFLAGS_PRECOMPILE =

CONFIG += warn_off \
    qt \
    lib_bundle \
    precompile_header

SOURCES *= LavaPEMainFrameBase.cpp
HEADERS *= LavaPE_UI_export.h LavaPEMainFrameBase.h redefines.h

win32:FORMS = $$system(dir /b *.ui)
else:FORMS = $$system(ls *.ui)
#message($$FORMS)
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT LAVAPEUI_EXPORT

QT += network widgets
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaPE_UI.framework/Versions/1/LavaPE_UI
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaPE_UI_all.h
