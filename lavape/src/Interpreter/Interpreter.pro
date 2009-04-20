TEMPLATE = lib
QMAKE_OBJCXXFLAGS_PRECOMPILE =
PH_FILES = $$system(ls *.ph)
CONFIG += warn_off \
    qt \
    debug \
    lib_bundle \
    precompile_header \
    lpcSOURCES = $$system(ls *.cpp)
SOURCES -= Interpreter_all.cpp
HEADERS = $$system(ls *.h)
HEADERS -= Interpreter_all.h
INCLUDEPATH = ../disco ../wxqDocView ../LavaBase ../LavaGUI ../Lava_UI ../LavaExecs/res/TOOLBUTTONS
mac:DEFINES += __Darwin
DEFINES += __UNIX__ NEEDS_INT_DEFINED QT_THREAD_SUPPORT INTERPRETER
QT += network
DESTDIR = ../../lib

macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/Interpreter.framework/Versions/1/Interpreter
else:QMAKE_LFLAGS += -L../../lib

macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase -framework LavaGUI
else:LIBS += -ldisco -lwxqDocView -lLavaBase -lLavaGUI

PRECOMPILED_HEADER = Interpreter_all.h
