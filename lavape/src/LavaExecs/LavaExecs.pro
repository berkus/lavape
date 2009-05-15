TEMPLATE = lib
PRO_EXPORT = LAVAEXECS
win32:PH_FILES = $$system(dir /b *.ph)
else:PH_FILES = $$system(ls *.ph)
include(../../lpc.prf)

QMAKE_OBJCXXFLAGS_PRECOMPILE =

SOURCES *= Check.cpp ClosedLevelVisitor.cpp ComboBar.cpp Constructs.cpp ConstructsV.cpp EditConst.cpp ExecFrame.cpp ExecUpdate.cpp ExecView.cpp \
    LavaExecsStringInit.cpp ProgText.cpp Scanner.cpp TableVisitor.cpp Tokens.cpp VisitorTraversal.cpp
SOURCES *= ConstructsG.cpp TokensG.cpp
HEADERS *= Check.h ComboBar.h ConstructsV.h ExecFrame.h ExecUpdate.h ExecView.h Scanner.h
HEADERS *= Constructs.h Tokens.h
CONFIG += warn_off \
    qt \
    debug-and-release \
    lib_bundle \
    precompile_header
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
