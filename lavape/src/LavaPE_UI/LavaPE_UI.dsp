# Microsoft Developer Studio Project File - Name="LavaPE_UI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LavaPE_UI - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LavaPE_UI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LavaPE_UI.mak" CFG="LavaPE_UI - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LavaPE_UI - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LavaPE_UI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LAVAPE_UI_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /I "." /I "..\disco" /I "..\wxqDocView" /I "..\LavaBase" /I "$(QTDIR)\include" /D "QT_DLL" /D "WIN32" /D "LAVAPEUI_EXPORT" /D "QT_THREAD_SUPPORT" /FD /c
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
# ADD LINK32 ..\disco\Release\disco.lib ..\wxqDocView\Release\wxqDocView.lib ..\LavaBase\Release\LavaBase.lib $(QTDIR)\librel\qt-mt$(QTR).lib /nologo /dll /machine:I386 /out:"..\LavaPE\Release\LavaPE_UI.dll"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "LAVAPE_UI_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\LavaBase" /I "." /I "..\disco" /I "..\wxqDocView" /I "$(QTDIR)\include" /D "QT_DLL" /D "WIN32" /D "LAVAPEUI_EXPORT" /D "QT_THREAD_SUPPORT" /FR /FD /GZ /c
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
# ADD LINK32 ..\disco\Debug\disco.lib ..\wxqDocView\Debug\wxqDocView.lib ..\LavaBase\Debug\LavaBase.lib $(QTDIR)\lib\qt-mt$(QTR).lib /nologo /dll /debug /machine:I386 /out:"..\LavaPE\Debug\LavaPE_UI.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "LavaPE_UI - Win32 Release"
# Name "LavaPE_UI - Win32 Debug"
# Begin Group "UI Files"

# PROP Default_Filter ".ui"
# Begin Source File

SOURCE=.\cmainframe.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

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
# Begin Source File

SOURCE=.\idd_attrbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_attrbox.ui
InputName=idd_attrbox

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
# Begin Source File

SOURCE=.\idd_chainformpage.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_chainformpage.ui
InputName=idd_chainformpage

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
# Begin Source File

SOURCE=.\idd_compspecbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_compspecbox.ui
InputName=idd_compspecbox

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
# Begin Source File

SOURCE=.\idd_corroverbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_corroverbox.ui
InputName=idd_corroverbox

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
# Begin Source File

SOURCE=.\idd_enumbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_enumbox.ui
InputName=idd_enumbox

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
# Begin Source File

SOURCE=.\idd_enumitem.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_enumitem.ui
InputName=idd_enumitem

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
# Begin Source File

SOURCE=.\idd_findbynamebox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_findbynamebox.ui
InputName=idd_findbynamebox

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
# Begin Source File

SOURCE=.\idd_findrefsbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_findrefsbox.ui
InputName=idd_findrefsbox

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
# Begin Source File

SOURCE=.\idd_fontcolorpage.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_fontcolorpage.ui
InputName=idd_fontcolorpage

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
# Begin Source File

SOURCE=.\idd_formbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_formbox.ui
InputName=idd_formbox

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
# Begin Source File

SOURCE=.\idd_formview.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_formview.ui
InputName=idd_formview

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
# Begin Source File

SOURCE=.\idd_ftextpage.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_ftextpage.ui
InputName=idd_ftextpage

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
# Begin Source File

SOURCE=.\idd_funcbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_funcbox.ui
InputName=idd_funcbox

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
# Begin Source File

SOURCE=.\idd_generalpage.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_generalpage.ui
InputName=idd_generalpage

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
# Begin Source File

SOURCE=.\idd_gotodeclsel.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_gotodeclsel.ui
InputName=idd_gotodeclsel

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
# Begin Source File

SOURCE=.\idd_implbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_implbox.ui
InputName=idd_implbox

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
# Begin Source File

SOURCE=.\idd_includebox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_includebox.ui
InputName=idd_includebox

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
# Begin Source File

SOURCE=.\idd_initbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_initbox.ui
InputName=idd_initbox

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
# Begin Source File

SOURCE=.\idd_interfacebox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_interfacebox.ui
InputName=idd_interfacebox

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
# Begin Source File

SOURCE=.\idd_iobox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_iobox.ui
InputName=idd_iobox

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
# Begin Source File

SOURCE=.\idd_iopage.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_iopage.ui
InputName=idd_iopage

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
# Begin Source File

SOURCE=.\idd_literalitem.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_literalitem.ui
InputName=idd_literalitem

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
# Begin Source File

SOURCE=.\idd_literalspage.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_literalspage.ui
InputName=idd_literalspage

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
# Begin Source File

SOURCE=.\idd_menuitem.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_menuitem.ui
InputName=idd_menuitem

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
# Begin Source File

SOURCE=.\idd_menupage.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_menupage.ui
InputName=idd_menupage

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
# Begin Source File

SOURCE=.\idd_packagebox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_packagebox.ui
InputName=idd_packagebox

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
# Begin Source File

SOURCE=.\idd_setbox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_setbox.ui
InputName=idd_setbox

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
# Begin Source File

SOURCE=.\idd_supportpage.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_supportpage.ui
InputName=idd_supportpage

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
# Begin Source File

SOURCE=.\idd_vtypebox.ui

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=.\idd_vtypebox.ui
InputName=idd_vtypebox

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
# Begin Group "UI.H Files"

# PROP Default_Filter ".ui.h"
# Begin Source File

SOURCE=.\cmainframe.ui.h
# End Source File
# Begin Source File

SOURCE=.\idd_fontcolorpage.ui.h
# End Source File
# Begin Source File

SOURCE=.\idd_ftextpage.ui.h
# End Source File
# Begin Source File

SOURCE=.\idd_iopage.ui.h
# End Source File
# Begin Source File

SOURCE=.\idd_literalitem.ui.h
# End Source File
# Begin Source File

SOURCE=.\idd_menuitem.ui.h
# End Source File
# Begin Source File

SOURCE=.\idd_menupage.ui.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\LavaPE_UI_export.h
# End Source File
# Begin Source File

SOURCE=.\redefines.h
# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Generated\cmainframe.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\cmainframe.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\cmainframe.h
InputName=cmainframe

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_attrbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_attrbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_attrbox.h
InputName=idd_attrbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_chainformpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_chainformpage.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_chainformpage.h
InputName=idd_chainformpage

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_compspecbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_compspecbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_compspecbox.h
InputName=idd_compspecbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_corroverbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_corroverbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_corroverbox.h
InputName=idd_corroverbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_enumbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_enumbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_enumbox.h
InputName=idd_enumbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_enumitem.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_enumitem.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_enumitem.h
InputName=idd_enumitem

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_findbynamebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_findbynamebox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_findbynamebox.h
InputName=idd_findbynamebox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_findrefsbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_findrefsbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_findrefsbox.h
InputName=idd_findrefsbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_fontcolorpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_fontcolorpage.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_fontcolorpage.h
InputName=idd_fontcolorpage

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_formbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_formbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_formbox.h
InputName=idd_formbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_formview.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_formview.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_formview.h
InputName=idd_formview

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_ftextpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_ftextpage.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_ftextpage.h
InputName=idd_ftextpage

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_funcbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_funcbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_funcbox.h
InputName=idd_funcbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_generalpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_generalpage.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_generalpage.h
InputName=idd_generalpage

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_gotodeclsel.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_gotodeclsel.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_gotodeclsel.h
InputName=idd_gotodeclsel

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_implbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_implbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_implbox.h
InputName=idd_implbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_includebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_includebox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_includebox.h
InputName=idd_includebox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_initbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_initbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_initbox.h
InputName=idd_initbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_interfacebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_interfacebox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_interfacebox.h
InputName=idd_interfacebox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_iobox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_iobox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_iobox.h
InputName=idd_iobox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_iopage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_iopage.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_iopage.h
InputName=idd_iopage

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_literalitem.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_literalitem.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_literalitem.h
InputName=idd_literalitem

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_literalspage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_literalspage.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_literalspage.h
InputName=idd_literalspage

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_menuitem.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_menuitem.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_menuitem.h
InputName=idd_menuitem

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_menupage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_menupage.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_menupage.h
InputName=idd_menupage

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_packagebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_packagebox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_packagebox.h
InputName=idd_packagebox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_setbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_setbox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_setbox.h
InputName=idd_setbox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_supportpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_supportpage.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_supportpage.h
InputName=idd_supportpage

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\idd_vtypebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\idd_vtypebox.h

!IF  "$(CFG)" == "LavaPE_UI - Win32 Release"

!ELSEIF  "$(CFG)" == "LavaPE_UI - Win32 Debug"

# Begin Custom Build - MOC'ing $(InputPath)
InputPath=.\Generated\idd_vtypebox.h
InputName=idd_vtypebox

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_cmainframe.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_attrbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_chainformpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_compspecbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_corroverbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_enumbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_enumitem.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_findbynamebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_findrefsbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_fontcolorpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_formbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_formview.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_ftextpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_funcbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_generalpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_gotodeclsel.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_implbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_includebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_initbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_interfacebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_iobox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_iopage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_literalitem.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_literalspage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_menuitem.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_menupage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_packagebox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_setbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_supportpage.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_idd_vtypebox.cpp
# End Source File
# End Group
# End Target
# End Project
