# Please, set manual your target OS
# ANY_OS - for desktop Windows and Linux
# MEEGO_OS - for MEEGO
# ANDROID_OS - for Android
TARGET_OS   =   ANY_OS

lessThan(QT_VERSION, 5.6) {
    error("mytetra requires at least Qt 5.6!")
}

lessThan(QT.webengine.VERSION, 5.6) {
    error("mytetra requires at least QtWebEngine 5.6!")
}

# Flags for profile application
# QMAKE_CXXFLAGS_DEBUG  +=  -pg
# QMAKE_LFLAGS_DEBUG    +=  -pg

# Create define variable in C++ code
DEFINES +=  "ANY_OS=1"
DEFINES +=  "MEEGO_OS=2"
DEFINES +=  "ANDROID_OS=3"
DEFINES +=  "TARGET_OS=$${TARGET_OS}"
#DEFINES    +=  "QT_NO_VERSION_TAGGING"

#DEFINES +=  QWEBENGINESETTINGS_PATHS

message(Building running in Qt major version: $${QT_MAJOR_VERSION})
message(Value of QT_NO_SESSIONMANAGER is: $${QT_NO_SESSIONMANAGER})

TEMPLATE    =   app

QT  =   gui     \
        core    \
        xml     \
        svg     \
        network

QT  +=  widgets
#QT +=  network webkit
QT  +=  webenginewidgets network

qtHaveModule(uitools):!embedded: QT +=  uitools
else: DEFINES   +=  QT_NO_UITOOLS

#DEFINES         +=  QWEBENGINEPAGE_SETNETWORKACCESSMANAGER

CONFIG  +=  qt      \
        warn_on     \
        console     \
        exception   \
        console     \
#        debug       \

CONFIG  +=  c++11
CONFIG  +=  staticlib

greaterThan(QT_MAJOR_VERSION, 4): QT    +=  widgets \
                                            printsupport

contains(TARGET_OS, ANY_OS) {
DESTDIR     =   bin
OBJECTS_DIR =   build
MOC_DIR     =   build
UI_DIR      =   build
}

VERSION     =   0.0.1
DEFINES     +=  APP_VERSION=\\\"$$VERSION\\\"

TARGET      =   mytetra
RESOURCES   =   bin/mytetra.qrc     \
    src/views/browser/data/data.qrc \
    src/views/browser/htmls/htmls.qrc

TRANSLATIONS    =   bin/resource/translations/mytetra_ru.ts
CODECFORTR      =   utf8

# QMAKE_LFLAGS  +=  -L/usr/lib/qt4/lib
INCLUDEPATH     +=  $${_PRO_FILE_PWD_}/src

contains(TARGET_OS, ANY_OS) {
 message(Building the any OS version...)
 SYSTEM_PROGRAM_NAME    =   mytetra
 BINARY_INSTALL_PATH    =   /usr/local/bin
}

contains(TARGET_OS, MEEGO_OS){
 message(Building the MeeGo OS version...)
 SYSTEM_PROGRAM_NAME    =   ru.webhamster.mytetra
 BINARY_INSTALL_PATH    =   /opt/$${SYSTEM_PROGRAM_NAME}/bin
}

contains(TARGET_OS, ANDROID_OS){
 message(Building the Android OS version...)
 SYSTEM_PROGRAM_NAME    =   mytetra
 BINARY_INSTALL_PATH    =   /
}

contains(DEFINES, QWEBENGINEPAGE_SETNETWORKACCESSMANAGER) {
    HEADERS     +=  networkaccessmanager.h
    SOURCES     +=  networkaccessmanager.cpp
}

build_all:!build_pass {
    CONFIG      -=  build_all
    CONFIG      +=  release
}

message(Set installation directory for binary file to $${BINARY_INSTALL_PATH})

# mytetra_binary.path   =   $${BINARY_INSTALL_PATH}
# mytetra_binary.files  =   bin/mytetra
# INSTALLS  +=  mytetra_binary

target.path     =   $${BINARY_INSTALL_PATH}
INSTALLS        +=  target

desktop_file.path   =   /usr/share/applications
contains(TARGET_OS, ANY_OS) {
 desktop_file.files =   desktop/any/mytetra.desktop
}
contains(TARGET_OS, MEEGO_OS) {
 desktop_file.files =   desktop/meego/mytetra.desktop
}
contains(TARGET_OS, ANDROID_OS) {
 desktop_file.files =   desktop/any/mytetra.desktop
}
INSTALLS    +=  desktop_file

icon_scalable_file.path     =   /usr/share/icons/hicolor/scalable/apps
icon_scalable_file.files    =   desktop/mytetra.svg
INSTALLS    +=  icon_scalable_file

icon_48_file.path   =   /usr/share/icons/hicolor/48x48/apps
icon_48_file.files  =   desktop/mytetra.png
INSTALLS    +=  icon_48_file


HEADERS     =   \
    src/main.h  \
    src/views/record/RecordInfoFieldsEditor.h \
    src/views/record/InfoFieldEnter.h \
    src/views/record/MetaEditor.h \
    src/libraries/wyedit/Editor.h \
    src/libraries/wyedit/EditorConfig.h \
    src/libraries/wyedit/EditorConfigDialog.h \
    src/libraries/wyedit/EditorConfigFont.h \
    src/libraries/wyedit/EditorConfigMisc.h \
    src/libraries/wyedit/EditorFindDialog.h \
    src/models/tree/XmlTree.h \
    src/models/tree/TreeModel.h \
    src/models/tree/TreeItem.h \
    src/views/record/AddNewRecord.h \
    src/libraries/ClipboardRecords.h \
    src/views/tree/TreeScreen.h \
    src/libraries/TrashMonitoring.h \
    src/libraries/wyedit/EditorContextMenu.h \
    src/libraries/wyedit/EditorTextArea.h \
    src/libraries/wyedit/indentslider/IndentSlider.h \
    src/libraries/wyedit/EditorSplitCellForm.h \
    src/libraries/wyedit/EditorAbsTableCell.h \
    src/libraries/wyedit/EditorAbsTable.h \
    src/libraries/wyedit/EditorAddTableForm.h \
    src/libraries/wyedit/EditorImageProperties.h \
    src/libraries/wyedit/EditorMultiLineInputDialog.h \
    src/libraries/FixedParameters.h \
    src/libraries/WindowSwitcher.h \
    src/libraries/wyedit/EditorShowText.h \
    src/views/dialog/ReduceMessageBox.h \
    src/views/dialog/OverTextToolButton.h \
    src/libraries/crypt/CryptService.h \
    src/libraries/DiskHelper.h \
    src/views/browser/autosaver.h \
    src/views/browser/bookmarks.h \
    src/views/browser/chasewidget.h \
    src/views/browser/downloadmanager.h \
    src/views/browser/edittreeview.h \
    src/views/browser/featurepermissionbar.h \
    src/views/browser/fullscreennotification.h \
    src/views/browser/history.h \
    src/views/browser/modelmenu.h \
    src/views/browser/networkaccessmanager.h \
    src/views/browser/searchlineedit.h \
    src/views/browser/settings.h \
    src/views/browser/squeezelabel.h \
    src/views/browser/tabwidget.h \
    src/views/browser/toolbarsearch.h \
    src/views/browser/urllineedit.h \
    src/views/browser/webview.h \
    src/views/browser/xbel.h \
    src/views/browser/entrance.h \
    src/views/browser/entranceinfo.h \
    src/libraries/FlatControl.h \
    src/utility/delegate.h \
    src/views/browser/browser.h \
    src/views/app_config/AppConfigDialog.h \
    src/views/app_config/AppConfigPageCrypt.h \
    src/views/app_config/AppConfigPageMain.h \
    src/views/app_config/AppConfigPageMisc.h \
    src/views/app_config/AppConfigPageSynchro.h \
    src/views/app_config/AppConfigPageTable.h \
    src/views/app_config/ConfigDialog.h \
    src/views/app_config/ConfigPage.h \
    src/views/attach_table/AttachTableScreen.h \
    src/views/attach_table/AttachTableView.h \
    src/controllers/attach_table/AttachTableController.h \
    src/models/app_config/AppConfig.h \
    src/models/app_config/AppConfigUpdater.h \
    src/models/attach_table/Attach.h \
    src/models/attach_table/AttachTableData.h \
    src/models/attach_table/AttachTableModel.h \
    src/models/database_config/DataBaseConfig.h \
    src/models/record_table/Record.h \
    src/views/console_emulator/ConsoleEmulator.h \
    src/views/console_emulator/ExecuteCommand.h \
    src/views/enter_password/EnterPassword.h \
    src/views/find_in_base_screen/FindScreen.h \
    src/views/main_window/hidabletabwidget.h \
    src/views/main_window/MainWindow.h \
    src/views/print_preview/PreviewView.h \
    src/views/print_preview/PrintPreview.h \
    src/views/record_table/verticalscrollarea.h \
    src/views/wait_clock/WaitClock.h \
    src/views/browser/edittableview.hxx \
    src/models/record_table/RecordModel.h \
    src/models/record_table/RecordProxyModel.h \
    src/views/record_table/RecordScreen.h \
    src/views/record_table/RecordView.h \
    src/views/record_table/RecordPrint.h \
    src/controllers/record_table/RecordController.h \
    src/models/record_table/ItemsFlat.h \
    src/models/tree/KnowModel.h \
    src/views/tree/KnowView.h \
    src/models/record_table/linker.hxx \
    src/models/tree/binder.hxx \
    src/models/tree/treeindex.hxx \
    src/models/record_table/recordindex.hxx
#    src/views/browser/cookiejar.h \



lessThan(QT_MAJOR_VERSION,5) {
HEADERS     +=  \
    src/libraries/qt_single_application/qtsingleapplication.h \
    src/libraries/qt_single_application/qtlockedfile.h \
    src/libraries/qt_single_application/qtlocalpeer.h \
    src/libraries/qt_single_application/qtsinglecoreapplication.h
}
else {
HEADERS     +=  \
    src/libraries/qt_single_application5/qtsingleapplication.h \
    src/libraries/qt_single_application5/qtlockedfile.h \
    src/libraries/qt_single_application5/qtlocalpeer.h \
    src/libraries/qt_single_application5/qtsinglecoreapplication.h
}

HEADERS     +=  \
    src/libraries/ClipboardBranch.h     \
    src/libraries/GlobalParameters.h    \
#    src/views/console_emulator/ExecuteCommand.h  \
#    src/views/console_emulator/ConsoleEmulator.h \
    src/libraries/WalkHistory.h     \
    src/libraries/crypt/Pbkdf2Qt.h  \
    src/libraries/crypt/RC5Simple.h \
    src/libraries/crypt/Password.h  \
    src/libraries/MtTableWidget.h   \
    src/libraries/MtStyledItemDelegate.h

SOURCES     =   src/main.cpp \
    src/views/record/RecordInfoFieldsEditor.cpp \
    src/views/record/InfoFieldEnter.cpp \
    src/views/record/MetaEditor.cpp \
    src/libraries/wyedit/Editor.cpp \
    src/libraries/wyedit/EditorConfig.cpp \
    src/libraries/wyedit/EditorConfigDialog.cpp \
    src/libraries/wyedit/EditorConfigFont.cpp \
    src/libraries/wyedit/EditorConfigMisc.cpp \
    src/libraries/wyedit/EditorFindDialog.cpp \
    src/models/tree/XmlTree.cpp \
    src/models/tree/TreeModel.cpp \
    src/models/tree/TreeItem.cpp \
    src/views/record/AddNewRecord.cpp \
    src/libraries/ClipboardRecords.cpp \
    src/views/tree/TreeScreen.cpp \
    src/libraries/TrashMonitoring.cpp \
    src/libraries/wyedit/EditorContextMenu.cpp \
    src/libraries/wyedit/EditorTextArea.cpp \
    src/libraries/wyedit/indentslider/IndentSlider.cpp \
    src/libraries/wyedit/EditorSplitCellForm.cpp \
    src/libraries/wyedit/EditorAbsTableCell.cpp \
    src/libraries/wyedit/EditorAbsTable.cpp \
    src/libraries/wyedit/EditorAddTableForm.cpp \
    src/libraries/wyedit/EditorImageProperties.cpp \
    src/libraries/wyedit/EditorMultiLineInputDialog.cpp \
    src/libraries/FixedParameters.cpp \
    src/libraries/WindowSwitcher.cpp \
    src/libraries/wyedit/EditorShowText.cpp \
    src/views/dialog/ReduceMessageBox.cpp \
    src/views/dialog/OverTextToolButton.cpp \
    src/libraries/crypt/CryptService.cpp \
    src/libraries/DiskHelper.cpp \
    src/views/browser/autosaver.cpp \
    src/views/browser/bookmarks.cpp \
    src/views/browser/chasewidget.cpp \
    src/views/browser/downloadmanager.cpp \
    src/views/browser/edittableview.cpp \
    src/views/browser/edittreeview.cpp \
    src/views/browser/featurepermissionbar.cpp \
    src/views/browser/fullscreennotification.cpp \
    src/views/browser/history.cpp \
    src/views/browser/modelmenu.cpp \
    src/views/browser/networkaccessmanager.cpp \
    src/views/browser/searchlineedit.cpp \
    src/views/browser/settings.cpp \
    src/views/browser/squeezelabel.cpp \
    src/views/browser/tabwidget.cpp \
    src/views/browser/toolbarsearch.cpp \
    src/views/browser/urllineedit.cpp \
    src/views/browser/webview.cpp \
    src/views/browser/xbel.cpp \
    src/views/browser/entrance.cpp \
    src/libraries/FlatControl.cpp \
    src/views/browser/browser.cpp \
    src/views/app_config/AppConfigDialog.cpp \
    src/views/app_config/AppConfigPageCrypt.cpp \
    src/views/app_config/AppConfigPageMain.cpp \
    src/views/app_config/AppConfigPageMisc.cpp \
    src/views/app_config/AppConfigPageRecordTable.cpp \
    src/views/app_config/AppConfigPageSynchro.cpp \
    src/views/app_config/ConfigDialog.cpp \
    src/views/app_config/ConfigPage.cpp \
    src/views/attach_table/AttachTableScreen.cpp \
    src/views/attach_table/AttachTableView.cpp \
    src/controllers/attach_table/AttachTableController.cpp \
    src/models/app_config/AppConfig.cpp \
    src/models/app_config/AppConfigUpdater.cpp \
    src/models/attach_table/Attach.cpp \
    src/models/attach_table/AttachTableData.cpp \
    src/models/attach_table/AttachTableModel.cpp \
    src/models/database_config/DataBaseConfig.cpp \
    src/models/record_table/Record.cpp \
    src/views/console_emulator/ConsoleEmulator.cpp \
    src/views/console_emulator/ExecuteCommand.cpp \
    src/views/enter_password/EnterPassword.cpp \
    src/views/find_in_base_screen/FindScreen.cpp \
    src/views/main_window/hidabletabwidget.cpp \
    src/views/main_window/MainWindow.cpp \
    src/views/print_preview/PreviewView.cpp \
    src/views/print_preview/PrintPreview.cpp \
    src/views/record_table/verticalscrollarea.cpp \
    src/views/wait_clock/WaitClock.cpp \
    src/models/record_table/RecordModel.cpp \
    src/models/record_table/RecordProxyModel.cpp \
    src/views/record_table/RecordPrint.cpp \
    src/views/record_table/RecordScreen.cpp \
    src/views/record_table/RecordView.cpp \
    src/controllers/record_table/RecordController.cpp \
    src/models/record_table/ItemsFlat.cpp \
    src/models/tree/KnowModel.cpp \
    src/views/tree/KnowView.cpp \
    src/models/tree/binder.cxx \
    src/models/record_table/linker.cxx \
    src/models/tree/treeindex.cxx \
    src/models/record_table/recordindex.cxx
#    src/views/browser/cookiejar.cpp \


lessThan(QT_MAJOR_VERSION,5) {
SOURCES     +=  \
    src/libraries/qt_single_application/qtsingleapplication.cpp \
    src/libraries/qt_single_application/qtlockedfile.cpp \
    src/libraries/qt_single_application/qtlockedfile_unix.cpp \
    src/libraries/qt_single_application/qtlockedfile_win.cpp \
    src/libraries/qt_single_application/qtlocalpeer.cpp \
    src/libraries/qt_single_application/qtsinglecoreapplication.cpp
}
else {
SOURCES     +=  \
    src/libraries/qt_single_application5/qtsingleapplication.cpp \
    src/libraries/qt_single_application5/qtlockedfile.cpp \
    src/libraries/qt_single_application5/qtlockedfile_unix.cpp \
    src/libraries/qt_single_application5/qtlockedfile_win.cpp \
    src/libraries/qt_single_application5/qtlocalpeer.cpp \
    src/libraries/qt_single_application5/qtsinglecoreapplication.cpp

}

SOURCES     +=  \
    src/libraries/ClipboardBranch.cpp \
    src/libraries/GlobalParameters.cpp \
    src/libraries/WalkHistory.cpp \
    src/libraries/crypt/Pbkdf2Qt.cpp \
    src/libraries/crypt/RC5Simple.cpp \
    src/libraries/crypt/Password.cpp \
    src/libraries/MtTableWidget.cpp \
    src/libraries/MtStyledItemDelegate.cpp


wince* { 
    CONFIG(debug, release|debug):addPlugins.sources     =   $$QT_BUILD_TREE/plugins/imageformats/qsvgd4.dll
    CONFIG(release, release|debug):addPlugins.sources   =   $$QT_BUILD_TREE/plugins/imageformats/qsvg4.dll
    addPlugins.path     =   imageformats
    DEPLOYMENT          +=  addPlugins
}

win32 {
   RC_FILE  =   browser.rc
}

mac {
    ICON    =   browser.icns
    QMAKE_INFO_PLIST    =   Info_mac.plist
    TARGET  =   mytetra
}

ANDROID_PACKAGE_SOURCE_DIR  =   $$PWD/android

OTHER_FILES +=  \
    android/AndroidManifest.xml

DISTFILES   +=          \
    src/uncrustify.cfg  \
    src/views/browser/browser.ico \
    src/views/browser/browser.icns \
    src/views/browser/browser.rc \
    src/views/browser/Info_mac.plist \
    README.md \
    doc/up_linker.png \
    doc/binder.png

FORMS       +=              \
    src/views/browser/addbookmarkdialog.ui \
    src/views/browser/bookmarks.ui \
    src/views/browser/cookies.ui \
    src/views/browser/cookiesexceptions.ui \
    src/views/browser/downloaditem.ui \
    src/views/browser/downloads.ui \
    src/views/browser/history.ui \
    src/views/browser/passworddialog.ui \
    src/views/browser/proxy.ui \
    src/views/browser/settings.ui


unix{
INCLUDEPATH     +=  src/views/browser
}


EXAMPLE_FILES   =   Info_mac.plist browser.icns browser.ico browser.rc



