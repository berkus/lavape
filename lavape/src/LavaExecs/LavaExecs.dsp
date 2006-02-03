# Microsoft Developer Studio Project File - Name="LavaExecs" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LavaExecs - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LavaExecs.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LavaExecs.mak" CFG="LavaExecs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LavaExecs - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LavaExecs - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Constrai"
# PROP BASE Intermediate_Dir ".\Constrai"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /I "." /I ".\res\TOOLBUTTONS" /I ".\Generated" /I "..\disco" /I "..\wxqDocView" /I "..\LavaPE" /I "..\LavaPE\Generated" /I "..\LavaPE\res" /I "..\LavaPE\res\TreeIcons" /I "..\LavaPE\res\Toolbuttons" /I "..\LavaBase" /I "..\LavaGUI" /I "..\LavaBase\Generated" /I "..\LavaPE_UI" /I "..\LavaPE_UI\Generated" /I "$(QT3DIR)\include" /D "QT_DLL" /D "LAVAEXECS_EXPORT" /D "EXECVIEW" /D "WIN32" /D "QT_THREAD_SUPPORT" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /win32
# ADD MTL /nologo /mktyplib203 /win32
# ADD BASE RSC /l 0x407
# ADD RSC /l 0x409 /i "..\LavaPE" /i "..\LavaBase"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ..\disco\Debug\disco.lib ..\LavaBase\Debug\LavaBase.lib $(QT3DIR)\lib\qt-mt$(QTR).lib advapi32.lib wsock32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\LavaPE\Debug\LavaExecs.dll"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Constrai"
# PROP BASE Intermediate_Dir "Constrai"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
LIB32=link.exe -lib
# ADD BASE CPP /nologo /MDd /W3 /Gm /Gi /Zi /Od /Gf /Gy /I "..\disco" /I "..\LavaPE" /I "..\LavaBase" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_MBCS" /D "EXECS_EXPORT" /FR /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /I "." /I ".\res\TOOLBUTTONS" /I ".\Generated" /I "..\disco" /I "..\wxqDocView" /I "..\LavaPE" /I "..\LavaPE\Generated" /I "..\LavaPE\res" /I "..\LavaPE\res\TreeIcons" /I "..\LavaPE\res\Toolbuttons" /I "..\LavaBase" /I "..\LavaGUI" /I "..\LavaBase\Generated" /I "..\LavaPE_UI" /I "..\LavaPE_UI\Generated" /I "$(QT3DIR)\include" /D "WIN32" /D "QT_DLL" /D "LAVAEXECS_EXPORT" /D "EXECVIEW" /D "QT_THREAD_SUPPORT" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /mktyplib203 /win32
# ADD MTL /nologo /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /i "..\LavaPE" /i "..\LavaBase"
# ADD RSC /l 0x409 /i "..\LavaPE" /i "..\LavaBase" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 ..\disco\Debug\disco.lib ..\LavaBase\Debug\LavaBase.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\LavaPE\Debug\LavaExecs.dll"
# ADD LINK32 ..\disco\Release\disco.lib ..\LavaBase\Release\LavaBase.lib ..\disco\Release\disco.lib ..\LavaBase\Release\LavaBase.lib $(QT3DIR)\librel\qt-mt$(QTR).lib kernel32.lib advapi32.lib user32.lib gdi32.lib wsock32.lib winspool.lib comdlg32.lib /nologo /subsystem:windows /dll /incremental:no /machine:I386 /out:"..\LavaPE\Release\LavaExecs.dll"
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "LavaExecs - Win32 Debug"
# Name "LavaExecs - Win32 Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Check.cpp
# End Source File
# Begin Source File

SOURCE=.\ComboBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Constructs.cpp
# End Source File
# Begin Source File

SOURCE=.\ConstructsG.cpp

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# ADD CPP /Ze
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ConstructsV.cpp
# End Source File
# Begin Source File

SOURCE=.\EditConst.cpp
# End Source File
# Begin Source File

SOURCE=.\ExecFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\ExecUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\ExecView.cpp
# End Source File
# Begin Source File

SOURCE=.\LavaExecsStringInit.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_ComboBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_ExecView.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgText.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Scanner.cpp
# End Source File
# Begin Source File

SOURCE=.\Tokens.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\TokensG.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Check.h
# End Source File
# Begin Source File

SOURCE=.\ComboBar.h

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\ComboBar.h
InputName=ComboBar

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QT3DIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Comment.ui.h
# End Source File
# Begin Source File

SOURCE=.\Constructs.h
# End Source File
# Begin Source File

SOURCE=.\Constructs.ph

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Applying LPC to $(InputPath)
InputPath=.\Constructs.ph
InputName=Constructs

BuildCmds= \
	..\Precompiler\LPC -I..\disco -I. $(InputName).ph

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)G.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Applying LPC to $(InputPath)
InputPath=.\Constructs.ph
InputName=Constructs

BuildCmds= \
	..\Precompiler\LPC -I..\disco -I. $(InputName).ph

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)G.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ExecFrame.h
# End Source File
# Begin Source File

SOURCE=.\ExecUpdate.h
# End Source File
# Begin Source File

SOURCE=.\ExecView.h

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# PROP Intermediate_Dir ".\Generated"
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\ExecView.h
InputName=ExecView

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QT3DIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LavaExecsStringInit.h
# End Source File
# Begin Source File

SOURCE=.\Scanner.h
# End Source File
# Begin Source File

SOURCE=.\Tokens.h
# End Source File
# Begin Source File

SOURCE=.\Tokens.ph

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Applying LPC to $(InputPath)
InputPath=.\Tokens.ph
InputName=Tokens

BuildCmds= \
	..\Precompiler\LPC -I..\disco -I. $(InputName).ph

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)G.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

# PROP BASE Ignore_Default_Tool 1
# PROP Ignore_Default_Tool 1
# Begin Custom Build - Applying LPC to $(InputPath)
InputPath=.\Tokens.ph
InputName=Tokens

BuildCmds= \
	..\Precompiler\LPC -I..\disco -I. $(InputName).ph

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName)G.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Generated\ComboBarDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\ComboBarDlg.h

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\Generated\ComboBarDlg.h
InputName=ComboBarDlg

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QT3DIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\Comment.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\Comment.h

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# Begin Custom Build - Moc'ing $(InputPath)
InputPath=.\Generated\Comment.h
InputName=Comment

"Generated\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QT3DIR)\bin\moc.exe $(InputPath) -o Generated\moc_$(InputName).cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Generated\moc_ComboBarDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Generated\moc_Comment.cpp
# End Source File
# End Group
# Begin Group "UI Files"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=ComboBarDlg.ui

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=ComboBarDlg.ui
InputName=ComboBarDlg

BuildCmds= \
	$(QT3DIR)\bin\uic.exe $(InputPath) -o Generated\$(InputName).h \
	$(QT3DIR)\bin\uic.exe -impl Generated\$(InputName).h $(InputPath) -o Generated\$(InputName).cpp \
	

"Generated\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Generated\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=Comment.ui

!IF  "$(CFG)" == "LavaExecs - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Applying UIC to $(InputPath)
InputPath=Comment.ui
InputName=Comment

BuildCmds= \
	$(QT3DIR)\bin\uic.exe $(InputPath) -o Generated\$(InputName).h \
	$(QT3DIR)\bin\uic.exe -impl Generated\$(InputName).h $(InputPath) -o Generated\$(InputName).cpp \
	

"Generated\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"Generated\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "LavaExecs - Win32 Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Group "ToolButtons"

# PROP Default_Filter "bmp xpm"
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\bitmap2.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\bmNewFunc.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\bmNewPFunc.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_conflict.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_ignore.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_inputArrow.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_insert.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_insertBefore.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_newLine.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_optLocalVar.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_showComments.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_toggle.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_toggleArrows.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q5_toggleSubstType.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_bitAnd.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_bitOr.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_bitXor.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_divide.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_equal.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_evaluate.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_false.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_greater.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_gtEqual.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_handle.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_interval.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_invert.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_lessEqual.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_lessThan.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_lshift.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_modulus.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_mult.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_notEqual.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_null.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_ord.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_plus.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_plusMinus.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_rshift.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_staticCall.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_true.xpm
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBUTTONS\Q6_vFuncCall.xpm
# End Source File
# End Group
# Begin Source File

SOURCE=.\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\bmNewFunc.bmp
# End Source File
# Begin Source File

SOURCE=.\bmNewPFunc.bmp
# End Source File
# Begin Source File

SOURCE=.\constr_tools.bmp
# End Source File
# Begin Source File

SOURCE=.\op_toolb.bmp
# End Source File
# Begin Source File

SOURCE=.\op_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Test.bmp
# End Source File
# End Group
# End Target
# End Project
