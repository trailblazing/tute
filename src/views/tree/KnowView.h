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
#include "models/tree/KnowModel.h"

extern enum QItemSelectionModel::SelectionFlag current_tree_selection_mode;
extern enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode;

extern const char *record_view_multi_instance_name;

class KnowModel;
class TreeItem;
class KnowView;
class TreeScreen;
class TreeModel;

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

    typedef std::function<bool(boost::intrusive_ptr<const TreeItem::Linker>, boost::intrusive_ptr<const TreeItem::Linker>)> substitute_condition;
    typedef std::function<boost::intrusive_ptr<TreeItem> (TreeModel::ModelIndex, boost::intrusive_ptr<TreeItem>, const substitute_condition &)> paste_strategy;
    typedef std::function<boost::intrusive_ptr<TreeItem> (TreeModel::ModelIndex, QString, QString)> add_new;
    typedef std::function<boost::intrusive_ptr<TreeItem> (TreeModel::ModelIndex, QString, const add_new &)> add_new_delegate;


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

    explicit KnowView(QString _name
                      , TreeScreen *_parent // QWidget *_parent = 0
                     );
    virtual ~KnowView();

    void sychronize();

    //    void source_model(boost::intrusive_ptr<TreeItem> _item);

    KnowModel *source_model()const; // {return _know_root;}
    KnowModel *source_model();
    //    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
    boost::intrusive_ptr<TreeItem> current_item();

    //    QModelIndex current_index(void)const;
    QModelIndex current_index(void);


    //    QModelIndex view_index_last(void)const;
    //    QModelIndex selection_to_pos(int _index);
    QModelIndex select_as_current(const QModelIndex &_index, std::function<QModelIndex(KnowView *, const QModelIndex &)> _strategy = [](KnowView *v, const QModelIndex &_i) ->QModelIndex{v->selectionModel()->select(_i, current_tree_selection_mode); return _i;});
    QModelIndex select_as_current(boost::intrusive_ptr<TreeItem> _item, std::function<QModelIndex(KnowView *, const QModelIndex &)> _strategy = [](KnowView *v, const QModelIndex &_i) ->QModelIndex{v->selectionModel()->select(_i, current_tree_selection_mode); return _i;});
    void update_selected_indexes(void);
    //    QModelIndexList index_localize(const QModelIndexList _origin_index_list);
    //    bool is_index_localized(const QModelIndexList _origin_index_list)const;

    //    template<bool insert_sibling_branch = true>
    boost::intrusive_ptr<TreeItem> view_add_new(TreeModel::ModelIndex _modelindex   // std::function<KnowModel *()> _current_model, QModelIndex _current_index
                                                , QString _name
                                                , const std::function<boost::intrusive_ptr<TreeItem> (TreeModel::ModelIndex, QString, QString)> &_branch_add_new_impl);
    KnowModel *know_model_board()const;
    boost::intrusive_ptr<TreeItem> session_root_item();
signals:
    void tapAndHoldGestureFinished(const QPoint &);

public slots:
    //private slots:
    void view_encrypt(void);
    void view_decrypt(void);
    void view_expand_selected(bool expand);
    void tree_empty_controll(void);
    void tree_crypt_control(void);


    //    template<bool insert_sibling_branch = true>
    boost::intrusive_ptr<TreeItem> view_insert_new(
        TreeModel::ModelIndex _modelindex  // std::function<KnowModel *()> _current_model, QModelIndex _current_index
        , const add_new_delegate &_branch_add_new
        , const add_new &_branch_add_new_impl
    );

    boost::intrusive_ptr<TreeItem> view_paste_children_from_children(TreeModel::ModelIndex _parent_modelindex, boost::intrusive_ptr<TreeItem> _blank_header, const substitute_condition &_substitute_condition);

    void view_paste_children_from_clipboard(TreeModel::ModelIndex _sibling_modelindex);
    boost::intrusive_ptr<TreeItem> view_paste_child(TreeModel::ModelIndex _modelindex, boost::intrusive_ptr<TreeItem> _source_item, const substitute_condition &_substitute_condition);

    QList<boost::intrusive_ptr<ItemsFlat::Linker> > view_delete_permanent(
        const std::function<KnowModel *()>      &_current_model
        , QList<boost::intrusive_ptr<TreeItem>> _items
        , const QString                         &_mode
        , const bool                            _cut_branch_confirm = false
    );

    void view_cut(bool _cut_branch_confirm = false);
    QModelIndexList view_copy(void);   // const;
    void view_edit(void);
    boost::intrusive_ptr<TreeItem> view_merge_to_left(TreeModel::ModelIndex modelindex    // const std::function<KnowModel*()> &_current_model, boost::intrusive_ptr<TreeItem> target
                                                      , boost::intrusive_ptr<TreeItem> source);


    boost::intrusive_ptr<TreeItem> view_paste_strategy(TreeModel::ModelIndex                                        _modelindex
                                                       , boost::intrusive_ptr<TreeItem>                             _result
                                                       , const substitute_condition                                 &_substitute_condition
                                                       , const paste_strategy                                       &_view_paste_strategy_impl
                                                       , const bool                                                 _item_is_brand_new
                                                       , const QUrl                                                 &_find_url
                                                       , const std::function<bool(boost::intrusive_ptr<TreeItem>)>  &_check_url
                                                      );
    void on_current_changed(const QModelIndex &current, const QModelIndex &previous);
    void on_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);
    void on_current_row_changed(const QModelIndex &current, const QModelIndex &previous);
    void on_pressed(const QModelIndex &_index);
    void know_model_save(void);
    void know_model_reload(void);
    void session_root_id(bool checked = true);
    // Действия при клике на ветку дерева
    void cursor_step_into(const QModelIndex &_index);
    void index_invoke(const QModelIndex &_index);
    boost::intrusive_ptr<TreeItem> cursor_follow_up(boost::intrusive_ptr<TreeItem> _new_session_root_item);
    boost::intrusive_ptr<TreeItem> cursor_follow_up_one_level(void);
    boost::intrusive_ptr<TreeItem> cursor_follow_up_to_absolute_root(void);
    boost::intrusive_ptr<TreeItem> intercept(TreeModel::ModelIndex modelindex);

    void synchronized(bool _synchronized);
    bool synchronized();




    boost::intrusive_ptr<TreeItem> item_register(const QUrl &_find_url
                                                 , const KnowView::paste_strategy &_view_paste_strategy
                                                 , equal_url_t _equal = [](boost::intrusive_ptr<const TreeItem> it, const QUrl &_url)->bool {return it->field("url") == _url.toString();}
                                                );

    boost::intrusive_ptr<TreeItem> item_bind(const QUrl &_find_url
                                             , const KnowView::paste_strategy &_view_paste_strategy
                                             , equal_url_t _equal = [](boost::intrusive_ptr<const TreeItem> it, const QUrl &_url)->bool {return it->field("url") == _url.toString();}
                                            );

    boost::intrusive_ptr<TreeItem> item_register(boost::intrusive_ptr<TreeItem> target
                                                 , const KnowView::paste_strategy &_view_paste_strategy
                                                 , equal_t _equal = [](boost::intrusive_ptr<const TreeItem> it, boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() == target->id();}
                                                );

    boost::intrusive_ptr<TreeItem> item_bind(boost::intrusive_ptr<TreeItem> target
                                             , const KnowView::paste_strategy &_view_paste_strategy
                                             , equal_t _equal = [](boost::intrusive_ptr<const TreeItem> it, boost::intrusive_ptr<const TreeItem> target)->bool {return it->id() == target->id();}
                                            );


protected:

    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture);

    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

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
    void setModel(KnowModel *model);
private:
    KnowModel       *_know_model_board;      // for tree screen
    KnowModel       *_know_root;

    // create custom delegate
    HtmlDelegate    *_delegate;
};

#endif // __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__

