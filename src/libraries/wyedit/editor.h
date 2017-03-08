#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <QBoxLayout>
#include <QDockWidget>
#include <QFontComboBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QStringList>
#include <QTextCodec>
#include <QTextEdit>
#include <QToolBar>
#include <QWidget>
#include <memory>

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include "editor_find_dialog.h"
#include "editor_show_text.h"
#include "libraries/fixed_parameters.h"

extern const std::string editor_prefix;

// Fix ugly Qt QSS bug
// #include "libraries/flat_control.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

// ----------------------------------------------------------
// WyEdit - визуальный редактор для MyTetra
// Волгодонск, 2010 г.
// Контакты: xintrea@gmail.com, www.webhamster.ru
// Данный исходный код распространяется под лицензией GPL v.3
// © Степанов С. М. 2010
// ----------------------------------------------------------

// 1.0 - Первая рабочая версия
// - Данная версия использовалась в MyTetra 1.25

// 1.1 - Основана на версии 1.0
// - Добавлена кнопка распахивания окна редактора и кнопка
// сворачивания/разворачивания панели инструментов
// - Доработан режим форматирования в код
// - Усилена очистка при нажатии кнопки [C]
// - Исправлено несколько проблемм, онаруженных пользователями
// - Данная версия использовалась в MyTetra 1.28

// 1.2 - Основана на версии 1.1
// - Переделан прототип конструктора
// - Переделан механизм инициализации объекта редактора,
// после создания объекта, необходимо установить начальные свойства
// с помощью методов
// initEnableAssembly()
// initConfigFileName()
// initEnableRandomSeed()
// и затем вызвать метод init()
// - Добавлен метод getVersion()

// 1.3 - Основана на версии 1.2
// - Сделаны методы для установки функций обратного вызова в редакторе,
// они нужны для замены стандартной работы с файлами
// set_save_callback()
// set_load_callback()
// - В редакторе сделано хранение произвольных полей данных,
// через них функции обратного вызова могут узнавать, в каких
// режимах им надо работать. Для этого добавлены методы
// setMiscField()
// getMiscField()

// 1.4 - Основана на версии 1.3
// - Добавлен метод setDisableToolList(), в котором можно указать имена
// инструментов, которые не нужно подгружать на панели
// инструментов редактора

// 1.5 - Основана на версии 1.4
// - Добавлен слот onModificationChanged(), чтобы правильно
// устанавливался флаг изменений после использования кнопки
// сохранения.
// Его использование закомментировано, так как выяснено,
// что флаг правильно устанавливается в документе объекта
// QTextEdit автоматически

// 1.6 - Основна на версии 1.5
// - В методе init() появился параметр mode, отвечащий за режим
// инициализации и отображения редактора - desktop или mobile

#define WYEDIT_VERSION "WyEdit v.1.7 / 21.06.2015"

#define MINIMUM_ALLOWED_FONT_SIZE 5
#define MAXIMUM_ALLOWED_FONT_SIZE 100

class gl_para;
extern std::shared_ptr<gl_para> gl_paras;

class EditorConfig;
class EditorTextEdit;
class EditorContextMenu;
class EditorTextArea;
class IndentSlider;
class FlatToolButton;
class FlatComboBox;
class FlatFontComboBox;
class FlatToolButton;
class FindScreen;
class EditorDock;
class SuperMenu;
class Blogger;
class QStackedWidget;
class QSettings;

class Editor : public QWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(Editor)
#else
	Q_OBJECT
#endif

	public:
	Editor(QStackedWidget *main_stack, Blogger *blogger_, std::shared_ptr<QSettings> topic_editor_config_, int mode, QStringList hide_editor_tools, bool enable_assembly = false, bool enable_random_seed = false);
	~Editor(void);

	std::shared_ptr<EditorConfig> _editor_config;
	std::unique_ptr<EditorFindDialog> _find_dialog;  // Виджет поиска
	// Кнопки форматирования текста
	FlatToolButton *_bold = nullptr;
	FlatToolButton *_italic = nullptr;
	FlatToolButton *_underline = nullptr;
	FlatToolButton *_monospace = nullptr;
	FlatToolButton *_code = nullptr;
	FlatToolButton *_clear = nullptr;

	FlatToolButton *_numeric_list = nullptr;
	FlatToolButton *_dot_list = nullptr;

	FlatToolButton *_indent_plus = nullptr;
	FlatToolButton *_indent_minus = nullptr;

	FlatToolButton *_align_left = nullptr;
	FlatToolButton *_align_center = nullptr;
	FlatToolButton *_align_right = nullptr;
	FlatToolButton *_align_width = nullptr;

	FlatToolButton *_action_open = nullptr;
	FlatToolButton *_action_save = nullptr;
	// , *action_Bold
	// , *action_Italic
	// , *action_Underline
	// , *action_Blockquote
	FlatToolButton *_action_link = nullptr;
	FlatToolButton *_action_image = nullptr;
	FlatToolButton *_action_more = nullptr;
	FlatToolButton *_action_preview = nullptr;
	FlatToolButton *_action_blog_this = nullptr;

	FlatToolButton *_settings = nullptr;

	// QFontComboBox
	FlatFontComboBox *_font_select = nullptr;
	FlatComboBox *_font_size = nullptr;
	FlatToolButton *_font_color = nullptr;

	FlatToolButton *_show_html = nullptr;
	FlatToolButton *_find_text = nullptr;
	FlatToolButton *_show_formatting = nullptr;

	FlatToolButton *_create_table = nullptr;
	FlatToolButton *_table_remove_row = nullptr;
	FlatToolButton *_table_remove_col = nullptr;
	FlatToolButton *_table_add_row = nullptr;
	FlatToolButton *_table_add_col = nullptr;
	FlatToolButton *_table_merge_cells = nullptr;
	FlatToolButton *_table_split_cell = nullptr;

	FlatToolButton *_insert_image_from_file = nullptr;
	FlatToolButton *_expand_edit_area = nullptr;
	FlatToolButton *_expand_tools_lines = nullptr;
	FlatToolButton *_save = nullptr;

	FlatToolButton *_back = nullptr;
	// FlatToolButton		*_freeze		= nullptr;          // reserved for read
	// only
	FlatToolButton *_find_in_base = nullptr;

	FlatToolButton *_show_text = nullptr;

	FlatToolButton *_to_attach = nullptr;
	QIcon _icon_attach_exists =
	    QIcon(":/resource/pic/attach_exists.svg");  // Иконка, когда аттачи есть
	QIcon _icon_attach_not_exists =
	    QIcon(":/resource/pic/attach.svg");  // Иконка, когда аттачей нет

	IndentSlider *_indent_slider = nullptr;

	// Горизонтальная линейка, содержащая кнопки форматирования
	QVBoxLayout *_textformat_buttons_layout = nullptr;
	QToolBar *_tools_line_0 = nullptr;
	QToolBar *_tools_line_1 = nullptr;

	// Вертикальная группировалка линеек кнопок и области редактирования
	QVBoxLayout *full_layout = nullptr;
// Editentry *_editentry;
//// Контекстное меню редактора
// EditorContextMenu   *_editor_context_menu = nullptr;

#ifdef ADD_ACTIONS_BY_NAME
	void add_action_by_name(QString toolName, QToolBar *line);
#endif
	const char *version(void);

	// void init_enable_assembly(bool flag);
	//	static QString init_config_file_name(QString config_full_name);
	// void init_enable_random_seed(bool flag);
	void init(int mode, QStringList hide_editor_tools, bool enable_assembly, bool enable_random_seed);

	// Методы работы с textarea
	void textarea(QString text);
	void textarea_editable(bool editable);
	QString textarea(void);
	QTextCursor textCursor() const;
	void setTextCursor(const QTextCursor &cursor);
	void insertPlainText(const QString &text);
	void setPlainText(const QString &text);
	QString toPlainText() const;
	void ensureCursorVisible() const;
	QTextDocument *document(void);
	void textarea_modified(bool modify);
	bool textarea_modified(void);

	// Абсолютный или относительный путь (т.е. директория),
	// куда будет сохраняться текст. Без завершающего слеша
	bool work_directory(QString dir_name);
	QString work_directory(void);

	// Имя файла, куда должен сохраняться текст
	// Без пути, только имя
	void file_name(QString _file_name);
	QString file_name(void);

	static void editor_load_callback(QObject *editor, QString &load_text);
	static void editor_save_callback(QObject *editor, const QString &save_text);
	void save_textarea();
	bool save_textarea_text();
	bool save_textarea_images(int mode);
	bool load_textarea();

	// Методы установки нестандартных процедур чтения и сохранения текста
	const std::function<void(QObject *editor, QString saveString)>
	save_callback() const;
	void save_callback(
	    const std::function<void(QObject *editor, QString saveString)> &func);

	std::function<void(QObject *editor, QString &String)> load_callback() const;
	void load_callback(
	    const std::function<void(QObject *editor, QString &String)> &func);

	// Метод установки функции переключения на предыдущее окно (для мобильного
	// интерфейса)
	void back_callback(void (*func)(void));

	// Метод установки функции нажатия на кнопку Attach
	void attach_callback(void (*func)(void));

	// Методы установки и чтения произвольных нестандартных данных
	// которые может хранить объект редактора

	template <typename field_key_type>
	void misc_field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, field_key_type>::type>::type& value);  // , typename field_type_switch = typename


	template <typename field_key_type>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, field_key_type>::type>::type misc_field() const;



	void clear_all_misc_field(void);
	void update_indentline_geometry();

	void dir_file_empty_reaction(int mode);
	int dir_file_empty_reaction(void);

	// Метод позволяющий управлять доступностью инструментов редактирования
	// void disable_tool_list(QStringList tool_names);

	int cursor_position(void);
	void cursor_position(int n);

	int scrollbar_position(void);
	void scrollbar_position(int n);

	// QToolBar *tool_bar();
	// SuperMenu *context_menu();

	enum { SAVE_IMAGES_SIMPLE = 0,
		   SAVE_IMAGES_REMOVE_UNUSED = 1 };

	enum {
		DIRFILEEMPTY_REACTION_SHOW_ERROR,
		DIRFILEEMPTY_REACTION_SUPPRESS_ERROR
	};

	enum { WYEDIT_DESKTOP_MODE = 0,
		   WYEDIT_MOBILE_MODE = 1 };

	signals:

	// Сигналы установки отступов на линейке с движками
	// согласно текущему форматированию
	void send_set_textindent_pos(int i)
#if QT_VERSION == 0x050600
	    W_SIGNAL(send_set_textindent_pos, (int), i)  //
#else
	    ;
#endif
	    void send_set_leftindent_pos(int i)
#if QT_VERSION == 0x050600
	        W_SIGNAL(send_set_leftindent_pos, (int), i)  //
#else
	    ;
#endif

	    void send_set_rightindent_pos(int i)
#if QT_VERSION == 0x050600
	        W_SIGNAL(send_set_rightindent_pos, (int), i)  //
#else
	    ;
#endif

	    void send_expand_edit_area(bool flag)
#if QT_VERSION == 0x050600
	        W_SIGNAL(send_expand_edit_area, (bool), flag)  //
#else
	    ;
#endif

	    void wyedit_find_in_base_clicked()
#if QT_VERSION == 0x050600
	        W_SIGNAL(wyedit_find_in_base_clicked)  //
#else
	    ;
#endif

	    public slots :

	    void on_update_indentline_geometry_slot(void);

	private slots:

	// Действия в области редактирования
	void on_bold_clicked(void);
	void on_italic_clicked(void);
	void on_underline_clicked(void);
	void on_monospace_clicked(void);
	void on_code_clicked(void);
	void on_clear_clicked(void);

	void on_numericlist_clicked(void);
	void on_dotlist_clicked(void);
	void on_indentplus_clicked(void);
	void on_indentminus_clicked(void);

	void on_alignleft_clicked(void);
	void on_aligncenter_clicked(void);
	void on_alignright_clicked(void);
	void on_alignwidth_clicked(void);

	void on_fontselect_changed(const QFont &font);
	void on_fontsize_changed(int i);
	void on_fontcolor_clicked();

	void set_fontselect_on_display(QString font_name);
	void set_fontsize_on_display(int n);

	void on_showhtml_clicked(void);
	void on_findtext_clicked(void);
	void on_settings_clicked(void);
	void on_showformatting_clicked(void);

	void on_createtable_clicked(void);
	void on_table_remove_row_clicked(void);
	void on_table_remove_col_clicked(void);
	void on_table_add_row_clicked(void);
	void on_table_add_col_clicked(void);
	void on_table_merge_cells_clicked(void);
	void on_table_split_cell_clicked(void);

	void on_insert_image_from_file_clicked(void);
	void on_expand_edit_area_clicked(void);
	void on_expand_tools_lines_clicked(void);
	void on_save_clicked(void);
	void on_back_clicked(void);
	// void on_freeze_clicked(void);
	void on_find_in_base_clicked(void);
	void on_show_text_clicked(void);
	void on_to_attach_clicked(void);

	void
	on_cursor_position_changed(void);  // Слот, контролирущий перемещение курсора
	void on_selection_changed(void);
	void on_undo(void);
	void on_redo(void);
	void cut(void);
	void copy(void);
	void paste(void);
	void on_selectAll(void);

	void on_findtext_signal_detect(const QString &text, QTextDocument::FindFlags flags);

	// Слоты обработки перемещения движков настройки отступов
	void on_indentline_change_textindent_pos(int i);
	void on_indentline_change_leftindent_pos(int i);
	void on_indentline_change_rightindent_pos(int i);
	void on_indentline_mouse_release(void);

	// Открытие контекстного меню
	void on_custom_contextmenu_requested(const QPoint &_position);

	void on_context_menu_edit_image_properties(void);

	// void onModificationChanged(bool flag);

	protected:
	// Область редактирования текста
	EditorTextArea *_text_area;

	private:
	bool _init_data_enable_assembly = true;
	// QString _init_data_config_file_name	= "";
	bool _init_data_enable_random_seed = false;

	// абочая директория редактора и файл текста
	// Используется при сохранении текста на диск
	QString _work_directory;
	QString _work_file_name;

	int _view_mode;  // ежим отображения редактора - WYEDIT_DESKTOP_MODE или
	                 // WYEDIT_MOBILE_MODE
	Blogger *_blog;

	private:
	void setup_signals(void);
	void setup_buttons(void);
	void setup_editor_area(void);
	void assembly_buttons(void);
	void assembly(void);
	void hide_all_tools_elements(void);
	void format_to_list(QTextListFormat::Style setFormat);
	void align_text(Qt::AlignmentFlag mode);
#ifdef ADD_ACTIONS_BY_NAME
	void add_actions_by_name(void);
#endif
	bool is_block_select(void);
	bool is_cursor_on_empty_line(void);
	bool is_cursor_on_space_line(void);
	bool is_image_select(void);
	bool is_cursor_on_image(void);

	QTextImageFormat image_format_on_select(void);
	QTextImageFormat image_format_on_cursor(void);

	void edit_image_properties(void);

	void update_tool_line_to_actual_format(void);
	void update_indentslider_to_actual_format(void);
	void update_align_button_higlight(bool activate);
	void update_outline_button_higlight(void);
	void set_outline_button_higlight(int button, bool active);
	bool is_key_for_tool_line_update(QKeyEvent *event);

	// Метод, переключающий состояние видимости полной панели инструментов
	// Если вызывается без параметра, метод сам переключает
	// Параметр 1 - включить полную видимость
	// Параметр -1 - выключить полную видимость
	void switch_expand_tools_lines(int flag = 0);

	// Переопределяется слот обработки клавиш
	// нужен для определения момента undo/redo
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event);

	void setup_closebutton(void);
	void assembly_closebutton(void);
	// void widget_hide(bool);

	QString _current_font_family;
	int _current_font_size;
	QString _current_font_color;
	bool _flag_set_font_parameters_enabled;
	int _current_text_indent;
	int _current_left_indent;
	int _current_right_indent;

	QColor _buttons_select_color;  // Цвет рамки выделенных кнопок

	bool _expand_edit_area_flag;  // аспахнуто ли на максимум окно редактора

	// Указатели на переопределенные функции записи и чтения редактируемого текста
	std::function<void(QObject *editor, QString saveString)>
	    save_callback_func;  // void (*save_callback_func)(QObject *editor, QString
	                         // saveString);
	std::function<void(QObject *editor, QString &String)>
	    load_callback_func;  // void (*load_callback_func)(QObject *editor, QString
	                         // &loadString);

	// Указатель на функцию переключения на предыдущее окно (для мобильного
	// интерфейса)
	void (*back_callback_func)(void);

	// Указатель на функцию открытия присоединенных файлов
	void (*attach_callback_func)(void);

	// Поля для хранения произвольных данных
	// Обычно используются для запоминания нестандартного набора данных
	// в объекте редактора, и считываются из функции обратного вызова
	// Fields to store arbitrary data. Commonly used for storing custom data set Object Editor, and read from the callback function
	full_fields_map _misc_fields;//QMap<QString, QString> _misc_fields;

	int _dir_file_empty_reaction = DIRFILEEMPTY_REACTION_SHOW_ERROR;
	QStringList _tools_name_list_in_line_0;
	QStringList _tools_name_list_in_line_1;

	// Список инструментов, которые ненужно подгружать
	QStringList _disable_tool_list;
	FlatToolButton *_close_button;
	QVBoxLayout *_toolsarea_of_close_button;

	enum {
		BT_BOLD,
		BT_ITALIC,
		BT_UNDERLINE,
		BT_ALIGN_LEFT,
		BT_ALIGN_CENTER,
		BT_ALIGN_RIGHT,
		BT_ALIGN_WIDTH
	};
	// QWidget *_hidetitlebar;
	// SuperMenu *_context_menu;

	friend class MetaEditor;
	friend class EditorDock;
	friend class Blogger;
	friend class SuperMenu;

	protected:
	// virtual void resizeEvent(QResizeEvent *e);
};


#ifndef EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION
#define EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(key_type) \
	extern template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type Editor::misc_field<key_type>() const;
#endif  // EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION

EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(id_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(pin_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(rating_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(name_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(author_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(home_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(url_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(tags_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(ctime_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(dir_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(file_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(crypt_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(has_attach_key)
EDITOR_EXTERN_MISC_FIELD_GET_DECLARATION(attach_size_key)


#ifndef EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION
#define EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(key_type) \
	extern template void Editor::misc_field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type& value);
#endif  // EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION

EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(id_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(pin_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(rating_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(name_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(author_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(home_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(url_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(tags_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(ctime_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(dir_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(file_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(crypt_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(has_attach_key)
EDITOR_EXTERN_MISC_FIELD_SET_DECLARATION(attach_size_key)


#endif /* _EDITOR_H_ */
