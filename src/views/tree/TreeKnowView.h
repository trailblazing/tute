#ifndef __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__
#define __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__

#include <QWidget>
#include <QTreeView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QTapAndHoldGesture>
#include <QEvent>
#include <QGestureEvent>


class TreeKnowView : public QTreeView {
    Q_OBJECT

public:
    explicit TreeKnowView(QWidget *parent = 0);
    virtual ~TreeKnowView();

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

};

#endif // __TREEKNOWVIEW_H__    // __KNOWTREEVIEW_H__

