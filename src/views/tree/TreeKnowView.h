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

class TreeKnowModel;
class TreeItem;

class TreeKnowView : public QTreeView {
    Q_OBJECT

public:
    explicit TreeKnowView(QWidget *parent = 0);
    virtual ~TreeKnowView();
    void setup_model(boost::intrusive_ptr<TreeItem> _item);
    std::shared_ptr<TreeKnowModel> source_model() {return _know_root;}
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
private:
    std::shared_ptr<TreeKnowModel>   _know_root;
};

#endif // __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__

