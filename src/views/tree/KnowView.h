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

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>


extern enum QItemSelectionModel::SelectionFlag current_tree_selection_mode;


class KnowModel;
class TreeItem;

class KnowView : public QTreeView {
    Q_OBJECT

public:
    explicit KnowView(QWidget *parent = 0);
    virtual ~KnowView();

    void sychronize();

    void source_model(boost::intrusive_ptr<TreeItem> _item);

    KnowModel *source_model()const {return _know_root;}
    //    void setModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;

    QModelIndex index_current(void);
    //    QModelIndex view_index_last(void)const;
    QModelIndex selection_to_pos(int to_pos);
    QModelIndex selection_to_pos(boost::intrusive_ptr<TreeItem> _item);

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

    template <class X> bool isDragableData(X *event);
    bool is_owner();
    void setModel(KnowModel *model);
private:
    KnowModel  *_know_root;
};

#endif // __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__

