#ifndef _FINDSCREEN_H_
#define _FINDSCREEN_H_

#include <memory>
#include <QMap>
#include <QWidget>
#include <QToolBar>
#include "models/record_table/ItemsFlat.h"
#include "models/tree/TreeItem.h"



class QLineEdit;
// class QPushButton;
class FlatToolButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QLabel;
class QCheckBox;
class QProgressDialog;

class TreeModelKnow;
class TreeItem;

class FindTableWidget;

class MtComboBox;
class FlatComboBox;
class QStackedWidget;
class ItemsFlat;
class TreeItem;

namespace browser {
    class ChaseWidget;
    class ToolbarSearch;
}

// Виджет поиска по базе

class FindScreen : public QWidget {
    Q_OBJECT

public:
    static const constexpr char *_find_in_base_expand = "findInBaseExpand"; // "find_in_base_expand";
    FindScreen(QString object_name, boost::intrusive_ptr<TreeItem> _selected_branch_root, QWidget *parent = 0);
    virtual ~FindScreen(void);
    //QToolBar *navigater() {return _navigater;}

    //    void toolbarsearch(browser::ToolbarSearch *toolbarsearch) {_toolbarsearch = toolbarsearch;}
    browser::ToolbarSearch *toolbarsearch() {return _toolbarsearch;}

    FlatToolButton *findstartbutton() {return _find_start_button;}

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
    boost::intrusive_ptr<TreeItem> find_clicked(void);
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
    void text_changed_from_another(const QString &);
    void find_clicked_after_another_text_changed(void);

private:

    //    QIcon _reloadicon;
    //    QIcon _stopicon;

    QToolBar *_navigater;
    //    //    QHBoxLayout *_navigater;

    QAction *_historyback;      //    FlatToolButton *_history_back;
    QAction *_historyforward;   //    FlatToolButton *_history_forward;
    QAction *_historyhome;      //    FlatToolButton *_history_home;
    QAction *_stopreload;       //    FlatToolButton *_stop_reload;

    browser::ChaseWidget *_chasewidget;

    QHBoxLayout     *_find_text_and_button_tools_area;

    FlatToolButton  *_find_start_button;   // QPushButton
    FlatToolButton  *_tools_expand;

    QVBoxLayout     *_close_button_tools_area;
    FlatToolButton  *_close_button;

    QHBoxLayout     *_combo_option_tools_area;
    FlatComboBox    *_word_regard;
    FlatComboBox    *_how_extract;
    FlatComboBox    *_tree_search_area;

    QHBoxLayout     *_where_find_line;
    QLabel          *_where_find_label;
    QCheckBox       *_find_in_pin;
    QCheckBox       *_find_in_name;
    QCheckBox       *_find_in_author;
    QCheckBox       *_find_in_home;
    QCheckBox       *_find_in_url;
    QCheckBox       *_find_in_tags;
    QCheckBox       *_find_in_text;

    QHBoxLayout     *_tools_line;
    QGridLayout     *_tools_grid;

    QVBoxLayout     *_central_desktop_layout;

    QProgressDialog *_progress;

    boost::intrusive_ptr<TreeItem>  _selected_branch_root;         // std::shared_ptr<RecordTable>  _resultset_data;        // = std::make_shared<TableData>();      //    FindTableWidget     *_findtable;
    browser::ToolbarSearch          *_toolbarsearch;    //    QLineEdit *_findtext;     //    QStackedWidget *_lineedits;

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

    boost::intrusive_ptr<TreeItem> find_start(void);
    boost::intrusive_ptr<TreeItem> find_recursive(boost::intrusive_ptr<TreeItem> curritem, boost::intrusive_ptr<TreeItem> _selected_branch_root);
    bool find_in_text_process(const QString &text);

    void switch_tools_expand(bool flag);

    // Поля, где нужно искать (Заголовок, текст, теги...)
    QMap<QString, bool> _search_area;

    // Список слов, которые нужно искать
    QStringList _search_word_list;

    int         _total_progress_counter;

    int         _cancel_flag;

};

#endif  /* _FINDSCREEN_H_ */

