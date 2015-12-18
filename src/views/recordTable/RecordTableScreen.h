#ifndef RECORDTABLESCREEN_H_
#define RECORDTABLESCREEN_H_

#include <QWidget>
#include <QListView>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStringList>
#include <QMap>
#include <QString>
#include <QByteArray>
#include <QItemSelection>
#include <QLabel>

class RecordTableController;
class VerticalScrollArea;

class RecordTableScreen : public QWidget {
    Q_OBJECT

public:
    RecordTableScreen(QWidget *parent = 0);
    virtual ~RecordTableScreen();

    int     getFirstSelectionPos(void);
    QString getFirstSelectionId(void);
    void    select_pos(int pos);
    void    select_id(QString id);

    void setTreePath(QString path);
    QString getTreePath(void);

    inline RecordTableController *
    getRecordTableController()
    {
        return recordTableController;
    }

    // Действия, используемые как на тулбаре, так и в контекстном меню списка записей
    QAction *_actionpin;
    QAction *actionAddNewToEnd;
    QAction *actionAddNewBefore;
    QAction *actionAddNewAfter;
    QAction *actionEditField;
    QAction *actionDelete;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionSettings;
    QAction *actionBack;
    QAction *actionFindInBase;
    QAction *actionSort;
    QAction *actionPrint;

public slots:

    // Обновление панели инструментов
    void toolsUpdate(void);


protected:
    void resizeEvent(QResizeEvent *e);
private slots:

    void onSyncroClick(void);
    void onWalkHistoryPreviousClick(void);
    void onWalkHistoryNextClick(void);
    void onBackClick(void);

private:
    QToolBar *toolsLine;
    QToolBar *extraToolsLine;

    QLabel  *treePathLabel;
    QString treePath;

    VerticalScrollArea      *_verticalscrollarea;
    RecordTableController   *recordTableController;

    QHBoxLayout *recordTableToolsLayout;
    QVBoxLayout *recordTableScreenLayout;

    QAction *actionMoveUp;
    QAction *actionMoveDn;
    QAction *actionSyncro;
    QAction *actionWalkHistoryPrevious;
    QAction *actionWalkHistoryNext;

    void setupUI(void);
    void setupSignals(void);
    void setupActions(void);
    void assembly(void);

    void disableAllActions(void);

};

#endif /* RECORDTABLESCREEN_H_ */
