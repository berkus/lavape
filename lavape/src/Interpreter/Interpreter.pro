TEMPLATE = lib
PRO_EXPORT = LAVAEXECS
include(../../lpc.prf)
QMAKE_OBJCXXFLAGS_PRECOMPILE =

CONFIG += warn_off \
    qt \
    lib_bundle \
    precompile_header

HEADERS *= ConstructsX.h Debugger.h LavaProgram.h \
../LavaExecs/Check.h ../LavaExecs/Tokens.h ../LavaExecs/Constructs.h
SOURCES *= ConstructsX.cpp Debugger.cpp LavaProgram.cpp \
../LavaExecs/ConstructsG.cpp ../LavaExecs/TokensG.cpp ../LavaExecs/ClosedLevelVisitor.cpp ../LavaExecs/TableVisitor.cpp ../LavaExecs/VisitorTraversal.cpp ../LavaExecs/Check.cpp ../LavaExecs/Tokens.cpp ../LavaExecs/Constructs.cpp
#message($$SOURCES:$$PH_FILES)

INCLUDEPATH = . ../disco ../wxqDocView ../LavaBase ../LavaGUI ../Lava_UI ../LavaExecs ../LavaExecs/res/TOOLBUTTONS

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT INTERPRETER_EXPORT INTERPRETER

QT += network
DESTDIR = ../../lib

macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/Interpreter.framework/Versions/1/Interpreter
else:QMAKE_LFLAGS += -L../../lib

macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase -framework LavaGUI
else:LIBS += -ldisco -lwxqDocView -lLavaBase -lLavaGUI

PRECOMPILED_HEADER = Interpreter_all.h
