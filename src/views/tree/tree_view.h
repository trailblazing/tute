#ifndef __TREEKNOWVIEW_H__	// __KNOWTREEVIEW_H__
#define __TREEKNOWVIEW_H__	// __KNOWTREEVIEW_H__

#include <memory>

#include <wobjectdefs.h>
#include <QObject>

#include <QWidget>
#include <QTreeView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTapAndHoldGesture>
#include <QEvent>
#include <QGestureEvent>
#include <QMimeData>
#include <QStyledItemDelegate>

#include "models/tree/tree_item.h"	// because of moc
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_know_model.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"

extern enum QItemSelectionModel::SelectionFlag	current_tree_selection_mode;
extern enum QItemSelectionModel::SelectionFlag	current_tree_current_index_mode;

extern const char *record_view_multi_instance_name;


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
	HtmlDelegate(tv_t *_tree_view);
    protected:
	void	paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize	sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	tv_t	*_tree_view;
};

W_REGISTER_ARGTYPE(tv_t)

class tv_t : public QTreeView {
    W_OBJECT(tv_t)

    public:
	typedef std::function<bool (boost::intrusive_ptr<const TreeItem>, const QUrl &)> equal_url_t;
	typedef std::function<bool (boost::intrusive_ptr<const TreeItem>, boost::intrusive_ptr<const TreeItem>)> equal_t;
	typedef std::function<bool (boost::intrusive_ptr<const Linker>)> substitute_condition;	// , boost::intrusive_ptr<const Linker>
	typedef std::function<bool (boost::intrusive_ptr<const Linker>, boost::intrusive_ptr<const Linker>)> substitute_condition_double;	//
	typedef std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeIndex>, boost::intrusive_ptr<TreeItem>, const substitute_condition &)> paste_strategy;
	typedef std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeIndex>, QString, QString)> add_new;
	typedef std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeIndex>, QString, const add_new &)> add_new_delegate;
	typedef std::function<QModelIndex(tv_t *const, const QModelIndex &)> select_strategy;
	typedef std::function<QModelIndex(tv_t *const, const QModelIndex &, const int)> current_strategy;
	typedef boost::intrusive_ptr<TreeItem>(tkm_t::*delete_strategy)(boost::intrusive_ptr<Linker>);


	struct LocalLizeInitializer {
	    LocalLizeInitializer(tv_t *_tree_view);
		//        QModelIndexList index_list() {return _origin_index_list;}
		//        QModelIndex     index() {return _index;}
	    QItemSelection operator()();
	    private:
		tv_t        *_tree_view;
		//        QModelIndexList _index_list;
		//        QModelIndex     _index;
	};

	explicit tv_t(QString _name, ts_t *_tree_screen);	// QWidget *_parent = 0

	virtual ~tv_t();

	void sychronize();

	//    void source_model(boost::intrusive_ptr<TreeItem> _item);

	tkm_t	*source_model() const;
	tkm_t	*source_model();
	//    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
	boost::intrusive_ptr<TreeItem> current_item();

	//    QModelIndex current_index(void)const;
	QModelIndex current_index(void) const;


	QModelIndex select_as_current(boost::intrusive_ptr<TreeIndex> _tree_index
				     , select_strategy _select_strategy	= [](tv_t *const v, const QModelIndex &_i) -> QModelIndex {assert(v);assert(v->selectionModel());assert(_i.isValid());v->selectionModel()->select(_i, current_tree_selection_mode);return _i;}
				     , current_strategy _current_strategy = [](tv_t *const v, const QModelIndex &_i, const int dummy = 0) -> QModelIndex {(void)(dummy);assert(v);assert(v->selectionModel());assert(_i.isValid());v->selectionModel()->setCurrentIndex(_i, current_tree_current_index_mode);return _i;});

	//    QModelIndex select_as_current_item(boost::intrusive_ptr<TreeItem> _item
	//                                       , select_strategy _select_strategy = [](KnowView *const v, const QModelIndex &_i) ->QModelIndex{assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->select(_i, current_tree_selection_mode); return _i;}
	//                                               , current_strategy _current_strategy = [](KnowView *const v, const QModelIndex &_i, const int dummy = 0) ->QModelIndex{(void)(dummy); assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->setCurrentIndex(_i, current_tree_current_index_mode); return _i;}
	//                                      );

	void update_selected_indexes(void);

	tkm_t				*know_model_board() const;
	boost::intrusive_ptr<TreeItem>	session_root_auto();
    signals:
	void tapandhold_gesture_finished(const QPoint &p) W_SIGNAL(tapandhold_gesture_finished, (const QPoint &), p)	// ;
	//    void zoomChanged();

    public slots:
	// private slots:
	void	encrypt(void);
	void	decrypt(void);
	void	expand_selected(bool expand);
	void	tree_empty_controll(void);
	void	tree_crypt_control(void);


	boost::intrusive_ptr<TreeItem> new_child(boost::intrusive_ptr<TreeIndex> _modelindex
						, QString _name
						, const add_new &_branch_add_new_impl);

	boost::intrusive_ptr<TreeItem> new_item(boost::intrusive_ptr<TreeIndex>  _modelindex
					       , const add_new_delegate &_branch_add_new
					       , const add_new &_branch_add_new_impl);

	boost::intrusive_ptr<TreeItem> paste_children_from_children(boost::intrusive_ptr<TreeIndex> _parent_modelindex, boost::intrusive_ptr<TreeItem> _blank_header, const substitute_condition_double &_substitute_condition);

	void paste_children_from_clipboard(boost::intrusive_ptr<TreeIndex> _sibling_modelindex);

	boost::intrusive_ptr<TreeItem> paste_child(boost::intrusive_ptr<TreeIndex> _treeindex, boost::intrusive_ptr<TreeItem> _source_item, const substitute_condition &_substitute_condition);
//	boost::intrusive_ptr<TreeItem> paste_sibling(boost::intrusive_ptr<TreeIndex> _treeindex, boost::intrusive_ptr<TreeItem> _source_item, const substitute_condition &_substitute_condition);

	QList<boost::intrusive_ptr<TreeItem> > delete_permanent(const std::function<tkm_t *()> &_current_model
							       , QList<boost::intrusive_ptr<TreeItem> > _items
							       , delete_strategy _delete_strategy
							       , const QString &_mode = "cut"
							       , const bool _cut_branch_confirm = false);

	void				cut(bool _cut_branch_confirm = false);
	QModelIndexList			copy(void);	// const;
	void				edit(void);
	boost::intrusive_ptr<TreeItem>	merge(boost::intrusive_ptr<TreeLevel> _tree_merge);	// boost::intrusive_ptr<TreeIndex> tree_index, boost::intrusive_ptr<TreeItem> source);





	void	on_current_changed(const QModelIndex &current, const QModelIndex &previous);
	void	on_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);
	void	on_current_row_changed(const QModelIndex &current, const QModelIndex &previous);
	//    void on_pressed(const QModelIndex &_index);   // will make index invalid
	void	know_model_save(void);
	void	know_model_reload(void);
	void	session_root_manual(bool checked = true);
	// Действия при клике на ветку дерева
	void				cursor_step_into(const index_tree &_index);
	void				index_invoke(browser::WebView *view, const index_tree &_index);
	boost::intrusive_ptr<TreeItem>	cursor_follow_up(boost::intrusive_ptr<TreeItem> _new_session_root_item);
	boost::intrusive_ptr<TreeItem>	cursor_follow_up(void);
	boost::intrusive_ptr<TreeItem>	cursor_follow_root(void);
	boost::intrusive_ptr<TreeItem>	intercept(boost::intrusive_ptr<TreeItem> result);

	void	synchronized(bool _synchronized);
	bool	synchronized();



	QModelIndex previous_index() const;
	//    void zoom(double scale_);
    protected:
	void	resizeEvent(QResizeEvent *e) override;
	bool	event(QEvent *event) override;
	bool	gestureEvent(QGestureEvent *event);
	void	tapandhold_gesture_triggered(QTapAndHoldGesture *gesture);

	void	dragEnterEvent(QDragEnterEvent *event) override;
	void	dragMoveEvent(QDragMoveEvent *event) override;
	void	dropEvent(QDropEvent *event) override;

	void mousePressEvent(QMouseEvent *event) override;	// Q_DECL_OVERRIDE;

	template <class X> bool is_dragable(X *event){
		// Проверяется, содержит ли объект переноса данные нужного формата
	    const QMimeData *mime_data = event->mimeData();
	    if(mime_data == nullptr)return false;
	    if(! (mime_data->hasFormat("mytetra/records")))return false;
	    QObject *source_object = qobject_cast<QObject *>(event->source());
	    if(source_object->objectName() == record_view_multi_instance_name)	// "recordTableView"
			return true;
	    else return false;
	}
	//    bool is_owner();
	void setModel(QAbstractItemModel *model) override;
    private:
	ts_t		*_tree_screen;
	tkm_t		*_know_model_board;	// for tree screen
	tkm_t		*_know_root;

	// create custom delegate
	HtmlDelegate	*_delegate;
	QModelIndex	_previous_index;
	double		_previous_scale;


	friend class AdjustingScrollArea;
	friend class ts_t;
};



#endif	// __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__
