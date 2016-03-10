#include <QtGui>
#include <QLayout>
#include <QMessageBox>
#include <QColor>
#include <QColorDialog>
#include <QStyle>
#include <QPalette>
#include <QStringList>
#include <QMenu>
#include <QObject>
#include <QtGlobal>
#include <QDateTime>
#include <QTextCursor>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QScrollBar>
#include <QSplitter>
#include <QToolButton>

#include "Editor.h"
#include "EditorConfig.h"
#include "EditorConfigDialog.h"
#include "EditorContextMenu.h"
#include "EditorTextArea.h"
#include "EditorSplitCellForm.h"
#include "indentslider/IndentSlider.h"
#include "EditorAbsTable.h"
#include "EditorAddTableForm.h"
#include "EditorImageProperties.h"
#include "EditorMultiLineInputDialog.h"
#include "views/record/MetaEditor.h"
#include "views/tree/TreeScreen.h"
#include "libraries/GlobalParameters.h"
#include "models/app_config/AppConfig.h"
#include "views/main_window/MainWindow.h"
#include "libraries/FlatControl.h"
#include "libraries/crypt/CryptService.h"

#include "main.h"
#include "libraries/DiskHelper.h"

extern AppConfig appconfig;

Editor::Editor(QWidget *parent) : QWidget(parent)
{
    _init_data_enable_assembly = true;
    _init_data_config_file_name = "";
    _init_data_enable_random_seed = false;

    _dir_file_empty_reaction = DIRFILEEMPTY_REACTION_SHOW_ERROR;

    save_callback_func = nullptr;
    load_callback_func = nullptr;
    back_callback_func = nullptr;
}


Editor::~Editor(void)
{
    delete _editor_config;
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
    delete _freeze;
    delete _find_in_base;
    delete _show_text;
    delete _to_attach;
    delete _indent_slider;
    delete _textformat_buttons_layout;
    delete _tools_line_0;
    delete _tools_line_1;
    delete _buttons_and_edit_layout;
    delete _editor_context_menu;
    delete _text_area;
}


const char *Editor::version(void)
{
    static const char *lib_version = WYEDIT_VERSION;
    return(lib_version);
}


void Editor::init_enable_assembly(bool flag)
{
    _init_data_enable_assembly = flag;
}


void Editor::init_config_file_name(QString name)
{
    _init_data_config_file_name = name;
}


void Editor::init_enable_random_seed(bool flag)
{
    _init_data_enable_random_seed = flag;
}


// Инициализация редактора
// Если mode=WYEDIT_DESKTOP_MODE - происходит обычная инициализация
// Если mode=WYEDIT_MOBILE_MODE - при инициализации в первую строку панели инструментов, слева, добавляется кнопка back
void Editor::init(int mode)
{
    _view_mode = mode;

    // Создается объект поддержки конфигурирования редактора
    _editor_config = new EditorConfig(_init_data_config_file_name, this);
    _editor_config->setObjectName("editorconfig");
    globalparameters.editor_config(_editor_config);

    // Выясняется перечень кнопок на панели инструментов
    _tools_list_in_line_0 = (_editor_config->get_tools_line_1()).split(",");
    _tools_list_in_line_1 = (_editor_config->get_tools_line_2()).split(",");

    // В мобильном режиме добавляется кнопка back (если ее нет)
    if(_view_mode == WYEDIT_MOBILE_MODE && !_tools_list_in_line_0.contains("back")) {
        _tools_list_in_line_0.prepend("separator");
        _tools_list_in_line_0.prepend("back");
    }

    // В мобильном режиме добавляется кнопка find_in_base (если ее нет)
    if(_view_mode == WYEDIT_MOBILE_MODE && !_tools_list_in_line_0.contains("find_in_base")) {
        _tools_list_in_line_0.append("separator");
        _tools_list_in_line_0.append("find_in_base");
    }

    // Создается виджет поиска, обязательно нужно указать parent чтобы
    // могли применяться флаги окна.
    // Виджет будет постоянно включен параллельно с работой редактора.
    // Только будет либо виден, либо невиден.
    // Create search widget, be sure to specify the parent to
    // Window flags can be used.
    // The widget will be constantly connected in parallel with the work of the editor.
    // Just to be either visible or invisible.
    _find_dialog = new EditorFindDialog(this);
    // findDialog->setVisible(false);
    _find_dialog->hide();
    _find_dialog->setHidden(true);

    // Создаётся контекстное меню
    _editor_context_menu = new EditorContextMenu(this);

    setup_buttons();

    setup_closebutton();
    assembly_closebutton();
    assembly_buttons();

    setup_editor_area();

    setup_signals();



    if(_init_data_enable_assembly) assembly();

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
    if(_editor_config->get_expand_tools_lines())
        switch_expand_tools_lines(1);
    else
        switch_expand_tools_lines(-1);

    if(_init_data_enable_random_seed) {
        QDateTime datetime = QDateTime::currentDateTime();
        int seed = datetime.toTime_t();
        // qDebug() << "Random generator init " << seed;
        srand(seed);
        // qDebug() << "Random generator " << rand();
    }
}


void Editor::setup_signals(void)
{
    // Создание сигналов, генерируемых кнопками форматирования текста
    connect(_bold, &FlatToolButton::clicked,        this, &Editor::on_bold_clicked);
    connect(_italic, &FlatToolButton::clicked,      this, &Editor::on_italic_clicked);
    connect(_underline, &FlatToolButton::clicked,   this, &Editor::on_underline_clicked);
    connect(_monospace, &FlatToolButton::clicked,   this, &Editor::on_monospace_clicked);
    connect(_code, &FlatToolButton::clicked,        this, &Editor::on_code_clicked);
    connect(_clear, &FlatToolButton::clicked,       this, &Editor::on_clear_clicked);

    connect(_numeric_list, &FlatToolButton::clicked, this, &Editor::on_numericlist_clicked);
    connect(_dot_list, &FlatToolButton::clicked,     this, &Editor::on_dotlist_clicked);

    connect(_indent_plus, &FlatToolButton::clicked,  this, &Editor::on_indentplus_clicked);
    connect(_indent_minus, &FlatToolButton::clicked, this, &Editor::on_indentminus_clicked);

    connect(_align_left, &FlatToolButton::clicked,   this, &Editor::on_alignleft_clicked);
    connect(_align_center, &FlatToolButton::clicked, this, &Editor::on_aligncenter_clicked);
    connect(_align_right, &FlatToolButton::clicked,  this, &Editor::on_alignright_clicked);
    connect(_align_width, &FlatToolButton::clicked,  this, &Editor::on_alignwidth_clicked);

    connect(_font_select, &FlatFontComboBox::currentFontChanged, this, &Editor::on_fontselect_changed);
    void (FlatComboBox::*_currentIndexChanged)(int index) = &FlatComboBox::currentIndexChanged;
    connect(_font_size, _currentIndexChanged, this, &Editor::on_fontsize_changed);
    connect(_font_color, &FlatToolButton::clicked, this, &Editor::on_fontcolor_clicked);

    connect(_show_html, &FlatToolButton::clicked, this, &Editor::on_showhtml_clicked);
    connect(_find_text, &FlatToolButton::clicked, this, &Editor::on_findtext_clicked);
    connect(_settings, &FlatToolButton::clicked, this, &Editor::on_settings_clicked);
    connect(_show_formatting, &FlatToolButton::clicked, this, &Editor::on_showformatting_clicked);

    // Работа с таблицами
    connect(_create_table, &FlatToolButton::clicked,    this, &Editor::on_createtable_clicked);
    connect(_table_remove_row, &FlatToolButton::clicked, this, &Editor::on_table_remove_row_clicked);
    connect(_table_remove_col, &FlatToolButton::clicked, this, &Editor::on_table_remove_col_clicked);
    connect(_table_add_row, &FlatToolButton::clicked,    this, &Editor::on_table_add_row_clicked);
    connect(_table_add_col, &FlatToolButton::clicked,    this, &Editor::on_table_add_col_clicked);
    connect(_table_merge_cells, &FlatToolButton::clicked, this, &Editor::on_table_merge_cells_clicked);
    connect(_table_split_cell, &FlatToolButton::clicked, this, &Editor::on_table_split_cell_clicked);

    connect(_insert_image_from_file, &FlatToolButton::clicked, this, &Editor::on_insert_image_from_file_clicked);
    connect(_expand_edit_area, &FlatToolButton::clicked, this, &Editor::on_expand_edit_area_clicked);
    connect(_expand_tools_lines, &FlatToolButton::clicked, this, &Editor::on_expand_tools_lines_clicked);
    connect(_save, &FlatToolButton::clicked, this, &Editor::on_save_clicked);
    connect(_back, &FlatToolButton::clicked, this, &Editor::on_back_clicked);
    connect(_freeze, &FlatToolButton::clicked, this, &Editor::on_freeze_clicked);

    connect(_find_in_base, &FlatToolButton::clicked, this, &Editor::on_find_in_base_clicked);
    connect(_show_text, &FlatToolButton::clicked, this, &Editor::on_show_text_clicked);
    connect(_to_attach, &FlatToolButton::clicked, this, &Editor::on_to_attach_clicked);

    connect(_text_area, &EditorTextArea::cursorPositionChanged, this, &Editor::on_cursor_position_changed);
    connect(_text_area, &EditorTextArea::selectionChanged,      this, &Editor::on_selection_changed);

    connect(_indent_slider, &IndentSlider::change_textindent_pos, this, &Editor::on_indentline_change_textindent_pos);
    connect(_indent_slider, &IndentSlider::change_leftindent_pos, this, &Editor::on_indentline_change_leftindent_pos);
    connect(_indent_slider, &IndentSlider::change_rightindent_pos, this, &Editor::on_indentline_change_rightindent_pos);
    connect(_indent_slider, &IndentSlider::mouse_release,            this, &Editor::on_indentline_mouse_release);

    connect(this, &Editor::send_set_textindent_pos, _indent_slider, &IndentSlider::set_textindent_pos);
    connect(this, &Editor::send_set_leftindent_pos, _indent_slider, &IndentSlider::set_leftindent_pos);
    connect(this, &Editor::send_set_rightindent_pos, _indent_slider, &IndentSlider::set_rightindent_pos);

    // Соединение сигнал-слот чтобы показать контекстное меню по правому клику в редакторе
    connect(_text_area, &EditorTextArea::customContextMenuRequested, this, &Editor::on_customContextMenuRequested);

    // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
    connect(_text_area, &EditorTextArea::tap_and_hold_gesture_finished, this, &Editor::on_customContextMenuRequested);

    // connect(textArea->document(), SIGNAL(modificationChanged (bool )),
    //         this, &Editor::(onModificationChanged(bool)));


    // Сигналы контекстного меню
    connect(_editor_context_menu, &EditorContextMenu::undo, this, &Editor::on_undo);
    connect(_editor_context_menu, &EditorContextMenu::redo, this, &Editor::on_redo);
    connect(_editor_context_menu, &EditorContextMenu::cut, this, &Editor::on_cut);
    connect(_editor_context_menu, &EditorContextMenu::copy, this, &Editor::on_copy);
    connect(_editor_context_menu, &EditorContextMenu::paste, this, &Editor::on_paste);
    connect(_editor_context_menu, &EditorContextMenu::selectAll, this, &Editor::on_selectAll);
    connect(_editor_context_menu, &EditorContextMenu::contextMenuEditImageProperties, this, &Editor::on_context_menu_edit_image_properties);

    // Вызов диалога поиска в тексте
    connect(_find_dialog, &EditorFindDialog::find_text, this, &Editor::on_findtext_signal_detect);

    connect(_text_area, &EditorTextArea::update_indentline_geometry_signal, this, &Editor::on_update_indentline_geometry_slot);
    connect(this->_close_button, &FlatToolButton::clicked, this, &Editor::widget_hide);

}


// Создание объектов кнопок для линейки форматирования текста
void Editor::setup_buttons(void)
{
    // Для того, чтобы WyEdit нормально добавлял кнопки на панель согласно файлу editorconf.ini,
    // имена объектов кнопок должны начинаться на "editor_tb"

    // To WyEdit normally add buttons to the panel according to the file editorconf.ini,
    // Object names must begin at the buttons "editor_tb"

    // Кнопка Bold
    _bold = new FlatToolButton(this);
    _bold->setAutoRaise(true);
    _bold->setShortcut(QKeySequence(QKeySequence::Bold));
    _bold->setStatusTip(tr("Bold (Ctrl+B)"));
    _bold->setIcon(  // style()->standardIcon(QStyle::SP_TitleBarShadeButton, 0, this) //
        QIcon(":/resource/pic/edit_bold.svg"));
    _bold->setObjectName("editor_tb_bold");


    // Кнопка Italic
    _italic = new FlatToolButton(this);
    _italic->setShortcut(QKeySequence(QKeySequence::Italic));
    _italic->setStatusTip(tr("Italic (Ctrl+I)"));
    _italic->setIcon(QIcon(":/resource/pic/edit_italic.svg"));
    _italic->setObjectName("editor_tb_italic");
    _italic->setAutoRaise(true);
    static_cast<FlatToolButton *>(_italic)->setAutoRaise(false);

    // Кнопка Underline
    _underline = new FlatToolButton(this);
    _underline->setShortcut(QKeySequence(QKeySequence::Underline));
    _underline->setStatusTip(tr("Underline (Ctrl+U)"));
    _underline->setIcon(QIcon(":/resource/pic/edit_underline.svg"));
    _underline->setObjectName("editor_tb_underline");
    _underline->setAutoRaise(true);
    // Кнопка Monospace
    _monospace = new FlatToolButton(this);
    _monospace->setShortcut(QKeySequence(tr("Ctrl+T")));
    _monospace->setStatusTip(tr("Monospace (Ctrl+T)"));
    _monospace->setIcon(QIcon(":/resource/pic/edit_monospace.svg"));
    _monospace->setObjectName("editor_tb_monospace");
    _monospace->setAutoRaise(true);
    // Кнопка Code
    _code = new FlatToolButton(this);
    _code->setShortcut(QKeySequence(tr("Ctrl+M")));
    _code->setStatusTip(tr("Code (Ctrl+M). Select a whole paragraphs to format text as code."));
    _code->setIcon(QIcon(":/resource/pic/edit_code.svg"));
    _code->setObjectName("editor_tb_code");
    _code->setAutoRaise(true);
    // Кнопка Clear
    _clear = new FlatToolButton(this);
    _clear->setShortcut(QKeySequence(tr("Ctrl+K")));
    _clear->setStatusTip(tr("Reset format (Ctrl+K). When selected whole paragraph both text and paragraph format is reset to default or just text format in other case."));
    _clear->setIcon(QIcon(":/resource/pic/edit_clear.svg"));
    _clear->setObjectName("editor_tb_clear");
    _clear->setAutoRaise(true);

    // Кнопка нумерованного списка
    _numeric_list = new FlatToolButton(this);
    _numeric_list->setStatusTip(tr("Numeric list"));
    _numeric_list->setIcon(QIcon(":/resource/pic/edit_listnumeric.svg"));
    _numeric_list->setObjectName("editor_tb_numericlist");
    _numeric_list->setAutoRaise(true);
    // Кнопка списка с точками
    _dot_list = new FlatToolButton(this);
    _dot_list->setStatusTip(tr("Marked list"));
    _dot_list->setIcon(QIcon(":/resource/pic/edit_listdot.svg"));
    _dot_list->setObjectName("editor_tb_dotlist");
    _dot_list->setAutoRaise(true);

    // Кнопка увеличения отступа
    _indent_plus = new FlatToolButton(this);
    _indent_plus->setStatusTip(tr("Increase indent"));
    _indent_plus->setIcon(QIcon(":/resource/pic/edit_indentplus.svg"));
    _indent_plus->setObjectName("editor_tb_indentplus");

    // Кнопка уменьшения отступа
    _indent_minus = new FlatToolButton(this);
    _indent_minus->setStatusTip(tr("Decrease indent"));
    _indent_minus->setIcon(QIcon(":/resource/pic/edit_indentminus.svg"));
    _indent_minus->setObjectName("editor_tb_indentminus");


    // Кнопка выравнивания по левому краю
    _align_left = new FlatToolButton(this);
    _align_left->setShortcut(QKeySequence(tr("Ctrl+L")));
    _align_left->setStatusTip(tr("Align left (Ctrl+L)"));
    _align_left->setIcon(QIcon(":/resource/pic/edit_alignleft.svg"));
    _align_left->setObjectName("editor_tb_alignleft");

    // Кнопка выравнивания по центру
    _align_center = new FlatToolButton(this);
    _align_center->setShortcut(QKeySequence(tr("Ctrl+E")));
    _align_center->setStatusTip(tr("Align center (Ctrl+E)"));
    _align_center->setIcon(QIcon(":/resource/pic/edit_aligncenter.svg"));
    _align_center->setObjectName("editor_tb_aligncenter");

    // Кнопка выравнивания по правому краю
    _align_right = new FlatToolButton(this);
    _align_right->setShortcut(QKeySequence(tr("Ctrl+R")));
    _align_right->setStatusTip(tr("Align right (Ctrl+R)"));
    _align_right->setIcon(QIcon(":/resource/pic/edit_alignright.svg"));
    _align_right->setObjectName("editor_tb_alignright");

    // Кнопка выравнивания по ширине
    _align_width = new FlatToolButton(this);
    _align_width->setShortcut(QKeySequence(tr("Ctrl+J")));
    _align_width->setStatusTip(tr("Align width (Ctrl+J)"));
    _align_width->setIcon(QIcon(":/resource/pic/edit_alignwidth.svg"));
    _align_width->setObjectName("editor_tb_alignwidth");


    // Выбор шрифта
    _font_select = new FlatFontComboBox(this);    // QFontComboBox(this);
    _font_select->setObjectName("editor_tb_fontselect");

    // Пустой пункт в конце списка шрифтов, используется для обозначения что в
    // выделенном тексте несколько шрифтов
    _font_select->addItem(" ");
    _font_select->setItemIcon((_font_select->count()) - 1, QIcon(":/resource/pic/edit_font_many.svg"));


    // Выбор размера шрифта
    _font_size = new FlatComboBox(this);

    for(int i = MINIMUM_ALLOWED_FONT_SIZE; i <= MAXIMUM_ALLOWED_FONT_SIZE; i++)
        _font_size->addItem(QString("%1").arg(i), i);

    _font_size->setCurrentIndex(_font_size->findData(10));
    _font_size->setMinimumContentsLength(3);
    _font_size->setEditable(true);
    QValidator *fontsizeValidator = new QIntValidator(MINIMUM_ALLOWED_FONT_SIZE, MAXIMUM_ALLOWED_FONT_SIZE, this);
    _font_size->setValidator(fontsizeValidator);
    _font_size->setObjectName("editor_tb_fontsize");

    // Пустой пункт в конце списка размеров шрифтов, используется для обозначения
    // что в выделенном тексте несколько размеров
    _font_size->addItem(" ", 0);


    // Кнопка выбора цвета шрифта
    _font_color = new FlatToolButton(this);
    // fontColor->setShortcut(QKeySequence(tr("Ctrl+F")));
    _font_color->setStatusTip(tr("Text color"));
    _font_color->setIcon(QIcon(":/resource/pic/edit_fontcolor.svg"));
    _font_color->setObjectName("editor_tb_fontcolor");


    // Кнопка вызова виджета поиска текста
    _find_text = new FlatToolButton(this);
    _find_text->setShortcut(QKeySequence(tr("Ctrl+F")));
    _find_text->setStatusTip(tr("Find text (Ctrl+F)"));
    _find_text->setIcon(QIcon(":/resource/pic/edit_findtext.svg"));
    _find_text->setObjectName("editor_tb_findtext");

    // Кнопка вызова виджета конфигурирования редактора
    _settings = new FlatToolButton(this);
    _settings->setStatusTip(tr("Editor settings"));
    //    settings->setIcon(QIcon(":/resource/pic/edit_settings.svg"));
    _settings->setIcon(QIcon(":/resource/pic/cogwheel.svg"));
    _settings->setObjectName("editor_tb_settings");

    // Кнопка просмотра HTML кода
    _show_html = new FlatToolButton(this);
    _show_html->setStatusTip(tr("Edit HTML code"));
    _show_html->setIcon(QIcon(":/resource/pic/edit_showhtml.svg"));
    _show_html->setObjectName("editor_tb_showhtml");

    // Кнопка включения отображения символов фарматирования
    _show_formatting = new FlatToolButton(this);
    _show_formatting->setStatusTip(tr("Show special chars"));
    _show_formatting->setIcon(QIcon(":/resource/pic/edit_showformatting.svg"));
    _show_formatting->setObjectName("editor_tb_showformatting");

    // Кнопка добавления новой таблицы
    _create_table = new FlatToolButton(this);
    _create_table->setStatusTip(tr("Create a new table"));
    _create_table->setIcon(QIcon(":/resource/pic/edit_createtable.svg"));
    _create_table->setObjectName("editor_tb_createtable");

    _table_remove_row = new FlatToolButton(this);
    _table_remove_row->setStatusTip(tr("Remove row(s)"));
    _table_remove_row->setIcon(QIcon(":/resource/pic/edit_table_remove_row.svg"));
    _table_remove_row->setObjectName("editor_tb_table_remove_row");

    _table_remove_col = new FlatToolButton(this);
    _table_remove_col->setStatusTip(tr("Remove column(s)"));
    _table_remove_col->setIcon(QIcon(":/resource/pic/edit_table_remove_col.svg"));
    _table_remove_col->setObjectName("editor_tb_table_remove_col");

    _table_add_row = new FlatToolButton(this);
    _table_add_row->setStatusTip(tr("Add row(s)"));
    _table_add_row->setIcon(QIcon(":/resource/pic/edit_table_add_row.svg"));
    _table_add_row->setObjectName("editor_tb_table_add_row");

    _table_add_col = new FlatToolButton(this);
    _table_add_col->setStatusTip(tr("Add column(s)"));
    _table_add_col->setIcon(QIcon(":/resource/pic/edit_table_add_col.svg"));
    _table_add_col->setObjectName("editor_tb_table_add_col");

    _table_merge_cells = new FlatToolButton(this);
    _table_merge_cells->setStatusTip(tr("Merge cells"));
    _table_merge_cells->setIcon(QIcon(":/resource/pic/edit_table_merge_cells.svg"));
    _table_merge_cells->setObjectName("editor_tb_table_merge_cells");

    _table_split_cell = new FlatToolButton(this);
    _table_split_cell->setStatusTip(tr("Split cell"));
    _table_split_cell->setIcon(QIcon(":/resource/pic/edit_table_split_cell.svg"));
    _table_split_cell->setObjectName("editor_tb_table_split_cell");

    _insert_image_from_file = new FlatToolButton(this);
    _insert_image_from_file->setStatusTip(tr("Insert image from file / edit image properties of selected image"));
    _insert_image_from_file->setIcon(QIcon(":/resource/pic/edit_insert_image_from_file.svg"));
    _insert_image_from_file->setObjectName("editor_tb_insert_image_from_file");

    _expand_edit_area = new FlatToolButton(this);
    _expand_edit_area->setStatusTip(tr("Expand edit area"));
    _expand_edit_area->setIcon(QIcon(":/resource/pic/edit_expand_text_area.svg"));
    _expand_edit_area->setObjectName("editor_tb_expand_edit_area");

    _expand_tools_lines = new FlatToolButton(this);
    _expand_tools_lines->setStatusTip(tr("Expand tools"));
    _expand_tools_lines->setIcon(QIcon(":/resource/pic/edit_expand_tools_lines.svg"));
    _expand_tools_lines->setObjectName("editor_tb_expand_tools_lines");

    _save = new FlatToolButton(this);
    _save->setStatusTip(tr("Save (Ctrl+S)"));
    _save->setShortcut(QKeySequence(tr("Ctrl+S")));
    _save->setIcon(QIcon(":/resource/pic/edit_save.svg"));
    _save->setObjectName("editor_tb_save");


    // Кнопка "назад", используется в мобильном интерфейсе
    _back = new FlatToolButton(this);
    _back->setStatusTip(tr("Back"));
    _back->setIcon(QIcon(":/resource/pic/mobile_back.svg"));
    _back->setObjectName("editor_tb_back");

    _freeze = new FlatToolButton(this);
    _freeze->setStatusTip(tr("Pin / freeze browser view"));
    _freeze->setIcon(QIcon(":/resource/pic/pentalpha.svg"));
    _freeze->setObjectName("editor_tb_freeze_browser_view");

    // Кнопка "поиск по базе", используется в мобильном интерфейсе
    _find_in_base = new FlatToolButton(this);
    _find_in_base->setStatusTip(tr("Find in base"));
    _find_in_base->setIcon(QIcon(":/resource/pic/find_in_base.svg"));
    _find_in_base->setObjectName("editor_tb_find_in_base");


    // Кнопка "показать текст" для просмотра текста в отдельном окне
    _show_text = new FlatToolButton(this);
    _show_text->setStatusTip(tr("Show text in detached window"));
    _show_text->setIcon(QIcon(":/resource/pic/edit_show_text.svg"));
    _show_text->setObjectName("editor_tb_show_text");

    // Кнопка переключения на аттачи
    _icon_attach_not_exists = QIcon(":/resource/pic/attach.svg");
    _icon_attach_exists = QIcon(":/resource/pic/attach_exists.svg");
    _to_attach = new FlatToolButton(this);
    _to_attach->setStatusTip(tr("Show attach files"));
    _to_attach->setIcon(_icon_attach_not_exists);
    _to_attach->setObjectName("editor_tb_attach");

    // Виджет настройки отступов
    _indent_slider = new IndentSlider(this->width(), 16, this);
    _indent_slider->setObjectName("editor_tb_indentslider");


    // Все только что созданные элементы скрываются
    hide_all_tools_elements();
}

void Editor::setup_closebutton(void)
{
    // Кнопка закрытия виджета
    _close_button = new FlatToolButton(this);
    _close_button->setVisible(true);
    _close_button->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton)); // SP_TitleBarCloseButton SP_DialogCloseButton
    _close_button->setAutoRaise(true);

    if(appconfig.getInterfaceMode() == "desktop") {
        int w = _close_button->geometry().width();
        int h = _close_button->geometry().height();
        int x = min(w, h) / 2; // imin(w, h) / 2;
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
    _toolsarea_of_close_button->setContentsMargins(0, 0, 0, 0);
    _toolsarea_of_close_button->addWidget(_close_button);
    _toolsarea_of_close_button->addStretch();
}


// Полное сокрытие виджета
void Editor::widget_hide(bool checked)
{
    Q_UNUSED(checked);
    //    // Запоминается размер сплиттера перед скрытием виджета
    //    QSplitter *findSplitterRel = find_object<QSplitter>("find_splitter");
    //    appconfig.set_findsplitter_size_list(findSplitterRel->sizes());

    // Виджет скрывается
    appconfig.set_editor_show(false);
    this->close();
}


void Editor::hide_all_tools_elements(void)
{
    QRegExp nameMask("editor_tb_.*");

    // QList<QWidget *> tb_tools_list=qFindChildren(qobject_cast<QObject *>(this),name_mask);
    QList<QWidget *> tbToolsList = this->findChildren<QWidget *>(nameMask);

    for(int i = 0; i < tbToolsList.size(); ++i)
        tbToolsList.at(i)->setVisible(false);
}


// Создание объекта области редактирования
void Editor::setup_editor_area(void)
{
    // Создается область редактирования
    _text_area = new EditorTextArea(this);
    _text_area->setObjectName("textArea");
    _text_area->setContextMenuPolicy(Qt::CustomContextMenu); // Меню определяется в программе

    QFont font;
    font.fromString(_editor_config->get_default_font());
    font.setPointSize(_editor_config->get_default_font_size());
    _text_area->selectAll();
    _text_area->setCurrentFont(font);
    _text_area->setFont(font);
}


// В линейку кнопок вставляется нужный инструмент
// Метод принимает имя инструмента
// и указатель на линейку
void Editor::insert_button_to_tools_line(QString toolName, QToolBar *line)
{
    // qDebug() << "Editor::insert_button_to_tools_line() disableToolList : " << disableToolList;

    if(toolName == "separator") {
        line->addSeparator();
    } else {
        QString name("editor_tb_" + toolName);
        QWidget *tool = this->findChild<QWidget *>(name);

        if(tool != nullptr) {
            // Если данный инструмент не содержится в списке заблокированных
            if(!_disable_tool_list.contains(toolName)) {
                tool->setVisible(true);

                line->addWidget(tool); // Инструмент добавляется на панель инструментов

                FlatToolButton *tb = qobject_cast<FlatToolButton *>(tool);

                if(tb != 0) {
                    tb->setAutoRaise(true);              // false
                }
            } else {
                FlatToolButton *tb = qobject_cast<FlatToolButton *>(tool);

                if(tb != 0) {
                    tb->setEnabled(false);
                }
            }
        } else
            critical_error("WyEdit: Can not find editor tool with name '" + toolName + "'. Please check editor *.ini file");
    }
}


// Сборка линейки редактирования текста в горизонтальную линейку
void Editor::assembly_buttons(void)
{
    // Заполнение горизонтальных линеек с кнопками форматирования текста
    _tools_line_0 = new QToolBar();
    _tools_line_1 = new QToolBar();

    update_tools_lines();

    /*
    QSize toolIconSize(16, 16);
    toolsLine1->setIconSize(toolIconSize);
    toolsLine2->setIconSize(toolIconSize);
    */

    QHBoxLayout *toolsLine1Layout = new QHBoxLayout();
    toolsLine1Layout->addWidget(_tools_line_0);
    toolsLine1Layout->addStretch();
    toolsLine1Layout->addLayout(_toolsarea_of_close_button);

    // Горизонтальные линейки собираются
    _textformat_buttons_layout = new QVBoxLayout();

    // textformatButtonsLayout->addWidget(toolsLine1);
    _textformat_buttons_layout->addLayout(toolsLine1Layout);
    _textformat_buttons_layout->addWidget(_tools_line_1);

    // Виджет настройки отступов виден только в desktop интерфейсе
    if(_view_mode == WYEDIT_DESKTOP_MODE) {
        _indent_slider->setVisible(true);
        _textformat_buttons_layout->addWidget(_indent_slider);
    } else
        _indent_slider->setVisible(false);

}


void Editor::update_tools_lines(void)
{
    for(int i = 0; i < _tools_list_in_line_0.size(); ++i) {
        QString b = _tools_list_in_line_0.at(i).trimmed();
        insert_button_to_tools_line(b, _tools_line_0);
    }

    for(int i = 0; i < _tools_list_in_line_1.size(); ++i) {
        QString b = _tools_list_in_line_1.at(i).trimmed();
        insert_button_to_tools_line(b, _tools_line_1);
    }
}


// Сборка редактора и его активизация
void Editor::assembly(void)
{
    // Создается вертикальная область, которая должна объединить
    // линейку кнопок редактирования и область редактирования текста
    _buttons_and_edit_layout = new QVBoxLayout(this);
    _buttons_and_edit_layout->setObjectName("buttons_and_edit_layout");

    // Добавляются линейки кнопок
    _buttons_and_edit_layout->addLayout(_textformat_buttons_layout);

    // Добавляется область редактирования
    _buttons_and_edit_layout->addWidget(_text_area);    // ?

    // Полученый набор элементов устанавливается для текущего окна
    setLayout(_buttons_and_edit_layout);

    // Границы убираются, так как данный объект будет использоваться как виджет
    QLayout *lt;
    lt = layout();
    lt->setContentsMargins(0, 2, 0, 0);
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


// Установка текста области редактирования
void Editor::textarea(QString text)
{
    _text_area->setHtml(text);

    // Очищается URL документа, так как документ создается "из ничего"
    _text_area->document()->setMetaInformation(QTextDocument::DocumentUrl, "");
}


// Установка запрета или разрешения редактирования
void Editor::textarea_editable(bool editable)
{
    if(editable == true) {
        // Если редактирование разрешено
        _text_area->setTextInteractionFlags(Qt::TextEditorInteraction);
        _text_area->setPalette(qApp->palette());
    } else {
        // Если редактирование запрещено
        _text_area->setTextInteractionFlags(Qt::TextBrowserInteraction);

        QPalette pal = qApp->palette();
        QColor inactiveColor;

        inactiveColor = qApp->palette().color(QPalette::Disabled, QPalette::Window);
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



QTextDocument *Editor::textarea_document(void)
{
    return _text_area->document();
}


bool Editor::work_directory(QString dir_name)
{
    QDir directory(dir_name);
    //    QDir directory_child(dir_name);

    if(directory.exists()) {
        _work_directory = dir_name;
        return true;
    } else {
        QString child_dir = directory.dirName();
        QString path = directory.absolutePath();
        path.remove(child_dir);
        directory.setPath(path);
        //        directory.rmdir(child_dir);// "mytetra" + get_unical_id();

        bool result = directory.mkdir(
                          child_dir // _work_directory // short_dir()
                      );

        if(!result) {
            critical_error("Editor::work_directory(QString dir_name) : Can't create directory '" + _work_directory + "'");
        }

        _work_directory = dir_name;
        //        critical_error("WyEdit: Can not set work directory to " + dirName + ". Directory not exists.");
        return result;  // false;
    }
}


QString Editor::work_directory(void)
{
    return _work_directory;
}


void Editor::file_name(QString _file_name)
{

    //    QString fileName = full_text_file_name();
    if(_file_name.size() > 0) {
        QFile f(_file_name);
        QFileInfo fileInfo(f);

        if(!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::ReadOnly))
            critical_error("Editor::file_name(QString _file_name) : Can\'t open text file " + _file_name + " for read / write.");

        if(fileInfo.absoluteDir().exists() && f.exists()) {
            _work_file_name = _file_name;
        }
    } else {
        _work_file_name = "";
    }
}


QString Editor::file_name(void)
{
    return _work_file_name;
}


// Function that replaces the standard function for reading Editor
// Editable text
// She is editor of passing a pointer to itself
// And variable reference loadText, which must be filled
// Attention! The method does not contain the data recording operation. Think about where to place it
void Editor::editor_load_callback(QObject *editor, QString &loadText)
{
    // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

    // Ссылка на объект редактора
    Editor *currEditor = qobject_cast<Editor *>(editor);

    // Нужно ли дешифровать данные при чтении
    bool workWithCrypt = false;

    if(currEditor->misc_field("crypt") == "1") {
        // Если не установлено ключа шифрации
        if(globalparameters.crypt_key().length() == 0) {
            loadText = "";
            return;
        }

        workWithCrypt = true;
    }

    // Файл, с которым работает редактор
    QString fileName = currEditor->work_directory() + "/" + currEditor->file_name();

    QFile f(fileName);

    if(f.exists() && f.open(QIODevice::ReadOnly)) {

        //    // Если нужный файл не существует
        //    if(!f.exists())
        //        critical_error("File " + fileName + " not found");

        //    // Открывается файл
        //    if(!f.open(QIODevice::ReadOnly))
        //        critical_error("File " + fileName + " not readable. Check permission.");

        // Если незашифровано
        if(workWithCrypt == false)
            loadText = QString::fromUtf8(f.readAll());
        else
            loadText = CryptService::decryptStringFromByteArray(globalparameters.crypt_key(), f.readAll()); // Если зашифровано
    } else {
        loadText = "";
    }
}


// Function that replaces the standard function of the editor to set down
// Editable text
// She is editor of passing a pointer to itself
// And the text to be written in the variable saveText
// Attention! The method does not contain the data recording operation. Think about where to place it
void Editor::editor_save_callback(QObject *editor, QString saveText)
{
    // qDebug() << "RecordTableScreen::editor_load_callback() : Dir" << dir << "File" << file;

    // Ссылка на объект редактора
    Editor *currEditor = qobject_cast<Editor *>(editor);

    // Нужно ли шифровать записываемый текст
    bool workWithCrypt = false;

    if(currEditor->misc_field("crypt") == "1") {
        // Если не установлено ключа шифрации
        if(globalparameters.crypt_key().length() == 0)
            return;

        workWithCrypt = true;
    }

    QString fileName = currEditor->work_directory() + "/" + currEditor->file_name();

    // Если шифровать ненужно
    if(workWithCrypt == false) {
        // Текст сохраняется в файл
        QFile wfile(fileName);

        if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text))
            critical_error("Editor::editor_save_callback() : Can\'t open text file " + fileName + " for write.");

        QTextStream out(&wfile);
        out.setCodec("UTF-8");
        out << saveText;
    } else {
        // Текст шифруется
        QByteArray encryptData = CryptService::encryptStringToByteArray(globalparameters.crypt_key(), saveText);

        // В файл сохраняются зашифрованные данные
        QFile wfile(fileName);

        if(!wfile.open(QIODevice::WriteOnly))
            critical_error("Editor::editor_save_callback() : Can\'t open binary file " + fileName + " for write.");

        wfile.write(encryptData);
    }


    // Вызывается сохранение картинок
    // В данной реализации картинки сохраняются незашифрованными
    // Called saving images // In this embodiment, the image is not stored encrypted
    currEditor->save_textarea_images(Editor::SAVE_IMAGES_REMOVE_UNUSED);
}


// Сохранение HTML кода документа в файл
bool Editor::save_textarea_text()
{
    if(_work_file_name.length() == 0) {
        critical_error("WyEdit: Save function. Not setted work file name for editor.");
        return false;
    }

    QString fullFileName = _work_directory + "/" + _work_file_name;

    // Создание нового файла записи
    QFile wfile(fullFileName);

    if(!wfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        critical_error("WyEdit: Cant open file " + fullFileName + " for write.");
        return false;
    }

    qDebug() << "Write to file " << _work_file_name;

    QTextStream out(&wfile);
    QString content = _text_area->document()->toHtml("UTF-8");
    out.setCodec("UTF-8");
    out << content;

    return true;
}


// Сохранение картинок, которые присутствуют в документе,
// в указанную директорию
bool Editor::save_textarea_images(int mode = SAVE_IMAGES_SIMPLE)
{
    qDebug() << "Save images...\n" ;
    qDebug() << "Block count" << _text_area->document()->blockCount() << "\n";

    if(_work_directory.length() == 0) {
        critical_error("WyEdit: Save images function. Not setted work directory.");
        return false;
    }

    // Перебираются блоки документа и находятся блоки с картинками
    QStringList imagesNames; // В список сохраняются имена найденных картинок
    QTextBlock textBlock = _text_area->document()->begin();

    while(textBlock.isValid()) {
        QTextBlock::iterator it;

        for(it = textBlock.begin(); !(it.atEnd()); ++it) {
            QTextFragment currentFragment = it.fragment();

            if(currentFragment.isValid()) {
                if(currentFragment.charFormat().isImageFormat()) {
                    // Найден блок с картинкой

                    // Выясняется формат картинки
                    QTextImageFormat imgFmt = currentFragment.charFormat().toImageFormat();

                    // Из формата выясняется имя картинки
                    QString imageName = imgFmt.name();
                    imagesNames << imageName;
                    qDebug() << "Image " << imageName << "\n"; // имя файла

                    // Если файла картинки не существует
                    QString imageFileName = _work_directory + "/" + imageName;
                    QFileInfo tryFile(imageFileName);

                    if(tryFile.exists() == false) {
                        qDebug() << "Save image data to file " << imageFileName;

                        // Из ресурсов вытягивается картинка
                        QVariant imageData = _text_area->document()->resource(QTextDocument::ImageResource, QUrl(imageName));

                        QImage picture = imageData.value<QImage>();

                        picture.save(imageFileName, "png");
                    }
                }
            }
        }

        textBlock = textBlock.next();
    }


    // Если при сохранении все лишние картинки должны удалиться в каталоге
    if(mode == SAVE_IMAGES_REMOVE_UNUSED) {
        // Выясняется список файлов в директории
        QDir directory(_work_directory);
        QStringList imageInDirectory = directory.entryList(QDir::Files);

        // Перебираются файлы в директории
        foreach(QString fileName, imageInDirectory)
            if(fileName.contains(QRegExp("\\.png$"))) // Обрабатыватся только *.png файлы
                if(!imagesNames.contains(fileName)) {
                    // Если в списке картинок нет текущего png файла,
                    // значит этот файл лишний и он удаляется
                    QFile currentFile(_work_directory + "/" + fileName);
                    currentFile.remove();
                }
    }

    qDebug() << "Save images finish\n" ;

    return true;
}


void Editor::save_textarea(void)
{
    qDebug() << "Save textarea...";

    // Если запись была открыта на просмотр и изменена
    if(work_directory().length() != 0 &&
       file_name().length() != 0 &&
       textarea_modified() == true) {
        // Перенос текущего файла записи в корзину
        qDebug() << "Try remove file " << file_name() << " from directory " << work_directory();

        if(QFileInfo(work_directory() + "/" + file_name()).exists()) {
            qDebug() << "File exists. Remove it.";
            DiskHelper::remove_file_to_trash(work_directory() + "/" + file_name());
        } else
            qDebug() << "Cant remove file. File not exists.";

        // Если происходит прямая работа с файлом текста
        if(load_callback_func == nullptr) {
            // Сохранение текста записи в файл
            save_textarea_text();

            // Сохранение картинок, которые есть в записи
            save_textarea_images(Editor::SAVE_IMAGES_REMOVE_UNUSED);
        } else {
            // Иначе задана функция обратного вызова для записи текста и картинок

            QString content = _text_area->document()->toHtml("UTF-8");
            save_callback_func(qobject_cast<QObject *>(this), content);
        }

        // Так как произошло сохранение,
        // отмечается что новый текст небыл еще изменен
        textarea_modified(false);
    }
}


bool Editor::load_textarea()
{
    // Если не заданы директория или файл
    // Но установлен режим подавления ошибки
    if(_work_file_name.length() == 0 || _work_directory.length() == 0)
        if(_dir_file_empty_reaction == DIRFILEEMPTY_REACTION_SUPPRESS_ERROR) {
            // Никаких действий с областью редактирования не делается
            // скорее всего она останется просто пустой
            return true;
        }


    if(_work_file_name.length() == 0) {
        critical_error("WyEdit: Load function. Not setted work file name for editor.");
        return false;
    }

    if(_work_directory.length() == 0) {
        critical_error("WyEdit: Not setted work directory for editor.");
        return false;
    }

    QString fileName = _work_directory + "/" + _work_file_name;

    QString content;

    // Если происходит прямая работа с файлом текста
    if(load_callback_func == nullptr) {
        // Создается объект файла с нужным именем
        QFile f(fileName);

        // Если нужный файл не существует
        if(!f.exists()) {
            critical_error("WyEdit: File " + fileName + " not found");
            return false;
        }

        // Если нужный файл недоступен для чтения
        if(!f.open(QIODevice::ReadOnly)) {
            critical_error("WyEdit: File " + fileName + " not readable. Check permission.");
            return false;
        }

        content = QString::fromUtf8(f.readAll());
    } else {
        // Иначе задана функция обратного вызова для установки начального текста
        QString resultString;
        load_callback_func(qobject_cast<QObject *>(this), resultString);
        content = resultString;
    }

    // Устанавливается URL документа, с помощью него будут высчитываться
    // относительные ссылки при загрузке картинок
    _text_area->document()->setMetaInformation(QTextDocument::DocumentUrl, "file:" + fileName);

    qDebug() << "Editor::load_textarea() Set document URL to " << fileName;

    // Текст из файла вставляется в область редактирования
    // QString content=QString::fromUtf8(f.readAll());
    // qDebug() << "Load content:";
    // qDebug() << content;
    _text_area->setHtml(content);

    // qDebug() << "Set content:";
    // qDebug() << textArea->toHtml();

    return true;
}


void Editor::textarea_modified(bool modify)
{
    qDebug() << "Editor::set_textarea_modified() :" << modify;
    _text_area->document()->setModified(modify);
}


bool Editor::textarea_modified(void)
{
    qDebug() << "Editor::get_textarea_modified() :" << _text_area->document()->isModified();
    return _text_area->document()->isModified();
}


/////////////////////////////////////////////////
// Форматирование текста
/////////////////////////////////////////////////

// Форматирование Bold
void Editor::on_bold_clicked(void)
{
    // Если выделение есть
    if(_text_area->textCursor().hasSelection()) {
        // Обычное форматирование
        if(_text_area->fontWeight() != QFont::Bold)
            _text_area->setFontWeight(QFont::Bold); // Bold
        else
            _text_area->setFontWeight(0); // Remove Bold
    } else {
        // Иначе надо выделить дополнительным курсором слово на
        // котором стоит курсор
        QTextCursor cursor = _text_area->textCursor();
        cursor.select(QTextCursor::WordUnderCursor);

        // К выделению применяется форматирование Bold
        QTextCharFormat format;

        if(cursor.charFormat().fontWeight() != QFont::Bold)
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
    if(_text_area->textCursor().hasSelection()) {
        // Обычное форматирование
        if(!_text_area->fontItalic())
            _text_area->setFontItalic(true);
        else
            _text_area->setFontItalic(false);
    } else {
        // Иначе надо выделить дополнительным курсором слово на
        // котором стоит курсор
        QTextCursor cursor = _text_area->textCursor();
        cursor.select(QTextCursor::WordUnderCursor);

        // К выделению применяется форматирование Italic
        QTextCharFormat format;

        if(!cursor.charFormat().fontItalic())
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
    if(_text_area->textCursor().hasSelection()) {
        // Обычное форматирование
        if(!_text_area->fontUnderline())
            _text_area->setFontUnderline(true);
        else
            _text_area->setFontUnderline(false);
    } else {
        // Иначе надо выделить дополнительным курсором слово на
        // котором стоит курсор
        QTextCursor cursor = _text_area->textCursor();
        cursor.select(QTextCursor::WordUnderCursor);

        // К выделению применяется форматирование Underline
        QTextCharFormat format;

        if(!cursor.charFormat().fontUnderline())
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
    if(_editor_config->get_monospace_font_size_apply() == true) {
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

    if(cursor.atBlockStart() == true || cursor.atBlockEnd() == true)
        flagStartBlockEdge = 1;

    cursor.setPosition(stop);

    if(cursor.atBlockStart() == true || cursor.atBlockEnd() == true)
        flagStopBlockEdge = 1;

    if(flagStartBlockEdge == 1 && flagStopBlockEdge == 1) return true;
    else return false;
}


// Стоит ли курсор на пустой строке (т.е. в строке есть только символ
// перевода на новую строку)
bool Editor::is_cursor_on_empty_line(void)
{
    if(!_text_area->textCursor().atBlockStart()) return false;

    // Создаётся дополнительный курсор
    QTextCursor cursor = _text_area->textCursor();

    // Выделяется анализируемая строка
    cursor.select(QTextCursor::LineUnderCursor);

    // Текст анализируемой строки
    QString selectedText = cursor.selectedText();

    if(selectedText.size() > 0)return false;
    else return true;
}


bool Editor::is_cursor_on_space_line(void)
{
    // Если есть выделение, функция работать не должна
    if(_text_area->textCursor().hasSelection())
        return false;

    // Создаётся дополнительный курсор
    QTextCursor cursor = _text_area->textCursor();

    // Выделяется анализируемая строка
    cursor.select(QTextCursor::LineUnderCursor);

    // Текст анализируемой строки
    QString selectedText = cursor.selectedText();

    if(selectedText.simplified().size() > 0)
        return false;
    else
        return true;
}


// Форматирование в качестве кода
void Editor::on_code_clicked(void)
{
    // Если выделения нет
    if(!_text_area->textCursor().hasSelection()) return;

    bool enableIndent;

    // Проверяется, выбран ли четко блок (блоки) текста
    // Блок - это текст между <p>...</p>
    if(!is_block_select()) {
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

        qDebug() << "Code format action, block " << blockStart << blockStop << " selection " << selectStart << selectStop;

        if(blockStart <= selectStart && blockStop >= selectStop)
            enableIndent = false; // Выбран кусок текста в пределах блока
        else
            return;
    } else
        enableIndent = true; // Выбран четко блок (блоки) текста, нужно делать отступ

    _text_area->textCursor().beginEditBlock();

    // Устанавливается шрифт
    QFont font;
    font.fromString(_editor_config->get_code_font());
    _text_area->setFontFamily(font.family());

    // Новый установленный шрифт показывается в выпадающем списке шрифтов
    set_fontselect_on_display(font.family());

    // Если в настройках разрешена автоматическая установка нужного размера
    if(_editor_config->get_code_font_size_apply() == true) {
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
    if(_editor_config->get_code_indent_size_apply() == true &&
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


// Очистка форматирования, т.е. установка стандартного шрифта,
// размера и убирание утолщения, наклона, подчеркивания
void Editor::on_clear_clicked(void)
{
    int startCursorPos = _text_area->textCursor().position();
    int stopCursorPos = _text_area->textCursor().anchor();
    qDebug() << "Cursor start position: " << startCursorPos << "Cursor stop position: " << stopCursorPos;


    bool flag_cursor_on_empty_line = is_cursor_on_empty_line();
    bool flag_cursor_on_space_line = is_cursor_on_space_line();

    // Очистка возможна только если что-то выделено
    // Или курсор стоит на пустой строке с одним символом перевода строки
    // Или курсор стоит на строке, в которой нет текста
    if(!(_text_area->textCursor().hasSelection() ||
         flag_cursor_on_empty_line ||
         flag_cursor_on_space_line))
        return;

    _text_area->textCursor().beginEditBlock();

    if(flag_cursor_on_space_line)
        (_text_area->textCursor()).select(QTextCursor::LineUnderCursor);


    // Создается стандартный шрифт
    QFont font;
    font.fromString(_editor_config->get_default_font());   // Стандартное начертание
    font.setPointSize(_editor_config->get_default_font_size());   // Стандартный размер

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
    if(is_block_select() ||
       flag_cursor_on_empty_line ||
       flag_cursor_on_space_line) {
        QTextBlockFormat format;

        // Убираются отступы
        format.setLeftMargin(0);   // Убирается левый отступ (который, возможно был установлен слайдером или кнопками изменения отступа)
        format.setRightMargin(0);
        format.setTopMargin(0);    // Убираются межстрочные интервалы, которые самопроизвольно появляются при вставке из других программ
        format.setBottomMargin(0);
        format.setAlignment(Qt::AlignLeft); // Выравнивание по левому краю

        // Применение форматирование
        _text_area->textCursor().setBlockFormat(format);
    }


    // Если была работа со строкой, в которой нет символов,
    // курсор переносится на начало строки, чтобы не путать пользователя
    if(flag_cursor_on_space_line)
        _text_area->moveCursor(QTextCursor::StartOfLine);


    // Очистка закомментирована, так как она заменена очисткой формата символов setCurrentCharFormat()
    // А так же эта очистка некорректно работает из-за особенностей вставки в Qt (первая строка получает отличный от остальных строк формат).
    // Думать дальше
    /*
    // Удаление какого-либо форматирования стилем
    QString htmlCode=textArea->textCursor().selection().toHtml();
    qDebug() << "Before remove style: " << htmlCode;

    // В регулярных выражениях Qt кванторы по-умолчанию жадные (greedy)
    // Поэтому напрямую регвыру указывается что кванторы должны быть ленивые
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
    textArea->textCursor().setPosition(stopCursorPos, QTextCursor::KeepAnchor);
    // textArea->setTextCursor( textArea->textCursor() );

    qDebug() << "Cursor position: " << textArea->textCursor().position() << "Cursor anchor: " << textArea->textCursor().anchor();
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
    if(!_text_area->textCursor().hasSelection()) return;

    // Форматирование в список возможно только если выделен блок
    if(!is_block_select()) return;

    // Если строки выбраны
    if(_text_area->textCursor().hasSelection()) {
        if(_text_area->textCursor().currentList() == 0 ||
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
            currentIndent = (int) _text_area->textCursor().blockFormat().leftMargin();

            // Создание форматирования
            QTextBlockFormat indentFormatting;
            indentFormatting.setLeftMargin(currentIndent);

            // Форматирование
            _text_area->textCursor().setBlockFormat(indentFormatting);

            // Создание форматирования по умолчанию чтобы убрать форматирование в список
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
    // Работа с отступом возможна только если выделен блок
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
    currentIndent = (int) _text_area->textCursor().blockFormat().leftMargin();

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
    currentIndent = (int) _text_area->textCursor().blockFormat().leftMargin();

    if((currentIndent - 10) > 0)indentForSet = currentIndent - 10;
    else indentForSet = 0;

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
void Editor::on_fontselect_changed(const QFont &font)
{
    if(_flag_set_font_parameters_enabled == false) return;

    _text_area->setFontFamily(font.family());

    _current_font_family = font.family();

    // Курсор после выбора возвращается в область редактирования
    _text_area->setFocus();
}


// Метод только меняет значение, показываемое списком шрифтов
void Editor::set_fontselect_on_display(QString fontName)
{
    _flag_set_font_parameters_enabled = false;

    if(fontName.size() > 0)
        _font_select->setCurrentIndex(_font_select->findText(fontName));
    else
        _font_select->setCurrentIndex(_font_select->count() - 1);

    _current_font_family = fontName;

    _flag_set_font_parameters_enabled = true;
}


// Слот, срабатывающий когда изменен размер шрифта через список размеров
void Editor::on_fontsize_changed(int i)
{
    if(_flag_set_font_parameters_enabled == false) return;

    int n = (_font_size->itemData(i)).toInt();

    if(n < MINIMUM_ALLOWED_FONT_SIZE) return;

    if(n > MAXIMUM_ALLOWED_FONT_SIZE) return;

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
    if(selectedColor.isValid()) {
        // Меняется цвет кнопки
        _font_color->setPalette(QPalette(selectedColor));

        // Если выделение есть
        if(_text_area->textCursor().hasSelection())
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
    if(!_text_area->textCursor().hasSelection()) return;

    // qDebug() << "Selection text " << textarea->textCursor().selection().toHtml();

    // Позиции начала и конца выделения
    int start = _text_area->textCursor().selectionStart();
    int stop = _text_area->textCursor().selectionEnd();
    // qDebug() << "Selection start " << start << " stop " << stop;

    // Создается дополнительный курсор, который будет пробегать по выделенному тексту
    QTextCursor cursor = _text_area->textCursor();
    cursor.setPosition(start);

    // Выравнивание относится к форматированию строк, начальное состояние
    // берётся из начального положения курсора
    int startAlign = cursor.blockFormat().alignment();

    // Курсор сдвигается на одну позицию вперёд
    cursor.movePosition(QTextCursor::NextCharacter);

    // Для анализа форматирования символов надо начинать
    // с позиции, следующей справа от начала выделения
    QString startFontFamily = cursor.charFormat().fontFamily(); // Шрифт
    qreal startSize = cursor.charFormat().fontPointSize(); // Размер шрифта
    bool startBold = false;

    if(cursor.charFormat().fontWeight() == QFont::Bold) startBold = true; // Тощина

    bool startItalic = cursor.charFormat().fontItalic(); // Наклон
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
    while(cursor.position() <= stop) {
        if(differentFontFlag == 0 && startFontFamily != cursor.charFormat().fontFamily())
            differentFontFlag = 1;

        if(differentSizeFlag == 0 && startSize != cursor.charFormat().fontPointSize())
            differentSizeFlag = 1;

        if(differentBoldFlag == 0) {
            int b = cursor.charFormat().fontWeight();

            if(startBold == false && b == QFont::Bold)differentBoldFlag = 1;
            else if(startBold == true && b == QFont::Normal)differentBoldFlag = 1;
        }

        if(differentItalicFlag == 0 && startItalic != cursor.charFormat().fontItalic())
            differentItalicFlag = 1;

        if(differentUnderlineFlag == 0 && startUnderline != cursor.charFormat().fontUnderline())
            differentUnderlineFlag = 1;

        if(differentAlignFlag == 0 && startAlign != cursor.blockFormat().alignment())
            differentAlignFlag = 1;

        // Курсор передвигается на одну позицию вперед
        // Если дальше двигаться некуда (конец документа) цикл досрочно завершается
        if(cursor.movePosition(QTextCursor::NextCharacter) == false) break;
    }


    // Список выбора шрифта начинает указывать на нужный шрифт
    if(differentFontFlag == 0)
        set_fontselect_on_display(startFontFamily); // Если всё выделение одним шрифтом
    else
        set_fontselect_on_display("");

    // Список выбора размера начинает указывать на нужный размер
    if(differentSizeFlag == 0)
        set_fontsize_on_display((int)startSize); // Если всё отформатировано одним размером
    else
        set_fontsize_on_display(0); // В выделении есть разные размеры

    // Кнопка Bold выключается, если есть разное Bold форматирование
    // и включается, если форматирование одинаковое,
    // и выделение начиналось с Bold
    if(differentBoldFlag == 1)
        set_outline_button_higlight(BT_BOLD, false);
    else if(startBold == true)
        set_outline_button_higlight(BT_BOLD, true);

    // Кнопка Italic
    if(differentItalicFlag == 1)
        set_outline_button_higlight(BT_ITALIC, false);
    else if(startItalic == true)
        set_outline_button_higlight(BT_ITALIC, true);

    // Кнопка Underline
    if(differentUnderlineFlag == 1)
        set_outline_button_higlight(BT_UNDERLINE, false);
    else if(startUnderline == true)
        set_outline_button_higlight(BT_UNDERLINE, true);

    // Кнопки выравнивания
    if(differentAlignFlag == 1)
        update_align_button_higlight(false);
    else
        update_align_button_higlight(true);
}


void Editor::update_tool_line_to_actual_format(void)
{
    // Список должен показывать текущий шрифт позиции, где находится курсор
    if(_current_font_family != _text_area->fontFamily())
        set_fontselect_on_display(_text_area->fontFamily());

    // Размер
    if(_current_font_size != (int)_text_area->fontPointSize())
        set_fontsize_on_display((int)_text_area->fontPointSize());

    // Кнопки форматирования начертания
    update_outline_button_higlight();

    // Кнопки выравнивания
    update_align_button_higlight(true);
}


void Editor::update_indentslider_to_actual_format(void)
{
    int i;

    i = (int)_text_area->textCursor().blockFormat().textIndent();

    if(_current_text_indent != i) {
        emit send_set_textindent_pos(i);
        _current_text_indent = i;
    }

    i = (int)_text_area->textCursor().blockFormat().leftMargin();

    if(_current_left_indent != i) {
        emit send_set_leftindent_pos(i);
        _current_left_indent = i;
    }

    i = (int)_text_area->textCursor().blockFormat().rightMargin();

    if(_current_right_indent != i) {
        emit send_set_rightindent_pos(i);
        _current_right_indent = i;
    }
}


// Слот вызывается при каждом перемещении курсора
void Editor::on_cursor_position_changed(void)
{
    // Если одновременно идет режим выделения
    // то обслуживание текущего шрифта и размера идет в on_selection_changed()
    if(_text_area->textCursor().hasSelection()) return;

    update_tool_line_to_actual_format();

    update_indentslider_to_actual_format();
}


bool Editor::is_key_for_tool_line_update(QKeyEvent *event)
{
    if(event->modifiers().testFlag(Qt::ControlModifier) ||
       event->modifiers().testFlag(Qt::AltModifier) ||
       event->modifiers().testFlag(Qt::MetaModifier) ||
       event->key() == Qt::Key_F1 ||
       event->key() == Qt::Key_F2 ||
       event->key() == Qt::Key_F3 ||
       event->key() == Qt::Key_F4 ||
       event->key() == Qt::Key_F5 ||
       event->key() == Qt::Key_F6 ||
       event->key() == Qt::Key_F7 ||
       event->key() == Qt::Key_F8 ||
       event->key() == Qt::Key_F9 ||
       event->key() == Qt::Key_F10 ||
       event->key() == Qt::Key_F11 ||
       event->key() == Qt::Key_F12)
        return true;
    else
        return false;
}


// Cлот отлавливает нажатия клавиш
void Editor::keyPressEvent(QKeyEvent *event)
{
    if(is_key_for_tool_line_update(event))
        update_tool_line_to_actual_format();
}


// Cлот отлавливает отжатия клавиш
void Editor::keyReleaseEvent(QKeyEvent *event)
{
    if(is_key_for_tool_line_update(event))
        update_tool_line_to_actual_format();
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


void Editor::on_cut(void)
{
    _text_area->cut();
    update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}


void Editor::on_copy(void)
{
    // Если выбрана только картинка или курсор стоит на позиции картинки
    if(is_image_select() || is_cursor_on_image()) {
        QTextImageFormat imageFormat;

        if(is_image_select())
            imageFormat = image_format_on_select();

        if(is_cursor_on_image())
            imageFormat = image_format_on_cursor();

        // Из формата выясняется имя картинки
        QString imageName = imageFormat.name();

        // Из ресурсов вытягивается картинка
        QVariant imageData = _text_area->document()->resource(QTextDocument::ImageResource, QUrl(imageName));
        QImage image = imageData.value<QImage>();

        // Создается ссылка на буфер обмена
        QClipboard *clipboard = QApplication::clipboard();

        // Копирвание картинки в буфер обмена
        clipboard->setImage(image);
    } else
        _text_area->copy(); // Обычное копирование

    update_tool_line_to_actual_format(); // Обновляется панель с кнопками
}


void Editor::on_paste(void)
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

    if(activate == false)return;

    if(_text_area->alignment() == Qt::AlignLeft)         _align_left->setPalette(palActive);
    else if(_text_area->alignment() == Qt::AlignHCenter) _align_center->setPalette(palActive);
    else if(_text_area->alignment() == Qt::AlignRight)   _align_right->setPalette(palActive);
    else if(_text_area->alignment() == Qt::AlignJustify) _align_width->setPalette(palActive);
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

    if(_text_area->fontWeight() == QFont::Bold) _bold->setPalette(palActive);

    if(_text_area->fontItalic() == true)        _italic->setPalette(palActive);

    if(_text_area->fontUnderline() == true)     _underline->setPalette(palActive);
}


void Editor::set_outline_button_higlight(int button, bool active)
{
    QPalette palActive, palInactive;
    palActive.setColor(QPalette::Normal, QPalette::Button, _buttons_select_color);
    palActive.setColor(QPalette::Normal, QPalette::Window, _buttons_select_color);

    if(button == BT_BOLD) {
        if(active == false) _bold->setPalette(palInactive);
        else              _bold->setPalette(palActive);

        return;
    }

    if(button == BT_ITALIC) {
        if(active == false) _italic->setPalette(palInactive);
        else              _italic->setPalette(palActive);

        return;
    }

    if(button == BT_UNDERLINE) {
        if(active == false) _underline->setPalette(palInactive);
        else              _underline->setPalette(palActive);

        return;
    }
}


// Показывание окна с исходным текстом HTML
void Editor::on_showhtml_clicked(void)
{
    EditorMultiLineInputDialog dialog(this);

    dialog.set_text(_text_area->toHtml());
    dialog.set_window_title(tr("Edit HTML source"));

    if(dialog.exec() != QDialog::Accepted)
        return;

    _text_area->setHtml(dialog.get_text());

    // Если в диалоговом окне был иземен HTML код
    if(dialog.isModified())
        _text_area->document()->setModified(true);
}


void Editor::on_findtext_clicked(void)
{
    _find_dialog->show();
    _find_dialog->activateWindow();
}


// Слот, принимающий данные от окна поиска текста
void Editor::on_findtext_signal_detect(const QString &text, QTextDocument::FindFlags flags)
{
    qDebug() << "Find text " << text << " with flags " << flags;

    if(!_text_area->find(text, flags)) {
        // findDialog->close();  //
        _find_dialog->hide();
        QMessageBox::information(this,
                                 tr("Search result"),
                                 tr("String '<b>") + text + tr("</b>' not found"),
                                 QMessageBox::Close);
    }
}


// Открытие контекстного меню в области редактирования
void Editor::on_customContextMenuRequested(const QPoint &pos)
{
    qDebug() << "In Editor on_customContextMenuRequested";

    // Конструирование меню
    // editor_context_menu=textarea->createStandardContextMenu();

    // Включение отображения меню на экране
    // menu.exec(event->globalPos());

    // Если выбрана картинка
    // Или нет выделения, но курсор находится на позиции картинки
    if(is_image_select() ||
       is_cursor_on_image())
        _editor_context_menu->set_edit_image_properties(true);
    else
        _editor_context_menu->set_edit_image_properties(false);

    // Контекстное меню запускается
    _editor_context_menu->exec(_text_area->viewport()->mapToGlobal(pos));
}


void Editor::on_settings_clicked(void)
{
    // Создается окно настроек, после выхода из этой функции окно удалится
    EditorConfigDialog dialog;
    dialog.show();
}


// Действия при нажатии кнопки отображения символов редактирования
void Editor::on_showformatting_clicked(void)
{
    QPalette palActive, palInactive;
    palActive.setColor(QPalette::Normal, QPalette::Button, _buttons_select_color);
    palActive.setColor(QPalette::Normal, QPalette::Window, _buttons_select_color);

    if(_text_area->get_showformatting() == false) {
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

    // Редактор запоминает отступ, чтобы было с чем сравнивать
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

    // Редактор запоминает отступ, чтобы было с чем сравнивать
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

    // Редактор запоминает отступ, чтобы было с чем сравнивать
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

    if(dialog.exec() != QDialog::Accepted) return;

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
                               BorderStyle_Solid
                              );
    tableFormat.setPadding(0);
    tableFormat.setCellPadding(0);
    tableFormat.setCellSpacing(-1);

    // Добавляется таблица с нужными размерами и форматированием
    // QTextTable *table=textarea->textCursor().insertTable(table_rows, table_columns, table_format);
    _text_area->textCursor().insertTable(tableRows, tableColumns, tableFormat);

    return;

    /*
    // Вставка новой таблицы через генериацию HTML кода
    // Если данные введены нормально
    if(table_vnum > 0 && table_hnum > 0 && table_width > 0)
     {
      QStringList tab;
      tab.clear();

      // QColor table_color;
      // table_color=qApp->palette().color(QPalette::Normal, QPalette::Base);
      // qDebug() << "Table background color " << table_color.name();
      // tab.append(QString("<table border=\"1\" align=\"center\" width=\"%1%\" cellspacing=\"0\" cellpadding=\"0\" bgcolor=\"%2%\">").arg(table_width).arg(table_color.name()));

      tab.append(QString("<table style=\"border-style:solid; border-right:none; border-bottom:none;\" border=\"1\" align=\"center\" width=\"%1%\" cellspacing=\"0\" cellpadding=\"3\">").arg(table_width));

      for (int i=0;i<table_hnum;i++)
       {
        tab.append(QString("<tr>"));
        for (int o=0;o<table_vnum;o++)
         {
          tab.append(QString("<td style=\"border-style:solid; border-right:none; border-bottom:none;\"><p></p></td>"));
         }
        tab.append(QString("</tr>"));
       }

      tab.append(QString("</table>"));

      QString table_text = tab.join("\n");
      QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(table_text);
      textarea->textCursor().insertFragment(fragment);
     }
    */

    /*
    // Вставка кода таблицы из файла
    QFile file("table.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
     critical_error("Can not open table file");

    QString tablecode;
    while(!file.atEnd())
    {
     QString line=file.readLine();
     tablecode=tablecode+line+"\n";
    }

    file.close();

    QTextDocumentFragment fragment = QTextDocumentFragment::fromHtml(tablecode);
    textarea->textCursor().insertFragment(fragment);

    return;
    */

}


// Удаление строк из таблицы
void Editor::on_table_remove_row_clicked(void)
{
    QTextCursor cursor(_text_area->textCursor());
    QTextTable *table = cursor.currentTable();

    if(table != 0) {
        QTextTableCell cell = table->cellAt(cursor);
        int cellRowCursor = cell.row(); // Текущий номер строки (счет с нуля)
        table->removeRows(cellRowCursor, 1); // Удаляются строки таблицы
    }
}


// Удаление столбцов из таблицы
void Editor::on_table_remove_col_clicked(void)
{
    QTextCursor cursor(_text_area->textCursor());
    QTextTable *table = cursor.currentTable();

    if(table != 0) {
        QTextTableCell cell = table->cellAt(cursor);
        int cellColCursor = cell.column(); // Текущий номер столбца (счет с нуля)
        table->removeColumns(cellColCursor, 1); // Удаляются столбцы таблицы
    }
}


// Добавление строк в таблицу
void Editor::on_table_add_row_clicked(void)
{
    QTextCursor cursor(_text_area->textCursor());
    QTextTable *table = cursor.currentTable();

    if(table) {
        QTextTableCell cell = table->cellAt(cursor); // Выясняется текущая ячейка
        int cellRowCursor = cell.row(); // Текущий номер строки (счет с нуля)

        bool ok = false;
        int addNum = QInputDialog::getInt(this, tr("Append rows to table"), tr("Append rows:"), 1, 1, 100, 1, &ok);

        if(ok && addNum > 0)
            table->insertRows(cellRowCursor + 1, addNum);
    }
}


// Добавление столбцов в таблицу
void Editor::on_table_add_col_clicked(void)
{
    QTextCursor cursor(_text_area->textCursor());
    QTextTable *table = cursor.currentTable();

    if(table) {
        QTextTableCell cell = table->cellAt(cursor); // Выясняется текущая ячейка
        int cellColCursor = cell.column(); // Текущий номер столбца (счет с нуля)

        bool ok = false;
        int addNum = QInputDialog::getInt(this, tr("Append columns to table"), tr("Append columns:"), 1, 1, 100, 1, &ok);

        if(ok && addNum > 0)
            table->insertColumns(cellColCursor + 1, addNum);
    }
}


// Объединение ячеек таблицы
void Editor::on_table_merge_cells_clicked(void)
{
    QTextCursor cursor(_text_area->textCursor());
    QTextTable *table = cursor.currentTable();

    if(table)
        table->mergeCells(cursor);
}


void Editor::on_table_split_cell_clicked(void)
{
    QTextCursor cursor(_text_area->textCursor());
    QTextTable *table = cursor.currentTable();

    // Если курсор находится где-то внутри таблицы
    if(table) {
        // Выясняется, выделены ячейки или нет
        int *currX = new int;
        int *currY = new int;
        int *currWidthX = new int;
        int *currWidthY = new int;
        cursor.selectedTableCells(currX, currY, currWidthX, currWidthY);

        // Если ячейки выделены, и захвачено для выделения более одной ячейки
        if(*currWidthX > 1 || *currWidthY > 1) {
            // Работа возможна только с одной ячейкой
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Split cell"));
            msgBox.setText(tr("Select a single cell."));
            msgBox.exec();
            return;
        }

        QTextTableCell cell = table->cellAt(cursor); // Выясняется текущая ячейка
        int cellColCursor = cell.column(); // Текущий номер столбца (счет с нуля)
        int cellRowCursor = cell.row(); // Текущий номер строки (счет с нуля)
        qDebug() << "Current cell is " << cellColCursor << cellRowCursor;

        // Курсором выделяется таблица
        cursor.setPosition(table->firstCursorPosition().position() - 1, QTextCursor::MoveAnchor);
        cursor.setPosition(table->lastCursorPosition().position() + 1, QTextCursor::KeepAnchor);

        // Выделенный фрагмент выделяется в отдельный объект
        QTextDocumentFragment fragment(cursor);

        EditorAbsTable current_table(table->columns(), table->rows(), fragment.toHtml());

        qDebug() << "This cell colspan " << current_table.get_cell_colspan(cellColCursor, cellRowCursor) << " rowspan " << current_table.get_cell_rowspan(cellColCursor, cellRowCursor);


        // Выясняется, на сколько сегментов можно разбивать ячейку
        int hSplitTo;
        int vSplitTo;
        int hCurrMerge = current_table.get_cell_colspan(cellColCursor, cellRowCursor);
        int vCurrMerge = current_table.get_cell_rowspan(cellColCursor, cellRowCursor);

        if(hCurrMerge == 0)hCurrMerge = 1;

        if(vCurrMerge == 0)vCurrMerge = 1;

        if(hCurrMerge == 1) hSplitTo = 0; // Можно разбивать на любое число
        else              hSplitTo = hCurrMerge;

        if(vCurrMerge == 1) vSplitTo = 0; // Можно разбивать на любое число
        else              vSplitTo = vCurrMerge;


        // Создается диалог запроса на сколько сегментов надо разбивать ячейку
        EditorSplitCellForm dialog(hSplitTo, vSplitTo);

        if(dialog.exec() == QDialog::Accepted) {
            int hSplit = dialog.get_num_split_h(); // Эти значения всегда 1 или больше
            int vSplit = dialog.get_num_split_v();

            // Разбивка по горизонтали
            if(hSplit > 1) {
                if(hCurrMerge == 1)
                    current_table.split_single_cell_by_horisontal(cellColCursor, cellRowCursor, hSplit);
                else
                    current_table.split_merged_cell_by_horisontal(cellColCursor, cellRowCursor);
            }

            // Разбивка по вертикали
            if(vSplit > 1) {
                if(vCurrMerge == 1)
                    current_table.split_single_cell_by_vertical(cellColCursor,
                                                                cellColCursor + hSplit - 1,
                                                                cellRowCursor,
                                                                vSplit);
                else
                    current_table.split_merged_cell_by_vertical(cellColCursor,
                                                                cellColCursor + hSplit - 1,
                                                                cellRowCursor);
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
    if(is_image_select() ||
       is_cursor_on_image()) {
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
    QTextBlock::iterator it;
    QTextFragment fragment;

    // Если есть выделение
    if(_text_area->textCursor().hasSelection()) {
        // Перебиратся фрагметы блока
        for(it = currentBlock.begin(); !(it.atEnd()); ++it) {
            fragment = it.fragment();

            // Если фрагмент содержит изображение
            if(fragment.isValid())
                if(fragment.charFormat().isImageFormat()) {
                    int fragmentStart = fragment.position();
                    int fragmentEnd = fragmentStart + fragment.length();
                    int selectionStart = _text_area->textCursor().selectionStart();
                    int selectionEnd = _text_area->textCursor().selectionEnd();

                    // Если начало и конец фрагмента совпадает с координатами выделения
                    // Проверяется и случай, когда блок выделен в обратную сторону
                    if((fragmentStart == selectionStart && fragmentEnd == selectionEnd) ||
                       (fragmentStart == selectionEnd && fragmentEnd == selectionStart)) {
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
    QTextBlock::iterator it;
    QTextFragment fragment;

    // Если есть выделение
    if(_text_area->textCursor().hasSelection()) {
        // Перебиратся фрагметы блока
        for(it = currentBlock.begin(); !(it.atEnd()); ++it) {
            fragment = it.fragment();

            // Если фрагмент содержит изображение
            if(fragment.isValid())
                if(fragment.charFormat().isImageFormat()) {
                    int fragmentStart = fragment.position();
                    int fragmentEnd = fragmentStart + fragment.length();
                    int selectionStart = _text_area->textCursor().selectionStart();
                    int selectionEnd = _text_area->textCursor().selectionEnd();

                    // Если начало и конец фрагмента совпадает с координатами выделения
                    // Проверяется и случай, когда блок выделен в обратную сторону
                    if((fragmentStart == selectionStart && fragmentEnd == selectionEnd) ||
                       (fragmentStart == selectionEnd && fragmentEnd == selectionStart)) {
                        QTextImageFormat imageFormat = fragment.charFormat().toImageFormat();

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
    if(_text_area->textCursor().hasSelection() == false) {
        QTextImageFormat imageFormat = _text_area->textCursor().charFormat().toImageFormat();

        if(imageFormat.isValid())
            return true;
    }

    return false;
}


// Формат картинки на которой находится курсор
QTextImageFormat Editor::image_format_on_cursor(void)
{
    // Проверка срабатывает только если нет выделения
    if(_text_area->textCursor().hasSelection() == false) {
        QTextImageFormat imageFormat = _text_area->textCursor().charFormat().toImageFormat();

        if(imageFormat.isValid())
            return imageFormat;
    }

    return QTextImageFormat();
}


// Обработка клавиши добавления картинки
void Editor::on_insert_image_from_file_clicked(void)
{

    // Если выделена картинка
    if(is_image_select() ||
       is_cursor_on_image()) {
        qDebug() << "Image selected";

        edit_image_properties();
    } else {
        // Иначе картинка не выделена, и срабатывает режим добавления
        // картинки из файла

        // Диалог выбора файлов катинок
        QFileDialog imageSelectDialog(this);
        imageSelectDialog.setFileMode(QFileDialog::ExistingFiles); // QFileDialog::ExistingFile
        imageSelectDialog.setNameFilter("*.png *.jpg *.gif");
        imageSelectDialog.setWindowTitle(tr("Insert image"));
        imageSelectDialog.setDirectory(QDir::homePath());
        imageSelectDialog.exec();

        // Выясняется список выбранных файлов
        QStringList files = imageSelectDialog.selectedFiles();

        // Если ни один файл не выбран
        if(files.size() == 0)
            return;

        // Перебираются файлы выбранных картинок
        for(int i = 0; i < files.size(); ++i) {
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
    } // Завершилось условие что картинка не выбрана и нужно добавлять из файла

}


void Editor::edit_image_properties(void)
{
    // Данные обрабатываемой картинки
    QTextImageFormat imageFormat;

    // Если выбрано изображение
    if(is_image_select())
        imageFormat = image_format_on_select();

    // Если изображение не выбрано, но курсор находится в позиции изображения
    if(is_cursor_on_image())
        imageFormat = image_format_on_cursor();


    // Выясняется имя картинки в ресурсах документа
    QString imageName = imageFormat.name();

    // По имени из ресурсов вытягивается кратинка
    QUrl urlName(imageName);
    QVariant imageData = _text_area->document()->resource(QTextDocument::ImageResource, urlName);
    QImage image = imageData.value<QImage>();

    // Выяснятся реальные размеры картики
    int realImageWidth = image.width();
    int realImageHeight = image.height();

    qDebug() << "Real image width " << realImageWidth << " height " << realImageHeight;
    qDebug() << "Format image width " << imageFormat.width() << " height " << imageFormat.height();

    // Создается и запускается диалог запроса размеров картинки
    EditorImageProperties dialog;
    dialog.set_info(tr("Real image size ") +
                    QString::number(realImageWidth) +
                    " x " +
                    QString::number(realImageHeight) +
                    tr(" pixels"));
    dialog.set_real_width(realImageWidth);
    dialog.set_real_height(realImageHeight);

    // Если в форматировании картинки не задан размер картинки
    if(imageFormat.width() == 0 && imageFormat.height() == 0) {
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
    if(dialog.exec() != QDialog::Accepted)
        return;

    imageFormat.setWidth(dialog.get_width());
    imageFormat.setHeight(dialog.get_height());

    // Если в новом формате картинки нет никаких ошибок
    if(imageFormat.isValid()) {

        // Если выбрано изображение
        if(is_image_select()) {
            QTextFragment fragment;

            // Блок, в пределах которого находится курсор
            QTextBlock currentBlock = _text_area->textCursor().block();
            QTextBlock::iterator it;

            // Перебиратся фрагметы блока
            // Так как известно, что картинка выделена, поиск фрагмента будет успешным
            for(it = currentBlock.begin(); !(it.atEnd()); ++it) {
                fragment = it.fragment();

                // Если фрагмент содержит изображение
                if(fragment.isValid() &&
                   fragment.charFormat().isImageFormat())
                    break; // Переменная fragment содержит только картинку
            }


            QTextCursor helper = _text_area->textCursor();

            helper.setPosition(fragment.position());
            helper.setPosition(fragment.position() + fragment.length(),
                               QTextCursor::KeepAnchor);
            helper.setCharFormat(imageFormat);
        }


        // Если изображение не выбрано, но курсор находится в позиции изображения
        if(is_cursor_on_image()) {
            int cursorPosition = _text_area->textCursor().position();

            QTextCursor helper = _text_area->textCursor();

            helper.setPosition(cursorPosition);

            if(_text_area->textCursor().atBlockStart())
                helper.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            else
                helper.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);

            helper.setCharFormat(imageFormat);
        }
    }

}


void Editor::on_expand_edit_area_clicked(void)
{
    if(_expand_edit_area_flag == false) {
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
    if(flag == 0) {
        bool is_expand = _editor_config->get_expand_tools_lines();

        if(is_expand) setFlag = false; // Если панель инструментов распахнута, надо сомкнуть
        else setFlag = true; // Иначе распахнуть
    } else {
        // Иначе метод вызывался с каким-то параметром
        if(flag == 1) setFlag = true;

        if(flag == -1) setFlag = false;
    }


    // Панели распахиваются/смыкаются (кроме первой линии инструментов)
    _tools_line_1->setVisible(setFlag);

    if(_view_mode == WYEDIT_DESKTOP_MODE)
        _indent_slider->setVisible(setFlag);

    // Запоминается новое состояние
    _editor_config->set_expand_tools_lines(setFlag);

    // На всякий случай обновляется геометрия расположения движков на слайд-панели
    update_indentline_geometry();
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

void Editor::on_freeze_clicked(void)
{
    // Обновление инфополей в области редактирования записи
    MetaEditor *metaeditor = globalparameters.meta_editor();  //MetaEditor *metaEditor = find_object<MetaEditor>(meta_editor_singleton_name);

    if(metaeditor)metaeditor->switch_pin();
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
    if(_text_area->toPlainText().length() == 0) {
        QMessageBox msgBox;
        msgBox.setText("Can't show empty text in detached window");
        msgBox.exec();

        return;
    }

    EditorShowText *showText = new EditorShowText(this);

    // Устанавливается флаг удаления диалога после закрытия его окна
    showText->setAttribute(Qt::WA_DeleteOnClose);

    if(misc_field("title").length() > 0)
        showText->setWindowTitle(misc_field("title"));

    QTextDocument *cloneDocument = _text_area->document()->clone(showText); // Для метода clone указан parent, который уничтожится при закрытии окна, и за собой уничтожит этот временный документ
    showText->setDocument(cloneDocument);

    showText->show();
}


void Editor::save_callback(void (*func)(QObject *editor, QString saveString))
{
    save_callback_func = func;
}


void Editor::load_callback(void (*func)(QObject *editor, QString &String))
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


void Editor::misc_field(QString name, QString value)
{
    _misc_fields[name] = value;
}


QString Editor::misc_field(QString name)
{
    if(_misc_fields.contains(name))
        return _misc_fields[name];
    else
        return QString();
}


void Editor::clear_all_misc_field(void)
{
    _misc_fields.clear();
}


void Editor::dir_file_empty_reaction(int mode)
{
    // Проверяется допустимость переданного значения
    if(mode == DIRFILEEMPTY_REACTION_SHOW_ERROR ||
       mode == DIRFILEEMPTY_REACTION_SUPPRESS_ERROR)
        _dir_file_empty_reaction = mode;
    else
        critical_error("Editor::setDirFileEmptyReaction() : Unsupport mode " + QString::number(mode));
}


int Editor::dir_file_empty_reaction(void)
{
    return _dir_file_empty_reaction;
}


// Метод позволяющий управлять доступностью инcтрументов редактирования
void Editor::disable_tool_list(QStringList toolNames)
{
    qDebug() << "Editor::setDisableToolList() : " << toolNames;
    _disable_tool_list = toolNames;
}


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

