# Microsoft Developer Studio Project File - Name="LavaGUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LavaGUI - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LavaGUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LavaGUI.mak" CFG="LavaGUI - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LavaGUI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LavaGUI - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/LavaVSS/LavaGUI", AHAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "." /I "..\disco" /I "$(QTDIR)\include" /I "..\LavaBase" /I "..\LavaPE\res" /I "..\wxqDocView" /D "WIN32" /D "EXECVIEW" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "LAVAGUI_EXPORT" /FR /YX /FD /c
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\LavaPE" /i "..\LavaBase"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\disco\Debug\disco.lib ..\LavaBase\Debug\LavaBase.lib $(QTDIR)\lib\qt-mt$(QTR).lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../LavaPE/Debug/LavaGUI.dll"

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /Gi /GX /Zi /Od /Gf /Gy /I "..\disco" /I "..\LavaBase" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXEXT" /D "LavaGUI_EXPORT" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "." /I "..\disco" /I "$(QTDIR)\include" /I "..\LavaBase" /I "..\LavaPE\res" /I "..\wxqDocView" /D "WIN32" /D "EXECVIEW" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "LAVAGUI_EXPORT" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /mktyplib203 /win32
# ADD MTL /nologo /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\disco\Debug\disco.lib ..\LavaBase\Debug\LavaBase.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\Lava\Lava___W\LavaGUI.dll"
# ADD LINK32 ..\disco\Release\disco.lib ..\LavaBase\Release\LavaBase.lib $(QTDIR)\librel\qt-mt$(QTR).lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:"..\LavaPE\Release\LavaGUI.dll"
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "LavaGUI - Win32 Debug"
# Name "LavaGUI - Win32 Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\CmdExec.cpp
# End Source File
# Begin Source File

SOURCE=.\Conv.cpp
# End Source File
# Begin Source File

SOURCE=.\FormWid.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIProg.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIProgBase.cpp
# End Source File
# Begin Source File

SOURCE=.\GUIwid.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaForm.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaGUIFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaGUIPopup.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaGUIView.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeGUI.cpp
# End Source File
# Begin Source File

SOURCE=.\TButton.cpp
# End Source File
# Begin Source File

SOURCE=.\TComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeSrch.cpp
# End Source File
# Begin Source File

SOURCE=.\TStatic.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\CmdExec.h
# End Source File
# Begin Source File

SOURCE=.\Conv.h
# End Source File
# Begin Source File

SOURCE=.\FormWid.h

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\FormWid.h
InputName=FormWid

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GUIProg.h
# End Source File
# Begin Source File

SOURCE=.\GUIProgBase.h
# End Source File
# Begin Source File

SOURCE=.\GUIwid.h
# End Source File
# Begin Source File

SOURCE=.\LavaForm.h
# End Source File
# Begin Source File

SOURCE=.\LavaGUIFrame.h

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaGUIFrame.h
InputName=LavaGUIFrame

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaGUIPopup.h

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaGUIPopup.h
InputName=LavaGUIPopup

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaGUIView.h

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaGUIView.h
InputName=LavaGUIView

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaMenu.h

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaMenu.h
InputName=LavaMenu

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MakeGUI.h
# End Source File
# Begin Source File

SOURCE=.\TButton.h

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\TButton.h
InputName=TButton

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TComboBox.h

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\TComboBox.h
InputName=TComboBox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TEdit.h

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\TEdit.h
InputName=TEdit

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TreeSrch.h
# End Source File
# Begin Source File

SOURCE=.\TStatic.h

!IF  "$(CFG)" == "LavaGUI - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\TStatic.h
InputName=TStatic

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaGUI - Win32 Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Group "ToolButtons"

# PROP Default_Filter "bmp xpm"
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Delete.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Delete.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Insert.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Insert.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_ok.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_ok.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_State.Value.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_State.Value.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Undo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Undo.xpm
# End Source File
# End Group
# Begin Source File

SOURCE=.\guibar.bmp
# End Source File
# Begin Source File

SOURCE=.\guibar1.bmp
# End Source File
# Begin Source File

SOURCE=.\guieditb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LavaGUI.rc2
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Generated\moc_FormWid.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaGUIFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaGUIPopup.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaGUIView.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_TButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_TComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_TEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_TStatic.cpp
# End Source File
# End Group
# End Target
# End Project
