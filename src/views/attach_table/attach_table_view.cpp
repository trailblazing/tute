
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QDebug>
#include <QHeaderView>
#include <QMenu>

#include "attach_table_screen.h"
#include "attach_table_view.h"
#include "controllers/attach_table/attach_table_controller.h"
#include "libraries/global_parameters.h"
#include "main.h"

extern gl_para gl_paras;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(AttachTableView)
#endif

AttachTableView::AttachTableView(QWidget* parent)
    : QTableView(parent)
{
    this->horizontalHeader()->setStretchLastSection(true);
    this->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделяется вся строка

    // Настройка области виджета для кинетической прокрутки
    set_kinetic_scrollarea(qobject_cast<QAbstractItemView*>(this));

    // Разрешение принимать жест QTapAndHoldGesture
    grabGesture(Qt::TapAndHoldGesture);
}

AttachTableView::~AttachTableView()
{
}

void AttachTableView::init(void)
{
    setupSignals();

    assemblyContextMenu();
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void AttachTableView::setupSignals(void)
{
    // Сигнал чтобы показать контекстное меню по правому клику на списке записей
    connect(this, &AttachTableView::customContextMenuRequested, this, &AttachTableView::onCustomContextMenuRequested);

    // Соединение сигнал-слот чтобы показать контекстное меню по долгому нажатию
    connect(this, &AttachTableView::tapAndHoldGestureFinished, this, &AttachTableView::onCustomContextMenuRequested);

    // Сигнал чтобы открыть на просмотр/редактирование файл по двойному клику
    connect(this, &AttachTableView::doubleClicked, controller, &AttachTableController::on_open_attach);
}

void AttachTableView::setController(AttachTableController* pController)
{
    controller = pController;
}

void AttachTableView::assemblyContextMenu(void)
{
    // Конструирование меню
    contextMenu = new QMenu(this);

    // find_object() не работает, потому что при инициализации еще не вызван метод assembly() у attachTableScreen, и этому объекту не задан родитель layout
    // AttachTableScreen *screenPointer=find_object<AttachTableScreen>("attachTableScreen");
    AttachTableScreen* screenPointer = qobject_cast<AttachTableScreen*>(controller->parent());

    contextMenu->addAction(screenPointer->actionAddAttach);
    contextMenu->addAction(screenPointer->actionEditFileName);
    contextMenu->addAction(screenPointer->actionDeleteAttach);
    contextMenu->addSeparator();
    contextMenu->addAction(screenPointer->actionOpenAttach);
    contextMenu->addAction(screenPointer->actionShowAttachInfo);
}

// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool AttachTableView::event(QEvent* event)
{
    if (event->type() == QEvent::Gesture) {
        qDebug() << "In gesture event(): " << event << " Event type: " << event->type();

        return gestureEvent(static_cast<QGestureEvent*>(event));
    }
    return QTableView::event(event);
}

void AttachTableView::resizeEvent(QResizeEvent* event)
{
    // Первый столбец должен занимать 80% ширины при любом размере таблицы
    QRect viewRect = this->rect();
    float columnWidth = (float)viewRect.width() * 0.8;
    this->setColumnWidth(0, columnWidth);

    // Отрисовка родительского класса
    QTableView::resizeEvent(event);
}

// Обработчик жестов
// Вызывается из обработчика событий event()
bool AttachTableView::gestureEvent(QGestureEvent* event)
{
    qDebug() << "In gestureEvent()" << event;
    if (QGesture* gesture = event->gesture(Qt::TapAndHoldGesture))
        tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture*>(gesture));
    return true;
}

// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов gestureEvent()
void AttachTableView::tapAndHoldGestureTriggered(QTapAndHoldGesture* gesture)
{
    qDebug() << "In tapAndHoldGestureTriggered()" << gesture;
    if (gesture->state() == Qt::GestureFinished)
        if (gl_paras.target_os() == "android")
            emit tapAndHoldGestureFinished(mapFromGlobal(gesture->position().toPoint()));
}

// Открытие контекстного меню в таблице приаттаченных файлов
void AttachTableView::onCustomContextMenuRequested(const QPoint& pos)
{
    // Включение отображения меню на экране
    contextMenu->exec(viewport()->mapToGlobal(pos));
}

// Получение номера первого выделенного элемента
int AttachTableView::getFirstSelectionPos(void)
{
    // Получение списка выделенных Item-элементов
    QModelIndexList selectItems = selectionModel()->selectedIndexes();
    if (selectItems.isEmpty())
        return -1; // Если ничего не выделено
    else
        return (selectItems.at(0)).row(); // Индекс первого выделенного элемента
}
