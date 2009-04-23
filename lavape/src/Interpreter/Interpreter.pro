TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =

CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header

PRO_EXPORT = LAVAEXECS
SOURCES *= $$system(ls *.cpp)
SOURCES -= Interpreter_all.cpp
HEADERS *= $$system(ls *.h)
HEADERS -= Interpreter_all.h
HEADERS *= ../LavaExecs/Check.h ../LavaExecs/Tokens.h ../LavaExecs/Constructs.h
#PH_FILES = $$system(ls ../LavaExecs/*.ph)
SOURCES *= ../LavaExecs/ConstructsG.cpp ../LavaExecs/TokensG.cpp ../LavaExecs/ClosedLevelVisitor.cpp ../LavaExecs/TableVisitor.cpp ../LavaExecs/VisitorTraversal.cpp ../LavaExecs/Check.cpp ../LavaExecs/Tokens.cpp ../LavaExecs/Constructs.cpp
#message($$SOURCES:$$PH_FILES)
INCLUDEPATH = . ../disco ../wxqDocView ../LavaBase ../LavaGUI ../Lava_UI ../LavaExecs ../LavaExecs/res/TOOLBUTTONS
mac:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT INTERPRETER LAVAEXECS_EXPORT
QT += network
DESTDIR = ../../lib

macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/Interpreter.framework/Versions/1/Interpreter
else:QMAKE_LFLAGS += -L../../lib

macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase -framework LavaGUI
else:LIBS += -ldisco -lwxqDocView -lLavaBase -lLavaGUI

PRECOMPILED_HEADER = Interpreter_all.h
