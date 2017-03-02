#ifndef __ATTACHTABLEVIEW_H__
#define __ATTACHTABLEVIEW_H__

#include <QGestureEvent>
#include <QMenu>
#include <QObject>
#include <QTableView>
#include <QTapAndHoldGesture>
#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class AttachTableController;

// Отображение таблицы с файлами (только таблица)

class AttachTableView : public QTableView {
#if QT_VERSION == 0x050600
    W_OBJECT(AttachTableView)
#else
    Q_OBJECT
#endif

public:
    AttachTableView(QWidget* parent = 0);
    virtual ~AttachTableView();

    void init(void);

    void setController(AttachTableController* pController);

    int getFirstSelectionPos(void);

signals:

    void tapAndHoldGestureFinished(const QPoint& p)
#if QT_VERSION == 0x050600
        W_SIGNAL(tapAndHoldGestureFinished, (const QPoint&), p) //
#else
        ;
#endif

        protected slots :

        void onCustomContextMenuRequested(const QPoint& pos);

protected:
    void setupSignals(void);
    void assemblyContextMenu(void);

    bool event(QEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    bool gestureEvent(QGestureEvent* event);
    void tapAndHoldGestureTriggered(QTapAndHoldGesture* gesture);

    AttachTableController* controller;

    QMenu* contextMenu;
};

#endif // __ATTACHTABLEVIEW_H__
