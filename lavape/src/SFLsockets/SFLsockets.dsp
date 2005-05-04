# Microsoft Developer Studio Project File - Name="SFLsockets" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SFLsockets - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SFLsockets.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SFLsockets.mak" CFG="SFLsockets - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SFLsockets - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SFLsockets - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SFLsockets - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SFLSOCKETS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /D "SFL_EXPORT" /D "WIN32" /D "BLOCKING_CONNECT" /D "DEBUG" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"..\LavaPE\Release\SFLsockets.dll"

!ELSEIF  "$(CFG)" == "SFLsockets - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "SFLSOCKETS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /D "SFL_EXPORT" /D "WIN32" /D "BLOCKING_CONNECT" /D "DEBUG" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x407
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\LavaPE\Debug\SFLsockets.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SFLsockets - Win32 Release"
# Name "SFLsockets - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\sflcons.c
# End Source File
# Begin Source File

SOURCE=.\sflconv.c
# End Source File
# Begin Source File

SOURCE=.\sflcvsb.c
# End Source File
# Begin Source File

SOURCE=.\sfldate.c
# End Source File
# Begin Source File

SOURCE=.\sfldir.c
# End Source File
# Begin Source File

SOURCE=.\sflenv.c
# End Source File
# Begin Source File

SOURCE=.\sflfile.c
# End Source File
# Begin Source File

SOURCE=.\sfllist.c
# End Source File
# Begin Source File

SOURCE=.\sflmem.c
# End Source File
# Begin Source File

SOURCE=.\sflnode.c
# End Source File
# Begin Source File

SOURCE=.\sflsock.c
# End Source File
# Begin Source File

SOURCE=.\sflstr.c
# End Source File
# Begin Source File

SOURCE=.\sflsymb.c
# End Source File
# Begin Source File

SOURCE=.\sflsyst.c
# End Source File
# Begin Source File

SOURCE=.\sfltok.c
# End Source File
# Begin Source File

SOURCE=.\sfltron.c
# End Source File
# Begin Source File

SOURCE=.\sfluid.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\prelude.h
# End Source File
# Begin Source File

SOURCE=..\SFLsockets\sflcons.h
# End Source File
# Begin Source File

SOURCE=.\sflconv.h
# End Source File
# Begin Source File

SOURCE=.\sfldate.h
# End Source File
# Begin Source File

SOURCE=.\sfldir.h
# End Source File
# Begin Source File

SOURCE=.\sflenv.h
# End Source File
# Begin Source File

SOURCE=..\SFLsockets\sflfile.h
# End Source File
# Begin Source File

SOURCE=..\SFLsockets\sfllist.h
# End Source File
# Begin Source File

SOURCE=..\SFLsockets\sflmem.h
# End Source File
# Begin Source File

SOURCE=.\sflnode.h
# End Source File
# Begin Source File

SOURCE=..\SFLsockets\sflprint.h
# End Source File
# Begin Source File

SOURCE=..\SFLsockets\sflsock.h
# End Source File
# Begin Source File

SOURCE=.\SFLsockets_all.h
# End Source File
# Begin Source File

SOURCE=.\sflstr.h
# End Source File
# Begin Source File

SOURCE=..\SFLsockets\sflsymb.h
# End Source File
# Begin Source File

SOURCE=.\sflsyst.h
# End Source File
# Begin Source File

SOURCE=..\SFLsockets\sfltok.h
# End Source File
# Begin Source File

SOURCE=.\sfltron.h
# End Source File
# Begin Source File

SOURCE=..\SFLsockets\sfluid.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
