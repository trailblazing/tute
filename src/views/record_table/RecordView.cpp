#include <QObject>
#include <QMimeData>
#include <QDrag>
#include <QVector>
#include <QHeaderView>
#include <QScrollBar>
#include "main.h"
#include "views/main_window/MainWindow.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "libraries/ClipboardRecords.h"
#include "RecordView.h"
#include "views/record_table/RecordScreen.h"
#include "models/record_table/RecordModel.h"
#include "models/record_table/RecordProxyModel.h"
#include "models/app_config/AppConfig.h"
#include "models/tree/TreeKnowModel.h"
#include "libraries/GlobalParameters.h"
#include "views/main_window/MainWindow.h"
#include "libraries/WindowSwitcher.h"
#include "controllers/record_table/RecordController.h"
#include "models/record_table/ItemsFlat.h"

extern GlobalParameters globalparameters;
extern AppConfig appconfig;


// Виджет, отображащий список записей в ветке


RecordView::RecordView(RecordScreen *_record_screen, RecordController *_record_controller)
    : QTableView(_record_screen)
    , _table_screen(_record_screen)
    , _record_controller(_record_controller)
    , _layout(new QVBoxLayout(this))
{
    // setObjectName(screen_name + "_view");

    // Изначально сортировка запрещена (заголовки столбцов не будут иметь треугольнички)
    this->setSortingEnabled(false);

    // Настройка области виджета для кинетической прокрутки
    set_kinetic_scrollarea(qobject_cast<QAbstractItemView *>(this));

    // Разрешение принимать жест QTapAndHoldGesture
    grabGesture(Qt::TapAndHoldGesture);

    _layout->setSpacing(0);

    _layout->setMargin(0);

    //    viewport()->getContentsMargins();
}


RecordView::~RecordView()
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
void RecordView::init(void)
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
        verticalHeader()->setDefaultSectionSize(calculate_iconsize_px());

    setHorizontalScrollBarPolicy(Qt::
                                 // ScrollBarAsNeeded  //
                                 ScrollBarAlwaysOff
                                );   // setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    restoreColumnWidth();

    // Разрешается перемещать секции заголовка таблицы
    _enable_move_section = true;

    // Нужно установить правила показа контекстного самодельного меню
    // чтобы оно могло вызываться
    assemblyContextMenu();
    setContextMenuPolicy(Qt::CustomContextMenu);


}


void RecordView::setupSignals(void)
{
    // Сигнал чтобы показать контекстное меню по правому клику на списке записей
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));

    // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
    connect(this, SIGNAL(tapAndHoldGestureFinished(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));

    // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
    // Signal to open for editing the parameters of the recording double click
    connect(this, &RecordView::doubleClicked, this, &RecordView::on_doubleclick);

    if(appconfig.getInterfaceMode() == "desktop")
        connect(this, &RecordView::listSelectionChanged, this, &RecordView::onSelectionChanged);

    // Для мобильного режима должен работать сигнал clicked, так как если засветка уже стоит на строке с записью, должна открыться запись
    // а в десктопном режиме этого не должно происходить, потому что запись уже видна на экране
    //    if(appconfig.getInterfaceMode() == "mobile")
    connect(this, &RecordView::clicked, this, &RecordView::onClickToRecord);

    RecordScreen *parentPointer = qobject_cast<RecordScreen *>(parent());

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


void RecordView::restoreHeaderState(void)
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


void RecordView::onSelectionChanged(const QItemSelection &selected,
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
void RecordView::onClickToRecord(const QModelIndex &index)
{
    click_record(index);
}


// Действия при выборе строки таблицы конечных записей. Принимает индекс Proxy модели
// Actions when choosing the final row of the table entries. Accepts index Proxy models
void RecordView::click_record(const QModelIndex &index)
{
    _record_controller->click_item(index);

    globalparameters.window_switcher()->switchFromRecordtableToRecord();
}


void RecordView::assemblyContextMenu(void)
{
    // Конструирование меню
    _context_menu = new QMenu(this);

    //    RecordTableScreen *_recordtablescreen = qobject_cast<RecordTableScreen *>(parent());

    _context_menu->addAction(_table_screen->_pin);
    _context_menu->addAction(_table_screen->_addnew_to_end);
    _context_menu->addAction(_table_screen->_edit_field);
    _context_menu->addAction(_table_screen->_delete);

    _context_menu->addSeparator();

    _context_menu->addAction(_table_screen->_addnew_before);
    _context_menu->addAction(_table_screen->_addnew_after);
    //    contextMenu->addAction(parentPointer->actionMoveUp);
    //    contextMenu->addAction(parentPointer->actionMoveDn);
    //    contextMenu->addAction(parentPointer->actionSyncro);

    _context_menu->addSeparator();

    _context_menu->addAction(_table_screen->_cut);
    _context_menu->addAction(_table_screen->_copy);
    _context_menu->addAction(_table_screen->_paste);

    _context_menu->addSeparator();

    _context_menu->addAction(_table_screen->_sort);
    _context_menu->addAction(_table_screen->_print);
    _context_menu->addAction(_table_screen->_settings);
}


// Открытие контекстного меню в таблице конечных записей
void RecordView::onCustomContextMenuRequested(const QPoint &pos)
{
    qDebug() << "In on_customContextMenuRequested";
    //    auto p = parent();

    //    RecordTableScreen *parentPointer = qobject_cast<RecordTableScreen *>(parent());
    //    RecordTableScreen *parentPointer = static_cast<RecordTableScreen *>(p);
    //    RecordTableScreen *_recordtablescreen = globalparameters.getRecordTableScreen();

    // Устанавливается надпись для пункта сортировки
    if(!this->isSortingEnabled())
        _table_screen->_sort->setText(tr("Enable sorting"));
    else
        _table_screen->_sort->setText(tr("Disable sorting"));

    // Запоминается номер колонки, по которой был произведен клик (номер колонки будет правильный, даже если записей мало и клик произошел под записями)
    int n = this->horizontalHeader()->logicalIndexAt(pos);
    qDebug() << "Click on column number " << n;
    _table_screen->_sort->setData(n);   // Запоминается в объект действия для сортировки


    // Включение отображения меню на экране
    // menu.exec(event->globalPos());
    _context_menu->exec(viewport()->mapToGlobal(pos));
}


// Слот, срабатывающий при нажатии кнопки редактирования записи
void RecordView::on_doubleclick(const QModelIndex &index)
{
    qDebug() << "In RecordTableView::editFieldContext";

    // Получение индекса выделенного элемента
    QModelIndexList selectItems = selectionModel()->selectedIndexes();

    if(selectItems.size() > 0) {
        //QModelIndex index = selectItems.at(0);

        //globalparameters.getMetaEditor()->switch_pin();

        //controller->open_website(index);    //controller->editFieldContext(index);

        // Нужно перерисовать окно редактирования чтобы обновились инфополя
        // делается это путем "повторного" выбора текущего пункта
        click_record(index); // Раньше было select()
        globalparameters.mainwindow()->editor_switch();

    }
}


// Слот, срабатывающий при нажатии кнопки редактирования записи
void RecordView::editFieldContext(void)
{
    qDebug() << "In RecordTableView::editFieldContext";

    // Получение индекса выделенного элемента
    QModelIndexList selectItems = selectionModel()->selectedIndexes();
    QModelIndex index = selectItems.at(0);

    _record_controller->edit_field_context(index);

    // Нужно перерисовать окно редактирования чтобы обновились инфополя
    // делается это путем "повторного" выбора текущего пункта
    click_record(index); // Раньше было select()
}

// Получение номера первого выделенного элемента
int RecordView::getFirstSelectionPos(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectItems = selectionModel()->selectedIndexes();

    if(selectItems.isEmpty())
        return -1; // Если ничего не выделено
    else
        return (selectItems.at(0)).row(); // Номер первого выделенного элемента
}


// Получение ID первого выделенного элемента
QString RecordView::getFirstSelectionId(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectItems = selectionModel()->selectedIndexes();

    if(selectItems.isEmpty())
        return ""; // Если ничего не выделено

    return selectItems.at(0).data(RECORD_ID_ROLE).toString();
}


// Получение модельного индекса первого выделенного элемента в Proxy модели
QModelIndex RecordView::first_selection_proxy_index(void)
{
    int pos = getFirstSelectionPos();

    if(pos == -1)
        return QModelIndex();

    // QModelIndex index = recordProxyModel->index( pos, 0 );
    QModelIndex index = _record_controller->pos_to_proxyindex(pos);

    return index;
}


// Получение модельного индекса первого выделенного элемента в Source модели
QModelIndex RecordView::first_selection_source_index(void)
{
    QModelIndex proxy_index = first_selection_proxy_index();

    if(!proxy_index.isValid())
        return QModelIndex();

    // QModelIndex index = recordProxyModel->mapToSource( proxyIndex );
    QModelIndex index = _record_controller->proxyindex_to_sourceindex(proxy_index);

    return index;
}


bool RecordView::isSelectedSetToTop(void)
{
    if(getFirstSelectionPos() == 0)return true;
    else return false;
}


bool RecordView::isSelectedSetToBottom(void)
{
    if(getFirstSelectionPos() == model()->rowCount() - 1)
        return true;
    else
        return false;
}


// Установка засветки в нужную строку на экране
void RecordView::setSelectionToPos(int iPos)
{
    QModelIndex index = _record_controller->pos_to_proxyindex(iPos); // Модельный индекс в Proxy модели
    int pos = index.row();

    // todo: Если это условие ни разу не сработает, значит преобразование ipos - pos надо просто убрать
    if(pos != iPos) {
        QMessageBox msgBox;
        msgBox.setText("In RecordTableView::setSelectionToPos() input pos not equal model pos");
        msgBox.exec();
    }

    int rowCount = _record_controller->row_count();

    if(pos > (rowCount - 1))
        return;

    // Простой механизм выбора строки. Похоже, что его использовать не получится
    selectRow(pos);

    //    auto recordSourceModel = controller->getRecordTableModel();
    //    QModelIndex selIdx = recordSourceModel->index(pos, 0);

    // Установка засветки на нужный индекс
    // Set the backlight to the desired index
    selectionModel()->setCurrentIndex(index   // selIdx
                                      , QItemSelectionModel::Select    // ClearAndSelect
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
void RecordView::moveCursorToNewRecord(int mode, int pos)
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
    if(mode == ADD_NEW_RECORD_TO_END
       || (mode == ADD_NEW_RECORD_AFTER && pos >= (model()->rowCount() - 1))
      ) {
        scrollToBottom();
    }

    int proxy_pos = _record_controller->pos_to_proxyindex(pos).row();

    selectRow(proxy_pos);
}



// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool RecordView::event(QEvent *event)
{
    if(event->type() == QEvent::Gesture) {
        qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
        return gestureEvent(static_cast<QGestureEvent *>(event));
    }

    return QTableView::event(event);
}


// Обработчик жестов
// Вызывается из обработчика событий
bool RecordView::gestureEvent(QGestureEvent *event)
{
    qDebug() << "In gestureEvent()" << event;

    if(QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
        tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(gesture));

    return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void RecordView::tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture)
{
    qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

    if(gesture->state() == Qt::GestureFinished)
        if(globalparameters.target_os() == "android")
            emit tapAndHoldGestureFinished(mapFromGlobal(gesture->position().toPoint()));
}


// Реакция на нажатие кнопок мышки
void RecordView::mousePressEvent(QMouseEvent *event)
{
    // Если нажата левая кнопка мыши
    if(event->buttons() == Qt::LeftButton) {
        _mouse_start_position = event->pos();
    }

    QTableView::mousePressEvent(event);
}


// Реакция на движение мышкой
void RecordView::mouseMoveEvent(QMouseEvent *event)
{
    // Если при движении нажата левая кнопка мышки
    if(event->buttons() & Qt::LeftButton) {
        // Выясняется расстояние от места начала нажатия
        int distance = (event->pos() - _mouse_start_position).manhattanLength();

        if(distance >= QApplication::startDragDistance() * 2) {
            startDrag();
        }
    }

    QTableView::mouseMoveEvent(event);
}


// Реакция на отпускание клавиши мышки
void RecordView::mouseReleaseEvent(QMouseEvent *event)
{
    QTableView::mouseReleaseEvent(event);
}


// Начало переноса записи
void RecordView::startDrag()
{
    qDebug() << "Start record drag\n";

    // Если действительно выбрана строка
    if(currentIndex().isValid()) {
        // Перед переносом нужно запомнить текст последней редактируемой записи, чтобы не перенесся неотредактированный вариант
        //        find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->save_text_area();

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
            //            find_object<TreeScreen>(tree_screen_singleton_name)
            globalparameters.tree_screen()->know_branch()->setData(QModelIndex(), QVariant(false), Qt::UserRole);
        }
    }
}


ClipboardRecords *RecordView::getSelectedRecords(void)
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
    _record_controller->add_items_to_clipboard(clipboardRecords, itemsForCopy);

    return clipboardRecords;
}


// Переопределенный сигнал (virtual protected slot)
void RecordView::selectionChanged(const QItemSelection &selected,
                                  const QItemSelection &deselected)
{
    // qDebug() << "RecordTableView::selectionChanged()";

    emit listSelectionChanged(selected, deselected);

    // Для корректной работы надо вызвать сигнал базового класса
    QTableView::selectionChanged(selected, deselected);
}


// Слот, срабатывающий после того, как был передвинут горизонтальный заголовок
void RecordView::onSectionMoved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);

    if(!_enable_move_section)
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

    _enable_move_section = false;

    // Перемещение в данном представлении сбрасывается, так как модель берет последовательность полей из настроек
    // После это кода logicalIindex=visualIndex для всех полей
    for(int logicalIdx = 0; logicalIdx < showFields.size(); logicalIdx++) {
        int visualIdx = horizontalHeader()->visualIndex(logicalIdx);

        if(visualIdx != logicalIdx)
            horizontalHeader()->moveSection(visualIdx, logicalIdx); // Этот вызов запустит срабатывание этого же слота sectionMoved(), поэтому нужен enableMoveSection
    }

    _enable_move_section = true;

    horizontalHeader()->reset();

    // Устанавливается ширина столбцов после перемещения
    horizontalHeader()->resizeSection(oldVisualIndex, newVisualWidth);
    horizontalHeader()->resizeSection(newVisualIndex, oldVisualWidth);

    saveColumnWidth();
}


void RecordView::onSectionResized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);

    saveColumnWidth();
}


// Сохранение ширины колонок в конфигфайл
void RecordView::saveColumnWidth(void)
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
void RecordView::restoreColumnWidth(void)
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
void RecordView::resizeEvent(QResizeEvent *e)
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
