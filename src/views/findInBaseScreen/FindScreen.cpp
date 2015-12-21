#include <cassert>
#include <utility>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QWidget>
#include <QProgressDialog>
#include <QLabel>
#include <QCheckBox>
#include <QMessageBox>
#include <QBoxLayout>
#include <QTextDocument>
#include <QLineEdit>
#include <QByteArray>
#include <QtWidgets/QStackedWidget>

#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "FindScreen.h"
#include "FindTableWidget.h"
#include "models/tree/KnowTreeModel.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/TreeItem.h"
#include "views/record/MetaEditor.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/TreeScreen.h"
#include "libraries/FlatControl.h"
#include "views/tree/KnowTreeView.h"
#include "views/browser/entrance.h"
#include "views/browser/toolbarsearch.h"
#include "views/browser/chasewidget.h"
#include "libraries/FlatControl.h"
#include "models/recordTable/TableData.h"

extern AppConfig appconfig;
extern GlobalParameters globalparameters;


FindScreen::FindScreen(QWidget *parent)
    : QWidget(parent)
    , _navigater(new QToolBar(this))
    , _chasewidget(new browser::ChaseWidget(QSize(17, 17), this))
    , _progress(new QProgressDialog(this))
      //    , _findtable(new FindTableWidget(this))
      //    , _result
    , _toolbarsearch(new browser::ToolbarSearch(this))
{
    _navigater->setMaximumWidth(130);
    _chasewidget->setMaximumWidth(20);
    _progress->reset();
    _toolbarsearch->setMinimumWidth(100);

    _toolbarsearch->sizePolicy().setHorizontalStretch(10);
    //_toolbarsearch->setContentsMargins(0, 0, 0, 0);

    //    if(appconfig.getInterfaceMode() == "desktop") {
    //        _navigater->setMinimumWidth(400);
    //    }

    setup_navigate();
    assembly_navigate();

    setupFindTextAndButton();
    assemblyFindTextAndButton();

    setupCloseButton();
    assemblyCloseButton();

    setupComboOption();
    //assemblyComboOption();

    setupWhereFindLine();
    assemblyWhereFindLine();

    setupUI();
    assembly();

    setupSignals();
}


FindScreen::~FindScreen(void)
{

}

void FindScreen::setup_navigate(void)
{
    //    assert(globalparameters.entrance());
    //    assert(globalparameters.entrance()->activebrowser());
    //    this->addAction(globalparameters.entrance()->activebrowser()->historyback());

    //    historyback_ = new FlatToolButton(this);
    //    historyback_->setStatusTip(tr("Back"));
    //    historyback_->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0, this));    //QIcon(":/resource/pic/pentalpha.svg");
    //    historyback_->setObjectName("Back");

    //_history_home = new FlatToolButton(this);
    _historyhome = new QAction(tr("Home"), _navigater);
    _historyhome->setIcon(
        //        QIcon(
        //            ":/resource/pic/up-arrow-circle.svg"    // ":/resource/pic/streamline_home.svg"
        //        )  //
        style()->standardIcon(QStyle::SP_ArrowUp, 0, this)
    );

    _historyhome->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));

    insertActionAsButton(_navigater, _historyhome); // _navigater->addAction(_historyhome);

    //_history_back = new FlatToolButton(this);
    _historyback = new QAction(tr("Back"), _navigater);
    _historyback->setShortcuts(QKeySequence::Back);
    //    _historyback->setIconVisibleInMenu(false);
    _historyback->setIcon(  //QIcon(":/resource/pic/walk_history_next.svg")//
        style()->standardIcon(QStyle::SP_ArrowBack, 0, this)
    );
    //    QMenu *_historybackmenu = new QMenu(this);
    //    _historyback->setMenu(_historybackmenu);
    //    connect(_historybackmenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowBackMenu()));
    //    connect(_historybackmenu, SIGNAL(triggered(QAction *)), this, SLOT(slotOpenActionUrl(QAction *)));
    insertActionAsButton(_navigater, _historyback); // _navigater->addAction(_historyback);
    //insertActionAsButton(_container, _historyback);

    //_history_forward = new FlatToolButton(this);
    _historyforward = new QAction(tr("Forward"), _navigater);
    _historyforward->setShortcuts(QKeySequence::Forward);
    _historyforward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward, 0, this));
    //    _historyforwardmenu = new QMenu(this);
    //    connect(_historyforwardmenu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowForwardMenu()));
    //    connect(_historyforwardmenu, SIGNAL(triggered(QAction *)), this, SLOT(slotOpenActionUrl(QAction *)));
    //    _historyforward->setMenu(_historyforwardmenu);
    insertActionAsButton(_navigater, _historyforward); // _navigater->addAction(_historyforward);


    //_stop_reload = new FlatToolButton(this);
    _stopreload = new QAction(_navigater);
    //_reloadicon = style()->standardIcon(QStyle::SP_BrowserReload);
    _stopreload->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    insertActionAsButton(_navigater, _stopreload); // _navigater->addAction(_stopreload);


}

void FindScreen::assembly_navigate(void)
{
    //    _navigater = new QHBoxLayout();
    //    _navigater->addWidget(_container);
    //    _navigater->addStretch();

}

// Текст поиска и кнопка "Поиск"
void FindScreen::setupFindTextAndButton(void)
{
    // Поле текста для поиска
    //_findtext = new QLineEdit();

    // Кнопка "Поиск"
    _findstartbutton = new FlatToolButton(this);    // QPushButton
    _findstartbutton->setText(tr("Find"));
    //    _findstartbutton->setDefault(true);
    _findstartbutton->setEnabled(false);
    _findstartbutton->setFixedWidth(50);
    //    _findstartbutton->setFlat(true);
    _findstartbutton->setAutoRaise(true);

    // Кнопка разворачивания инструментов
    toolsExpand = new FlatToolButton(this);
    toolsExpand->setIcon(QIcon(":/resource/pic/find_in_base_expand_tools.svg"));
    toolsExpand->setEnabled(true);
    toolsExpand->setAutoRaise(true);
}


// Текст поиска и кнопка "Поиск"
void FindScreen::assemblyFindTextAndButton(void)
{
    toolsAreaFindTextAndButton = new QHBoxLayout();
    //    QWidget *con = new QWidget(this);
    //    QHBoxLayout *addressbar = new QHBoxLayout();
    //    addressbar->addWidget(_toolbarsearch);
    //    addressbar->addWidget(_chasewidget);

    //    addressbar->setContentsMargins(0, 0, 0, 0);
    //    con->setLayout(addressbar);
    toolsAreaFindTextAndButton->addWidget(_navigater);
    //    toolsAreaFindTextAndButton->addWidget(_history_home);
    //    toolsAreaFindTextAndButton->addWidget(_history_forward);
    //    toolsAreaFindTextAndButton->addWidget(_history_back);
    //    toolsAreaFindTextAndButton->addWidget(_stop_reload);
    //    toolsAreaFindTextAndButton->addLayout(addressbar);
    toolsAreaFindTextAndButton->addWidget(_toolbarsearch);  //_findtext
    //    toolsAreaFindTextAndButton->addWidget(con);
    toolsAreaFindTextAndButton->addWidget(_chasewidget);
    toolsAreaFindTextAndButton->addWidget(_findstartbutton);
    toolsAreaFindTextAndButton->addWidget(toolsExpand);
    //toolsAreaFindTextAndButton->addStretch();
    //toolsAreaFindTextAndButton->setContentsMargins(0, 0, 0, 0);
}


// Набор опций поиска в виде выпадающих списков
void FindScreen::setupComboOption(void)
{
    // Выбор "Любое слово" - "Все слова"
    wordRegard = new FlatComboBox();
    wordRegard->addItem(QIcon(":/resource/pic/find_in_base_any.svg"), tr("Any word"));
    wordRegard->addItem(QIcon(":/resource/pic/find_in_base_all.svg"), tr("All words"));
    wordRegard->setCurrentIndex(appconfig.get_findscreen_wordregard());

    // Выбор "Только целые слова" - "Подстрока"
    howExtract = new FlatComboBox();
    howExtract->addItem(QIcon(":/resource/pic/find_in_base_separate.svg"), tr("Whole words"));
    howExtract->addItem(QIcon(":/resource/pic/find_in_base_substring.svg"), tr("Substring"));
    howExtract->setCurrentIndex(appconfig.get_findscreen_howextract());

    // Выбор "Во всей базе" - "В текущей ветке"
    treeSearchArea = new FlatComboBox();
    treeSearchArea->addItem(QIcon(":/resource/pic/find_in_base_search_all.svg"), tr("Entire base")); // Вся база
    treeSearchArea->addItem(QIcon(":/resource/pic/find_in_base_search_branch.svg"), tr("In current branch")); // Текущая ветка
    treeSearchArea->setCurrentIndex(appconfig.getFindScreenTreeSearchArea());

    if(appconfig.getInterfaceMode() == "desktop") {
        treeSearchArea->hide();
    }

    if(appconfig.getInterfaceMode() == "mobile") {
        // wordRegard->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        // howExtract->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        // treeSearchArea->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

        // wordRegard->showMinimized();
        // howExtract->showMinimized();
        // treeSearchArea->showMinimized();

        wordRegard->setMinimumContentsLength(1);
        wordRegard->setMaximumWidth(getCalculateIconSizePx() * 2);
        wordRegard->setMinimumWidth(getCalculateIconSizePx() * 2);
        wordRegard->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

        howExtract->setMinimumContentsLength(1);
        howExtract->setMaximumWidth(getCalculateIconSizePx() * 2);
        howExtract->setMinimumWidth(getCalculateIconSizePx() * 2);
        howExtract->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

        treeSearchArea->setMinimumContentsLength(1);
        treeSearchArea->setMaximumWidth(getCalculateIconSizePx() * 2);
        treeSearchArea->setMinimumWidth(getCalculateIconSizePx() * 2);
        treeSearchArea->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    }
}


// Набор опций поиска в виде выпадающих списков
void FindScreen::assemblyComboOption(void)
{
    toolsAreaComboOption = new QHBoxLayout();
    //    toolsAreaComboOption->addWidget(wordRegard);
    //    toolsAreaComboOption->addWidget(howExtract);
    //    toolsAreaComboOption->addWidget(treeSearchArea);
    //toolsAreaComboOption->addStretch();
}


void FindScreen::setupCloseButton(void)
{
    // Кнопка закрытия виджета
    closeButton = new FlatToolButton(this);
    closeButton->setVisible(true);
    closeButton->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton)); // SP_TitleBarCloseButton SP_DialogCloseButton
    closeButton->setAutoRaise(true);

    if(appconfig.getInterfaceMode() == "desktop") {
        int w = closeButton->geometry().width();
        int h = closeButton->geometry().height();
        int x = imin(w, h) / 2;
        closeButton->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
        closeButton->setMinimumSize(x, x);
        closeButton->setMaximumSize(x, x);
        closeButton->resize(x, x);
    }
}


void FindScreen::assemblyCloseButton(void)
{
    // Вертикальная область с кнопкой закрытия и распоркой
    // чтобы кнопка была вверху
    toolsAreaCloseButton = new QVBoxLayout();
    toolsAreaCloseButton->setContentsMargins(0, 0, 0, 0);
    toolsAreaCloseButton->addWidget(closeButton);
    toolsAreaCloseButton->addStretch();
}


void FindScreen::setupWhereFindLine(void)
{
    whereFindLabel = new QLabel(tr("Find in: "));

    findInPin = new QCheckBox(tr("Pin"));
    findInPin->setChecked(appconfig.get_findscreen_find_in_field("pin"));

    findInName = new QCheckBox(tr("Title"));
    findInName->setChecked(appconfig.get_findscreen_find_in_field("name"));

    findInAuthor = new QCheckBox(tr("Author(s)"));
    findInAuthor->setChecked(appconfig.get_findscreen_find_in_field("author"));

    findInAuthor = new QCheckBox(tr("Author(s)"));
    findInAuthor->setChecked(appconfig.get_findscreen_find_in_field("author"));

    findInHome = new QCheckBox(tr("Home"));
    findInHome->setChecked(appconfig.get_findscreen_find_in_field("home"));

    findInUrl = new QCheckBox(tr("Url"));
    findInUrl->setChecked(appconfig.get_findscreen_find_in_field("url"));

    findInTags = new QCheckBox(tr("Tags"));
    findInTags->setChecked(appconfig.get_findscreen_find_in_field("tags"));

    findInText = new QCheckBox(tr("Text"));
    findInText->setChecked(appconfig.get_findscreen_find_in_field("text"));
}


void FindScreen::assemblyWhereFindLine(void)
{
    whereFindLine = new QHBoxLayout();
    whereFindLine->setEnabled(false);

    if(appconfig.getInterfaceMode() == "desktop")
        whereFindLine->addWidget(whereFindLabel);

    if(appconfig.getInterfaceMode() == "mobile")
        whereFindLabel->hide();

    whereFindLine->addWidget(wordRegard);
    whereFindLine->addWidget(howExtract);

    if(appconfig.getInterfaceMode() == "mobile") whereFindLine->addWidget(treeSearchArea);

    whereFindLine->addWidget(findInPin);
    whereFindLine->addWidget(findInName);
    whereFindLine->addWidget(findInAuthor);
    whereFindLine->addWidget(findInHome);
    whereFindLine->addWidget(findInUrl);
    whereFindLine->addWidget(findInTags);
    whereFindLine->addWidget(findInText);

    whereFindLine->addStretch();

    whereFindLine->setContentsMargins(3, 0, 0, 0); // Устанавливаются границы
}


void FindScreen::setupSignals(void)
{
    // При каждом изменении текста в строке запроса
    connect(_toolbarsearch, &browser::ToolbarSearch::textChanged, this, &FindScreen::enableFindButton);

    // При каждом изменении текста извне может вырабатыватся этот сигнал
    // Он вырабатывается в слоте setFindText()
    connect(this, &FindScreen::textChangedFromAnother, this, &FindScreen::enableFindButton);


    // При нажатии Enter в строке запроса
    connect(_toolbarsearch, &browser::ToolbarSearch::returnPressed, this
    , [this]() {FindScreen::findClicked();}
           );

    // При нажатии кнопки Find
    connect(_findstartbutton, &QPushButton::clicked, this
    , [this]() {FindScreen::findClicked();}
           );

    // При нажатии кнопки разворачивания/сворачивания инструментов поиска
    connect(toolsExpand, &FlatToolButton::clicked, this, &FindScreen::toolsExpandClicked);

    // После установки текста извне, вырабатывается этот сигнал
    connect(this, &FindScreen::findClickedAfterAnotherTextChanged, this
    , [this]() {FindScreen::findClicked();}
           );


    // При нажатии кнопки закрытия
    connect(closeButton, &FlatToolButton::clicked, this, &FindScreen::widgetHide);
    connect(closeButton, &FlatToolButton::clicked, [] {
        auto win = globalparameters.entrance()->activiated_registered().first;

        if(win)win->updateToolbarActionText(false);
    });


    // Сигналы для запоминания состояния интерфейса
    connect(wordRegard, wordRegard->currentindexchanged, this, &FindScreen::changedWordRegard);

    connect(howExtract, howExtract->currentindexchanged, this, &FindScreen::changedHowExtract);

    connect(treeSearchArea, treeSearchArea->currentindexchanged, this, &FindScreen::changedTreeSearchArea);

    //    assert(globalparameters.vtab());
    //    connect(globalparameters.vtab(), &QTabWidget::currentChanged
    //            , this, &FindScreen::changedTreeSearchArea    // , &appconfig, &AppConfig::setFindScreenTreeSearchArea
    //           );

    connect(findInPin, &QCheckBox::stateChanged, this, &FindScreen::changedFindInPin);

    connect(findInName, &QCheckBox::stateChanged, this, &FindScreen::changedFindInName);

    connect(findInAuthor, &QCheckBox::stateChanged, this, &FindScreen::changedFindInAuthor);

    connect(findInHome, &QCheckBox::stateChanged, this, &FindScreen::changedFindInHome);

    connect(findInUrl, &QCheckBox::stateChanged, this, &FindScreen::changedFindInUrl);

    connect(findInTags, &QCheckBox::stateChanged, this, &FindScreen::changedFindInTags);

    connect(findInText, &QCheckBox::stateChanged, this, &FindScreen::changedFindInText);
}


void FindScreen::setupUI(void)
{
    //_findtable = new FindTableWidget();
    //_findtable->hide();
    //_progress = new QProgressDialog(this);
    _progress->hide();
}


void FindScreen::assembly(void)
{
    centralDesktopLayout = new QVBoxLayout(this);

    if(appconfig.getInterfaceMode() == "desktop") {
        toolsLine = new QHBoxLayout();
        toolsLine->setEnabled(true);
        toolsLine->addLayout(toolsAreaFindTextAndButton);
        //toolsLine->addLayout(toolsAreaComboOption);
        toolsLine->addLayout(toolsAreaCloseButton);

        centralDesktopLayout->addLayout(toolsLine);
    }

    if(appconfig.getInterfaceMode() == "mobile") {
        toolsGrid = new QGridLayout();
        toolsGrid->setEnabled(true);
        toolsGrid->addLayout(toolsAreaFindTextAndButton, 0, 0);
        toolsGrid->addLayout(toolsAreaCloseButton,       0, 1);
        //toolsGrid->addLayout(toolsAreaComboOption,       1, 0);

        centralDesktopLayout->addLayout(toolsGrid);
    }

    centralDesktopLayout->addLayout(whereFindLine);
    //    centralDesktopLayout->addWidget(_findtable, 10);
    centralDesktopLayout->setContentsMargins(0, 0, 0, 0); // Boundaries removed // Границы убираются
    centralDesktopLayout->setSizeConstraint(QLayout::   // SetFixedSize // this setting will lead TableScreen can not resize!!!
                                            SetNoConstraint
                                           );
    centralDesktopLayout->setMargin(0);
    centralDesktopLayout->setSpacing(0);

    // whereFindLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    // _findtable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    this->setLayout(centralDesktopLayout);

    switchToolsExpand(appconfig.getFindInBaseExpand());
}

void FindScreen::adjustSize()
{
    //    int height = toolsLine->isEnabled() ? toolsLine->sizeHint().height() : 0 + whereFindLine->isEnabled() ? whereFindLine->sizeHint().height() : 0;
    //    setMinimumHeight(height);
    //    setMaximumHeight(height);
    //    setMinimumWidth(width());
    //    setMaximumWidth(width());
    QWidget::adjustSize();
}

void FindScreen::resizeEvent(QResizeEvent *e)
{
    //    adjustSize();
    //    auto hint = whereFindLine->sizeHint();
    //    int height = whereFindLine->sizeHint().height();
    setMinimumHeight(toolsLine->sizeHint().height() + whereFindLine->sizeHint().height());
    setMaximumHeight(toolsLine->sizeHint().height() + whereFindLine->sizeHint().height());
    QWidget::resizeEvent(e);
}

void FindScreen::enableFindButton(const QString &text)
{
    _findstartbutton->setEnabled(!text.isEmpty());
}


// Слот, с помощью которого другие виджеты могут устанавливать
// текст для поиска
void FindScreen::setFindText(QString text)
{
    //_findtext
    _toolbarsearch->setText(text);

    emit textChangedFromAnother(text);
    emit findClickedAfterAnotherTextChanged();
}


// Слот, срабатывающий при нажатии на кнопку начала поиска
std::shared_ptr<TableData> FindScreen::findClicked(void)
{
    // Поля, где нужно искать (Заголовок, текст, теги...)
    searchArea["pin"]       = findInPin->isChecked();
    searchArea["name"]      = findInName->isChecked();
    searchArea["author"]    = findInAuthor->isChecked();
    searchArea["home"]      = findInHome->isChecked();
    searchArea["url"]       = findInUrl->isChecked();
    searchArea["tags"]      = findInTags->isChecked();
    searchArea["text"]      = findInText->isChecked();

    // Проверяется, установлено ли хоть одно поле для поиска
    int findEnableFlag = 0;

    foreach(bool value, searchArea)
        if(value == true)findEnableFlag = 1;

    // Если не отмечены поля для поиска
    if(findEnableFlag == 0) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Cannot start find process"));
        messageBox.setText(tr("Verify that you selected fields for search for starting find process."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return nullptr;
    }

    // Выясняется список слов, которые нужно искать
    searchWordList = text_delimiter_decompose(_toolbarsearch->text());

    if(searchWordList.size() == 0) {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Can not start find process"));
        messageBox.setText(tr("The search request is too short. Enter at least one word."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        return nullptr;
    }

    return findStart();
}


std::shared_ptr<TableData> FindScreen::findStart(void)
{

    if(globalparameters.vtab()->currentWidget()->objectName() == "recordTableScreen"
       && !find_object<TreeScreen>("treeScreen")->getCurrentItemIndex().isValid()
      ) {
        appconfig.setFindScreenTreeSearchArea(2);
    }

    // Сохраняется текущая редактируемая запись, чтобы и в ней
    // были найдены введенные перед нажатием Find данные, если они есть
    find_object<MainWindow>("mainwindow")->saveTextarea();

    //    // Очищается таблица результата поиска
    //    _findtable->re_initialize();

    // Выясняется ссылка на модель дерева данных
    KnowTreeModel *search_model = static_cast<KnowTreeModel *>(find_object<KnowTreeView>("knowTreeView")->model());


    // Выясняется стартовый элемент в дереве, с которого будет начат поиск
    // Выясняется сколько всего конечных записей
    std::shared_ptr<TreeItem> start_item;
    std::shared_ptr<TableData> source;
    int total_records = 0;


    //__________________________________________________________________________________________________


    auto global_search_prepare = [&](std::shared_ptr<TreeItem> &start_item, int &total_records , std::shared_ptr<TableData> &result) {
        // Корневой элемент дерева
        start_item = search_model->rootItem;    // this change the value of local smart pointer, which can't be return to outer start_item, so function parameter type must be a reference.
        // Количество элементов (веток) во всем дереве
        total_records = search_model->getAllRecordCount();
        result->empty();
    };

    auto branch_search_prepare = [&](std::shared_ptr<TreeItem> &start_item, int &total_records , std::shared_ptr<TableData> &result) {

        // Индекс текущей выбранной ветки
        QModelIndex currentItemIndex = find_object<TreeScreen>("treeScreen")->getCurrentItemIndex();

        // Текущая ветка
        start_item = search_model->getItem(currentItemIndex);

        // Количество элементов (веток) в текущей ветке и всех подветках
        total_records = search_model->getRecordCountForItem(start_item);
        result->empty();
    };

    Q_UNUSED(branch_search_prepare)

    auto result_set_search_prepare = [&](std::shared_ptr<TreeItem> &start_item, int &total_records , std::shared_ptr<TableData> &result, std::shared_ptr<TableData> &source) {
        // to be done
        QMap<QString, QString> data;
        start_item = std::make_shared<TreeItem>(data);

        source.swap(_result);
        QDomDocument doc;
        auto dommodel = source->dom_from_data(&doc);    // source->init(startItem, QDomElement());
        start_item->recordtableInit(dommodel);

        result = std::make_shared<TableData>();        // assert(_result->size() == 0); //_result->empty();
        total_records = source->size();
    };


    auto prepare_progressbar = [&]() {
        // Показывается виджет линейки наполняемости
        _progress->reset();
        _progress->setLabelText(tr("Search..."));
        _progress->setRange(0, total_records);
        _progress->setModal(true);
        _progress->setMinimumDuration(0);
        _progress->show();

        // Обнуляется счетчик обработанных конечных записей
        totalProgressCounter = 0;
        cancelFlag = 0;

    };


    auto assert_start_item = [&]() {
        // Если стартовый элемент не был установлен
        //        if(!startItem)
        //        {
        QMessageBox messageBox(this);
        messageBox.setWindowTitle(tr("Cannot start find process"));
        messageBox.setText(tr("Can\'t set start search element in tree."));
        messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
        messageBox.exec();
        //        return nullptr;
        //        } else
        //            return startItem;
    };


    auto close_progressbar = [&]() {
        // Виджет линейки наполняемости скрывается
        _progress->reset();
        _progress->hide();
        _progress->close();
    };

    auto final_search = [&](std::shared_ptr<TreeItem> &start_item, std::shared_ptr<TableData> &result) {
        qDebug() << "Start finding in " << total_records << " records";
        prepare_progressbar();
        //Вызывается рекурсивный поиск в дереве
        find_recursive(start_item, result);

        //        if(result->size() == 0 && appconfig.getFindScreenTreeSearchArea() == 2) {
        //            global_search_prepare(start_item, total_records, result);
        //        }

    };

    auto output = [&](std::shared_ptr<TableData> &result) {

        // После вставки всех данных подгоняется ширина колонок
        //        _findtable->updateColumnsWidth();
        globalparameters.getTreeScreen()->on_found(result);
    };


    //__________________________________________________________________________________________________




    if( // appconfig.getFindScreenTreeSearchArea() == 2
        //        globalparameters.vtab()->currentWidget()->objectName() == "recordTableScreen"
        //        && !find_object<TreeScreen>("treeScreen")->getCurrentItemIndex().isValid()
        _result->size() > 0
    ) { // search in last search result
        result_set_search_prepare(start_item, total_records, _result, source);

        if(!start_item) {assert_start_item(); return nullptr;}

        if(0 != total_records) {
            final_search(start_item, _result);
        }
    }

    if(0 == _result->size()) {

        //    if( // appconfig.getFindScreenTreeSearchArea() == 1
        //        // globalparameters.vtab()->currentWidget()->objectName() == "treeScreen"
        //        find_object<TreeScreen>("treeScreen")->getCurrentItemIndex().isValid()
        //    ) { // If you want to search the current branch // Если нужен поиск в текущей ветке
        //        branch_search_prepare(start_item, total_records, _result);
        //    }

        //    if( // appconfig.getFindScreenTreeSearchArea() == 0
        //        globalparameters.vtab()->currentWidget()->objectName() == "treeScreen"
        //        && !find_object<TreeScreen>("treeScreen")->getCurrentItemIndex().isValid()
        //    ) { // Если нужен поиск во всем дереве

        global_search_prepare(start_item, total_records, _result);

        if(!start_item) {assert_start_item(); return nullptr;}

        if(0 != total_records) {
            final_search(start_item, _result);
        }

        //    }

    }





    //        close_progressbar();
    //    } else {

    close_progressbar();



    // create a new note and open in browser
    // if it is a web address, open it
    // else, open from search engine
    //    }

    output(_result);


    return _result;
}


void FindScreen::find_recursive(std::shared_ptr<TreeItem> curritem, std::shared_ptr<TableData>  result)
{
    // Если была нажата отмена поиска
    if(cancelFlag == 1)return;

    // Если ветка зашифрована, и пароль не был введен
    if(curritem->getField("crypt") == "1" &&
       globalparameters.getCryptKey().length() == 0)
        return;

    // Если в ветке присутсвует таблица конечных записей
    if(curritem->recordtableGetRowCount() > 0) {
        // Обработка таблицы конечных записей

        // Выясняется ссылка на таблицу конечных записей
        std::shared_ptr<TableData> searchRecordTable = curritem->recordtableGetTableData();

        // Перебираются записи таблицы
        for(int i = 0; i < searchRecordTable->size(); i++) {
            // Обновляется линейка наполняемости
            _progress->setValue(++totalProgressCounter);
            qApp->processEvents();

            if(_progress->wasCanceled()) {
                cancelFlag = 1;
                return;
            }

            // Результаты поиска в полях
            QMap<QString, bool> iteration_search_result;

            iteration_search_result["pin"]      = false;
            iteration_search_result["name"]     = false;
            iteration_search_result["author"]   = false;
            iteration_search_result["home"]     = false;
            iteration_search_result["url"]      = false;
            iteration_search_result["tags"]     = false;
            iteration_search_result["text"]     = false;

            // Текст в котором будет проводиться поиск
            QString inspectText;

            // Цикл поиска в отмеченных пользователем полях
            QMapIterator<QString, bool> j(iteration_search_result);

            while(j.hasNext()) {
                j.next();
                QString key = j.key();

                // Если в данном поле нужно проводить поиск
                if(searchArea[key] == true) {
                    if(key != "text") {
                        // Поиск в обычном поле
                        inspectText = searchRecordTable->field(key, i);
                        iteration_search_result[key] = findInTextProcess(inspectText);
                    } else {
                        // Поиск в тексте записи
                        inspectText = searchRecordTable->text(i);
                        QTextDocument textdoc;
                        textdoc.setHtml(inspectText);
                        iteration_search_result[key] = findInTextProcess(textdoc.toPlainText());
                    }
                }
            } // Закрылся цикл поиска в полях


            // Проверяется, есть ли поле, в котором поиск был успешен
            int findFlag = 0;

            foreach(bool value, iteration_search_result)
                if(value == true)findFlag = 1;

            // Если запись найдена
            if(findFlag == 1) {
                qDebug() << "Find succesfull in " << searchRecordTable->field("name", i);

                // QString pin0 = curritem->getField("pin");
                // QString pin1 = searchRecordTable->getField("pin", i);   // work

                //                // В таблицу результатов поиска добавляются данные
                //                // Имя записи
                //                // Имя ветки
                //                // Теги
                //                // Путь к ветке
                //                // ID записи в таблице конечных записей
                //                _findtable->addRow(searchRecordTable->field("name", i)
                //                                   , searchRecordTable->field("pin", i)  // curritem->getField("pin")
                //                                   , curritem->getField("name")
                //                                   , searchRecordTable->field("tags", i)
                //                                   , curritem->getPath()
                //                                   , searchRecordTable->field("id", i)
                //                                  );
                result->insert_new_record(ADD_NEW_RECORD_TO_END, result->work_pos(), searchRecordTable->record_fat(i));
            }

        } // Закрылся цикл перебора записей в таблице конечных записей
    } // Закрылось условие что в ветке есть таблица конечных записей


    // Рекурсивная обработка каждой подчиненной ветки
    for(int i = 0; i < curritem->childCount(); i++)
        find_recursive(curritem->child(i), result);

}


// Поиск в переданном тексте
// Учитываются состояния переключателей wordregard и howextract
bool FindScreen::findInTextProcess(const QString &text)
{
    int findWordCount = 0;
    int findFlag = 0;

    // Перебираются искомые слова
    for(int i = 0; i < searchWordList.size(); ++i) {
        findFlag = 0;

        // Если надо найти совпадение целого слова
        if(howExtract->currentIndex() == 0) {
            // Текст разбивается на слова с очисткой от лишних знаков
            // и проверяется, есть ли в полученном списке текущее слово
            if(text_delimiter_decompose(text).contains(searchWordList.at(i), Qt::CaseInsensitive))
                findFlag = 1;
        } else {
            // Если надо найти слово как подстроку
            if(text.contains(searchWordList.at(i), Qt::CaseInsensitive))
                findFlag = 1;
        }

        // Если слово было найдено, количество найденных слов увеличивается
        if(findFlag == 1)findWordCount++;

        // Если ищется хотя бы одно совпадение
        if(findFlag == 1 && wordRegard->currentIndex() == 0)
            return true; // То при первом же совпадении цикл прекращается
    }

    // Искалось хотябы одно совпадение, но небыло найдено
    if(wordRegard->currentIndex() == 0) return false;
    else {
        // Иначе требовалось найти все слова в запросе
        if(findWordCount == searchWordList.size())
            return true;
        else
            return false;
    }

}


void FindScreen::changedWordRegard(int pos)
{
    appconfig.set_findscreen_wordregard(pos);
}


void FindScreen::changedHowExtract(int pos)
{
    appconfig.set_findscreen_howextract(pos);
}


void FindScreen::changedTreeSearchArea(int pos)
{
    appconfig.setFindScreenTreeSearchArea(pos);
}

void FindScreen::changedFindInPin(int state)
{
    changedFindInField("pin", state);
}

void FindScreen::changedFindInName(int state)
{
    changedFindInField("name", state);
}


void FindScreen::changedFindInAuthor(int state)
{
    changedFindInField("author", state);
}

void FindScreen::changedFindInHome(int state)
{
    changedFindInField("home", state);
}

void FindScreen::changedFindInUrl(int state)
{
    changedFindInField("url", state);
}


void FindScreen::changedFindInTags(int state)
{
    changedFindInField("tags", state);
}


void FindScreen::changedFindInText(int state)
{
    changedFindInField("text", state);
}


void FindScreen::changedFindInField(QString fieldname, int state)
{
    bool i;

    if(state == Qt::Checked) i = true;
    else i = false;

    appconfig.set_findscreen_find_in_field(fieldname, i);
}


void FindScreen::widgetShow(void)
{
    appconfig.set_findscreen_show(true);
    this->show();

    // При появлении виджета курсор должен сразу стоять на поле ввода
    //_findtext
    _toolbarsearch->setFocus();
}


// Полное сокрытие виджета
void FindScreen::widgetHide(void)
{
    // Запоминается размер сплиттера перед скрытием виджета
    QSplitter *findSplitterRel = find_object<QSplitter>("find_splitter");
    appconfig.findsplitter_sizelist(findSplitterRel->sizes());

    // Виджет скрывается
    appconfig.set_findscreen_show(false);
    this->close();
}


// Слот, срабатывающий при клике на кнопку expand
void FindScreen::toolsExpandClicked(void)
{
    // Если нужно сомкнуть инструменты
    if(findInName->isVisible()) {
        switchToolsExpand(false);
        appconfig.setFindInBaseExpand(false);
    } else {
        switchToolsExpand(true);
        appconfig.setFindInBaseExpand(true);
    }
}


void FindScreen::switchToolsExpand(bool flag)
{
    // toolsAreaComboOption->setVisible(flag);
    // whereFindLine->setVisible(flag);

    // Выпадающие списки скрываются в мобильном интерфейсе, так как они на отдельной строке
    if(appconfig.getInterfaceMode() == "mobile") {
        findInPin->setVisible(flag);
        wordRegard->setVisible(flag);
        howExtract->setVisible(flag);
        treeSearchArea->setVisible(flag);
    }

    // Надпись Find in видна и управляется только в desktop режиме интерфейса
    if(appconfig.getInterfaceMode() == "desktop") {
        whereFindLabel->setVisible(flag);
    }

    // Флаги поиска скрываются для любого интерфейса, так как они всегда находятся на отдельной строке
    wordRegard->setVisible(flag);
    howExtract->setVisible(flag);
    //    treeSearchArea->setVisible(flag);
    findInPin->setVisible(flag);
    findInName->setVisible(flag);
    findInAuthor->setVisible(flag);
    findInHome->setVisible(flag);
    findInUrl->setVisible(flag);
    findInTags->setVisible(flag);
    findInText->setVisible(flag);

    whereFindLine->setEnabled(flag);
    this->adjustSize();
}

// dangerous!
void FindScreen::remove_id(const QString &id)
{
    _result->delete_record_by_id(id);     // _findtable->remove_id(id);
}

// dangerous!
void FindScreen::remove_row(const int row)
{
    _result->delete_record(row);     // _findtable->remove_row(row);
}
