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
#include "models/record_table/Record.h"
#include "models/tree/TreeItem.h"
#include "controllers/record_table/RecordController.h"
#include "views/record_table/RecordScreen.h"
#include "libraries/GlobalParameters.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/ItemsFlat.h"
#include "views/browser/history.h"
#include "views/browser/browser.h"
#include "views/browser/webview.h"
#include "utility/delegate.h"
#include "models/tree/TreeItem.h"
#include "views/tree/TreeScreen.h"
#include "views/tree/KnowView.h"
#include "models/tree/KnowModel.h"
#include "libraries/DiskHelper.h"





class GlobalParameters;
extern GlobalParameters globalparameters;
extern QMap<Qt::CheckState, QString> _string_from_check_state;
extern QMap<QString, Qt::CheckState> _state_check_from_string;
extern QString get_unical_id(void);


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
class TreeItem;
class RecordModel;
class RecordView;
struct CouplerDelegation;




QT_END_NAMESPACE

namespace browser {
    class WebView;
}

//extern boost::intrusive_ptr<Record> request_record(
//    const QUrl &_url
//    , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>>
//    , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>, browser::WebView *, boost::intrusive_ptr<Record>>>
//);

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
        void cloneTabSignal(int index);
        void closeTabSignal(int index);
        void closeOtherTabsSignal(int index);
        void reloadTabSignal(int index);
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
        void showStatusBarMessage(const QString &message, int timeout = 0);
        void linkHovered(const QString &link);  // , int timeout = 0);
        void loadProgress(int progress);
        void geometryChangeRequested(const QRect &geometry);
        void menuBarVisibilityChangeRequested(bool visible);
        void statusBarVisibilityChangeRequested(bool visible);
        void toolBarVisibilityChangeRequested(bool visible);

#if defined(QWEBENGINEPAGE_PRINTREQUESTED)
        void printRequested(QWebEngineFrame *frame);
#endif

    public:
        TabWidget(FindScreen *_find_screen, MetaEditor *_editor_screen, RecordScreen *_record_screen, Browser *_browser, MainWindow *_main_window);
        //                  , TableController *_page_controller
        //                  , boost::intrusive_ptr<TreeItem> _shadow_branch_root

        typedef typename CouplerDelegation::bind_interface          bind_interface;
        typedef typename CouplerDelegation::activate_interface      activate_interface;
        typedef typename CouplerDelegation::bounded_item_interface  bounded_item_interface;
        typedef typename CouplerDelegation::bounded_page_interface  bounded_page_interface;

        typedef typename CouplerDelegation::bind_helper         bind_helper;
        typedef typename CouplerDelegation::activate_helper     activate_helper;
        typedef typename CouplerDelegation::bounded_item_helper bounded_item_helper;
        typedef typename CouplerDelegation::bounded_page_helper bounded_page_helper;

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
        //        void lineEditStack(QStackedWidget *lineedits);
        QLineEdit *currentLineEdit() const;
        WebView *currentWebView() const;
        WebView *webView(int index) const;
        QLineEdit *lineEdit(int index) const;
        int webViewIndex(WebView *webView) const;

        QByteArray saveState() const;
        bool restoreState(const QByteArray &state);

        void setProfile(QWebEngineProfile *setProfile);

        template<typename url_type = url_full>
        WebView * find(const QUrl &find_url) const
        {
            WebView *view = nullptr;

            for(int i = 0; i < count(); i++) {
                auto current_view = webView(i);

                if(current_view != nullptr) {
                    auto it = current_view->page()->bounded_item();

                    if(it) {

                        if(it->url<url_type>() == url_type()(find_url)) {
                            view = current_view;
                            break;
                        }

                    }
                }
            }

            return view;
        }

        WebView *find(boost::intrusive_ptr<TreeItem> it_find)const;
        WebView *find_nopin()const;
        Browser *browser() {return _browser;}

        struct Coupler : public std::enable_shared_from_this<Coupler> {
            TabWidget                       *_tabmanager;
            boost::intrusive_ptr<TreeItem>  _bounded_item;
            WebPage                         *_bounded_page;
            bool                            _make_current;

            Coupler(
                TabWidget                           *_tabmanager
                , boost::intrusive_ptr<TreeItem>    _bounded_item
                , bool                              _make_current = true
            );

            //                : _tabmanager(_tabmanager), _bounded_item(_bounded_item), _bounded_page(nullptr), _make_current(_make_current)
            //            {
            //                _bounded_item->record_binder(std::make_shared<CouplerDelegation>(
            //                                                 std::make_shared<bounded_item_interface>("", &Coupler::bounded_item, shared_from_this())
            //                                                 , std::make_shared<bounded_page_interface>("", &Coupler::bounded_page, shared_from_this())
            //                                                 , std::make_shared<bind_interface>("", &Coupler::binder, shared_from_this())
            //                                                 , std::make_shared<activate_interface> ("", &Coupler::activator, shared_from_this())
            //                                             ));
            //            }


            WebView *binder();   // , boost::intrusive_ptr<TreeItem>(TreeItem::* _bind)(WebPage *)
            boost::intrusive_ptr<TreeItem> bounded_item() {return _bounded_item;}
            WebPage *bounded_page() {return _bounded_page;}
            WebView *activator();


        };

        //        void sychronize_metaeditor_to_record(boost::intrusive_ptr<Record> record);
        //        RecordModel *source_model() {return _record_controller->source_model();}
        //        void tree_item(boost::intrusive_ptr<TreeItem> item) {_shadow_branch_root = item;}

        //        std::shared_ptr<RecordTable> table_data() {return _page_tree_item->record_table();}
        //        void reset_tabledata(std::shared_ptr<RecordTable> table_data) {_page_tree_item->record_table(table_data);}

        TabBar *tabbar() {return _tabbar;}
        RecordController *record_controller() {return _record_controller;}

        boost::intrusive_ptr<TreeItem> item_request_from_tree_impl(boost::intrusive_ptr<TreeItem> item);
        boost::intrusive_ptr<TreeItem> item_request_from_tree(boost::intrusive_ptr<TreeItem> item);

        // boost::intrusive_ptr<TreeItem> item_request_from_tree_impl(const QUrl &_url);
        template<typename url_type = url_full>
        inline boost::intrusive_ptr<TreeItem> item_request_from_tree_impl(const QUrl &_url)
        {
            TreeScreen *_tree_screen    = globalparameters.tree_screen();
            //    auto _know_model_root = tree_screen->know_root();
            auto _know_model_board      = _tree_screen->know_model_board();
            auto _current_view_model    = _tree_screen->tree_view()->source_model();

            boost::intrusive_ptr<TreeItem> _result(nullptr);    // =  _know_model_board->root_item();
            //    boost::intrusive_ptr<TreeItem> _source_root_item = tree_screen->know_branch()->item(TreeModel::delegater(_url));    // on know_root semantic
            boost::intrusive_ptr<TreeItem> _source_item = _know_model_board->item_by_url<url_type>(_url);

            //    if(_source_root_item && !_source_item) {
            //        auto result = tree_screen->cut_from_root(_source_root_item);

            //        if(result)_source_item = tree_screen->paste_to_branch(result, _know_model_branch);

            //        assert(result);
            //        assert(_source_item);
            //        assert((_source_item == result) && (result == _source_root_item));
            //    }

            bool item_is_brand_new = false;
            //    //    if(_record_controller) {
            //    auto browser_pages = this->_source_model->browser_pages();
            //    assert(browser_pages);

            //    if(browser_pages) {
            auto v = find<url_type>(_url);

            if(v) {
                _result = v->page()->record_binder()->bounded_item();
                assert(_result->url<url_type>() == url_type()(_url));
            }

            if(_source_item && _source_item != _know_model_board->root_item()) {
                if(!_result) {
                    assert(_source_item->url<url_type>() == url_type()(_url));

                    if(_source_item->is_lite())_source_item->to_fat();

                    //            //            _source_item->binder(generator);
                    //            //            _source_item->activator(activator);

                    //            //            _item = register_item_to_browser_source_model(_source_item);

                    //            _source_item->is_registered_to_record_controller_and_tabmanager(false);
                    //            // _source_item->self_bind();
                    _result = _source_item;
                } else {
                    assert(_result->url<url_type>() == url_type()(_url));
                    assert(_source_item->url<url_type>() == url_type()(_url));
                    // assert(_result->fragment() == _source_item->fragment());

                    if(_result != _source_item) {
                        _result = _source_item; // assert(_item.get() == _source_item.get());
                        // _result = const_cast<KnowModel *>(_know_model_board)->duplicated_remove(_result, _source_item);
                    }

                    if(_result->is_lite())_result->to_fat();

                    if(_result->field("id") == "")_result->field("id", get_unical_id());

                    assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);

                    //            //            _result->binder(generator);
                    //            //            _result->activator(activator);

                    //            _result->is_registered_to_record_controller_and_tabmanager(false);
                    //            // _item->self_bind();
                }

                assert(!_result->is_lite());
                //        assert(_result->is_registered_to_record_controller_and_tabmanager());

            } else {
                item_is_brand_new = true;

                if(!_result) {

                    //                int pos = _record_ontroller->getFirstSelectionPos();
                    //                Record *previous_record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

                    //                if(previous_record) {

                    //                    Record record;

                    //                    if(record.isLite())record.switchToFat();

                    //                    //QString title = d->view->title(); // not ready yet
                    //                    //record.setNaturalFieldSource("id",   previous_record->getNaturalFieldSource("id"));   // id concept?
                    //                    record.setNaturalFieldSource("pin",   "");
                    //                    record.setNaturalFieldSource("name",   previous_record->getNaturalFieldSource("name"));
                    //                    record.setNaturalFieldSource("author", previous_record->getNaturalFieldSource("author"));
                    //                    record.setNaturalFieldSource("url",    _url.toString());    // only changed
                    //                    record.setNaturalFieldSource("tags",   previous_record->getNaturalFieldSource("tags"));

                    //                    _record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();
                    //                    _record = recordtabledata->getRecordByUrl(_url);
                    //                    //                int pos = _record_ontroller->getFirstSelectionPos();
                    //                    //                _record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);
                    //                } else {




                    //    record.generator(generator);


                    // Имя директории, в которой расположены файлы картинок, используемые в тексте и приаттаченные файлы
                    QString directory = DiskHelper::create_temp_directory();  //

                    QMap<QString, QString> data;
                    data["id"]      = get_unical_id();
                    data["pin"]     = _string_from_check_state[Qt::Unchecked];
                    data["name"]    = "";
                    data["author"]  = "";
                    data["home"]    = _url.toString();
                    data["url"]     = _url.toString();
                    data["tags"]    = "";
                    data["dir"]     = data["id"];
                    data["file"]    = "text.html";

                    boost::intrusive_ptr<TreeItem> item
                        = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));

                    //                if(record.isLite())
                    item->to_fat();
                    item->text_to_fat("");
                    //            //                QString title = _url.toString(); // not ready yet
                    //            item->field("id",       get_unical_id());
                    //            item->field("pin",      _check_state[Qt::Unchecked]);
                    //            item->field("name",     "");
                    //            item->field("author",   "");
                    //            item->field("home",     _url.toString());    // only changed
                    //            item->field("url",      _url.toString());    // only changed
                    //            item->field("tags",     "");

                    //            //                _record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);   //recordTableController->autoAddNewAfterContext();
                    //            //                _record = recordtabledata->getRecordByUrl(_url);
                    //            //                //                int pos = _record_ontroller->getFirstSelectionPos();
                    //            //                //                _record = _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

                    //            //                //            }

                    //            //                record->binder(generator);
                    //            //                record->activator(activator);

                    item->picture_files(DiskHelper::get_files_from_directory(directory, "*.png"));


                    // Пока что принята концепция, что файлы нельзя приаттачить в момент создания записи
                    // Запись должна быть создана, потом можно аттачить файлы.
                    // Это ограничение для "ленивого" программинга, но пока так
                    // record->setAttachFiles( DiskHelper::getFilesFromDirectory(directory, "*.bin") );

                    // Временная директория с картинками и приаттаченными файлами удаляется
                    DiskHelper::remove_directory(directory);

                    if(item->is_lite())item->to_fat();

                    //            //            item->binder(generator);
                    //            //            item->activator(activator);

                    //            //            _item = register_item_to_browser_source_model(item);
                    //            item->is_registered_to_record_controller_and_tabmanager(true);
                    //            // item->self_bind();
                    //            //                assert(_record);
                    //            //                assert(_record->is_registered());
                    //            //                _record->active_immediately(active_immediately);
                    //            //                _record->generator(generator);


                    _result = item; // assert(_item.get() == item.get());
                } else {
                    assert(_result->url<url_type>() == url_type()(_url));

                    if(_result->is_lite())_result->to_fat();

                    if(_result->field("id") == "")_result->field("id", get_unical_id());

                    assert(_result->is_registered_to_browser() || _result->field("url") == browser::Browser::_defaulthome);

                    //            //            _result->binder(generator);
                    //            //            _result->activator(activator);

                    //            _result->is_registered_to_record_controller_and_tabmanager(true);
                    //            // _item->self_bind();
                }

                {
                    auto _view_index = _tree_screen->tree_view()->index_current();

                    // if(idx.isValid()) {

                    auto it = _current_view_model->item(_view_index);  //item_from_id(static_cast<TreeItem *>(_view_index.internalPointer())->id());
                    assert(it);
                    assert(_current_view_model->is_id_exists(it->id()));

                    //        if(_item->is_lite())_item->to_fat();

                    if(it != _result && item_is_brand_new) {
                        assert(_result->url<url_type>() == url_type()(_url));
                        // assert(_result->fragment() == _url.fragment());
                        // int pos
                        _result = _tree_screen->branch_paste_as_sibling(_current_view_model, _view_index, _result); // it->insert_new_item(it->current_count() - 1, _result);
                        assert(_result);
                        _tree_screen->synchronized(false);
                        // assert(_result == it->child(pos));
                    }

                    _tree_screen->knowtree_save();
                    // }
                }
            }

            if(_result->is_lite())_result->to_fat();

            //    //        //            else {
            //    //        //                //                assert(_record->is_registered());
            //    //        //                _record->binder(generator);
            //    //        //                _record->activator(activator);
            //    //        //                //                _record->generate();    // why?
            //    //        //            }

            //    _result->record_binder(_record_binder);
            //    //    _result->activator(activator);

            assert(_result != _know_model_board->root_item());
            //    //    assert(_result->is_registered_to_record_controller_and_tabmanager());
            //    assert(_result->field("url") == _url.toString());   // maybe other url loaded !
            //    //    } // browser_pages

            //    //    }

            //    //    }

            if(_result->field("dir") == "")_result->field("dir", _result->field("id"));

            if(_result->field("file") == "")_result->field("file", "text.html");

            //    //    assert(_record);
            assert(_result->url<url_type>() == url_type()(_url));
            // assert(_result->fragment() == _url.fragment());
            return _result;
        }

        //        boost::intrusive_ptr<TreeItem> item_request_from_tree(const QUrl &_url);
        template<typename url_type = url_full>
        boost::intrusive_ptr<TreeItem> item_request_from_tree(const QUrl &_url)
        {
            boost::intrusive_ptr<TreeItem> re;
            auto it = item_request_from_tree_impl<url_type>(_url);

            re = item_registered_imperative_equip(it);

            return re;

        }
        //        boost::intrusive_ptr<TreeItem> item_request_from_tree_fragment(const QUrl &_url);

        boost::intrusive_ptr<TreeItem> item_registered_imperative_equip(boost::intrusive_ptr<TreeItem> item);

        RecordModel *source_model() {return _record_controller->source_model();}
        RecordView *view() {return _record_controller->view();}

        void addnew_to_end(void);
        void addnew_before(void);


        // Слот для добавления новой записи после выделенной строки
        // Slot to add a new record after the highlighted row
        void addnew_after(void);
        void on_edit_fieldcontext(void);

        // Обработка клика по удалению записи в контекстном меню и по кнопке на панели
        void delete_context(void);

        // Копирование отмеченных записей в буфер обмена с удалением
        // из таблицы конечных записей
        void cut(void);

        // Копирование отмеченных записей в буфер обмена
        void copy(void);


        // Insert records from the clipboard    // Вставка записей из буфера обмена
        void paste(void);
        // Слот, срабатывающий при вызове настроек
        void settings(void);

        // Перемещение записи вверх
        void move_up(void);


        // Перемещение записи вниз
        void move_dn(void);

        // Клик по пункту "Сортировка" в контекстном меню
        void on_sort_click(void);

        void on_print_click(void);

        TabWidget *tabmanager() {return this;}
    protected:
        void mouseDoubleClickEvent(QMouseEvent *event);
        void contextMenuEvent(QContextMenuEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void resizeEvent(QResizeEvent *);   // Q_DECL_OVERRIDE;

    public slots:
        void loadUrlInCurrentTab(const QUrl &url);

        //        void new_view_void() {newTab(false);}
        //BrowserView *new_dummy();

        WebView *newTab(boost::intrusive_ptr<TreeItem> item
                        , bool make_current
                        = true
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
        void moveTab(int fromIndex, int toIndex);

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
        // void moveTab(int fromIndex, int toIndex);
        void onTabsChanged();
        void fullScreenRequested(QWebEngineFullScreenRequest request);
        WebView *view_no_pinned();
    private:
        Browser             *_browser;
        // RecordScreen        *_record_screen;
        RecordController    *_record_controller;

        QAction             *_recentlyclosedtabsaction;
        QAction             *_newtabaction;
        QAction             *_closetabaction;
        QAction             *_nexttabaction;
        QAction             *_previoustabaction;

        QMenu               *_recentlyclosedtabsmenu;
        static const int    _recentlyclosedtabssize = 10;

        QList<QUrl>                 _recentlyclosedtabs;
        QList<WebActionMapper *>    _actions;

        // HistoryCompletionModel *_completionModel;// = new HistoryCompletionModel(this);
        QCompleter              *_lineeditcompleter;// = new QCompleter(_completionModel, this);
        QStackedWidget          *_lineedits;
        TabBar                  *_tabbar;
        QWebEngineProfile       *_profile;
        QWebEngineView          *_fullscreenview;
        FullScreenNotification  *_fullscreennotification;
    protected:

        //        active_record _active_record;
        //        sd::_interface<sd::meta_info<void *>, WebView *, Record *const> _active;
        //        //        sd::method<sd::meta_info<void *const>> _active_r;
        //        boost::intrusive_ptr<TreeItem> _shadow_branch_root;
        //        TableModel              *_shadow_source_model;
        //        std::shared_ptr<TableData>  _table_data;


        friend class Browser;
        friend class ToolbarSearch;
    };

    //    class PopupWindow :
    //    //        public TabWidget {  //
    //    //        public QWidget {
    //        public Browser {
    //        Q_OBJECT
    //    public:
    //        PopupWindow(TabWidget *tabmanager, QWebEngineProfile *const setProfile, QUrl const &url, TableController *_record_controller
    //                    , TableController *_page_controller
    //                    //                    , Browser *parent
    //                   );

    //        //        QWebEnginePage
    //        WebPage *page() const;

    //    private Q_SLOTS:
    //        void setUrl(const QUrl &url);

    //        void adjustGeometry(const QRect &newGeometry);

    //    private:
    //        QLineEdit   *_addressbar;
    //        WebView     *_view;

    //        struct ActiveRecordBinder {
    //            PopupWindow         *_the;
    //            TabWidget           *_tabmanager;
    //            QWebEngineProfile   *_profile;
    //            TableController     *_record_controller;
    //            TableController     *_page_controller;
    //            WebView             *_view;

    //            ~ActiveRecordBinder() {delete _view;}
    //            ActiveRecordBinder(
    //                TabWidget *tabmanager
    //                , PopupWindow *const the
    //                , QWebEngineProfile *profile
    //                , TableController *_record_controller
    //                , TableController *_page_controller
    //            ) :
    //                _the(the)
    //                , _tabmanager(tabmanager)
    //                , _profile(profile)
    //                , _record_controller(_record_controller)
    //                , _page_controller(_page_controller)
    //                , _view(nullptr)
    //            {}

    //            WebView *binder(boost::intrusive_ptr<Record> record)
    //            {
    //                //            assert(!record->unique_page());   // assert by record it self

    //                //            if(!record->unique_page())
    //                return  _view =
    //                            new WebView(record, _profile, _tabmanager, _the, _record_controller
    //                                        , _page_controller
    //                                       );
    //                //                _the->newTab(record, true, _the->_record_controller
    //                //                             , _the->_page_controller
    //                //                            );

    //                //            else
    //                //                return record->unique_page()->view();
    //            }

    //            WebView *activator(boost::intrusive_ptr<Record> record)
    //            {
    //                return record->unique_page()->active();
    //            }

    //        };

    //    };

}

QT_END_NAMESPACE

#endif // TABWIDGET_H



