

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <chrono>

#include <QColor>
#include <QColorDialog>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QLayout>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QPalette>
#include <QScrollBar>
#include <QSplitter>
#include <QStackedWidget>
#include <QStringList>
#include <QStyle>
#include <QTextCursor>
#include <QToolButton>
#include <QtGlobal>
#include <QtGui>

#include "editor.h"
#include "editor_abs_table.h"
#include "editor_add_table_form.h"
#include "editor_config.h"
#include "editor_config_dialog.h"
#include "editor_context_menu.h"
#include "editor_image_properties.h"
#include "editor_multi_line_input_dialog.h"
#include "editor_split_cell_form.h"
#include "editor_text_area.h"
#include "indentslider/indent_slider.h"
#include "libraries/crypt/crypt_service.h"
#include "libraries/disk_helper.h"
#include "libraries/fixed_parameters.h"
#include "libraries/flat_control.h"
#include "libraries/global_parameters.h"
#include "libraries/qtm/blogger.h"
#include "libraries/qtm/super_menu.h"
#include "main.h"
#include "models/app_config/app_config.h"
#include "utility/add_action.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/tree/tree_screen.h"

#include "libraries/qtm/addimg.xpm"
#include "libraries/qtm/addlink.xpm"
#include "libraries/qtm/blog-this.xpm"
// #include "libraries/qtm/fileopen.xpm"
// #include "libraries/qtm/filesave.xpm"
#include "libraries/qtm/bquote.xpm"
#include "libraries/qtm/more.xpm"
#include "libraries/qtm/paragraph.xpm"
#include "libraries/qtm/preview.xpm"
#include "libraries/qtm/textbold.xpm"
#include "libraries/qtm/textital.xpm"
#include "libraries/qtm/textul.xpm"
// #include "libraries/qtm/addtag.xpm"
// #include "libraries/qtm/addctag.xpm"
// #include "libraries/qtm/remtag.xpm"

#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "libraries/window_switcher.h"
#include "views/browser/docker.h"
extern std::shared_ptr<AppConfig> appconfig;
extern const char* action_find_in_base;
const std::string editor_prefix = "editor_tb_";

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(Editor)
#endif

Editor::Editor(QStackedWidget* main_stack, Blogger* blogger_, std::shared_ptr<QSettings> topic_editor_config_, int mode, QStringList hide_editor_tools, bool enable_assembly, bool enable_random_seed)
    : QWidget(main_stack)
    , _editor_config([&]() -> std::shared_ptr<EditorConfig> {
	    return std::make_shared<EditorConfig>(topic_editor_config_, nullptr);
    }())
    , _find_dialog(std::make_unique<EditorFindDialog>(_editor_config, nullptr))
    , _tools_line_0(new QToolBar())
    , _tools_line_1(new QToolBar())
    , _text_area(new EditorTextArea(this))
    , _blogger(blogger_)
{
	// _init_data_enable_assembly		= true;
	// _init_data_config_file_name		= "";
	// _init_data_enable_random_seed	= false;

	// _dir_file_empty_reaction = DIRFILEEMPTY_REACTION_SHOW_ERROR;

	save_callback_func = nullptr;
	load_callback_func = nullptr;
	back_callback_func = nullptr;
	work_directory(blogger_->_current_topic_full_folder_name, QString(Blogger::_default_filename) + ".html"); // file_name(blogger_->_default_filename + ".html");
	// init_enable_assembly(enable_assembly);
	// init_enable_random_seed(enable_random_seed);
	init(mode, hide_editor_tools, enable_assembly, enable_random_seed);
}

Editor::~Editor(void)
{
	// delete _editor_config;
	delete _bold;
	delete _italic;
	delete _underline;
	delete _monospace;
	delete _code;
	delete _clear;
	delete _numeric_list;
	delete _dot_list;
	delete _indent_plus;
	delete _indent_minus;
	delete _align_left;
	delete _align_center;
	delete _align_right;
	delete _align_width;
	delete _settings;
	delete _font_select;
	delete _font_size;
	delete _font_color;
	delete _show_html;
	delete _find_text;
	delete _show_formatting;
	delete _create_table;
	delete _table_remove_row;
	delete _table_remove_col;
	delete _table_add_row;
	delete _table_add_col;
	delete _table_merge_cells;
	delete _table_split_cell;
	delete _insert_image_from_file;
	delete _expand_edit_area;
	delete _expand_tools_lines;
	delete _save;
	delete _back;
	// delete _freeze;
	delete _find_in_base;
	delete _show_text;
	delete _to_attach;
	delete _indent_slider;
	delete _textformat_buttons_layout;
	delete _tools_line_0;
	delete _tools_line_1;
	delete full_layout;
	// delete _editor_context_menu;
	delete _text_area;
}

const char* Editor::version(void)
{
	static const char* lib_version = WYEDIT_VERSION;

	return lib_version;
}

// void Editor::init_enable_assembly(bool flag){
// _init_data_enable_assembly = flag;
// }

// QString Editor::init_config_file_name(QString config_full_name){
//	// name				= globalparameters.work_directory()
//	// + "/" + globalparameters.target_os() + "/editorconf.ini"
//	auto location = gl_paras->root_path() + "/" + gl_paras->target_os();
//	assert(config_full_name.contains(location));
//	QDir conf_dir(location);
//	if(!conf_dir.exists())
//		if(!QDir::root().mkpath(location))
//			critical_error("void Editor::init_config_file_name(QString
//"
//			               "config_full_name) can not make dir \""
//			               + gl_paras->target_os() + "\"");
//	QFile conf_file(config_full_name);
//	if(!conf_file.exists())
//		if(!QFile::copy(":/resource/standardconfig/" +
//gl_paras->target_os() + "/editorconf.ini", config_full_name))
//			critical_error("void Editor::init_config_file_name(QString
//"
//			               "config_full_name) can not copy file \""
//			               + config_full_name + "\"");
//	conf_file.setPermissions(config_full_name, QFile::ReadUser |
//QFile::WriteUser);
//	// _init_data_config_file_name = config_full_name;
//	return config_full_name;
//}

// void Editor::init_enable_random_seed(bool flag){
// _init_data_enable_random_seed = flag;
// }

// Инициализация редактора
// Если mode=WYEDIT_DESKTOP_MODE - происходит обычная инициализация
// Если mode=WYEDIT_MOBILE_MODE - при инициализации в первую строку панели
// инструментов, слева, добавляется кнопка back
void Editor::init(int mode, QStringList hide_editor_tools, bool enable_assembly, bool enable_random_seed)
{
	_view_mode = mode;

	// Создается объект поддержки конфигурирования редактора
	// _editor_config =
	// std::make_shared<EditorConfig>(_init_data_config_file_name, nullptr);//
	// this
	_editor_config->setObjectName("editorconfig");
	//	gl_paras->editor_config(_editor_config);

	// Выясняется перечень кнопок на панели инструментов
	_tools_name_list_in_line_0 = (_editor_config->tools_line_0()).split(",");
	_tools_name_list_in_line_1 = (_editor_config->tools_line_1()).split(",");
	// В мобильном режиме добавляется кнопка back (если ее нет)
	if (_view_mode == WYEDIT_MOBILE_MODE &&
	    !_tools_name_list_in_line_0.contains("back")) {
		_tools_name_list_in_line_0.prepend("separator");
		_tools_name_list_in_line_0.prepend("back");
	}
	// В мобильном режиме добавляется кнопка find_in_base (если ее нет)
	if (_view_mode == WYEDIT_MOBILE_MODE &&
	    !_tools_name_list_in_line_0.contains(action_find_in_base)) {
		_tools_name_list_in_line_0.append("separator");
		_tools_name_list_in_line_0.append(action_find_in_base);
	}
	// Создается виджет поиска, обязательно нужно указать parent чтобы
	// могли применяться флаги окна.
	// Виджет будет постоянно включен параллельно с работой редактора.
	// Только будет либо виден, либо невиден.
	// Create search widget, be sure to specify the parent to
	// Window flags can be used.
	// The widget will be constantly connected in parallel with the work of the
	// editor.
	// Just to be either visible or invisible.
	// _find_dialog = std::make_unique<EditorFindDialog>(_editor_config,
	// nullptr);// this
	_find_dialog->setVisible(false);
	_find_dialog->hide();
	_find_dialog->setHidden(true);

	//// Создаётся контекстное меню
	// _editor_context_menu = new EditorContextMenu(this);

	setup_buttons();

	setup_closebutton();
	assembly_closebutton();
	assembly_buttons();

	setup_editor_area();

	setup_signals();
	if (enable_assembly) assembly();
	_current_font_family = "";
	_current_font_size = 0;
	_current_font_color = "#000000";
	_flag_set_font_parameters_enabled = true;
	_current_text_indent = 0;
	_current_left_indent = 0;
	_current_right_indent = 0;
	_buttons_select_color = QColor(125, 170, 240, 150); // 92,134,198

	// Вначале редактор находится в обычном нераспахнутом состоянии
	_expand_edit_area_flag = false;

	update_indentline_geometry();
	// Устанавливается состояние распахнуто или нет панель инструментов
	if (_editor_config->get_expand_tools_lines())
		switch_expand_tools_lines(1);
	else
		switch_expand_tools_lines(-1);
	if (enable_random_seed) {
		// QDateTime	datetime = QDateTime::currentDateTime();
		int seed = std::chrono::system_clock::to_time_t(
		    std::chrono::system_clock::now()); // datetime.toTime_t();
		// qDebug() << "Random generator init " << seed;
		srand(seed);
		// qDebug() << "Random generator " << rand();
	}
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	// disable_tool_list(hide_editor_tools);
	qDebug() << "Editor::setDisableToolList() : " << hide_editor_tools;
	_disable_tool_list = hide_editor_tools;
}

void Editor::setup_signals(void)
{
	// Создание сигналов, генерируемых кнопками форматирования текста
	// connect(_bold, &FlatToolButton::clicked, this,
	// &Editor::on_bold_clicked);
	// connect(_italic, &FlatToolButton::clicked, this,
	// &Editor::on_italic_clicked);
	// connect(_underline, &FlatToolButton::clicked, this,
	// &Editor::on_underline_clicked);
	// connect(_monospace, &FlatToolButton::clicked, this,
	// &Editor::on_monospace_clicked);
	// connect(_code, &FlatToolButton::clicked, this,
	// &Editor::on_code_clicked);
	// connect(_clear, &FlatToolButton::clicked, this,
	// &Editor::on_clear_clicked);

	// connect(_numeric_list, &FlatToolButton::clicked, this,
	// &Editor::on_numericlist_clicked);
	// connect(_dot_list, &FlatToolButton::clicked, this,
	// &Editor::on_dotlist_clicked);

	// connect(_indent_plus, &FlatToolButton::clicked, this,
	// &Editor::on_indentplus_clicked);
	// connect(_indent_minus, &FlatToolButton::clicked, this,
	// &Editor::on_indentminus_clicked);

	// connect(_align_left, &FlatToolButton::clicked, this,
	// &Editor::on_alignleft_clicked);
	// connect(_align_center, &FlatToolButton::clicked, this,
	// &Editor::on_aligncenter_clicked);
	// connect(_align_right, &FlatToolButton::clicked, this,
	// &Editor::on_alignright_clicked);
	// connect(_align_width, &FlatToolButton::clicked, this,
	// &Editor::on_alignwidth_clicked);

	////	connect(_font_select, &FlatFontComboBox::currentFontChanged, this,
	/// &Editor::on_fontselect_changed);
	// void (FlatComboBox::*_currentIndexChanged)(int index) =
	// &FlatComboBox::currentIndexChanged;
	// connect(_font_size, _currentIndexChanged, this,
	// &Editor::on_fontsize_changed);
	// connect(_font_color, &FlatToolButton::clicked, this,
	// &Editor::on_fontcolor_clicked);

	// connect(_show_html, &FlatToolButton::clicked, this,
	// &Editor::on_showhtml_clicked);
	// connect(_find_text, &FlatToolButton::clicked, this,
	// &Editor::on_findtext_clicked);
	// connect(_settings, &FlatToolButton::clicked, this,
	// &Editor::on_settings_clicked);
	// connect(_show_formatting, &FlatToolButton::clicked, this,
	// &Editor::on_showformatting_clicked);

	// абота с таблицами
	// connect(_create_table, &FlatToolButton::clicked, this,
	// &Editor::on_createtable_clicked);
	// connect(_table_remove_row, &FlatToolButton::clicked, this,
	// &Editor::on_table_remove_row_clicked);
	// connect(_table_remove_col, &FlatToolButton::clicked, this,
	// &Editor::on_table_remove_col_clicked);
	// connect(_table_add_row, &FlatToolButton::clicked, this,
	// &Editor::on_table_add_row_clicked);
	// connect(_table_add_col, &FlatToolButton::clicked, this,
	// &Editor::on_table_add_col_clicked);
	// connect(_table_merge_cells, &FlatToolButton::clicked, this,
	// &Editor::on_table_merge_cells_clicked);
	// connect(_table_split_cell, &FlatToolButton::clicked, this,
	// &Editor::on_table_split_cell_clicked);

	// connect(_insert_image_from_file, &FlatToolButton::clicked, this,
	// &Editor::on_insert_image_from_file_clicked);
	// connect(_expand_edit_area, &FlatToolButton::clicked, this,
	// &Editor::on_expand_edit_area_clicked);
	// connect(_expand_tools_lines, &FlatToolButton::clicked, this,
	// &Editor::on_expand_tools_lines_clicked);
	// connect(_save, &FlatToolButton::clicked, this,
	// &Editor::on_save_clicked);
	// connect(_back, &FlatToolButton::clicked, this,
	// &Editor::on_back_clicked);
	// connect(_freeze, &FlatToolButton::clicked, this,
	// &Editor::on_freeze_clicked);

	// connect(_find_in_base, &FlatToolButton::clicked, this,
	// &Editor::on_find_in_base_clicked);
	// connect(_show_text, &FlatToolButton::clicked, this,
	// &Editor::on_show_text_clicked);
	// connect(_to_attach, &FlatToolButton::clicked, this,
	// &Editor::on_to_attach_clicked);
	connect(_text_area, &EditorTextArea::textChanged, [&] { _blogger->_entry_ever_saved = false; });
	connect(_text_area, &EditorTextArea::cursorPositionChanged, this, &Editor::on_cursor_position_changed);
	connect(_text_area, &EditorTextArea::selectionChanged, this, &Editor::on_selection_changed);

	connect(_indent_slider, &IndentSlider::change_textindent_pos, this, &Editor::on_indentline_change_textindent_pos);
	connect(_indent_slider, &IndentSlider::change_leftindent_pos, this, &Editor::on_indentline_change_leftindent_pos);
	connect(_indent_slider, &IndentSlider::change_rightindent_pos, this, &Editor::on_indentline_change_rightindent_pos);
	connect(_indent_slider, &IndentSlider::mouse_release, this, &Editor::on_indentline_mouse_release);

	connect(this, &Editor::send_set_textindent_pos, _indent_slider, &IndentSlider::set_textindent_pos);
	connect(this, &Editor::send_set_leftindent_pos, _indent_slider, &IndentSlider::set_leftindent_pos);
	connect(this, &Editor::send_set_rightindent_pos, _indent_slider, &IndentSlider::set_rightindent_pos);

	// Соединение сигнал-слот чтобы показать контекстное меню по правому клику в
	// редакторе
	connect(_text_area, &EditorTextArea::customContextMenuRequested, this, &Editor::on_custom_contextmenu_requested);

	// Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
	connect(_text_area, &EditorTextArea::tap_and_hold_gesture_finished, this, &Editor::on_custom_contextmenu_requested);

	// connect(textArea->document(), SIGNAL(modificationChanged (bool )),
	// this, &Editor::(onModificationChanged(bool)));

	// moved to SuperMenu
	//// Сигналы контекстного меню
	// connect(_editor_context_menu, &EditorContextMenu::undo, this,
	// &Editor::on_undo);
	// connect(_editor_context_menu, &EditorContextMenu::redo, this,
	// &Editor::on_redo);
	// connect(_editor_context_menu, &EditorContextMenu::cut, this,
	// &Editor::cut);
	// connect(_editor_context_menu, &EditorContextMenu::copy, this,
	// &Editor::copy);
	// connect(_editor_context_menu, &EditorContextMenu::paste, this,
	// &Editor::paste);
	// connect(_editor_context_menu, &EditorContextMenu::selectAll, this,
	// &Editor::on_selectAll);
	// connect(_editor_context_menu,
	// &EditorContextMenu::contextMenuEditImageProperties, this,
	// &Editor::on_context_menu_edit_image_properties);

	// Вызов диалога поиска в тексте
	connect(_find_dialog.get(), &EditorFindDialog::find_text, this, &Editor::on_findtext_signal_detect);

	connect(_text_area, &EditorTextArea::update_indentline_geometry_signal, this, &Editor::on_update_indentline_geometry_slot);
	// connect(this->_close_button, &FlatToolButton::clicked, this,
	// &Editor::widget_hide);

	// Соединение сигнал-слот чтобы показать контекстное меню по правому клику на
	// ветке
	connect(this, &Editor::customContextMenuRequested, this, &Editor::on_custom_contextmenu_requested);

	//// Соединение сигнал-слот чтобы показать контекстное меню по долгому
	// нажатию
	// connect(this, &Editor::tapandhold_gesture_finished, this,
	// &Editor::on_custom_contextmenu_requested);

	// connect(_editor_screen, &MetaEditor::send_expand_edit_area, this,
	// &wn_t::on_expand_edit_area);
	connect(this, &Editor::send_expand_edit_area, gl_paras->main_window(), &wn_t::on_expand_edit_area);

	// connect(_editor_screen, &MetaEditor::wyedit_find_in_base_clicked,
	// globalparameters.window_switcher(), &WindowSwitcher::find_in_base_click);
	connect(this, &Editor::wyedit_find_in_base_clicked, gl_paras->window_switcher(), &WindowSwitcher::find_in_base_click);
}

// Создание объектов кнопок для линейки форматирования текста
void Editor::setup_buttons(void)
{
	// Для того, чтобы WyEdit нормально добавлял кнопки на панель согласно файлу
	// editorconf.ini,
	// имена объектов кнопок должны начинаться на "editor_tb"

	// To WyEdit normally add buttons to the panel according to the file
	// editorconf.ini,
	// Object names must begin at the buttons "editor_tb"

	// Кнопка Bold
	_bold = add_action(_tools_line_0, this, tr((editor_prefix + "bold").c_str()), tr("Bold (Ctrl+B)"), tr("Bold (Ctrl+B)"), QIcon(":/resource/pic/edit_bold.svg"), [&](bool) { on_bold_clicked(); }, QKeySequence(QKeySequence::Bold));

	// Кнопка Italic
	_italic = add_action(
	    _tools_line_0, this, tr((editor_prefix + "italic").c_str()), tr("Italic (Ctrl+I)"), tr("Italic (Ctrl+I)"), QIcon(":/resource/pic/edit_italic.svg"), [&](bool) { on_italic_clicked(); }, QKeySequence(QKeySequence::Italic));

	// Кнопка Underline
	_underline =
	    add_action(_tools_line_0, this, tr((editor_prefix + "underline").c_str()), tr("Underline (Ctrl+U)"), tr("Underline (Ctrl+U)"), QIcon(":/resource/pic/edit_underline.svg"), [&](bool) { on_underline_clicked(); }, QKeySequence(QKeySequence::Underline));

	// action_Bold
	// = add_action(_tools_line_0, this, tr(std::string(editor_prefix +
	// "Bold").c_str())
	// , tr("Bold"), tr("Make the selected text bold")
	// , QIcon(QPixmap(mini_bold_xpm))
	// , [&](bool){_blog_editor->makeBold();});
	// action_Italic
	// = add_action(_tools_line_0, this, tr(std::string(editor_prefix +
	// "Italic").c_str())
	// , tr("Italic"), tr("Italicise the selected text")
	// , QIcon(QPixmap(mini_ital_xpm))
	// , [&](bool){_blog_editor->makeItalic();});
	// action_Underline
	// = add_action(_tools_line_0, this, tr(std::string(editor_prefix +
	// "Underline").c_str())
	// , tr("Underline"), tr("Underline the selected text")
	// , QIcon(QPixmap(underline_xpm))
	// , [&](bool){_blog_editor->makeUnderline();});

	// Кнопка Monospace
	_monospace = add_action(
	    _tools_line_0, this, tr((editor_prefix + "monospace").c_str()), tr("Monospace (Ctrl+T+T)"), tr("Monospace (Ctrl+T+T)"), QIcon(":/resource/pic/edit_monospace.svg"), [&](bool) { on_monospace_clicked(); }, QKeySequence(tr("Ctrl+T+T")));

	// Кнопка Code
	_code = add_action(
	    _tools_line_0, this, tr((editor_prefix + "code").c_str()), tr("Code (Ctrl+M). Select a whole paragraphs to format text as code."), tr("Code (Ctrl+M). Select a whole paragraphs to format text as code."), QIcon(":/resource/pic/edit_code.svg"), [&](bool) { on_code_clicked(); }, QKeySequence(tr("Ctrl+M")));
	// action_Blockquote
	// = add_action(_tools_line_0, this, tr(std::string(editor_prefix +
	// "Blockquote").c_str())
	// , tr("Blockquote")
	// , tr("Mark the selected text as a block of quoted text")
	// , QIcon(QPixmap(bquote_xpm))
	// , [&](bool){_blog_editor->makeBlockquote();});

	// Кнопка Clear
	_clear =
	    add_action(_tools_line_0, this, tr((editor_prefix + "clear").c_str()), tr("Reset format (Ctrl+K). When selected whole paragraph both "
										      "text and paragraph format is reset to default or just "
										      "text format in other case."),
		tr("Reset format (Ctrl+K). When selected whole paragraph both "
		   "text and paragraph format is reset to default or just "
		   "text format in other case."),
		QIcon(":/resource/pic/edit_clear.svg"), [&](bool) { on_clear_clicked(); }, QKeySequence(tr("Ctrl+K")));

	// Кнопка нумерованного списка
	_numeric_list = add_action(_tools_line_0, this, tr((editor_prefix + "numericlist").c_str()), tr("Numeric list"), tr("Numeric list"), QIcon(":/resource/pic/edit_listnumeric.svg"), [&](bool) { on_numericlist_clicked(); });

	// Кнопка списка с точками
	_dot_list =
	    add_action(_tools_line_0, this, tr((editor_prefix + "dotlist").c_str()), tr("Marked list"), tr("Marked list"), QIcon(":/resource/pic/edit_listdot.svg"), [&](bool) { on_dotlist_clicked(); });

	// Кнопка увеличения отступа
	_indent_plus = add_action(_tools_line_0, this, tr((editor_prefix + "indentplus").c_str()), tr("Increase indent"), tr("Increase indent"), QIcon(":/resource/pic/edit_indentplus.svg"), [&](bool) { on_indentplus_clicked(); });

	// Кнопка уменьшения отступа
	_indent_minus = add_action(_tools_line_0, this, tr((editor_prefix + "indentminus").c_str()), tr("Decrease indent"), tr("Decrease indent"), QIcon(":/resource/pic/edit_indentminus.svg"), [&](bool) { on_indentminus_clicked(); });

	// Кнопка выравнивания по левому краю
	_align_left = add_action(
	    _tools_line_0, this, tr((editor_prefix + "alignleft").c_str()), tr("Align left (Ctrl+L)"), tr("Align left (Ctrl+L)"), QIcon(":/resource/pic/edit_alignleft.svg"), [&](bool) { on_alignleft_clicked(); }, QKeySequence(tr("Ctrl+L")));

	// Кнопка выравнивания по центру
	_align_center = add_action(
	    _tools_line_0, this, tr((editor_prefix + "aligncenter").c_str()), tr("Align center (Ctrl+E)"), tr("Align center (Ctrl+E)"), QIcon(":/resource/pic/edit_aligncenter.svg"), [&](bool) { on_aligncenter_clicked(); }, QKeySequence(tr("Ctrl+E")));

	// Кнопка выравнивания по правому краю
	_align_right = add_action(
	    _tools_line_0, this, tr((editor_prefix + "alignright").c_str()), tr("Align right (Ctrl+R)"), tr("Align right (Ctrl+R)"), QIcon(":/resource/pic/edit_alignright.svg"), [&](bool) { on_alignright_clicked(); }, QKeySequence(tr("Ctrl+R")));

	// Кнопка выравнивания по ширине
	_align_width = add_action(
	    _tools_line_0, this, tr((editor_prefix + "alignwidth").c_str()), tr("Align width (Ctrl+J)"), tr("Align width (Ctrl+J)"), QIcon(":/resource/pic/edit_alignwidth.svg"), [&](bool) { on_alignwidth_clicked(); }, QKeySequence(tr("Ctrl+J")));

	// move from EditingWindow

	_action_open = add_action(
	    _tools_line_0, this, tr(std::string(editor_prefix + "Open ...").c_str()), tr("Open ..."), tr("Load a saved entry from the disk"), QIcon(":/resource/pic/fileopen.svg"), [&](bool) { _blogger->choose(); });
	_action_save =
	    add_action(_tools_line_0, this, tr(std::string(editor_prefix + "Save").c_str()), tr("Save"), tr("Save this entry to disk for editing or publication later"), QIcon(":/resource/pic/save.svg"), [&](bool) { _blogger->save_text_context(); });

	_action_link = add_action(
	    _tools_line_0, this, tr(std::string(editor_prefix + "Link").c_str()), tr("Link"), tr("Inserts a link to another page, the location to be specified now"), QIcon(":/resource/pic/link.svg") // QPixmap(linkIcon_xpm)
	    ,
	    [&](bool isAutoLink) { _blogger->insertLink(isAutoLink); }
	    //	    _blogger, &Blogger::insertLink
	    );
	_action_image = add_action(
	    _tools_line_0, this, tr(std::string(editor_prefix + "Image").c_str()), tr("Image"), tr(std::string(std::string("Insert an image into the text, the location, "
															   "alignment and alternate \n") +
												       "text to be specified now")
													.c_str()),
	    QIcon(":/resource/pic/flip.svg") // QPixmap(imgIcon_xpm)
	    ,
	    [&](bool) { _blogger->insertImage(); });
	_action_more =
	    add_action(_tools_line_0, this, tr(std::string(editor_prefix + "More").c_str()), tr("More"), tr((std::string("Set this as the end of the main entry (which "
															 "appears on the index) \n") +
														"and the beginning of the extended entry (which appears " + "when you click More, \n" + "Extended entry or view the entry on its own page).")
														 .c_str()),
		QIcon(":/resource/pic/more.svg") // QPixmap(more_xpm)
		,
		[&](bool) { _blogger->insertMore(); });
	_action_preview =
	    add_action(_tools_line_0, this, tr(std::string(editor_prefix + "Entry in preview").c_str()), tr("Entry in preview"), tr("Preview the entry in this window"), QIcon(":/resource/pic/preview.svg") // QPixmap(previewIcon_xpm)
		,
		[&](bool checked) { _blogger->doPreview(checked); });
	_action_preview->setCheckable(true);
	_action_preview->setChecked(false);
	_action_blog_this = add_action(
	    _tools_line_0, this, tr(std::string(editor_prefix + "Blog this!").c_str()), tr("Blog this!"), tr("Post this entry to the blog"), QIcon(
																		 ":/resource/pic/pull.svg") // pentalpha.svg//QPixmap(blogThisIcon_xpm)
	    ,
	    [&](bool) { _blogger->blogThis(); });

	_tools_line_0->addSeparator();
	_tools_line_0->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	// temporary comment out
	// _tools_line_0->adjustSize();     // without this, without display
	//

	_expand_tools_lines = add_action(
	    _tools_line_0, this, tr((editor_prefix + "expand_tools_lines").c_str()), tr("Expand tools"), tr("Expand tools"), QIcon(":/resource/pic/edit_expand_tools_lines.svg"), [&](bool) { on_expand_tools_lines_clicked(); });

	// Кнопка переключения на аттачи
	// _icon_attach_not_exists	=  QIcon(":/resource/pic/attach.svg");
	// _icon_attach_exists		=
	// QIcon(":/resource/pic/attach_exists.svg");
	_to_attach = add_action(
	    _tools_line_0, this, tr((editor_prefix + "attach").c_str()), tr("Show attach files"), tr("Show attach files"), _icon_attach_not_exists, [&](bool) { _blogger->to_attach_layout(); });

	_insert_image_from_file = add_action(
	    _tools_line_0, this, tr((editor_prefix + "insert_image_from_file").c_str()), tr("Insert image from file / edit image properties of selected image"), tr("Insert image from file / edit image properties of selected image"), QIcon(":/resource/pic/edit_insert_image_from_file.svg"), [&](bool) { on_insert_image_from_file_clicked(); });

	_expand_edit_area = add_action(
	    _tools_line_0, this, tr((editor_prefix + "expand_edit_area").c_str()), tr("Expand edit area"), tr("Expand edit area"), QIcon(":/resource/pic/edit_expand_text_area.svg"), [&](bool) { on_expand_edit_area_clicked(); });

	_save =
	    add_action(_tools_line_0, this, tr((editor_prefix + "save").c_str()), tr("Save (Ctrl+S)"), tr("Save (Ctrl+S)"), QIcon(":/resource/pic/edit_save.svg"), [&](bool) { on_save_clicked(); }, QKeySequence(tr("Ctrl+S")));

	// Кнопка "назад", используется в мобильном интерфейсе
	// Button "back", used in the mobile interface
	_back = add_action(_tools_line_0, this, tr((editor_prefix + "back").c_str()), tr("Back"), tr("Back"), QIcon(":/resource/pic/mobile_back.svg"), [&](bool) { WindowSwitcher::record_to_record_screen(); });

	// Кнопка "поиск по базе", используется в мобильном интерфейсе
	// Button "search database" used in the mobile interface
	_find_in_base = add_action(_tools_line_0, this, tr((editor_prefix + "find_in_base").c_str()), tr("Find in base"), tr("Find in base"), QIcon(":/resource/pic/find_in_base.svg"), [&](bool) { on_find_in_base_clicked(); });
	// Кнопка вызова виджета поиска текста
	_find_text = add_action(
	    _tools_line_0, this, tr((editor_prefix + "findtext").c_str()), tr("Find text (Ctrl+Shift+F)"), tr("Find text (Ctrl+Shift+F)"), QIcon(":/resource/pic/edit_findtext.svg"), [&](bool) { on_findtext_clicked(); }, QKeySequence(tr("Ctrl+Shift+F")));

	// Кнопка вызова виджета конфигурирования редактора
	_settings =
	    add_action(_tools_line_0, this, tr((editor_prefix + "settings").c_str()), tr("Editor settings"), tr("Editor settings"), QIcon(":/resource/pic/cogwheel.svg"), [&](bool) { on_settings_clicked(); });

	// Кнопка просмотра HTML кода
	_show_html =
	    add_action(_tools_line_0, this, tr((editor_prefix + "showhtml").c_str()), tr("Edit HTML code"), tr("Edit HTML code"), QIcon(":/resource/pic/edit_showhtml.svg"), [&](bool) { on_showhtml_clicked(); });

	// Кнопка включения отображения символов фарматирования
	_show_formatting = add_action(
	    _tools_line_0, this, tr((editor_prefix + "showformatting").c_str()), tr("Show special chars"), tr("Show special chars"), QIcon(":/resource/pic/edit_showformatting.svg"), [&](bool) { on_showformatting_clicked(); });

	// Кнопка "показать текст" для просмотра текста в отдельном окне
	_show_text = add_action(
	    _tools_line_0, this, tr((editor_prefix + "show_text").c_str()), tr("Show text in detached window"), tr("Show text in detached window"), QIcon(":/resource/pic/edit_show_text.svg"), [&](bool) { on_show_text_clicked(); });

	//
	// Выбор шрифта
	_font_select = add_action(
	    _tools_line_1, this, tr((editor_prefix + "fontselect").c_str()), tr("Select font type"), tr("Select font type"), QIcon(":/resource/pic/edit_font_many.svg"), (FlatFontComboBox*)(nullptr), this, &Editor::on_fontselect_changed);

	// Выбор размера шрифта
	_font_size =
	    add_action(_tools_line_1, this, tr((editor_prefix + "fontsize").c_str()), tr("Select font size"), tr("Select font size"), QIcon(), (FlatComboBox*)(nullptr), this, &Editor::on_fontsize_changed);
	// _font_size = new FlatComboBox(this);
	for (int i = MINIMUM_ALLOWED_FONT_SIZE; i <= MAXIMUM_ALLOWED_FONT_SIZE; i++) _font_size->addItem(QString("%1").arg(i), i);
	_font_size->setCurrentIndex(_font_size->findData(10));
	_font_size->setMinimumContentsLength(3);
	_font_size->setEditable(true);

	QValidator* fontsize_validator = new QIntValidator(
	    MINIMUM_ALLOWED_FONT_SIZE, MAXIMUM_ALLOWED_FONT_SIZE, this);
	_font_size->setValidator(fontsize_validator);
	// _font_size->setObjectName("editor_tb_fontsize");

	//// Пустой пункт в конце списка размеров шрифтов, используется для
	// обозначения
	//// что в выделенном тексте несколько размеров
	// An empty item at the end of the font size of the list, is used to denote
	// That the selected text in several sizes
	// _font_size->addItem(" ", 0);

	// Кнопка выбора цвета шрифта
	_font_color =
	    add_action(_tools_line_1, this, tr((editor_prefix + "fontcolor").c_str()), tr("Text color"), tr("Text color"), QIcon(":/resource/pic/edit_fontcolor.svg"), [&](bool) { on_fontcolor_clicked(); });

	// Кнопка добавления новой таблицы
	_create_table = add_action(_tools_line_1, this, tr((editor_prefix + "createtable").c_str()), tr("Create a new table"), tr("Create a new table"), QIcon(":/resource/pic/edit_createtable.svg"), [&](bool) { on_createtable_clicked(); });

	_table_remove_row = add_action(
	    _tools_line_1, this, tr((editor_prefix + "table_remove_row").c_str()), tr("Remove row(s)"), tr("Remove row(s)"), QIcon(":/resource/pic/edit_table_remove_row.svg"), [&](bool) { on_table_remove_row_clicked(); });

	_table_remove_col = add_action(
	    _tools_line_1, this, tr((editor_prefix + "table_remove_col").c_str()), tr("Remove column(s)"), tr("Remove column(s)"), QIcon(":/resource/pic/edit_table_remove_col.svg"), [&](bool) { on_table_remove_col_clicked(); });

	_table_add_row = add_action(_tools_line_1, this, tr((editor_prefix + "table_add_row").c_str()), tr("Add row(s)"), tr("Add row(s)"), QIcon(":/resource/pic/edit_table_add_row.svg"), [&](bool) { on_table_add_row_clicked(); });

	_table_add_col = add_action(_tools_line_1, this, tr((editor_prefix + "table_add_col").c_str()), tr("Add column(s)"), tr("Add column(s)"), QIcon(":/resource/pic/edit_table_add_col.svg"), [&](bool) { on_table_add_col_clicked(); });

	_table_merge_cells = add_action(
	    _tools_line_1, this, tr((editor_prefix + "table_merge_cells").c_str()), tr("Merge cells"), tr("Merge cells"), QIcon(":/resource/pic/edit_table_merge_cells.svg"), [&](bool) { on_table_merge_cells_clicked(); });

	_table_split_cell = add_action(
	    _tools_line_1, this, tr((editor_prefix + "table_split_cell").c_str()), tr("Split cell"), tr("Split cell"), QIcon(":/resource/pic/edit_table_split_cell.svg"), [&](bool) { on_table_split_cell_clicked(); });

	// Виджет настройки отступов
	_indent_slider = new IndentSlider(this->width(), 16, this);
	_indent_slider->setObjectName("editor_tb_indentslider");

	// Все только что созданные элементы скрываются
	hide_all_tools_elements();
}

void Editor::setup_closebutton(void)
{
	// Кнопка закрытия виджета
	_close_button = new FlatToolButton("", this);
	_close_button->setVisible(true);
	_close_button->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton)); // SP_TitleBarCloseButton
	// SP_DialogCloseButton
	// _close_button->setAutoRaise(true);
	//	connect(_close_button, &FlatToolButton::clicked
	//	       , [&](bool checked){
	//			(void) checked;
	//			gl_paras->editor_docker()->hide();
	//			appconfig->editor_show(false);
	//		});
	if (appconfig->interface_mode() == "desktop") {
		int w = _close_button->geometry().width();
		int h = _close_button->geometry().height();
		int x = min(w, h); /// 2; // imin(w, h) / 2;
		_close_button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
		_close_button->setMinimumSize(x, x);
		_close_button->setMaximumSize(x, x);
		_close_button->resize(x, x);
	}
}

void Editor::assembly_closebutton(void)
{
	// Вертикальная область с кнопкой закрытия и распоркой
	// чтобы кнопка была вверху
	_toolsarea_of_close_button = new QVBoxLayout();
	// _toolsarea_of_close_button->setContentsMargins(0, 0, 0, 0);
	_toolsarea_of_close_button->addWidget(_close_button);
	_toolsarea_of_close_button->addStretch();
}

//// Полное сокрытие виджета
// void Editor::widget_hide(bool checked){
// Q_UNUSED(checked);
////    // Запоминается размер сплиттера перед скрытием виджета
////    QSplitter *findSplitterRel =
// find_object<QSplitter>("find_splitter");
////    appconfig->set_findsplitter_size_list(findSplitterRel->sizes());

//// Виджет скрывается
// appconfig->editor_show(false);
// this->close();
// }

void Editor::hide_all_tools_elements(void)
{
	QRegExp nameMask("editor_tb_.*");

	// QList<QWidget *> tb_tools_list=qFindChildren(qobject_cast<QObject
	// *>(this),name_mask);
	QList<QWidget*> tbToolsList = this->findChildren<QWidget*>(nameMask);
	for (int i = 0; i < tbToolsList.size(); ++i) tbToolsList.at(i)->setVisible(false);
}

// Создание объекта области редактирования
void Editor::setup_editor_area(void)
{
	// Создается область редактирования
	// _text_area = new EditorTextArea(this);
	_text_area->setObjectName("textArea");
	_text_area->setContextMenuPolicy(
	    Qt::CustomContextMenu); // Меню определяется в программе

	QFont font;
	font.fromString(_editor_config->get_default_font());
	font.setPointSize(_editor_config->get_default_font_size());
	_text_area->selectAll();
	_text_area->setCurrentFont(font);
	_text_area->setFont(font);
}

#ifdef ADD_ACTIONS_BY_NAME
// В линейку кнопок вставляется нужный инструмент
// Метод принимает имя инструмента
// и указатель на линейку
void Editor::add_action_by_name(QString toolName, QToolBar* line)
{
	// qDebug() << "Editor::insert_button_to_tools_line() disableToolList : " <<
	// disableToolList;
	if (toolName == "separator")
		line->addSeparator();
	else {
		QString name("editor_tb_" + toolName);
		QWidget* tool = this->findChild<QWidget*>(name);
		if (tool != nullptr) {
			// Если данный инструмент не содержится в списке заблокированных
			if (!_disable_tool_list.contains(toolName)) {
				tool->setVisible(true);

				line->addWidget(tool); // Инструмент добавляется на панель инструментов

				FlatToolButton* tb = qobject_cast<FlatToolButton*>(tool);
				if (tb != 0) tb->setAutoRaise(true); // false
			} else {
				FlatToolButton* tb = qobject_cast<FlatToolButton*>(tool);
				if (tb != 0) tb->setEnabled(false);
			}
		} else
			critical_error("WyEdit: Can not find editor tool with name '" + toolName + "'. Please check editor *.ini file");
	}
}
#endif

// Сборка линейки редактирования текста в горизонтальную линейку
void Editor::assembly_buttons(void)
{
//// Заполнение горизонтальных линеек с кнопками форматирования текста
// _tools_line_0	= new QToolBar();
// _tools_line_1	= new QToolBar();
#ifdef ADD_ACTIONS_BY_NAME
	add_actions_by_name();
#endif

	// QSize _tool_icon_size(16, 16);
	// _tools_line_0->setIconSize(_tool_icon_size);
	// _tools_line_0->setIconSize(_tool_icon_size);

	QHBoxLayout* tools_line_0_layout = new QHBoxLayout();
	tools_line_0_layout->addWidget(_tools_line_0);
	tools_line_0_layout->addStretch();
	// toolsLine1Layout->addWidget(_close_button);//
	tools_line_0_layout->addLayout(_toolsarea_of_close_button);

	// Горизонтальные линейки собираются
	_textformat_buttons_layout = new QVBoxLayout();

	// textformatButtonsLayout->addWidget(toolsLine1);
	_textformat_buttons_layout->addLayout(tools_line_0_layout);
	QHBoxLayout* tools_line_1_layout = new QHBoxLayout();
	tools_line_1_layout->addWidget(_tools_line_1);
	// _textformat_buttons_layout->addWidget(_tools_line_1);
	_textformat_buttons_layout->addLayout(tools_line_1_layout);
	// Виджет настройки отступов виден только в desktop интерфейсе
	if (_view_mode == WYEDIT_DESKTOP_MODE) {
		_indent_slider->setVisible(true);
		_textformat_buttons_layout->addWidget(_indent_slider);
	} else
		_indent_slider->setVisible(false);
}
#ifdef ADD_ACTIONS_BY_NAME
void Editor::add_actions_by_name(void)
{
	for (int i = 0; i < _tools_name_list_in_line_0.size(); ++i) {
		QString action_name = _tools_name_list_in_line_0.at(i).trimmed();
		add_action_by_name(action_name, _tools_line_0);
	}
	for (int i = 0; i < _tools_name_list_in_line_1.size(); ++i) {
		QString action_name = _tools_name_list_in_line_1.at(i).trimmed();
		add_action_by_name(action_name, _tools_line_1);
	}
}
#endif
// Сборка редактора и его активизация
void Editor::assembly(void)
{
	// Создается вертикальная область, которая должна объединить
	// линейку кнопок редактирования и область редактирования текста
	full_layout = new QVBoxLayout(this);
	full_layout->setObjectName("buttons_and_edit_layout");
	QHBoxLayout* tool_bar_layout = new QHBoxLayout();
	tool_bar_layout->addLayout(_textformat_buttons_layout);
	tool_bar_layout->setMargin(0);
	tool_bar_layout->setContentsMargins(0, 0, 0, 0);
	// Добавляются линейки кнопок
	full_layout->addLayout(tool_bar_layout); //_textformat_buttons_layout
	QHBoxLayout* text_layout = new QHBoxLayout();
	text_layout->addWidget(_text_area);
	// Добавляется область редактирования
	full_layout->addLayout(text_layout); // addWidget(_text_area);// reconfiged by
					     // MetaEditor::assembly
	full_layout->setStretch(1, 1);
	// Полученый набор элементов устанавливается для текущего окна
	setLayout(full_layout);

	// Границы убираются, так как данный объект будет использоваться как виджет
	QLayout* lt;
	lt = layout();
	lt->setMargin(0);
	lt->setContentsMargins(0, 0, 0, 0); // setContentsMargins(0, 2, 0, 0);
}

void Editor::on_update_indentline_geometry_slot()
{
	update_indentline_geometry();
}

// Синхронизация размеров линейки отступов относительно области редактирования
void Editor::update_indentline_geometry()
{
	// Синхронизируется ширина виджета линейки отступов
	_indent_slider->set_widget_width(_text_area->width());

	// Синхронизируется геометрия линейки отступов
	int leftPos = _text_area->get_indent_started_left();
	int rightPos = _text_area->get_indent_started_right();
	_indent_slider->set_indentline_left_pos(leftPos);
	_indent_slider->set_indentline_right_pos(rightPos);

	_indent_slider->update();
}

// Set the text of the editing area// Установка текста области редактирования
void Editor::textarea(QString text)
{
	_text_area->setHtml(text);

	// Очищается URL документа, так как документ создается "из ничего"
	_text_area->document()->setMetaInformation(QTextDocument::DocumentUrl, "");
}

// Установка запрета или разрешения редактирования
void Editor::textarea_editable(bool editable)
{
	if (editable == true) {
		// Если редактирование разрешено
		_text_area->setTextInteractionFlags(Qt::TextEditorInteraction);
		_text_area->setPalette(sapp_t::instance()->palette());
	} else {
		// Если редактирование запрещено
		_text_area->setTextInteractionFlags(Qt::TextBrowserInteraction);

		QPalette pal = sapp_t::instance()->palette();
		QColor inactiveColor;

		inactiveColor = sapp_t::instance()->palette().color(QPalette::Disabled, QPalette::Window);
		pal.setColor(QPalette::Normal, QPalette::Base, inactiveColor);
		_text_area->setPalette(pal);

		// QPalette plt=standart_style->standardPalette();
	}
}

// Получение текста области редактирования в формате HTML
QString Editor::textarea(void)
{
	return _text_area->document()->toHtml("UTF-8");
}

QTextCursor Editor::textCursor() const
{
	return _text_area->textCursor();
}
void Editor::setTextCursor(const QTextCursor& cursor)
{
	_text_area->setTextCursor(cursor);
}
void Editor::insertPlainText(const QString& text)
{
	_text_area->insertPlainText(text);
}
void Editor::setPlainText(const QString& text)
{
	_text_area->setPlainText(text);
}
QString Editor::toPlainText() const
{
	return _text_area->toPlainText();
}
void Editor::ensureCursorVisible() const
{
	_text_area->ensureCursorVisible();
}

QTextDocument* Editor::document(void)
{
	return _text_area->document();
}

bool Editor::work_directory(const QString& dir_name, const QString& file_name_)
{
	bool result = false;
	auto original_dir = _work_directory;
	QDir directory(dir_name);
	// QDir directory_child(dir_name);
	if (directory.exists()) {
		_work_directory = dir_name;
		result = true;
	} else {
		QString child_dir = directory.dirName();
		QString path = directory.absolutePath();
		path.remove(child_dir);
		directory.setPath(path);
		// directory.rmdir(child_dir);// globalparameters.main_program_file()
		// + get_unical_id();
		result = directory.mkdir(child_dir); // _work_directory // short_dir()
		if (!result)
			critical_error(
			    "Editor::work_directory(QString dir_name) : Can't create directory '" + _work_directory + "'");
		_work_directory = dir_name;
		//		// critical_error("WyEdit: Can not set work directory to " + dirName + ". Directory not exists.");
		//		return result; // false;
	}
	auto file_name = [&](QString file_name_) {
		// QString fileName = full_text_file_name();
		if (file_name_.size() > 0) {
			QFile f(file_name_);
			QFileInfo fileInfo(f);
			if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::ReadOnly))
				critical_error("Editor::file_name(QString _file_name) : Can\'t open text file " + file_name_ + " for read / write.");
			if (fileInfo.absoluteDir().exists() && f.exists()) _work_file_name = file_name_;
		} else
			_work_file_name = "";
	};

	file_name(file_name_);
	if (original_dir != dir_name) load_textarea();
	return result;
}

QString Editor::work_directory(void) const
{
	return _work_directory;
}


QString Editor::file_name(void) const
{
	return _work_file_name;
}

// Function that replaces the standard function for reading Editor
// Editable text
// She is editor of passing a pointer to itself
// And variable reference loadText, which must be filled
// Attention! The method does not contain the data recording operation. Think
// about where to place it
void Editor::editor_load_callback(QObject* editor, QString& load_text)
{
	// qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir <<
	// "File" << file;

	// Ссылка на объект редактора
	Editor* curr_editor = qobject_cast<Editor*>(editor);

	// Нужно ли дешифровать данные при чтении
	bool work_with_crypt = false;
	if (curr_editor->misc_field<crypt_key>() == crypt_value(true)) {
		// Если не установлено ключа шифрации
		if (gl_paras->crypt_key().length() == 0) {
			load_text = "";

			return;
		}
		work_with_crypt = true;
	}
	// Файл, с которым работает редактор
	QString file_name = curr_editor->work_directory() + "/" + curr_editor->file_name();

	QFile f(file_name);
	if (f.exists() && f.open(QIODevice::ReadOnly)) {
		//// Если нужный файл не существует
		// if(!f.exists())
		// critical_error("File " + fileName + " not found");
		//// Открывается файл
		// if(!f.open(QIODevice::ReadOnly))
		// critical_error("File " + fileName + " not readable. Check
		// permission.");
		// Если незашифровано
		if (work_with_crypt == false)
			load_text = QString::fromUtf8(f.readAll());
		else
			load_text = CryptService::decryptStringFromByteArray(
			    gl_paras->crypt_key(), f.readAll()); // Если зашифровано
	} else
		load_text = "";
}

// Function that replaces the standard function of the editor to set down
// Editable text
// She is editor of passing a pointer to itself
// And the text to be written in the variable saveText
// Attention! The method does not contain the data recording operation. Think
// about where to place it
void Editor::editor_save_callback(QObject* editor, const QString& save_text)
{
	// qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir <<
	// "File" << file;

	// Ссылка на объект редактора
	Editor* curr_editor = qobject_cast<Editor*>(editor);

	// Нужно ли шифровать записываемый текст
	bool work_with_crypt = false;
	if (curr_editor->misc_field<crypt_key>() == crypt_value(true)) {
		// Если не установлено ключа шифрации
		if (gl_paras->crypt_key().length() == 0) return;
		work_with_crypt = true;
	}
	QString file_name =
	    curr_editor->work_directory() + "/" + curr_editor->file_name();
	// Если шифровать ненужно
	if (work_with_crypt == false) {
		// Текст сохраняется в файл
		QFile file(file_name);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) critical_error("Editor::editor_save_callback() : Can\'t open text file " + file_name + " for write.");
		QTextStream out(&file);
		out.setCodec("UTF-8");
		out << save_text;
	} else {
		// Текст шифруется
		QByteArray encryptData = CryptService::encryptStringToByteArray(gl_paras->crypt_key(), save_text);
		// В файл сохраняются зашифрованные данные
		QFile file(file_name);
		if (!file.open(QIODevice::WriteOnly))
			critical_error("Editor::editor_save_callback() : Can\'t open binary file " + file_name + " for write.");
		file.write(encryptData);
	}
	// Вызывается сохранение картинок
	// В данной реализации картинки сохраняются незашифрованными
	// Called saving images // In this embodiment, the image is not stored
	// encrypted
	curr_editor->save_textarea_images(Editor::SAVE_IMAGES_REMOVE_UNUSED);
}

// Сохранение HTML кода документа в файл
bool Editor::save_textarea_text()
{
	if (_work_file_name.length() == 0) {
		critical_error(
		    "WyEdit: Save function. Not setted work file name for editor.");

		return false;
	}
	QString fullFileName = _work_directory + "/" + _work_file_name;

	// Создание нового файла записи
	QFile file(fullFileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		critical_error("WyEdit: Cant open file " + fullFileName + " for write.");

		return false;
	}
	qDebug() << "Write to file " << _work_file_name;

	QTextStream out(&file);
	QString content = _text_area->document()->toHtml("UTF-8");
	out.setCodec("UTF-8");
	out << content;

	return true;
}

// Сохранение картинок, которые присутствуют в документе,
// в указанную директорию
bool Editor::save_textarea_images(int mode = SAVE_IMAGES_SIMPLE)
{
	qDebug() << "Save images...\n";
	qDebug() << "Block count" << _text_area->document()->blockCount() << "\n";
	if (_work_directory.length() == 0) {
		critical_error("WyEdit: Save images function. Not setted work directory.");

		return false;
	}
	// Перебираются блоки документа и находятся блоки с картинками
	QStringList imagesNames; // В список сохраняются имена найденных картинок
	QTextBlock textBlock = _text_area->document()->begin();
	while (textBlock.isValid()) {
		// QTextBlock::iterator it;
		for (auto it = textBlock.begin(); !(it.atEnd()); ++it) {
			QTextFragment currentFragment = it.fragment();
			if (currentFragment.isValid()) {
				if (currentFragment.charFormat().isImageFormat()) {
					// Найден блок с картинкой

					// Выясняется формат картинки
					QTextImageFormat imgFmt =
					    currentFragment.charFormat().toImageFormat();

					// Из формата выясняется имя картинки
					QString imageName = imgFmt.name();
					imagesNames << imageName;
					qDebug() << "Image " << imageName << "\n"; // имя файла

					// Если файла картинки не существует
					QString imageFileName = _work_directory + "/" + imageName;
					QFileInfo tryFile(imageFileName);
					if (tryFile.exists() == false) {
						qDebug() << "Save image data to file " << imageFileName;

						// Из ресурсов вытягивается картинка
						QVariant imageData = _text_area->document()->resource(
						    QTextDocument::ImageResource, QUrl(imageName));

						QImage picture = imageData.value<QImage>();

						picture.save(imageFileName, "png");
					}
				}
			}
		}
		textBlock = textBlock.next();
	}
	// Если при сохранении все лишние картинки должны удалиться в каталоге
	if (mode == SAVE_IMAGES_REMOVE_UNUSED) {
		// Выясняется список файлов в директории
		QDir directory(_work_directory);
		QStringList imageInDirectory = directory.entryList(QDir::Files);

		// Перебираются файлы в директории
		foreach (QString fileName, imageInDirectory)
			if (fileName.contains(
				QRegExp("\\.png$"))) // Обрабатыватся только *.png файлы
				if (!imagesNames.contains(fileName)) {
					// Если в списке картинок нет текущего png файла,
					// значит этот файл лишний и он удаляется
					QFile currentFile(_work_directory + "/" + fileName);
					currentFile.remove();
				}
	}
	qDebug() << "Save images finish\n";

	return true;
}

void Editor::save_textarea(void)
{
	qDebug() << "Save textarea...";
	auto dir = work_directory();
	auto fname = file_name();
	auto full_path = dir + "/" + fname;
	// Если запись была открыта на просмотр и изменена
	if (dir.length() != 0 && fname.length() != 0 && textarea_modified() == true) {
		// Перенос текущего файла записи в корзину
		qDebug() << "Try remove file " << file_name() << " from directory "
			 << work_directory();
		if (QFileInfo(full_path).exists()) {
			qDebug() << "File exists. Remove it.";
			if (0 < filesize(full_path.toStdString().c_str())) DiskHelper::backup(full_path);
		} else
			qDebug() << "Cant remove file. File not exists.";
		// Если происходит прямая работа с файлом текста
		if (load_callback_func == nullptr) {
			// Сохранение текста записи в файл
			save_textarea_text();

			// Сохранение картинок, которые есть в записи
			save_textarea_images(Editor::SAVE_IMAGES_REMOVE_UNUSED);
		} else {
			// Иначе задана функция обратного вызова для записи текста и картинок

			QString content = _text_area->document()->toHtml("UTF-8");
			save_callback_func(qobject_cast<QObject*>(this), content);
		}
		// Так как произошло сохранение,
		// отмечается что новый текст небыл еще изменен
		textarea_modified(false);
	}
}

bool Editor::load_textarea()
{
	bool result = false;
	// Если не заданы директория или файл
	// Но установлен режим подавления ошибки
	if (_work_file_name.length() == 0 || _work_directory.length() == 0)
		if (_dir_file_empty_reaction == DIRFILEEMPTY_REACTION_SUPPRESS_ERROR) {
			// Никаких действий с областью редактирования не делается

			// скорее всего она останется просто пустой
			result = true;
		}
	if (_work_file_name.length() == 0) {
		critical_error("WyEdit: Load function. Not setted work file name for editor.");

		//		return false;
	}
	if (_work_directory.length() == 0) {
		critical_error("WyEdit: Not setted work directory for editor.");

		//		return false;
	}
	QString fileName = _work_directory + "/" + _work_file_name;

	QString content;
	// Если происходит прямая работа с файлом текста
	if (load_callback_func == nullptr) {
		// Создается объект файла с нужным именем
		QFile f(fileName);
		// Если нужный файл не существует
		if (!f.exists()) {
			result = save_textarea_text();
			//			critical_error("WyEdit: File " + fileName + " not found");
			//			return false;
		}
		// Если нужный файл недоступен для чтения
		if (!f.open(QIODevice::ReadOnly)) {
			critical_error("WyEdit: File " + fileName + " not readable. Check permission.");
			result = false;
		}
		content = QString::fromUtf8(f.readAll());
	} else {
		// Иначе задана функция обратного вызова для установки начального текста
		QString resultString;
		load_callback_func(qobject_cast<QObject*>(this), resultString);
		content = resultString;
	}
	// Устанавливается URL документа, с помощью него будут высчитываться
	// относительные ссылки при загрузке картинок
	_text_area->document()->setMetaInformation(QTextDocument::DocumentUrl, "file:" + fileName);

	qDebug() << "Editor::load_textarea() Set document URL to " << fileName;
	// assert(content != "");
	// Текст из файла вставляется в область редактирования
	// QString content=QString::fromUtf8(f.readAll());
	// qDebug() << "Load content:";
	// qDebug() << content;
	_text_area->setHtml(content);

	// qDebug() << "Set content:";
	// qDebug() << textArea->toHtml();

	return result;
}

void Editor::textarea_modified(bool modify)
{
	qDebug() << "Editor::set_textarea_modified() :" << modify;
	_text_area->document()->setModified(modify);
}

bool Editor::textarea_modified(void)
{
	qDebug() << "Editor::get_textarea_modified() :"
		 << _text_area->document()->isModified();

	return _text_area->document()->isModified();
}

/////////////////////////////////////////////////
// Форматирование текста
/////////////////////////////////////////////////

// Форматирование Bold
void Editor::on_bold_clicked(void)
{
	// Если выделение есть
	if (_text_area->textCursor().hasSelection()) {
		// Обычное форматирование
		if (_text_area->fontWeight() != QFont::Bold) {
			//
			_text_area->setFontWeight(QFont::Bold); // Bold
								// qtm implementation:
								// _text_area->insertPlainText(QString("<strong>%1</strong>").arg(_text_area->textCursor().selectedText()));
		} else {
			//
			_text_area->setFontWeight(0); // Remove Bold
		}
	} else {
		// Иначе надо выделить дополнительным курсором слово на
		// котором стоит курсор
		QTextCursor cursor = _text_area->textCursor();
		cursor.select(QTextCursor::WordUnderCursor);

		// К выделению применяется форматирование Bold
		QTextCharFormat format;
		if (cursor.charFormat().fontWeight() != QFont::Bold)
			format.setFontWeight(QFont::Bold);
		else
			format.setFontWeight(0);
		cursor.mergeCharFormat(format);
	}
	update_outline_button_higlight();
}

// Форматирование Italic
void Editor::on_italic_clicked(void)
{
	// Если выделение есть
	if (_text_area->textCursor().hasSelection()) {
		// Обычное форматирование
		if (!_text_area->fontItalic()) {
			_text_area->setFontItalic(true);
			// qtm implementation:
			// _text_area->insertPlainText(QString("<em>%1</em>").arg(_text_area->textCursor().selectedText()));
		} else {
			//
			_text_area->setFontItalic(false);
		}
	} else {
		// Иначе надо выделить дополнительным курсором слово на
		// котором стоит курсор
		QTextCursor cursor = _text_area->textCursor();
		cursor.select(QTextCursor::WordUnderCursor);

		// К выделению применяется форматирование Italic
		QTextCharFormat format;
		if (!cursor.charFormat().fontItalic())
			format.setFontItalic(true);
		else
			format.setFontItalic(false);
		cursor.mergeCharFormat(format);
	}
	update_outline_button_higlight();
}

// Форматирование подчеркивания
void Editor::on_underline_clicked(void)
{
	// Если выделение есть
	if (_text_area->textCursor().hasSelection()) {
		// Обычное форматирование
		if (!_text_area->fontUnderline()) {
			_text_area->setFontUnderline(true);
			// qtm implementation:
			// _text_area->insertPlainText(QString("<u>%1</u>").arg(_text_area->textCursor().selectedText()));
		} else {
			//
			_text_area->setFontUnderline(false);
		}
	} else {
		// Иначе надо выделить дополнительным курсором слово на
		// котором стоит курсор
		QTextCursor cursor = _text_area->textCursor();
		cursor.select(QTextCursor::WordUnderCursor);

		// К выделению применяется форматирование Underline
		QTextCharFormat format;
		if (!cursor.charFormat().fontUnderline())
			format.setFontUnderline(true);
		else
			format.setFontUnderline(false);
		cursor.mergeCharFormat(format);
	}
	update_outline_button_higlight();
}

// Форматирование моноширинным шрифтом
void Editor::on_monospace_clicked(void)
{
	_text_area->textCursor().beginEditBlock();

	// Устанавливается шрифт
	QFont font;
	font.fromString(_editor_config->get_monospace_font());
	_text_area->setFontFamily(font.family());

	// Новый установленный шрифт показывается в выпадающем списке шрифтов
	set_fontselect_on_display(font.family());
	// Если в настройках разрешена автоматическая установка нужного размера
	// моноширинного шрифта
	if (_editor_config->get_monospace_font_size_apply() == true) {
		// Устанавливается нужный размер
		_text_area->setFontPointSize(_editor_config->get_monospace_font_size());

		// В выпадающем списке размеров выставляется установленный размер
		set_fontsize_on_display(_editor_config->get_monospace_font_size());
	}
	_text_area->textCursor().endEditBlock();
}

// Выбран ли блок текста (т.е. находятся ли начало и конец выделения
// точно на границах блока)
bool Editor::is_block_select(void)
{
	// Выясняются позиции начала и конца выделения
	int start = _text_area->textCursor().selectionStart();
	int stop = _text_area->textCursor().selectionEnd();

	// Высталяются флаги, находится ли начало и конец выделения на границах блока.
	// Это определяется с помощью дополнительного курсора
	int flagStartBlockEdge = 0;
	int flagStopBlockEdge = 0;
	QTextCursor cursor = _text_area->textCursor();
	cursor.setPosition(start);
	if (cursor.atBlockStart() == true || cursor.atBlockEnd() == true) flagStartBlockEdge = 1;
	cursor.setPosition(stop);
	if (cursor.atBlockStart() == true || cursor.atBlockEnd() == true) flagStopBlockEdge = 1;
	if (flagStartBlockEdge == 1 && flagStopBlockEdge == 1)
		return true;
	else
		return false;
}

// Стоит ли курсор на пустой строке (т.е. в строке есть только символ
// перевода на новую строку)
bool Editor::is_cursor_on_empty_line(void)
{
	if (!_text_area->textCursor().atBlockStart()) return false;
	// Создаётся дополнительный курсор
	QTextCursor cursor = _text_area->textCursor();

	// Выделяется анализируемая строка
	cursor.select(QTextCursor::LineUnderCursor);

	// Текст анализируемой строки
	QString selectedText = cursor.selectedText();
	if (selectedText.size() > 0)
		return false;
	else
		return true;
}

bool Editor::is_cursor_on_space_line(void)
{
	// Если есть выделение, функция работать не должна
	if (_text_area->textCursor().hasSelection()) return false;
	// Создаётся дополнительный курсор
	QTextCursor cursor = _text_area->textCursor();

	// Выделяется анализируемая строка
	cursor.select(QTextCursor::LineUnderCursor);

	// Текст анализируемой строки
	QString selectedText = cursor.selectedText();
	if (selectedText.simplified().size() > 0)
		return false;
	else
		return true;
}

// Форматирование в качестве кода
void Editor::on_code_clicked(void)
{
	// qtm implementation:
	// _text_area->insertPlainText(QString("<blockquote>%1</blockquote>").arg(_text_area->textCursor().selectedText()));
	// Если выделения нет
	if (!_text_area->textCursor().hasSelection()) return;
	bool enableIndent;
	// Проверяется, выбран ли четко блок (блоки) текста
	// Блок - это текст между <p>...</p>
	if (!is_block_select()) {
		// Если выбран не четко блок (блоки)

		// Нужно определить, находится ли выделение в пределах одного блока
		// Если выделение вылазит за пределы одного блока, форматировать нельзя

		// Выясняются позиции начала и конца блока
		QTextBlock currentBlock = _text_area->textCursor().block();
		int blockStart = currentBlock.position();
		int blockStop = currentBlock.position() + currentBlock.length();

		// Выясняются позиции начала и конца выделения
		int selectStart = _text_area->textCursor().selectionStart();
		int selectStop = _text_area->textCursor().selectionEnd();

		qDebug() << "Code format action, block " << blockStart << blockStop
			 << " selection " << selectStart << selectStop;
		if (blockStart <= selectStart && blockStop >= selectStop)
			enableIndent = false; // Выбран кусок текста в пределах блока
		else
			return;
	} else
		enableIndent =
		    true; // Выбран четко блок (блоки) текста, нужно делать отступ

	_text_area->textCursor().beginEditBlock();

	// Устанавливается шрифт
	QFont font;
	font.fromString(_editor_config->get_code_font());
	_text_area->setFontFamily(font.family());

	// Новый установленный шрифт показывается в выпадающем списке шрифтов
	set_fontselect_on_display(font.family());
	// Если в настройках разрешена автоматическая установка нужного размера
	if (_editor_config->get_code_font_size_apply() == true) {
		// Устанавливается нужный размер
		_text_area->setFontPointSize(_editor_config->get_code_font_size());

		// В выпадающем списке размеров выставляется установленный размер
		set_fontsize_on_display(_editor_config->get_code_font_size());
	}
	// Устанавливается цвет текста
	QColor codeColor(_editor_config->get_code_font_color());
	_text_area->setTextColor(codeColor);
	// Если в настройках разрешена автоматическая установка нужного отступа
	// И выбран четко блок (блоки)
	if (_editor_config->get_code_indent_size_apply() == true &&
	    enableIndent == true) {
		// Создание форматирования
		QTextBlockFormat indentFormatting;
		indentFormatting.setLeftMargin(_editor_config->get_code_indent_size());
		indentFormatting.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

		// Форматирование для добавления отступа
		_text_area->textCursor().mergeBlockFormat(indentFormatting);

		update_indentslider_to_actual_format();
	}
	_text_area->textCursor().endEditBlock();
}

void Editor::on_save_clicked(void)
{
	save_textarea();
}

void Editor::on_back_clicked(void)
{
	// back_callback_func(qobject_cast<QObject *>(this));
	back_callback_func();
}

// void Editor::on_freeze_clicked(void){
//// Обновление инфополей в области редактирования записи
// auto *metaeditor = globalparameters.edit_entry();   // MetaEditor
// *metaEditor = find_object<MetaEditor>(meta_editor_singleton_name);
// if(metaeditor) metaeditor->switch_pin();
// }
// Очистка форматирования, т.е. установка стандартного шрифта,
// размера и убирание утолщения, наклона, подчеркивания
void Editor::on_clear_clicked(void)
{
	int startCursorPos = _text_area->textCursor().position();
	int stopCursorPos = _text_area->textCursor().anchor();
	qDebug() << "Cursor start position: " << startCursorPos
		 << "Cursor stop position: " << stopCursorPos;

	bool flag_cursor_on_empty_line = is_cursor_on_empty_line();
	bool flag_cursor_on_space_line = is_cursor_on_space_line();
	// Очистка возможна только если что-то выделено
	// Или курсор стоит на пустой строке с одним символом перевода строки
	// Или курсор стоит на строке, в которой нет текста
	if (!(_text_area->textCursor().hasSelection() || flag_cursor_on_empty_line ||
		flag_cursor_on_space_line)) return;
	_text_area->textCursor().beginEditBlock();
	if (flag_cursor_on_space_line) (_text_area->textCursor()).select(QTextCursor::LineUnderCursor);
	// Создается стандартный шрифт
	QFont font;
	font.fromString(_editor_config->get_default_font()); // Стандартное начертание
	font.setPointSize(
	    _editor_config->get_default_font_size()); // Стандартный размер

	// Применяется стандартный шрифт
	_text_area->setCurrentFont(font);

	// Новый установленный шрифт показывается в выпадающем списке шрифтов
	set_fontselect_on_display(font.family());

	// В выпадающем списке размеров выставляется установленный размер
	set_fontsize_on_display(_editor_config->get_default_font_size());

	// Очищается формат символов
	QColor clearColor;
	QBrush clearBrush(clearColor);
	QTextCharFormat clearCharFormat;
	clearCharFormat.setForeground(clearBrush);
	_text_area->setCurrentCharFormat(clearCharFormat);
	// Если выделен блок
	// или курсор на пустой линии
	// или курсор на линии на которой нет символов
	if (is_block_select() || flag_cursor_on_empty_line ||
	    flag_cursor_on_space_line) {
		QTextBlockFormat format;

		// Убираются отступы
		format.setLeftMargin(0); // Убирается левый отступ (который, возможно был
		// установлен слайдером или кнопками изменения
		// отступа)
		format.setRightMargin(0);
		format.setTopMargin(0); // Убираются межстрочные интервалы, которые
		// самопроизвольно появляются при вставке из других
		// программ
		format.setBottomMargin(0);
		format.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

		// Применение форматирование
		_text_area->textCursor().setBlockFormat(format);
	}
	// Если была работа со строкой, в которой нет символов,
	// курсор переносится на начало строки, чтобы не путать пользователя
	if (flag_cursor_on_space_line) _text_area->moveCursor(QTextCursor::StartOfLine);
	// Очистка закомментирована, так как она заменена очисткой формата символов
	// setCurrentCharFormat()
	// А так же эта очистка некорректно работает из-за особенностей вставки в Qt
	// (первая строка получает отличный от остальных строк формат).
	// Думать дальше
	/*
	     // Удаление какого-либо форматирования стилем
	     QString htmlCode=textArea->textCursor().selection().toHtml();
	     qDebug() << "Before remove style: " << htmlCode;

	     // В регулярных выражениях Qt кванторы по-умолчанию жадные (greedy)
	     // Поэтому напрямую регвыру указывается что кванторы должны быть
	   ленивые
	     QRegExp replace_expression("style=\".*\"");
	     replace_expression.setMinimal(true);

	     htmlCode.replace(replace_expression, "");
	     qDebug() << "After remove style: " << htmlCode;

	     QString currStyleSheet=textArea->document()->defaultStyleSheet();
	     textArea->document()->setDefaultStyleSheet(" ");

	     textArea->textCursor().removeSelectedText();
	     textArea->textCursor().insertHtml(htmlCode);

	     textArea->document()->setDefaultStyleSheet(currStyleSheet);

	     textArea->textCursor().setPosition(startCursorPos);
	     textArea->textCursor().setPosition(stopCursorPos,
	   QTextCursor::KeepAnchor);
	     // textArea->setTextCursor( textArea->textCursor() );

	     qDebug() << "Cursor position: " <<
	   textArea->textCursor().position() <<
	     "Cursor anchor: " << textArea->textCursor().anchor();
	 */

	_text_area->textCursor().endEditBlock();

	// Вызывается метод, как будто переместился курсор с выделением, чтобы
	// обновились состояния подсветок кнопок форматирования
	on_selection_changed();

	update_indentslider_to_actual_format();
}

void Editor::format_to_list(QTextListFormat::Style setFormat)
{
	// Если выделения нет
	if (!_text_area->textCursor().hasSelection()) return;
	// Форматирование в список возможно только если выделен блок
	if (!is_block_select()) return;
	// Если строки выбраны
	if (_text_area->textCursor().hasSelection()) {
		if (_text_area->textCursor().currentList() == 0 ||
		    _text_area->textCursor().currentList()->format().style() != setFormat) {
			// Cтроки еще не отформатированы в данный вид списка,
			// надо отформатировать в список

			qDebug() << "Formatting to list";

			// Применение форматирования
			_text_area->textCursor().createList(setFormat);
		} else {
			// Строки уже были отформатированы в список, надо форматирование
			// сбросить в стандартное

			qDebug() << "Remove list formatting";

			// Выяснение текущего отступа
			int currentIndent;
			currentIndent = (int)_text_area->textCursor().blockFormat().leftMargin();

			// Создание форматирования
			QTextBlockFormat indentFormatting;
			indentFormatting.setLeftMargin(currentIndent);

			// Форматирование
			_text_area->textCursor().setBlockFormat(indentFormatting);

			// Создание форматирования по умолчанию чтобы убрать форматирование в
			// список
			// QTextBlockFormat formatting;
			// formatting.setAlignment(Qt::AlignLeft);

			// Форматирование
			// textarea->textCursor().setBlockFormat(formatting);
		}
	} else {
		// Вставляется первый пустой пункт
		_text_area->textCursor().insertList(setFormat);
	}
	// Выравнивание прокрутки чтоб курсор был виден если он уехал вниз
	_text_area->ensureCursorVisible();
}

// Форматирование в нумерованный список
void Editor::on_numericlist_clicked(void)
{
	format_to_list(QTextListFormat::ListDecimal);
}

// Форматирование в список с точечками
void Editor::on_dotlist_clicked(void)
{
	format_to_list(QTextListFormat::ListDisc);
}

// Добавление отступа
void Editor::on_indentplus_clicked(void)
{
	// абота с отступом возможна только если выделен блок
	// Или курсор стоит в начале строки
	// Закомментировано. Теперь отступ можно добавлять в любой момент
	/*
	     int start=textarea->textCursor().selectionStart();
	     QTextCursor cursor=textarea->textCursor();
	     cursor.setPosition(start);
	     if( !(is_block_select() || cursor.atBlockStart()) ) return;
	 */

	int currentIndent;

	// Выяснение текущего отступа
	currentIndent = (int)_text_area->textCursor().blockFormat().leftMargin();

	// Создание форматирования
	QTextBlockFormat indentFormatting;
	indentFormatting.setLeftMargin(currentIndent + 10); // setTextIndent(10);

	// Форматирование
	_text_area->textCursor().mergeBlockFormat(indentFormatting);

	update_indentslider_to_actual_format();
}

// Убирание отступа
void Editor::on_indentminus_clicked(void)
{
	int currentIndent, indentForSet;

	// Выяснение текущего отступа
	currentIndent = (int)_text_area->textCursor().blockFormat().leftMargin();
	if ((currentIndent - 10) > 0)
		indentForSet = currentIndent - 10;
	else
		indentForSet = 0;
	// Создание форматирования
	QTextBlockFormat indentFormatting;
	indentFormatting.setLeftMargin(indentForSet);

	// Форматирование
	_text_area->textCursor().mergeBlockFormat(indentFormatting);

	update_indentslider_to_actual_format();
}

// Форматирование по левому краю
void Editor::on_alignleft_clicked(void)
{
	align_text(Qt::AlignLeft);
}

// Форматирование по центру
void Editor::on_aligncenter_clicked(void)
{
	align_text(Qt::AlignHCenter);
}

// Форматирование по правому краю
void Editor::on_alignright_clicked(void)
{
	align_text(Qt::AlignRight);
}

// Форматирование по ширине
void Editor::on_alignwidth_clicked(void)
{
	align_text(Qt::AlignJustify);
}

// Выравнивание текста, вспомогательный метод
void Editor::align_text(Qt::AlignmentFlag mode)
{
	// Создание форматирования
	QTextBlockFormat formatting;
	formatting.setAlignment(mode);

	// Форматирование
	_text_area->textCursor().mergeBlockFormat(formatting);

	update_align_button_higlight(true);
}

// Слот, срабатывающий при изменении шрифта в списке шрифтов
void Editor::on_fontselect_changed(const QFont& font)
{
	if (_flag_set_font_parameters_enabled == false) return;
	_text_area->setFontFamily(font.family());

	_current_font_family = font.family();

	// Курсор после выбора возвращается в область редактирования
	_text_area->setFocus();
}

// Метод только меняет значение, показываемое списком шрифтов
void Editor::set_fontselect_on_display(QString font_name)
{
	_flag_set_font_parameters_enabled = false;
	if (font_name.size() > 0)
		_font_select->setCurrentIndex(_font_select->findText(font_name));
	else
		_font_select->setCurrentIndex(_font_select->count() - 1);
	_current_font_family = font_name;

	_flag_set_font_parameters_enabled = true;
}

// Слот, срабатывающий когда изменен размер шрифта через список размеров
void Editor::on_fontsize_changed(int i)
{
	if (_flag_set_font_parameters_enabled == false) return;
	int n = (_font_size->itemData(i)).toInt();
	if (n < MINIMUM_ALLOWED_FONT_SIZE) return;
	if (n > MAXIMUM_ALLOWED_FONT_SIZE) return;
	_text_area->setFontPointSize(n);

	_current_font_size = n;

	// Курсор после выбора возвращается в область редактирования
	_text_area->setFocus();
}

// Метод только меняет значение, показываемое списком размеров шрифта
void Editor::set_fontsize_on_display(int n)
{
	_flag_set_font_parameters_enabled = false;

	_font_size->setCurrentIndex(_font_size->findData(n));
	_current_font_size = n;

	_flag_set_font_parameters_enabled = true;
}

// Слот, срабатыващий при нажатии на кнопку выбора цвета текста
void Editor::on_fontcolor_clicked()
{
	// Текущий цвет возле курсора
	QColor currentColor = _text_area->textColor();

	// Диалог запроса цвета
	QColor selectedColor = QColorDialog::getColor(currentColor, this);
	// Если цвет выбран, и он правильный
	if (selectedColor.isValid()) {
		// Меняется цвет кнопки
		_font_color->setPalette(QPalette(selectedColor));
		// Если выделение есть
		if (_text_area->textCursor().hasSelection())
			_text_area->setTextColor(selectedColor); // Меняется цвет текста
		else {
			// Иначе надо выделить дополнительным курсором слово на
			// котором стоит курсор
			QTextCursor cursor = _text_area->textCursor();
			cursor.select(QTextCursor::WordUnderCursor);

			QTextCharFormat format;
			format.setForeground(selectedColor);

			cursor.mergeCharFormat(format);
		}
	}
}

// Слот вызывается при каждом движении курсора в момент выделения текста
void Editor::on_selection_changed(void)
{
	// Если выделения нет
	if (!_text_area->textCursor().hasSelection()) return;
	// qDebug() << "Selection text " <<
	// textarea->textCursor().selection().toHtml();

	// Позиции начала и конца выделения
	int start = _text_area->textCursor().selectionStart();
	int stop = _text_area->textCursor().selectionEnd();
	// qDebug() << "Selection start " << start << " stop " << stop;

	// Создается дополнительный курсор, который будет пробегать по выделенному
	// тексту
	QTextCursor cursor = _text_area->textCursor();
	cursor.setPosition(start);

	// Выравнивание относится к форматированию строк, начальное состояние
	// берётся из начального положения курсора
	auto startAlign = cursor.blockFormat().alignment();

	// Курсор сдвигается на одну позицию вперёд
	cursor.movePosition(QTextCursor::NextCharacter);

	// Для анализа форматирования символов надо начинать
	// с позиции, следующей справа от начала выделения
	QString startFontFamily = cursor.charFormat().fontFamily(); // Шрифт
	qreal startSize = cursor.charFormat().fontPointSize();      // азмер шрифта
	bool startBold = false;
	if (cursor.charFormat().fontWeight() == QFont::Bold) startBold = true; // Тощина

	bool startItalic = cursor.charFormat().fontItalic();       // Наклон
	bool startUnderline = cursor.charFormat().fontUnderline(); // Подчеркивание

	int differentFontFlag = 0;
	int differentSizeFlag = 0;
	int differentBoldFlag = 0;
	int differentItalicFlag = 0;
	int differentUnderlineFlag = 0;
	int differentAlignFlag = 0;
	// Пробегаем по выделенному куску текста дополнительным курсором
	// и выясняем, есть ли разные шрифты, разные размеры,
	// разные начертания символов, разное выравнивание в выделенном тексте
	while (cursor.position() <= stop) {
		if (differentFontFlag == 0 &&
		    startFontFamily != cursor.charFormat().fontFamily()) differentFontFlag = 1;
		if (differentSizeFlag == 0 &&
		    startSize != cursor.charFormat().fontPointSize()) differentSizeFlag = 1;
		if (differentBoldFlag == 0) {
			int b = cursor.charFormat().fontWeight();
			if (startBold == false && b == QFont::Bold)
				differentBoldFlag = 1;
			else if (startBold == true && b == QFont::Normal)
				differentBoldFlag = 1;
		}
		if (differentItalicFlag == 0 &&
		    startItalic != cursor.charFormat().fontItalic()) differentItalicFlag = 1;
		if (differentUnderlineFlag == 0 &&
		    startUnderline != cursor.charFormat().fontUnderline()) differentUnderlineFlag = 1;
		if (differentAlignFlag == 0 &&
		    startAlign != cursor.blockFormat().alignment()) differentAlignFlag = 1;
		// Курсор передвигается на одну позицию вперед
		// Если дальше двигаться некуда (конец документа) цикл досрочно завершается
		if (cursor.movePosition(QTextCursor::NextCharacter) == false) break;
	}
	// Список выбора шрифта начинает указывать на нужный шрифт
	if (differentFontFlag == 0)
		set_fontselect_on_display(
		    startFontFamily); // Если всё выделение одним шрифтом
	else
		set_fontselect_on_display("");
	// Список выбора размера начинает указывать на нужный размер
	if (differentSizeFlag == 0)
		set_fontsize_on_display(
		    (int)startSize); // Если всё отформатировано одним размером
	else
		set_fontsize_on_display(0); // В выделении есть разные размеры
	// Кнопка Bold выключается, если есть разное Bold форматирование
	// и включается, если форматирование одинаковое,
	// и выделение начиналось с Bold
	if (differentBoldFlag == 1)
		set_outline_button_higlight(BT_BOLD, false);
	else if (startBold == true)
		set_outline_button_higlight(BT_BOLD, true);
	// Кнопка Italic
	if (differentItalicFlag == 1)
		set_outline_button_higlight(BT_ITALIC, false);
	else if (startItalic == true)
		set_outline_button_higlight(BT_ITALIC, true);
	// Кнопка Underline
	if (differentUnderlineFlag == 1)
		set_outline_button_higlight(BT_UNDERLINE, false);
	else if (startUnderline == true)
		set_outline_button_higlight(BT_UNDERLINE, true);
	// Кнопки выравнивания
	if (differentAlignFlag == 1)
		update_align_button_higlight(false);
	else
		update_align_button_higlight(true);
}

void Editor::update_tool_line_to_actual_format(void)
{
	// Список должен показывать текущий шрифт позиции, где находится курсор
	if (_current_font_family != _text_area->fontFamily()) set_fontselect_on_display(_text_area->fontFamily());
	// азмер
	if (_current_font_size != (int)_text_area->fontPointSize()) set_fontsize_on_display((int)_text_area->fontPointSize());
	// Кнопки форматирования начертания
	update_outline_button_higlight();

	// Кнопки выравнивания
	update_align_button_higlight(true);
}

void Editor::update_indentslider_to_actual_format(void)
{
	int i;

	i = (int)_text_area->textCursor().blockFormat().textIndent();
	if (_current_text_indent != i) {
		emit send_set_textindent_pos(i);
		_current_text_indent = i;
	}
	i = (int)_text_area->textCursor().blockFormat().leftMargin();
	if (_current_left_indent != i) {
		emit send_set_leftindent_pos(i);
		_current_left_indent = i;
	}
	i = (int)_text_area->textCursor().blockFormat().rightMargin();
	if (_current_right_indent != i) {
		emit send_set_rightindent_pos(i);
		_current_right_indent = i;
	}
}

// Слот вызывается при каждом перемещении курсора
void Editor::on_cursor_position_changed(void)
{
	// Если одновременно идет режим выделения
	// то обслуживание текущего шрифта и размера идет в on_selection_changed()
	if (_text_area->textCursor().hasSelection()) return;
	update_tool_line_to_actual_format();

	update_indentslider_to_actual_format();
}

bool Editor::is_key_for_tool_line_update(QKeyEvent* event)
{
	if (event->modifiers().testFlag(Qt::ControlModifier) ||
	    event->modifiers().testFlag(Qt::AltModifier) ||
	    event->modifiers().testFlag(Qt::MetaModifier) ||
	    event->key() == Qt::Key_F1 || event->key() == Qt::Key_F2 ||
	    event->key() == Qt::Key_F3 || event->key() == Qt::Key_F4 ||
	    event->key() == Qt::Key_F5 || event->key() == Qt::Key_F6 ||
	    event->key() == Qt::Key_F7 || event->key() == Qt::Key_F8 ||
	    event->key() == Qt::Key_F9 || event->key() == Qt::Key_F10 ||
	    event->key() == Qt::Key_F11 || event->key() == Qt::Key_F12)
		return true;
	else
		return false;
}

// Cлот отлавливает нажатия клавиш
void Editor::keyPressEvent(QKeyEvent* event)
{
	if (is_key_for_tool_line_update(event)) update_tool_line_to_actual_format();
}

// Cлот отлавливает отжатия клавиш
void Editor::keyReleaseEvent(QKeyEvent* event)
{
	if (is_key_for_tool_line_update(event)) update_tool_line_to_actual_format();
}

void Editor::on_undo(void)
{
	qDebug() << "Undo slot normal running ";
	_text_area->undo();
	update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}

void Editor::on_redo(void)
{
	qDebug() << "Redo slot normal running ";
	_text_area->redo();
	update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}

void Editor::cut(void)
{
	_text_area->cut();
	update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}

void Editor::copy(void)
{
	// Если выбрана только картинка или курсор стоит на позиции картинки
	if (is_image_select() || is_cursor_on_image()) {
		QTextImageFormat imageFormat;
		if (is_image_select()) imageFormat = image_format_on_select();
		if (is_cursor_on_image()) imageFormat = image_format_on_cursor();
		// Из формата выясняется имя картинки
		QString imageName = imageFormat.name();

		// Из ресурсов вытягивается картинка
		QVariant imageData = _text_area->document()->resource(
		    QTextDocument::ImageResource, QUrl(imageName));
		QImage image = imageData.value<QImage>();

		// Создается ссылка на буфер обмена
		QClipboard* clipboard = QApplication::clipboard();

		// Копирвание картинки в буфер обмена
		clipboard->setImage(image);
	} else
		_text_area->copy(); // Обычное копирование

	update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}

void Editor::paste(void)
{
	_text_area->paste();
	update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}

void Editor::on_selectAll(void)
{
	_text_area->selectAll();
	update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}

// Обновление подсветки кнопок выравнивания текста
// Если параметр activate=false, все кнопки будут выставлены в неактивные
// Если параметр activate=true, будет подсвечена кнопка, соответсвующая
// текущему форматированию
void Editor::update_align_button_higlight(bool activate)
{
	QPalette palActive, palInactive;
	palActive.setColor(QPalette::Normal, QPalette::Button, _buttons_select_color);
	palActive.setColor(QPalette::Normal, QPalette::Window, _buttons_select_color);

	_align_left->setPalette(palInactive);
	_align_center->setPalette(palInactive);
	_align_right->setPalette(palInactive);
	_align_width->setPalette(palInactive);
	if (activate == false) return;
	if (_text_area->alignment() == Qt::AlignLeft)
		_align_left->setPalette(palActive);
	else if (_text_area->alignment() == Qt::AlignHCenter)
		_align_center->setPalette(palActive);
	else if (_text_area->alignment() == Qt::AlignRight)
		_align_right->setPalette(palActive);
	else if (_text_area->alignment() == Qt::AlignJustify)
		_align_width->setPalette(palActive);
}

// Обновление подсветки клавиш начертания текста
void Editor::update_outline_button_higlight(void)
{
	QPalette palActive, palInactive;
	palActive.setColor(QPalette::Normal, QPalette::Button, _buttons_select_color);
	palActive.setColor(QPalette::Normal, QPalette::Window, _buttons_select_color);

	_bold->setPalette(palInactive);
	_italic->setPalette(palInactive);
	_underline->setPalette(palInactive);
	if (_text_area->fontWeight() == QFont::Bold) _bold->setPalette(palActive);
	if (_text_area->fontItalic() == true) _italic->setPalette(palActive);
	if (_text_area->fontUnderline() == true) _underline->setPalette(palActive);
}

void Editor::set_outline_button_higlight(int button, bool active)
{
	QPalette palActive, palInactive;
	palActive.setColor(QPalette::Normal, QPalette::Button, _buttons_select_color);
	palActive.setColor(QPalette::Normal, QPalette::Window, _buttons_select_color);
	if (button == BT_BOLD) {
		if (active == false)
			_bold->setPalette(palInactive);
		else
			_bold->setPalette(palActive);
		return;
	}
	if (button == BT_ITALIC) {
		if (active == false)
			_italic->setPalette(palInactive);
		else
			_italic->setPalette(palActive);
		return;
	}
	if (button == BT_UNDERLINE) {
		if (active == false)
			_underline->setPalette(palInactive);
		else
			_underline->setPalette(palActive);
		return;
	}
}

// Показывание окна с исходным текстом HTML
void Editor::on_showhtml_clicked(void)
{
	EditorMultiLineInputDialog dialog(this);

	dialog.set_text(_text_area->toHtml());
	dialog.set_window_title(tr("Edit HTML source"));
	if (dialog.exec() != QDialog::Accepted) return;
	_text_area->setHtml(dialog.get_text());
	// Если в диалоговом окне был иземен HTML код
	if (dialog.isModified()) _text_area->document()->setModified(true);
}

void Editor::on_findtext_clicked(void)
{
	_find_dialog->show();
	_find_dialog->activateWindow();
}

// Слот, принимающий данные от окна поиска текста
void Editor::on_findtext_signal_detect(const QString& text, QTextDocument::FindFlags flags)
{
	qDebug() << "Find text " << text << " with flags " << flags;
	if (!_text_area->find(text, flags)) {
		// findDialog->close();  //
		_find_dialog->hide();
		QMessageBox::information(this, tr("Search result"), tr("String '<b>") + text + tr("</b>' not found"), QMessageBox::Close);
	}
}

// Открытие контекстного меню в области редактирования
void Editor::on_custom_contextmenu_requested(const QPoint& _position)
{
	qDebug() << "In Editor on_customContextMenuRequested";
	// Конструирование меню
	// editor_context_menu=textarea->createStandardContextMenu();
	// Включение отображения меню на экране
	// menu.exec(event->globalPos());
	// Если выбрана картинка
	// Или нет выделения, но курсор находится на позиции картинки
	auto _context_menu = _blogger->super_menu();
	if (is_image_select() || is_cursor_on_image())
		_context_menu->set_edit_image_properties(true);
	else
		_context_menu->set_edit_image_properties(false);
	// Контекстное меню запускается

	_context_menu->exec(
	    _text_area->viewport()->mapToGlobal(_position) // mapToGlobal(_position)
	    );                                             // this->point()->mapToGlobal(_position)
}

void Editor::on_settings_clicked(void)
{
	// Создается окно настроек, после выхода из этой функции окно удалится
	EditorConfigDialog dialog(_editor_config);
	dialog.show();
}

// Действия при нажатии кнопки отображения символов редактирования
void Editor::on_showformatting_clicked(void)
{
	QPalette palActive, palInactive;
	palActive.setColor(QPalette::Normal, QPalette::Button, _buttons_select_color);
	palActive.setColor(QPalette::Normal, QPalette::Window, _buttons_select_color);
	if (_text_area->get_showformatting() == false) {
		_text_area->set_showformatting(true);
		_show_formatting->setPalette(palActive);
	} else {
		_text_area->set_showformatting(false);
		_show_formatting->setPalette(palInactive);
	}
}

// Действия при перемещении движка абзацного отступа
void Editor::on_indentline_change_textindent_pos(int i)
{
	// Создание форматирования
	QTextBlockFormat indentFormatting;
	indentFormatting.setTextIndent(i);

	// Форматирование для добавления отступа
	_text_area->textCursor().mergeBlockFormat(indentFormatting);

	// едактор запоминает отступ, чтобы было с чем сравнивать
	// при перемещении курсора со строки на строку
	_current_text_indent = i;

	_text_area->show_indetedge(true);
	_text_area->set_indentedge_pos(_indent_slider->get_current_indentedge_pos() - _text_area->lineWidth() - 1);
}

// Действия при перемещении движка левого отступа
void Editor::on_indentline_change_leftindent_pos(int i)
{
	// Создание форматирования
	QTextBlockFormat indentFormatting;
	indentFormatting.setLeftMargin(i);

	// Форматирование для добавления отступа
	_text_area->textCursor().mergeBlockFormat(indentFormatting);

	// едактор запоминает отступ, чтобы было с чем сравнивать
	// при перемещении курсора со строки на строку
	_current_left_indent = i;

	_text_area->show_indetedge(true);
	_text_area->set_indentedge_pos(_indent_slider->get_current_indentedge_pos() - _text_area->lineWidth() - 1);
}

// Действия при перемещении движка правого отступа
void Editor::on_indentline_change_rightindent_pos(int i)
{
	// Создание форматирования
	QTextBlockFormat indentFormatting;
	indentFormatting.setRightMargin(i);

	// Форматирование для добавления отступа
	_text_area->textCursor().mergeBlockFormat(indentFormatting);

	// едактор запоминает отступ, чтобы было с чем сравнивать
	// при перемещении курсора со строки на строку
	_current_right_indent = i;

	_text_area->show_indetedge(true);
	_text_area->set_indentedge_pos(_indent_slider->get_current_indentedge_pos() - _text_area->lineWidth() - 1);
}

// Действия в момент отпускания кнопки мышки над линейкой отступов
void Editor::on_indentline_mouse_release(void)
{
	_text_area->show_indetedge(false); // Скрывается вертикальная линия
	_text_area->set_indentedge_pos(0); // Координата вертикальной линии обнуляется
}

// Действия при нажатии кнопки создания новой таблицы
void Editor::on_createtable_clicked(void)
{
	// Создается и запускается диалог создания новой таблицы
	EditorAddTableForm dialog;
	if (dialog.exec() != QDialog::Accepted) return;
	// Выясняются введенные в диалоге данные
	int tableColumns = dialog.get_columns();
	int tableRows = dialog.get_rows();
	int tableWidth = dialog.get_width();

	// Целочислительный формат ширины таблицы преобразуется в проценты
	QTextLength tableWidthInPercent(QTextLength::PercentageLength, tableWidth);

	// Создается форматирование таблицы
	QTextTableFormat tableFormat;
	tableFormat.setWidth(tableWidthInPercent);
	tableFormat.setAlignment(Qt::AlignHCenter);
	tableFormat.setBorder(1);
	tableFormat.setBorderStyle(QTextFrameFormat::
		// BorderStyle_None //
	    BorderStyle_Solid);
	tableFormat.setPadding(0);
	tableFormat.setCellPadding(0);
	tableFormat.setCellSpacing(-1);

	// Добавляется таблица с нужными размерами и форматированием
	// QTextTable *table=textarea->textCursor().insertTable(table_rows,
	// table_columns, table_format);
	_text_area->textCursor().insertTable(tableRows, tableColumns, tableFormat);

	return;
	//// Вставка новой таблицы через генериацию HTML кода
	//// Если данные введены нормально
	//// Insert a new table through generiatsiyu HTML code
	//// If the data is entered properly
	// if(table_vnum > 0 && table_hnum > 0 && table_width > 0){
	// QStringList tab;
	// tab.clear();

	//// QColor table_color;
	//// table_color=sapp_t::instance()->palette().color(QPalette::Normal,
	// QPalette::Base);
	//// qDebug() << "Table background color " << table_color.name();
	//// tab.append(QString("<table border=\"1\" align=\"center\"
	// width=\"%1%\" cellspacing=\"0\" cellpadding=\"0\"
	// bgcolor=\"%2%\">").arg(table_width).arg(table_color.name()));

	// tab.append(QString("<table style=\"border-style:solid;
	// border-right:none; border-bottom:none;\" border=\"1\" align=\"center\"
	// width=\"%1%\" cellspacing=\"0\" cellpadding=\"3\">").arg(table_width));
	// for(int i = 0; i < table_hnum; i ++){
	// tab.append(QString("<tr>"));
	// for(int o = 0; o < table_vnum; o ++) tab.append(QString("<td
	// style=\"border-style:solid; border-right:none;
	// border-bottom:none;\"><p></p></td>"));
	// tab.append(QString("</tr>"));
	// }
	// tab.append(QString("</table>"));

	// QString					table_text	=
	// tab.join("\n");
	// QTextDocumentFragment	fragment	=
	// QTextDocumentFragment::fromHtml(table_text);
	// textarea->textCursor().insertFragment(fragment);
	// }

	//// Вставка кода таблицы из файла
	//// Inserting a code table from a file
	// QFile file("table.txt");
	// if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
	// critical_error("Can not open table file");
	// QString tablecode;
	// while(! file.atEnd()){
	// QString line = file.readLine();
	// tablecode = tablecode + line + "\n";
	// }
	// file.close();

	// QTextDocumentFragment fragment =
	// QTextDocumentFragment::fromHtml(tablecode);
	// textarea->textCursor().insertFragment(fragment);

	// return;
}

// Удаление строк из таблицы
void Editor::on_table_remove_row_clicked(void)
{
	QTextCursor cursor(_text_area->textCursor());
	QTextTable* table = cursor.currentTable();
	if (table != 0) {
		QTextTableCell cell = table->cellAt(cursor);
		int cellRowCursor = cell.row();      // Текущий номер строки (счет с нуля)
		table->removeRows(cellRowCursor, 1); // Удаляются строки таблицы
	}
}

// Удаление столбцов из таблицы
void Editor::on_table_remove_col_clicked(void)
{
	QTextCursor cursor(_text_area->textCursor());
	QTextTable* table = cursor.currentTable();
	if (table != 0) {
		QTextTableCell cell = table->cellAt(cursor);
		int cellColCursor = cell.column();      // Текущий номер столбца (счет с нуля)
		table->removeColumns(cellColCursor, 1); // Удаляются столбцы таблицы
	}
}

// Добавление строк в таблицу
void Editor::on_table_add_row_clicked(void)
{
	QTextCursor cursor(_text_area->textCursor());
	QTextTable* table = cursor.currentTable();
	if (table) {
		QTextTableCell cell = table->cellAt(cursor); // Выясняется текущая ячейка
		int cellRowCursor = cell.row();              // Текущий номер строки (счет с нуля)

		bool ok = false;
		int addNum = QInputDialog::getInt(this, tr("Append rows to table"), tr("Append rows:"), 1, 1, 100, 1, &ok);
		if (ok && addNum > 0) table->insertRows(cellRowCursor + 1, addNum);
	}
}

// Добавление столбцов в таблицу
void Editor::on_table_add_col_clicked(void)
{
	QTextCursor cursor(_text_area->textCursor());
	QTextTable* table = cursor.currentTable();
	if (table) {
		QTextTableCell cell = table->cellAt(cursor); // Выясняется текущая ячейка
		int cellColCursor = cell.column();           // Текущий номер столбца (счет с нуля)

		bool ok = false;
		int addNum = QInputDialog::getInt(this, tr("Append columns to table"), tr("Append columns:"), 1, 1, 100, 1, &ok);
		if (ok && addNum > 0) table->insertColumns(cellColCursor + 1, addNum);
	}
}

// Объединение ячеек таблицы
void Editor::on_table_merge_cells_clicked(void)
{
	QTextCursor cursor(_text_area->textCursor());
	QTextTable* table = cursor.currentTable();
	if (table) table->mergeCells(cursor);
}

void Editor::on_table_split_cell_clicked(void)
{
	QTextCursor cursor(_text_area->textCursor());
	QTextTable* table = cursor.currentTable();
	// Если курсор находится где-то внутри таблицы
	if (table) {
		// Выясняется, выделены ячейки или нет
		int* currX = new int;
		int* currY = new int;
		int* currWidthX = new int;
		int* currWidthY = new int;
		cursor.selectedTableCells(currX, currY, currWidthX, currWidthY);
		// Если ячейки выделены, и захвачено для выделения более одной ячейки
		if (*currWidthX > 1 || *currWidthY > 1) {
			// абота возможна только с одной ячейкой
			QMessageBox msgBox;
			msgBox.setWindowTitle(tr("Split cell"));
			msgBox.setText(tr("Select a single cell."));
			msgBox.exec();

			return;
		}
		QTextTableCell cell = table->cellAt(cursor); // Выясняется текущая ячейка
		int cellColCursor = cell.column();           // Текущий номер столбца (счет с нуля)
		int cellRowCursor = cell.row();              // Текущий номер строки (счет с нуля)
		qDebug() << "Current cell is " << cellColCursor << cellRowCursor;

		// Курсором выделяется таблица
		cursor.setPosition(table->firstCursorPosition().position() - 1, QTextCursor::MoveAnchor);
		cursor.setPosition(table->lastCursorPosition().position() + 1, QTextCursor::KeepAnchor);

		// Выделенный фрагмент выделяется в отдельный объект
		QTextDocumentFragment fragment(cursor);

		EditorAbsTable current_table(table->columns(), table->rows(), fragment.toHtml());

		qDebug() << "This cell colspan "
			 << current_table.get_cell_colspan(cellColCursor, cellRowCursor)
			 << " rowspan "
			 << current_table.get_cell_rowspan(cellColCursor, cellRowCursor);

		// Выясняется, на сколько сегментов можно разбивать ячейку
		int hSplitTo;
		int vSplitTo;
		int hCurrMerge =
		    current_table.get_cell_colspan(cellColCursor, cellRowCursor);
		int vCurrMerge =
		    current_table.get_cell_rowspan(cellColCursor, cellRowCursor);
		if (hCurrMerge == 0) hCurrMerge = 1;
		if (vCurrMerge == 0) vCurrMerge = 1;
		if (hCurrMerge == 1)
			hSplitTo = 0; // Можно разбивать на любое число
		else
			hSplitTo = hCurrMerge;
		if (vCurrMerge == 1)
			vSplitTo = 0; // Можно разбивать на любое число
		else
			vSplitTo = vCurrMerge;
		// Создается диалог запроса на сколько сегментов надо разбивать ячейку
		EditorSplitCellForm dialog(hSplitTo, vSplitTo);
		if (dialog.exec() == QDialog::Accepted) {
			int hSplit = dialog.get_num_split_h(); // Эти значения всегда 1 или больше
			int vSplit = dialog.get_num_split_v();
			// азбивка по горизонтали
			if (hSplit > 1) {
				if (hCurrMerge == 1)
					current_table.split_single_cell_by_horisontal(cellColCursor, cellRowCursor, hSplit);
				else
					current_table.split_merged_cell_by_horisontal(cellColCursor, cellRowCursor);
			}
			// азбивка по вертикали
			if (vSplit > 1) {
				if (vCurrMerge == 1)
					current_table.split_single_cell_by_vertical(
					    cellColCursor, cellColCursor + hSplit - 1, cellRowCursor, vSplit);
				else
					current_table.split_merged_cell_by_vertical(
					    cellColCursor, cellColCursor + hSplit - 1, cellRowCursor);
			}
			qDebug() << "New table HTML text " << current_table.get_table();

			cursor.beginEditBlock();
			cursor.deleteChar();
			cursor.insertHtml(current_table.get_table());
			cursor.endEditBlock();

			return;
		} else {
		}
	} // Закончилось условие что курсор находится где-то внутри таблицы
}

void Editor::on_context_menu_edit_image_properties()
{
	// Если выделена картинка
	if (is_image_select() || is_cursor_on_image()) {
		qDebug() << "Image selected";

		edit_image_properties();
	}
}

// Метод, определяющий, выбрана ли только одна картинка
bool Editor::is_image_select(void)
{
	// Происходит анализ, выделена ли картинка
	bool is_image_select_flag = false;

	// Блок, в пределах которого находится курсор
	QTextBlock currentBlock = _text_area->textCursor().block();
	// QTextBlock::iterator it;
	QTextFragment fragment;
	// Если есть выделение
	if (_text_area->textCursor().hasSelection()) {
		// Перебиратся фрагметы блока
		for (auto it = currentBlock.begin(); !(it.atEnd()); ++it) {
			fragment = it.fragment();
			// Если фрагмент содержит изображение
			if (fragment.isValid())
				if (fragment.charFormat().isImageFormat()) {
					int fragmentStart = fragment.position();
					int fragmentEnd = fragmentStart + fragment.length();
					int selectionStart = _text_area->textCursor().selectionStart();
					int selectionEnd = _text_area->textCursor().selectionEnd();
					// Если начало и конец фрагмента совпадает с координатами выделения
					// Проверяется и случай, когда блок выделен в обратную сторону
					if ((fragmentStart == selectionStart &&
						fragmentEnd == selectionEnd) ||
					    (fragmentStart == selectionEnd &&
						fragmentEnd == selectionStart)) {
						is_image_select_flag = true;
						break;
					}
				}
		}
	}
	return is_image_select_flag;
}

// Формат картинки, которая выделена (если выделена единственная картинка)
QTextImageFormat Editor::image_format_on_select(void)
{
	// Блок, в пределах которого находится курсор
	QTextBlock currentBlock = _text_area->textCursor().block();
	// QTextBlock::iterator it;
	QTextFragment fragment;
	// Если есть выделение
	if (_text_area->textCursor().hasSelection()) {
		// Перебиратся фрагметы блока
		for (auto it = currentBlock.begin(); !(it.atEnd()); ++it) {
			fragment = it.fragment();
			// Если фрагмент содержит изображение
			if (fragment.isValid())
				if (fragment.charFormat().isImageFormat()) {
					int fragmentStart = fragment.position();
					int fragmentEnd = fragmentStart + fragment.length();
					int selectionStart = _text_area->textCursor().selectionStart();
					int selectionEnd = _text_area->textCursor().selectionEnd();
					// Если начало и конец фрагмента совпадает с координатами выделения
					// Проверяется и случай, когда блок выделен в обратную сторону
					if ((fragmentStart == selectionStart &&
						fragmentEnd == selectionEnd) ||
					    (fragmentStart == selectionEnd &&
						fragmentEnd == selectionStart)) {
						QTextImageFormat imageFormat =
						    fragment.charFormat().toImageFormat();

						return imageFormat;
					}
				}
		}
	}
	return QTextImageFormat();
}

// Проверка, находится ли курсор на позиции, где находится картинка
bool Editor::is_cursor_on_image(void)
{
	// Проверка срабатывает только если нет выделения
	if (_text_area->textCursor().hasSelection() == false) {
		QTextImageFormat imageFormat =
		    _text_area->textCursor().charFormat().toImageFormat();
		if (imageFormat.isValid()) return true;
	}
	return false;
}

// Формат картинки на которой находится курсор
QTextImageFormat Editor::image_format_on_cursor(void)
{
	// Проверка срабатывает только если нет выделения
	if (_text_area->textCursor().hasSelection() == false) {
		QTextImageFormat imageFormat =
		    _text_area->textCursor().charFormat().toImageFormat();
		if (imageFormat.isValid()) return imageFormat;
	}
	return QTextImageFormat();
}

// Обработка клавиши добавления картинки
void Editor::on_insert_image_from_file_clicked(void)
{
	// Если выделена картинка
	if (is_image_select() || is_cursor_on_image()) {
		qDebug() << "Image selected";

		edit_image_properties();
	} else {
		// Иначе картинка не выделена, и срабатывает режим добавления
		// картинки из файла

		// Диалог выбора файлов катинок
		QFileDialog image_select_dialog(this);
		image_select_dialog.setFileMode(
		    QFileDialog::ExistingFiles); // QFileDialog::ExistingFile
		image_select_dialog.setNameFilter("*.png *.jpg *.gif");
		image_select_dialog.setWindowTitle(tr("Insert image"));
		image_select_dialog.setDirectory(QDir::homePath());
		image_select_dialog.exec();

		// Выясняется список выбранных файлов
		QStringList files = image_select_dialog.selectedFiles();
		// Если ни один файл не выбран
		if (files.size() == 0) return;
		// Перебираются файлы выбранных картинок
		for (int i = 0; i < files.size(); ++i) {
			// Текущее имя файла
			QString currFileName = files.at(i);

			// Картинка загружается из файла
			QImage image = QImageReader(currFileName).read();

			// Внутреннее имя картинки
			QString imageName = "image" + QString::number(rand()) + ".png";

			// Картинка добавляется в хранилище документа
			_text_area->document()->addResource(QTextDocument::ImageResource, QUrl(imageName), image);

			// Создается описание форматированной картинки
			// QTextImageFormat imageFormat;
			// imageFormat.setName(link.toString());

			// Картинка вставляется в текст
			QTextCursor cursor = _text_area->textCursor();
			cursor.insertImage(imageName);
		} // Закончился цикл перебора файлов картинок
	}         // Завершилось условие что картинка не выбрана и нужно добавлять из файла
}

void Editor::edit_image_properties(void)
{
	// Данные обрабатываемой картинки
	QTextImageFormat imageFormat;
	// Если выбрано изображение
	if (is_image_select()) imageFormat = image_format_on_select();
	// Если изображение не выбрано, но курсор находится в позиции изображения
	if (is_cursor_on_image()) imageFormat = image_format_on_cursor();
	// Выясняется имя картинки в ресурсах документа
	QString imageName = imageFormat.name();

	// По имени из ресурсов вытягивается кратинка
	QUrl urlName(imageName);
	QVariant imageData =
	    _text_area->document()->resource(QTextDocument::ImageResource, urlName);
	QImage image = imageData.value<QImage>();

	// Выяснятся реальные размеры картики
	int realImageWidth = image.width();
	int realImageHeight = image.height();

	qDebug() << "Real image width " << realImageWidth << " height "
		 << realImageHeight;
	qDebug() << "Format image width " << imageFormat.width() << " height "
		 << imageFormat.height();

	// Создается и запускается диалог запроса размеров картинки
	EditorImageProperties dialog;
	dialog.set_info(tr("Real image size ") + QString::number(realImageWidth) + " x " + QString::number(realImageHeight) + tr(" pixels"));
	dialog.set_real_width(realImageWidth);
	dialog.set_real_height(realImageHeight);
	// Если в форматировании картинки не задан размер картинки
	if (imageFormat.width() == 0 && imageFormat.height() == 0) {
		// В окне настройки стартовый размер задается как размер картинки
		dialog.set_width(realImageWidth);
		dialog.set_height(realImageHeight);
	} else {
		// В окне настройки стартовый размер задается согласно форматированию
		dialog.set_width(imageFormat.width());
		dialog.set_height(imageFormat.height());
	}
	dialog.update_percent();
	// Запуск диалога на выполнение
	if (dialog.exec() != QDialog::Accepted) return;
	imageFormat.setWidth(dialog.get_width());
	imageFormat.setHeight(dialog.get_height());
	// Если в новом формате картинки нет никаких ошибок
	if (imageFormat.isValid()) {
		// Если выбрано изображение
		if (is_image_select()) {
			QTextFragment fragment;

			// Блок, в пределах которого находится курсор
			QTextBlock currentBlock = _text_area->textCursor().block();
			// QTextBlock::iterator it;
			// Перебиратся фрагметы блока
			// Так как известно, что картинка выделена, поиск фрагмента будет успешным
			for (auto it = currentBlock.begin(); !(it.atEnd()); ++it) {
				fragment = it.fragment();
				// Если фрагмент содержит изображение
				if (fragment.isValid() && fragment.charFormat().isImageFormat()) break; // Переменная fragment содержит только картинку
			}
			QTextCursor helper = _text_area->textCursor();

			helper.setPosition(fragment.position());
			helper.setPosition(fragment.position() + fragment.length(), QTextCursor::KeepAnchor);
			helper.setCharFormat(imageFormat);
		}
		// Если изображение не выбрано, но курсор находится в позиции изображения
		if (is_cursor_on_image()) {
			int cursorPosition = _text_area->textCursor().position();

			QTextCursor helper = _text_area->textCursor();

			helper.setPosition(cursorPosition);
			if (_text_area->textCursor().atBlockStart())
				helper.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
			else
				helper.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
			helper.setCharFormat(imageFormat);
		}
	}
}

void Editor::on_expand_edit_area_clicked(void)
{
	if (_expand_edit_area_flag == false) {
		_expand_edit_area_flag = true;
		emit send_expand_edit_area(_expand_edit_area_flag);
	} else {
		_expand_edit_area_flag = false;
		emit send_expand_edit_area(_expand_edit_area_flag);
	}
}

void Editor::on_expand_tools_lines_clicked(void)
{
	switch_expand_tools_lines();
}

// Метод, переключающий состояние видимости полной панели инструментов
// Если вызывается без параметра (по умолчанию 0), метод сам переключает
// Параметр 1 - включить полную видимость
// Параметр -1 - выключить полную видимость
void Editor::switch_expand_tools_lines(int flag)
{
	bool setFlag = true;
	// Если метод был вызван без параметра
	if (flag == 0) {
		bool is_expand = _editor_config->get_expand_tools_lines();
		if (is_expand)
			setFlag = false; // Если панель инструментов распахнута, надо сомкнуть
		else
			setFlag = true; // Иначе распахнуть
	} else {
		// Иначе метод вызывался с каким-то параметром
		if (flag == 1) setFlag = true;
		if (flag == -1) setFlag = false;
	}
	// Панели распахиваются/смыкаются (кроме первой линии инструментов)
	_tools_line_1->setVisible(setFlag);
	if (_view_mode == WYEDIT_DESKTOP_MODE) _indent_slider->setVisible(setFlag);
	// Запоминается новое состояние
	_editor_config->set_expand_tools_lines(setFlag);

	// На всякий случай обновляется геометрия расположения движков на слайд-панели
	update_indentline_geometry();
}

void Editor::on_to_attach_clicked(void)
{
	attach_callback_func();
}

void Editor::on_find_in_base_clicked(void)
{
	emit wyedit_find_in_base_clicked();
}

// Слот нажатия кнопки показа текста в отдельном открепляемом окне
void Editor::on_show_text_clicked(void)
{
	if (_text_area->toPlainText().length() == 0) {
		QMessageBox msgBox;
		msgBox.setText("Can't show empty text in detached window");
		msgBox.exec();

		return;
	}
	EditorShowText* showText = new EditorShowText(this);

	// Устанавливается флаг удаления диалога после закрытия его окна
	showText->setAttribute(Qt::WA_DeleteOnClose);
	if (misc_field<name_key>() != name_value("")) showText->setWindowTitle(misc_field<name_key>());
	// For the clone method, the parent is specified, which will be destroyed when the window is closed, and will destroy this temporary document
	QTextDocument* cloneDocument = _text_area->document()->clone(showText); // Для метода clone указан parent, который уничтожится при закрытии окна, и за собой уничтожит этот временный документ
	showText->setDocument(cloneDocument);

	showText->show();
}

const std::function<void(QObject* editor, QString saveString)> Editor::save_callback() const
{
	return save_callback_func;
}

void Editor::save_callback(const std::function<void(QObject* editor, QString saveString)>& func)
{
	save_callback_func = func;
}

std::function<void(QObject* editor, QString& String)> Editor::load_callback() const
{
	return load_callback_func;
}

void Editor::load_callback(const std::function<void(QObject* editor, QString& String)>& func)
{
	load_callback_func = func;
}

void Editor::back_callback(void (*func)(void))
{
	back_callback_func = func;
}

void Editor::attach_callback(void (*func)(void))
{
	attach_callback_func = func;
}

//struct clear_full_fields_map {

//	full_fields_map *const _misc_fields;

//	clear_full_fields_map(full_fields_map *const misc_fields_)
//	    : _misc_fields(misc_fields_) {
//	}

//	template <typename key_t, typename value_t>
//	void operator()(const boost::fusion::pair<key_t, value_t> &) const {
//		//		(void)pair;
//		//		pair.second = value_t();  //
//		boost::fusion::at_key<key_t>(*_misc_fields) = value_t();
//	}
//};

void Editor::clear_all_misc_field(void)
{
	//	_misc_fields.clear();
	boost::fusion::for_each(_misc_fields, clear_full_fields_map(&_misc_fields));
}

void Editor::dir_file_empty_reaction(int mode)
{
	// Проверяется допустимость переданного значения
	if (mode == DIRFILEEMPTY_REACTION_SHOW_ERROR ||
	    mode == DIRFILEEMPTY_REACTION_SUPPRESS_ERROR)
		_dir_file_empty_reaction = mode;
	else
		critical_error("Editor::setDirFileEmptyReaction() : Unsupport mode " + QString::number(mode));
}

int Editor::dir_file_empty_reaction(void)
{
	return _dir_file_empty_reaction;
}

//// Метод позволяющий управлять доступностью инcтрументов редактирования
// void Editor::disable_tool_list(QStringList tool_names){
// qDebug() << "Editor::setDisableToolList() : " << tool_names;
// _disable_tool_list = tool_names;
// }

int Editor::cursor_position(void)
{
	return _text_area->textCursor().position();
}

void Editor::cursor_position(int n)
{
	QTextCursor cursor = _text_area->textCursor();

	cursor.setPosition(n);

	_text_area->setTextCursor(cursor);
}

int Editor::scrollbar_position(void)
{
	return _text_area->verticalScrollBar()->value();
}

void Editor::scrollbar_position(int n)
{
	_text_area->verticalScrollBar()->setValue(n);
}

std::shared_ptr<EditorConfig> Editor::editor_config() const { return _editor_config; }
// QToolBar *Editor::tool_bar(){return _tools_line_0;}

// SuperMenu *Editor::context_menu(){return _context_menu;}
#include "libraries/wyedit/editor.inl"
