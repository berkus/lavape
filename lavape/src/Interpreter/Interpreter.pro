TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =

phlinks.target = Constructs.ph
phlinks.depends = ../LavaExecs/Constructs.ph
phlinks.commands = ln -s ../LavaExecs/Constructs.ph .
phlinks.variable_out = HEADERS
QMAKE_EXTRA_TARGETS += phlinks
PRE_TARGETDEPS += Constructs.ph

phlinks2.target = Tokens.ph
phlinks2.depends = ../LavaExecs/Tokens.ph
phlinks2.commands = ln -s ../LavaExecs/Tokens.ph .
phlinks2.variable_out = HEADERS
QMAKE_EXTRA_TARGETS += phlinks2
PRE_TARGETDEPS += Tokens.ph

phlinks3.target = Check.h
phlinks3.depends = ../LavaExecs/Check.h
phlinks3.commands = ln -s ../LavaExecs/Check.h .
QMAKE_EXTRA_TARGETS += phlinks3
PRE_TARGETDEPS += Check.h

phlinks4.target = Check.cpp
phlinks4.depends = ../LavaExecs/Check.cpp
phlinks4.commands = ln -s ../LavaExecs/Check.cpp .
#phlinks4.variable_out = SOURCES
QMAKE_EXTRA_TARGETS += phlinks4
PRE_TARGETDEPS += Check.cpp

phlinks5.target = Constructs.cpp
phlinks5.depends = ../LavaExecs/Constructs.cpp
phlinks5.commands = ln -s ../LavaExecs/Constructs.cpp .
#phlinks5.variable_out = SOURCES
QMAKE_EXTRA_TARGETS += phlinks5
PRE_TARGETDEPS += Constructs.cpp

phlinks6.target = Tokens.cpp
phlinks6.depends = ../LavaExecs/Tokens.cpp
phlinks6.commands = ln -s ../LavaExecs/Tokens.cpp .
#phlinks6.variable_out = SOURCES
QMAKE_EXTRA_TARGETS += phlinks6
PRE_TARGETDEPS += Tokens.cpp

phlinks7.target = ClosedLevelVisitor.cpp
phlinks7.depends = ../LavaExecs/ClosedLevelVisitor.cpp
phlinks7.commands = ln -s ../LavaExecs/ClosedLevelVisitor.cpp .
#phlinks7.variable_out = SOURCES
QMAKE_EXTRA_TARGETS += phlinks7
PRE_TARGETDEPS += ClosedLevelVisitor.cpp

phlinks8.target = TableVisitor.cpp
phlinks8.depends = ../LavaExecs/TableVisitor.cpp
phlinks8.commands = ln -s ../LavaExecs/TableVisitor.cpp .
#phlinks8.variable_out = SOURCES
QMAKE_EXTRA_TARGETS += phlinks8
PRE_TARGETDEPS += TableVisitor.cpp

phlinks9.target = VisitorTraversal.cpp
phlinks9.depends = ../LavaExecs/VisitorTraversal.cpp
phlinks9.commands = ln -s ../LavaExecs/VisitorTraversal.cpp .
#phlinks9.variable_out = SOURCES
QMAKE_EXTRA_TARGETS += phlinks9
PRE_TARGETDEPS += VisitorTraversal.cpp

PH_FILES = $$system(ls *.ph)

PRO_EXPORT = LAVAEXECS
CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header \
    lpc
SOURCES *= $$system(ls *.cpp)
SOURCES -= Interpreter_all.cpp
HEADERS *= $$system(ls *.h)
HEADERS -= Interpreter_all.h
#message($$SOURCES:$$HEADERS)
INCLUDEPATH = . ../disco ../wxqDocView ../LavaBase ../LavaGUI ../Lava_UI ../LavaExecs/res/TOOLBUTTONS
mac:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT INTERPRETER LAVAEXECS_EXPORT
QT += network
DESTDIR = ../../lib

macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/Interpreter.framework/Versions/1/Interpreter
else:QMAKE_LFLAGS += -L../../lib

macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase -framework LavaGUI
else:LIBS += -ldisco -lwxqDocView -lLavaBase -lLavaGUI

PRECOMPILED_HEADER = Interpreter_all.h
