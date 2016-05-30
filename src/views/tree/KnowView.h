#ifndef __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__
#define __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__

#include <memory>
#include <QWidget>
#include <QTreeView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTapAndHoldGesture>
#include <QEvent>
#include <QGestureEvent>
#include <QMimeData>
#include <QStyledItemDelegate>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include "models/record_table/linker.hxx"
#include "models/tree/TreeItem.h"
#include "models/tree/treeindex.hxx"
#include "models/tree/KnowModel.h"
#include "models/record_table/RecordModel.h"

extern enum QItemSelectionModel::SelectionFlag current_tree_selection_mode;
extern enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode;

extern const char *record_view_multi_instance_name;


class TreeScreen;
class KnowModel;
class TreeItem;
class KnowView;
class TreeScreen;
class TreeModel;
class RecordModel;
struct Linker;
struct RecordIndex;
class AdjustingScrollArea;
// http://stackoverflow.com/questions/1956542/how-to-make-item-view-render-rich-html-text-in-qt
class HtmlDelegate : public QStyledItemDelegate {
public:
    HtmlDelegate(KnowView *_tree_view);
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    KnowView *_tree_view;
};



class KnowView : public QTreeView {
    Q_OBJECT

public:
    typedef std::function<bool(boost::intrusive_ptr<const TreeItem>, const QUrl &)> equal_url_t;
    typedef std::function<bool(boost::intrusive_ptr<const TreeItem>, boost::intrusive_ptr<const TreeItem>)> equal_t;
    typedef std::function<bool(boost::intrusive_ptr<const Linker>)> substitute_condition; // , boost::intrusive_ptr<const Linker>
    typedef std::function<bool(boost::intrusive_ptr<const Linker>, boost::intrusive_ptr<const Linker>)> substitute_condition_double; //
    typedef std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeIndex>, boost::intrusive_ptr<TreeItem>, const substitute_condition &)> paste_strategy;
    typedef std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeIndex>, QString, QString)> add_new;
    typedef std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeIndex>, QString, const add_new &)> add_new_delegate;
    typedef std::function<QModelIndex(KnowView *const, const QModelIndex &)> select_strategy;
    typedef std::function<QModelIndex(KnowView *const, const QModelIndex &, const int)> current_strategy;
    typedef boost::intrusive_ptr<TreeItem>(KnowModel::*delete_strategy)(boost::intrusive_ptr<Linker>);


    struct LocalLizeInitializer {
        LocalLizeInitializer(KnowView *_tree_view);
        //        QModelIndexList index_list() {return _origin_index_list;}
        //        QModelIndex     index() {return _index;}
        QItemSelection  operator()();
    private:
        KnowView        *_tree_view;
        //        QModelIndexList _index_list;
        //        QModelIndex     _index;
    };

    explicit KnowView(QString _name, TreeScreen *_tree_screen); // QWidget *_parent = 0

    virtual ~KnowView();

    void sychronize();

    //    void source_model(boost::intrusive_ptr<TreeItem> _item);

    KnowModel *source_model()const;
    KnowModel *source_model();
    //    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
    boost::intrusive_ptr<TreeItem> current_item();

    //    QModelIndex current_index(void)const;
    QModelIndex current_index(void) const;


    QModelIndex select_as_current(boost::intrusive_ptr<TreeIndex> _modelindex
                                  , select_strategy _select_strategy
                                  = [](KnowView *const v, const QModelIndex &_i) ->QModelIndex{assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->select(_i, current_tree_selection_mode); return _i;}
                                    , current_strategy _current_strategy
                                  = [](KnowView *const v, const QModelIndex &_i, const int dummy = 0) ->QModelIndex{(void)(dummy); assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->setCurrentIndex(_i, current_tree_current_index_mode); return _i;}
                                 );

    //    QModelIndex select_as_current_item(boost::intrusive_ptr<TreeItem> _item
    //                                       , select_strategy _select_strategy = [](KnowView *const v, const QModelIndex &_i) ->QModelIndex{assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->select(_i, current_tree_selection_mode); return _i;}
    //                                               , current_strategy _current_strategy = [](KnowView *const v, const QModelIndex &_i, const int dummy = 0) ->QModelIndex{(void)(dummy); assert(v); assert(v->selectionModel()); assert(_i.isValid()); v->selectionModel()->setCurrentIndex(_i, current_tree_current_index_mode); return _i;}
    //                                      );

    void update_selected_indexes(void);

    KnowModel *know_model_board()const;
    boost::intrusive_ptr<TreeItem> session_root_auto();
signals:
    void tapAndHoldGestureFinished(const QPoint &);
    //    void zoomChanged();

public slots:
    //private slots:
    void view_encrypt(void);
    void view_decrypt(void);
    void view_expand_selected(bool expand);
    void tree_empty_controll(void);
    void tree_crypt_control(void);


    boost::intrusive_ptr<TreeItem> view_add_new(boost::intrusive_ptr<TreeIndex> _modelindex
                                                , QString _name
                                                , const add_new &_branch_add_new_impl);

    boost::intrusive_ptr<TreeItem> view_insert_new(boost::intrusive_ptr<TreeIndex>  _modelindex
                                                   , const add_new_delegate &_branch_add_new
                                                   , const add_new &_branch_add_new_impl
                                                  );

    boost::intrusive_ptr<TreeItem> view_paste_children_from_children(boost::intrusive_ptr<TreeIndex> _parent_modelindex, boost::intrusive_ptr<TreeItem> _blank_header, const substitute_condition_double &_substitute_condition);

    void view_paste_children_from_clipboard(boost::intrusive_ptr<TreeIndex> _sibling_modelindex);

    boost::intrusive_ptr<TreeItem> view_paste_child(boost::intrusive_ptr<TreeIndex> _treeindex, boost::intrusive_ptr<TreeItem> _source_item, const substitute_condition &_substitute_condition);


    QList<boost::intrusive_ptr<TreeItem>> view_delete_permanent(const std::function<KnowModel *()>      &_current_model
                                                              , QList<boost::intrusive_ptr<TreeItem>> _items
                                                              , delete_strategy                       _delete_strategy
                                                              , const QString                         &_mode
                                                              = "cut"
                                                                , const bool                          _cut_branch_confirm
                                                              = false
                                                             );

    void view_cut(bool _cut_branch_confirm = false);
    QModelIndexList view_copy(void);   // const;
    void view_edit(void);
    boost::intrusive_ptr<TreeItem> view_merge(boost::intrusive_ptr<TreeIndex> modelindex, boost::intrusive_ptr<TreeItem> source);





    void on_current_changed(const QModelIndex &current, const QModelIndex &previous);
    void on_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);
    void on_current_row_changed(const QModelIndex &current, const QModelIndex &previous);
    //    void on_pressed(const QModelIndex &_index);   // will make index invalid
    void know_model_save(void);
    void know_model_reload(void);
    void session_root_manual(bool checked = true);
    // Действия при клике на ветку дерева
    void cursor_step_into(const QModelIndex &_index);
    void index_invoke(const QModelIndex &_index);
    boost::intrusive_ptr<TreeItem> cursor_follow_up(boost::intrusive_ptr<TreeItem> _new_session_root_item);
    boost::intrusive_ptr<TreeItem> cursor_follow_up(void);
    boost::intrusive_ptr<TreeItem> cursor_follow_root(void);
    boost::intrusive_ptr<TreeItem> intercept(boost::intrusive_ptr<TreeIndex> modelindex);

    void synchronized(bool _synchronized);
    bool synchronized();



    QModelIndex previous_index()const;
    //    void zoom(double scale_);
protected:
    void resizeEvent(QResizeEvent *e);
    bool event(QEvent *event)override;
    bool gestureEvent(QGestureEvent *event);
    void tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture);

    void dragEnterEvent(QDragEnterEvent *event)override;
    void dragMoveEvent(QDragMoveEvent *event)override;
    void dropEvent(QDropEvent *event)override;

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    template <class X> bool isDragableData(X *event)
    {
        // Проверяется, содержит ли объект переноса данные нужного формата
        const QMimeData *mime_data = event->mimeData();

        if(mime_data == nullptr)
            return false;

        if(!(mime_data->hasFormat("mytetra/records")))
            return false;

        QObject *sourceObject = qobject_cast<QObject *>(event->source());

        if(sourceObject->objectName() == record_view_multi_instance_name) // "recordTableView"
            return true;
        else
            return false;
    }

    //    bool is_owner();
    void setModel(QAbstractItemModel *model)override;
private:
    TreeScreen      *_tree_screen;
    KnowModel       *_know_model_board;      // for tree screen
    KnowModel       *_know_root;

    // create custom delegate
    HtmlDelegate    *_delegate;
    QModelIndex     _previous_index;
    double          _previous_scale;
    friend class AdjustingScrollArea;
    friend class TreeScreen;
};

#endif // __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__
