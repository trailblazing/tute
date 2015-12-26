#include <QSizePolicy>

#include "InfoFieldEnter.h"
#include "main.h"
#include "models/app_config/AppConfig.h"
#include "views/browser/tabwidget.h"
#include "libraries/FlatControl.h"


extern AppConfig appconfig;

QMap<Qt::CheckState, QString> _check_state{
    {
        Qt::CheckState::Checked
        , QChar(0x2705)//QChar(0x2713)//QChar(0x2611) // QChar(Qt::UNICODE_ACCEL + 2713) // QChar(Qt::ALT + 251)  //  "v"   //QChar(0xfb)
    }
    , {Qt::CheckState::Unchecked, ""}
};

QMap<QString, Qt::CheckState> _state_check{
    {
        QChar(0x2705) // QChar(Qt::UNICODE_ACCEL + 2713) // QChar(Qt::ALT + 251)  //  "v"    //QChar(0xfb)
        , Qt::CheckState::Checked
    }
    , {"", Qt::CheckState::Unchecked}
};

// Виджет ввода инфополей


InfoFieldEnter::InfoFieldEnter(QWidget *parent)
    : QWidget(parent)
    , _recordpinlabel(new QLabel(this))
    , _recordpin(new QCheckBox(this))
{
    //    _check_state[Qt::CheckState::Checked] = "v";
    //    _check_state[Qt::CheckState::Unchecked] = "";

    //    _state_check[QString("v")] = Qt::CheckState::Checked;
    //    _state_check[QString("")] = Qt::CheckState::Unchecked;

    setup_ui();
    setup_signals();
    assembly();
}

InfoFieldEnter::~InfoFieldEnter()
{

}

void InfoFieldEnter::setup_ui(void)
{
    _recordpinlabel->setText(tr("Pin"));
    _recordpin->setFixedWidth(17);
    // Элементы для запроса названия записи
    recordNameLabel = new QLabel(this);
    recordNameLabel->setText(tr("Title"));
    recordName = new QLineEdit(this);
    recordName->setMinimumWidth(500);

    // Элементы для запроса автора (авторов)
    recordAuthorLabel = new QLabel(this);
    recordAuthorLabel->setText(tr("Author(s)"));
    recordAuthor = new QLineEdit(this);


    // Элементы для запроса Url источника
    recordHomeLabel = new QLabel(this);
    recordHomeLabel->setText(tr("Home"));
    recordHome = new QLineEdit(this);

    // Элементы для запроса Url источника
    recordUrlLabel = new QLabel(this);
    recordUrlLabel->setText(tr("Url"));
    recordUrl = new QLineEdit(this);

    // Элементы для запроса текстовых меток
    recordTagsLabel = new QLabel(this);
    recordTagsLabel->setText(tr("Tags"));
    recordTags = new QLineEdit(this);

    // Кнопка раскрытия или закрытия полей author, url, tags...
    // Она в два раза меньше обычного размера
    expandInfo = new FlatToolButton(this);
    expandInfo->setObjectName("infoFieldEnterExpandInfo");
    expandInfo->setVisible(true);
    int w = expandInfo->geometry().width();
    int h = expandInfo->geometry().height();
    int x = imin(w, h) / 2;
    expandInfo->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
    expandInfo->setMinimumSize(x, x);
    expandInfo->setMaximumSize(x, x);
    expandInfo->resize(x, x);

    if(appconfig.get_addnewrecord_expand_info() == "0") {
        expandInfo->setIcon(QIcon(":/resource/pic/triangl_dn.svg"));
        // expandInfo->setIcon(this->style()->standardIcon(QStyle::SP_ArrowDown));
    } else {
        expandInfo->setIcon(QIcon(":/resource/pic/triangl_up.svg"));
        // expandInfo->setIcon(this->style()->standardIcon(QStyle::SP_ArrowUp));
    }
}


void InfoFieldEnter::setup_signals(void)
{
    connect(expandInfo, SIGNAL(pressed()), this, SLOT(expandInfoClick(void)));
}


// Элементы собираются в размещалку
void InfoFieldEnter::assembly(void)
{
    // Размещалка элементов
    infoFieldLayout = new QGridLayout(); // Попробовать this
    infoFieldLayout->setMargin(8);
    infoFieldLayout->setSpacing(10);

    int y = -1;

    infoFieldLayout->addWidget(_recordpinlabel, ++y, 0);
    infoFieldLayout->addWidget(_recordpin, y, 1);

    infoFieldLayout->addWidget(recordNameLabel, ++y, 0);
    infoFieldLayout->addWidget(recordName, y, 1);

    infoFieldLayout->addWidget(expandInfo, y, 2);

    infoFieldLayout->addWidget(recordAuthorLabel, ++y, 0);
    infoFieldLayout->addWidget(recordAuthor, y, 1);

    infoFieldLayout->addWidget(recordHomeLabel, ++y, 0);
    infoFieldLayout->addWidget(recordHome, y, 1);

    infoFieldLayout->addWidget(recordUrlLabel, ++y, 0);
    infoFieldLayout->addWidget(recordUrl, y, 1);

    infoFieldLayout->addWidget(recordTagsLabel, ++y, 0);
    infoFieldLayout->addWidget(recordTags, y, 1);

    // Устанавливается видимость или невидимость полей author, url, tags...
    expandInfoOnDisplay(appconfig.get_addnewrecord_expand_info());

    // Полученый набор элементов устанавливается для текущего виджета
    setLayout(infoFieldLayout);

    // Границы убираются, так как данный объект будет использоваться
    // как виджет
    QLayout *lt;
    lt = layout();
    lt->setContentsMargins(0, 0, 0, 0);

    // setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
}


void InfoFieldEnter::expandInfoOnDisplay(QString expand)
{
    bool i;

    if(expand == "0")
        i = false;
    else
        i = true;


    _recordpinlabel->setVisible(i);
    _recordpin->setVisible(i);

    recordAuthorLabel->setVisible(i);
    recordAuthor->setVisible(i);

    recordHomeLabel->setVisible(i);
    recordHome->setVisible(i);

    recordUrlLabel->setVisible(i);
    recordUrl->setVisible(i);

    recordTagsLabel->setVisible(i);
    recordTags->setVisible(i);
}


void InfoFieldEnter::expandInfoClick(void)
{
    // Если в данный момент информация "свернута"
    if(appconfig.get_addnewrecord_expand_info() == "0") {
        // Надо информацию развернуть
        expandInfoOnDisplay("1");

        appconfig.set_addnewrecord_expand_info("1");

        expandInfo->setIcon(QIcon(":/resource/pic/triangl_up.svg"));
    } else {
        // Надо информацию свернуть
        expandInfoOnDisplay("0");

        appconfig.set_addnewrecord_expand_info("0");

        expandInfo->setIcon(QIcon(":/resource/pic/triangl_dn.svg"));
    }
}


void InfoFieldEnter::setFocusToStart(void)
{
    recordName->setFocus(Qt::TabFocusReason);
}


bool InfoFieldEnter::checkFieldName(QString name)
{
    if(name == "pin" ||
       name == "name" ||
       name == "author" ||
       name == "home" ||
       name == "url" ||
       name == "tags")
        return true;
    else
        return false;
}


QString InfoFieldEnter::getField(QString name)
{
    if(checkFieldName(name)) {
        if(name == "pin")   return  _check_state[_recordpin->checkState()];

        if(name == "name")  return  recordName->text();

        if(name == "author")return  recordAuthor->text();

        if(name == "home")  return  recordHome->text();

        if(name == "url")   return  recordUrl->text();

        if(name == "tags")  return  recordTags->text();
    } else
        criticalError("Can not get field " + name + " in InfoFieldEnter method get_field");

    return QString();
}


void InfoFieldEnter::setField(QString name, QString value)
{
    if(checkFieldName(name)) {
        if(name == "pin")       _recordpin->setCheckState(_state_check[value]);

        if(name == "name")      recordName->setText(value);

        if(name == "author")    recordAuthor->setText(value);

        if(name == "home")      recordHome->setText(value);

        if(name == "url")       recordUrl->setText(value);

        if(name == "tags")      recordTags->setText(value);
    } else
        criticalError("Can not set field " + name + " in InfoFieldEnter method set_field");
}
