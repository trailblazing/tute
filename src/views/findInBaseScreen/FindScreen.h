#ifndef _FINDSCREEN_H_
#define _FINDSCREEN_H_

#include <memory>
#include <QMap>
#include <QWidget>
#include <QToolBar>
#include "models/recordTable/TableData.h"

class QLineEdit;
// class QPushButton;
class FlatToolButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QLabel;
class QCheckBox;
class QProgressDialog;

class KnowTreeModel;
class TreeItem;

class FindTableWidget;

class MtComboBox;
class FlatComboBox;
class QStackedWidget;
class TableData;

namespace browser {
    class ChaseWidget;
    class ToolbarSearch;
}

// Виджет поиска по базе

class FindScreen : public QWidget {
    Q_OBJECT

public:
    FindScreen(QWidget *parent = 0);
    virtual ~FindScreen(void);
    //QToolBar *navigater() {return _navigater;}

    void toolbarsearch(browser::ToolbarSearch *toolbarsearch) {_toolbarsearch = toolbarsearch;}
    browser::ToolbarSearch *toolbarsearch() {return _toolbarsearch;}

    FlatToolButton *findstartbutton() {return _findstartbutton;}

    QAction *historyback() {return _historyback;}
    QAction *historyforward() {return _historyforward;}
    QAction *historyhome() {return _historyhome;}
    QAction *stopreload() {return _stopreload;}
    browser::ChaseWidget *chasewidget() {return _chasewidget;}
    void remove_id(const QString &id);
    void remove_row(const int row);
    void adjustSize();

public slots:

    void widgetShow(void);
    void widgetHide(void);
    std::shared_ptr<TableData> findClicked(void);
    void setFindText(QString text);

protected:
    virtual void resizeEvent(QResizeEvent *e);
private slots:

    void enableFindButton(const QString &text);
    void toolsExpandClicked(void);

    void changedWordRegard(int pos);
    void changedHowExtract(int pos);
    void changedTreeSearchArea(int pos);

    void changedFindInPin(int state);
    void changedFindInName(int state);
    void changedFindInAuthor(int state);
    void changedFindInHome(int state);
    void changedFindInUrl(int state);
    void changedFindInTags(int state);
    void changedFindInText(int state);

signals:

    // Сигнал вырабатывается, когда обнаружено что в слоте setFindText()
    // был изменен текст для поиска
    void textChangedFromAnother(const QString &);

    void findClickedAfterAnotherTextChanged(void);

private:

    //    QIcon _reloadicon;
    //    QIcon _stopicon;

    QToolBar *_navigater;

    //    //    QHBoxLayout *_navigater;
    //    //    FlatToolButton *historyback_;
    QAction *_historyback;
    QAction *_historyforward;
    QAction *_historyhome;
    QAction *_stopreload;
    //    FlatToolButton *_history_back;
    //    FlatToolButton *_history_forward;
    //    FlatToolButton *_history_home;
    //    FlatToolButton *_stop_reload;
    browser::ChaseWidget *_chasewidget;

    QHBoxLayout     *toolsAreaFindTextAndButton;

    FlatToolButton  *_findstartbutton;   // QPushButton
    FlatToolButton  *toolsExpand;

    QVBoxLayout     *toolsAreaCloseButton;
    FlatToolButton  *closeButton;

    QHBoxLayout *toolsAreaComboOption;
    FlatComboBox *wordRegard;
    FlatComboBox *howExtract;
    FlatComboBox *treeSearchArea;

    QHBoxLayout *whereFindLine;
    QLabel      *whereFindLabel;
    QCheckBox   *findInPin;
    QCheckBox   *findInName;
    QCheckBox   *findInAuthor;
    QCheckBox   *findInHome;
    QCheckBox   *findInUrl;
    QCheckBox   *findInTags;
    QCheckBox   *findInText;

    QHBoxLayout *toolsLine;
    QGridLayout *toolsGrid;

    QVBoxLayout *centralDesktopLayout;

    QProgressDialog *_progress;
    //    QLineEdit *_findtext;
    //    QStackedWidget *_lineedits;
    //    FindTableWidget             *_findtable;    // result of finding?

    std::shared_ptr<TableData>  _result = std::make_shared<TableData>();
    browser::ToolbarSearch      *_toolbarsearch;

    void setup_navigate(void);
    void assembly_navigate(void);

    void setupFindTextAndButton(void);
    void assemblyFindTextAndButton(void);

    void setupComboOption(void);
    void assemblyComboOption(void);

    void setupCloseButton(void);
    void assemblyCloseButton(void);

    void setupWhereFindLine(void);
    void assemblyWhereFindLine(void);

    void setupUI(void);
    void assembly(void);

    void setupSignals(void);

    void changedFindInField(QString fieldname, int state);

    std::shared_ptr<TableData> findStart(void);
    void find_recursive(std::shared_ptr<TreeItem> curritem, std::shared_ptr<TableData> result);
    bool findInTextProcess(const QString &text);

    void switchToolsExpand(bool flag);

    // Поля, где нужно искать (Заголовок, текст, теги...)
    QMap<QString, bool> searchArea;

    // Список слов, которые нужно искать
    QStringList searchWordList;

    int totalProgressCounter;

    int cancelFlag;

};

#endif  /* _FINDSCREEN_H_ */

