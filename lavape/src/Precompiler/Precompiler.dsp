# Microsoft Developer Studio Project File - Name="Precompiler" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Precompiler - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Precompiler.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Precompiler.mak" CFG="Precompiler - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Precompiler - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Precompiler - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Precompiler - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /I "..\disco" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /Z<none> /O<none> /Fr /YX
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ..\disco\Release\disco.lib $(QTDIR)\librel\qt-mt$(QTR).lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"LPC.exe"

!ELSEIF  "$(CFG)" == "Precompiler - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\disco" /D "WIN32" /D "_DEBUG" /D "DISCO_IMPORT" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\disco\Debug\disco.lib $(QTDIR)\lib\qt-mt$(QTR).lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"LPC.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Precompiler - Win32 Release"
# Name "Precompiler - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Args.cpp
# End Source File
# Begin Source File

SOURCE=.\Atoms.cpp
# End Source File
# Begin Source File

SOURCE=.\CdpCoder.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdError.cpp
# End Source File
# Begin Source File

SOURCE=.\CmdParms.cpp
# End Source File
# Begin Source File

SOURCE=.\Coder.cpp
# End Source File
# Begin Source File

SOURCE=.\ContxtCK.cpp
# End Source File
# Begin Source File

SOURCE=.\DParser.cpp
# End Source File
# Begin Source File

SOURCE=.\ElemCode.cpp
# End Source File
# Begin Source File

SOURCE=.\FioCoder.cpp
# End Source File
# Begin Source File

SOURCE=.\IntCode.cpp
# End Source File
# Begin Source File

SOURCE=.\Parser.cpp
# End Source File
# Begin Source File

SOURCE=.\PC.cpp
# End Source File
# Begin Source File

SOURCE=.\PCerrors.cpp
# End Source File
# Begin Source File

SOURCE=.\ProCoder.cpp
# End Source File
# Begin Source File

SOURCE=.\Scanner.cpp
# End Source File
# Begin Source File

SOURCE=.\SigCoder.cpp
# End Source File
# Begin Source File

SOURCE=.\TypCoder.cpp
# End Source File
# Begin Source File

SOURCE=.\XParser.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Args.h
# End Source File
# Begin Source File

SOURCE=.\Atoms.h
# End Source File
# Begin Source File

SOURCE=.\CdpCoder.h
# End Source File
# Begin Source File

SOURCE=.\CmdError.h
# End Source File
# Begin Source File

SOURCE=.\CmdParms.h
# End Source File
# Begin Source File

SOURCE=.\Coder.h
# End Source File
# Begin Source File

SOURCE=.\ContxtCK.h
# End Source File
# Begin Source File

SOURCE=.\DParser.h
# End Source File
# Begin Source File

SOURCE=.\ElemCode.h
# End Source File
# Begin Source File

SOURCE=.\FioCoder.h
# End Source File
# Begin Source File

SOURCE=.\IntCode.h
# End Source File
# Begin Source File

SOURCE=.\Parser.h
# End Source File
# Begin Source File

SOURCE=.\PCerrors.h
# End Source File
# Begin Source File

SOURCE=.\ProCoder.h
# End Source File
# Begin Source File

SOURCE=.\Scanner.h
# End Source File
# Begin Source File

SOURCE=.\SigCoder.h
# End Source File
# Begin Source File

SOURCE=.\TypCoder.h
# End Source File
# Begin Source File

SOURCE=.\XParser.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\LavaPE\Debug\disco.dll

!IF  "$(CFG)" == "Precompiler - Win32 Release"

!ELSEIF  "$(CFG)" == "Precompiler - Win32 Debug"

# Begin Custom Build
InputPath=..\LavaPE\Debug\disco.dll
InputName=disco

"$(InputName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(InputPath) .

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
