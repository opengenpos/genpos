# Auto-generated by EclipseNSIS Script Wizard
# Jun 21, 2010 5:12:58 PM

# --------------------------------------------------------------------------
#    Georgia Southern University Research Services Foundation
#    donated by NCR to the research foundation in 2002 and maintained here
#    since. Created open source version using MIT license without proprietary
#    information and created OpenGenPOS GitHub repository, https://github.com/opengenpos/genpos
#       2002  -> NHPOS Rel 1.4  (Windows CE for NCR 7448, Visual Studio Embedded)
#       2003  -> NHPOS Rel 2.0.0  (Windows XP for NCR touch screen, Datacap for EPT)
#       2006  -> NHPOS Rel 2.0.4  (Windows XP, Rel 2.0.4.51)
#       2006  -> NHPOS Rel 2.0.5  (Windows XP, US Customs, Store and Forward, Mobile Terminal, Rel 2.0.5.76)
#       2007  -> NHPOS Rel 2.1.0  (Windows XP, Condiment Edit and Tim Horton without Rel 2.0.5 changes, Rel 2.1.0.141)
#       2012  -> GenPOS Rel 2.2.0 (Windows 7, Amtrak and VCS, merge Rel 2.0.5 into Rel 2.1.0)
#       2014  -> GenPOS Rel 2.2.1 (Windows 7, Datacap Out of Scope, US Customs, Amtrak, VCS)
#       2016  -> GenPOS Rel 2.2.2 (Windows 7, maintenance releases using Visual Studio 2005 in parallel with Rel 2.3.0 using Visual Studio 2013)
#       2015  -> GenPOS Rel 2.3.0 (Windows 7, EMV card support, Out Of Scope control, compiler change to Visual Studio 2013 from Visual Studio 2005)
#       2019  -> GenPOS Rel 2.3.1 (Windows 10, compiler change to Visual Studio 2015 from Visual Studio 2013)
#       2020  -> GenPOS Rel 2.3.2 (Unreleased. Windows 10, 64 bit totals)
#       2021  -> GenPOS Rel 2.4.0 (Conversion to OpenGenPOS)
#
#    moved from Visual Studio 6.0 to Visual Studio 2005 with Rel 2.2.0

Name "OpenGenPOS"

# General Symbol Definitions
# NOTE: Change the build version number here as well as
#       places where ALL_N_VERSIONINFO and ALL_M_VERSIONINFO
#       are defined. See file versioninfo.h which contains these
#       defines for GenPOS terminal application.
#
#       See as well the copyright macro ALL_Q_COPYRIGHT.
#
#       See as well macro for OutFile below containing installer file name.
!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 2.4.0.002
!define COMPANY NCR
!define URL https://github.com/opengenpos

# MUI Symbol Definitions
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_LICENSEPAGE_RADIOBUTTONS
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI2.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "License GenPOS.rtf"
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile GenPOS_rel_02_04_00_002.exe
InstallDir C:\FlashDisk\NCR\Saratoga
CRCCheck force
XPStyle on
ShowInstDetails show
VIProductVersion "${VERSION}"
VIAddVersionKey ProductName "$(^Name)"
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription "GENPOS Terminal Installer"
VIAddVersionKey LegalCopyright "Copyright (C) 2002-2021, Georgia Southern University"
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show
RequestExecutionLevel admin

# Installer sections
Section -Main SEC0000
    SetOverwrite on
    
    SetOutPath $INSTDIR\Database
    File NHPOS\defaultlayout
#    File NHPOS\BUSINESSLOGIC\PARAMINI
#    File NHPOS\BUSINESSLOGIC\PARAMTDR
    
    SetOutPath $INSTDIR\Program
    File NHPOS\bin\release\*.dll
    File NHPOS\bin\release\*.exe
    File NHPOS\bin\release\DeviceConfig.txt
    
    
    SetOutPath $INSTDIR\Web
    File NHPOS\WEB\index.html
    File NHPOS\WEB\standard.css
    File NHPOS\WEB\standard.js
    File NHPOS\WEB\systemstatus.html
    FILE NHPOS\WEB\storeforward.html
    FILE NHPOS\WEB\preauth.html
    
    SetOutPath $FONTS
    File andalemo.ttf
    
    SetOutPath $INSTDIR
    
    # The location of the Visual Studio 2005 C++ Runtime Redistributable depends on the version of
    # Windows being used to build the application. Beginning with Windows 7 there are now two different
    # folder hierarchies for installed programs.
    File "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\redist\1033\vcredist_x86.exe"
    # File "C:\Program Files (x86)\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86\vcredist_x86.exe"
    # File "C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86\vcredist_x86.exe"
    
    # setting the output path is critical, it affects the working directory of the following shortcuts
    SetOutPath $INSTDIR\Program
    # must create the directory before attempting to create shortcuts in it
    CreateDirectory "$SMPROGRAMS\$StartMenuGroup"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\GenPOS Device Configuration Utility.lnk" $INSTDIR\Program\DeviceConfig.exe
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\GenPOS Terminal Application.lnk" $INSTDIR\Program\Framework.exe
    CreateShortcut "$DESKTOP\GenPOS Device Configuration Utility.lnk" $INSTDIR\Program\DeviceConfig.exe
    CreateShortcut "$DESKTOP\GenPOS Terminal Application.lnk" $INSTDIR\Program\Framework.exe
    
#	ExecWait '"$INSTDIR\vcredist_x86.exe"'  #normal run (dialog with cancel button)
	ExecWait '"$INSTDIR\vcredist_x86.exe" /q:a /c:"msiexec /i vcredist.msi /qb!"'	#dialog with no cancel
#	ExecWait '"$INSTDIR\vcredist_x86.exe" /q:a /c:"msiexec /i vcredist.msi /qn"'	#totally silent
	
    
    WriteRegStr HKLM "${REGKEY}\Components" Main 1
SectionEnd

Section -post SEC0001
	Delete /REBOOTOK vcredist_x86.exe
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk" $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o -un.Main UNSEC0000
    Delete /REBOOTOK "$DESKTOP\GenPOS Device Configuration Utility.lnk"
    Delete /REBOOTOK "$DESKTOP\GenPOS Terminal Application.lnk"
#    Delete /REBOOTOK $FONTS\andalemo.ttf
    DeleteRegValue HKLM "${REGKEY}\Components" Main
SectionEnd

Section -un.post UNSEC0001
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\Uninstall $(^Name).lnk"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    # using /r forces the delete even if the directory isn't empty (which it won't be)
    RmDir /r /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    RmDir /r /REBOOTOK $INSTDIR\Program
    RmDir /r /REBOOTOK $INSTDIR\Web
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
    StrCpy $StartMenuGroup "$(^Name)"
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    StrCpy $StartMenuGroup "$(^Name)"
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
FunctionEnd
