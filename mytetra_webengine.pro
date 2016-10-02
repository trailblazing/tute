# Please, set manual your target OS
# ANY_OS - for desktop Windows and Linux
# MEEGO_OS - for MEEGO
# ANDROID_OS - for Android
TARGET_OS   =   ANY_OS

lessThan(QT_VERSION, 5.7) {
    error("mytetra requires at least Qt 5.7!")
}

lessThan(QT.webengine.VERSION, 5.7) {
    error("mytetra requires at least QtWebEngine 5.7!")
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
DEFINES    += "USE_POPUP_WINDOW"

message(Building running in Qt major version: $${QT_MAJOR_VERSION})
message(Building running in Qt version: $${QT_VERSION})
message(Value of QT_NO_SESSIONMANAGER is: $${QT_NO_SESSIONMANAGER})

TEMPLATE    =   app

QT  =   gui     \
        core    \
        xml     \
        svg     \
        network

greaterThan(QT_MAJOR_VERSION, 4): QT    +=  widgets             \
                                            printsupport        \
                                            webenginewidgets
#QT  +=  widgets
#QT +=  webkit
#QT  +=  webenginewidgets

qtHaveModule(uitools):!embedded: QT +=  uitools
else: DEFINES   +=  QT_NO_UITOOLS

#DEFINES         +=  QWEBENGINEPAGE_SETNETWORKACCESSMANAGER
DEFINES         +=  QWEBENGINEHISTORY_RESTORESESSION

CONFIG  +=  qt      \
        warn_on     \
        console     \
        exception   \
        console     \
#        debug

CONFIG  +=  c++14
CONFIG  +=  gnu++14
#CONFIG  +=  staticlib static

QMAKE_CXXFLAGS += -std=c++14 -std=gnu++14


# http://blog.qt.io/blog/2011/10/28/rpath-and-runpath/


PROJECT_QT_VERSION      = $$(QT5DIR)

#PROJECT_QT_LIBS      = PROJECT_QT_VERSION/lib64
#PROJECT_QT_LIBS      = PROJECT_QT_VERSION/lib
PROJECT_QT_LIBS      = $$PROJECT_QT_VERSION/lib





TARGETDEPS += $$PROJECT_QT_LIBS/libQt5Svg.so            \
        $$PROJECT_QT_LIBS/libQt5WebEngineWidgets.so     \
        $$PROJECT_QT_LIBS/libQt5PrintSupport.so         \
        $$PROJECT_QT_LIBS/libQt5Widgets.so              \
        $$PROJECT_QT_LIBS/libQt5WebEngineCore.so        \
        $$PROJECT_QT_LIBS/libQt5Quick.so                \
        $$PROJECT_QT_LIBS/libQt5Gui.so                  \
        $$PROJECT_QT_LIBS/libQt5Xml.so                  \
        $$PROJECT_QT_LIBS/libQt5Qml.so                  \
        $$PROJECT_QT_LIBS/libQt5Network.so              \
        $$PROJECT_QT_LIBS/libQt5WebChannel.so           \
        $$PROJECT_QT_LIBS/libQt5Core.so                 \
        $$PROJECT_QT_LIBS/libQt5WebEngine.so            \
        $$PROJECT_QT_LIBS/libQt5Positioning.so


INCLUDEPATH     += $$PROJECT_QT_VERSION/include
#DEPENDPATH      += $$PROJECT_QT_VERSION/include

#INCLUDEPATH     += $$PROJECT_QT_VERSION/lib/
DEPENDPATH      += . $$PROJECT_QT_LIBS

LIBS            += -L$$PROJECT_QT_LIBS -lQt5Svg -lQt5WebEngineWidgets -lQt5PrintSupport -lQt5Widgets -lQt5WebEngineCore -lQt5Quick -lQt5Gui -lQt5Xml -lQt5Qml -lQt5Network -lQt5WebChannel -lQt5Core -lQt5WebEngine -lQt5Positioning

contains(TARGET_OS, ANY_OS) {
DESTDIR     =   bin
OBJECTS_DIR =   build
MOC_DIR     =   build
UI_DIR      =   build
}

VERSION     =   0.0.1
DEFINES     +=  APP_VERSION=\\\"$$VERSION\\\"

TARGET      =   mytetra_webengine
RESOURCES   =   bin/mytetra.qrc     \
    src/views/browser/data/data.qrc \
    src/views/browser/htmls/htmls.qrc

TRANSLATIONS    =   bin/resource/translations/mytetra_ru.ts
CODECFORTR      =   utf8

# QMAKE_LFLAGS  +=  -L/usr/lib/qt4/lib
INCLUDEPATH     +=  $${_PRO_FILE_PWD_}/src

INCLUDEPATH     += ../../verdigris/src

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
    src/utility/delegate.h \
    src/views/browser/browser.h \
    src/views/browser/edittableview.hxx \
    src/models/record_table/linker.hxx \
    src/models/tree/binder.hxx \
    src/utility/util.hxx \
    src/models/tree/tree_know_model.h \
    src/models/tree/tree_model.h \
    src/models/tree/tree_item.h \
    src/models/tree/tree_index.hxx \
    src/views/tree/tree_view.h \
    src/views/tree/tree_screen.h \
    src/controllers/attach_table/attach_table_controller.h \
    src/controllers/record_table/record_controller.h \
    src/libraries/clipboard_branch.h \
    src/libraries/clipboard_records.h \
    src/libraries/disk_helper.h \
    src/libraries/fixed_parameters.h \
    src/libraries/flat_control.h \
    src/libraries/global_parameters.h \
    src/libraries/mt_styled_item_delegate.h \
    src/libraries/mt_table_widget.h \
    src/libraries/trash_monitoring.h \
    src/libraries/walk_history.h \
    src/libraries/window_switcher.h \
    src/libraries/crypt/crypt_service.h \
    src/libraries/crypt/password.h \
    src/libraries/crypt/pbkdf2qt.h \
    src/libraries/crypt/rc5simple.h \
    src/libraries/wyedit/indentslider/indent_slider.h \
    src/libraries/wyedit/editor.h \
    src/libraries/wyedit/editor_abs_table.h \
    src/libraries/wyedit/editor_abs_table_cell.h \
    src/libraries/wyedit/editor_add_table_form.h \
    src/libraries/wyedit/editor_config.h \
    src/libraries/wyedit/editor_config_dialog.h \
    src/libraries/wyedit/editor_config_font.h \
    src/libraries/wyedit/editor_config_misc.h \
    src/libraries/wyedit/editor_context_menu.h \
    src/libraries/wyedit/editor_find_dialog.h \
    src/libraries/wyedit/editor_multi_line_input_dialog.h \
    src/libraries/wyedit/editor_show_text.h \
    src/libraries/wyedit/editor_split_cell_form.h \
    src/libraries/wyedit/editor_text_area.h \
    src/models/app_config/app_config.h \
    src/models/app_config/app_config_updater.h \
    src/models/attach_table/attach.h \
    src/models/attach_table/attach_table_data.h \
    src/models/attach_table/attach_table_model.h \
    src/models/database_config/database_config.h \
    src/models/record_table/items_flat.h \
    src/models/record_table/record.h \
    src/models/record_table/record_model.h \
    src/models/record_table/record_index.hxx \
    src/models/record_table/record_model_proxy.h \
    src/models/tree/tree_xml.h \
    src/views/app_config/app_config_dialog.h \
    src/views/app_config/app_config_page_crypt.h \
    src/views/app_config/app_config_page_main.h \
    src/views/app_config/app_config_page_misc.h \
    src/views/app_config/app_config_page_synchro.h \
    src/views/app_config/app_config_page_table.h \
    src/views/app_config/config_dialog.h \
    src/views/app_config/config_page.h \
    src/views/attach_table/attach_table_screen.h \
    src/views/attach_table/attach_table_view.h \
    src/views/console_emulator/console_emulator.h \
    src/views/console_emulator/execute_command.h \
    src/views/dialog/over_text_tool_button.h \
    src/views/dialog/reduce_message_box.h \
    src/views/enter_password/enter_password.h \
    src/views/find_in_base_screen/find_screen.h \
    src/views/main_window/main_window.h \
    src/views/main_window/hidable_tabwidget.h \
    src/views/print_preview/print_preview.h \
    src/views/print_preview/view_preview.h \
    src/views/record/add_new_record.h \
    src/views/record/info_field_enter.h \
    src/views/record/meta_editor.h \
    src/views/record/record_info_fields_editor.h \
    src/views/record_table/record_print.h \
    src/views/record_table/record_screen.h \
    src/views/record_table/record_view.h \
    src/views/record_table/vertical_scrollarea.h \
    src/views/wait_clock/wait_clock.h \
    src/libraries/wyedit/editor_image_properties.h
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

HEADERS     +=
#    src/views/console_emulator/ExecuteCommand.h  \
#    src/views/console_emulator/ConsoleEmulator.h \

SOURCES     =   src/main.cpp \
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
    src/views/browser/browser.cpp \
    src/models/tree/binder.cxx \
    src/models/record_table/linker.cxx \
    src/models/tree/tree_know_model.cpp \
    src/models/tree/tree_model.cpp \
    src/models/tree/tree_item.cpp \
    src/models/tree/tree_index.cxx \
    src/views/tree/tree_view.cpp \
    src/views/tree/tree_screen.cpp \
    src/controllers/attach_table/attach_table_controller.cpp \
    src/controllers/record_table/record_controller.cpp \
    src/libraries/crypt/crypt_service.cpp \
    src/libraries/crypt/password.cpp \
    src/libraries/crypt/pbkdf2qt.cpp \
    src/libraries/crypt/rc5simple.cpp \
    src/libraries/wyedit/indentslider/indent_slider.cpp \
    src/libraries/wyedit/editor.cpp \
    src/libraries/wyedit/editor_abs_table.cpp \
    src/libraries/wyedit/editor_abs_table_cell.cpp \
    src/libraries/wyedit/editor_add_table_form.cpp \
    src/libraries/wyedit/editor_config.cpp \
    src/libraries/wyedit/editor_config_dialog.cpp \
    src/libraries/wyedit/editor_config_font.cpp \
    src/libraries/wyedit/editor_config_misc.cpp \
    src/libraries/wyedit/editor_context_menu.cpp \
    src/libraries/wyedit/editor_find_dialog.cpp \
    src/libraries/wyedit/editor_image_properties.cpp \
    src/libraries/wyedit/editor_multi_line_input_dialog.cpp \
    src/libraries/wyedit/editor_show_text.cpp \
    src/libraries/wyedit/editor_split_cell_form.cpp \
    src/libraries/wyedit/editor_text_area.cpp \
    src/libraries/clipboard_branch.cpp \
    src/libraries/clipboard_records.cpp \
    src/libraries/disk_helper.cpp \
    src/libraries/fixed_parameters.cpp \
    src/libraries/flat_control.cpp \
    src/libraries/global_parameters.cpp \
    src/libraries/mt_styled_item_delegate.cpp \
    src/libraries/mt_table_widget.cpp \
    src/libraries/trash_monitoring.cpp \
    src/libraries/walk_history.cpp \
    src/libraries/window_switcher.cpp \
    src/models/app_config/app_config.cpp \
    src/models/app_config/app_config_updater.cpp \
    src/models/attach_table/attach.cpp \
    src/models/attach_table/attach_table_data.cpp \
    src/models/attach_table/attach_table_model.cpp \
    src/models/database_config/database_config.cpp \
    src/models/record_table/items_flat.cpp \
    src/models/record_table/record.cpp \
    src/models/record_table/record_index.cxx \
    src/models/record_table/record_model.cpp \
    src/models/record_table/record_model_proxy.cpp \
    src/models/tree/tree_xml.cpp \
    src/views/app_config/app_config_dialog.cpp \
    src/views/app_config/app_config_page_crypt.cpp \
    src/views/app_config/app_config_page_main.cpp \
    src/views/app_config/app_config_page_misc.cpp \
    src/views/app_config/app_config_page_synchro.cpp \
    src/views/app_config/config_dialog.cpp \
    src/views/app_config/config_page.cpp \
    src/views/attach_table/attach_table_screen.cpp \
    src/views/attach_table/attach_table_view.cpp \
    src/views/console_emulator/console_emulator.cpp \
    src/views/console_emulator/execute_command.cpp \
    src/views/dialog/over_text_tool_button.cpp \
    src/views/dialog/reduce_message_box.cpp \
    src/views/enter_password/enter_password.cpp \
    src/views/find_in_base_screen/find_screen.cpp \
    src/views/main_window/hidable_tabwidget.cpp \
    src/views/main_window/main_window.cpp \
    src/views/print_preview/view_preview.cpp \
    src/views/print_preview/print_preview.cpp \
    src/views/record/add_new_record.cpp \
    src/views/record/info_field_enter.cpp \
    src/views/record/meta_editor.cpp \
    src/views/record/record_info_fields_editor.cpp \
    src/views/record_table/record_print.cpp \
    src/views/record_table/record_screen.cpp \
    src/views/record_table/record_view.cpp \
    src/views/record_table/vertical_scrollarea.cpp \
    src/views/wait_clock/wait_clock.cpp \
    src/views/app_config/app_config_page_table.cpp
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

SOURCES     +=


wince* {
    CONFIG(debug, release|debug):addPlugins.sources     =   $$QT_BUILD_TREE/plugins/imageformats/qsvgd4.dll
    CONFIG(release, release|debug):addPlugins.sources   =   $$QT_BUILD_TREE/plugins/imageformats/qsvg4.dll
    addPlugins.path     =   imageformats
    DEPLOYMENT          +=  addPlugins
}

win32 {
    QMAKE_CXXFLAGS += /MP
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
    doc/binder.png \
    bin/browser.conf \
    bin/resource/standartconfig/android/browser.conf \
    bin/resource/standartconfig/android/entrance.ini \
    bin/resource/standartconfig/android/mode.ini \
    bin/resource/standartconfig/meego/browser.conf \
    bin/resource/standartconfig/meego/entrance.ini \
    bin/resource/standartconfig/meego/mode.ini

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


EXAMPLE_FILES   =   Info_mac.plist browser.icns browser.ico browser.rc  \
                        cookiejar.h cookiejar.cpp  # FIXME: these are currently unused.




#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5Svg
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5Svg
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5Svg

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5WebEngineWidgets
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5WebEngineWidgets
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5WebEngineWidgets

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5PrintSupport
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5PrintSupport
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5PrintSupport

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5Widgets
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5Widgets
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5Widgets

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5WebEngineCore
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5WebEngineCore
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5WebEngineCore

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5Quick
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5Quick
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5Quick

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5Gui
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5Gui
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5Gui

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5Xml
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5Xml
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5Xml

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5Qml
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5Qml
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5Qml

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5Network
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5Network
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5Network

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5WebChannel
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5WebChannel
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5WebChannel

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/release/ -lQt5Core
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/debug/ -lQt5Core
#else:unix: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5Core

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include

#unix|win32: LIBS += -L$$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/lib/ -lQt5WebEngine

#INCLUDEPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
#DEPENDPATH += $$PWD/../../GUI/Qt/Qt5.7.0/5.7/gcc_64/include
