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
class RecordController;
extern GlobalParameters globalparameters;
namespace browser {
    class WebView;
}

//extern boost::intrusive_ptr<Record> request_record(const QUrl &_url
//                                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>>
//                                              , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>>
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


        Entrance(QString object_name
                 , RecordController *record_controller
                 , boost::intrusive_ptr<TreeItem> _shadow_branch_root
                 , browser::ToolbarSearch *toolbarsearch
                 , const QString &style_source
                 , QWidget *parent
                 = 0
                   , Qt::WindowFlags flags
                 = 0
                );
        ~Entrance();
        //    BrowserWindow *getBrowserWindow() {return mainWindow();}
        //        void setupDynamicSignals(void);

        std::pair<Browser *, WebView *> invoke_page(Record *const record);  //= register_record(QUrl(DockedWindow::_defaulthome))

        std::pair<Browser *, WebView *> equip_registered(boost::intrusive_ptr<TreeItem> record    // = boost::intrusive_ptr<Record>(nullptr)
                                                        );
        //        WebView *active_record_alternative(Record *const record) ;

        struct ActiveRecordBinder {
            Entrance    *_the;
            //            WebPage     *_page;
            ActiveRecordBinder(Entrance *the): _the(the)    //              , _page(nullptr)
            {}

            WebView *binder(boost::intrusive_ptr<TreeItem> item, boost::intrusive_ptr<TreeItem>(TreeItem::* _bind)(WebPage *))
            {
                //                _page = _the->equip_registered(record).second->page();
                WebView *view = _the->equip_registered(item).second;
                (item.get()->*_bind)(view->page());
                return view;
            }

            WebView *activator(boost::intrusive_ptr<TreeItem> item) {assert(item->page_valid()); return item->unique_page()->active();}
        };

        std::pair<Browser *, WebView *> activiated_registered();
        QList<QPointer<Browser> > &window_list();  //        QList<DockedWindow*> window_raw_list();
        void clean();
        //        std::pair<DockedWindow *, WebView *> active_record(Record *const record);
        void active_url(const QUrl &url);
        void active(boost::intrusive_ptr<TreeItem> item);
        bool restore_state(const QByteArray &state);
        std::pair<Browser *, WebView *> find(boost::intrusive_ptr<TreeItem> record);
        std::pair<Browser *, WebView *> find(QUrl url);
        //BrowserView *create_view(Record *record, BrowserWindow *window);

        //        Q_INVOKABLE void runScriptOnOpenViews(const QString &);
        void settingstylesource(const QString &style_source) {_style_source = style_source;}

    public slots:

        void init_setting(void);


        void set_scrollbars(bool hide);
        void set_cache(bool cache, int cache_size);
        void finished(QNetworkReply *reply);
        void ssl_errors(QNetworkReply *reply, const QList<QSslError> &errors);
        //        QAction *getactionFreeze() { return _actionFreeze; }
        //        void openLinkInNewTab();
        //        void onLoadFinished(bool);
        void setup_actions(void);

        //        WebView *new_dockedwindow(Record *const record);
        WebView *new_view(QUrl const &url);
        std::pair<Browser *, WebView *> new_browser(QUrl const &url);
        std::pair<Browser *, WebView *> new_browser(boost::intrusive_ptr<TreeItem> record);
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
        //        void open_url(const QUrl &url);
        //        void setUrl(const QUrl &url);
    protected:
        void resizeEvent(QResizeEvent *);
    private:

        //        void(Entrance::*invoke_ptr)(const QUrl &url);

        void setup_ui(void);
        void setup_signals(ToolbarSearch *toolbarsearch);
        void assembly(void);

        QList<QPointer<Browser> >       _main_windows;
        RecordController                *_record_controller;
        boost::intrusive_ptr<TreeItem>  _shadow_branch_root;
        QString                         _style_source;
        //void urlChanged(const QUrl &_url){onUrlChanged(_url);}
        QAction                         *_actionFreeze;
        //struct BrowserViewPrivate *d;
        //        QDockWidget *_dockwidget;
        //        DockedWindow *_browser;
        QWidget                         *_hidetitlebar;
        QMetaObject::Connection         _home_connection;    // for disconnect
        friend class QtSingleApplication;
    };

}


QT_END_NAMESPACE

#endif // _BROWSERMANAGER_H_
