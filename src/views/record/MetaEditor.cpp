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
#include "views/find_in_base_screen/FindScreen.h"
#include "models/app_config/AppConfig.h"
#include "views/attach_table/AttachTableScreen.h"
#include "views/tree/TreeScreen.h"
#include "views/record_table/RecordView.h"
#include "views/record_table/RecordScreen.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/RecordTable.h"
#include "libraries/FlatControl.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/FindScreen.h"

extern GlobalParameters globalparameters;
extern AppConfig appconfig;

namespace browser {
    class WebPage;
}

MetaEditor::MetaEditor(QString object_name, FindScreen *_find_screen) : Editor()
{
    setObjectName(object_name);
    Editor::disable_tool_list(appconfig.getHideEditorTools());

    Editor::init_enable_assembly(false);
    Editor::init_config_file_name(globalparameters.work_directory() + "/editorconf.ini");
    Editor::init_enable_random_seed(false);

    if(appconfig.getInterfaceMode() == "desktop")
        Editor::init(Editor::WYEDIT_DESKTOP_MODE);
    else if(appconfig.getInterfaceMode() == "mobile")
        Editor::init(Editor::WYEDIT_MOBILE_MODE);
    else
        critical_error("In MetaEditor constructor unknown interface mode: " + appconfig.getInterfaceMode());

    setupLabels();
    setupUI();
    metaAssembly();

    setupSignals(_find_screen);

    // В редакторе устанавливается функция обратного вызова на кнопку Attach
    set_attach_callback(toAttachCallback);

    update_indentline_geometry();
}


MetaEditor::~MetaEditor(void)
{

}


void MetaEditor::setupSignals(FindScreen *_find_screen)
{
    connect(this, &MetaEditor::setFindTextSignal
            , _find_screen  // globalparameters.getFindScreen()
            , &FindScreen::find_text);

}


void MetaEditor::setupLabels(void)
{
    // Путь в дереве до данной записи в виде названий веток (только для мобильного интерфейса)
    _tree_path = new QLabel(this);
    _tree_path->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                        Qt::TextSelectableByKeyboard);

    if(appconfig.getInterfaceMode() == "desktop")
        _tree_path->setVisible(false);
    else
        _tree_path->setVisible(true);

    _tree_path->setWordWrap(true);

    _record_pin = new QCheckBox(this);
    //    recordPin->setTextInteractionFlags(Qt::TextSelectableByMouse |
    //                                       Qt::TextSelectableByKeyboard);
    _record_pin->setFixedWidth(17);
    _record_pin->setVisible(false);
    //    recordPin->setCheckState(_state_check["0"]);
    //    recordPin->setWordWrap(true);

    // Название записи
    _record_name = new QLabel(this);
    _record_name->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                          Qt::TextSelectableByKeyboard);
    _record_name->setVisible(false);
    _record_name->setWordWrap(true);

    // Автор
    _record_author = new QLabel(this);
    _record_author->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                            Qt::TextSelectableByKeyboard);
    _record_author->setVisible(false);
    _record_author->setWordWrap(true);


    // Url
    _label_home = new QLabel(this);
    _label_home->setText(tr("<B>Home:</B> "));
    _label_home->setVisible(false);


    // Url
    _label_url = new QLabel(this);
    _label_url->setText(tr("<B>Url:</B> "));
    _label_url->setVisible(false);

    _record_home = new ClickableLabel(this);
    //    recordHome->setOpenExternalLinks(true);
    _record_home->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                          Qt::TextSelectableByKeyboard |
                                          Qt::LinksAccessibleByMouse |
                                          Qt::LinksAccessibleByKeyboard);
    _record_home->setVisible(false);
    _record_home->setWordWrap(true);

    _record_url = new ClickableLabel(this);
    //    recordUrl->setOpenExternalLinks(true);
    _record_url->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                         Qt::TextSelectableByKeyboard |
                                         Qt::LinksAccessibleByMouse |
                                         Qt::LinksAccessibleByKeyboard);
    _record_url->setVisible(false);
    _record_url->setWordWrap(true);


    // Текстовые метки
    _label_tags = new QLabel(this);
    _label_tags->setText(tr("<B>Tags:</B> "));
    _label_tags->setVisible(false);
}


void MetaEditor::bind(std::shared_ptr<Record> r)
{
    _record = r;
    assert(_record);
    //    TableController *table_controller = globalparameters.table_screen()->table_controller();
    //    assert(table_controller);
    //    int pos = table_controller->table_model()->table_data()->get_pos_by_id(r->getNaturalFieldSource("id"));
    //    table_controller->sychronize_metaeditor_to_record(pos);

    //    setPin(r->getNaturalFieldSource("pin"));
    //    setName(r->getNaturalFieldSource("name"));
    //    setAuthor(r->getNaturalFieldSource("author"));
    //    setTags(r->getNaturalFieldSource("tags"));
    //    setHome(r->getNaturalFieldSource("home"));
    //    setUrl(r->getNaturalFieldSource("url"));

    //    if(r->isLite())
    //        set_textarea(r->getTextDirectFromLite());
    //    else
    //        set_textarea(r->getTextFromFat());

    QObject::disconnect(_url_connection);

    _url_connection = QObject::connect(_record_url, &ClickableLabel::mousePressEvent
                                       // linkActivated
                                       //            , _record->binded_only_page()
    , [this](QMouseEvent * ev) {
        Q_UNUSED(ev)
        assert(_record->unique_page());
        _record->active();
    });

    QObject::disconnect(_home_connection);
    _home_connection = QObject::connect(_record_home, &ClickableLabel::mousePressEvent
                                        //            , _record->binded_only_page()
    , [this](QMouseEvent * ev) {
        Q_UNUSED(ev)
        //            Q_UNUSED(home)
        browser::WebPage *page = _record->unique_page();
        assert(page);
        QString home = _record->natural_field_source("home");

        if(_record->natural_field_source("url") != home)
            _record->natural_field_source("url", home);

        page->equip_registered(_record)->active(); // page->load(_record, true);
        //        _record->active();
    });

}
void MetaEditor::setupUI(void)
{
    // Область текстовых меток, которые выглядят на экране как [метка1] [метка2] [метка3] ...
    _record_tags_layout = new QHBoxLayout();
    _record_tags_layout->setAlignment(Qt::AlignLeft);
    _record_tags_layout->setMargin(0);

    // QHBoxLayout невозможно добавить в QScrollArea, поэтому оборачивается в виджет
    _record_tags_container = new QWidget();
    _record_tags_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    _record_tags_container->setLayout(_record_tags_layout);

    // Создается QScrollArea и в нее добавляется виджет с QHBoxLayout
    _record_tags_scrollarea = new QScrollArea();
    _record_tags_scrollarea->setContentsMargins(0, 0, 0, 0);   // Убирается отступ от границ содержимого
    _record_tags_scrollarea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _record_tags_scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Убирается горизонтальная полоса прокрутки
    _record_tags_scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // Убирается вертикальная полоса прокрутки
    _record_tags_scrollarea->setFrameShape(QFrame::NoFrame); // Убирается тонкая линия вокруг QScrollArea
    _record_tags_scrollarea->setWidget(_record_tags_container);

    _attachtable_screen = new AttachTableScreen(this);
}


void MetaEditor::metaAssembly(void)
{
    // Сборка виджета редактирования текста (основной виджет)
    _editor_main_screen    = new QWidget(this);
    _editor_main_layer     = new QGridLayout(_editor_main_screen);

    _editor_main_layer->addLayout(_textformat_buttons_layout, 0, 0, 1, 2);
    _editor_main_layer->addWidget(_tree_path,                1, 0, 1, 2);
    //    editorMainLayer->addWidget(recordPin,               2, 0, 1, 2);
    _editor_main_layer->addWidget(_record_name,              2, 0, 1, 2);
    _editor_main_layer->addWidget(_record_author,            3, 1, 1, 2);

    _editor_main_layer->addWidget(_text_area,                4, 0, 1, 2);

    _editor_main_layer->addWidget(_label_home,               5, 0);
    _editor_main_layer->addWidget(_record_home,              5, 1);

    _editor_main_layer->addWidget(_label_url,                6, 0);
    _editor_main_layer->addWidget(_record_url,               6, 1);

    _editor_main_layer->addWidget(_label_tags,               7, 0);
    _editor_main_layer->addWidget(_record_tags_scrollarea,    7, 1); // Было addLayout(recordTagsLayout ...)

    _editor_main_layer->setColumnStretch(1, 1);
    _editor_main_layer->setRowStretch(4, 1);   // for textArea auto fullfill

    _editor_main_layer->setContentsMargins(0, 0, 0, 0);

    _editor_main_screen->setLayout(_editor_main_layer);


    // Сборка виджетов в один слой
    _meta_editor_join_layer = new QVBoxLayout(this);
    _meta_editor_join_layer->addWidget(_editor_main_screen);
    _meta_editor_join_layer->addWidget(_attachtable_screen);

    this->setLayout(_meta_editor_join_layer);


    // Границы убираются, так как данный объект будет использоваться как виджет
    QLayout *lt;
    lt = layout();
    lt->setContentsMargins(0, 2, 0, 0);

    // По-умолчанию отображается слой редатирования
    switchToEditorLayout();
}


void MetaEditor::switchToEditorLayout(void)
{
    _attachtable_screen->hide(); // Что бы небыло мерцания, вначале нужно делать сокрытие текущего виджета
    _editor_main_screen->show();
}


void MetaEditor::switchToAttachLayout(void)
{
    _editor_main_screen->hide();
    _attachtable_screen->show();
}


// Статическая функция, обрабатывает клик в редакторе по кнопке переключения на список прикрепляемых файлов
void MetaEditor::toAttachCallback(void)
{
    MetaEditor *edView = find_object<MetaEditor>(meta_editor_singleton_name);
    edView->switchToAttachLayout();
}


// Слот для установки значений инфополей на экране
void MetaEditor::setField(QString n, QString v)
{
    if(n == "pin")          setPin(v);
    else if(n == "name")    setName(v);
    else if(n == "author")  setAuthor(v);
    else if(n == "home")    setHome(v);
    else if(n == "url")     setUrl(v);
    else if(n == "tags")    setTags(v);
    else
        critical_error("metaeditor.set_field Undestand field " + n + " with value " + v);
}


void MetaEditor::clearAll(void)
{
    qDebug() << "MetaEditor::clearAll()" ;

    // Очистка для слоя редактора
    setPin("");
    setName("");
    setAuthor("");
    setUrl("");
    setTags("");
    set_textarea("");

    work_directory("");
    set_file_name("");

    clear_all_misc_field();

    // Иконка аттачей должна показывать что аттачей нет
    _to_attach->setIcon(_icon_attach_not_exists);

    // Очистка для слоя приаттаченных файлов
    _attachtable_screen->clear();
}


void MetaEditor::setTreePath(QString path)
{
    _tree_path->setVisible(true);
    _tree_path->setText(tr("<b>Path: </b>") + path);
}

void MetaEditor::switch_pin()
{
    RecordController *recordtablecontroller = globalparameters.table_screen()->table_controller();

    if(recordtablecontroller) {
        RecordModel *recordmodel = recordtablecontroller->table_model();
        RecordView *recordtableview = recordtablecontroller->view();
        int pos = recordtablecontroller->first_selectionpos();

        if(recordmodel && -1 != pos) {
            // Выясняется ссылка на таблицу конечных данных
            std::shared_ptr<RecordTable> table = recordmodel->tree_item()->tabledata();    //getTableData();

            QString pin = table->field(pos, "pin");
            _record_pin->setCheckState(_state_check[pin]);

            QString home = table->field(pos, "url");

            // Переданные отредактированные поля преобразуются в вид имя-значение
            QMap<QString, QString> editData;

            if(_record_pin->checkState() == Qt::CheckState::Checked) {
                _record_pin->setCheckState(Qt::CheckState::Unchecked);

                setPin(pin = _check_state[Qt::CheckState::Unchecked]);

                editData["pin"] = pin;

            } else {
                _record_pin->setCheckState(Qt::CheckState::Checked);

                setPin(pin = _check_state[Qt::CheckState::Checked]);
                setHome(home);

                editData["pin"] = pin;
                editData["home"] = home;

            }

            //            // Переданные отредактированные поля преобразуются в вид имя-значение
            //            QMap<QString, QString> editData;
            //            editData["pin"] = pin;
            //    editData["name"] = name;
            //    editData["author"] = author;
            //            editData["home"] = home;
            //    editData["url"] = url;
            //    editData["tags"] = tags;

            // Обновление новых данных в таблице конечных записей
            table->edit_record_fields(pos, editData);


            // Сохранение дерева веток
            //find_object<TreeScreen>(tree_screen_singleton_name)->saveKnowTree();
            TreeScreen *treescreen = globalparameters.tree_screen();

            if(treescreen)treescreen->save_knowtree();

            if(recordtableview)recordtableview->setSelectionToPos(pos);
        }
    }
}

void MetaEditor::setPin(QString pin)
{
    //recordPin->setVisible(true);
    _record_pin->setCheckState(_state_check[pin]);
    //    recordPin->setText("<b>" + pin + "</b>");
}

void MetaEditor::setName(QString name)
{
    _record_name->setVisible(true);
    _record_name->setText("<b>" + name + "</b>");
}


void MetaEditor::setAuthor(QString author)
{
    if(author.length() == 0) {
        _record_author->setVisible(false);
        _record_author->setText("");
    } else {
        _record_author->setVisible(true);
        _record_author->setText("<i>" + author + "</i>");
    }
}


void MetaEditor::setHome(QString url)
{
    if(url.length() == 0) {
        _label_home->setVisible(false);
        _record_home->setVisible(false);

        _record_home->setText("");
    } else {
        _label_home->setVisible(true);
        _record_home->setVisible(true);

        if(url.size() > 64)
            _record_home->setText("<a href=\"" + url + "\">" + url.left(64) + "...</a>");
        else
            _record_home->setText("<a href=\"" + url + "\">" + url + "</a>");
    }
}


void MetaEditor::setUrl(QString url)
{
    if(url.length() == 0) {
        _label_url->setVisible(false);
        _record_url->setVisible(false);

        _record_url->setText("");
    } else {
        _label_url->setVisible(true);
        _record_url->setVisible(true);

        if(url.size() > 64)
            _record_url->setText("<a href=\"" + url + "\">" + url.left(64) + "...</a>");
        else
            _record_url->setText("<a href=\"" + url + "\">" + url + "</a>");
    }
}


void MetaEditor::setTags(QString tags)
{
    // Строка с метками запоминается в явном виде
    _record_tags_text = tags;


    // Строка с метками разделяется на отдельные меки
    _record_tags_text_list = _record_tags_text.split(QRegExp("[,;]+"), QString::SkipEmptyParts);

    // В каждой метке убираются лишние пробелы по краям
    for(int i = 0; i < _record_tags_text_list.size(); ++i)
        _record_tags_text_list[i] = _record_tags_text_list.at(i).trimmed();


    // Очищается слой с метками
    QLayoutItem *childItem;

    while((childItem = _record_tags_layout->takeAt(0)) != 0) {
        _record_tags_layout->removeItem(childItem);

        if(childItem->widget() != nullptr && childItem->widget() != 0)
            delete(childItem->widget());

        delete childItem;
    }

    // Очищается список меток
    _record_tags_labels.clear();

    // Создаются метки для экрана и запоминаются в список
    for(int i = 0; i < _record_tags_text_list.size(); ++i) {
        // Объект метки на экране
        QLabel *tempLabel = new QLabel(this);
        tempLabel->setOpenExternalLinks(false);
        tempLabel->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                           Qt::TextSelectableByKeyboard |
                                           Qt::LinksAccessibleByMouse |
                                           Qt::LinksAccessibleByKeyboard);
        tempLabel->setVisible(true);
        tempLabel->setWordWrap(false);

        // Настройка, чтоб не появлялась некрасивая пунктирная рамка
        // tempLabel->setFrameStyle(QFrame::NoFrame);
        tempLabel->setFocusPolicy(Qt::NoFocus);

        // Метке задается текст
        // В Url указывается порядковый номер метки
        QString labelText = "<a href=\"" + QString::number(i) + "\">" + _record_tags_text_list.at(i) + "</a>";
        tempLabel->setText(labelText);

        // Клик по метке будет вызывать сигнал, слот будет принимать Url метки,
        // то есть в данном случае строку с номером метки
        // Clicking on a label will cause the signal slot will accept Url tag
        // Ie in this case the line with the number of tags
        connect(tempLabel, SIGNAL(linkActivated(const QString &)), this, SLOT(onClickToTag(const QString &)));

        // Метка запоминается в список меток
        _record_tags_labels << tempLabel;

        // Метка добавляется в область размещения
        _record_tags_layout->addWidget(tempLabel);
    }

    // В конец списка виджетов добавляется распорка
    _record_tags_layout->addStretch();

    // Если нечего выводить на экран
    if(tags.length() == 0 || _record_tags_text_list.size() == 0) {
        _label_tags->setVisible(false);

        for(int i = 0; i < _record_tags_labels.size(); ++i)
            _record_tags_labels.at(i)->setVisible(false);
    } else {
        _label_tags->setVisible(true);

        for(int i = 0; i < _record_tags_labels.size(); ++i)
            _record_tags_labels.at(i)->setVisible(true);
    }

    _record_tags_container->adjustSize();
    _record_tags_scrollarea->setMaximumHeight(_record_tags_container->height());
}


// Слот принимает Url метки. Url состоит из порядкового номера метки,
// по нему восстанавливается текст метки
// Slot accepts Url tags . Url consists of a serial number label ,
// On it recovers the label text
void MetaEditor::onClickToTag(const QString &link_text)
{
    qDebug() << "Click to tag " << link_text;

    // Текст метки
    QString tag = _record_tags_text_list.at(link_text.toInt());
    qDebug() << "Tag text " << tag;

    // -----------------------------
    // Запуск поиска по тексту метки
    // -----------------------------

    // Определяется ссылка на виджет поиска
    FindScreen *findScreen = find_object<FindScreen>(find_screen_singleton_name);

    // Если виджет не показан, он выводится на экран
    if(findScreen->isVisible() == false)
        findScreen->widget_show();

    emit setFindTextSignal(tag);
}
