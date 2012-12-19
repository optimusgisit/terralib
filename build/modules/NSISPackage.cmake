# There is a bug in NSI that does not handle full unix paths properly. Make
# sure there is at least one set of four (4) backlasshes.

set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/../../resources/images\\\\terralib_install_icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/../../resources/images\\\\terralib_uninstall_icon.ico")
set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} ${PKG_NAME}")
set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.dpi.inpe.br")
set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.funcate.org.br")

#LIST ( APPEND CPACK_NSIS_CREATE_ICONS_EXTRA ${CPACK_NSIS_CREATE_ICONS_EXTRA}
#"\nSetOutPath \\\"$INSTDIR\\\\bin\\\""
#"\nCreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\TerraView.lnk\\\" \\\"$INSTDIR\\\\bin\\\\TerraView.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\terralib_install_icon.ico\\\""
#"\nCreateShortCut \\\"$DESKTOP\\\\TerraView.lnk\\\" \\\"$INSTDIR\\\\bin\\\\TerraView.exe\\\" \\\"\\\" \\\"$INSTDIR\\\\bin\\\\terralib_install_icon.ico\\\""
#)

# LIST ( APPEND CPACK_NSIS_DELETE_ICONS_EXTRA ${CPACK_NSIS_DELETE_ICONS_EXTRA}
#"\nDelete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\TerraView.lnk\\\""
#"\nDelete \\\"$DESKTOP\\\\TerraView.lnk\\\""
#)