
!define PRODUCT_VERSION "V1.0.0"
!define PRODUCT_NAME "Chaopy"
!define PRODUCT_MODE "Release"
!define OUTPUT_FILE "chaopy_installer_win_x64_${PRODUCT_VERSION}.exe"
!define INSTALL_DIR "$PROGRAMFILES\Chaopy"

OutFile ${OUTPUT_FILE}

InstallDir ${INSTALL_DIR}

InstallDirRegKey HKCU "Software\Chaopy" "InstallPath"

LicenseData ..\LICENSE

Page license
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "Install"

  CreateDirectory $INSTDIR

  SetOutPath $INSTDIR
  File /r "Default\*.*"
  File /r "..\bin\${PRODUCT_MODE}\*.*"

  WriteRegStr HKCU "Software\Chaopy" "InstallPath" "$INSTDIR"
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section "Uninstall"

  Delete "$INSTDIR\*.*"

  RMDir /r "$INSTDIR"

  DeleteRegKey HKCU "Software\Chaopy"

SectionEnd
