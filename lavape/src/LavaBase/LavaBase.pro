TEMPLATE = lib
PRO_EXPORT = LAVABASE
include(../../lpc.prf)
win32:PH_FILES = $$system(dir /b *.ph)
else:PH_FILES = $$system(ls *.ph)
#message($$SOURCES:$$PH_FILES)

QMAKE_OBJCXXFLAGS_PRECOMPILE =

SOURCES *= AboutBox.cpp BAdapter.cpp DumpView.cpp InterprBase.cpp LavaAppBase.cpp LavaBaseDoc.cpp LavaBaseStringInit.cpp LavaThread.cpp PEBaseDoc.cpp SylTraversal.cpp SynIDTable.cpp SynIO.cpp Syntax.cpp
SOURCES *= SyntaxG.cpp
HEADERS *= AboutBox.h BAdapter.h BASEMACROS.h DumpView.h LavaAppBase.h LavaBaseDoc.h LavaBaseStringInit.h LavaThread.h PEBaseDoc.h SafeInt.h SylTraversal.h SynIDTable.h SynIO.h Syntax.h
CONFIG += warn_off \
    qt \
    lib_bundle \
    precompile_header
win32:FORMS = $$system(ls *.ui)
else:FORMS = $$system(ls *.ui)
INCLUDEPATH = ../disco ../wxqDocView

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT LAVABASE_EXPORT

QT += network
DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaBase.framework/Versions/1/LavaBase
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView
else:LIBS += -ldisco -lwxqDocView
PRECOMPILED_HEADER = LavaBase_all.h
