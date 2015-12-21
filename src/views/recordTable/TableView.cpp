#include <QObject>
#include <QMimeData>
#include <QDrag>
#include <QVector>
#include <QHeaderView>
#include <QScrollBar>
#include "main.h"
#include "views/mainWindow/MainWindow.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "libraries/ClipboardRecords.h"
#include "TableView.h"
#include "views/recordTable/TableScreen.h"
#include "models/recordTable/TableModel.h"
#include "models/recordTable/TableProxyModel.h"
#include "models/appConfig/AppConfig.h"
#include "models/tree/KnowTreeModel.h"
#include "libraries/GlobalParameters.h"
#include "views/mainWindow/MainWindow.h"
#include "libraries/WindowSwitcher.h"
#include "controllers/recordTable/TableController.h"


extern GlobalParameters globalparameters;
extern AppConfig appconfig;


// Виджет, отображащий список записей в ветке


TableView::TableView(TableScreen *recordtablescreen, TableController *controller)
    : QTableView(recordtablescreen)
    , _recordtablescreen(recordtablescreen)
    , _controller(controller)
    , _layout(new QVBoxLayout(this))
{
    // Изначально сортировка запрещена (заголовки столбцов не будут иметь треугольнички)
    this->setSortingEnabled(false);

    // Настройка области виджета для кинетической прокрутки
    setKineticScrollArea(qobject_cast<QAbstractItemView *>(this));

    // Разрешение принимать жест QTapAndHoldGesture
    grabGesture(Qt::TapAndHoldGesture);

    _layout->setSpacing(0);

    _layout->setMargin(0);

    //    viewport()->getContentsMargins();
}


TableView::~TableView()
{
    delete _layout;
}


//void RecordTableView::setController(RecordTableController *pController)
//{
//    _controller = pController;
//}


// Пришлось ввести метод init, так как инициализация невозможна без
// созданных в parent QAction, а создать в parent QAction можно только
// при наличии ссылки на данный объект
// Причина в том, что одни и те же QAction используются в двух местах -
// в RecordTableScreen и здесь в контекстном меню
void TableView::init(void)
{
    qDebug() << "RecordTableView::init()";

    setupSignals();

    setSelectionMode(QAbstractItemView::
                     SingleSelection  // MultiSelection //ExtendedSelection
                    ); // // It was previously Extended Selection, but this mode is not suitable for Drag and Drop // Ранее было ExtendedSelection, но такой режим не подходит для Drag and Drop
    setSelectionBehavior(QAbstractItemView::SelectRows);

    restoreHeaderState();

    // Растягивание последней секции до размеров виджета
    horizontalHeader()->setStretchLastSection(true);

    // Заголовки не должны выглядеть нажатыми
    horizontalHeader()->setHighlightSections(false);

    // Горизонтальные заголовки делаются перемещяемыми
#if QT_VERSION >= 0x040000 && QT_VERSION < 0x050000
    horizontalHeader()->setMovable(true);
#endif
#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
    horizontalHeader()->setSectionsMovable(true);
#endif

    // Установка высоты строки с принудительной стилизацией (если это необходимо),
    // так как стилизация через QSS для элементов QTableView полноценно не работает
    // У таблицы есть вертикальные заголовки, для каждой строки, в которых отображается номер строки.
    // При задании высоты вертикального заголовка, высота применяется и для всех ячеек в строке.
    verticalHeader()->setDefaultSectionSize(verticalHeader()->minimumSectionSize());
    int height = appconfig.getUglyQssReplaceHeightForTableView();

    if(height != 0)
        verticalHeader()->setDefaultSectionSize(height);

    if(appconfig.getInterfaceMode() == "mobile")
        verticalHeader()->setDefaultSectionSize(getCalculateIconSizePx());

    setHorizontalScrollBarPolicy(Qt::
                                 // ScrollBarAsNeeded  //
                                 ScrollBarAlwaysOff
                                );   // setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    restoreColumnWidth();

    // Разрешается перемещать секции заголовка таблицы
    enableMoveSection = true;

    // Нужно установить правила показа контекстного самодельного меню
    // чтобы оно могло вызываться
    assemblyContextMenu();
    setContextMenuPolicy(Qt::CustomContextMenu);


}


void TableView::setupSignals(void)
{
    // Сигнал чтобы показать контекстное меню по правому клику на списке записей
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));

    // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
    connect(this, SIGNAL(tapAndHoldGestureFinished(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));

    // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
    // Signal to open for editing the parameters of the recording double click
    connect(this, &TableView::doubleClicked, this, &TableView::on_doubleclick);

    if(appconfig.getInterfaceMode() == "desktop")
        connect(this, &TableView::listSelectionChanged, this, &TableView::onSelectionChanged);

    // Для мобильного режима должен работать сигнал clicked, так как если засветка уже стоит на строке с записью, должна открыться запись
    // а в десктопном режиме этого не должно происходить, потому что запись уже видна на экране
    if(appconfig.getInterfaceMode() == "mobile")
        connect(this, &TableView::clicked, this, &TableView::onClickToRecord);

    TableScreen *parentPointer = qobject_cast<TableScreen *>(parent());

    // Сигналы для обновления панели инструментов при изменении в selectionModel()
    connect(this->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            parentPointer, SLOT(toolsUpdate(void)));
    connect(this->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            parentPointer, SLOT(toolsUpdate(void)));

    // Сигналы для обновления панели инструментов
    connect(this, SIGNAL(activated(const QModelIndex &)), parentPointer, SLOT(toolsUpdate(void)));
    connect(this, SIGNAL(clicked(const QModelIndex &)), parentPointer, SLOT(toolsUpdate(void)));
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)), parentPointer, SLOT(toolsUpdate(void)));
    connect(this, SIGNAL(entered(const QModelIndex &)), parentPointer, SLOT(toolsUpdate(void)));
    connect(this, SIGNAL(pressed(const QModelIndex &)), parentPointer, SLOT(toolsUpdate(void)));
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), parentPointer, SLOT(toolsUpdate(void)));

    connect(this->horizontalHeader(), SIGNAL(sectionMoved(int, int, int)), this, SLOT(onSectionMoved(int, int, int)));
    connect(this->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(onSectionResized(int, int, int)));
}


void TableView::restoreHeaderState(void)
{
    // Видимость горизонтальных заголовков
    if(appconfig.getRecordTableShowHorizontalHeaders() == false)
        horizontalHeader()->hide();
    else
        horizontalHeader()->show();

    // Видимость вертикальных заголовков
    if(appconfig.getRecordTableShowVerticalHeaders() == false)
        verticalHeader()->hide();
    else
        verticalHeader()->show();
}


void TableView::onSelectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected)
{
    QModelIndex selectRecord;
    QModelIndex deselectRecord;

    if(!selected.indexes().isEmpty())
        selectRecord = selected.indexes().at(0);

    if(!deselected.indexes().isEmpty())
        deselectRecord = deselected.indexes().at(0);

    /*
    qDebug() << "RecordTableView::onSelectionChanged()";
    qDebug() << "Current index. row() " << selectRecord.row() << " isValid() " << selectRecord.isValid();
    qDebug() << "Previous index. row() " << deselectRecord.row() << " isValid() " << deselectRecord.isValid();
    */

    // return;

    if(selectRecord.isValid())
        click_record(selectRecord);
}


// Слот клика по записи. Принимает индекс Proxy модели
void TableView::onClickToRecord(const QModelIndex &index)
{
    click_record(index);
}


// Действия при выборе строки таблицы конечных записей. Принимает индекс Proxy модели
// Actions when choosing the final row of the table entries. Accepts index Proxy models
void TableView::click_record(const QModelIndex &index)
{
    _controller->click_record(index);

    globalparameters.getWindowSwitcher()->switchFromRecordtableToRecord();
}


void TableView::assemblyContextMenu(void)
{
    // Конструирование меню
    contextMenu = new QMenu(this);

    //    RecordTableScreen *_recordtablescreen = qobject_cast<RecordTableScreen *>(parent());

    contextMenu->addAction(_recordtablescreen->_actionpin);
    contextMenu->addAction(_recordtablescreen->actionAddNewToEnd);
    contextMenu->addAction(_recordtablescreen->actionEditField);
    contextMenu->addAction(_recordtablescreen->actionDelete);

    contextMenu->addSeparator();

    contextMenu->addAction(_recordtablescreen->actionAddNewBefore);
    contextMenu->addAction(_recordtablescreen->actionAddNewAfter);
    //    contextMenu->addAction(parentPointer->actionMoveUp);
    //    contextMenu->addAction(parentPointer->actionMoveDn);
    //    contextMenu->addAction(parentPointer->actionSyncro);

    contextMenu->addSeparator();

    contextMenu->addAction(_recordtablescreen->actionCut);
    contextMenu->addAction(_recordtablescreen->actionCopy);
    contextMenu->addAction(_recordtablescreen->actionPaste);

    contextMenu->addSeparator();

    contextMenu->addAction(_recordtablescreen->actionSort);
    contextMenu->addAction(_recordtablescreen->actionPrint);
    contextMenu->addAction(_recordtablescreen->actionSettings);
}


// Открытие контекстного меню в таблице конечных записей
void TableView::onCustomContextMenuRequested(const QPoint &pos)
{
    qDebug() << "In on_customContextMenuRequested";
    //    auto p = parent();

    //    RecordTableScreen *parentPointer = qobject_cast<RecordTableScreen *>(parent());
    //    RecordTableScreen *parentPointer = static_cast<RecordTableScreen *>(p);
    //    RecordTableScreen *_recordtablescreen = globalparameters.getRecordTableScreen();

    // Устанавливается надпись для пункта сортировки
    if(!this->isSortingEnabled())
        _recordtablescreen->actionSort->setText(tr("Enable sorting"));
    else
        _recordtablescreen->actionSort->setText(tr("Disable sorting"));

    // Запоминается номер колонки, по которой был произведен клик (номер колонки будет правильный, даже если записей мало и клик произошел под записями)
    int n = this->horizontalHeader()->logicalIndexAt(pos);
    qDebug() << "Click on column number " << n;
    _recordtablescreen->actionSort->setData(n);   // Запоминается в объект действия для сортировки


    // Включение отображения меню на экране
    // menu.exec(event->globalPos());
    contextMenu->exec(viewport()->mapToGlobal(pos));
}


// Слот, срабатывающий при нажатии кнопки редактирования записи
void TableView::on_doubleclick(const QModelIndex &index)
{
    qDebug() << "In RecordTableView::editFieldContext";

    // Получение индекса выделенного элемента
    QModelIndexList selectItems = selectionModel()->selectedIndexes();

    if(selectItems.size() > 0) {
        //QModelIndex index = selectItems.at(0);

        //globalparameters.getMetaEditor()->switch_pin();

        //controller->openWebsite(index);    //controller->editFieldContext(index);

        // Нужно перерисовать окно редактирования чтобы обновились инфополя
        // делается это путем "повторного" выбора текущего пункта
        click_record(index); // Раньше было select()
        globalparameters.mainwindow()->editor_switch();

    }
}


// Слот, срабатывающий при нажатии кнопки редактирования записи
void TableView::editFieldContext(void)
{
    qDebug() << "In RecordTableView::editFieldContext";

    // Получение индекса выделенного элемента
    QModelIndexList selectItems = selectionModel()->selectedIndexes();
    QModelIndex index = selectItems.at(0);

    _controller->edit_fieldcontext(index);

    // Нужно перерисовать окно редактирования чтобы обновились инфополя
    // делается это путем "повторного" выбора текущего пункта
    click_record(index); // Раньше было select()
}

// Получение номера первого выделенного элемента
int TableView::getFirstSelectionPos(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectItems = selectionModel()->selectedIndexes();

    if(selectItems.isEmpty())
        return -1; // Если ничего не выделено
    else
        return (selectItems.at(0)).row(); // Номер первого выделенного элемента
}


// Получение ID первого выделенного элемента
QString TableView::getFirstSelectionId(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectItems = selectionModel()->selectedIndexes();

    if(selectItems.isEmpty())
        return ""; // Если ничего не выделено

    return selectItems.at(0).data(RECORD_ID_ROLE).toString();
}


// Получение модельного индекса первого выделенного элемента в Proxy модели
QModelIndex TableView::getFirstSelectionProxyIndex(void)
{
    int pos = getFirstSelectionPos();

    if(pos == -1)
        return QModelIndex();

    // QModelIndex index = recordProxyModel->index( pos, 0 );
    QModelIndex index = _controller->pos_to_proxyindex(pos);

    return index;
}


// Получение модельного индекса первого выделенного элемента в Source модели
QModelIndex TableView::getFirstSelectionSourceIndex(void)
{
    QModelIndex proxyIndex = getFirstSelectionProxyIndex();

    if(!proxyIndex.isValid())
        return QModelIndex();

    // QModelIndex index = recordProxyModel->mapToSource( proxyIndex );
    QModelIndex index = _controller->proxyindex_to_sourceindex(proxyIndex);

    return index;
}


bool TableView::isSelectedSetToTop(void)
{
    if(getFirstSelectionPos() == 0)return true;
    else return false;
}


bool TableView::isSelectedSetToBottom(void)
{
    if(getFirstSelectionPos() == model()->rowCount() - 1)
        return true;
    else
        return false;
}


// Установка засветки в нужную строку на экране
void TableView::setSelectionToPos(int iPos)
{
    QModelIndex index = _controller->pos_to_proxyindex(iPos); // Модельный индекс в Proxy модели
    int pos = index.row();

    // todo: Если это условие ни разу не сработает, значит преобразование ipos - pos надо просто убрать
    if(pos != iPos) {
        QMessageBox msgBox;
        msgBox.setText("In RecordTableView::setSelectionToPos() input pos not equal model pos");
        msgBox.exec();
    }

    int rowCount = _controller->row_count();

    if(pos > (rowCount - 1))
        return;

    // Простой механизм выбора строки. Похоже, что его использовать не получится
    selectRow(pos);

    //    auto recordSourceModel = controller->getRecordTableModel();
    //    QModelIndex selIdx = recordSourceModel->index(pos, 0);

    // Установка засветки на нужный индекс
    // Set the backlight to the desired index
    selectionModel()->setCurrentIndex(index   // selIdx
                                      , QItemSelectionModel::
                                      Select    // ClearAndSelect
                                     );

    // В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал смены строки в модели выбора)
    // Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
    // In response to the mobile version of the record is no choice (not processed signal line change to the selection model)
    // Therefore, the recording must be made a virtual click to fill the final table of records
    if(appconfig.getInterfaceMode() == "mobile")
        emit this->clicked(index); // QModelIndex selIdx=recordSourceModel->index(pos, 0);

    // emit this->clicked(index);

    scrollTo(currentIndex());   // QAbstractItemView::PositionAtCenter

    this->setFocus();   // ?
}


// mode - режим в котором добавлялась новая запись
// pos - позиция новой записи в размерности Source модели
void TableView::moveCursorToNewRecord(int mode, int pos)
{
    // Установка курсора на только что созданную позицию
    /*
    QModelIndex selIdx=recordSourceModel->index(selPos, 0); // Создание индекса из номера
    selectionModel()->setCurrentIndex(selIdx,QItemSelectionModel::ClearAndSelect);
    */

    // В QTableView некорректно работает установка на только что созданную строку
    // Это как-то связано с отрисовкой виджета QTableView
    // Прокрутка к только что созданной строке через selectRow() показывает только
    // верхнюю часть новой строки. Чтобы этого избежать, при добавлении в конец
    // таблицы конечных записей, установка прокрутки делается через scrollToBottom()
    if(mode == ADD_NEW_RECORD_TO_END ||
       (mode == ADD_NEW_RECORD_AFTER && pos >= (model()->rowCount() - 1)))
        scrollToBottom();

    int proxyPos = _controller->pos_to_proxyindex(pos).row();

    selectRow(proxyPos);
}



// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool TableView::event(QEvent *event)
{
    if(event->type() == QEvent::Gesture) {
        qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
        return gestureEvent(static_cast<QGestureEvent *>(event));
    }

    return QTableView::event(event);
}


// Обработчик жестов
// Вызывается из обработчика событий
bool TableView::gestureEvent(QGestureEvent *event)
{
    qDebug() << "In gestureEvent()" << event;

    if(QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
        tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(gesture));

    return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void TableView::tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture)
{
    qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

    if(gesture->state() == Qt::GestureFinished)
        if(globalparameters.getTargetOs() == "android")
            emit tapAndHoldGestureFinished(mapFromGlobal(gesture->position().toPoint()));
}


// Реакция на нажатие кнопок мышки
void TableView::mousePressEvent(QMouseEvent *event)
{
    // Если нажата левая кнопка мыши
    if(event->buttons() == Qt::LeftButton) {
        mouseStartPos = event->pos();
    }

    QTableView::mousePressEvent(event);
}


// Реакция на движение мышкой
void TableView::mouseMoveEvent(QMouseEvent *event)
{
    // Если при движении нажата левая кнопка мышки
    if(event->buttons() & Qt::LeftButton) {
        // Выясняется расстояние от места начала нажатия
        int distance = (event->pos() - mouseStartPos).manhattanLength();

        if(distance >= QApplication::startDragDistance() * 2) {
            startDrag();
        }
    }

    QTableView::mouseMoveEvent(event);
}


// Реакция на отпускание клавиши мышки
void TableView::mouseReleaseEvent(QMouseEvent *event)
{
    QTableView::mouseReleaseEvent(event);
}


// Начало переноса записи
void TableView::startDrag()
{
    qDebug() << "Start record drag\n";

    // Если действительно выбрана строка
    if(currentIndex().isValid()) {
        // Перед переносом нужно запомнить текст последней редактируемой записи, чтобы не перенесся неотредактированный вариант
        find_object<MainWindow>("mainwindow")->saveTextarea();

        // Копирование выделенных строк в объект переноса
        QDrag *drag = new QDrag(this);
        drag->setMimeData(getSelectedRecords());

        // Запуск операции перетаскивания объекта
        unsigned int result = drag->exec(Qt::MoveAction);

        // Если перетаскивание завершено
        if(result == 0) {
            // Сюда код дойдет после того, как перетаскивание будет закончено
            qDebug() << "Success drag and drop move record";

            // todo: Совершенно непонятно, где удалять объект drag.
            // Если удалять в этом месте, имеем сегфолт
            // delete drag;

            // В модели данных обнуляется оформление элемента, который (возможно) подсвечивался при Drag And Drop
            find_object<TreeScreen>("treeScreen")->_knowtreemodel->setData(QModelIndex(), QVariant(false), Qt::UserRole);
        }
    }
}


ClipboardRecords *TableView::getSelectedRecords(void)
{
    // Получение списка Item-элементов, подлежащих копированию
    QModelIndexList itemsForCopy = selectionModel()->selectedIndexes();

    // В списке должны остаться только элементы столбца 0
    // (так как ранее одна строка была одним элементом списка,
    // а теперь используется таблица, и при одной выделенной строке
    // выделено несколько элементов)
    // The list should only elements of the column 0
    // (As earlier one line was one element of the list,
    // And now uses a table, and a dedicated line
    // Select multiple items)
    QMutableListIterator<QModelIndex> iterator(itemsForCopy);

    while(iterator.hasNext()) {
        iterator.next();

        QModelIndex index = iterator.value();

        if(index.column() > 0)
            iterator.remove();
    }


    // Список возвращается в произвольном порядке, не в таком как на экране
    // поэтому его нужно отсортировать по QModelIndex
    qSort(itemsForCopy.begin(), itemsForCopy.end());

    qDebug() << "Get selected records";

    for(int i = 0; i < itemsForCopy.size(); ++i)
        qDebug() << itemsForCopy.at(i).data().toString();


    // Объект с данными для заполнения буфера обмена
    ClipboardRecords *clipboardRecords = new ClipboardRecords();
    clipboardRecords->clear();

    // Объект заполняется выбранными записями
    _controller->add_records_toclipboard(clipboardRecords, itemsForCopy);

    return clipboardRecords;
}


// Переопределенный сигнал (virtual protected slot)
void TableView::selectionChanged(const QItemSelection &selected,
                                       const QItemSelection &deselected)
{
    // qDebug() << "RecordTableView::selectionChanged()";

    emit listSelectionChanged(selected, deselected);

    // Для корректной работы надо вызвать сигнал базового класса
    QTableView::selectionChanged(selected, deselected);
}


// Слот, срабатывающий после того, как был передвинут горизонтальный заголовок
void TableView::onSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);

    if(!enableMoveSection)
        return;

    // Если была включена сортировка
    /*
    if( this->isSortingEnabled() )
      if( horizontalHeader()->sortIndicatorSection())
        horizontalHeader()->setSortIndicator(n, Qt::AscendingOrder); // Треугольничек сортировки переставляется на нужный столбец
    */

    // Запоминается ширина столбцов
    int oldVisualWidth = horizontalHeader()->sectionSize(oldVisualIndex);
    int newVisualWidth = horizontalHeader()->sectionSize(newVisualIndex);

    // В настройках последовательность полей меняется
    QStringList showFields = appconfig.getRecordTableShowFields();
    showFields.move(oldVisualIndex, newVisualIndex);
    appconfig.setRecordTableShowFields(showFields);

    qDebug() << "New show field sequence" << showFields;

    enableMoveSection = false;

    // Перемещение в данном представлении сбрасывается, так как модель берет последовательность полей из настроек
    // После это кода logicalIindex=visualIndex для всех полей
    for(int logicalIdx = 0; logicalIdx < showFields.size(); logicalIdx++) {
        int visualIdx = horizontalHeader()->visualIndex(logicalIdx);

        if(visualIdx != logicalIdx)
            horizontalHeader()->moveSection(visualIdx, logicalIdx); // Этот вызов запустит срабатывание этого же слота sectionMoved(), поэтому нужен enableMoveSection
    }

    enableMoveSection = true;

    horizontalHeader()->reset();

    // Устанавливается ширина столбцов после перемещения
    horizontalHeader()->resizeSection(oldVisualIndex, newVisualWidth);
    horizontalHeader()->resizeSection(newVisualIndex, oldVisualWidth);

    saveColumnWidth();
}


void TableView::onSectionResized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);

    saveColumnWidth();
}


// Сохранение ширины колонок в конфигфайл
void TableView::saveColumnWidth(void)
{
    // Выясняется количество полей
    int count = appconfig.getRecordTableShowFields().size();

    QStringList columnWidthList;

    for(int i = 0; i < count; i++) {
        QString width;
        width.setNum(columnWidth(i));
        columnWidthList << width;
    }

    appconfig.setRecordTableFieldsWidth(columnWidthList);

    // qDebug() << "Save column width " << columnWidthList;
}


// Восстановление ширины колонок из конфигфайла
void TableView::restoreColumnWidth(void)
{
    QStringList columnWidthList = appconfig.getRecordTableFieldsWidth();

    // qDebug() << "Restore column width " << columnWidthList;

    // Восстанавливается ширина всех колонок без последней
    // Чтобы последняя растягивалась по месту
    for(int i = 0; i < columnWidthList.size() - 1; i++)
        setColumnWidth(i, columnWidthList[i].toInt());

    resizeEvent(nullptr);
}

//// if pin and title width beyond container width, when click title item, widget will move left, pin column will disappeared
//void RecordTableView::on_parent_resizevent(QResizeEvent *e)
//{
//    Q_UNUSED(e)

//    //    int vscrollbar_width = this->verticalScrollBar()->width();
//    //    int viewport_width = this->maximumViewportSize().width();
//    //    int _width = width();
//    //    int parent_width = e->size().width();
//    //    auto margin = viewport()->contentsMargins();
//    //    auto rect =  viewport()->contentsRect();
//    auto rect1 = contentsRect();
//    auto real_width = rect1.width();
//    //    auto size_width = size().width();

//    if((columnWidth(0) + columnWidth(1)) >= real_width) {
//        setColumnWidth(0, 30);
//        setColumnWidth(1, (real_width >= 30) ? real_width - 30 : columnWidth(1));
//    } else {
//        setColumnWidth(0, 30);
//        real_width = this->contentsRect().width();

//        if(real_width >= 300) {
//            setColumnWidth(1, 300 - 30); // restoreColumnWidth();
//        } else {
//            setColumnWidth(1, (real_width >= 30) ? real_width - 30 : columnWidth(1));
//        }
//    }

//    //    saveColumnWidth();
//}


// if pin and title width beyond container width, when click title item, widget will move left, pin column will disappeared
void TableView::resizeEvent(QResizeEvent *e)
{
    //    Q_UNUSED(e)

    //    int vscrollbar_width = this->verticalScrollBar()->width();
    //    int viewport_width = this->maximumViewportSize().width();
    //    int _width = width();
    //    int parent_width = e->size().width();
    //    auto margin = viewport()->contentsMargins();
    //    auto rect0 =  viewport()->contentsRect();
    auto rect = contentsRect();
    auto real_width = rect.width();
    //    auto size_width = size().width();

    if((columnWidth(0) + columnWidth(1)) >= real_width) {
        setColumnWidth(0, 30);
        setColumnWidth(1, (real_width >= 30) ? real_width - 30 : columnWidth(1));
    } else {
        setColumnWidth(0, 30);
        real_width = this->contentsRect().width();

        if(real_width >= 300) {
            setColumnWidth(1, 300 - 30); // restoreColumnWidth();
        } else {
            setColumnWidth(1, (real_width >= 30) ? real_width - 30 : columnWidth(1));
        }
    }

    QTableView::resizeEvent(e);
}
