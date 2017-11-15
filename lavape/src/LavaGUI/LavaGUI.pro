TEMPLATE = lib
include(../../lpc.prf)

QMAKE_OBJCXXFLAGS_PRECOMPILE =

CONFIG += warn_off \
    qt \
    lib_bundle

SOURCES *= CmdExec.cpp Conv.cpp FormWid.cpp GUIProg.cpp GUIProgBase.cpp GUIWid.cpp LavaForm.cpp \
    LavaGUIFrame.cpp LavaGUIPopup.cpp LavaGUIView.cpp LavaMenu.cpp MakeGUI.cpp \
    TButton.cpp TComboBox.cpp TEdit.cpp TreeSrch.cpp TStatic.cpp
HEADERS *= CmdExec.h Conv.h FormWid.h GUIProg.h GUIProgBase.h GUIres.h GUIWid.h LavaForm.h \
    LavaGUIFrame.h LavaGUIPopup.h LavaGUIView.h LavaMenu.h MakeGUI.h \
    TButton.h TComboBox.h TEdit.h TreeSrch.h TStatic.h

INCLUDEPATH = ../disco ../wxqDocView ../LavaBase res/TOOLBUTTONS ../LavaPE ../LavaPE_UI ../LavaPE/res ../LavaPE/res/TOOLBUTTONS ../LavaPE/res/TreeIcons

win32:DEFINES += WIN32
else:DEFINES += __UNIX__
DEFINES += __$$OPSYS NEEDS_INT_DEFINED QT_THREAD_SUPPORT LAVAGUI_EXPORT

QT += core network
contains(QT_MAJOR_VERSION,5):QT += widgets

DESTDIR = ../../lib
macx:QMAKE_LFLAGS += -F../../lib -Wl,-install_name,@executable_path/../../../../lib/LavaGUI.framework/Versions/1/LavaGUI
else:QMAKE_LFLAGS += -L../../lib
macx:LIBS += -framework disco -framework wxqDocView -framework LavaBase
else:LIBS += -ldisco -lwxqDocView -lLavaBase
PRECOMPILED_HEADER = LavaGUI_all.h
