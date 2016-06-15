#include <QString>

#include "main.h"
#include "WalkHistory.h"
#include "models/tree/KnowModel.h"
#include "views/tree/KnowView.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/TreeScreen.h"


extern GlobalParameters globalparameters;

WalkHistory::WalkHistory(void)
{
    clear();
}


WalkHistory::~WalkHistory(void)
{

}


void WalkHistory::clear(void)
{
    qDebug() << "WalkHistory::clear()";

    history_id.clear();
    _data.clear();

    _history_point = -1;
    _drop_flag = false;
}


void WalkHistory::add(QString _record_id
                      , int _cursor_position
                      , int _scrollbar_position
                      , int mode)
{
    // qDebug() << "WalkHistory::add() : id " << id;
    // qDebug() << "WalkHistory::add() : mode " << mode;
    // qDebug() << "WalkHistory::add() start status:";
    // print();

    if(_record_id.length() == 0)
        return;

    _data[_record_id]._cursor_position = _cursor_position;
    _data[_record_id]._scrollbar_position = _scrollbar_position;

    if(_drop_flag) {
        // qDebug() << "WalkHistory::add() : Dropping adding.";
        return;
    }

    // Если происходит просто запоминание без движения по истории
    if(mode == WALK_HISTORY_GO_NONE) {
        // Если указатель истории не указывает на конец истории (на вершину стека)
        if(_history_point != (history_id.length() - 1) && _history_point != (-1)) {
            // Значит указатель находится где-то в середине истории

            // Нужно удалить все старшие записи от указателя истории
            int deleteStart = _history_point + 1;
            int deleteEnd = history_id.length();
            int deleteCount = deleteEnd - deleteStart;

            for(int i = 0; i < deleteCount; i++)
                history_id.removeLast();
        }

        // Повторяющийся идентификатор не запоминается
        if(history_id.size() > 0)
            if(history_id.last() == _record_id) {
                // print();
                return;
            }

        // Идентификатор добавляется в историю
        history_id << _record_id;
        _history_point = history_id.length() - 1;

        // Если история слишком большая
        if(history_id.length() > WALK_HISTORY_MAX) {
            history_id.removeFirst();
            _history_point = history_id.length() - 1;
        }
    } // Закончилось условие что происходит запоминание без движения по истории


    // Если происходит запоминание с движением назад по истории
    if(mode == WALK_HISTORY_GO_PREVIOUS) {
        // Если история была пуста
        if(_history_point == -1) {
            // Идентификатор просто добавляется в историю
            history_id << _record_id;
            _history_point = 0;

            // print();
            return;
        }

        // Если указатель находится в конце истории
        if(_history_point == (history_id.length() - 1)) {
            // Если добавляемый идентификатор не равен последнему в истории
            // Его нужно добавить
            if(_record_id != history_id.last()) {
                // Идентификатор добавляется в историю
                history_id << _record_id;
                _history_point = history_id.length() - 1;

                // Если история слишком большая
                if(history_id.length() > WALK_HISTORY_MAX) {
                    history_id.removeFirst();
                    _history_point = history_id.length() - 1;
                }
            }

            // Происходит перемещение указателя назад
            if(_history_point > 0)
                _history_point--;
        } // Закрылось условие что указатель находился в конце истории
        else {
            // Иначе указатель находится где-то в середине истории

            if(_record_id != history_id.at(_history_point)) {
                // Невозможная ситуация. При правильной работе механизма выбора и кликов
                // по записям, она не может возникнуть. Но так как возможны баги в разных
                // версиях Qt, жесткой реации на получение такой ситуации делать нельзя
                QString message = "WARNING! Bad walk history state if move previous. History max index " + QString::number(history_id.length() - 1) + ", History point " + QString::number(_history_point) + ", History id " + history_id.at(_history_point) + ", Current id " + _record_id;
                qDebug() << message;

                check_id(_record_id);
                check_id(history_id.at(_history_point));

                // critical_error(message);
                // return;
            }

            // Происходит перемещение указателя назад
            if(_history_point > 0)
                _history_point--;
        }
    } // Закончилось условие что происходит запоминание с движением назад по истории


    // Если происходит запоминание с движением вперед по истории
    if(mode == WALK_HISTORY_GO_NEXT) {
        // Если история была пуста
        if(_history_point == -1) {
            // Идентификатор просто добавляется в историю
            history_id << _record_id;
            _history_point = 0;

            // print();
            return;
        }


        // Если указатель находится в конце истории
        if(_history_point == (history_id.length() - 1)) {

            // Если добавляемый идентификатор не равен последнему в истории
            // Его нужно добавить
            if(_record_id != history_id.last()) {
                // Идентификатор добавляется в историю
                history_id << _record_id;
                _history_point = history_id.length() - 1;

                // Если история слишком большая
                if(history_id.length() > WALK_HISTORY_MAX) {
                    history_id.removeFirst();
                    _history_point = history_id.length() - 1;
                }
            }

            // print();
            return;
        } else {
            // Иначе указатель находится где-то в середине истории

            if(_record_id != history_id.at(_history_point)) {
                // Невозможная ситуация. При правильной работе механизма выбора и кликов
                // по записям, она не может возникнуть. Но так как возможны баги в разных
                // версиях Qt, жесткой реации на получение такой ситуации делать нельзя
                QString message = "WARNING! Bad walk history state if move next. History max index " + QString::number(history_id.length() - 1) + ", History point " + QString::number(_history_point) + ", History id " + history_id.at(_history_point) + ", Current id " + _record_id;
                qDebug() << message;

                check_id(_record_id);
                check_id(history_id.at(_history_point));

                // critical_error(message);
                // return;
            }

            // Происходит перемещение указателя вперед
            if(_history_point < (history_id.length() - 1))
                _history_point++;
        }

    } // Закончилось условие что происходит запоминание с движением вперед по истории


    // print();
}


void WalkHistory::switch_to_previous(void)
{
    if(_history_point > 0)
        _history_point--;

    qDebug() << "WalkHistory::switchToPrevious() :";
    // print();
}


void WalkHistory::switch_to_next(void)
{
    if(_history_point < (history_id.length() - 1))
        _history_point++;

    qDebug() << "WalkHistory::switchToNext() :";
    // print();
}


QString WalkHistory::record_id()
{
    if(_history_point >= 0 && _history_point <= (history_id.length() - 1))
        return history_id[_history_point];
    else
        return QString();
}


int WalkHistory::cursor_position(QString id)
{
    if(_data.contains(id))
        return _data[id]._cursor_position;
    else
        return 0;
}


int WalkHistory::scrollbar_position(QString id)
{
    if(_data.contains(id))
        return _data[id]._scrollbar_position;
    else
        return 0;
}


void WalkHistory::remove_history_data(QString id)
{
    if(_data.contains(id))
        _data.remove(id);
}


void WalkHistory::print(void)
{
    qDebug() << "WalkHistory table ---v";
    qDebug() << "WalkHistory pointer: " << _history_point;

    for(int i = 0; i < history_id.size(); i++)
        qDebug() << "WalkHistory " << i << ":" << history_id.at(i);

    qDebug() << "WalkHistory table ---^";
}


void WalkHistory::set_drop(bool flag)
{
    _drop_flag = flag;
}


// Проверка наличия идентификатора в базе, и его исключение из истории, если
// идентификатор не обнаружен
void WalkHistory::check_id(QString id)
{
    // Выясняется ссылка на модель дерева данных
    KnowModel *dataModel = static_cast<KnowModel *>(    // find_object<TreeKnowView>("knowTreeView")
                                   globalparameters.tree_screen()->view()->model()
                               );

    // Если запись с указанным идентификатором существует
    if(dataModel->record_path(id).length() > 0)
        return; // Ничего делать не нужно


    // Здесь запись с указанным идентификатором не существует, и его надо
    // удалить из истории

    // Выясняется, сколько раз встречается несуществущий идентификатор в истории
    // и удаляется из истории
    int idRemoveCount = history_id.removeAll(id);

    // Происходит перемещение указателя назад на столько ячеек, сколько
    // было удалено идентификторов
    for(int i = 0; i < idRemoveCount; i++)
        if(_history_point > 0)
            _history_point--;
}

