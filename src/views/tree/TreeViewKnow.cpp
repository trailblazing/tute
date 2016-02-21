#include <QDebug>
#include <QMimeData>
#include <QAbstractItemModel>
#include <QMessageBox>

#include "main.h"
#include "TreeViewKnow.h"
#include "TreeScreen.h"
#include "libraries/ClipboardRecords.h"
#include "libraries/GlobalParameters.h"
#include "models/tree/TreeModelKnow.h"
#include "models/record_table/ItemsFlat.h"
#include "models/tree/TreeItem.h"
#include "models/record_table/RecordModel.h"
#include "views/main_window/MainWindow.h"
#include "views/record_table/RecordScreen.h"
#include "views/record/MetaEditor.h"
#include "controllers/record_table/RecordController.h"
#include "views/browser/webview.h"



extern GlobalParameters globalparameters;
const char *knowtreeview_singleton_name = "knowtreeview";

TreeViewKnow::TreeViewKnow(QWidget *parent) : QTreeView(parent)
{
    // Разрешение принимать Drop-события
    setAcceptDrops(true);
    setDropIndicatorShown(true);

    // Разрешение принимать жест QTapAndHoldGesture
    grabGesture(Qt::TapAndHoldGesture);

    // Настройка области виджета для кинетической прокрутки
    set_kinetic_scrollarea(qobject_cast<QAbstractItemView *>(this));
}


TreeViewKnow::~TreeViewKnow()
{

}


// Обработчик событий, нужен только для QTapAndHoldGesture (долгое нажатие)
bool TreeViewKnow::event(QEvent *event)
{
    if(event->type() == QEvent::Gesture) {
        qDebug() << "In gesture event(): " << event << " Event type: " << event->type();
        return gestureEvent(static_cast<QGestureEvent *>(event));
    }

    return QTreeView::event(event);
}


// Обработчик жестов
// Вызывается из обработчика событий
bool TreeViewKnow::gestureEvent(QGestureEvent *event)
{
    qDebug() << "In gestureEvent()" << event;

    if(QGesture *gesture = event->gesture(Qt::TapAndHoldGesture))
        tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(gesture));

    return true;
}


// Обработчик жеста TapAndHoldGesture
// Вызывается из обработчика жестов
void TreeViewKnow::tapAndHoldGestureTriggered(QTapAndHoldGesture *gesture)
{
    qDebug() << "In tapAndHoldGestureTriggered()" << gesture;

    if(gesture->state() == Qt::GestureFinished)
        if(globalparameters.target_os() == "android")
            emit tapAndHoldGestureFinished(mapFromGlobal(gesture->position().toPoint()));
}


void TreeViewKnow::dragEnterEvent(QDragEnterEvent *event)
{
    if(isDragableData(event)) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}


void TreeViewKnow::dragMoveEvent(QDragMoveEvent *event)
{
    if(isDragableData(event)) {
        event->acceptProposedAction();

        // Выясняется элемент дерева, над которым находится курсор
        QModelIndex index = indexAt(event->pos());

        // Указатель на родительский элемент, чтобы далее получить модель данных
        TreeScreen *parentPointer = qobject_cast<TreeScreen *>(parent());

        // В модели данных отмечается элемент дерева, над которым находится курсор
        parentPointer->_root_model->setData(index, QVariant(true), Qt::UserRole);
    } else
        event->ignore();
}


template <class X> bool TreeViewKnow::isDragableData(X *event)
{
    // Проверяется, содержит ли объект переноса данные нужного формата
    const QMimeData *mimeData = event->mimeData();

    if(mimeData == nullptr)
        return false;

    if(!(mimeData->hasFormat("mytetra/records")))
        return false;

    QObject *sourceObject = qobject_cast<QObject *>(event->source());

    if(sourceObject->objectName() == "recordTableView")
        return true;
    else
        return false;
}


void TreeViewKnow::dropEvent(QDropEvent *event)
{
    qDebug() << "dropEvent() - Start";

    if(isDragableData(event)) {
        qDebug() << "Try move record by drag and drop";

        // Извлечение объекта
        const ClipboardRecords *clipboardRecords;
        clipboardRecords = qobject_cast<const ClipboardRecords *>(event->mimeData());

        // Печать в консоль содержимого перетаскиваемого объекта (для отладки)
        clipboardRecords->print();

        // Выясняется элемент дерева, над которым был сделан Drop
        QModelIndex index = indexAt(event->pos());

        // Если отпускание мышки произошло не на ветке дерева, а на свободном пустом месте в области виджета дерева
        if(!index.isValid())
            return;

        // Указатель на родительский элемент
        TreeScreen *parentPointer = qobject_cast<TreeScreen *>(parent());

        // Выясняется ссылка на элемент дерева (на ветку), над которым был совершен Drop
        auto tree_item_drop = parentPointer->_root_model->item(index);

        // Выясняется ссылка на таблицу данных ветки, над которой совершен Drop
        auto items_flat = tree_item_drop;    // ->record_table();

        // Исходная ветка в момент Drop (откуда переностся запись) - это выделенная курсором ветка
        QModelIndex indexFrom = find_object<TreeScreen>(tree_screen_singleton_name)->current_index();

        // Выясняется ссылка на элемент дерева (на ветку), откуда переностся запись
        auto treeItemDrag = parentPointer->_root_model->item(indexFrom);

        // Если перенос происходит в ту же самую ветку
        if(indexFrom == index)
            return;

        // Если перенос происходит из не зашифрованной ветки в зашифрованную, а пароль не установлен
        if(treeItemDrag->field("crypt") != "1" &&
           tree_item_drop->field("crypt") == "1" &&
           globalparameters.crypt_key().length() == 0) {
            // Выводится уведомление что невозможен перенос без пароля
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Warning!"));
            msgBox.setText(tr("Cant move this item to encrypt item. Please open crypt item (entry password) before."));
            msgBox.setIcon(QMessageBox::Information);
            msgBox.exec();

            return;
        }


        // Перенос записей, хранящихся в MimeData
        // В настоящий момент в MimeData попадает только одна запись,
        // но в дальнейшем планируется переносить несколько записей
        // и здесь код подготовлен для переноса нескольких записей
        RecordController *_record_controller = treeItemDrag->unique_page()->record_controller(); // find_object<RecordController>("table_screen_controller"); // Указатель на контроллер таблицы конечных записей

        for(int i = 0; i < clipboardRecords->size(); i++) {
            // Полные данные записи
            boost::intrusive_ptr<TreeItem> record = clipboardRecords->record(i);

            // Удаление записи из исходной ветки, удаление должно быть вначале, чтобы сохранился ID записи
            // В этот момент вид таблицы конечных записей показывает таблицу, из которой совершается Drag
            // TreeItem *treeItemFrom=parentPointer->knowTreeModel->getItem(indexFrom);
            _record_controller->remove_child(record->field("id"));

            // Если таблица конечных записей после удаления перемещенной записи стала пустой
            if(_record_controller->row_count() == 0)
                find_object<MetaEditor>(meta_editor_singleton_name)->clear_all(); // Нужно очистить поле редактирования чтобы не видно было текста последней удаленной записи

            //            find_object<RecordScreen>(table_screen_singleton_name)
            _record_controller->record_screen()->tools_update();

            // Добавление записи в базу
            items_flat->insert_new_item(0, record, ADD_NEW_RECORD_TO_END);

            // Сохранение дерева веток
            find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();
        }

        // Обновление исходной ветки чтобы было видно что записей убавилось
        parentPointer->update_branch_on_screen(indexFrom);

        // Обновлении конечной ветки чтобы было видно что записей прибавилось
        parentPointer->update_branch_on_screen(index);

        // В модели данных обнуляется элемент, который подсвечивался при Drag And Drop
        parentPointer->_root_model->setData(QModelIndex(), QVariant(false), Qt::UserRole);
    }
}

