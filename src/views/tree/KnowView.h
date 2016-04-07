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


#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>


extern enum QItemSelectionModel::SelectionFlag current_tree_selection_mode;
extern enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode;

extern const char *record_view_multi_instance_name;

class KnowModel;
class TreeItem;

class KnowView : public QTreeView {
    Q_OBJECT

public:
    explicit KnowView(QString _name, QWidget *_parent = 0);
    virtual ~KnowView();

    void sychronize();

    void source_model(boost::intrusive_ptr<TreeItem> _item);

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
    QModelIndexList index_localize(const QModelIndexList _origin_index_list);
    bool is_index_localized(const QModelIndexList _origin_index_list)const;

signals:
    void tapAndHoldGestureFinished(const QPoint &);

public slots:

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
        const QMimeData *mimeData = event->mimeData();

        if(mimeData == nullptr)
            return false;

        if(!(mimeData->hasFormat("mytetra/records")))
            return false;

        QObject *sourceObject = qobject_cast<QObject *>(event->source());

        if(sourceObject->objectName() == record_view_multi_instance_name) // "recordTableView"
            return true;
        else
            return false;
    }

    bool is_owner();
    void setModel(KnowModel *model);
private:
    KnowModel  *_know_root;
};

#endif // __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__

