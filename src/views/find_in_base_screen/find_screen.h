#ifndef _FINDSCREEN_H_
#define _FINDSCREEN_H_

#include <memory>

#include "libraries/global_parameters.h"
#include "models/record_table/items_flat.h"
#include "models/tree/tree_item.h"
#include "utility/delegate.h"
#include <QMap>
#include <QToolBar>
#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class QLineEdit;
// class QPushButton;
class FlatToolButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QLabel;
class QCheckBox;
class QProgressDialog;
class vector_t;
class i_t;
class tkm_t;
class FindTableWidget;
// class MtComboBox;
class FlatComboBox;
class QStackedWidget;

class rctrl_t;
class ts_t;
struct Linker;

namespace web {
	class ChaseWidget;
	class ToolbarSearch;
	class TabWidget;
}

#ifndef SHOW_PROCESS_DIALOG
// #define SHOW_PROCESS_DIALOG
#endif

// Виджет поиска по базе
struct back_ground {
	boost::intrusive_ptr<i_t> _selected_branch_root;
};

class FindScreen : public QWidget
// , public back_ground
{
#if QT_VERSION == 0x050600
	W_OBJECT(FindScreen)
#else
	Q_OBJECT
#endif

    public:
	static constexpr char _find_in_base_expand[] = "findInBaseExpand"; // "find_in_base_expand";
	FindScreen(QString object_name, ts_t* tree_screen_, QWidget* parent = 0);
	virtual ~FindScreen(void);
	// QToolBar *navigater() {return _navigater;}

	FlatToolButton* findstartbutton();

	//	QAction* historyback() { return _historyback; }
	//	QAction* historyforward() { return _historyforward; }
	//	QAction* historyhome() { return _historyhome; }
	//	QAction* stopreload() { return _stopreload; }
	//	QAction* stop() { return _stop; }
	//	QAction* reload() { return _reload; }

	//	void historyback(QAction* hb);
	//	void historyforward(QAction* hf);
	//	void historyhome(QAction* hh);
	//	void stopreload(QAction* sl);
	//	void stop(QAction* st);
	//	void reload(QAction* rl);

	web::ChaseWidget* chasewidget();

	// void remove_child(const QString &id);
	// void remove_child(const int row);
	void adjust_size();

	// std::shared_ptr<sd::_interface<sd::meta_info<void >, RecordController *>>
	// reocrd_controller;  // for entrance
	// std::shared_ptr<sd::_interface<sd::meta_info<void >, web::TabWidget *>>
	// tabmanager;         // for entrance

	//	web::ToolbarSearch* toolbarsearch() const;
	QStackedWidget* lineedit_stack() { return _lineedit_stack; }
	void lineedit_stack(QStackedWidget* lineedit_stack_) { _lineedit_stack = lineedit_stack_; }
	void switch_stack(QStackedWidget* lineedit_stack_ = nullptr);
	void browser(web::Browser* bro);
    public slots:

	void widget_show(void);
	void widget_hide(void);
	boost::intrusive_ptr<i_t> find_internal_decomposed(const QString& search_text);
	void find_text(QString text_);

	void replace_navigater(QToolBar* nv);

    protected:
	virtual void resizeEvent(QResizeEvent* e);
    private slots:

	void enable_findbutton(const QString& text);
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
	void text_changed_from_another(const QString& str)
#if QT_VERSION == 0x050600
	    W_SIGNAL(text_changed_from_another, (const QString&), str) //
#else
	    ;
#endif
	    void find_clicked_after_another_text_changed(void)
#if QT_VERSION == 0x050600
		W_SIGNAL(find_clicked_after_another_text_changed) //
#else
	    ;
#endif

	    private :

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

	boost::intrusive_ptr<i_t> find_start(void);

	boost::intrusive_ptr<i_t>&
	find_recursive(boost::intrusive_ptr<i_t>& final_result, boost::intrusive_ptr<i_t> _session_root_item, boost::intrusive_ptr<i_t> curritem);

	bool find_in_text_process(const QString& text);

	void switch_tools_expand(bool flag);

	// QIcon _reloadicon;
	// QIcon _stopicon;
	ts_t* _tree_screen;
	QToolBar* _navigater;
	////    QHBoxLayout *_navigater;

	QAction* _historyback;    // FlatToolButton *_history_back;
	QAction* _historyforward; // FlatToolButton *_history_forward;
	QAction* _historyhome;    // FlatToolButton *_history_home;
	QAction* _stopreload;     // FlatToolButton *_stop_reload;

	QAction* _stop;
	QAction* _reload;

	web::ChaseWidget* _chasewidget;

	QHBoxLayout* _find_text_and_button_tools_area;

	FlatToolButton* _find_start_button; // QPushButton
	FlatToolButton* _tools_expand;

	QVBoxLayout* _close_button_tools_area;
	FlatToolButton* _close_button;

	QHBoxLayout* _combo_option_tools_area;
	FlatComboBox* _word_regard;
	FlatComboBox* _how_extract;
	FlatComboBox* _tree_search_area;

	QHBoxLayout* _where_find_line;
	QLabel* _where_find_label;
	QCheckBox* _find_in_pin;
	QCheckBox* _find_in_name;
	QCheckBox* _find_in_author;
	QCheckBox* _find_in_home;
	QCheckBox* _find_in_url;
	QCheckBox* _find_in_tags;
	QCheckBox* _find_in_text;

	QHBoxLayout* _tools_line;
	QGridLayout* _tools_grid;

	QVBoxLayout* _central_desktop_layout;
#ifdef SHOW_PROCESS_DIALOG
	QProgressDialog* _progress;
#endif
	// bool            _is_search_global = true;
	// FindTableWidget     *_findtable;
	// std::shared_ptr<ItemsFlat>      _selected_branch_as_pages;
	QVBoxLayout* _stack_layout;
	web::ToolbarSearch* _toolbarsearch_buffer; // QLineEdit *_findtext;     //
	// QStackedWidget *_lineedits;
	QStackedWidget* _lineedit_stack;

	// Поля, где нужно искать (Заголовок, текст, теги...)
	QMap<QString, bool> _search_area;

	// Список слов, которые нужно искать
	QStringList _search_word_list;

	int _total_progress_counter = 0;

	int _cancel_flag = 0;
	browser_ref _browser;
	friend class web::ToolbarSearch;
};

#endif /* _FINDSCREEN_H_ */
