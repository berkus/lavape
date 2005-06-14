# Microsoft Developer Studio Project File - Name="LavaBase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LavaBase - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LavaBase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LavaBase.mak" CFG="LavaBase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LavaBase - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LavaBase - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/LavaVSS/LavaBase", XFAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LavaBase - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /I "." /I "Generated" /I "..\disco" /I "..\SFLsockets" /I "..\wxqDocView" /I "..\LavaPE_UI" /I "$(QTDIR)\include" /D "WIN32" /D "LAVABASE_EXPORT" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\disco\Release\disco.lib ..\wxqDocView\Release\wxqDocView.lib $(QTDIR)\librel\qassistantclient.lib $(QTDIR)\librel\qt-mt$(QTR).lib kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comdlg32.lib wsock32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"..\LavaPE\Release\LavaBase.dll"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "." /I "Generated" /I "..\disco" /I "..\SFLsockets" /I "..\wxqDocView" /I "..\LavaPE_UI" /I "$(QTDIR)\include" /D "WIN32" /D "LAVABASE_EXPORT" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FR /FD /c
# SUBTRACT CPP /WX /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\disco\Debug\disco.lib ..\wxqDocView\Debug\wxqDocView.lib $(QTDIR)\lib\qassistantclient.lib $(QTDIR)\lib\qt-mt$(QTR).lib kernel32.lib user32.lib gdi32.lib winspool.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comdlg32.lib wsock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"../LavaPE/Debug/LavaBase.dll" /libpath:"../wxqDocView/Debug" /libpath:"$(QTDIR)/lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "LavaBase - Win32 Release"
# Name "LavaBase - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AboutBox.cpp

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# ADD CPP /I ".\..\disco"
# SUBTRACT CPP /I "..\disco"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BAdapter.cpp

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# ADD CPP /W3

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DumpView.cpp
# End Source File
# Begin Source File

SOURCE=.\InterprBase.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaAppBase.cpp

!IF  "$(CFG)" == "LavaBase - Win32 Release"

# SUBTRACT CPP /Gf /Gy

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# ADD CPP /I ".\..\disco"
# SUBTRACT CPP /I "..\disco"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaBaseDoc.cpp

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# ADD CPP /I ".\..\disco"
# SUBTRACT CPP /I "..\disco"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaBaseStringInit.cpp

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# ADD CPP /I ".\..\disco"
# SUBTRACT CPP /I "..\disco"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaThread.cpp
# End Source File
# Begin Source File

SOURCE=.\PEBaseDoc.cpp

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# ADD CPP /I ".\..\disco"
# SUBTRACT CPP /I "..\disco"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SylTraversal.cpp

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# ADD CPP /I ".\..\disco"
# SUBTRACT CPP /I "..\disco"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SynIDTable.cpp

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# ADD CPP /I ".\..\disco"
# SUBTRACT CPP /I "..\disco"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AboutBox.h

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\AboutBox.h
InputName=AboutBox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BAdapter.h
# End Source File
# Begin Source File

SOURCE=.\DumpView.h

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\DumpView.h
InputName=DumpView

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaAppBase.h

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaAppBase.h
InputName=LavaAppBase

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaBaseDoc.h

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\LavaBaseDoc.h
InputName=LavaBaseDoc

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaBaseStringInit.h
# End Source File
# Begin Source File

SOURCE=.\LavaThread.h
# End Source File
# Begin Source File

SOURCE=.\MACROS.h
# End Source File
# Begin Source File

SOURCE=.\PEBaseDoc.h

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\PEBaseDoc.h
InputName=PEBaseDoc

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SafeInt.h
# End Source File
# Begin Source File

SOURCE=.\SylTraversal.h
# End Source File
# Begin Source File

SOURCE=.\SynIDTable.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\LavaBase.rc2
# End Source File
# Begin Source File

SOURCE=.\res\LavaDoc.ico
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Generated\idd_aboutbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_aboutbox.h

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\Generated\idd_aboutbox.h
InputName=idd_aboutbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_aboutbox.ui.h
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_AboutBox.cpp

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# ADD CPP /I ".\..\disco"
# SUBTRACT CPP /I "..\disco"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_DumpView.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_aboutbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaAppBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_LavaBaseDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_PEBaseDoc.cpp
# End Source File
# End Group
# Begin Group "UI Files"

# PROP Default_Filter ".ui"
# Begin Source File

SOURCE=.\idd_aboutbox.ui

!IF  "$(CFG)" == "LavaBase - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaBase - Win32 Debug"

# PROP Intermediate_Dir "Generated"
# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_aboutbox.ui
InputName=idd_aboutbox

BuildCmds= \
	$(QTDIR)\bin\uic.exe $(InputPath) -o Generated\$(InputName).h \
	$(QTDIR)\bin\uic.exe -impl $(InputName).h $(InputPath) -o Generated\$(InputName).cpp \
	

"Generated\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Generated\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
