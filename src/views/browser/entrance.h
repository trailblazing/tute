#ifndef _BROWSERMANAGER_H_
#define _BROWSERMANAGER_H_

#include <QWidget>
#include <QList>
#include <QUrl>
#include <QDockWidget>
#include "utility/delegate.h"
#include "models/record_table/Record.h"
#include "libraries/GlobalParameters.h"
#include "browser.h"
#include "webview.h"


class QNetworkReply;
class QSslError;
class QtSingleApplication;
class TableController;
extern GlobalParameters globalparameters;
namespace browser {
    class WebView;
}

//extern std::shared_ptr<Record> request_record(const QUrl &_url
//                                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>>
//                                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, std::shared_ptr<Record>>>
//                                             );

QT_BEGIN_NAMESPACE


namespace browser {

    // Architecture:
    // Entrance(<=>DockWidget)__DockedWindow(s)
    //               |
    //               |____Browser
    //                         |
    //                         |____TabWidget
    //                                  |
    //                                  |____WebView(s)
    //                                           |
    //                                           |____WebPage


    //    class DockWidget : public QDockWidget {
    //    public:
    //        explicit DockWidget(QWidget *parent, DockedWindow *browser, Qt::WindowFlags flags = Qt::WindowFlags());
    //    };


    // entrance.ini
    class Entrance : public QDockWidget {
        Q_OBJECT
    public:


        Entrance(QString object_name, TableController *recordtablecontroller, TableController *_page_controller, browser::ToolbarSearch *toolbarsearch, const QString &style_source, QWidget *parent = 0, Qt::WindowFlags flags = 0);
        ~Entrance();
        //    BrowserWindow *getBrowserWindow() {return mainWindow();}
        //        void setupDynamicSignals(void);

        std::pair<Browser *, WebView *> invoke_page(Record *const record);  //= register_record(QUrl(DockedWindow::_defaulthome))

        std::pair<Browser *, WebView *> equip_registered(std::shared_ptr<Record> record = std::shared_ptr<Record>(nullptr));
        //        WebView *active_record_alternative(Record *const record) ;

        struct ActiveRecordBinder {
            Entrance    *_the;
            //            WebPage     *_page;
            ActiveRecordBinder(Entrance *the): _the(the)    //              , _page(nullptr)
            {}

            WebView *binder(std::shared_ptr<Record> record)
            {
                //                _page = _the->equip_registered(record).second->page();
                return _the->equip_registered(record).second;
            }

            WebView *activator(std::shared_ptr<Record> record) {return record->unique_page()->active();}
        };

        std::pair<Browser *, WebView *> activiated_registered();
        QList<QPointer<Browser> > &window_list();  //        QList<DockedWindow*> window_raw_list();
        void clean();
        //        std::pair<DockedWindow *, WebView *> active_record(Record *const record);
        void active_url(const QUrl &url);
        bool restore_state(const QByteArray &state);
        std::pair<Browser *, WebView *> find(std::shared_ptr<Record> record);
        std::pair<Browser *, WebView *> find(QUrl url);
        //BrowserView *create_view(Record *record, BrowserWindow *window);

        //        Q_INVOKABLE void runScriptOnOpenViews(const QString &);
        void settingstylesource(const QString &style_source) {_style_source = style_source;}

    public slots:

        void initSetting(void);


        void setScrollbars(bool hide);
        void setCache(bool cache, int cacheSize);
        void finished(QNetworkReply *reply);
        void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
        //        QAction *getactionFreeze() { return _actionFreeze; }
        //        void openLinkInNewTab();
        //        void onLoadFinished(bool);
        void setupActions(void);

        //        WebView *new_dockedwindow(Record *const record);
        WebView *new_view(QUrl const &url);
        std::pair<Browser *, WebView *> new_browser(QUrl const &url);
        std::pair<Browser *, WebView *> new_browser(std::shared_ptr<Record> record);
        Browser *new_browser(const QByteArray &state);
        Entrance *prepend(Browser *);
        void on_activate_window();
        //        void on_splitter_moved(int pos, int index);

#if defined(Q_OS_OSX)
        void lastWindowClosed();
#endif

    private slots:
        //        void loadUrl(Record *record);
        //        void loadUrl(const int pos);
        void openUrl(const QUrl &url);
        //        void setUrl(const QUrl &url);
    protected:
        void resizeEvent(QResizeEvent *);
    private:

        //        void(Entrance::*invoke_ptr)(const QUrl &url);

        void setupUI(void);
        void setupSignals(ToolbarSearch *toolbarsearch);
        void assembly(void);

        QList<QPointer<Browser> >   _main_windows;
        TableController             *_record_controller;
        TableController             *_page_controller;
        QString                     _style_source;
        //void urlChanged(const QUrl &_url){onUrlChanged(_url);}
        QAction                     *_actionFreeze;
        //struct BrowserViewPrivate *d;
        //        QDockWidget *_dockwidget;
        //        DockedWindow *_browser;
        QWidget                     *_hidetitlebar;
        QMetaObject::Connection     _home_connection;    // for disconnect
        friend class QtSingleApplication;
    };

}


QT_END_NAMESPACE

#endif // _BROWSERMANAGER_H_
