# Microsoft Developer Studio Project File - Name="LavaPE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LAVAPE - WIN32 RELEASE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LavaPE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LavaPE.mak" CFG="LAVAPE - WIN32 RELEASE"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LavaPE - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LavaPE - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/LavaVSS/LavaPE", LQAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LavaPE - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "." /I ".\Generated" /I "..\wxqDocView" /I "..\disco" /I "..\SFLsockets" /I "$(QTDIR)\include" /I "..\LavaBase" /I "..\LavaBase\Generated" /I "..\LavaGUI" /I "..\LavaExecs" /I "..\LavaExecs\Generated" /I "res" /I "res\ToolButtons" /I "res\TreeIcons" /I "..\LavaPE_UI" /I "..\LavaPE_UI\Generated" /D "WIN32" /D "NDEBUG" /D "EXECVIEW" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ACCESSIBILITY_SUPPORT" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "../LavaBase" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\wxqDocView\Release\wxqDocView.lib ..\LavaGUI\Release\LavaGUI.lib ..\disco\Release\disco.lib ..\LavaBase\Release\LavaBase.lib ..\LavaExecs\Release\LavaExecs.lib ..\SFLsockets\Release\SFLsockets.lib $(QTDIR)\librel\qtmain.lib $(QTDIR)\librel\qassistantclient.lib $(QTDIR)\librel\qt-mt$(QTR).lib kernel32.lib advapi32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "." /I ".\Generated" /I "..\wxqDocView" /I "..\disco" /I "..\SFLsockets" /I "$(QTDIR)\include" /I "..\LavaBase" /I "..\LavaBase\Generated" /I "..\LavaGUI" /I "..\LavaExecs" /I "..\LavaExecs\Generated" /I "res" /I "res\ToolButtons" /I "res\TreeIcons" /I "..\LavaPE_UI" /I "..\LavaPE_UI\Generated" /D "_DEBUG" /D "WIN32" /D "EXECVIEW" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_ACCESSIBILITY_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /i "../LavaBase" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\wxqDocView\Debug\wxqDocView.lib ..\LavaGUI\Debug\LavaGUI.lib ..\disco\Debug\disco.lib ..\LavaBase\Debug\LavaBase.lib ..\LavaExecs\Debug\LavaExecs.lib ..\SFLsockets\Debug\SFLsockets.lib $(QTDIR)\lib\qtmain.lib $(QTDIR)\lib\qassistantclient.lib $(QTDIR)\lib\qt-mt$(QTR).lib kernel32.lib advapi32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"../wxqDocView/Debug" /libpath:"$(QTDIR)/lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "LavaPE - Win32 Release"
# Name "LavaPE - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Bars.cpp
# End Source File
# Begin Source File

SOURCE=.\Boxes.cpp
# End Source File
# Begin Source File

SOURCE=.\ExecTree.cpp
# End Source File
# Begin Source File

SOURCE=.\FindRefsBox.cpp
# End Source File
# Begin Source File

SOURCE=.\GenHTML.cpp
# End Source File
# Begin Source File

SOURCE=.\InclView.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaPE.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaPEDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaPEFrames.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaPEStringInit.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaPEView.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaPEWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\VTView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Bars.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\Bars.h
InputName=Bars

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Boxes.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\Boxes.h
InputName=Boxes

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ExecTree.h
# End Source File
# Begin Source File

SOURCE=.\FindRefsBox.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\FindRefsBox.h
InputName=FindRefsBox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InclView.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\InclView.h
InputName=InclView

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaPE.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaPE.h
InputName=LavaPE

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaPEDoc.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaPEDoc.h
InputName=LavaPEDoc

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaPEFrames.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaPEFrames.h
InputName=LavaPEFrames

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaPEStringInit.h
# End Source File
# Begin Source File

SOURCE=.\LavaPEView.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaPEView.h
InputName=LavaPEView

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaPEWizard.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaPEWizard.h
InputName=LavaPEWizard

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\redefines.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\TreeView.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\TreeView.h
InputName=TreeView

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VTView.h

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\VTView.h
InputName=VTView

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Generated\moc_Bars.cpp

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_Boxes.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_FindRefsBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_InclView.cpp

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaPE.cpp

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaPEDoc.cpp

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaPEFrames.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaPEView.cpp

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaPEWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_TreeView.cpp

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_VTView.cpp

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

# ADD CPP /I ".."

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Group "ToolButtons"

# PROP Default_Filter "bmp xpm"
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_EditSel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_EditSel.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_FindByName.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_FindByName.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_FindRefs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_FindRefs.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_GotoDecl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_GotoDecl.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_GotoImpl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_GotoImpl.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q2_MakeGUI.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_MakeLitStr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_MakeLitStr.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_Override.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_Override.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_Return.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_Return.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_ShowForm.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_ShowForm.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_ShowOvers.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q2_ShowOvers.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_Include.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_Include.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewCompo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewCompo.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewCompoSpec.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewCompoSpec.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewEnum.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewEnum.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewEnumItem.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewEnumItem.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewFunc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewFunc.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewImpl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewImpl.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewInitiator.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewInitiator.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewInterface.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewInterface.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewMember.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewMember.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewPackage.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewPackage.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewSet.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewSet.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewVT.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q3_NewVT.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_Check.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_Check.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_CheckAll.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_CheckAll.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_Collaps.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_Collaps.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_Expand.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_Expand.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_HideEmptyOpts.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_HideEmptyOpts.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_NextComment.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_NextComment.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_NextError.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_NextError.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_PrevComment.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_PrevComment.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_PrevError.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_PrevError.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_Run.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_Run.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_ShowAllOpts.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_ShowAllOpts.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_ShowBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_ShowBar.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_ShowOpts.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q4_ShowOpts.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Cascade.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Cascade.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_CompoNew.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_CompoNew.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Copy.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Copy.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Cut.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Cut.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Delete.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_fileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_fileNew.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_FileOpen.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Insert.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_ok.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Paste.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Paste.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Redo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Redo.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Save.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Save.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_SaveAll.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_SaveAll.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_State.Value.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_TileHori.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_TileHori.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_TileVert.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_TileVert.xpm
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Undo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ToolButtons\Q_Undo.xpm
# End Source File
# End Group
# Begin Group "TreeIcons"

# PROP Default_Filter "xpm"
# Begin Source File

SOURCE=.\res\TreeIcons\PX_basicatt.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_BasicType.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_classAttr.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_classimpl.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_classint.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_coimpl.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_commentt.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_cospec.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_definitions.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_ensure.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_ensureDis.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_enum.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_enumattr.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_enumsel.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_errtab.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_exec.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_execEmpty.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_findtab.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_form.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_formAt.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_formimpl.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_function.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_initiato.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_inputs.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_invariant.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_invariantDis.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_lavafile.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_literal.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_members.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_outputs.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_overwrit.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_package.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_paramatt.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_params.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_patpara.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_require.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_requireDis.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_setatt.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_settyp.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_undefine.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_xcommask.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_xerrcommask.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TreeIcons\PX_xerrmask.xpm
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\alias.bmp
# End Source File
# Begin Source File

SOURCE=.\res\appli.rc

!IF  "$(CFG)" == "LavaPE - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\basicat1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\basicatt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BasicType.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\classAttr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\classimpl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\classint.bmp
# End Source File
# Begin Source File

SOURCE=.\res\coimpl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\commentt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\constrai.bmp
# End Source File
# Begin Source File

SOURCE=.\res\constrempty.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cospec.bmp
# End Source File
# Begin Source File

SOURCE=.\res\definitions.bmp
# End Source File
# Begin Source File

SOURCE=.\res\enum.bmp
# End Source File
# Begin Source File

SOURCE=.\res\enumattr.bmp
# End Source File
# Begin Source File

SOURCE=.\res\enumsel.bmp
# End Source File
# Begin Source File

SOURCE=.\res\errtab.bmp
# End Source File
# Begin Source File

SOURCE=.\res\execfram.ico
# End Source File
# Begin Source File

SOURCE=.\res\findtab.bmp
# End Source File
# Begin Source File

SOURCE=.\res\form.bmp
# End Source File
# Begin Source File

SOURCE=.\res\formAt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\formfram.ico
# End Source File
# Begin Source File

SOURCE=.\res\formimpl.bmp
# End Source File
# Begin Source File

SOURCE=.\res\function.bmp
# End Source File
# Begin Source File

SOURCE=.\res\idr_lava.ico
# End Source File
# Begin Source File

SOURCE=.\res\initiato.bmp
# End Source File
# Begin Source File

SOURCE=.\res\inputs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Lava.ico
# End Source File
# Begin Source File

SOURCE=.\res\LavaComponent.ico
# End Source File
# Begin Source File

SOURCE=.\res\LavaDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\lavafile.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LavaPE.rc2
# End Source File
# Begin Source File

SOURCE=.\res\literal.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\members.bmp
# End Source File
# Begin Source File

SOURCE=.\res\menutest.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Namespace.bmp
# End Source File
# Begin Source File

SOURCE=.\res\outputs.bmp
# End Source File
# Begin Source File

SOURCE=.\res\overwrit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\paramatt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\params.bmp
# End Source File
# Begin Source File

SOURCE=.\res\patpara.bmp
# End Source File
# Begin Source File

SOURCE=.\res\setatt.bmp
# End Source File
# Begin Source File

SOURCE=.\res\settyp.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tb_treen.bmp
# End Source File
# Begin Source File

SOURCE=.\res\tbnew.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolb_detai.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\undefine.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xcommask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xerrcommask.bmp
# End Source File
# Begin Source File

SOURCE=.\res\xerrmask.bmp
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\LavaPE.cnt
# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat
# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\LavaCom.ico
# End Source File
# Begin Source File

SOURCE=.\res\lavape.ico
# End Source File
# Begin Source File

SOURCE=.\LavaPE.reg
# End Source File
# Begin Source File

SOURCE=.\res\lavatask.ico
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Debug\std.lava

!IF  "$(CFG)" == "LavaPE - Win32 Release"

# Begin Custom Build
InputPath=.\Debug\std.lava
InputName=std

"Release\$(InputName).lava" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) Release\$(InputName).lava

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaPE - Win32 Debug"

!ENDIF 

# End Source File
# End Target
# End Project
