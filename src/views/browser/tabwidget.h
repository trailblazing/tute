/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include "utility/delegate.h"
#include <QtWebEngineWidgets/QWebEngineFullScreenRequest>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QShortcut>
#include "models/recordTable/Record.h"
#include "controllers/recordTable/RecordTableController.h"
#include "views/recordTable/RecordTableScreen.h"
#include "libraries/GlobalParameters.h"
#include "models/recordTable/RecordTableModel.h"
#include "models/recordTable/RecordTableData.h"
#include "views/browser/history.h"
#include "views/browser/browser.h"
#include "views/browser/webview.h"
#include "utility/delegate.h"


class GlobalParameters;
extern GlobalParameters globalparameters;

QT_BEGIN_NAMESPACE
class QWebEngineDownloadItem;
class QWebEngineProfile;
class QWebEngineView;
QT_END_NAMESPACE

#include <QWebEnginePage>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

#include <QtCore/QUrl>
#include <QtWidgets/QTabWidget>

QT_BEGIN_NAMESPACE

class QCompleter;
class QLineEdit;
class QMenu;
class QStackedWidget;
QT_END_NAMESPACE

namespace browser {
    class WebView;
}

extern Record *request_record(
    const QUrl &_url
    , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, Record *const>>
    , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, void>>
);

QT_BEGIN_NAMESPACE

namespace browser {

    class Browser;
    class FullScreenNotification;

    /*
        Tab bar with a few more features such as a context menu and shortcuts
     */
    class TabBar : public QTabBar {
        Q_OBJECT

    signals:
        void newTab();
        void cloneTab(int index);
        void closeTab(int index);
        void closeOtherTabs(int index);
        void reloadTab(int index);
        void reloadAllTabs();
        void tabMoveRequested(int fromIndex, int toIndex);

    public:
        TabBar(QWidget *parent = 0);

    protected:
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);

    private slots:
        void selectTabAction();
        void cloneTab();
        void closeTab();
        void closeOtherTabs();
        void reloadTab();
        void contextMenuRequested(const QPoint &position);

    private:
        QList<QShortcut *> _tabshortcuts;
        friend class TabWidget;

        QPoint _dragstartpos;
        int _dragcurrentindex;
    };

    //#include <QWebEnginePage>

    //    QT_BEGIN_NAMESPACE
    //    class QAction;
    //    QT_END_NAMESPACE


    //    template<typename T>
    class WebView;

    /*!
        A proxy object that connects a single browser action
        to one child webpage action at a time.

        Example usage: used to keep the main window stop action in sync with
        the current tabs webview's stop action.
     */
    class WebActionMapper : public QObject {
        Q_OBJECT

    public:
        WebActionMapper(QAction *root, QWebEnginePage::WebAction webAction, QObject *parent);
        QWebEnginePage::WebAction webAction() const;
        void addChild(QAction *action);
        void updateCurrent(QWebEnginePage *currentParent);

    private slots:
        void rootTriggered();
        void childChanged();
        void rootDestroyed();
        void currentDestroyed();

    private:
        QWebEnginePage *_currentparent;
        QAction *_root;
        QWebEnginePage::WebAction _webaction;
    };

    //#include <QtCore/QUrl>
    //#include <QtWidgets/QTabWidget>
    //    QT_BEGIN_NAMESPACE
    //    class QCompleter;
    //    class QLineEdit;
    //    class QMenu;
    //    class QStackedWidget;
    //    QT_END_NAMESPACE




    /*!
        TabWidget that contains WebViews and a stack widget of associated line edits.

        Connects up the current tab's signals to this class's signal and uses WebActionMapper
        to proxy the actions.
     */

    // browsertabmanager
    class TabWidget : public QTabWidget {
        Q_OBJECT

    signals:
        // tab widget signals
        void loadPage(const QString &url);
        void tabsChanged();
        void lastTabClosed();

        // current tab signals
        void setCurrentTitle(const QString &url);
        void showStatusBarMessage(const QString &message);
        void linkHovered(const QString &link);
        void loadProgress(int progress);
        void geometryChangeRequested(const QRect &geometry);
        void menuBarVisibilityChangeRequested(bool visible);
        void statusBarVisibilityChangeRequested(bool visible);
        void toolBarVisibilityChangeRequested(bool visible);

#if defined(QWEBENGINEPAGE_PRINTREQUESTED)
        void printRequested(QWebEngineFrame *frame);
#endif

    public:
        TabWidget(RecordTableController *recordtablecontroller, Browser *parent);
        ~TabWidget();
        void clear();
        void addWebAction(QAction *action, QWebEnginePage::WebAction webAction);

        QAction *newTabAction() const;
        QAction *closeTabAction() const;
        QAction *recentlyClosedTabsAction() const;
        QAction *nextTabAction() const;
        QAction *previousTabAction() const;

        //QWidget *
        QStackedWidget *lineEditStack() const;
        void lineEditStack(QStackedWidget *lineedits);
        QLineEdit *currentLineEdit() const;
        WebView *currentWebView() const;
        WebView *webView(int index) const;
        QLineEdit *lineEdit(int index) const;
        int webViewIndex(WebView *webView) const;

        QByteArray saveState() const;
        bool restoreState(const QByteArray &state);

        void setProfile(QWebEngineProfile *setProfile);

        WebView *find(const QUrl &url) const;
        WebView *find_nopin()const;
        Browser *browser() {return _window;}

        struct NewTab {
            TabWidget *_the;
            bool _make_current;
            RecordTableController *_recordtablecontroller;
            WebView *view;
            NewTab(TabWidget *the
                   , bool make_current
                   = true
                     , RecordTableController *recordtablecontroller
                   = globalparameters.getRecordTableScreen()->getRecordTableController()
                  ): _the(the) , _make_current(make_current), _recordtablecontroller(recordtablecontroller), view(nullptr) {}
            WebView *generator(Record *const record)
            {
                return view = _the->newTab(record, _make_current, _recordtablecontroller);
            }
            void activator()
            {
                view->page()->active();
            }

        };

    protected:
        void mouseDoubleClickEvent(QMouseEvent *event);
        void contextMenuEvent(QContextMenuEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);

    public slots:
        void loadUrlInCurrentTab(const QUrl &url);

        //        void new_view_void() {newTab(false);}
        //BrowserView *new_dummy();

        WebView *newTab(Record *const record
                        // = request_record(QUrl(DockedWindow::_defaulthome))
                        //  , bool openinnewtab = false
                        , bool make_current
                        = true
                          , RecordTableController *recordtablecontroller
                        = globalparameters.getRecordTableScreen()->getRecordTableController()

                       );

        //        void new_view(bool make_current = false);    //true

        void cloneTab(int index = -1);
        void requestCloseTab(int index = -1);
        void closeTab(int index = -1);
        void closeOtherTabs(int index);
        void reloadTab(int index = -1);
        void reloadAllTabs();
        void nextTab();
        void previousTab();

    private slots:
        void currentChanged(int index);
        void aboutToShowRecentTabsMenu();
        void aboutToShowRecentTriggeredAction(QAction *action);
        void downloadRequested(QWebEngineDownloadItem *download);
        void webViewLoadStarted();
        void webViewIconChanged();
        void webViewTitleChanged(const QString &title);
        void webViewUrlChanged(const QUrl &url);
        void lineEditReturnPressed();
        void windowCloseRequested();
        void moveTab(int fromIndex, int toIndex);
        void onTabsChanged();
        void fullScreenRequested(QWebEngineFullScreenRequest request);
    private:
        QAction *_recentlyclosedtabsaction;
        QAction *_newtabaction;
        QAction *_closetabaction;
        QAction *_nexttabaction;
        QAction *_previoustabaction;

        QMenu *_recentlyclosedtabsmenu;
        static const int _recentlyclosedtabssize = 10;
        QList<QUrl> _recentlyclosedtabs;
        QList<WebActionMapper *> _actions;
        // HistoryCompletionModel *_completionModel;// = new HistoryCompletionModel(this);
        QCompleter *_lineeditcompleter;// = new QCompleter(_completionModel, this);
        QStackedWidget *_lineedits;
        TabBar *_tabbar;
        QWebEngineProfile *_profile;
        QWebEngineView *_fullscreenview;
        FullScreenNotification *_fullscreennotification;
        RecordTableController *_recordtablecontroller;
        friend class ToolbarSearch;

        //        active_record _active_record;
        //        sd::_interface<sd::meta_info<void *>, WebView *, Record *const> _active;
        //        //        sd::method<sd::meta_info<void *const>> _active_r;
        Browser *_window;

    };

    class PopupWindow :
        public TabWidget {  // public QWidget
        Q_OBJECT
    public:
        PopupWindow(QWebEngineProfile *const setProfile, QUrl const &url, RecordTableController *_recordtablecontroller, Browser *parent);

        QWebEnginePage *page() const;

    private Q_SLOTS:
        void setUrl(const QUrl &url);

        void adjustGeometry(const QRect &newGeometry);

    private:
        QLineEdit   *_addressbar;
        WebView     *_view;

    };

}

QT_END_NAMESPACE

#endif // TABWIDGET_H



