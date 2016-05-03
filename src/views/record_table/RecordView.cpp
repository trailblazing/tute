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
#include "models/tree/KnowModel.h"
#include "libraries/GlobalParameters.h"
#include "views/main_window/MainWindow.h"
#include "libraries/WindowSwitcher.h"
#include "controllers/record_table/RecordController.h"
#include "models/record_table/ItemsFlat.h"
#include "views/tree/KnowView.h"



extern GlobalParameters globalparameters;
extern AppConfig appconfig;
extern const char *record_view_multi_instance_name;
extern enum QItemSelectionModel::SelectionFlag current_tree_selection_mode;
extern enum QItemSelectionModel::SelectionFlag current_tree_current_index_mode;

// Виджет, отображащий список записей в ветке


RecordView::RecordView(RecordScreen *_record_screen, RecordController *_record_controller)
    : QTableView(_record_screen)
    , _context_menu(new QMenu(this))
    , _record_screen(_record_screen)
    , _record_controller(_record_controller)
    , _layout(new QVBoxLayout(this))
{
    setObjectName(record_view_multi_instance_name);   // screen_name + "_view"

    // Изначально сортировка запрещена (заголовки столбцов не будут иметь треугольнички)
    this->setSortingEnabled(false);

    // Настройка области виджета для кинетической прокрутки
    set_kinetic_scrollarea(qobject_cast<QAbstractItemView *>(this));

    // Разрешение принимать жест QTapAndHoldGesture
    grabGesture(Qt::TapAndHoldGesture);

    _layout->setSpacing(0);

    _layout->setMargin(0);

    //    viewport()->getContentsMargins();

    init();
}


RecordView::~RecordView()
{
    // delete
    _context_menu->deleteLater();
    // delete
    _layout->deleteLater();
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

    setup_signals();

    setSelectionMode(QAbstractItemView::ExtendedSelection); // // It was previously Extended Selection, but this mode is not suitable for Drag and Drop // Ранее было ExtendedSelection, но такой режим не подходит для Drag and Drop   // SingleSelection  // MultiSelection //ExtendedSelection


    setSelectionBehavior(QAbstractItemView::SelectRows);

    restore_header_state();

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
    int height = appconfig.ugly_qss_replace_height_for_table_view();

    if(height != 0)
        verticalHeader()->setDefaultSectionSize(height);

    if(appconfig.interface_mode() == "mobile")
        verticalHeader()->setDefaultSectionSize(calculate_iconsize_px());

    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);   // setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // ScrollBarAsNeeded  //ScrollBarAlwaysOff

    restore_column_width();

    // Разрешается перемещать секции заголовка таблицы
    _enable_move_section = true;

    // Нужно установить правила показа контекстного самодельного меню
    // чтобы оно могло вызываться
    assembly_context_menu();
    setContextMenuPolicy(Qt::CustomContextMenu);


}


void RecordView::setup_signals(void)
{
    // Сигнал чтобы показать контекстное меню по правому клику на списке записей
    connect(this, &RecordView::customContextMenuRequested, this, &RecordView::on_custom_context_menu_requested);

    // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
    connect(this, &RecordView::tap_and_hold_gesture_finished, this, &RecordView::on_custom_context_menu_requested);

    // Сигнал чтобы открыть на редактирование параметры записи при двойном клике
    // Signal to open for editing the parameters of the recording double click
    connect(this, &RecordView::doubleClicked, this, &RecordView::on_doubleclick);

    if(appconfig.interface_mode() == "desktop")
        connect(this, &RecordView::list_selection_changed, this, &RecordView::on_selection_changed);

    // Для мобильного режима должен работать сигнал clicked, так как если засветка уже стоит на строке с записью, должна открыться запись
    // а в десктопном режиме этого не должно происходить, потому что запись уже видна на экране
    //    if(appconfig.getInterfaceMode() == "mobile")
    connect(this, &RecordView::clicked, this, &RecordView::on_click_to_record);

    RecordScreen *parentPointer = qobject_cast<RecordScreen *>(parent());

    // Сигналы для обновления панели инструментов при изменении в selectionModel()
    connect(this->selectionModel(), &QItemSelectionModel::currentChanged, parentPointer, &RecordScreen::tools_update);
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, parentPointer, &RecordScreen::tools_update);

    // Сигналы для обновления панели инструментов
    connect(this, &RecordView::activated, parentPointer, &RecordScreen::tools_update);
    connect(this, &RecordView::clicked, parentPointer, &RecordScreen::tools_update);
    connect(this, &RecordView::doubleClicked, parentPointer, &RecordScreen::tools_update);
    connect(this, &RecordView::entered, parentPointer, &RecordScreen::tools_update);
    connect(this, &RecordView::pressed, parentPointer, &RecordScreen::tools_update);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, parentPointer, &RecordScreen::tools_update);

    connect(this->horizontalHeader(), &QHeaderView::sectionMoved, this, &RecordView::on_section_moved);
    connect(this->horizontalHeader(), &QHeaderView::sectionResized, this, &RecordView::on_section_resized);
}


void RecordView::restore_header_state(void)
{
    // Видимость горизонтальных заголовков
    if(appconfig.record_table_show_horizontal_headers() == false)
        horizontalHeader()->hide();
    else
        horizontalHeader()->show();

    // Видимость вертикальных заголовков
    if(appconfig.record_table_show_vertical_headers() == false)
        verticalHeader()->hide();
    else
        verticalHeader()->show();
}


void RecordView::on_selection_changed(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndex select_record_index;
    QModelIndex deselect_record_index;

    if(!selected.indexes().isEmpty())
        select_record_index = selected.indexes().at(0);

    if(!deselected.indexes().isEmpty())
        deselect_record_index = deselected.indexes().at(0);


    //    qDebug() << "RecordTableView::onSelectionChanged()";
    //    qDebug() << "Current index. row() " << selectRecord.row() << " isValid() " << selectRecord.isValid();
    //    qDebug() << "Previous index. row() " << deselectRecord.row() << " isValid() " << deselectRecord.isValid();


    // return;

    //    if(select_record_index.isValid())
    //        click_record(select_record_index);    // this lead RecordScreen::disable_all_actions does not work correctly
}


// Слот клика по записи. Принимает индекс Proxy модели
void RecordView::on_click_to_record(const QModelIndex &proxy_index)
{
    click_record(proxy_index);
}


// Действия при выборе строки таблицы конечных записей. Принимает индекс Proxy модели
// Actions when choosing the final row of the table entries. Accepts index Proxy models
void RecordView::click_record(const QModelIndex &proxy_index)
{
    _record_controller->item_click(proxy_index);

    globalparameters.window_switcher()->switchFromRecordtableToRecord();
}


void RecordView::assembly_context_menu(void)
{
    //    // Конструирование меню
    //    _context_menu = new QMenu(this);

    //    RecordTableScreen *_recordtablescreen = qobject_cast<RecordTableScreen *>(parent());

    _context_menu->addAction(_record_screen->_pin);
    _context_menu->addAction(_record_screen->_addnew_to_end);
    _context_menu->addAction(_record_screen->_edit_field);
    _context_menu->addAction(_record_screen->_close);


    _context_menu->addSeparator();

    _context_menu->addAction(_record_screen->_addnew_before);
    _context_menu->addAction(_record_screen->_addnew_after);
    _context_menu->addAction(_record_screen->_action_move_up);
    _context_menu->addAction(_record_screen->_action_move_dn);
    _context_menu->addAction(_record_screen->_action_syncro);

    _context_menu->addSeparator();

    _context_menu->addAction(_record_screen->_cut);
    _context_menu->addAction(_record_screen->_copy);
    _context_menu->addAction(_record_screen->_paste);
    _context_menu->addAction(_record_screen->_delete);

    _context_menu->addSeparator();

    _context_menu->addAction(_record_screen->_sort);
    _context_menu->addAction(_record_screen->_print);
    _context_menu->addAction(_record_screen->_settings);
}


// Открытие контекстного меню в таблице конечных записей
void RecordView::on_custom_context_menu_requested(const QPoint &pos)
{
    qDebug() << "In on_customContextMenuRequested";
    //    auto p = parent();

    //    RecordTableScreen *parentPointer = qobject_cast<RecordTableScreen *>(parent());
    //    RecordTableScreen *parentPointer = static_cast<RecordTableScreen *>(p);
    //    RecordTableScreen *_recordtablescreen = globalparameters.getRecordTableScreen();

    // Устанавливается надпись для пункта сортировки
    if(!this->isSortingEnabled())
        _record_screen->_sort->setText(tr("Enable sorting"));
    else
        _record_screen->_sort->setText(tr("Disable sorting"));

    // Запоминается номер колонки, по которой был произведен клик (номер колонки будет правильный, даже если записей мало и клик произошел под записями)
    int n = this->horizontalHeader()->logicalIndexAt(pos);
    qDebug() << "Click on column number " << n;
    _record_screen->_sort->setData(n);   // Запоминается в объект действия для сортировки


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
void RecordView::edit_field_context(void)
{
    qDebug() << "In RecordTableView::editFieldContext";

    // Получение индекса выделенного элемента
    QModelIndexList selectItems = selectionModel()->selectedIndexes();
    QModelIndex proxy_index;

    for(auto &si : selectItems) {
        if(0 == si.column())
            proxy_index = si;   // selectItems.at(0);
    }

    auto current_index = currentIndex();

    //    auto _index0 = _record_controller->proxyindex_to_sourceindex(proxy_index);
    //    auto _index1 = _record_controller->sourceindex_to_proxyindex(proxy_index);  // invalid

    //    proxy_index = _record_controller->pos_to_proxyindex(proxy_index.row());
    assert(proxy_index.isValid());

    assert(proxy_index.row() == current_index.row());

    if(_record_controller->edit_field_context(proxy_index)) {   // proxy_index
        // Нужно перерисовать окно редактирования чтобы обновились инфополя
        // делается это путем "повторного" выбора текущего пункта
        click_record(proxy_index);  // proxy_index // Раньше было select()
    }
}

// Получение номера первого выделенного элемента
int RecordView::selection_first_pos(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectItems = selectionModel()->selectedIndexes();

    if(selectItems.isEmpty())
        return -1; // Если ничего не выделено
    else
        return (selectItems.at(0)).row(); // Номер первого выделенного элемента
}


// Получение ID первого выделенного элемента
QString RecordView::selection_first_id(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectItems = selectionModel()->selectedIndexes();

    if(selectItems.isEmpty())
        return ""; // Если ничего не выделено

    return selectItems.at(0).data(RECORD_ID_ROLE).toString();
}


// Получение модельного индекса первого выделенного элемента в Proxy модели
QModelIndex RecordView::selection_first_proxy_index(void)
{
    int pos = selection_first_pos();

    if(pos == -1)
        return QModelIndex();

    // QModelIndex index = recordProxyModel->index( pos, 0 );
    QModelIndex index = _record_controller->proxypos_to_proxyindex(pos);

    return index;
}


// Получение модельного индекса первого выделенного элемента в Source модели
QModelIndex RecordView::selection_first_source_index(void)
{
    QModelIndex proxy_index = selection_first_proxy_index();

    if(!proxy_index.isValid())
        return QModelIndex();

    // QModelIndex index = recordProxyModel->mapToSource( proxyIndex );
    QModelIndex index = _record_controller->proxyindex_to_sourceindex(proxy_index);

    return index;
}


bool RecordView::is_selected_set_to_top(void)
{
    if(selection_first_pos() == 0)return true;
    else return false;
}


bool RecordView::is_selected_set_to_bottom(void)
{
    if(selection_first_pos() == model()->rowCount() - 1)
        return true;
    else
        return false;
}

void RecordView::cursor_to_index(boost::intrusive_ptr<TreeItem> it)
{
    auto pos = _record_controller->source_model()->position(it);
    cursor_to_index(pos);
}

// Установка засветки в нужную строку на экране
void RecordView::cursor_to_index(int pos)
{
    int proxy_pos = _record_controller->sourcepos_to_proxypos(pos);
    QModelIndex index = _record_controller->proxypos_to_proxyindex(proxy_pos); // Модельный индекс в Proxy модели
    int pos_real = index.row();

    // todo: Если это условие ни разу не сработает, значит преобразование ipos - pos надо просто убрать
    if(pos_real != pos) {
        QMessageBox msgBox;
        msgBox.setText("In RecordView::cursor_to_index() input pos not equal model pos");
        msgBox.exec();
    }

    int rowCount = _record_controller->row_count();

    if(pos_real < rowCount) {   // if(pos_real > (rowCount - 1))return;


        // Простой механизм выбора строки. Похоже, что его использовать не получится
        selectRow(pos_real);

        //    auto recordSourceModel = controller->getRecordTableModel();
        //    QModelIndex selIdx = recordSourceModel->index(pos, 0);

        selectionModel()->select(index, current_tree_selection_mode);
        // Установка засветки на нужный индекс
        // Set the backlight to the desired index
        selectionModel()->setCurrentIndex(index   // selIdx
                                          , current_tree_current_index_mode // QItemSelectionModel::Select    // ClearAndSelect
                                         );

        // В мобильной версии реакции на выбор записи нет (не обрабатывается сигнал смены строки в модели выбора)
        // Поэтому по записи должен быть сделан виртуальный клик, чтобы заполнилась таблица конечных записей
        // In response to the mobile version of the record is no choice (not processed signal line change to the selection model)
        // Therefore, the recording must be made a virtual click to fill the final table of records
        if(appconfig.interface_mode() == "mobile")
            emit this->clicked(index); // QModelIndex selIdx=recordSourceModel->index(pos, 0);

        // emit this->clicked(index);

        scrollTo(currentIndex());   // QAbstractItemView::PositionAtCenter

        //    this->setFocus();   // ?
    }
}


// mode - режим в котором добавлялась новая запись
// pos - позиция новой записи в размерности Source модели
void RecordView::move_cursor_to_new_record(int mode, int pos)
{
    // Установка курсора на только что созданную позицию

    //    QModelIndex selIdx = recordSourceModel->index(selPos, 0); // Создание индекса из номера
    //    selectionModel()->setCurrentIndex(selIdx, QItemSelectionModel::ClearAndSelect);


    // В QTableView некорректно работает установка на только что созданную строку
    // Это как-то связано с отрисовкой виджета QTableView
    // Прокрутка к только что созданной строке через selectRow() показывает только
    // верхнюю часть новой строки. Чтобы этого избежать, при добавлении в конец
    // таблицы конечных записей, установка прокрутки делается через scrollToBottom()
    if(mode == add_new_record_to_end
       || (mode == add_new_record_after && pos >= (model()->rowCount() - 1))
      ) {
        scrollToBottom();
    }

    int proxy_pos = _record_controller->proxypos_to_proxyindex(pos).row();

    selectRow(proxy_pos);


    QModelIndex index = _record_controller->proxypos_to_proxyindex(pos);
    selectionModel()->select(index, current_tree_selection_mode);
    // Установка засветки на нужный индекс
    // Set the backlight to the desired index
    selectionModel()->setCurrentIndex(index   // selIdx
                                      , current_tree_current_index_mode // QItemSelectionModel::Select    // ClearAndSelect
                                     );

}



// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool RecordView::event(QEvent *event)
{
    if(event->type() == QEvent::Gesture) {
        qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
        return gesture_event(static_cast<QGestureEvent *>(event));
    }

    return QTableView::event(event);
}


// Обработчик жестов
// Вызывается из обработчика событий
bool RecordView::gesture_event(QGestureEvent *event)
{
    qDebug() << "In gestureEvent()" << event;

    if(QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
        tap_and_hold_gesture_triggered(static_cast<QTapAndHoldGesture *>(gesture));

    return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void RecordView::tap_and_hold_gesture_triggered(QTapAndHoldGesture *gesture)
{
    qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

    if(gesture->state() == Qt::GestureFinished)
        if(globalparameters.target_os() == "android")
            emit tap_and_hold_gesture_finished(mapFromGlobal(gesture->position().toPoint()));
}


// Реакция на нажатие кнопок мышки
void RecordView::mousePressEvent(QMouseEvent *event)
{

    TreeScreen *_tree_screen = globalparameters.tree_screen();  // static_cast<TreeScreen *>(this->parent());
    // get the buttons type
    Qt::MouseButtons mouse_button = event->buttons();

    _mouse_start_position = event->pos();
    QModelIndex next_index = indexAt(event->pos());

    // Если нажата левая кнопка мыши
    if(mouse_button == Qt::LeftButton) {    // || mouse_button == Qt::RightButton

        selectionModel()->select(next_index, QItemSelectionModel::SelectCurrent);
        //        auto it = _record_controller->source_model()->item(next_index.row());
        //        _tree_screen->tree_view()->select_as_current(it);

    } else if(mouse_button == Qt::RightButton) {    // only the right mouse buton
        //        _mouse_start_position = event->pos();
        //        //select item at cursor position
        //        //        QPersistentModelIndex
        //        QModelIndex next_index = indexAt(event->pos());
        //        selectionModel()->clear();
        selectionModel()->select(next_index, QItemSelectionModel::ClearAndSelect);    // Select
        selectionModel()->setCurrentIndex(next_index, QItemSelectionModel::SelectCurrent);
        assert(next_index == currentIndex());
        _record_controller->select_pos(next_index.row());   //
        emit clicked(next_index);




        //        // start the context menu
        //        QModelIndexList select_indexes(selectedIndexes());

        //        if(select_indexes.size() > 0 && select_indexes[0].isValid()) {
        //            _tree_screen->_context_menu->exec(event->pos());  // QCursor::pos()
        //        }

    }

    auto it = _record_controller->source_model()->item(next_index.row());
    auto tree_view = _tree_screen->tree_view();
    tree_view->select_as_current(TreeModel::ModelIndex([&] {return tree_view->source_model();}, it->parent(), it->parent()->sibling_order([&](boost::intrusive_ptr<const TreeItem::Linker> il) {return il == it->linker() && il->host() == it && it->parent() == il->host_parent();})));

    //    else {
    //        //call the parents function
    //        QTableView::mousePressEvent(event);
    //    }

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
            start_drag();
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
void RecordView::start_drag()
{
    qDebug() << "Start record drag\n";

    // Если действительно выбрана строка
    if(currentIndex().isValid()) {
        // Перед переносом нужно запомнить текст последней редактируемой записи, чтобы не перенесся неотредактированный вариант
        //        find_object<MainWindow>("mainwindow")
        globalparameters.mainwindow()->save_text_area();

        // Копирование выделенных строк в объект переноса
        QDrag *drag = new QDrag(this);
        drag->setMimeData(get_selected_records());

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
            globalparameters.tree_screen()->tree_view()->source_model()->setData(QModelIndex(), QVariant(false), Qt::UserRole);
        }
    }
}


ClipboardRecords *RecordView::get_selected_records(void)
{
    // Получение списка Item-элементов, подлежащих копированию
    QModelIndexList indexes_for_copy = selectionModel()->selectedIndexes();

    // В списке должны остаться только элементы столбца 0
    // (так как ранее одна строка была одним элементом списка,
    // а теперь используется таблица, и при одной выделенной строке
    // выделено несколько элементов)
    // The list should only elements of the column 0
    // (As earlier one line was one element of the list,
    // And now uses a table, and a dedicated line
    // Select multiple items)
    QMutableListIterator<QModelIndex> iterator(indexes_for_copy);

    while(iterator.hasNext()) {
        iterator.next();

        QModelIndex index = iterator.value();

        if(index.column() > 0)
            iterator.remove();
    }


    // Список возвращается в произвольном порядке, не в таком как на экране
    // поэтому его нужно отсортировать по QModelIndex
    qSort(indexes_for_copy.begin(), indexes_for_copy.end());

    qDebug() << "Get selected records";

    for(int i = 0; i < indexes_for_copy.size(); ++i)
        qDebug() << indexes_for_copy.at(i).data().toString();


    // Объект с данными для заполнения буфера обмена
    ClipboardRecords *clipboardRecords = new ClipboardRecords();
    clipboardRecords->clear();

    // Объект заполняется выбранными записями
    _record_controller->add_items_to_clipboard(clipboardRecords, indexes_for_copy);

    return clipboardRecords;
}


// Переопределенный сигнал (virtual protected slot)
void RecordView::selectionChanged(const QItemSelection &selected,
                                  const QItemSelection &deselected)
{
    // qDebug() << "RecordTableView::selectionChanged()";

    emit list_selection_changed(selected, deselected);

    // Для корректной работы надо вызвать сигнал базового класса
    QTableView::selectionChanged(selected, deselected);
}


// Слот, срабатывающий после того, как был передвинут горизонтальный заголовок
void RecordView::on_section_moved(int logicalIndex, int oldVisualIndex, int newVisualIndex)
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
    QStringList showFields = appconfig.record_table_show_fields();
    showFields.move(oldVisualIndex, newVisualIndex);
    appconfig.record_table_show_fields(showFields);

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

    save_column_width();
}


void RecordView::on_section_resized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(logicalIndex);
    Q_UNUSED(oldSize);
    Q_UNUSED(newSize);

    save_column_width();
}


// Сохранение ширины колонок в конфигфайл
void RecordView::save_column_width(void)
{
    // Выясняется количество полей
    int count = appconfig.record_table_show_fields().size();

    QStringList columnWidthList;

    for(int i = 0; i < count; i++) {
        QString width;
        width.setNum(columnWidth(i));
        columnWidthList << width;
    }

    appconfig.record_table_fields_width(columnWidthList);

    // qDebug() << "Save column width " << columnWidthList;
}


// Восстановление ширины колонок из конфигфайла
void RecordView::restore_column_width(void)
{
    QStringList columnWidthList = appconfig.record_table_fields_width();

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
    int adjust_width = 60;
    //    auto size_width = size().width();

    if((columnWidth(0) + columnWidth(1)) >= real_width) {
        setColumnWidth(0, 30);
        setColumnWidth(1, (real_width >= adjust_width) ? real_width - adjust_width : columnWidth(1));
    } else {
        setColumnWidth(0, 30);
        real_width = this->contentsRect().width();

        if(real_width >= 300) {
            setColumnWidth(1, 300 - adjust_width); // restoreColumnWidth();
        } else {
            setColumnWidth(1, (real_width >= adjust_width) ? real_width - adjust_width : columnWidth(1));
        }
    }

    QTableView::resizeEvent(e);
}
