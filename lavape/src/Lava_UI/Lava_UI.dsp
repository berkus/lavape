# Microsoft Developer Studio Project File - Name="Lava_UI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Lava_UI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lava_UI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lava_UI.mak" CFG="Lava_UI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lava_UI - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Lava_UI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lava_UI - Win32 Release"

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
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LAVA_UI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /I "." /I "..\disco" /I "..\wxqDocView" /I "$(QTDIR)\include" /D "WIN32" /D "QT_DLL" /D "LAVAUI_EXPORT" /D "QT_THREAD_SUPPORT" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ..\disco\Release\disco.lib ..\wxqDocView\Release\wxqDocView.lib ..\LavaBase\Release\LavaBase.lib $(QTDIR)\librel\qt-mt$(QTR).lib /nologo /dll /machine:I386 /out:"..\LavaPE\Release\Lava_UI.dll"

!ELSEIF  "$(CFG)" == "Lava_UI - Win32 Debug"

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
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "LAVA_UI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "..\disco" /I "..\wxqDocView" /I "$(QTDIR)\include" /D "QT_DLL" /D "WIN32" /D "LAVAUI_EXPORT" /D "QT_THREAD_SUPPORT" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /mktyplib203 /win32
# ADD MTL /nologo /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\disco\Debug\disco.lib ..\wxqDocView\Debug\wxqDocView.lib ..\LavaBase\Debug\LavaBase.lib $(QTDIR)\lib\qt-mt$(QTR).lib /nologo /dll /debug /machine:I386 /out:"..\LavaPE\Debug\Lava_UI.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Lava_UI - Win32 Release"
# Name "Lava_UI - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Lava_UI_export.h
# End Source File
# Begin Source File

SOURCE=.\redefines.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "UI files"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=.\cmainframe.ui

!IF  "$(CFG)" == "Lava_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "Lava_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\cmainframe.ui
InputName=cmainframe

BuildCmds= \
	$(QTDIR)\bin\uic.exe -o Generated\$(InputName).h $(InputPath) \
	$(QTDIR)\bin\uic.exe -o Generated\$(InputName).cpp -impl Generated\$(InputName).h $(InputPath) \
	

"Generated\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Generated\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "UI.H files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cmainframe.ui.h
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Generated\cmainframe.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\cmainframe.h

!IF  "$(CFG)" == "Lava_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "Lava_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\cmainframe.h
InputName=cmainframe

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_cmainframe.cpp
# End Source File
# End Group
# End Target
# End Project
