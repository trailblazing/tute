#ifndef _FINDSCREEN_H_
#define _FINDSCREEN_H_

#include <memory>
#include <QMap>
#include <QWidget>
#include <QToolBar>
#include "models/record_table/TableData.h"

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
    static const constexpr char *_find_in_base_expand = "findInBaseExpand"; // "find_in_base_expand";
    FindScreen(QString object_name, QWidget *parent = 0);
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
    void adjust_size();

public slots:

    void widget_show(void);
    void widget_hide(void);
    std::shared_ptr<TableData> find_clicked(void);
    void find_text(QString text);

protected:
    virtual void resizeEvent(QResizeEvent *e);
private slots:

    void enable_findbutton(const QString &text);
    void tools_expand_clicked(void);

    void word_regard(int pos);
    void how_extract(int pos);
    void tree_search_area(int pos);

    void if_find_in_pin(int state);
    void if_find_in_name(int state);
    void if_find_in_author(int state);
    void if_find_in_home(int state);
    void if_find_in_url(int state);
    void if_find_in_tags(int state);
    void if_find_in_text(int state);

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

    void setup_findtext_and_button(void);
    void assembly_findtext_and_button(void);

    void setup_combooption(void);
    void assembly_combooption(void);

    void setup_closebutton(void);
    void assembly_closebutton(void);

    void setup_wherefind_line(void);
    void assembly_wherefind_line(void);

    void setup_ui(void);
    void assembly(void);

    void setup_signals(void);

    void if_find_in_field(QString fieldname, int state);

    std::shared_ptr<TableData> find_start(void);
    void find_recursive(std::shared_ptr<TreeItem> curritem, std::shared_ptr<TableData> result);
    bool find_in_text_process(const QString &text);

    void switch_tools_expand(bool flag);

    // Поля, где нужно искать (Заголовок, текст, теги...)
    QMap<QString, bool> searchArea;

    // Список слов, которые нужно искать
    QStringList searchWordList;

    int totalProgressCounter;

    int cancelFlag;

};

#endif  /* _FINDSCREEN_H_ */

