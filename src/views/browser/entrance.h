#ifndef _BROWSERMANAGER_H_
#define _BROWSERMANAGER_H_

#include <QWidget>
#include <QList>
#include <QUrl>
#include <QDockWidget>
#include "utility/delegate.h"
#include "models/recordTable/Record.h"
#include "libraries/GlobalParameters.h"
#include "dockedwindow.h"
#include "webview.h"


class QNetworkReply;
class QSslError;
class QtSingleApplication;
class RecordTableController;
extern GlobalParameters globalparameters;
namespace browser {
    class WebView;
}
extern Record *request_record(const QUrl &_url
                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>
                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>
                             );

QT_BEGIN_NAMESPACE


namespace browser {

    // Architecture:
    // Entrance(<=>DockWidget)__DockedWindow(s)
    //               |
    //               |____DockedWindow
    //                         |
    //                         |____TabManager
    //                                  |
    //                                  |____PageView(s)
    //                                           |
    //                                           |____Page


    //    class DockWidget : public QDockWidget {
    //    public:
    //        explicit DockWidget(QWidget *parent, DockedWindow *browser, Qt::WindowFlags flags = Qt::WindowFlags());
    //    };


    // entrance.ini
    class Entrance : public QDockWidget {
        Q_OBJECT
    public:


        Entrance(RecordTableController *recordtablecontroller, const QString &style_source, QWidget *parent = 0, Qt::WindowFlags flags = 0);
        ~Entrance();
        //    BrowserWindow *getBrowserWindow() {return mainWindow();}
        //        void setupDynamicSignals(void);

        std::pair<DockedWindow *, WebView *> invoke_page(Record *const record);  //= register_record(QUrl(BrowserWindow::defaultHome))

        std::pair<DockedWindow *, WebView *> active_chain(Record *const record = nullptr);
        //        WebView *active_record_alternative(Record *const record) ;

        struct active_record_alternative {
            Entrance *_the;
            WebView *_view;
            active_record_alternative(Entrance *the): _the(the), _view(nullptr) {}
            WebView *generator(Record *const record) {return _view = _the->active_chain(record).second;}
            void activator() {_view->page()->active();}
        };

        //        DockedWindow *activebrowser();
        QList<QPointer<DockedWindow> > &window_list();  //        QList<DockedWindow*> window_raw_list();
        void clean();
        //        std::pair<DockedWindow *, WebView *> active_record(Record *const record);
        void active_url(const QUrl &url);
        bool restore_state(const QByteArray &state);
        std::pair<DockedWindow *, WebView *> find_record_in_browser(Record *const record);
        std::pair<DockedWindow *, WebView *> find(QUrl url);
        //BrowserView *create_view(Record *record, BrowserWindow *window);

        //        Q_INVOKABLE void runScriptOnOpenViews(const QString &);
        void settingstylesource(const QString &style_source) {_style_source = style_source;}

    public slots:

        void initSetting(void);


        void setScrollbars(bool hide);
        void setCache(bool cache, int cacheSize);
        void finished(QNetworkReply *reply);
        void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
        QAction *getactionFreeze() { return _actionFreeze; }
        //        void openLinkInNewTab();
        //        void onLoadFinished(bool);
        void setupActions(void);

        //        WebView *new_dockedwindow(Record *const record);
        WebView *new_dockedwindow_view(QUrl const &url);
        std::pair<DockedWindow *, WebView *> new_dockedwindow(QUrl const &url);
        DockedWindow *new_dockedwindow(const QByteArray &state);
        Entrance *prepend(DockedWindow *);
        void on_activate_window();

#if defined(Q_OS_OSX)
        void lastWindowClosed();
#endif

    private slots:
        //        void loadUrl(Record *record);
        //        void loadUrl(const int pos);
        void openUrl(const QUrl &url);
        //        void setUrl(const QUrl &url);

    private:

        //        void(Entrance::*invoke_ptr)(const QUrl &url);

        void setupUI(void);
        void setupSignals(void);
        void assembly(void);

        QList<QPointer<DockedWindow> > _mainWindows;
        RecordTableController *_recordtablecontroller;
        QString _style_source;
        //void urlChanged(const QUrl &_url){onUrlChanged(_url);}
        QAction *_actionFreeze;
        //struct BrowserViewPrivate *d;
        //        QDockWidget *_dockwidget;
        //        DockedWindow *_browser;
        QWidget *_hidetitlebar;
        QMetaObject::Connection _home_connection;    // for disconnect
        friend class QtSingleApplication;
    };

}


QT_END_NAMESPACE

#endif // _BROWSERMANAGER_H_
