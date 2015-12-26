#ifndef RECORDLISTSCREEN_H_
#define RECORDLISTSCREEN_H_

#include <QWidget>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStringList>
#include <QItemSelection>
#include <QMenu>
#include <QTapAndHoldGesture>
#include <QEvent>
#include <QGestureEvent>

class ClipboardRecords;
class TableController;
class TableScreen;

class TableView : public QTableView {
    Q_OBJECT

public:
    TableView(QString screen_name, TableScreen *recordtablescreen, TableController *controller);
    virtual ~TableView();

    void setController(TableController *pController);

    void init(void);

    void restoreHeaderState(void);
    void restoreColumnWidth(void);

    int getFirstSelectionPos(void);
    QString getFirstSelectionId(void);

    void setSelectionToPos(int pos);

    QModelIndex getFirstSelectionProxyIndex(void);
    QModelIndex getFirstSelectionSourceIndex(void);

    bool isSelectedSetToTop(void);
    bool isSelectedSetToBottom(void);

    ClipboardRecords *getSelectedRecords(void);

    void moveCursorToNewRecord(int mode, int pos);
    //    void on_parent_resizevent(QResizeEvent *e);


signals:

    void listSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void tapAndHoldGestureFinished(const QPoint &);


public slots:

    // Открытие контекстного меню
    void onCustomContextMenuRequested(const QPoint &pos);

    // Слот, срабатывающий после перетаскивания колонки
    void onSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
    void onSectionResized(int logicalIndex, int oldSize, int newSize);

    // Вызов действий из контекстного меню или из контроллера для редактирования инфополей записи
    void editFieldContext(void);
    void on_doubleclick(const QModelIndex &index);

protected slots:

    // Реакия на сдвиг засветки клавишами или мышкой
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    // Слот, который автоматически срабатыват при изменении selection в списке
    // Этот слот нигде не надо прописывать через connect(), так как он
    // является переопределенным, так как его тип virtual protected slot
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void onClickToRecord(const QModelIndex &index);

protected:

    QMenu           *contextMenu;
    TableScreen     *_tablescreen;
    TableController *_controller;
    QVBoxLayout     *_layout;

    void setupSignals(void);

    void assemblyContextMenu(void);

    void editField(int pos,
                   QString name,
                   QString author,
                   QString url,
                   QString tags);

    void deleteRecords(void);

    // Реакия на выбор записи мышкой или клавишами
    // The response to the record selection with the mouse or keys
    void click_record(const QModelIndex &index);

    bool event(QEvent *event);
    bool gestureEvent(QGestureEvent *event);
    void tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture);

    QPoint mouseStartPos;

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void saveColumnWidth(void);

    bool enableMoveSection;

    void resizeEvent(QResizeEvent *e);

private:

    void startDrag();
    friend class TableScreen;
    friend class VerticalScrollArea;
};

#endif /* RECORDLISTSCREEN_H_ */
