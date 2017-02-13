
#include <functional>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QtDebug>

#include "editor_wrap.h"
#include "main.h"

#include "libraries/flat_control.h"
#include "libraries/global_parameters.h"
#include "libraries/wyedit/editor.h"
#include "libraries/wyedit/editor_text_area.h"
#include "models/app_config/app_config.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/record.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_item.h"
#include "views/attach_table/attach_table_screen.h"
#include "views/browser/entrance.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/hidable_tabwidget.h"
#include "views/main_window/main_window.h"
#include "views/record/editentry.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

#ifdef USE_QTM
#include "libraries/qtm/editing_window.h"
#endif

extern gl_para globalparameters;
extern AppConfig appconfig;

namespace browser {
class WebPage;
class Entrance;
}

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(ClickableLabel)
#endif

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(MetaEditor)
#endif

EditorWrap::EditorWrap(FindScreen* find_screen, EditingWindow* editing_win, QStackedWidget* main_stack_, QString object_name)
    : Editor(main_stack_, editing_win)
    //    : QDockWidget(_find_screen)
    , _editor_main_screen(new QWidget(this)) // Сборка виджета редактирования текста (основной виджет)
    , _editor_main_layer(new QGridLayout(_editor_main_screen))
    , _label_pin(new QLabel(_editor_main_screen))
    //	  , _tree_path_content(new QLabel(_editor_main_screen))
    , _item_pin(new QCheckBox(_editor_main_screen))
    , _item_name(new QLabel(_editor_main_screen))
    , _item_author(new QLabel(_editor_main_screen))
    , _label_home(new QLabel(_editor_main_screen))
    , _label_url(new QLabel(_editor_main_screen))
    , _item_home(new ClickableLabel(_editor_main_screen))
    , _item_url(new ClickableLabel(_editor_main_screen))
    , _item_tags_scrollarea(new QScrollArea(_editor_main_screen))
    , _item_tags_scrollarea_layout(new QHBoxLayout(_item_tags_scrollarea))
    , _item_tags_container(new QWidget(_item_tags_scrollarea))
    , _item_tags_layout(new QHBoxLayout(_item_tags_container))
    , _label_tags(new QLabel(_item_tags_container))
    , _attachtable_screen(new AttachTableScreen(this))
    , _meta_editor_join_layer(new QVBoxLayout(this))
    , _find_screen(find_screen)
    , _main_stack(main_stack_)
    , _editing_win(editing_win)
{ //      , _hidetitlebar(new QWidget(this, Qt::FramelessWindowHint | Qt::CustomizeWindowHint))
    setObjectName(object_name);
    _attachtable_screen->setHidden(true);
    // #ifdef USE_QTM

    //    setWindowFlags(	// Qt::Window |
    //	Qt::FramelessWindowHint
    //	// |Qt::Popup
    //	| Qt::CustomizeWindowHint
    //	// | Qt::SplashScreen  // http://www.qtforum.org/article/20174/how-to-create-borderless-windows-with-no-title-bar.html?s=86e2c5a6509f28a482adbb7d9f3654bb2058a301#post75829
    //	// | Qt::DockWidgetArea::NoDockWidgetArea
    //	| Qt::MaximizeUsingFullscreenGeometryHint
    //	);

    //    setAutoFillBackground(true);
    //    adjustSize();

    //    setFeatures(QDockWidget::NoDockWidgetFeatures
    ////	| QDockWidget::DockWidgetVerticalTitleBar
    //	// | Qt::DockWidgetArea::NoDockWidgetArea
    //	// | Qt::MaximizeUsingFullscreenGeometryHint
    //	);	// AllDockWidgetFeatures

    //    QWidget *titleBar = titleBarWidget();

    //    setTitleBarWidget(_hidetitlebar);
    //    _hidetitlebar->setGeometry(0, 0, 0, 0);
    //    _hidetitlebar->setVisible(false);
    //    _hidetitlebar->close();

    //    delete titleBar;

    //    _blog_editor = std::make_unique<EditingWindow>();
    //    _blog_editor->setSTI(0);	// No STI
    //    _blog_editor->setWindowTitle(QObject::tr("QTM - new entry [*]"));

    //    _editor = std::make_unique<Editor>(new Editor(this));
    ////    _editor->hide_all_tools_elements();
    ////    _editor->hide();

    //    _blog_editor->setParent(_editor.get());
    //    _editor->setWidget(_blog_editor.get());
    //	//    _blog_editor->setParent(this);
    //	//    setWidget(_blog_editor.get());
    //    if(_blog_editor->handleArguments()) _blog_editor->show();
    //    else _blog_editor->close();
    //    _blog_editor->raise();
    //    _editor->show();

    // #endif

    Editor::disable_tool_list(appconfig.hide_editor_tools());

    Editor::init_enable_assembly(false);
    Editor::
        init_config_file_name(globalparameters.root_path() + "/" + globalparameters.target_os() + "/editorconf.ini");
    Editor::init_enable_random_seed(false);
    if (appconfig.interface_mode() == "desktop")
        Editor::init(Editor::WYEDIT_DESKTOP_MODE);
    else if (appconfig.interface_mode() == "mobile")
        Editor::init(Editor::WYEDIT_MOBILE_MODE);
    else
        critical_error("In MetaEditor constructor unknown interface mode: " + appconfig.interface_mode());
    setup_labels();
    setup_ui();
    assembly();
    initialize_data();
    adjustSize();
    setup_signals(_find_screen);

    // В редакторе устанавливается функция обратного вызова на кнопку Attach
    attach_callback(&EditorWrap::to_attach_callback);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    Editor::update_indentline_geometry();
    connect(_item_pin, &QCheckBox::clicked, [&](bool checked) { pin(checked ? _string_from_check_state[Qt::CheckState::Checked] : _string_from_check_state[Qt::CheckState::Unchecked]); });
}

EditorWrap::~EditorWrap(void) { _url_connection_initialized = false; }

void EditorWrap::setup_signals(FindScreen* _find_screen)
{
    connect(this, &EditorWrap::set_find_text_signal, _find_screen, &FindScreen::find_text);
}

void EditorWrap::setup_labels(void)
{
    _label_pin->setText(tr("<B>Pin status:</B> "));
    _label_pin->setVisible(true);
    //	// Путь в дереве до данной записи в виде названий веток (только для мобильного интерфейса)
    //	// _tree_path = new QLabel(this);
    //	_tree_path_content->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    //	if(appconfig.interface_mode() == "desktop") _tree_path_content->setVisible(false);
    //	else _tree_path_content->setVisible(true);
    //	_tree_path_content->setWordWrap(true);

    // _item_pin = new QCheckBox(this);
    // recordPin->setTextInteractionFlags(Qt::TextSelectableByMouse |
    // Qt::TextSelectableByKeyboard);
    _item_pin->setFixedWidth(17);
    //	_item_pin->setVisible(false);
    //	// recordPin->setCheckState(_state_check["0"]);
    //	// recordPin->setWordWrap(true);

    // Название записи
    // _item_name = new QLabel(this);
    _item_name->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _item_name->setVisible(false);
    _item_name->setWordWrap(true);

    // Автор
    // _item_author = new QLabel(this);
    _item_author->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _item_author->setVisible(false);
    _item_author->setWordWrap(true);

    // Url
    // _label_home = new QLabel(this);
    _label_home->setText(tr("<B>Home:</B> "));
    _label_home->setVisible(false);

    // Url
    // _label_url = new QLabel(this);
    _label_url->setText(tr("<B>Url:</B> "));
    _label_url->setVisible(false);

    // _item_home = new ClickableLabel(this);
    // recordHome->setOpenExternalLinks(true);
    _item_home->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    _item_home->setVisible(false);
    _item_home->setWordWrap(true);

    // _item_url = new ClickableLabel(this);
    // recordUrl->setOpenExternalLinks(true);
    _item_url->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
    _item_url->setVisible(false);
    _item_url->setWordWrap(true);

    // Текстовые метки
    // _label_tags = new QLabel(this);
    _label_tags->setText(tr("<B>Tags:</B> "));
    _label_tags->setVisible(false);
}

void EditorWrap::bind(boost::intrusive_ptr<TreeItem> item_to_be_bound)
{
    // boost::intrusive_ptr<TreeItem> item_to_be_bound = record_index->target();
    _item = item_to_be_bound;
    assert(_item);
    // TableController *table_controller = globalparameters.table_screen()->table_controller();
    // assert(table_controller);
    // int pos = table_controller->table_model()->table_data()->get_pos_by_id(r->getNaturalFieldSource("id"));
    // table_controller->sychronize_metaeditor_to_record(pos);
    // setPin(r->getNaturalFieldSource("pin"));
    // setName(r->getNaturalFieldSource("name"));
    // setAuthor(r->getNaturalFieldSource("author"));
    // setTags(r->getNaturalFieldSource("tags"));
    // setHome(r->getNaturalFieldSource("home"));
    // setUrl(r->getNaturalFieldSource("url"));
    // if(r->isLite())
    // set_textarea(r->getTextDirectFromLite());
    // else
    // set_textarea(r->getTextFromFat());
    pin(_item->field<pin_type>());
    name(_item->field<name_type>());
    author(_item->field<author_type>()); // Надпись Author(s)
    home(_item->field<home_type>());
    url(_item->field<url_type>());
    if (_url_connection_initialized)
        QObject::disconnect(_url_connection);
    _url_connection = QObject::connect(_item_url, &ClickableLabel::mousePressEvent // linkActivated		// , _record->binded_only_page()
        ,
        [this](QMouseEvent* ev) {
            Q_UNUSED(ev)
            assert(_item);
            assert(_item->page()); // _item->page_valid() &&
            _item->activate(std::bind(&HidableTabWidget::find, globalparameters.main_window()->vtab_record(), std::placeholders::_1));
        });
    _url_connection_initialized = true;
    if (_home_connection_initialized)
        QObject::disconnect(_home_connection);
    _home_connection = QObject::connect(_item_home, &ClickableLabel::mousePressEvent, [&](QMouseEvent* ev) {
        Q_UNUSED(ev)
        // Q_UNUSED(home)
        assert(_item);
        //		assert(_item->page_valid());
        browser::WebPage* page = _item->page();
        assert(page);
        QString home = _item->field<home_type>();
        QString url = _item->field<url_type>();
        if (home != browser::Browser::_defaulthome && url != home) {
            //		    _item->field("url", home);
            static_cast<QWebEnginePage*>(page)->setUrl(QUrl(home)); // item_bind(_item)->activate(); // page->load(_record, true);
            //		    _record->active();
        }
    });
    _home_connection_initialized = true;
}

boost::intrusive_ptr<TreeItem> EditorWrap::item() { return _item; }

void EditorWrap::setup_ui(void)
{
    // Область текстовых меток, которые выглядят на экране как [метка1] [метка2] [метка3] ...
    // _item_tags_layout = new QHBoxLayout();
    _item_tags_layout->setAlignment(Qt::AlignLeft);
    _item_tags_layout->setMargin(0);

    // QHBoxLayout невозможно добавить в QScrollArea, поэтому оборачивается в виджет
    // _item_tags_container = new QWidget();
    _item_tags_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    _item_tags_container->setLayout(_item_tags_layout);

    _item_tags_scrollarea_layout->setMargin(0);
    _item_tags_scrollarea_layout->addWidget(_item_tags_container);
    // Создается QScrollArea и в нее добавляется виджет с QHBoxLayout
    // _item_tags_scrollarea = new QScrollArea();
    _item_tags_scrollarea->setContentsMargins(0, 0, 0, 0); // Убирается отступ от границ содержимого
    _item_tags_scrollarea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum); // (QSizePolicy::Expanding, QSizePolicy::Preferred);
    _item_tags_scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Убирается горизонтальная полоса прокрутки
    _item_tags_scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Убирается вертикальная полоса прокрутки
    _item_tags_scrollarea->setFrameShape(QFrame::NoFrame); // Убирается тонкая линия вокруг QScrollArea

    _item_tags_scrollarea->setLayout(_item_tags_scrollarea_layout); // _item_tags_scrollarea->setWidget(_item_tags_container);
    _item_tags_scrollarea->setToolTip("Tags scroll area");
    // _attachtable_screen = new AttachTableScreen(this);
}

void EditorWrap::assembly(void)
{
    //	layout()->invalidate();

    //// Сборка виджета редактирования текста (основной виджет)
    // _editor_main_screen    = new QWidget(this);
    // _editor_main_layer     = new QGridLayout(_editor_main_screen);
    QHBoxLayout* tool_bar_layout = new QHBoxLayout();
    tool_bar_layout->addLayout(_textformat_buttons_layout);

    //	_editor_main_layer->addLayout(_textformat_buttons_layout, 0, 0, 1, 2);
    _editor_main_layer->addLayout(tool_bar_layout, 0, 0, 1, 2);
    _editor_main_layer->addWidget(_label_pin, 1, 0, 1, 1);
    _editor_main_layer->addWidget(_item_pin, 1, 1, 1, 1);
    //	_editor_main_layer->addWidget(_tree_path_content, 1, 1, 1, 1);

    _editor_main_layer->addWidget(_item_name, 2, 0, 1, 2);
    _editor_main_layer->addWidget(_item_author, 3, 0, 1, 2);

    _editor_main_layer->addWidget(_text_area, 4, 0, 1, 2);

    _editor_main_layer->addWidget(_label_home, 5, 0, 1, 1);
    _editor_main_layer->addWidget(_item_home, 5, 1, 1, 1);

    _editor_main_layer->addWidget(_label_url, 6, 0, 1, 1);
    _editor_main_layer->addWidget(_item_url, 6, 1, 1, 1);

    _editor_main_layer->addWidget(_label_tags, 7, 0, 1, 1);
    _editor_main_layer->addWidget(_item_tags_scrollarea, 7, 1, 1, 1); //    _editor_main_layer->addLayout(_item_tags_scrollarea_layout, 7, 1, 1, 1);	// Было addLayout(recordTagsLayout ...)    //
    _editor_main_layer->setColumnStretch(0, 0);
    _editor_main_layer->setColumnStretch(1, 1);
    _editor_main_layer->setRowStretch(4, 2); // for textArea auto fullfill

    _editor_main_layer->setContentsMargins(0, 0, 0, 0);

    _editor_main_screen->setLayout(_editor_main_layer);

    // Сборка виджетов в один слой
    // _meta_editor_join_layer = new QVBoxLayout(this);
    _meta_editor_join_layer->addWidget(_editor_main_screen); // _meta_editor_join_layer->addLayout(_editor_main_layer);
    _meta_editor_join_layer->addWidget(_attachtable_screen);

    _meta_editor_join_layer->setStretch(0, 1);
    _meta_editor_join_layer->setContentsMargins(0, 0, 0, 0);
    this->setLayout(_meta_editor_join_layer);

    // Границы убираются, так как данный объект будет использоваться как виджет
    // The boundaries are removed, as this facility will be used as a widget
    layout()->setContentsMargins(0, 0, 0, 0); // setContentsMargins(0, 2, 0, 0);

    // По-умолчанию отображается слой редатирования
    to_editor_layout();
}

void EditorWrap::to_editor_layout(void)
{
    _attachtable_screen->setVisible(false);
    _attachtable_screen->hide(); // Что бы небыло мерцания, вначале нужно делать сокрытие текущего виджета
    _editor_main_screen->show();
}

void EditorWrap::to_attach_layout(void)
{
    _editor_main_screen->hide();
    _attachtable_screen->setVisible(true);
    _attachtable_screen->show();
}

// Статическая функция, обрабатывает клик в редакторе по кнопке переключения на список прикрепляемых файлов
void EditorWrap::to_attach_callback(void)
{
    auto* editentry_ = globalparameters.edit_entry(); // find_object<MetaEditor>(meta_editor_singleton_name);
    editentry_->to_attach_layout();
}

// Слот для установки значений инфополей на экране
void EditorWrap::field(QString n, QString v)
{
    if (n == "pin")
        pin(v);
    else if (n == "name")
        name(v);
    else if (n == "author")
        author(v);
    else if (n == "home")
        home(v);
    else if (n == "url")
        url(v);
    else if (n == "tags")
        tags(v);
    else
        critical_error("metaeditor.set_field Undestand field " + n + " with value " + v);
}

void EditorWrap::initialize_data(void)
{
    qDebug() << "MetaEditor::clear_all()";
    if (!_item) {
        // Очистка для слоя редактора
        pin("");
        name("");
        author("");
        url("");
        tags("");
        textarea("");

        work_directory("");
        file_name("");

        clear_all_misc_field();

        // Иконка аттачей должна показывать что аттачей нет
        _to_attach->setIcon(_icon_attach_not_exists);

        // Очистка для слоя приаттаченных файлов
        _attachtable_screen->clear();
    }
}

// void MetaEditor::tree_path(const QString &path){
//	_tree_path_content->setVisible(true);
//	_tree_path_content->setText(tr("<b>Path: </b>") + path);
// }

void EditorWrap::pin(const QString& pin_)
{
    auto set_pin = [&](bool checked) {
        //	if(checked != ("" != _string_from_check_state[_item_pin->checkState()])){
        // if(globalparameters.entrance()->activiated_browser()) {
        browser::TabWidget* _tabmanager = globalparameters.main_window()->vtab_record()->activated_browser()->tabmanager();
        // record_screens()->record_controller();
        if (_tabmanager) {
            RecordModel* source_model = _tabmanager->source_model();
            auto _record_controller = _tabmanager->record_controller();
            rv_t* record_view = _tabmanager->view();
            pos_source pos_source_(_tabmanager->currentIndex()); // first_selectionpos();
            if (source_model && -1 != pos_source_) {
                // Выясняется ссылка на таблицу конечных данных
                // auto item = source_model->tree_item();  //->record_table();    //getTableData();

                QString p = source_model->item(pos_source_)->field<pin_type>(); // "pin"
                _item_pin->setCheckState(_state_check_from_string[p]);

                QString h = source_model->item(pos_source_)->field<url_type>(); // "url"

                // Переданные отредактированные поля преобразуются в вид имя-значение
                QMap<QString, QString> edit_data;
                if (checked) {
                    _item_pin->setCheckState(Qt::CheckState::Checked);

                    //				pin(p = _string_from_check_state[Qt::CheckState::Checked]);
                    home(h);

                    edit_data["pin"] = p;
                    edit_data["home"] = h;
                } else { // (_item_pin->checkState() == Qt::CheckState::Checked)
                    _item_pin->setCheckState(Qt::CheckState::Unchecked);

                    //				pin(p = _string_from_check_state[Qt::CheckState::Unchecked]);

                    edit_data["pin"] = p;
                }
                //// Переданные отредактированные поля преобразуются в вид имя-значение
                // QMap<QString, QString> editData;
                // editData["pin"] = pin;
                // editData["name"] = name;
                // editData["author"] = author;
                // editData["home"] = home;
                // editData["url"] = url;
                // editData["tags"] = tags;

                // Обновление новых данных в таблице конечных записей
                source_model->item(pos_source_)->field<pin_type>(edit_data["pin"]);
                source_model->item(pos_source_)->field<home_type>(edit_data["home"]);

                // Сохранение дерева веток
                // find_object<TreeScreen>(tree_screen_singleton_name)->saveKnowTree();
                ts_t* _tree_screen = globalparameters.tree_screen();
                if (record_view) {
                    _record_controller->select_as_current(_record_controller->index<pos_proxy>(pos_source_));
                    auto _binder = _item->binder();
                    if (_binder) {
                        auto host = _binder->host();
                        if (host) {
                            RecordIndex::synchronize(host);
                            auto source_model = [&]() { return _tree_screen->view()->source_model(); };
                            source_model()->emit_datachanged_signal(source_model()->index(host));
                        }
                        _record_controller->on_recordtable_configchange();
                    }
                }
                if (_tree_screen)
                    _tree_screen->view()->know_model_save();
            }
        }
        //	}
    };
    // recordPin->setVisible(true);
    _item_pin->setCheckState(_state_check_from_string[pin_]);
    // recordPin->setText("<b>" + pin + "</b>");
    if (_item && (_state_check_from_string[_item->field<pin_type>()] != _item_pin->checkState()))
        set_pin(pin_ != "");
}

void EditorWrap::name(const QString& name_)
{
    _item_name->setVisible(true);
    _item_name->setText("<b>" + name_ + "</b>");
}

void EditorWrap::author(const QString& author_)
{
    if (author_.length() == 0) {
        _item_author->setVisible(false);
        _item_author->setText("");
    } else {
        _item_author->setVisible(true);
        _item_author->setText("<i>" + author_ + "</i>");
    }
}

void EditorWrap::home(const QString& url_)
{
    if (url_.length() == 0) {
        _label_home->setVisible(false);
        _item_home->setVisible(false);

        _item_home->setText("");
    } else {
        _label_home->setVisible(true);
        _item_home->setVisible(true);
        if (url_.size() > 64)
            _item_home->setText("<a href=\"" + url_ + "\">" + url_.left(64) + "...</a>");
        else
            _item_home->setText("<a href=\"" + url_ + "\">" + url_ + "</a>");
    }
}

void EditorWrap::url(const QString& url_)
{
    if (url_.length() == 0) {
        _label_url->setVisible(false);
        _item_url->setVisible(false);

        _item_url->setText("");
    } else {
        _label_url->setVisible(true);
        _item_url->setVisible(true);
        if (url_.size() > 64)
            _item_url->setText("<a href=\"" + url_ + "\">" + url_.left(64) + "...</a>");
        else
            _item_url->setText("<a href=\"" + url_ + "\">" + url_ + "</a>");
    }
}

void EditorWrap::tags(const QString& tags_)
{
    _item_tags_text_list.clear();

    // Строка с метками запоминается в явном виде
    _item_tags_text = tags_;

    // Строка с метками разделяется на отдельные меки
    _item_tags_text_list = _item_tags_text.split(QRegExp("[,;]+"), QString::SkipEmptyParts);
    // В каждой метке убираются лишние пробелы по краям
    for (int i = 0; i < _item_tags_text_list.size(); ++i)
        _item_tags_text_list[i] = _item_tags_text_list.at(i).trimmed();
    //// Очищается слой с метками
    // QLayoutItem *child_item;
    // auto size = _item_tags_text_list.size();
    // auto count = _item_tags_layout->count();
    while (_item_tags_layout->count() > 0) { // (child_item = _item_tags_layout->takeAt(0)) != 0
        auto child_item = _item_tags_layout->takeAt(0);
        auto widget = child_item->widget();
        _item_tags_layout->removeItem(child_item);
        if (widget) { // if(child_item->widget() != nullptr && child_item->widget() != 0)
            disconnect(static_cast<QLabel*>(widget), &QLabel::linkActivated, this, &EditorWrap::on_click_to_tag);
            delete (widget);
        }
        delete child_item;
    }
    // Очищается список меток
    _item_tags_labels.clear();
    if (_item_tags_text_list.size() > 0) {
        // Создаются метки для экрана и запоминаются в список
        for (auto& text : _item_tags_text_list) {
            // Объект метки на экране
            QLabel* _label = new QLabel(this);
            _label->setOpenExternalLinks(true); // false
            _label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
            _label->setVisible(true);
            _label->setWordWrap(false);

            // Настройка, чтоб не появлялась некрасивая пунктирная рамка
            // tempLabel->setFrameStyle(QFrame::NoFrame);
            _label->setFocusPolicy(Qt::NoFocus);

            // Метке задается текст
            // В Url указывается порядковый номер метки
            QString label_text = "<a href=\""
                + text // QString::number(i)
                + "\">" + text + "</a>";

            _label->setText(label_text);

            // Клик по метке будет вызывать сигнал, слот будет принимать Url метки,
            // то есть в данном случае строку с номером метки
            // Clicking on a label will cause the signal slot will accept Url tag
            // Ie in this case the line with the number of tags
            connect(_label, &QLabel::linkActivated, this, &EditorWrap::on_click_to_tag);

            // Метка запоминается в список меток
            _item_tags_labels << _label;

            // Метка добавляется в область размещения
            _item_tags_layout->addWidget(_label); // addItem(new QWidgetItem(_label));  //
        }
        // End of widgets added to the list spacer  // В конец списка виджетов добавляется распорка
        _item_tags_layout->addStretch();
    }
    //	// Если нечего выводить на экран
    //	if(tags_.length() == 0 || _item_tags_text_list.size() == 0){
    //		_label_tags->setVisible(false);
    //		_label_tags->hide();
    //		for(auto &label : _item_tags_labels) label->setVisible(false);
    //		_item_tags_container->setVisible(false);
    //		_item_tags_container->hide();
    //		_item_tags_container->setHidden(true);
    //		_item_tags_scrollarea->setVisible(false);   // without this, editor screen is always filled by pane
    //		_item_tags_scrollarea->hide();
    //		_item_tags_scrollarea->setHidden(true);
    //		_item_tags_scrollarea->adjustSize();
    //		this->adjustSize();
    //	}else{
    _label_tags->setVisible(true);
    for (auto& label : _item_tags_labels)
        label->setVisible(true);
    _label_tags->adjustSize();
    _item_tags_container->setVisible(true);
    _item_tags_container->show();
    _item_tags_container->activateWindow();

    _item_tags_container->adjustSize(); // setMaximumHeight(_item_tags_layout->sizeHint().height());   //

    auto height = _item_tags_container->height();
    _label_tags->resize(_label_tags->width(), height + 5);
    _item_tags_scrollarea->setMaximumHeight(height + 5);
    _item_tags_scrollarea->adjustSize();
    _item_tags_scrollarea->setVisible(true);
    _item_tags_scrollarea->show();
    _item_tags_scrollarea->activateWindow();
    this->adjustSize();
    //	}
}

// Слот принимает Url метки. Url состоит из порядкового номера метки,
// по нему восстанавливается текст метки
// Slot accepts Url tags . Url consists of a serial number label ,
// On it recovers the label text
void EditorWrap::on_click_to_tag(const QString& link_text)
{
    qDebug() << "Click to tag " << link_text;

    // Текст метки
    QString tag = _item_tags_text_list.at(link_text.toInt());
    qDebug() << "Tag text " << tag;
    // -----------------------------
    // Запуск поиска по тексту метки
    // -----------------------------
    // Определяется ссылка на виджет поиска
    // FindScreen *_find_screen = globalparameters.find_screen();    // find_object<FindScreen>(find_screen_singleton_name);
    // Если виджет не показан, он выводится на экран
    if (_find_screen->isVisible() == false)
        _find_screen->widget_show();
    emit set_find_text_signal(tag);
}

ClickableLabel::ClickableLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent, f)
{
}

ClickableLabel::ClickableLabel(const QString& text, QWidget* parent, Qt::WindowFlags f)
    : QLabel(text, parent, f)
{
}

void EditorWrap::resizeEvent(QResizeEvent* e)
{
    (void)e;
    //	_editing_win->writeSettings();
}
