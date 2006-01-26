# Microsoft Developer Studio Project File - Name="Lava" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Lava - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Lava.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Lava.mak" CFG="Lava - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Lava - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Lava - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/LavaVSS/Lava", VEAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Lava - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /I "..\disco" /I "..\LavaBase" /I "..\wxqDocView" /I "..\Interpreter" /I "../SFLsockets" /I "..\LavaExecs" /I "..\LavaGUI" /I "..\LavaBase\Generated" /I "..\Lava_UI" /I "..\Lava_UI\Generated" /I "res" /I "$(QT3DIR)\include" /D "WIN32" /D "INTERPRETER" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ..\disco\Release\disco.lib ..\wxqDocView\Release\wxqDocView.lib ..\Interpreter\Release\Interpreter.lib ..\LavaBase\Release\LavaBase.lib ..\LavaGUI\Release\LavaGUI.lib $(QT3DIR)\librel\qtmain.lib $(QT3DIR)\librel\qassistantclient.lib $(QT3DIR)\librel\qt-mt$(QTR).lib kernel32.lib advapi32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib /nologo /subsystem:windows /machine:I386 /out:"..\LavaPE\Release\Lava.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Lava - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "..\disco" /I "..\LavaBase" /I "..\wxqDocView" /I "..\Interpreter" /I "../SFLsockets" /I "..\LavaExecs" /I "..\LavaGUI" /I "..\LavaBase\Generated" /I "..\Lava_UI" /I "..\Lava_UI\Generated" /I "res" /I "$(QT3DIR)\include" /D "WIN32" /D "INTERPRETER" /D "QT_THREAD_SUPPORT" /D "QT_DLL" /FR /FD /c
# SUBTRACT CPP /X /YX
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\disco\Debug\disco.lib ..\wxqDocView\Debug\wxqDocView.lib ..\Interpreter\Debug\Interpreter.lib ..\LavaBase\Debug\LavaBase.lib ..\LavaGUI\Debug\LavaGUI.lib $(QT3DIR)\lib\qtmain.lib $(QT3DIR)\lib\qassistantclient.lib $(QT3DIR)\lib\qt-mt$(QTR).lib kernel32.lib advapi32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"../LavaPE/Debug/Lava.exe"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Lava - Win32 Release"
# Name "Lava - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Lava.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Lava.h

!IF  "$(CFG)" == "Lava - Win32 Release"

!ELSEIF  "$(CFG)" == "Lava - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\Lava.h
InputName=Lava

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QT3DIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaDoc.h

!IF  "$(CFG)" == "Lava - Win32 Release"

!ELSEIF  "$(CFG)" == "Lava - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaDoc.h
InputName=LavaDoc

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QT3DIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MainFrm.h

!IF  "$(CFG)" == "Lava - Win32 Release"

!ELSEIF  "$(CFG)" == "Lava - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\MainFrm.h
InputName=MainFrm

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QT3DIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Interpreter\Tokens.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bmp26.bmp
# End Source File
# Begin Source File

SOURCE=.\guibar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\IToolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Lava.ico
# End Source File
# Begin Source File

SOURCE=.\res\Lava.rc
# End Source File
# Begin Source File

SOURCE=.\res\LavaCom.ico
# End Source File
# Begin Source File

SOURCE=.\res\LavaDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\LavaTask.ico
# End Source File
# Begin Source File

SOURCE=.\res\lavatype.bmp
# End Source File
# Begin Source File

SOURCE=.\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Generated\moc_Lava.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_MainFrm.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Lava.reg
# End Source File
# Begin Source File

SOURCE=.\res\Lava.xpm
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
