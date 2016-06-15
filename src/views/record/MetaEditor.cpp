#include <functional>
#include <QBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QSplitter>
#include <QtDebug>


#include "main.h"
#include "MetaEditor.h"

#include "libraries/wyedit/Editor.h"
#include "libraries/wyedit/EditorTextArea.h"
#include "libraries/GlobalParameters.h"
#include "models/app_config/AppConfig.h"
#include "views/attach_table/AttachTableScreen.h"
#include "models/record_table/Record.h"
#include "views/record_table/RecordView.h"
#include "views/record_table/RecordScreen.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/ItemsFlat.h"
#include "models/tree/TreeItem.h"
#include "views/tree/TreeScreen.h"
#include "libraries/FlatControl.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "views/browser/entrance.h"
#include "views/browser/tabwidget.h"
#include "views/tree/KnowView.h"



extern GlobalParameters globalparameters;
extern AppConfig appconfig;

namespace browser {
    class WebPage;
    class Entrance;
}

MetaEditor::MetaEditor(QString object_name, FindScreen *_find_screen)
    : Editor()
      , _find_screen(_find_screen)
      , _tree_path(new QLabel(this))
      , _item_pin(new QCheckBox(this))
      , _item_name(new QLabel(this))
      , _item_author(new QLabel(this))
      , _label_home(new QLabel(this))
      , _label_url(new QLabel(this))
      , _item_home(new ClickableLabel(this))
      , _item_url(new ClickableLabel(this))
      , _label_tags(new QLabel(this))
      , _item_tags_scrollarea(new QScrollArea(this))
      , _item_tags_scrollarea_layout(new QHBoxLayout(_item_tags_scrollarea))
      , _item_tags_container(new QWidget(_item_tags_scrollarea))
      , _item_tags_layout(new QHBoxLayout(_item_tags_container))
      , _attachtable_screen(new AttachTableScreen(this))
      // Сборка виджета редактирования текста (основной виджет)
      , _editor_main_screen(new QWidget(this))
      , _editor_main_layer(new QGridLayout(_editor_main_screen))
      , _meta_editor_join_layer(new QVBoxLayout(this)){
    setObjectName(object_name);
    Editor::disable_tool_list(appconfig.hide_editor_tools());

    Editor::init_enable_assembly(false);
    Editor::init_config_file_name(globalparameters.work_directory() + "/editorconf.ini");
    Editor::init_enable_random_seed(false);
    if(appconfig.interface_mode() == "desktop")Editor::init(Editor::WYEDIT_DESKTOP_MODE);
    else if(appconfig.interface_mode() == "mobile")Editor::init(Editor::WYEDIT_MOBILE_MODE);
    else critical_error("In MetaEditor constructor unknown interface mode: " + appconfig.interface_mode());
    setup_labels();
    setup_ui();
    assembly();
    clear_all();
    adjustSize();
    setup_signals(_find_screen);

    // В редакторе устанавливается функция обратного вызова на кнопку Attach
    attach_callback(to_attach_callback);

    Editor::update_indentline_geometry();
}


MetaEditor::~MetaEditor(void)
{}


void MetaEditor::setup_signals(FindScreen *_find_screen){
    connect(this, &MetaEditor::set_find_text_signal, _find_screen, &FindScreen::find_text);
}


void MetaEditor::setup_labels(void){
    // Путь в дереве до данной записи в виде названий веток (только для мобильного интерфейса)
    // _tree_path = new QLabel(this);
    _tree_path->setTextInteractionFlags(Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard);
    if(appconfig.interface_mode() == "desktop")_tree_path->setVisible(false);
    else _tree_path->setVisible(true);
    _tree_path->setWordWrap(true);

    // _item_pin = new QCheckBox(this);
    // recordPin->setTextInteractionFlags(Qt::TextSelectableByMouse |
    // Qt::TextSelectableByKeyboard);
    _item_pin->setFixedWidth(17);
    _item_pin->setVisible(false);
    // recordPin->setCheckState(_state_check["0"]);
    // recordPin->setWordWrap(true);

    // Название записи
    // _item_name = new QLabel(this);
    _item_name->setTextInteractionFlags(Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard);
    _item_name->setVisible(false);
    _item_name->setWordWrap(true);

    // Автор
    // _item_author = new QLabel(this);
    _item_author->setTextInteractionFlags(Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard);
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
    _item_home->setTextInteractionFlags(Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard |
        Qt::LinksAccessibleByMouse |
        Qt::LinksAccessibleByKeyboard);
    _item_home->setVisible(false);
    _item_home->setWordWrap(true);

    // _item_url = new ClickableLabel(this);
    // recordUrl->setOpenExternalLinks(true);
    _item_url->setTextInteractionFlags(Qt::TextSelectableByMouse |
        Qt::TextSelectableByKeyboard |
        Qt::LinksAccessibleByMouse |
        Qt::LinksAccessibleByKeyboard);
    _item_url->setVisible(false);
    _item_url->setWordWrap(true);


    // Текстовые метки
    // _label_tags = new QLabel(this);
    _label_tags->setText(tr("<B>Tags:</B> "));
    _label_tags->setVisible(false);
}


void MetaEditor::bind(boost::intrusive_ptr<TreeItem> item_to_be_bound){
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

    QObject::disconnect(_url_connection);

    _url_connection = QObject::connect(_item_url, &ClickableLabel::mousePressEvent  // linkActivated
            // , _record->binded_only_page()
                                      , [this](QMouseEvent *ev){
                Q_UNUSED(ev)
                assert(_item);
                assert(_item->page_valid() && _item->page());
                _item->activate(std::bind(&browser::Entrance::find, globalparameters.entrance(), std::placeholders::_1));
            });

    QObject::disconnect(_home_connection);
    _home_connection = QObject::connect(_item_home, &ClickableLabel::mousePressEvent
                                       , [&](QMouseEvent *ev){
                Q_UNUSED(ev)
                // Q_UNUSED(home)
                assert(_item);
                assert(_item->page_valid());
                browser::WebPage *page = _item->page();
                assert(page);
                QString home = _item->field<home_type>();
                QString url = _item->field<url_type>();
                if(home != browser::Browser::_defaulthome && url != home){
// _item->field("url", home);
                    static_cast<QWebEnginePage *>(page)->setUrl(QUrl(home)); // item_bind(_item)->activate(); // page->load(_record, true);
                    // _record->active();
                }
            });
}


boost::intrusive_ptr<TreeItem> MetaEditor::item(){
    return _item;
}

void MetaEditor::setup_ui(void){
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
    _item_tags_scrollarea->setContentsMargins(0, 0, 0, 0);   // Убирается отступ от границ содержимого
    _item_tags_scrollarea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);   // (QSizePolicy::Expanding, QSizePolicy::Preferred);
    _item_tags_scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Убирается горизонтальная полоса прокрутки
    _item_tags_scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Убирается вертикальная полоса прокрутки
    _item_tags_scrollarea->setFrameShape(QFrame::NoFrame); // Убирается тонкая линия вокруг QScrollArea

    _item_tags_scrollarea->setLayout(_item_tags_scrollarea_layout); // _item_tags_scrollarea->setWidget(_item_tags_container);

    // _attachtable_screen = new AttachTableScreen(this);
}


void MetaEditor::assembly(void){
    //// Сборка виджета редактирования текста (основной виджет)
    // _editor_main_screen    = new QWidget(this);
    // _editor_main_layer     = new QGridLayout(_editor_main_screen);

    _editor_main_layer->addLayout(_textformat_buttons_layout,   0, 0, 1, 2);
    _editor_main_layer->addWidget(_item_pin,                    1, 0, 1, 1);
    _editor_main_layer->addWidget(_tree_path,                   1, 1, 1, 1);

    _editor_main_layer->addWidget(_item_name,                   2, 0, 1, 2);
    _editor_main_layer->addWidget(_item_author,                 3, 1, 1, 2);

    _editor_main_layer->addWidget(_text_area,                   4, 0, 1, 2);

    _editor_main_layer->addWidget(_label_home,                  5, 0, 1, 1);
    _editor_main_layer->addWidget(_item_home,                   5, 1, 1, 1);

    _editor_main_layer->addWidget(_label_url,                   6, 0, 1, 1);
    _editor_main_layer->addWidget(_item_url,                    6, 1, 1, 1);

    _editor_main_layer->addWidget(_label_tags,                  7, 0, 1, 1);
    _editor_main_layer->addWidget(_item_tags_scrollarea,        7, 1, 1, 1); // addLayout(_item_tags_scrollarea_layout, 7, 1, 1, 1);    // Было addLayout(recordTagsLayout ...)    //

    _editor_main_layer->setColumnStretch(1, 1);
    _editor_main_layer->setRowStretch(4, 1);   // for textArea auto fullfill

    _editor_main_layer->setContentsMargins(0, 0, 0, 0);

    _editor_main_screen->setLayout(_editor_main_layer);


    // Сборка виджетов в один слой
    // _meta_editor_join_layer = new QVBoxLayout(this);
    _meta_editor_join_layer->addWidget(_editor_main_screen);
    _meta_editor_join_layer->addWidget(_attachtable_screen);

    this->setLayout(_meta_editor_join_layer);


    // Границы убираются, так как данный объект будет использоваться как виджет
    QLayout *lt;
    lt = layout();
    lt->setContentsMargins(0, 0, 0, 0); // setContentsMargins(0, 2, 0, 0);

    // По-умолчанию отображается слой редатирования
    to_editor_layout();
}


void MetaEditor::to_editor_layout(void){
    _attachtable_screen->hide(); // Что бы небыло мерцания, вначале нужно делать сокрытие текущего виджета
    _editor_main_screen->show();
}


void MetaEditor::to_attach_layout(void){
    _editor_main_screen->hide();
    _attachtable_screen->show();
}


// Статическая функция, обрабатывает клик в редакторе по кнопке переключения на список прикрепляемых файлов
void MetaEditor::to_attach_callback(void){
    MetaEditor *edView = globalparameters.meta_editor();    // find_object<MetaEditor>(meta_editor_singleton_name);
    edView->to_attach_layout();
}


// Слот для установки значений инфополей на экране
void MetaEditor::field(QString n, QString v){
    if(n == "pin")pin(v);
    else if(n == "name")name(v);
    else if(n == "author")author(v);
    else if(n == "home")home(v);
    else if(n == "url")url(v);
    else if(n == "tags")tags(v);
    else critical_error("metaeditor.set_field Undestand field " + n + " with value " + v);
}


void MetaEditor::clear_all(void){
    qDebug() << "MetaEditor::clear_all()";

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


void MetaEditor::tree_path(QString path){
    _tree_path->setVisible(true);
    _tree_path->setText(tr("<b>Path: </b>") + path);
}

void MetaEditor::switch_pin(){
    // if(globalparameters.entrance()->activiated_browser()) {
    browser::TabWidget *_tabmanager = globalparameters.entrance()->activated_browser()->tabmanager();
    // record_screens()->record_controller();
    if(_tabmanager){
        RecordModel *source_model = _tabmanager->source_model();
        auto _record_controller = _tabmanager->record_controller();
        RecordView *record_view = _tabmanager->view();
        PosSource pos_source_(_tabmanager->currentIndex());   // first_selectionpos();
        if(source_model && - 1 != pos_source_){
            // Выясняется ссылка на таблицу конечных данных
            // auto item = source_model->tree_item();  //->record_table();    //getTableData();

            QString p = source_model->item(pos_source_)->field<pin_type>(); // "pin"
            _item_pin->setCheckState(_state_check_from_string[p]);

            QString h = source_model->item(pos_source_)->field<url_type>(); // "url"

            // Переданные отредактированные поля преобразуются в вид имя-значение
            QMap<QString, QString> edit_data;
            if(_item_pin->checkState() == Qt::CheckState::Checked){
                _item_pin->setCheckState(Qt::CheckState::Unchecked);

                pin(p = _string_from_check_state[Qt::CheckState::Unchecked]);

                edit_data["pin"] = p;
            }else{
                _item_pin->setCheckState(Qt::CheckState::Checked);

                pin(p = _string_from_check_state[Qt::CheckState::Checked]);
                home(h);

                edit_data["pin"] = p;
                edit_data["home"] = h;
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
            TreeScreen *treescreen = globalparameters.tree_screen();
            if(treescreen)treescreen->view()->know_model_save();
            if(record_view)_record_controller->cursor_to_index(_record_controller->index<PosProxy>(pos_source_));
        }
    }
    // }
}

void MetaEditor::pin(QString pin_){
    // recordPin->setVisible(true);
    _item_pin->setCheckState(_state_check_from_string[pin_]);
    // recordPin->setText("<b>" + pin + "</b>");
}

void MetaEditor::name(QString name_){
    _item_name->setVisible(true);
    _item_name->setText("<b>" + name_ + "</b>");
}


void MetaEditor::author(QString author_){
    if(author_.length() == 0){
        _item_author->setVisible(false);
        _item_author->setText("");
    }else{
        _item_author->setVisible(true);
        _item_author->setText("<i>" + author_ + "</i>");
    }
}


void MetaEditor::home(QString url_){
    if(url_.length() == 0){
        _label_home->setVisible(false);
        _item_home->setVisible(false);

        _item_home->setText("");
    }else{
        _label_home->setVisible(true);
        _item_home->setVisible(true);
        if(url_.size() > 64)_item_home->setText("<a href=\"" + url_ + "\">" + url_.left(64) + "...</a>");
        else _item_home->setText("<a href=\"" + url_ + "\">" + url_ + "</a>");
    }
}


void MetaEditor::url(QString url_){
    if(url_.length() == 0){
        _label_url->setVisible(false);
        _item_url->setVisible(false);

        _item_url->setText("");
    }else{
        _label_url->setVisible(true);
        _item_url->setVisible(true);
        if(url_.size() > 64)_item_url->setText("<a href=\"" + url_ + "\">" + url_.left(64) + "...</a>");
        else _item_url->setText("<a href=\"" + url_ + "\">" + url_ + "</a>");
    }
}


void MetaEditor::tags(QString tags_){

    _item_tags_text_list.clear();

    // Строка с метками запоминается в явном виде
    _item_tags_text = tags_;


    // Строка с метками разделяется на отдельные меки
    _item_tags_text_list = _item_tags_text.split(QRegExp("[,;]+"), QString::SkipEmptyParts);
    // В каждой метке убираются лишние пробелы по краям
    for(int i = 0; i < _item_tags_text_list.size(); ++ i)_item_tags_text_list[i] = _item_tags_text_list.at(i).trimmed();
    //// Очищается слой с метками
    // QLayoutItem *child_item;
    // auto size = _item_tags_text_list.size();
    // auto count = _item_tags_layout->count();
    while(_item_tags_layout->count() > 0){  // (child_item = _item_tags_layout->takeAt(0)) != 0
        auto child_item = _item_tags_layout->takeAt(0);
        _item_tags_layout->removeItem(child_item);
        auto widget = child_item->widget();
        if(widget){     // if(child_item->widget() != nullptr && child_item->widget() != 0)
            disconnect(static_cast<QLabel *>(widget), &QLabel::linkActivated, this, &MetaEditor::on_click_to_tag);
            delete (widget);
        }
        delete child_item;
    }
    // Очищается список меток
    _item_tags_labels.clear();
    // Создаются метки для экрана и запоминаются в список
    for(auto &text : _item_tags_text_list){
        // Объект метки на экране
        QLabel *_label = new QLabel(this);
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
            + text                   // QString::number(i)
            + "\">" + text + "</a>";

        _label->setText(label_text);

        // Клик по метке будет вызывать сигнал, слот будет принимать Url метки,
        // то есть в данном случае строку с номером метки
        // Clicking on a label will cause the signal slot will accept Url tag
        // Ie in this case the line with the number of tags
        connect(_label, &QLabel::linkActivated, this, &MetaEditor::on_click_to_tag);

        // Метка запоминается в список меток
        _item_tags_labels << _label;

        // Метка добавляется в область размещения
        _item_tags_layout->addWidget(_label);   // addItem(new QWidgetItem(_label));  //
    }
    // В конец списка виджетов добавляется распорка
    _item_tags_layout->addStretch();
    // Если нечего выводить на экран
    if(tags_.length() == 0 || _item_tags_text_list.size() == 0){
        _label_tags->setVisible(false);
        for(auto &label : _item_tags_labels)label->setVisible(false);
    }else{
        _label_tags->setVisible(true);
        for(auto &label : _item_tags_labels)label->setVisible(true);
        _label_tags->adjustSize();
        _item_tags_container->adjustSize(); // setMaximumHeight(_item_tags_layout->sizeHint().height());   //
        auto height = _item_tags_container->height();
        _label_tags->resize(_label_tags->width(), height + 5);
        _item_tags_scrollarea->setMaximumHeight(height + 5);
    }
}


// Слот принимает Url метки. Url состоит из порядкового номера метки,
// по нему восстанавливается текст метки
// Slot accepts Url tags . Url consists of a serial number label ,
// On it recovers the label text
void MetaEditor::on_click_to_tag(const QString &link_text){
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
    if(_find_screen->isVisible() == false)_find_screen->widget_show();
    emit set_find_text_signal(tag);
}
