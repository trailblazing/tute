project(QTM)

# set (CMAKE_PREFIX_PATH "$Qt5")  
set(Qt5 $ENV{QT5DIR} CACHE STRING "Qt5 install path")
list(APPEND CMAKE_PREFIX_PATH ${Qt5})


find_package(Qt5Widgets REQUIRED)

if (Qt5Widgets_FOUND)
    if (Qt5Widgets_VERSION VERSION_LESS 5.7.0)
        message(FATAL_ERROR "Minimum supported Qt5 version is 5.7.0!")
    endif()
else()
    message(SEND_ERROR "The Qt5Widgets library could not be found!")
endif(Qt5Widgets_FOUND)


if( Qt5 )
  cmake_minimum_required( VERSION 2.8.9 )
endif( Qt5 )

set( QTM_VERSION 1.3.15 )

IF(APPLE)
    set( PROGNAME QTM )
    set( MACOSX_BUNDLE_SHORT_VERSION_STRING ${QTM_VERSION} )
    set( MACOSX_BUNDLE_VERSION ${QTM_VERSION} )
    set( MACOSX_BUNDLE_LONG_VERSION_STRING ${QTM_VERSION} )
    set( MACOSX_BUNDLE_ICON_FILE QTM.icns )
    IF( MAC_UNIVERSAL )
        set( CMAKE_OSX_ARCHITECTURES ppc;i386 ) #Comment out if not universal binary
    ENDIF( MAC_UNIVERSAL )
    set( STI_SUPERCLASS QObject )
    add_definitions( -DSTI_SUPERCLASS=QObject )
#find_library( COCOA_LIBRARY Cocoa )
ELSE(APPLE)
    set( PROGNAME qtm )
    set( STI_SUPERCLASS QSystemTrayIcon )
    add_definitions( -DSTI_SUPERCLASS=QSystemTrayIcon )
ENDIF(APPLE)

if( NOT Qt5 )
  find_package (Qt4 REQUIRED)
endif( NOT Qt5 )

if( DONT_USE_DBUS OR WIN32 ) # Because you don't get D-Bus on Windows
  if( NOT USE_DBUS )         # But let's let the user specify it, in case someone ports it  
    set( USE_DBUS FALSE )
  endif( NOT USE_DBUS )
else( DONT_USE_DBUS )
  if( NOT APPLE )		# This doesn't preclude the user from specifying D-Bus
    set( USE_DBUS TRUE )	# on the command line.
  endif( NOT APPLE )
endif( DONT_USE_DBUS OR WIN32 )

if( DONT_USE_STI )
  set( USE_STI FALSE )
  set( USE_DBUS FALSE )
else( DONT_USE_STI )
  set( USE_STI TRUE )
endif( DONT_USE_STI )

if( USE_DBUS )
  set( USE_STI TRUE )
  set( DBUSADAPTOR_CC DBusAdaptor.cc )
  set( DBUSADAPTOR_H DBusAdaptor.h )
else( USE_DBUS )
  add_definitions( -DDONT_USE_DBUS )
endif( USE_DBUS )

if( USE_STI )
  add_definitions( -DUSE_SYSTRAYICON )
  set( SYSTRAYICON_H SysTrayIcon.h )
  set( SYSTRAYICON_CC SysTrayIcon.cc )
  set( QUICKPOSTTEMPLATEFORM_UI QuickpostTemplateForm.ui )
  set( QUICKPOSTTEMPLATEDIALOG_H QuickpostTemplateDialog.h )
  set( QUICKPOSTTEMPLATEDIALOG_CC QuickpostTemplateDialog.cc )
  set( QUICKPOSTTEMPLATE_H QuickpostTemplate.h )
  set( QUICKPOSTTEMPLATE_CC QuickpostTemplate.cc )
  set( SUPERMENU_CC SuperMenu.cc )
  set( SUPERMENU_H SuperMenu.h )
#set( STATUSWIDGET_CC StatusWidget.cc )
# set( STATUSWIDGET_H StatusWidget.h )
# set( STATUSWIDGETBASE_UI StatusWidgetBase.ui )
#set( MACFUNCTIONS_MM macFunctions.mm )
#set( MACFUNCTIONS_H macFunctions.h )
  if( APPLE )
    add_definitions( -DSUPERMENU )
#add_definitions( "-x objective-c++" )
  endif( APPLE )
endif( USE_STI )

# Packager can specify not to use QPlainTextEdit, even if Qt 4.4.0 is present
# Note, the author has found that it does not scroll in between going below the
# bottom of the widget and pressing Enter.

if( NOT USE_PTE )
  set( DONT_USE_PTE TRUE )
endif( NOT USE_PTE )

if( DONT_USE_PTE )
  add_definitions( -DDONT_USE_PTE )
endif( DONT_USE_PTE )

if( UNIX AND NOT APPLE )

#Generate desktop entry file

  if( NOT DESKTOP_ENTRY )
    set( DESKTOP_ENTRY qtm.desktop )
  endif( NOT DESKTOP_ENTRY )

  if( UNITY_LAUNCHER )
    set( UNITY_ARG --unity )
    add_definitions( -DUNITY_LAUNCHER )
    set( LISTDIALOGBASE_UI ListDialogBase.ui )
  endif( UNITY_LAUNCHER )

  if( DESKTOP_ENTRY_SUSE )
    set( QTM_DESKTOP_SH qtm-desktop-suse.sh )
  else( DESKTOP_ENTRY_SUSE )
    set( QTM_DESKTOP_SH qtm-desktop.sh )
  endif( DESKTOP_ENTRY_SUSE )

  add_custom_command( OUTPUT ${DESKTOP_ENTRY}
    COMMAND touch ${QTM_BINARY_DIR}/${DESKTOP_ENTRY}
    COMMAND sh ${QTM_SOURCE_DIR}/${QTM_DESKTOP_SH} ${CMAKE_INSTALL_PREFIX} ${UNITY_ARG} >${QTM_BINARY_DIR}/${DESKTOP_ENTRY}
    DEPENDS ${QTM_DESKTOP_SH}
    COMMENT "Generating desktop entry file"
    )
  add_custom_target( DESKTOP_ENTRY_FILE ALL
    DEPENDS ${DESKTOP_ENTRY}
    )

#Generate manpage

  if( NOT MANPAGE_DIRECTORY )
    set( MANPAGE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/share/man/man1 )
  endif( NOT MANPAGE_DIRECTORY)

  add_custom_command( OUTPUT qtm.1.gz
    COMMAND touch qtm.1
    COMMAND sh ${QTM_SOURCE_DIR}/qtm-manpage.sh ${QTM_VERSION} >${QTM_BINARY_DIR}/qtm.1
    COMMAND gzip -9 ${QTM_BINARY_DIR}/qtm.1
    DEPENDS qtm-manpage.sh
    COMMENT "Generating manpage"
    )
  add_custom_target( MANPAGE_FILE ALL
    DEPENDS qtm.1.gz
    )

  set_directory_properties( ADDITIONAL_MAKE_CLEAN_FILES 
		qtm.1
		qtm.1.gz
	  qtm_version.h
	  qtmico.o )
  
  set( APP_ICON images/qtm-logo1.png )

# Generate Markdown location file

  if( NOT MARKDOWN_LOCATION )
    if( INSTALL_MARKDOWN )
      set( MARKDOWN_LOCATION ${CMAKE_INSTALL_PREFIX}/share/qtm/Markdown.pl )
    else( INSTALL_MARKDOWN )
      set( MARKDOWN_LOCATION /usr/bin/markdown )
    endif( INSTALL_MARKDOWN )
  endif( NOT MARKDOWN_LOCATION )
  set( MARKDOWN_HEADER ${QTM_BINARY_DIR}/markdown_header.h )
  configure_file( ${QTM_SOURCE_DIR}/markdown_header.h.in ${QTM_BINARY_DIR}/markdown_header.h @ONLY IMMEDIATE )

endif( UNIX AND NOT APPLE )

if( WIN32 )
  if( NOT MARKDOWN_LOCATION )
    set( MARKDOWN_LOCATION %APPDIRPATH%/Markdown )
  endif( NOT MARKDOWN_LOCATION )
  set( MARKDOWN_HEADER ${QTM_BINARY_DIR}/markdown_header.h )
  configure_file( ${QTM_SOURCE_DIR}/markdown_header.h.in ${QTM_BINARY_DIR}/markdown_header.h @ONLY IMMEDIATE )
endif( WIN32 )

if( QDEBUG )
  set( CMAKE_VERBOSE_MAKEFILE ON )
  add_definitions( -g -O1 )
else( NOT QDEBUG )
  add_definitions( -DNO_DEBUG_OUTPUT )
  #set( CMAKE_SKIP_RPATH ON )
endif( QDEBUG )

if( NOT DESKTOP_ENTRY_PATH )
  set( DESKTOP_ENTRY_PATH ${CMAKE_INSTALL_PREFIX}/share/applications )
endif( NOT DESKTOP_ENTRY_PATH )

if( NO_SSL )
  add_definitions( -DDONT_USE_SSL )
endif( NO_SSL )

if( USE_DBUS )
  cmake_minimum_required(VERSION 2.4.5)
else( USE_DBUS )
  cmake_minimum_required(VERSION 2.4.0)
endif( USE_DBUS )

set(QTM_SRCS
    AccountsDialog.cc
    Application.cc
    EditingWindow.cc
    EditingWindow_ResponseHandlers.cc
    Highlighter.cc
    locationlineedit.cc
    main.cc
    PrefsDialog.cc
    QijSearchWidget.cc
    ${QUICKPOSTTEMPLATE_CC}
    ${QUICKPOSTTEMPLATEDIALOG_CC}
    ${SAFEHTTP_CC}
#${MACFUNCTIONS_MM}
    ${SUPERMENU_CC}
# ${STATUSWIDGET_CC}
    ${SYSTRAYICON_CC}
    ${DBUSADAPTOR_CC}
    XmlRpcHandler.cc
)

set(QTM_MOC_HDRS
    AccountsDialog.h
    Application.h
    EditingWindow.h
    Highlighter.h
    locationlineedit.h
    PrefsDialog.h
    QijSearchWidget.h
    ${QUICKPOSTTEMPLATE_H}
    ${QUICKPOSTTEMPLATEDIALOG_H}
    ${SAFEHTTP_H}
    ${SUPERMENU_H}
#${STATUSWIDGET_H}
    ${SYSTRAYICON_H}
    ${DBUSADAPTOR_H}
)

set(QTM_UIS
    aboutbox.ui
    AccountsForm.ui
#EditingWindowBase.ui
    NewCategoryForm.ui
    PrefsForm.ui
    ImageEntry.ui
    LinkEntry.ui
    ${LISTDIALOGBASE_UI}
    password-form.ui
    QijSearchWidget.ui
    ${QUICKPOSTTEMPLATEFORM_UI}
    SideWidget.ui
    ${STATUSWIDGETBASE_UI}
)

set(QTM_RCS
    qtm.qrc
    EditingWindow.qrc
    QijSearchWidget.qrc
)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0005 OLD)
  if( Qt5 )
    cmake_policy( SET CMP0020 NEW )
  endif( Qt5 )
endif(COMMAND cmake_policy)

add_definitions( -Wall )

configure_file( ${QTM_SOURCE_DIR}/qtm_version.h.in ${QTM_BINARY_DIR}/qtm_version.h @ONLY IMMEDIATE )
set( GENERATED_SOURCES ${QTM_BINARY_DIR}/qtm_version.h ${MARKDOWN_HEADER} )

SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
if( USE_DBUS )
  set( QT_USE_QTDBUS TRUE )
endif( USE_DBUS )
if( MSVC )
  #set( QT_USE_QTMAIN TRUE )
  add_definitions( -DQT_DLL )
endif( MSVC )

# find_package (Qt4 REQUIRED)

if( Qt5 )
  #set( CMAKE_AUTOMOC ON )
  set( CMAKE_INCLUDE_CURRENT_DIR ON )
  find_package( Qt5Widgets REQUIRED )
  find_package( Qt5Network REQUIRED )
  find_package( Qt5Xml REQUIRED)
  set( QTM_MODULES Widgets Network Xml )
  if( USE_DBUS )
    find_package( Qt5DBus REQUIRED )
    set( Qt5DBUS_ DBus )
    set( QTM_MODULES ${QTM_MODULES} DBus )
  endif( USE_DBUS )
  if( WIN32 )
    find_package( Qt5WinExtras REQUIRED )
    set( QTM_MODULES ${QTM_MODULES} WinExtras )
  endif( WIN32 )

  qt5_wrap_ui( QTM_UIS_H ${QTM_UIS} )
  qt5_wrap_cpp( QTM_MOC_SRCS ${QTM_MOC_HDRS} OPTIONS -DSTI_SUPERCLASS=${STI_SUPERCLASS} )
  qt5_add_resources( QTM_RC_SRCS ${QTM_RCS} )
  if( MSVC )
    set( QT_MAIN_LIB c:/Qt/Qt5.0.0beta2/5.0.0-beta2/msvc2010/lib/qtmain.lib )
  endif( MSVC )

else( Qt5 )
include (${QT_USE_FILE})

if( USE_DBUS AND NOT QT_QTDBUS_INCLUDE_DIR )
  set( QT_QTDBUS_INCLUDE_DIR ${QT_INCLUDE_DIR}/QtDBus )
  set( QTDBUS_EXTRA QtDBus )
  add_definitions( -I${QT_QTDBUS_INCLUDE_DIR} )
endif( USE_DBUS AND NOT QT_QTDBUS_INCLUDE_DIR )

QT4_WRAP_UI(QTM_UIS_H ${QTM_UIS})

QT4_WRAP_CPP(QTM_MOC_SRCS ${QTM_MOC_HDRS} OPTIONS -DSTI_SUPERCLASS=${STI_SUPERCLASS} )

QT4_ADD_RESOURCES(QTM_RC_SRCS ${QTM_RCS})

endif( Qt5 )

if(MINGW)
    add_custom_command(OUTPUT qtmico.o
                       COMMAND windres.exe -iqtm.rc -oqtmico.o)
    set(QTM_SRCS ${QTM_SRCS} qtmico.o)
endif(MINGW)

if( MSVC )
    set( QTM_RC qtm.rc )
endif( MSVC )  

IF(APPLE)
    add_executable(${PROGNAME} MACOSX_BUNDLE ${QTM_SRCS} ${QTM_UIS_H} ${QTM_MOC_SRCS} ${QTM_RC_SRCS} ${GENERATED_SOURCES})
    if( Qt5 )
      qt5_use_modules( ${PROGNAME} Widgets Network Xml )
    endif( Qt5 )
    add_custom_command( TARGET ${PROGNAME} POST_BUILD
      COMMAND mkdir ARGS -p ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
      COMMAND cp ARGS ${MACOSX_BUNDLE_ICON_FILE} ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
      COMMAND cp ARGS ${QTM_SOURCE_DIR}/Markdown/Markdown.pl ${CMAKE_CURRENT_BINARY_DIR}/${PROGNAME}.app/Contents/Resources
      )
ELSE(NOT APPLE)
    add_executable(${PROGNAME} WIN32 ${QTM_SRCS} ${QTM_UIS_H} ${QTM_MOC_SRCS} ${QTM_RC_SRCS} ${QTM_RC} ${GENERATED_SOURCES})
    if( Qt5 )
      qt5_use_modules( ${PROGNAME} Widgets Network Xml ${Qt5DBUS_} )
    endif( Qt5 )
    if( NOT QDEBUG )
     if( MINGW )  
      add_custom_command( TARGET ${PROGNAME} POST_BUILD
	COMMAND strip ARGS ${QTM_BINARY_DIR}/${PROGNAME}.exe
	)
     endif( MINGW )
    endif( NOT QDEBUG )
ENDIF(APPLE)


INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${QTM_SOURCE_DIR})

if( USE_RPATH )
  set( CMAKE_SKIP_BUILD_RPATH TRUE )
  set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
  set( CMAKE_INSTALL_RPATH "${QT_LIBRARY_DIR}" )
  set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
endif( USE_RPATH )

ADD_CUSTOM_TARGET( confclean COMMAND rm -rf Makefile CMakeFiles/ CMakeCache.txt cmake_install.cmake )

SET_TARGET_PROPERTIES( ${PROGNAME}
	PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE )
if( Qt5 )
  QT5_USE_MODULES( ${PROGNAME} ${QTM_MODULES} )
else( Qt5 )  
  TARGET_LINK_LIBRARIES(${PROGNAME} ${QT_LIBRARIES} ${QT_MAIN_LIB} ${QTDBUS_EXTRA} )
endif( Qt5 )

  if( QDEBUG AND MINGW )
    SET_TARGET_PROPERTIES (${PROGNAME} PROPERTIES LINK_FLAGS -Wl,-subsystem,console )
endif( QDEBUG AND MINGW )

install(TARGETS ${PROGNAME} DESTINATION bin)
if(UNIX AND NOT APPLE)
  install(FILES ${QTM_BINARY_DIR}/${DESKTOP_ENTRY} DESTINATION ${DESKTOP_ENTRY_PATH})
  install( FILES ${APP_ICON} DESTINATION share/icons )
  install( FILES ${QTM_BINARY_DIR}/qtm.1.gz DESTINATION ${MANPAGE_DIRECTORY} )
  if( UNITY_LAUNCHER )
    install( FILES ${QTM_SOURCE_DIR}/qtm-new.py DESTINATION bin RENAME qtm-new )
    install( FILES ${QTM_SOURCE_DIR}/qtm-choose.py DESTINATION bin RENAME qtm-choose )
    install( FILES ${QTM_SOURCE_DIR}/qtm-chooseRecent.py DESTINATION bin RENAME qtm-chooseRecent )
    install( FILES ${QTM_SOURCE_DIR}/qtm-quickpost.py DESTINATION bin RENAME qtm-quickpost )
    install( FILES ${QTM_SOURCE_DIR}/qtm-chooseTemplate.py DESTINATION bin RENAME qtm-chooseTemplate )
  endif( UNITY_LAUNCHER )
  if( INSTALL_MARKDOWN )
    install( FILES ${QTM_SOURCE_DIR}/Markdown/Markdown.pl DESTINATION share/qtm )
  endif( INSTALL_MARKDOWN )
endif(UNIX AND NOT APPLE)
