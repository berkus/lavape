# Microsoft Developer Studio Project File - Name="disco" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=disco - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "disco.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "disco.mak" CFG="disco - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "disco - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "disco - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""$/LavaVSS/disco", JAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "disco - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "..\wxqDocView" /I "..\SFLsockets" /I "$(QTDIR)\include" /D "WIN32" /D "DISCO_EXPORT" /D "_DEBUG" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\SFLsockets\Debug\SFLsockets.lib $(QTDIR)\lib\qt-mt$(QTR).lib kernel32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"..\LavaPE\Debug\disco.dll" /libpath:"$(QTDIR)/lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "disco - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "DISCO_EXPORT" /FR /Yu"disco.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "..\wxqDocView" /I "..\SFLsockets" /I "$(QTDIR)\include" /D "WIN32" /D "NDEBUG" /D "DISCO_EXPORT" /D "WXMAKINGDLL" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\LavaPE\Debug\disco.dll"
# ADD LINK32 ..\SFLsockets\Release\SFLsockets.lib $(QTDIR)\librel\qt-mt$(QTR).lib kernel32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /out:"..\LavaPE\Release\disco.dll"
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "disco - Win32 Debug"
# Name "disco - Win32 Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AnyType.cpp
# End Source File
# Begin Source File

SOURCE=.\ASN1.cpp
# End Source File
# Begin Source File

SOURCE=.\ASN1File.cpp
# End Source File
# Begin Source File

SOURCE=.\ASN1pp.cpp
# End Source File
# Begin Source File

SOURCE=.\CDPpp.cpp
# End Source File
# Begin Source File

SOURCE=.\ChainA.cpp
# End Source File
# Begin Source File

SOURCE=.\CHAINANY.cpp
# End Source File
# Begin Source File

SOURCE=.\ChString.cpp
# End Source File
# Begin Source File

SOURCE=.\Convert.cpp
# End Source File
# Begin Source File

SOURCE=.\DateTime.cpp
# End Source File
# Begin Source File

SOURCE=.\DIO.cpp
# End Source File
# Begin Source File

SOURCE=.\DString.cpp
# End Source File
# Begin Source File

SOURCE=.\Halt.cpp
# End Source File
# Begin Source File

SOURCE=.\InFile.cpp
# End Source File
# Begin Source File

SOURCE=.\MachDep.cpp
# End Source File
# Begin Source File

SOURCE=.\NestA.cpp
# End Source File
# Begin Source File

SOURCE=.\NESTANY.cpp
# End Source File
# Begin Source File

SOURCE=.\OSDep.cpp
# End Source File
# Begin Source File

SOURCE=.\OutFile.cpp
# End Source File
# Begin Source File

SOURCE=.\PMDump.cpp
# End Source File
# Begin Source File

SOURCE=.\SET.cpp
# End Source File
# Begin Source File

SOURCE=.\SETpp.cpp
# End Source File
# Begin Source File

SOURCE=.\STR.cpp
# End Source File
# Begin Source File

SOURCE=.\StrIFile.cpp
# End Source File
# Begin Source File

SOURCE=.\StrOFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SynIO.cpp
# End Source File
# Begin Source File

SOURCE=.\Syntax.cpp
# End Source File
# Begin Source File

SOURCE=.\SyntaxG.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\wb_obj.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AnyType.h
# End Source File
# Begin Source File

SOURCE=.\ASN1.h
# End Source File
# Begin Source File

SOURCE=.\ASN1File.h
# End Source File
# Begin Source File

SOURCE=.\ASN1pp.h
# End Source File
# Begin Source File

SOURCE=.\CDPpp.h
# End Source File
# Begin Source File

SOURCE=.\ChainA.h
# End Source File
# Begin Source File

SOURCE=.\CHAINANY.h
# End Source File
# Begin Source File

SOURCE=.\ChString.h
# End Source File
# Begin Source File

SOURCE=.\Code98.h
# End Source File
# Begin Source File

SOURCE=.\Convert.h
# End Source File
# Begin Source File

SOURCE=.\DateTime.h
# End Source File
# Begin Source File

SOURCE=.\DIO.h
# End Source File
# Begin Source File

SOURCE=.\DString.h
# End Source File
# Begin Source File

SOURCE=.\fdQueues.h
# End Source File
# Begin Source File

SOURCE=.\Halt.h
# End Source File
# Begin Source File

SOURCE=.\InFile.h
# End Source File
# Begin Source File

SOURCE=.\MachDep.h
# End Source File
# Begin Source File

SOURCE=.\MACROS.h
# End Source File
# Begin Source File

SOURCE=.\NestA.h
# End Source File
# Begin Source File

SOURCE=.\NESTANY.h
# End Source File
# Begin Source File

SOURCE=.\OSDep.h
# End Source File
# Begin Source File

SOURCE=.\OutFile.h
# End Source File
# Begin Source File

SOURCE=.\PMDump.h
# End Source File
# Begin Source File

SOURCE=.\SETpp.h
# End Source File
# Begin Source File

SOURCE=.\STR.h
# End Source File
# Begin Source File

SOURCE=.\StrIFile.h
# End Source File
# Begin Source File

SOURCE=.\StrOFile.h
# End Source File
# Begin Source File

SOURCE=.\SynIO.h
# End Source File
# Begin Source File

SOURCE=.\Syntax.h
# End Source File
# Begin Source File

SOURCE=.\Syntax.ph

!IF  "$(CFG)" == "disco - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Applying LPC to $(InputPath)
InputPath=.\Syntax.ph
InputName=Syntax

BuildCmds= \
	..\Precompiler\LPC.exe -X DISCO -I. $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)G.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "disco - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build
InputPath=.\Syntax.ph
InputName=Syntax

BuildCmds= \
	PCdir\pp -I..\disco -I. -X DISCO $(InputPath)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)G.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SYSTEM.h
# End Source File
# Begin Source File

SOURCE=.\TextFile.h
# End Source File
# Begin Source File

SOURCE=.\UNIX.h
# End Source File
# Begin Source File

SOURCE=.\wx_obj.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
