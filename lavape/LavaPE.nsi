; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "LavaPE"
!define PRODUCT_VERSION "0.9.1"
!define PRODUCT_PUBLISHER "The lavape project "
!define PRODUCT_WEB_SITE "http://lavape.sourceforge.net/index.htm"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\LavaPE.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define HelpLink "https://sourceforge.net/tracker/?group_id=93752&atid=605431"
!define URLUpdateInfo "https://sourceforge.net/project/showfiles.php?group_id=93752"
!define PRODUCT_UNINST_ROOT_KEY "HKCU"

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "lavape-0.9.1-win32-bin.exe"
Var instForAll
Var userName
Var unInstString
Var installedBy
Var installedFor

InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
;InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""

;--------------------------------
;Interface Configuration

  !define MUI_HEADERIMAGE
;  !define MUI_HEADERIMAGE_RIGHT
  !define MUI_HEADERIMAGE_BITMAP_NOSTRETCH
  ;!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\win.bmp" ; optional
  !define MUI_HEADERIMAGE_BITMAP "volcano2.png" ; optional
  !define MUI_ABORTWARNING

; MUI Settings
;!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "COPYING.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
;!define MUI_FINISHPAGE_RUN_FUNCTION function_name
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\LavaPE.exe"
!define MUI_FINISHPAGE_RUN_PARAMETERS "..\samples\HelloWorld.lava"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Portuguese"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
  
  ReadRegStr $installedBy ${PRODUCT_UNINST_ROOT_KEY} ${PRODUCT_UNINST_KEY} "InstalledBy"
  IfErrors continue
  MessageBox MB_ICONSTOP|MB_OK "There exists already an installation of ${PRODUCT_NAME} under your user ID.$\n\
    Please uninstall it first!"
  Abort

continue:
  ClearErrors
  UserInfo::GetName
  IfErrors askAll ; Win 9X !
  Pop $userName
  UserInfo::GetAccountType
  Pop $1
  StrCmp $1 "Admin" askAll 0
  StrCpy $instForAll "no"
  MessageBox MB_ICONEXCLAMATION|MB_OK "Warning: Lava file type associations will not be registered,$\n\
    since you don't have administrator privileges!"
  Goto done
  
askAll:
  MessageBox MB_ICONQUESTION|MB_YESNO "Do you want to install ${PRODUCT_NAME} ${PRODUCT_VERSION} for all users?" IDYES 0 IDNO +3
  StrCpy $instForAll "yes"
  Goto done
  StrCpy $instForAll "no"
done:
FunctionEnd

Section "LavaPE (required)" SEC01
  SectionIn RO
  
  StrCmp $instForAll "yes" 0 +2
  SetShellVarContext all
  
  SetOutPath $INSTDIR
  File "*.txt"

  SetOutPath $INSTDIR\bin
  File "bin\*.exe"
;  File src\LavaPE\Release\*.manifest
;  File src\Lava\Release\*.manifest
;  File src\Precompiler\Release\*.manifest
  File "bin\*.dll"
  File "bin\*.lava"
  SetFileAttributes $OUTDIR\std.lava READONLY|ARCHIVE
  File "bin\*.htm"
  File "bin\*.bat"
  File "C:\Qt4\Qt440msvc\bin\assistant.exe"
  File "C:\Qt4\Qt440msvc\bin\QtAssistantClient4.dll"
  File "C:\Qt4\Qt440msvc\bin\QtCore4.dll"
  File "C:\Qt4\Qt440msvc\bin\QtGui4.dll"
  File "C:\Qt4\Qt440msvc\bin\QtNetwork4.dll"
  File "C:\Qt4\Qt440msvc\bin\QtXml4.dll"
  File "C:\Qt4\Qt440msvc\plugins\imageformats\qgif4.dll"
;  File "C:\Qt4\Qt432msvc\bin\QtSvg4.dll"

;  File "C:\Qt4\Qt432msvc\plugins\imageformats\qgif4.dll"
;  File "C:\Qt4\Qt432msvc\plugins\imageformats\qtiff4.dll"
;  File "C:\Qt4\Qt432msvc\plugins\imageformats\qjpeg4.dll"
;  File "C:\Qt4\Qt432msvc\plugins\imageformats\qmng4.dll"
;  File "C:\Qt4\Qt432msvc\plugins\imageformats\qsvg4.dll"

  SetOutPath $INSTDIR\bin\std
  File /r "bin\std\*.*"

  SetOutPath $INSTDIR\bin\LavaIcons
  File "bin\LavaIcons\*.*"
  
  SetOutPath $INSTDIR\bin\Microsoft.VC90.CRT
  File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\*"
  
  SetOutPath $INSTDIR\doc
  File /r /x .svn /x _* ".\doc\*.*"

  SetOutPath $INSTDIR\samples
  File /r /x .svn ".\samples\*.*"

  CreateDirectory "$INSTDIR\bin\Components"
  CreateShortCut "$INSTDIR\bin\Components\LCompoImpl.lcom.lnk" "$INSTDIR\samples\LCompoImpl.lcom"
  
  SetOutPath $INSTDIR\bin\Components
  File "bin\Components\LavaStream.lava"
  File "bin\Components\LavaStream.dll"
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  SetOutPath $INSTDIR\bin ; working directory for automatic launch of LavaPE
SectionEnd

Section "Start Menu Folder"
  CreateDirectory "$SMPROGRAMS\LavaPE"
  CreateShortCut "$SMPROGRAMS\LavaPE\Lava Programming Environment.lnk" "$INSTDIR\bin\LavaPE.exe"
  CreateShortCut "$SMPROGRAMS\LavaPE\Lava Interpreter.lnk" "$INSTDIR\bin\Lava.exe"
  CreateShortCut "$SMPROGRAMS\LavaPE\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\LavaPE\Uninstall.lnk" "$INSTDIR\uninst.exe"SectionEnd
SectionEnd

Section "Desktop Folder"
  CreateDirectory "$DESKTOP\LavaPE"
  CreateShortCut "$DESKTOP\LavaPE\Lava Programming Environment.lnk" "$INSTDIR\bin\LavaPE.exe"
  CreateShortCut "$DESKTOP\LavaPE\Lava Interpreter.lnk" "$INSTDIR\bin\Lava.exe"
  CreateShortCut "$DESKTOP\LavaPE\Lava Samples.lnk" "$INSTDIR\samples"
  CreateShortCut "$DESKTOP\LavaPE\License.lnk" "$INSTDIR\COPYING.txt"
  CreateShortCut "$DESKTOP\LavaPE\Samples Readme.lnk" "$INSTDIR\SamplesReadme.txt"
  CreateShortCut "$DESKTOP\LavaPE\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$DESKTOP\LavaPE\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\LavaPE.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bin\LavaPE.exe,0"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "HelpLink" "${HelpLink}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLUpdateInfo" "${URLUpdateInfo}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstalledBy" "$userName"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "InstalledForAllUsers" "$instForAll"
  WriteRegStr HKCR "Lava.Component\protocol\StdFileEditing\server" "" "$INSTDIR\bin\LavaPE.exe %1"
  WriteRegStr HKCR "Lava.Component\protocol\StdFileEditing\verb\0" "" "&Edit"
  WriteRegStr HKCR "Lava.Component\shell\open\command" "" "$INSTDIR\bin\LavaPE.exe %1"
  WriteRegStr HKCR "Lava.Object\protocol\StdFileEditing\server" "" "$INSTDIR\bin\Lava.exe %1"
  WriteRegStr HKCR "Lava.Object\protocol\StdFileEditing\verb\0" "" "&Edit"
  WriteRegStr HKCR "Lava.Object\shell\open\command" "" "$INSTDIR\bin\Lava.exe %1"
  WriteRegStr HKCR "Lava.Program\protocol\StdFileEditing\server" "" "$INSTDIR\bin\Lava.exe %1"
  WriteRegStr HKCR "Lava.Program\protocol\StdFileEditing\verb\0" "" "&Edit"
  WriteRegStr HKCR "Lava.Program\shell\Edit" "" "&Edit with LavaPE"
  WriteRegStr HKCR "Lava.Program\shell\Edit\command" "" "$INSTDIR\bin\LavaPE.exe %1"
  WriteRegStr HKCR "Lava.Program\shell\open\command" "" "$INSTDIR\bin\Lava.exe %1"
  WriteRegStr HKCR ".lava" "" "Lava.Program"
  WriteRegStr HKCR ".lcom" "" "Lava.Component"
  WriteRegStr HKCR ".ldoc" "" "Lava.Object"
  
  RMDir /r "$DOCUMENTS\..\.assistant"
    ; to prevent erroneous reuse of outdated .assistant data belonging to
    ; a former installation in a different INSTDIR
  SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "The entire Lava software, including the required components from TrollTech's Qt4 toolkit, Lava sample programs, and HTML online help files"
!insertmacro MUI_FUNCTION_DESCRIPTION_END


;Function un.onUninstSuccess
;  HideWindow
;  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) has been uninstalled successfully."
;FunctionEnd

Function un.onInit
!insertmacro MUI_UNGETLANGUAGE

  ReadRegStr $installedBy ${PRODUCT_UNINST_ROOT_KEY} ${PRODUCT_UNINST_KEY} "InstalledBy"
  IfErrors 0 continue
  MessageBox MB_ICONSTOP|MB_OK "${PRODUCT_NAME} ${PRODUCT_VERSION} has been installed under a different user ID,$\n\
    please switch to that user id for uninstalling ${PRODUCT_NAME} ${PRODUCT_VERSION}!"
  Abort
  
continue:
  ReadRegStr $unInstString ${PRODUCT_UNINST_ROOT_KEY} ${PRODUCT_UNINST_KEY} "UninstallString"
  StrCmp $unInstString "$INSTDIR\uninst.exe" continue2 0
  MessageBox MB_ICONSTOP|MB_OK "${PRODUCT_NAME} ${PRODUCT_VERSION} has been installed under a different user ID,$\n\
    please switch to that user id for uninstalling ${PRODUCT_NAME} ${PRODUCT_VERSION}!"
  Abort
  
continue2:
  ReadRegStr $instForAll ${PRODUCT_UNINST_ROOT_KEY} ${PRODUCT_UNINST_KEY} "InstalledForAllUsers"
  StrCmp $instForAll "yes" 0 ask
  StrCpy $installedFor " for all users"
  ClearErrors
  UserInfo::GetName
  IfErrors ask ; Win 9X !
  UserInfo::GetAccountType
  Pop $1
  StrCmp $1 "Admin" ask 0
  MessageBox MB_ICONSTOP|MB_OK "You are not authorized to uninstall ${PRODUCT_NAME} ${PRODUCT_VERSION},$\ninstalled$installedFor by $installedBy!"
  Abort
  
ask:
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Do you really want to uninstall ${PRODUCT_NAME} ${PRODUCT_VERSION},$\ninstalled$installedFor by $installedBy?" IDYES done
  Abort

done:
FunctionEnd

Section Uninstall
  StrCmp $instForAll "yes" 0 +2
  SetShellVarContext all
  
  RMDir /r "$SMPROGRAMS\LavaPE"
  RMDir /r "$DESKTOP\LavaPE"
  RMDir /r "$INSTDIR"
  RMDir /r "$PROFILE\.assistant"
    ; to prevent erroneous reuse of outdated .assistant data belonging to
    ; a former installation in a different INSTDIR
  
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKCR "Lava.Component"
  DeleteRegKey HKCR "Lava.Object"
  DeleteRegKey HKCR "Lava.Program"
  DeleteRegKey HKCR ".lava"
  DeleteRegKey HKCR ".lcom"
  DeleteRegKey HKCR ".ldoc"
  DeleteRegKey HKCU "Software\Fraunhofer-SIT"
;  SetAutoClose true
SectionEnd
