#include <QObject>
#include <QMimeData>
#include <QProgressBar>
#include <QMap>
#include <QList>
#include <QFontMetrics>

#include "main.h"
#include "TableView.h"
#include "TableScreen.h"
#include "views/recordTable/verticalscrollarea.h"
#include "views/mainWindow/MainWindow.h"
#include "views/record/MetaEditor.h"
#include "models/appConfig/AppConfig.h"
#include "views/findInBaseScreen/FindScreen.h"
#include "libraries/WindowSwitcher.h"
#include "libraries/GlobalParameters.h"
#include "controllers/recordTable/TableController.h"
#include "libraries/FlatControl.h"
#include "views/browser/toolbarsearch.h"


extern GlobalParameters globalparameters;
extern AppConfig appconfig;


// Виджет, который отображает список записей в ветке
// c кнопками управления

TableScreen::TableScreen(QWidget *parent)
    : QWidget(parent)
    , _recordtable_controller(new TableController(this))
      //    , _recordtree_search(new browser::ToolbarSearch(this))
{
    // Инициализируется контроллер списка записей
    //    recordTableController = new RecordTableController(this);

    _recordtable_controller->setObjectName("recordTableController");

    setupActions();

    _recordtable_controller->init();


    setupUI();

    setupSignals();
    assembly();
}


TableScreen::~TableScreen()
{
    //    delete _recordtree_search;
    delete _recordtable_controller;
    delete _vertical_scrollarea;
}


// Настройка возможных действий
void TableScreen::setupActions(void)
{
    _actionpin = new QAction(tr("Pin note"), this);
    _actionpin->setStatusTip(tr("Pin a note"));
    _actionpin->setIcon(QIcon(":/resource/pic/pin.svg"));
    connect(_actionpin, &QAction::triggered
    , [&](bool checked = false) {
        Q_UNUSED(checked)
        // Обновление инфополей в области редактирования записи
        MetaEditor *metaeditor = globalparameters.getMetaEditor();  //MetaEditor *metaEditor = find_object<MetaEditor>("editorScreen");

        if(metaeditor)metaeditor->switch_pin();
    }
           );

    // Добавление записи
    // a->setShortcut(tr("Ctrl+X"));
    actionAddNewToEnd = new QAction(tr("Add note"), this);
    actionAddNewToEnd->setStatusTip(tr("Add a new note"));
    actionAddNewToEnd->setIcon(QIcon(":/resource/pic/note_add.svg"));
    //    setIcon(style()->standardIcon(QStyle::SP_FileIcon, 0, this));
    connect(actionAddNewToEnd, SIGNAL(triggered()), _recordtable_controller, SLOT(addNewToEndContext()));

    // Добавление записи до
    actionAddNewBefore = new QAction(tr("Add note before"), this);
    actionAddNewBefore->setStatusTip(tr("Add a note before selected"));
    connect(actionAddNewBefore, SIGNAL(triggered()), _recordtable_controller, SLOT(addNewBeforeContext()));

    // Добавление записи после
    actionAddNewAfter = new QAction(tr("Add note after"), this);
    actionAddNewAfter->setStatusTip(tr("Add a note after selected"));
    connect(actionAddNewAfter, SIGNAL(triggered()), _recordtable_controller, SLOT(addNewAfterContext()));

    // Редактирование записи
    actionEditField = new QAction(tr("Edit properties (pin, name, author, url, tags...)"), this);
    actionEditField->setStatusTip(tr("Edit note properties (pin, name, author, url, tags...)"));
    actionEditField->setIcon(QIcon(":/resource/pic/note_edit.svg"));
    connect(actionEditField, SIGNAL(triggered()), _recordtable_controller, SLOT(onEditFieldContext()));

    // Удаление записи
    actionDelete = new QAction(tr("Delete note(s)"), this);
    actionDelete->setStatusTip(tr("Delete note(s)"));
    actionDelete->setIcon(QIcon(":/resource/pic/note_delete.svg"));
    connect(actionDelete, SIGNAL(triggered()), _recordtable_controller, SLOT(deleteContext()));

    // Удаление записи с копированием в буфер обмена
    actionCut = new QAction(tr("&Cut note(s)"), this);
    actionCut->setStatusTip(tr("Cut notes(s) to clipboard"));
    actionCut->setIcon(QIcon(":/resource/pic/cb_cut.svg"));
    connect(actionCut, SIGNAL(triggered()), _recordtable_controller, SLOT(cut()));

    // Копирование записи (записей) в буфер обмена
    actionCopy = new QAction(tr("&Copy note(s)"), this);
    actionCopy->setStatusTip(tr("Copy note(s) to clipboard"));
    actionCopy->setIcon(QIcon(":/resource/pic/cb_copy.svg"));
    connect(actionCopy, SIGNAL(triggered()), _recordtable_controller, SLOT(copy()));

    // Вставка записи из буфера обмена
    actionPaste = new QAction(tr("&Paste note(s)"), this);
    actionPaste->setStatusTip(tr("Paste note(s) from clipboard"));
    actionPaste->setIcon(QIcon(":/resource/pic/cb_paste.svg"));
    connect(actionPaste, SIGNAL(triggered()), _recordtable_controller, SLOT(paste()));

    // Настройка внешнего вида таблицы конечных записей
    actionSettings = new QAction(tr("&View settings"), this);
    actionSettings->setStatusTip(tr("Setup table view settins"));
    actionSettings->setIcon(QIcon(":/resource/pic/cogwheel.svg"));
    connect(actionSettings, SIGNAL(triggered()), _recordtable_controller, SLOT(settings()));

    // Перемещение записи вверх
    actionMoveUp = new QAction(tr("&Move Up"), this);
    actionMoveUp->setStatusTip(tr("Move note up"));
    actionMoveUp->setIcon(QIcon(":/resource/pic/move_up.svg"));
    connect(actionMoveUp, SIGNAL(triggered()), _recordtable_controller, SLOT(moveUp()));

    // Перемещение записи вниз
    actionMoveDn = new QAction(tr("&Move Down"), this);
    actionMoveDn->setStatusTip(tr("Move note down"));
    actionMoveDn->setIcon(QIcon(":/resource/pic/move_dn.svg"));
    connect(actionMoveDn, SIGNAL(triggered()), _recordtable_controller, SLOT(moveDn()));

    // Поиск по базе (клик связывается с действием в MainWindow)
    actionFindInBase = new QAction(tr("Find in base"), this);
    actionFindInBase->setStatusTip(tr("Find in base"));
    actionFindInBase->setIcon(QIcon(":/resource/pic/find_in_base.svg"));

    // Синхронизация
    actionSyncro = new QAction(tr("Synchronization"), this);
    actionSyncro->setStatusTip(tr("Run synchronization"));
    actionSyncro->setIcon(QIcon(":/resource/pic/synchronization.svg"));
    connect(actionSyncro, SIGNAL(triggered()), this, SLOT(onSyncroClick()));

    // Перемещение по истории посещаемых записей назад
    actionWalkHistoryPrevious = new QAction(tr("Previous viewing note"), this);
    actionWalkHistoryPrevious->setShortcut(tr("Ctrl+<"));
    actionWalkHistoryPrevious->setStatusTip(tr("Previous note has been viewing"));
    actionWalkHistoryPrevious->setIcon(QIcon(":/resource/pic/walk_history_previous.svg"));
    connect(actionWalkHistoryPrevious, SIGNAL(triggered()), this, SLOT(onWalkHistoryPreviousClick()));

    // Перемещение по истории посещаемых записей вперед
    actionWalkHistoryNext = new QAction(tr("Next viewing note"), this);
    actionWalkHistoryNext->setShortcut(tr("Ctrl+>"));
    actionWalkHistoryNext->setStatusTip(tr("Next note has been viewing"));
    actionWalkHistoryNext->setIcon(QIcon(":/resource/pic/walk_history_next.svg"));
    connect(actionWalkHistoryNext, SIGNAL(triggered()), this, SLOT(onWalkHistoryNextClick()));

    // Кнопка Назад (Back) в мобильном интерфейсе
    actionBack = new QAction(tr("Back to item tree"), this);
    actionBack->setStatusTip(tr("Back to item tree"));
    actionBack->setIcon(QIcon(":/resource/pic/mobile_back.svg"));
    connect(actionBack, SIGNAL(triggered()), this, SLOT(onBackClick()));

    // Действия по сортировке
    actionSort = new QAction(tr("Toggle sorting"), this);
    actionSort->setStatusTip(tr("Enable/disable sorting by column"));
    actionSort->setIcon(QIcon(":/resource/pic/sort.svg"));
    connect(actionSort, SIGNAL(triggered()), _recordtable_controller, SLOT(onSortClick()));

    // Кнопка вызова печати таблицы конечных записей
    actionPrint = new QAction(tr("Print table"), this);
    actionPrint->setStatusTip(tr("Print current notes table"));
    actionPrint->setIcon(QIcon(":/resource/pic/drops.svg"));   //actionPrint->setIcon(QIcon(":/resource/pic/print_record_table.svg"));
    connect(actionPrint, SIGNAL(triggered()), _recordtable_controller, SLOT(onPrintClick()));

    // Сразу после создания все действия запрещены
    disableAllActions();
}


void TableScreen::setupUI(void)
{
    _toolsline = new QToolBar(this);

    /*
    QSize toolBarIconSize(16,16);
    toolsLine->setIconSize(toolBarIconSize);
    */

    if(appconfig.getInterfaceMode() == "mobile") {
        insertActionAsButton(_toolsline, actionBack);
        _toolsline->addSeparator();
    }

    insertActionAsButton(_toolsline, _actionpin);
    insertActionAsButton(_toolsline, actionAddNewToEnd);



    if(appconfig.getInterfaceMode() == "desktop") {
        insertActionAsButton(_toolsline, actionEditField);
        insertActionAsButton(_toolsline, actionDelete);
    }

    insertActionAsButton(_toolsline, actionWalkHistoryPrevious);
    insertActionAsButton(_toolsline, actionWalkHistoryNext);
    insertActionAsButton(_toolsline, actionMoveUp);
    insertActionAsButton(_toolsline, actionMoveDn);

    _toolsline->addSeparator();

    insertActionAsButton(_toolsline, actionCut);
    insertActionAsButton(_toolsline, actionCopy);
    insertActionAsButton(_toolsline, actionPaste);

    _toolsline->addSeparator();

    insertActionAsButton(_toolsline, actionSort);
    insertActionAsButton(_toolsline, actionPrint);
    insertActionAsButton(_toolsline, actionSettings);

    _extra_toolsline = new QToolBar(this);

    if(appconfig.getInterfaceMode() == "desktop") {
        insertActionAsButton(_extra_toolsline, actionSyncro);
        //        insertActionAsButton(extraToolsLine, actionWalkHistoryPrevious);
        //        insertActionAsButton(extraToolsLine, actionWalkHistoryNext);
    }

    insertActionAsButton(_extra_toolsline, actionFindInBase);

    _treepathlabel = new QLabel(this);

    //    _treepath_button = new FlatToolButton(this);

    _treepathlabel->setWordWrap(true);
    //    treePathLabel->setLineWidth(contentsRect().width());
    //    treePathLabel->setAlignment(Qt::AlignRight);
    //    treePathLabel->baseSize();

    //    int width = recordTableController->getView()->contentsRect().width();
    //    treePathLabel->setMaximumWidth(contentsRect().width());
    //    treePathLabel->setMinimumWidth(contentsRect().width());
    if(appconfig.getInterfaceMode() == "desktop")
        _treepathlabel->hide();

    _vertical_scrollarea = new VerticalScrollArea(
        std::make_shared<sd::_interface<sd::meta_info<void *>, void, QResizeEvent *>>("", &TableView::resizeEvent, _recordtable_controller->view())
        , this
    );
}


void TableScreen::setupSignals(void)
{
    //    connect(this, &RecordTableScreen::resizeEvent, _treepath_button
    //            // , &QLabel::resizeEvent
    //    , [this](QResizeEvent * e) {
    //        //        int width = recordTableController->getView()->contentsRect().width();
    //        _treepath_button->setMaximumWidth(
    //            e->size().width()   //
    //            //            contentsRect().width()
    //        );
    //        _treepath_button->setMinimumWidth(
    //            e->size().width()   //
    //            //            contentsRect().width()
    //        );

    //    }
    //           );
}


void TableScreen::assembly(void)
{
    _recordtable_toolslayout = new QHBoxLayout();
    _recordtable_toolslayout->addWidget(_toolsline);
    _recordtable_toolslayout->addStretch();
    _recordtable_toolslayout->addWidget(_extra_toolsline);

    //    _recordtree_searchlayout = new QHBoxLayout();
    //    _recordtree_searchlayout->addWidget(_recordtree_search);
    _recordtable_screenlayout = new QVBoxLayout();
    _recordtable_screenlayout->setObjectName("recordtablescreen_QVBoxLayout");



    //    //    QHBoxLayout *_treepath_button_layout = new QHBoxLayout();

    //    //    _treepath_button_layout->addWidget(_treepath_button);
    //    //    _treepath_button_layout->setMargin(0);
    //    //    _treepath_button_layout->setSpacing(0);
    //    ////    _treepath_button_layout->setSizeConstraint();

    //    //    if(appconfig.getInterfaceMode() == "mobile") {
    //    recordTableScreenLayout->addWidget(
    //        //        _treepath_button_layout    //
    //        _treepath_button
    //    );
    //    //    }

    //    _recordtable_screenlayout->addLayout(_recordtree_searchlayout);
    _recordtable_screenlayout->addLayout(_recordtable_toolslayout);






    // how to use VerticalScrollArea class:
    TableView *rtview = _recordtable_controller->view(); //    auto rtview = new QWidget(this);

    rtview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    _vertical_scrollarea->setWidget(rtview);
    rtview->viewport()->installEventFilter(_vertical_scrollarea);
    QVBoxLayout *baseLayout = new QVBoxLayout();
    baseLayout->addWidget(_vertical_scrollarea);
    //    _verticalscrollarea->viewport()->installEventFilter(rtview);


    _recordtable_screenlayout->addLayout(baseLayout); //

    //    _recordtable_screenlayout->addWidget(_recordtable_controller->view());

    //    recordTableScreenLayout->addWidget(_verticalscrollarea);

    //    recordTableController->getView()->viewport()->installEventFilter(someFilterWidget);







    //    _recordtable_screenlayout->setSizeConstraint(QLayout::SetNoConstraint);

    setLayout(_recordtable_screenlayout);

    // Границы убираются, так как данный объект будет использоваться как виджет
    QLayout *lt;
    lt = layout();
    lt->setContentsMargins(0, 2, 0, 0);
    lt->setSpacing(0);

}


void TableScreen::resizeEvent(QResizeEvent *e)
{

    _recordtable_controller->view()->resizeEvent(e);

    QWidget::resizeEvent(e);

    //    int rw = contentsRect().width();
    //    int vw = recordTableController->getView()->contentsRect().width();
    //    int vwe = recordTableController->getView()->width();
    //    int w = width();
    //    int bw = this->baseSize().width();
    //    int border = this->geometry().width();

    //    _treepath_button->setMaximumWidth(
    //        //        e->size().width() - 10 // contentsMargins().left() - contentsMargins().right() //
    //        contentsRect().width() - 1
    //    );

    //    _treepath_button->setMinimumWidth(
    //        //        e->size().width() - 10//contentsMargins().left() - contentsMargins().right() //
    //        contentsRect().width() - 1
    //    );

}

// Отключение всех действий над записями
// (но не всех действий на панели инструментов, так как на панели инструментов есть действия, не оказывающие воздействия на записи)
void TableScreen::disableAllActions(void)
{
    _actionpin->setEnabled(false);
    actionAddNewToEnd->setEnabled(false);
    actionAddNewBefore->setEnabled(false);
    actionAddNewAfter->setEnabled(false);
    actionEditField->setEnabled(false);
    actionDelete->setEnabled(false);

    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);
    actionPaste->setEnabled(false);

    actionMoveUp->setEnabled(false);
    actionMoveDn->setEnabled(false);
}


void TableScreen::toolsUpdate(void)
{
    qDebug() << "recordtablescreen::tools_update()";

    // Отключаются все действия
    disableAllActions();

    if(_recordtable_controller->is_table_notexists())
        return;

    // Выясняется, содержит ли текущая ветка подчиненные ветки
    /*
    QModelIndex index = find_object<TreeScreen>("treeScreen")->get_selection_model()->currentIndex();
    TreeItem *item = find_object<TreeScreen>("treeScreen")->kntrmodel->getItem(index);
    int branch_have_children=0;
    if(item->childCount()>0)branch_have_children=1;
    */

    // Включаются те действия которые разрешены

    _actionpin->setEnabled(true);
    // Добавление записи
    // Добавлять можно к любой ветке
    actionAddNewToEnd->setEnabled(true);

    // Добавление записи до
    // Добавлять "до" можно только тогда, когда выбрана только одна строка
    // и не включена сортировка
    if(_recordtable_controller->view()->selectionModel()->hasSelection()
       && (_recordtable_controller->view()->selectionModel()->selectedRows()).size() == 1
       && _recordtable_controller->view()->isSortingEnabled() == false
      ) {
        actionAddNewBefore->setEnabled(true);
    }

    // Добавление записи после
    // Добавлять "после" можно только тогда, когда выбрана только одна строка
    // и не включена сортировка
    if(_recordtable_controller->view()->selectionModel()->hasSelection()
       && (_recordtable_controller->view()->selectionModel()->selectedRows()).size() == 1
       && _recordtable_controller->view()->isSortingEnabled() == false
      ) {
        actionAddNewAfter->setEnabled(true);
    }

    // Редактирование записи
    // Редактировать можно только тогда, когда выбрана только одна строка
    if(_recordtable_controller->view()->selectionModel()->hasSelection()
       && (_recordtable_controller->view()->selectionModel()->selectedRows()).size() == 1
      ) {
        actionEditField->setEnabled(true);
    }

    // Удаление записи
    // Пункт активен только если запись (или записи) выбраны в списке
    if(_recordtable_controller->view()->selectionModel()->hasSelection()) {
        actionDelete->setEnabled(true);
    }

    // Удаление с копированием записи в буфер обмена
    // Пункт активен только если запись (или записи) выбраны в списке
    if(_recordtable_controller->view()->selectionModel()->hasSelection()) {
        actionCut->setEnabled(true);
    }

    // Копирование записи в буфер обмена
    // Пункт активен только если запись (или записи) выбраны в списке
    if(_recordtable_controller->view()->selectionModel()->hasSelection()) {
        actionCopy->setEnabled(true);
    }

    // Вставка записи из буфера обмена
    // Вставлять записи можно только тогда, когда выбрана
    // только одна строка (добавляется после выделеной строки)
    // или не выбрано ни одной строки (тогда добавляется в конец списка)
    // или записей вообще нет
    // И проверяется, содержит ли буфер обмена данные нужного формата
    if((_recordtable_controller->view()->selectionModel()->hasSelection() && (_recordtable_controller->view()->selectionModel()->selectedRows()).size() == 1)
       || _recordtable_controller->view()->selectionModel()->hasSelection() == false
       || _recordtable_controller->view()->model()->rowCount() == 0
      ) {
        const QMimeData *mimeData = QApplication::clipboard()->mimeData();

        if(mimeData != NULL) {
            if(mimeData->hasFormat("mytetra/records")) {
                actionPaste->setEnabled(true);
            }
        }
    }

    // Перемещение записи вверх
    // Пункт возможен только когда выбрана одна строка
    // и указатель стоит не на начале списка
    // и не включена сортировка
    if(_recordtable_controller->view()->selectionModel()->hasSelection()
       && (_recordtable_controller->view()->selectionModel()->selectedRows()).size() == 1
       && _recordtable_controller->view()->isSelectedSetToTop() == false
       && _recordtable_controller->view()->isSortingEnabled() == false
      ) {
        actionMoveUp->setEnabled(true);
    }

    // Перемещение записи вниз
    // Пункт возможен только когда выбрана одна строка
    // и указатель стоит не в конце списка
    // и не включена сортировка
    if(_recordtable_controller->view()->selectionModel()->hasSelection()
       && (_recordtable_controller->view()->selectionModel()->selectedRows()).size() == 1
       && _recordtable_controller->view()->isSelectedSetToBottom() == false
       && _recordtable_controller->view()->isSortingEnabled() == false
      ) {
        actionMoveDn->setEnabled(true);
    }

    // Обновляется состояние области редактирования текста
    if(_recordtable_controller->view()->selectionModel()->hasSelection()
       && _recordtable_controller->row_count() > 0
      ) {
        qDebug() << "In table select present";
        qDebug() << "In table row count is" << _recordtable_controller->row_count();
        find_object<MetaEditor>("editorScreen")->set_textarea_editable(true);
    } else {
        qDebug() << "In table select non present";
        find_object<MetaEditor>("editorScreen")->set_textarea_editable(false);
    }
}


// Получение номера первого выделенного элемента в таблице записи на экране
int TableScreen::getFirstSelectionPos(void)
{
    return _recordtable_controller->first_selectionpos();
}


// Получение ID первого выделенного элемента в таблице записи на экране
QString TableScreen::getFirstSelectionId(void)
{
    return _recordtable_controller->first_selectionid();
}


// Установка засветки в нужную строку в таблице записи на экране
void TableScreen::select_pos(int pos)
{
    _recordtable_controller->select_pos(pos);
}


// Установка засветки в нужную запись в таблице записей на экране
void TableScreen::select_id(QString id)
{
    _recordtable_controller->select_id(id);
}



// Действия при нажатии кнопки синхронизации
void TableScreen::onSyncroClick(void)
{
    find_object<MainWindow>("mainwindow")->synchronization();
}


void TableScreen::onWalkHistoryPreviousClick(void)
{
    find_object<MainWindow>("mainwindow")->goWalkHistoryPrevious();
}


void TableScreen::onWalkHistoryNextClick(void)
{
    find_object<MainWindow>("mainwindow")->goWalkHistoryNext();
}


// Возвращение к дереву разделов в мобильном интерфейсе
void TableScreen::onBackClick(void)
{
    globalparameters.getWindowSwitcher()->switchFromRecordtableToTree();
}


void TableScreen::setTreePath(QString path)
{
    _treepath = path; // Запоминается путь к ветке в виде строки
    _treepathlabel->setText(tr("<b>Path:</b> ") + _treepath);
    //    _treepath_button->setText(tr("<b>Path:</b> ") + _treepath);
}


QString TableScreen::getTreePath(void)
{
    return _treepath;
}


