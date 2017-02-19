#ifndef __TREEKNOWVIEW_H__ // __KNOWTREEVIEW_H__
#define __TREEKNOWVIEW_H__ // __KNOWTREEVIEW_H__

#include <memory>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QGestureEvent>
#include <QMimeData>
#include <QStyledItemDelegate>
#include <QTapAndHoldGesture>
#include <QTreeView>
#include <QWidget>

#include "libraries/global_parameters.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_item.h" // because of moc
#include "models/tree/tree_know_model.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

extern enum QItemSelectionModel::SelectionFlag current_tree_selection_mode;
extern enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode;

extern const char* record_view_multi_instance_name;

class ts_t;
class tkm_t;
class TreeItem;
class tv_t;
class ts_t;
class tm_t;
class RecordModel;
struct Linker;
struct RecordIndex;
class AdjustingScrollArea;

struct index_tree;
struct TreeLevel;

class HtmlDelegate;

// http://stackoverflow.com/questions/1956542/how-to-make-item-view-render-rich-html-text-in-qt
class HtmlDelegate : public QStyledItemDelegate {
    //    W_OBJECT(HtmlDelegate)
public:
    HtmlDelegate(tv_t* _tree_view);

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
    tv_t* _tree_view;
};

#if QT_VERSION == 0x050600
W_REGISTER_ARGTYPE(tv_t)
#endif

class tv_t : public QTreeView {
#if QT_VERSION == 0x050600
    W_OBJECT(tv_t)
#else
    Q_OBJECT
#endif

public:
    typedef std::function<bool(boost::intrusive_ptr<const TreeItem>, const QUrl&)> equal_url_t;
    typedef std::function<bool(boost::intrusive_ptr<const TreeItem>, boost::intrusive_ptr<const TreeItem>)> equal_t;
    typedef std::function<bool(boost::intrusive_ptr<const Linker>)> substitute_condition; // , boost::intrusive_ptr<const Linker>
    typedef std::function<bool(boost::intrusive_ptr<const Linker>, boost::intrusive_ptr<const Linker>)> substitute_condition_double; //
    typedef std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeIndex>, boost::intrusive_ptr<TreeItem>, const substitute_condition&)> paste_strategy;
    typedef std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeIndex>, QString, QString)> add_new;
    typedef std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeIndex>, QString, const add_new&)> add_new_delegate;
    typedef std::function<QModelIndex(tv_t* const, const QModelIndex&)> select_strategy;
    typedef std::function<QModelIndex(tv_t* const, const QModelIndex&, const int)> current_strategy;
    typedef boost::intrusive_ptr<TreeItem> (tkm_t::*delete_strategy)(boost::intrusive_ptr<Linker>);

    struct LocalLizeInitializer {
        LocalLizeInitializer(tv_t* _tree_view);
        //        QModelIndexList index_list() {return _origin_index_list;}
        //        QModelIndex     index() {return _index;}
        QItemSelection operator()();

    private:
        tv_t* _tree_view;
        //        QModelIndexList _index_list;
        //        QModelIndex     _index;
    };

    explicit tv_t(QString _name, ts_t* _tree_screen); // QWidget *_parent = 0

    virtual ~tv_t();

    //	void sychronize();

    //    void source_model(boost::intrusive_ptr<TreeItem> _item);

    tkm_t* source_model() const;
    tkm_t* source_model();
    //    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
    boost::intrusive_ptr<TreeItem> current_item();

    //    QModelIndex current_index(void)const;
    QModelIndex current_index(void) const;

    QModelIndex select_as_current(boost::intrusive_ptr<TreeIndex> _tree_index, select_strategy _select_strategy = [](tv_t* const v, const QModelIndex& _i) -> QModelIndex {assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->select(_i, current_tree_selection_mode); return _i; }, current_strategy _current_strategy = [](tv_t* const v, const QModelIndex& _i, const int dummy = 0) -> QModelIndex {(void)(dummy); assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->setCurrentIndex(_i, current_tree_current_index_mode); return _i; });

    //    QModelIndex select_as_current_item(boost::intrusive_ptr<TreeItem> _item
    //                                       , select_strategy _select_strategy = [](KnowView *const v, const QModelIndex &_i) ->QModelIndex{assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->select(_i, current_tree_selection_mode); return _i;}
    //                                               , current_strategy _current_strategy = [](KnowView *const v, const QModelIndex &_i, const int dummy = 0) ->QModelIndex{(void)(dummy); assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->setCurrentIndex(_i, current_tree_current_index_mode); return _i;}
    //                                      );

    void update_selected_indexes(void);

    tkm_t* know_model_board() const;
    boost::intrusive_ptr<TreeItem> session_root_auto();
signals:
    void tapandhold_gesture_finished(const QPoint& p)
#if QT_VERSION == 0x050600
        W_SIGNAL(tapandhold_gesture_finished, (const QPoint&), p) //
#else
        ;
#endif
        //    void zoomChanged();

        public slots :
        // private slots:
        void encrypt(void);
    void decrypt(void);
    void expand_selected(bool expand);
    boost::intrusive_ptr<TreeItem> tree_empty_controll(void);
    void tree_crypt_control(void);

    boost::intrusive_ptr<TreeItem> new_child(boost::intrusive_ptr<TreeIndex> _modelindex, QString _name, const add_new& _branch_add_new_impl);

    boost::intrusive_ptr<TreeItem> new_item(boost::intrusive_ptr<TreeIndex> _modelindex, const add_new_delegate& _branch_add_new, const add_new& _branch_add_new_impl);

    QList<boost::intrusive_ptr<TreeItem> > move_children(boost::intrusive_ptr<TreeIndex> _parent_modelindex, boost::intrusive_ptr<TreeItem> _blank_header, const substitute_condition_double& _substitute_condition);

    void paste_clipboard(boost::intrusive_ptr<TreeIndex> _sibling_modelindex);

    boost::intrusive_ptr<TreeItem> move(boost::intrusive_ptr<TreeIndex> _treeindex, boost::intrusive_ptr<TreeItem> _source_item, const substitute_condition& _substitute_condition, bool save_immediately = false);
    //	boost::intrusive_ptr<TreeItem> paste_sibling(boost::intrusive_ptr<TreeIndex> _treeindex, boost::intrusive_ptr<TreeItem> _source_item, const substitute_condition &_substitute_condition);

    QList<boost::intrusive_ptr<TreeItem> > delete_permanent(const std::function<tkm_t*()>& _current_model, QList<boost::intrusive_ptr<TreeItem> > _items, delete_strategy _delete_strategy, const QString& _mode = "cut", const bool _cut_branch_confirm = false);

    void cut(bool _cut_branch_confirm = false);
    QModelIndexList copy(void); // const;
    void modify(void);
    boost::intrusive_ptr<TreeItem> merge(boost::intrusive_ptr<TreeLevel> _tree_merge); // boost::intrusive_ptr<TreeIndex> tree_index, boost::intrusive_ptr<TreeItem> source);

    void on_current_changed(const QModelIndex& current, const QModelIndex& previous);
    void on_selection_changed(const QItemSelection& selected, const QItemSelection& deselected);
    void on_current_row_changed(const QModelIndex& current, const QModelIndex& previous);
    //    void on_pressed(const QModelIndex &_index);   // will make index invalid
    void know_model_save(void);
    void know_model_reload(void);
    void session_root_manual(bool checked = true);
#ifdef USE_INDEX_FOCUS
    // Действия при клике на ветку дерева
    void cursor_focus(const index_tree& _index);
#endif
    browser::WebView* index_invoke(boost::intrusive_ptr<TreeIndex> _tree_index); // browser::WebView *view, const index_tree &_index
    std::pair<boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem> > cursor_focus(boost::intrusive_ptr<TreeItem> to_be_contained = nullptr);
    //	std::pair<boost::intrusive_ptr<TreeItem>, boost::intrusive_ptr<TreeItem> > cursor_follow_up(void);
    //	boost::intrusive_ptr<TreeItem>	cursor_follow_up(void);
    boost::intrusive_ptr<TreeItem> intercept(boost::intrusive_ptr<TreeItem> result);

    void synchronized(bool _synchronized);
    bool synchronized();

    QModelIndex previous_index() const;
    //    void zoom(double scale_);
protected:
    void resizeEvent(QResizeEvent* e) override;
    bool event(QEvent* event) override;
    bool gestureEvent(QGestureEvent* event);
    void tapandhold_gesture_triggered(QTapAndHoldGesture* gesture);

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override; // Q_DECL_OVERRIDE;

    template <class X>
    bool is_dragable(X* event)
    {
        // Проверяется, содержит ли объект переноса данные нужного формата
        const QMimeData* mime_data = event->mimeData();
        if (mime_data == nullptr)
            return false;
        if (!(mime_data->hasFormat(QString(gl_para::_program_instance_name) + "/records")))
            return false;
        QObject* source_object = qobject_cast<QObject*>(event->source());
        if (source_object->objectName() == record_view_multi_instance_name) // "recordTableView"
            return true;
        else
            return false;
    }
    //    bool is_owner();
    void setModel(QAbstractItemModel* model) override;
    virtual void drawBranches(QPainter* painter, const QRect& rect, const QModelIndex& index) const override;

private:
    ts_t* _tree_screen;
    tkm_t* _know_model_board; // for tree screen
    tkm_t* _know_root;

    // create custom delegate
    HtmlDelegate* _delegate;
    QModelIndex _previous_index;
    double _previous_scale;

    friend class AdjustingScrollArea;
    friend class ts_t;
};

#endif // __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__

// *  Contact List Widget
// *  Copyright (C) 2011  Martin Klapetek <martin.klapetek@gmail.com>
// *
// *  This library is free software; you can redistribute it and/or
// *  modify it under the terms of the GNU Lesser General Public
// *  License as published by the Free Software Foundation; either
// *  version 2.1 of the License, or (at your option) any later version.
// *
// *  This library is distributed in the hope that it will be useful,
// *  but WITHOUT ANY WARRANTY; without even the implied warranty of
// *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// *  Lesser General Public License for more details.
// *
// *  You should have received a copy of the GNU Lesser General Public
// *  License along with this library; if not, write to the Free Software
// *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

//	#ifndef CONTACT_LIST_WIDGET_H
//	#define CONTACT_LIST_WIDGET_H

//	#include <QTreeView>
////	#include <TelepathyQt/Types>
////	#include <TelepathyQt/Connection>

// class AccountsModel;
// class ContactModelItem;
// class ContactListWidgetPrivate;

// namespace Tp {
//    class PendingOperation;
// }

// class ContactListWidget : public QTreeView {
//    Q_OBJECT Q_DECLARE_PRIVATE(ContactListWidget)
//    Q_DISABLE_COPY(ContactListWidget)

//    public:
//	explicit ContactListWidget(QWidget *parent);
//	virtual ~ContactListWidget();

//	AccountsModel	*accountsModel();
//	void		setAccountManager(const Tp::AccountManagerPtr &accountManager);

//    public Q_SLOTS:
//	void	toggleOfflineContacts(bool show);
//	void	toggleGroups(bool show);
//	void	toggleSortByPresence(bool sort);

//	void	setFilterString(const QString &string);
//	void	showSettingsKCM();

//	void	onSwitchToFullView();
//	void	onSwitchToCompactView();

//	void	onShowAllContacts();
//	void	onShowUnblockedContacts();
//	void	onShowBlockedContacts();

//    private Q_SLOTS:
//	void	onNewAccountAdded(const Tp::AccountPtr &account);
//	void	onContactListClicked(const QModelIndex &index);
//	void	onContactListDoubleClicked(const QModelIndex &index);

//	void	onNewGroupModelItemsInserted(const QModelIndex &index, int start, int end);
//	void	addOverlayButtons();

//	void	startTextChannel(ContactModelItem *contactItem);
//	void	startFileTransferChannel(ContactModelItem *contactItem);
//	void	startAudioChannel(ContactModelItem *contactItem);
//	void	startVideoChannel(ContactModelItem *contactItem);
//	void	startDesktopSharing(ContactModelItem *contactItem);

// Q_SIGNALS:
//	void	enableOverlays(bool);
//	void	accountManagerReady(Tp::PendingOperation *op);
//	void	genericOperationFinished(Tp::PendingOperation *op);

//    protected:
//	void		setDropIndicatorRect(const QRect &rect);
//	virtual bool	event(QEvent *event);
//	virtual void	mousePressEvent(QMouseEvent *event);
//	virtual void	mouseMoveEvent(QMouseEvent *event);
//	virtual void	paintEvent(QPaintEvent *event);
//	virtual void	dropEvent(QDropEvent *event);
//	virtual void	dragEnterEvent(QDragEnterEvent *event);
//	virtual void	dragMoveEvent(QDragMoveEvent *event);
//	virtual void	dragLeaveEvent(QDragLeaveEvent *event);
//	virtual void	drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;

//    private:
//	void requestFileTransferChannels(const Tp::AccountPtr &account, const Tp::ContactPtr &contact, const QStringList &filenames, const QDateTime &userActionTime);

//	friend class ContextMenu;
//	ContactListWidgetPrivate *const d_ptr;
// };

//	#endif	// CONTACT_LIST_WIDGET_H
