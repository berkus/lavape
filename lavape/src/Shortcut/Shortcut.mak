# Microsoft Developer Studio Generated NMAKE File, Based on Shortcut.dsp
!IF "$(CFG)" == ""
CFG=Shortcut - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Shortcut - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Shortcut - Win32 Release" && "$(CFG)" != "Shortcut - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Shortcut.mak" CFG="Shortcut - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Shortcut - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Shortcut - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Shortcut - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Shortcut.exe"


CLEAN :
	-@erase "$(INTDIR)\Shortcut.obj"
	-@erase "$(INTDIR)\Shortcut.pch"
	-@erase "$(INTDIR)\Shortcut.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Shortcut.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\Shortcut.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\Shortcut.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Shortcut.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\Shortcut.pdb" /machine:I386 /out:"$(OUTDIR)\Shortcut.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Shortcut.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Shortcut.res"

"$(OUTDIR)\Shortcut.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Shortcut - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Shortcut.exe" "$(OUTDIR)\Shortcut.pch" "$(OUTDIR)\Shortcut.bsc"


CLEAN :
	-@erase "$(INTDIR)\Shortcut.obj"
	-@erase "$(INTDIR)\Shortcut.pch"
	-@erase "$(INTDIR)\Shortcut.res"
	-@erase "$(INTDIR)\Shortcut.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Shortcut.bsc"
	-@erase "$(OUTDIR)\Shortcut.exe"
	-@erase "$(OUTDIR)\Shortcut.ilk"
	-@erase "$(OUTDIR)\Shortcut.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
RSC_PROJ=/l 0x407 /fo"$(INTDIR)\Shortcut.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Shortcut.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Shortcut.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Shortcut.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\Shortcut.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Shortcut.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Shortcut.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Shortcut.res"

"$(OUTDIR)\Shortcut.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Shortcut.dep")
!INCLUDE "Shortcut.dep"
!ELSE 
!MESSAGE Warning: cannot find "Shortcut.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Shortcut - Win32 Release" || "$(CFG)" == "Shortcut - Win32 Debug"
SOURCE=.\Shortcut.cpp

!IF  "$(CFG)" == "Shortcut - Win32 Release"


"$(INTDIR)\Shortcut.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Shortcut.pch"


!ELSEIF  "$(CFG)" == "Shortcut - Win32 Debug"


"$(INTDIR)\Shortcut.obj"	"$(INTDIR)\Shortcut.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Shortcut.rc

"$(INTDIR)\Shortcut.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Shortcut - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\Shortcut.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Shortcut.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Shortcut - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Shortcut.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Shortcut.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

