# Microsoft Developer Studio Project File - Name="Interpreter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Interpreter - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Interpreter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Interpreter.mak" CFG="Interpreter - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Interpreter - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Interpreter - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/LavaVSS/Interpreter", WDAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Interpreter - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "..\LavaExecs" /I "." /I "$(QTDIR)\include" /I "..\disco" /I "..\SFLsockets" /I "..\LavaBase" /I "..\wxqDocView" /I "..\Lava_UI" /I "..\Lava_UI\Generated" /D "QT_DLL" /D "LAVAEXECS_EXPORT" /D "WIN32" /D "INTERPRETER" /D "QT_THREAD_SUPPORT" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\disco\Release\disco.lib ..\SFLsockets\Release\SFLsockets.lib ..\LavaBase\Release\LavaBase.lib ..\wxqDocView\Release\wxqDocView.lib $(QTDIR)\librel\qt-mt$(QTR).lib ole32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\LavaPE\Release\Interpreter.dll"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Interpreter - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "." /I "$(QTDIR)\include" /I "..\disco" /I "..\SFLsockets" /I "..\LavaBase" /I "..\LavaExecs" /I "..\wxqDocView" /I "..\LavaPE" /I "..\Lava_UI" /I "..\Lava_UI\Generated" /D "QT_DLL" /D "LAVAEXECS_EXPORT" /D "WIN32" /D "INTERPRETER" /D "QT_THREAD_SUPPORT" /FR /YX /FD /c
# ADD BASE MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\disco\Debug\disco.lib ..\SFLsockets\Debug\SFLsockets.lib ..\LavaBase\Debug\LavaBase.lib ..\wxqDocView\Debug\wxqDocView.lib $(QTDIR)\lib\qt-mt$(QTR).lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\LavaPE\Debug\Interpreter.dll"

!ENDIF 

# Begin Target

# Name "Interpreter - Win32 Release"
# Name "Interpreter - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\LavaExecs\Check.cpp
# End Source File
# Begin Source File

SOURCE=..\LavaExecs\Constructs.cpp
# End Source File
# Begin Source File

SOURCE=..\LavaExecs\ConstructsG.cpp
# End Source File
# Begin Source File

SOURCE=.\ConstructsX.cpp

!IF  "$(CFG)" == "Interpreter - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Interpreter - Win32 Debug"

# ADD CPP /W3
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DbgThread.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaProgram.cpp
# End Source File
# Begin Source File

SOURCE=..\LavaExecs\MakeTable.cpp
# End Source File
# Begin Source File

SOURCE=..\LavaExecs\Tokens.cpp
# End Source File
# Begin Source File

SOURCE=..\LavaExecs\TokensG.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\LavaExecs\Check.h
# End Source File
# Begin Source File

SOURCE=..\LavaExecs\Constructs.h
# End Source File
# Begin Source File

SOURCE=..\LavaExecs\Constructs.ph
# End Source File
# Begin Source File

SOURCE=.\DbgThread.h
# End Source File
# Begin Source File

SOURCE=.\LavaProgram.h
# End Source File
# Begin Source File

SOURCE=..\LavaExecs\Tokens.h
# End Source File
# Begin Source File

SOURCE=..\LavaExecs\Tokens.ph
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
