#ifndef _METAEDITOR_H_
#define _METAEDITOR_H_

#include <memory>


#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QTextCodec>
#include <QDockWidget>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include "libraries/wyedit/editor.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"





#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif


#ifndef USE_QTM
#define USE_QTM
#endif

extern QMap<Qt::CheckState, QString>	_string_from_check_state;
extern QMap<QString, Qt::CheckState>	_state_check_from_string;


class QHBoxLayout;
class QGridLayout;
class QScrollArea;
class QSplitter;
class AttachTableScreen;
class Record;
class TreeItem;
class FindScreen;
class RecordModel;
class EditingWindow;

class ClickableLabel : public QLabel {
#if QT_VERSION == 0x050600
    W_OBJECT(ClickableLabel)
#else
    Q_OBJECT
#endif

    public:
	ClickableLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
//	W_CONSTRUCTOR(QWidget *, Qt::WindowFlags)
	//
	ClickableLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
//	W_CONSTRUCTOR(const QString &, QWidget *, Qt::WindowFlags)
// Q_SIGNALS:
    signals:
	void mousePressEvent(QMouseEvent *ev)
#if QT_VERSION == 0x050600
	W_SIGNAL(mousePressEvent, (QMouseEvent *), ev)	//
#else
	;
#endif
};



class MetaEditor : public Editor
//    : public QDockWidget
{
#if QT_VERSION == 0x050600
    W_OBJECT(MetaEditor)
#else
    Q_OBJECT
#endif

    public slots:

	void field(QString n, QString v);
	void clear_all(void);
	void on_click_to_tag(const QString &text);


    signals:
	// Сигнал вырабатывается при клике на текстовой метке
	void set_find_text_signal(QString text)
#if QT_VERSION == 0x050600
	W_SIGNAL(set_find_text_signal, text)	//
#else
	;
#endif

    public:
	MetaEditor(QString object_name, EditingWindow *editingwindow_, FindScreen *_find_screen);
	~MetaEditor(void);

	void tree_path(const QString &path);
	void pin(const QString &pin_);
	void switch_pin();
	void name(const QString &name_);
	void author(const QString &author_);
	void home(const QString &url);
	void url(const QString &url_);
	void tags(const QString &tags_);

	static void to_attach_callback(void);

	void to_editor_layout(void);
	void to_attach_layout(void);
	void bind(boost::intrusive_ptr<TreeItem> item_to_be_bound);		// {_record = r;}

	boost::intrusive_ptr<TreeItem>	item();		// {return _record;}

// #ifdef USE_QTM
//    public:
////	FlatToolButton		*_to_attach = nullptr;
////	QIcon _icon_attach_exists;		// Иконка, когда аттачи есть
////	QIcon _icon_attach_not_exists;		// Иконка, когда аттачей нет

//	// Методы установки нестандартных процедур чтения и сохранения текста
//	const std::function<void (QObject *editor, QString saveString)> save_callback() const {return _editor->save_callback();}
//	void save_callback(const std::function<void (QObject *editor, QString saveString)> &func){_editor->save_callback(func);}

//	std::function<void (QObject *editor, QString &String)> load_callback() const {return _editor->load_callback();}
//	void load_callback(const std::function<void (QObject *editor, QString &String)> &func){_editor->load_callback(func);}
//	static void editor_load_callback(QObject *editor, QString &loadText){Editor::editor_load_callback(editor, loadText);}
//	static void editor_save_callback(QObject *editor, QString saveText){Editor::editor_save_callback(editor, saveText);}
//	// Абсолютный или относительный путь (т.е. директория),
//	// куда будет сохраняться текст. Без завершающего слеша
//	bool work_directory(QString dir_name){return _editor->work_directory(dir_name);}
//	QString work_directory(void){return _editor->work_directory();}

//	// Имя файла, куда должен сохраняться текст
//	// Без пути, только имя
//	void file_name(QString file_name_){_editor->file_name(file_name_);}
//	QString file_name(void){return _editor->file_name();}
//	void save_textarea(){_editor->save_textarea();}

//	void dir_file_empty_reaction(int mode){_editor->dir_file_empty_reaction(mode);}
//	int dir_file_empty_reaction(void){return _editor->dir_file_empty_reaction();}

//	enum {
//	    DIRFILEEMPTY_REACTION_SHOW_ERROR,
//	    DIRFILEEMPTY_REACTION_SUPPRESS_ERROR
//	};
//	// Методы установки и чтения произвольных нестандартных данных
//	// которые может хранить объект редактора
//	void misc_field(QString name, QString value){_editor->misc_field(name, value);}
//	QString misc_field(QString name){return _editor->misc_field(name);}
//	bool load_textarea(){return _editor->load_textarea();}
//	int cursor_position(void){return _editor->cursor_position();}
//	void cursor_position(int n){_editor->cursor_position(n);}

//	int scrollbar_position(void){return _editor->scrollbar_position();}
//	void scrollbar_position(int n){_editor->scrollbar_position(n);}

//	void send_expand_edit_area(bool flag){_editor->send_expand_edit_area(flag);}
// #if QT_VERSION == 0x050600
//	W_SIGNAL(send_expand_edit_area, (bool), flag)	//
// #else
////	;
// #endif

//	void wyedit_find_in_base_clicked(){_editor->wyedit_find_in_base_clicked();}
// #if QT_VERSION == 0x050600
//	W_SIGNAL(wyedit_find_in_base_clicked)	//
// #else
////	;
// #endif

//	QTextDocument * textarea_document(void){return _editor->textarea_document();}
//	void textarea_editable(bool editable){_editor->textarea_editable(editable);}
//	// Метод позволяющий управлять доступностью инструментов редактирования
//	void disable_tool_list(QStringList toolNames){_editor->disable_tool_list(toolNames);}

//	void init_enable_assembly(bool flag){_editor->init_enable_assembly(flag);}
//	void init_config_file_name(QString name){_editor->init_config_file_name(name);}
//	void init_enable_random_seed(bool flag){_editor->init_enable_random_seed(flag);}
//	void init(int mode){_editor->init(mode);}

//	// Метод установки функции переключения на предыдущее окно (для мобильного интерфейса)
//	void back_callback(void (*func)(void)){_editor->back_callback(func);}

//	// Метод установки функции нажатия на кнопку Attach
//	void attach_callback(void (*func)(void)){_editor->attach_callback(func);}
//	void update_indentline_geometry(){_editor->update_indentline_geometry();}
//	void textarea(QString text){_editor->textarea(text);}
//	void clear_all_misc_field(void){_editor->clear_all_misc_field();}
////	// Горизонтальная линейка, содержащая кнопки форматирования
////	QVBoxLayout		*_textformat_buttons_layout = nullptr;
////	// Область редактирования текста
////	EditorTextArea *_text_area;


//	FlatToolButton *to_attach() const {return _editor->_to_attach;}

//	QIcon icon_attach_exists() const {return _editor->_icon_attach_exists;}
//	QIcon icon_attach_not_exists() const {return _editor->_icon_attach_not_exists;}

//	QVBoxLayout *textformat_buttons_layout() const {return _editor->_textformat_buttons_layout;}
//	EditorTextArea *text_area() const {return _editor->_text_area;}

// #endif

    private:
	void setup_labels(void);
	void setup_ui(void);
	void assembly(void);
	void setup_signals(FindScreen *_find_screen);

//	std::unique_ptr<EditingWindow> _blog_editor;
//	std::unique_ptr<Editor> _editor;
	FindScreen	*_find_screen;
	QWidget		*_editor_main_screen;
	QGridLayout	*_editor_main_layer;
	QLabel		*_tree_path;		// Надпись Path (только для мобильного интерфейса)
	QCheckBox	*_item_pin;

	QLabel		*_item_name;		// Надпись Title
	QLabel		*_item_author;		// Надпись Author(s)

	QLabel		*_label_home;		// Inscription    // Надпись "Url"
	QLabel		*_label_url;		// Inscription    // Надпись "Url"

	ClickableLabel	*_item_home;
	ClickableLabel	*_item_url;




	QScrollArea	*_item_tags_scrollarea;
	QHBoxLayout	*_item_tags_scrollarea_layout;
	QWidget		*_item_tags_container;
	QHBoxLayout	*_item_tags_layout;
	QLabel		*_label_tags;		// Надпись "Tags"

	QString	_item_tags_text;
	QStringList _item_tags_text_list;
	QList<QLabel *> _item_tags_labels;

	QSplitter       *_editor_and_filetable_splitter;

	// Виджет слоя прикрепляемых файлов
	AttachTableScreen   *_attachtable_screen;

	// Виджет слоя редактирования текста


	// Группировалка виджетов всех слоев (слоя редактирования и слоя прикрепляемых файлов)
	QVBoxLayout			*_meta_editor_join_layer;
	QMetaObject::Connection	_home_connection;	// for disconnect
	QMetaObject::Connection	_url_connection;	// for disconnect
	boost::intrusive_ptr<TreeItem>	_item;
//	QWidget *_hidetitlebar;
	EditingWindow *_editingwindow;
};

#endif /* _METAEDITOR_H_ */
