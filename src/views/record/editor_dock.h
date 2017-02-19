#ifndef _EDITORMANAGER_H_
#define _EDITORMANAGER_H_

#include <set>

#include "libraries/qtm/editing_window.h"
#include "utility/delegate.h"
#include <QDockWidget>
#include <QList>
#include <QTextDocument>
#include <QUrl>
#include <QWidget>
// #include "models/record_table/Record.h"
// #include "libraries/GlobalParameters.h"
// #include "browser.h"
// #include "webview.h"
// #include "views/record/MetaEditor.h"
// #include "models/tree/TreeItem.h"

// #include "views/browser/tabwidget.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class tkm_t;
class QNetworkReply;
class QSslError;
class sapp_t;
class rctrl_t;
class HidableTabWidget;
class ts_t;
class FindScreen;
class EditorWrap;
class wn_t;
class AppConfig;
class Record;
class TreeItem;
class gl_para;
class EditorWrap;
class EditorTextArea;
class FlatToolButton;
class QVBoxLayout;
class EditingWindow;
class SuperMenu;

extern std::shared_ptr<gl_para> gl_paras;

namespace browser {
class WebView;
class Browser;
class ToolbarSearch;
class Profile;
class BrowserDock;
}

// QT_BEGIN_NAMESPACE

// namespace browser {
// Architecture:
// Entrance(<=>DockWidget)__DockedWindow(s)
//               |
//               |____Browser(s)
//                         |
//                         |____TabWidget
//                                  |
//                                  |____WebView(s)
//                                           |
//                                           |____WebPage

class EditorDock : public QDockWidget {
#if QT_VERSION == 0x050600
    W_OBJECT(Entrance)
#else
    Q_OBJECT
#endif

public:
    EditorDock(QString object_name, ts_t* tree_screen, FindScreen* _find_screen, HidableTabWidget* vtab_record, browser::BrowserDock* browser_dock, wn_t* main_window
        //		   , std::shared_ptr<AppConfig> appconfig_
        ,
        const QString& style_source, browser::Profile* profile, Qt::WindowFlags flags = 0);
    ~EditorDock();

    void style_source(const QString& style_source);
    QString style_source();

    //	EditorWrap *editor();

    // #endif

    //	EditingWindow *blog_editor();
    //	EditingWindow *add_blog_editor(EditingWindow *ew);
    SuperMenu* super_menu();
    // Метод установки функции переключения на предыдущее окно (для мобильного
    // интерфейса)
    void back_callback(void (*func)(void));
    // Q_SIGNALS:
signals:
    void editing_activated(EditingWindow* w);
public slots:
    void editor_switch(void);
    void init_setting(void);

    void set_scrollbars(bool hide);
    void set_cache(bool cache, int cache_size);
    void finished(QNetworkReply* reply);
    void ssl_errors(QNetworkReply* reply, const QList<QSslError>& errors);

    void setup_actions(void);

    EditorDock* prepend(browser::Browser*);
    void on_activate_window();

#if defined(Q_OS_OSX)
    void lastWindowClosed();
#endif

private slots:

    void on_back_clicked();

protected:
    void resizeEvent(QResizeEvent* e);

private:
    void setup_ui(void);

    void assembly(void);
    // Указатель на функцию переключения на предыдущее окно (для мобильного интерфейса)
    void (*back_callback_func)(void);

    //	QList<EditingWindow *> _editing_list;
    ts_t* _tree_screen = nullptr;
    FindScreen* _find_screen = nullptr;
    //	MetaEditor                          *_editor_screen;
    //        HidableTabWidget                *_vtab_tree;
    browser::BrowserDock* _entrance = nullptr;
    wn_t* _main_window = nullptr;
    //	std::shared_ptr<AppConfig> _appconfig;
    // RecordController                *_record_controller;
    QString _style_source;
    browser::Profile* _profile = nullptr;
    Qt::WindowFlags _flags = 0;
    QWidget* _hidetitlebar = nullptr;

    SuperMenu* _super_menu = nullptr;
    EditingWindow* _blog_editor = nullptr;
    //		MetaEditor                          *_editor		=
    // nullptr;

    //		QSplitter *_splitter;
    //		std::shared_ptr<QSettings> _splitter_config;
    //		QString _splitter_groupname;
    //		QString _splitter_sizelist;

    QMetaObject::Connection _home_connection; // for disconnect

    friend class sapp_t;
    friend class EditorWrap;
    friend class Editor;
    friend class EditingWindow;
    friend class SideTabWidget;
    //	EditingWindow *blog_editor() const;
};
// }

// QT_END_NAMESPACE

#endif // _EDITORMANAGER_H_
