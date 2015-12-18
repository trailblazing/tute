#include <QString>
#include <QDir>
#include <QString>
#include <QDesktopWidget>
#include <QStackedWidget>

#include "main.h"
#include "models/appConfig/AppConfig.h"
#include "views/browser/entrance.h"
#include "MainWindow.h"
#include "views/printPreview/PrintPreview.h"
#include "views/appConfigWindow/AppConfigDialog.h"
#include "libraries/TrashMonitoring.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/recordTable/RecordTableScreen.h"
#include "models/tree/TreeItem.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "models/tree/KnowTreeModel.h"
#include "libraries/GlobalParameters.h"
#include "views/consoleEmulator/ExecuteCommand.h"
#include "libraries/WalkHistory.h"
#include "libraries/WindowSwitcher.h"

#include "libraries/wyedit/EditorTextArea.h"
#include "views/browser/browser.h"
#include "libraries/qtSingleApplication5/qtsingleapplication.h"

extern AppConfig appconfig;
extern TrashMonitoring trashmonitoring;
extern GlobalParameters globalparameters;
extern WalkHistory walkhistory;


MainWindow::MainWindow(
    GlobalParameters &globalparameters
    , const AppConfig &appconfig
    , const DataBaseConfig &databaseconfig)
    : QMainWindow()
    , _globalparameters(globalparameters)
    , _appconfig(appconfig)
    , _databaseconfig(databaseconfig)
    , _recordtablecontroller(nullptr)
    , v_right_splitter(new QSplitter(Qt::Vertical))
    , find_splitter(new QSplitter(Qt::Vertical))
    , _vtabwidget(new HidableTabWidget(this))
    , v_left_splitter(new QSplitter(Qt::Horizontal))    // Qt::Vertical
    , hSplitter(new QSplitter(Qt::Horizontal))
    , _filemenu(new QMenu(tr("&File"), this))
    , _toolsmenu(new QMenu(tr("&Tools"), this))
    , _helpmenu(new QMenu(tr("&Help"), this))
{
    _globalparameters.mainwindow(this);

    extern QObject *mainwindow;
    mainwindow = this;
    setObjectName("mainwindow");

    installEventFilter(this);

    initFileMenu();
    initToolsMenu();
    //    initHelpMenu();
    QMainWindow::menuBar()->hide();

    setupUI();
    setupSignals();
    assembly();

    //    initFileMenu();
    //    initToolsMenu();
    //    initHelpMenu();

    setupIconActions();
    createTrayIcon();
    setIcon();

    if(QSystemTrayIcon::isSystemTrayAvailable())
        trayIcon->show();

    // Инициализируется объект слежения за корзиной
    trashmonitoring.init(_appconfig.get_trashdir());
    trashmonitoring.update();

    // Закрывать ли по-настоящему окно при обнаружении сигнала closeEvent
    enableRealClose = false;

    // Инициализация генератора случайных чисел
    init_random();


}


MainWindow::~MainWindow()
{
    saveAllState();

    delete  browser_entrance;
    delete  windowSwitcher;
    delete  statusBar;
    delete  editorScreen;
    delete  findScreenDisp;
    delete  recordTableScreen;
    delete  treeScreen;
    delete  _vtabwidget;

    delete  v_right_splitter;
    delete  find_splitter;
    delete  _vtabwidget;
    delete  v_left_splitter;
    delete  hSplitter;

    delete  _filemenu;
    delete  _toolsmenu;
    delete  _helpmenu;

}


void MainWindow::setupUI(void)
{
    // При создании объектов не указывается parent, так как он буден задан в момент вставки в layout в методе assembly()

    treeScreen = new TreeScreen(_appconfig, this);
    treeScreen->setObjectName("treeScreen");
    _globalparameters.setTreeScreen(treeScreen);
    //    _treetable_hidden = treeScreen->isHidden();
    //    connect(treeScreen, &TreeScreen::hide, this, [this]() {_treetable_hidden = true;});
    //    connect(treeScreen, &TreeScreen::show, this, [this]() {_treetable_hidden = false;});

    recordTableScreen = new RecordTableScreen(this);
    recordTableScreen->setObjectName("recordTableScreen");
    _globalparameters.setRecordTableScreen(recordTableScreen);
    //    _recordtable_hidden = recordTableScreen->isHidden();
    //    connect(recordTableScreen, &RecordTableScreen::hide, this, [this]() {_recordtable_hidden = true;});
    //    connect(recordTableScreen, &RecordTableScreen::show, this, [this]() {_recordtable_hidden = false;});

    findScreenDisp = new FindScreen(this);
    findScreenDisp->setObjectName("findScreenDisp");
    _globalparameters.setFindScreen(findScreenDisp);
    //findScreenDisp->hide();

    editorScreen = new MetaEditor();
    editorScreen->setObjectName("editorScreen");
    _globalparameters.setMetaEditor(editorScreen);

    if(!_appconfig.get_editor_show())
        editorScreen->hide();

    statusBar = new QStatusBar(this);
    statusBar->setObjectName("statusBar");
    setStatusBar(statusBar);
    _globalparameters.setStatusBar(statusBar);

    // Вспомогательный объект переключения окон, используется в мобильном интерфейсе
    windowSwitcher = new WindowSwitcher(this);
    windowSwitcher->setObjectName("windowSwitcher");
    _globalparameters.setWindowSwitcher(windowSwitcher);
    //    windowSwitcher->findInBaseClick();

    if(recordTableScreen) {
        browser_entrance = new browser::Entrance(
            _recordtablecontroller
            = recordTableScreen->getRecordTableController()
              , _globalparameters.style_source()
            , this
            , Qt::Widget  // Qt::MaximizeUsingFullscreenGeometryHint
        );
        //    browsermanager->adjustSize();
        browser_entrance->setScrollbars(true);
        browser_entrance->setObjectName("entrance");
        _globalparameters.entrance(browser_entrance);
    }

    // todo: Для проверки, почему то в этом месте поиск объекта по имени не работает, разобраться.
    // MetaEditor *edView=find_object<MetaEditor>("editorScreen");
}


void MainWindow::setupSignals(void)
{
    connect(editorScreen, &MetaEditor::send_expand_edit_area, this, &MainWindow::onExpandEditArea);

    // Сигнал, генерирующийся при выходе из оконных систем X11 и Windows
    connect(qApp, SIGNAL(commitDataRequest(QSessionManager &)), this, SLOT(commitData(QSessionManager &)));

    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(onFocusChanged(QWidget *, QWidget *)));

    // Связывание сигналов кнопки поиска по базе с действием по открытию виджета поиска по базе
    connect(treeScreen->_actionlist["findInBase"], SIGNAL(triggered()), globalparameters.getWindowSwitcher(), SLOT(findInBaseClick()));
    connect(browser_entrance->getactionFreeze(), SIGNAL(triggered()), globalparameters.getWindowSwitcher(), SLOT(findInBaseClick()));
    connect(recordTableScreen->actionFindInBase, SIGNAL(triggered()), globalparameters.getWindowSwitcher(), SLOT(findInBaseClick()));
    connect(editorScreen, SIGNAL(wyeditFindInBaseClicked()), globalparameters.getWindowSwitcher(), SLOT(findInBaseClick()));
}


void MainWindow::assembly(void)
{
    //    v_right_splitter = new QSplitter(Qt::Vertical);
    v_right_splitter->addWidget(browser_entrance);
    v_right_splitter->addWidget(editorScreen);             // Text entries // Текст записи
    v_right_splitter->setCollapsible(0
                                     , false        // if true, make editor can overload it
                                    );              // The list of final entries can not link up    // Список конечных записей не может смыкаться
    v_right_splitter->setCollapsible(1, false);            // The contents of the recording can not link up    // Содержимое записи не может смыкаться
    v_right_splitter->setObjectName("v_right_splitter");

    //    find_splitter = new QSplitter(Qt::Vertical);
    find_splitter->addWidget(v_right_splitter);             //findSplitter->addWidget(hSplitter);
    find_splitter->addWidget(findScreenDisp);
    find_splitter->setCollapsible(0, false);         // Верхняя часть не должна смыкаться
    find_splitter->setCollapsible(1, false);         // Часть для поиска не должна смыкаться
    find_splitter->setObjectName("find_splitter");

    //    _qtabwidget = new QTabWidget(this);

    _vtabwidget->setTabPosition(QTabWidget::West);
    _vtabwidget->addTab(treeScreen, QIcon(":/resource/pic/branch.svg"), "Tree");
    _vtabwidget->addTab(recordTableScreen, QIcon(":/resource/pic/maple.svg"), "Record");
    globalparameters.vtab(_vtabwidget);

    //    v_left_splitter = new QSplitter(
    //        Qt::Horizontal  // Qt::Vertical
    //    );
    v_left_splitter->addWidget(_vtabwidget);
    //    v_left_splitter->addWidget(treeScreen);
    //    v_left_splitter->addWidget(recordTableScreen);
    v_left_splitter->setCollapsible(0, false);
    //    v_left_splitter->setCollapsible(1, false);
    v_left_splitter->setObjectName("v_left_splitter");

    //    hSplitter = new QSplitter(Qt::Horizontal);
    hSplitter->addWidget(
        v_left_splitter
    );
    //hSplitter->addWidget(treeScreen);             // Tree branches    // Дерево веток
    //hSplitter->addWidget(recordTableScreen);      // The list of final entries    // Список конечных записей
    hSplitter->addWidget(find_splitter);             //hSplitter->addWidget(vSplitter);
    hSplitter->setCollapsible(0, false);            // Дерево веток не может смыкаться
    hSplitter->setCollapsible(1, false);            // Столбец со списком и содержимым записи не может смыкаться
    hSplitter->setObjectName("hsplitter");

    //    connect(find_splitter, &QSplitter::splitterMoved, browser_entrance, &browser::Entrance::on_splitter_moved);
    //    connect(find_splitter, &QSplitter::splitterMoved, recordTableScreen, &RecordTableScreen::on_splitter_moved);

    //    findSplitter=new QSplitter(Qt::Vertical);
    //    findSplitter->addWidget(hSplitter);
    //    findSplitter->addWidget(findScreenDisp);
    //    findSplitter->setCollapsible(0,false);        // Верхняя часть не должна смыкаться
    //    findSplitter->setCollapsible(1,false);        // Часть для поиска не должна смыкаться
    //    findSplitter->setObjectName("find_splitter");

    setCentralWidget(hSplitter);                    //setCentralWidget(findSplitter);

}


void MainWindow::saveAllState(void)
{
    // Сохранение данных в поле редактирования
    saveTextarea();

    // Сохраняются данные сеанса работы
    saveGeometry();
    save_tree_position();
    save_recordtable_position();
    saveEditorCursorPosition();
    saveEditorScrollBarPosition();

    // Синхронизируется с диском конфиг программы
    appconfig.sync();
}


// Слот, срабатывающий когда происходит выход из оконной системы
void MainWindow::commitData(QSessionManager &manager)
{
    Q_UNUSED(manager);
    qDebug() << "Session manager send commit data signal.";

    applicationFastExit();
}


// Восстанавливается геометрия окна и позиции основных разделителей
void MainWindow::restoreGeometry(void)
{
    if(globalparameters.getTargetOs() == "android")
        setWindowState(Qt::WindowMaximized); // Для Андроида окно просто разворачивается на весь экран
    else {
        QRect rect = appconfig.mainwin_geometry();
        resize(rect.size());
        move(rect.topLeft());
    }

    // move(rect.topLeft());
    // resize(rect.size());

    v_right_splitter->setSizes(appconfig.vspl_sizelist());
    hSplitter->setSizes(appconfig.hspl_sizelist());
    v_left_splitter->setSizes(appconfig.v_leftsplitter_sizelist());
    find_splitter->setSizes(appconfig.findsplitter_sizelist());
}


// Запоминается геометрия окна и позиции основных разделителей
void MainWindow::saveGeometry(void)
{
    qDebug() << "Save window geometry and splitter sizes";

    QRect geom(pos(), size());

    appconfig.mainwin_geometry(geom.x(), geom.y(),
                               geom.width(), geom.height());

    // mytetraconfig.set_mainwingeometry(geometry().x(), geometry().y(),
    //                                   geometry().width(), geometry().height());

    appconfig.vspl_sizelist(v_right_splitter->sizes());
    appconfig.hspl_sizelist(hSplitter->sizes());
    appconfig.v_leftsplitter_sizelist(v_left_splitter->sizes());

    // Запоминается размер сплиттера только при видимом виджете поиска,
    // т.к. если виджета поиска невидно, будет запомнен нуливой размер

    // if(findScreenDisp->isVisible()) - так делать нельзя, т.к.
    // данный метод вызывается из декструктора главного окна, и к этому моменту
    // виджет уже невиден

    if(appconfig.get_findscreen_show())
        appconfig.findsplitter_sizelist(find_splitter->sizes());
}


void MainWindow::restore_tree_position(void)
{
    // Путь к последнему выбранному в дереве элементу
    QStringList path = appconfig.get_tree_position();

    qDebug() << "MainWindow::restoreTreePosition() : " << path;

    set_tree_position(path);
}


void MainWindow::save_tree_position(void)
{
    // Получение QModelIndex выделенного в дереве элемента
    QModelIndex index = treeScreen->getCurrentItemIndex();

    // Получаем указатель вида TreeItem
    TreeItem *item = treeScreen->_knowtreemodel->getItem(index);

    // Сохраняем путь к элементу item
    appconfig.set_tree_position(item->getPath());
}


void MainWindow::set_tree_position(QStringList path)
{
    if(treeScreen->_knowtreemodel->isItemValid(path) == false)
        return;

    // Получаем указатель на элемент вида TreeItem, используя путь
    TreeItem *item = treeScreen->_knowtreemodel->getItem(path);

    qDebug() << "Set tree position to " << item->getField("name") << " id " << item->getField("id");

    // Из указателя на элемент TreeItem получаем QModelIndex
    QModelIndex setto = treeScreen->_knowtreemodel->getIndexByItem(item);

    // Курсор устанавливается в нужную позицию
    treeScreen->setCursorToIndex(setto);
}


bool MainWindow::isTreePositionCrypt()
{
    QStringList path = appconfig.get_tree_position();

    if(treeScreen->_knowtreemodel->isItemValid(path) == false) return false;

    // Получаем указатель на элемент вида TreeItem, используя путь
    TreeItem *item = treeScreen->_knowtreemodel->getItem(path);

    if(item->getField("crypt") == "1")
        return true;
    else
        return false;
}


void MainWindow::restore_recordtable_position(void)
{
    QString id = appconfig.get_recordtable_selected_record_id();

    if(id.length() > 0)
        select_id(id);
}


void MainWindow::save_recordtable_position(void)
{
    QString id = recordTableScreen->getFirstSelectionId();

    appconfig.set_recordtable_selected_record_id(id);
}


void MainWindow::select_id(QString id)
{
    recordTableScreen->select_id(id);
}


void MainWindow::saveEditorCursorPosition(void)
{
    int n = editorScreen->getCursorPosition();

    appconfig.setEditorCursorPosition(n);
}


void MainWindow::restoreEditorCursorPosition(void)
{
    int n = appconfig.getEditorCursorPosition();

    editorScreen->setCursorPosition(n);
}


void MainWindow::saveEditorScrollBarPosition(void)
{
    int n = editorScreen->getScrollBarPosition();

    appconfig.setEditorScrollBarPosition(n);
}


void MainWindow::restoreEditorScrollBarPosition(void)
{
    int n = appconfig.getEditorScrollBarPosition();

    editorScreen->setScrollBarPosition(n);
}


void MainWindow::restoreFindOnBaseVisible(void)
{
    bool n = appconfig.get_findscreen_show();

    // Определяется ссылка на виджет поиска
    FindScreen *findScreenRel = find_object<FindScreen>("findScreenDisp");

    if(n)
        findScreenRel->show();
    else
        findScreenRel->hide();
}


// Создание раздела меню File
void MainWindow::initFileMenu(void)
{
    // Создание меню
    //    _filemenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(_filemenu);

    // Создание тулбара
    /*
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle(tr("File Actions"));
    addToolBar(tb);
    */

    QAction *a;

    /*
    a = new QAction(tr("&New"), this);
    a->setShortcut(QKeySequence::New);
    connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
    // tb->addAction(a);
    menu->addAction(a);

    a = new QAction(tr("&Open..."), this);
    a->setShortcut(QKeySequence::Open);
    connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
    // tb->addAction(a);
    menu->addAction(a);

    menu->addSeparator();

    a = new QAction(tr("&Save"), this);
    a->setShortcut(QKeySequence::Save);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
    a->setEnabled(false);
    // tb->addAction(a);
    menu->addAction(a);

    a = new QAction(tr("Save &As..."), this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    menu->addAction(a);
    menu->addSeparator();
    */

    a = new QAction(tr("&Print..."), this);
    a->setShortcut(QKeySequence::Print);
    connect(a, &QAction::triggered, this, &MainWindow::filePrint);
    // tb->addAction(a);
    _filemenu->addAction(a);

    a = new QAction(tr("Print Preview..."), this);
    connect(a, &QAction::triggered, this, &MainWindow::filePrintPreview);
    _filemenu->addAction(a);

    a = new QAction(tr("&Export PDF..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(a, &QAction::triggered, this, &MainWindow::filePrintPdf);
    // tb->addAction(a);
    _filemenu->addAction(a);

    _filemenu->addSeparator();

    //    a = new QAction(tr("&Quit"), this);
    //    a->setShortcut(Qt::CTRL + Qt::Key_Q);
    //    connect(a, SIGNAL(triggered()), this, SLOT(applicationExit()));
    //    _filemenu->addAction(a);

}


// Создание раздела меню Tools
void MainWindow::initToolsMenu(void)
{
    // Создание меню
    //_toolsmenu = new QMenu(tr("&Tools"), this);
    menuBar()->addMenu(_toolsmenu);

    QAction *a;

    a = new QAction(tr("Find in ba&se"), this);
    connect(a, &QAction::triggered, this, &MainWindow::toolsFind);
    _toolsmenu->addAction(a);

    auto b = new QAction(tr("Editor"), this);
    connect(b, &QAction::triggered, this, &MainWindow::editor_switch);
    _toolsmenu->addAction(b);

    _toolsmenu->addSeparator();


    if(appconfig.getInterfaceMode() == "desktop") {
        a = new QAction(tr("Main &preferences"), this);
        connect(a, &QAction::triggered, this, &MainWindow::toolsPreferences);
        _toolsmenu->addAction(a);
    } else {
        // Создание подменю
        QMenu *menu = new QMenu(tr("Main &preferences"), this);
        initPreferencesMenu(menu);
    }
}


// Заполнение подраздела меню Preferences
void MainWindow::initPreferencesMenu(QMenu *menu)
{
    QAction *a;

    a = new QAction(tr("Main"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsFind()));
    menu->addAction(a);

    a = new QAction(tr("Crypt"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
    menu->addAction(a);

    a = new QAction(tr("Synchro"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
    menu->addAction(a);

    a = new QAction(tr("RecordTable"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
    menu->addAction(a);

    a = new QAction(tr("Misc"), this);
    // connect(a, SIGNAL(triggered()), this, SLOT(toolsPreferences()));
    menu->addAction(a);
}


// Создание раздела меню Help
void MainWindow::initHelpMenu(void)
{
    // Создание меню
    //_helpmenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(_helpmenu);

    QAction *a;

    a = new QAction(tr("About MyTetra"), this);
    connect(a, &QAction::triggered, this, &MainWindow::onClickHelpAboutMyTetra);
    _helpmenu->addAction(a);

    a = new QAction(tr("About Qt"), this);
    connect(a, &QAction::triggered, this, &MainWindow::onClickHelpAboutQt);
    _helpmenu->addAction(a);
}


// Новая коллекция
void MainWindow::fileNew(void)
{

}

// Открыть коллекцию
void MainWindow::fileOpen(void)
{

}

// Сохранить текущую статью
bool MainWindow::fileSave(void)
{
    return true;
}

// Сохранить текущую статью как файл
bool MainWindow::fileSaveAs(void)
{
    return true;
}

// Напечатать текущую статью
void MainWindow::filePrint(void)
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Document"));

    if(dlg->exec() == QDialog::Accepted)
        editorScreen->get_textarea_document()->print(&printer);

    delete dlg;
#endif
}


// Предпросмотр печати текущей статьи
void MainWindow::filePrintPreview(void)
{
    PrintPreview *preview = new PrintPreview(editorScreen->get_textarea_document(), this);

    preview->setModal(true);
    preview->setAttribute(Qt::WA_DeleteOnClose);
    preview->show();
}


// Вывод текущей статьи в PDF файл
void MainWindow::filePrintPdf(void)
{
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                    QString(), "*.pdf");

    if(!fileName.isEmpty()) {
        if(QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        editorScreen->get_textarea_document()->print(&printer);
    }

#endif
}


// Слот - Нормальный выход из программы
void MainWindow::applicationExit(void)
{
    saveAllState();

    // Если в конфиге настроено, что нужно синхронизироваться при выходе
    // И задана команда синхронизации
    if(appconfig.get_synchroonexit())
        if(appconfig.get_synchrocommand().trimmed().length() > 0)
            synchronization();

    // Запуск выхода из программы
    enableRealClose = true;
    emit close();
}


// Быстрый выход из программы, без возможности синхронизации
void MainWindow::applicationFastExit(void)
{
    saveAllState();

    // Запуск выхода из программы
    enableRealClose = true;
    emit close();
}


void MainWindow::toolsFind(void)
{
    // Определяется ссылка на виджет поиска
    FindScreen *findScreenRel = find_object<FindScreen>("findScreenDisp");

    if(!(findScreenRel->isVisible()))
        findScreenRel->show();
    else
        findScreenRel->hide();
}

void MainWindow::editor_switch(void)
{

    MetaEditor *editorScreen = find_object<MetaEditor>("editorScreen");

    if(!(editorScreen->isVisible())) {
        editorScreen->show();
        appconfig.set_editor_show(true);
    } else {
        editorScreen->hide();
        appconfig.set_editor_show(false);
    }
}

void MainWindow::toolsPreferences(void)
{
    // Создается окно настроек, после выхода из этой функции окно удалится
    AppConfigDialog dialog("");
    dialog.show();
}


// Слот, срабатывающий при нажатии в редакторе кнопки "распахнуть"
// Флаг показывает, что нужно сделать
// true - распахнуть область отводимую редактору
// false - сделать область, отводимую редактору, обычной
void MainWindow::onExpandEditArea(bool flag)
{
    //    static QSize entrance_size = globalparameters.entrance()->size();
    //    static QSize tree_size = globalparameters.entrance()->size();
    //    static QSize recordtable_size = globalparameters.entrance()->size();
    //    static QSize vtab_size = globalparameters.entrance()->size();

    //    static bool _treetable_hidden;     // = globalparameters.getTreeScreen()->isHidden();
    //    static bool recordtable_hidden; // = globalparameters.getRecordTableScreen()->isHidden();

    if(flag) {
        globalparameters.entrance()->hide();    // resize(QSize(0, 0)); //

        //        if(!globalparameters.getTreeScreen()->isHidden()) {
        //            _treetable_hidden = false;
        //            //            globalparameters.getTreeScreen()->resize(QSize(0, tree_size.height()));
        //            globalparameters.getTreeScreen()->hide();    // resize(QSize(0, 0)); //
        //        } else {
        //            _treetable_hidden = true;
        //        }

        //        if(!globalparameters.getRecordTableScreen()->isHidden()) {
        //            _recordtable_hidden = false;
        //            //            globalparameters.getRecordTableScreen()->resize(QSize(0, recordtable_size.height()));
        //            globalparameters.getRecordTableScreen()->hide(); // resize(QSize(0, 0)); //
        //        } else {
        //            _recordtable_hidden = true;
        //        }

        emit _vtabwidget->hideAction.setChecked(true);
        //        QTabWidget *tab = globalparameters.vtab();
        //        tab->currentWidget()->hide();

        //        auto * tab_pane = tab->findChild<QStackedWidget *>();
        //        tab_pane->hide();

        //        QRect rect = tab->geometry();
        //        tab->setGeometry(rect.left(), rect.top(), 0, rect.height());

        //                setGeometry(rect.top(), rect.left(), 0, rect.height());

        //                resize(QSize(0, v_left_splitter->height())); // hide();
    } else {
        globalparameters.entrance()->show();    // resize(entrance_size); //

        //        if(!_treetable_hidden) {
        //            globalparameters.getTreeScreen()->show();    // resize(tree_size); //
        //        }

        //        if(!_recordtable_hidden) {
        //            globalparameters.getRecordTableScreen()->show(); // resize(recordtable_size); //
        //        }

        //        emit _vtabwidget->hideAction.toggle();
        emit _vtabwidget->hideAction.setChecked(false);
        //        globalparameters.vtab()->resize(vtab_size); // show();
    }
}


void MainWindow::onClickHelpAboutMyTetra(void)
{
    QString version = QString::number(APPLICATION_RELEASE_VERSION) + "." + QString::number(APPLICATION_RELEASE_SUBVERSION) + "." + QString::number(APPLICATION_RELEASE_MICROVERSION);

    QString infoProgramName;
    QString infoVersion;
    QString infoAuthor;
    QString infoEmail;
    QString infoLicense;
    QString infoTargetOs;
    QString infoProgramFile;
    QString infoWorkDirectory;
    QString infoDevicePixelRatio;
    QString infoPhysicalDpi;
    QString infoPhysicalDpiX;
    QString infoPhysicalDpiY;

    infoProgramName = "<b>MyTetra</b> - smart manager<br/>for information collecting<br/><br/>";
    infoVersion = "v." + version + "<br/><br/>";
    infoAuthor = "Author: Sergey M. Stepanov<br/>";
    infoEmail = "Author Email:<i>xintrea@gmail.com</i><br/><br/>";
    infoLicense = "GNU General Public License v.3.0<br/><br/>";
    infoTargetOs = "Target OS: " + globalparameters.getTargetOs() + "<br/>";
    infoProgramFile = "Program file: " + globalparameters.getMainProgramFile() + "<br/>";
    infoWorkDirectory = "Work directory: " + globalparameters.getWorkDirectory() + "<br/>";

#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
    infoDevicePixelRatio = "Device pixel ratio: " + (QString::number(qApp->devicePixelRatio(), 'f', 8)) + "<br/>";
    infoPhysicalDpi = "Physical DPI (from screen): " + (QString::number(QApplication::screens().at(0)->physicalDotsPerInch(), 'f', 8)) + "<br/>";
#endif

    infoPhysicalDpiX = "Physical DPI X (from desktop): " + (QString::number(qApp->desktop()->physicalDpiX(), 'f', 8)) + "<br/>";
    infoPhysicalDpiY = "Physical DPI Y (from desktop): " + (QString::number(qApp->desktop()->physicalDpiY(), 'f', 8)) + "<br/>";

    QString info = infoProgramName +
                   infoVersion +
                   infoAuthor +
                   infoEmail +
                   infoLicense +
                   infoTargetOs +
                   infoProgramFile +
                   infoWorkDirectory +
                   infoDevicePixelRatio +
                   infoPhysicalDpi +
                   infoPhysicalDpiX +
                   infoPhysicalDpiY;

    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->about(this,
                  "MyTetra v." + version,
                  info);
}


void MainWindow::onClickHelpAboutQt(void)
{
    QMessageBox *msgBox = new QMessageBox(this);
    msgBox->aboutQt(this);
}


void MainWindow::synchronization(void)
{
    // Сохраняются данные в поле редактирования
    saveTextarea();

    // Сохраняются данные о курсорах в дереве и таблице конечных записей
    save_tree_position();
    save_recordtable_position();
    saveEditorCursorPosition();
    saveEditorScrollBarPosition();

    // Считывается команда синхронизации
    QString command = appconfig.get_synchrocommand();

    // Если команда синхронизации пуста
    if(command.trimmed().length() == 0) {
        QMessageBox::warning(this,
                             tr("MyTetra: can't synchronization"),
                             tr("Do not set synchronization command.<br>Check the setting in \"Syncro\" section in \"Tools\" menu"),
                             QMessageBox::Close);
        return;
    }


    // Макрос %a заменяется на путь к директории базы данных
    // QString databasePath=globalParameters.getWorkDirectory()+"/"+mytetraConfig.get_tetradir();
    QDir databaseDir(appconfig.get_tetradir());
    QString databasePath = databaseDir.canonicalPath();

    command.replace("%a", databasePath);

    // Запуск команды синхронизации
    ExecuteCommand cons;
    cons.setWindowTitle(tr("MyTetra synchronization"));
    cons.setMessageText(tr("Synchronization in progress, please wait..."));

    cons.setCommand(command);
    cons.run();

    // Блокируется история
    walkhistory.setDrop(true);

    // Заново считываются данные в дерево
    treeScreen->reloadKnowTree();
    restore_tree_position();
    restore_recordtable_position();
    restoreEditorCursorPosition();
    restoreEditorScrollBarPosition();

    // Разблокируется история посещений элементов
    walkhistory.setDrop(false);
}


void MainWindow::setupIconActions(void)
{
    actionTrayRestore = new QAction(tr("&Restore window"), this);
    connect(actionTrayRestore, SIGNAL(triggered()), this, SLOT(showNormal()));

    actionTrayMaximize = new QAction(tr("Ma&ximize window"), this);
    connect(actionTrayMaximize, SIGNAL(triggered()), this, SLOT(showMaximized()));

    actionTrayMinimize = new QAction(tr("Mi&nimize window"), this);
    connect(actionTrayMinimize, SIGNAL(triggered()), this, SLOT(hide()));

    actionTrayQuit = new QAction(tr("&Quit"), this);
    connect(actionTrayQuit, SIGNAL(triggered()), this, SLOT(applicationExit()));
}


void MainWindow::createTrayIcon(void)
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(actionTrayRestore);
    trayIconMenu->addAction(actionTrayMaximize);
    trayIconMenu->addAction(actionTrayMinimize);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(actionTrayQuit);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}


void MainWindow::setIcon(void)
{
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    QIcon icon = QIcon(":/resource/pic/logo.svg");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    // tray_icon->setToolTip(iconComboBox->itemText(index));
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(QSystemTrayIcon::isSystemTrayAvailable() == false) return;

    switch(reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            if(isVisible()) {
                if(isMinimized()) showNormal();
                else hide();
            } else {
                if(isMinimized()) showNormal();
                else show();
            }

        default:
            ;
    }
}


// Слот закрытия окна
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(enableRealClose == false) {
        if(QSystemTrayIcon::isSystemTrayAvailable() == false) return;

        // При приходе события закрыть окно, событие игнорируется
        // и окно просто делается невидимым. Это нужно чтобы при закрытии окна
        // программа не завершала работу
        if(trayIcon->isVisible()) {
            hide();
            event->ignore();
        }
    }
}


bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    Q_UNUSED(o);

    // qDebug() << "Event: " << e->type();

    // Отлавливание потери фокуса
    // QEvent::ActivationChange
    if(e->type() == QEvent::WindowDeactivate) {
        qDebug() << "Main window focus deactivate, save all state.";
        saveAllState();
    }

    return false; // Продолжать оработку событий дальше
}


void MainWindow::goWalkHistoryPrevious(void)
{
    editorScreen->save_textarea();

    QString id = editorScreen->getMiscField("id");
    walkhistory.add(id,
                    editorScreen->getCursorPosition(),
                    editorScreen->getScrollBarPosition(),
                    WALK_HISTORY_GO_PREVIOUS);
    walkhistory.setDrop(true);

    goWalkHistory();
}


void MainWindow::goWalkHistoryNext(void)
{
    editorScreen->save_textarea();

    QString id = editorScreen->getMiscField("id");
    walkhistory.add(id,
                    editorScreen->getCursorPosition(),
                    editorScreen->getScrollBarPosition(),
                    WALK_HISTORY_GO_NEXT);
    walkhistory.setDrop(true);

    goWalkHistory();
}


void MainWindow::goWalkHistory(void)
{
    // Выясняется идентификатор записи, на которую надо переключиться
    QString id = walkhistory.getId();

    if(id.length() == 0) {
        walkhistory.setDrop(false);
        return;
    }

    // Выясняется путь к ветке, где находится данная запись
    QStringList path = treeScreen->_knowtreemodel->getRecordPath(id);

    // Проверяем, есть ли такая ветка
    if(treeScreen->_knowtreemodel->isItemValid(path) == false) {
        walkhistory.setDrop(false);
        return;
    }


    // Выясняется позицию записи в таблице конечных записей
    TreeItem *item = treeScreen->_knowtreemodel->getItem(path);

    // Проверяем, есть ли такая позиция
    if(item->recordtableGetTableData()->isRecordExists(id) == false) {
        walkhistory.setDrop(false);
        return;
    }

    set_tree_position(path);
    select_id(id);

    if(appconfig.getRememberCursorAtHistoryNavigation()) {
        editorScreen->setCursorPosition(walkhistory.getCursorPosition(id));
        editorScreen->setScrollBarPosition(walkhistory.getScrollBarPosition(id));
    }

    walkhistory.setDrop(false);
}


// Метод, вызываемый из всех точек интерфейса, в которых происходит
// переход к другой записи. Метод вызывается до перехода, чтобы сохранить
// текст редактируемой записи
void MainWindow::saveTextarea(void)
{
    QString id = editorScreen->getMiscField("id");

    qDebug() << "MainWindow::saveTextarea() : id :" << id;

    editorScreen->save_textarea();

    walkhistory.add(id,
                    editorScreen->getCursorPosition(),
                    editorScreen->getScrollBarPosition());
}


// Слот, обрабатывающий смену фокуса на виджетах
void MainWindow::onFocusChanged(QWidget *widgetFrom, QWidget *widgetTo)
{
    Q_UNUSED(widgetFrom);

    if(widgetTo == NULL)
        return;

    qDebug() << "MainWindow::onFocusChanged() to " << widgetTo->objectName();

    return; // Временно ничего не делает
}


