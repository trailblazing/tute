
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QString>

#include "libraries/global_parameters.h"
#include "main.h"
#include "models/tree/tree_know_model.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"
#include "walk_history.h"

extern std::shared_ptr<gl_para> gl_paras;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(WalkHistory)
#endif

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

	_history_ids.clear();
	_data.clear();

	_history_iterator = -1;
	_leave_mark_point = -1;
	_drop_flag = false;
}

void WalkHistory::append(const id_value& _record_id)
{
	// Duplicate identifier is not remembered // Повторяющийся идентификатор не запоминается
	if (_history_ids.size() > 0) {
		if (_history_ids.last() != _record_id) { // return; // print();

			// The identifier is added to the story // Идентификатор добавляется в историю
			_history_ids << _record_id;
			// If the story is too big // Если история слишком большая
			if (_history_ids.length() > WALK_HISTORY_MAX) {
				_history_ids.removeFirst();
				if (_history_iterator > 0)
					_history_iterator--;
				if (_leave_mark_point > 0)
					_leave_mark_point--;
			}

			// }	// If there is a memorization with the movement backward in the history // Если происходит запоминание с движением назад по истории
		}
	} else {
		_history_ids << _record_id;
	}
	_history_iterator = _history_ids.length() - 1;
}

template <>
void WalkHistory::add<WALK_HISTORY_GO_NONE>(const id_value& _record_id, int _cursor_position, int _scrollbar_position)
{
	if (static_cast<QString>(_record_id).length() > 0) { // return;
		_data[_record_id]._cursor_position = _cursor_position;
		_data[_record_id]._scrollbar_position = _scrollbar_position;
		if (!_drop_flag
		    // || ((_history_ids.size() > 0) ? (_history_ids.last() != _record_id) : false) //  _record_id != record_id()
		    ) { //
			// qDebug() << "WalkHistory::add() : Dropping adding.";
			// return;	}
			// If it is just a memorization without moving through history // Если происходит просто запоминание без движения по истории
			// if(mode == WALK_HISTORY_GO_NONE){
			// If the history pointer does not point to the end of the story (to the top of the stack) // Если указатель истории не указывает на конец истории (на вершину стека)
			if (_history_iterator != (_history_ids.length() - 1) && _history_iterator != (-1)) {
				// Значит указатель находится где-то в середине истории
				// You need to delete all older records from the history index // Нужно удалить все старшие записи от указателя истории
				int deleteStart = _history_iterator + 1;
				int deleteEnd = _history_ids.length();
				int deleteCount = deleteEnd - deleteStart;
				for (int i = 0; i < deleteCount; i++)
					_history_ids.removeLast();
			}
			append(_record_id);
			_leave_mark_point = -1;
		}
	}
}

template <>
void WalkHistory::add<WALK_HISTORY_GO_PREVIOUS>(const id_value& _record_id, int _cursor_position, int _scrollbar_position)
{
	if (static_cast<QString>(_record_id).length() > 0) { // return;
		_data[_record_id]._cursor_position = _cursor_position;
		_data[_record_id]._scrollbar_position = _scrollbar_position;
		if (!_drop_flag) {
			// qDebug() << "WalkHistory::add() : Dropping adding.";
			// return;		}
			// if(mode == WALK_HISTORY_GO_PREVIOUS){
			// If the story was empty // Если история была пуста
			if (_history_iterator != -1) {
				// If the pointer is not at the end of the story // Если указатель находится в конце истории
				if (                                                   //0 < _history_iterator &&
				    _history_iterator < (_history_ids.length() - 1)) { //

					// The pointer is somewhere in the middle of the story // Иначе указатель находится где-то в середине истории
					if (_record_id != _history_ids.at(_history_iterator)) {
						// Невозможная ситуация. При правильной работе механизма выбора и кликов по записям, она не может возникнуть. Но так как возможны баги в разных версиях Qt, жесткой реации на получение такой ситуации делать нельзя
						// Impossible situation. If the selection mechanism works properly and clicks on records, it can not occur. But since bugs are possible in different versions of Qt, you can not do a hard reaction to receiving such a situation
						QString message =
						    "WARNING! Bad walk history state if move previous. "
						    "History max index " +
						    QString::number(_history_ids.length() - 1) +
						    ", History point " + QString::number(_history_iterator) +
						    ", History id " + _history_ids.at(_history_iterator) +
						    ", Current id " + _record_id;
						qDebug() << message;

						clear_invalid_from_tree(_record_id);
						clear_invalid_from_tree(id_value(_history_ids.at(_history_iterator)));

						// critical_error(message); // return;
					}
					//		// The pointer moves backward // Происходит перемещение указателя назад
					//		if (_history_point > 0)
					//					_history_iterator--;// switch_to_previous();
				} else {
					_leave_mark_point = _history_iterator;
					// The condition that the pointer was at the end of the story was closed // Закрылось условие что указатель находился в конце истории
					// Если добавляемый идентификатор не равен последнему в истории
					// Его нужно добавить
					append(_record_id);
					//		// Происходит перемещение указателя назад
					//		if (_history_point > 0)
					//			_history_point--;
				}
				// }		// The condition has ended that memorization with movement back occurs // Закончилось условие что происходит запоминание с движением назад
				// On history // по истории
				if (_history_iterator > 0)
					_history_iterator--;
			} else {

				append(_record_id);
				//				// The identifier is simply added to the story // Идентификатор просто добавляется в историю
				//				_history_ids << _record_id;
				if (_history_iterator != 0) _history_iterator = 0;

				//				// print(); return;
			}
		}
	}
}

template <>
void WalkHistory::add<WALK_HISTORY_GO_NEXT>(const id_value& _record_id, int _cursor_position, int _scrollbar_position)
{
	if (static_cast<QString>(_record_id).length() > 0) { // return;
		_data[_record_id]._cursor_position = _cursor_position;
		_data[_record_id]._scrollbar_position = _scrollbar_position;
		if (!_drop_flag) {
			// qDebug() << "WalkHistory::add() : Dropping adding.";
			// return;		}
			// Если происходит запоминание с движением вперед по истории
			// if(mode == WALK_HISTORY_GO_NEXT){
			// If the story was empty // Если история была пуста
			if (_history_iterator != -1) {
				// // If the pointer is not at the end of the story // Если указатель находится в конце истории
				if (_history_iterator < (_history_ids.length() - 1)) {
					// The pointer is somewhere in the middle of the story // Иначе указатель находится где-то в середине истории
					if (_record_id != _history_ids.at(_history_iterator)) {
						// Невозможная ситуация. При правильной работе механизма выбора и кликов по записям, она не может возникнуть. Но так как возможны баги в разных версиях Qt, жесткой реации на получение такой ситуации делать нельзя
						// Impossible situation. If the selection mechanism works properly and clicks on records, it can not occur. But since bugs are possible in different versions of Qt, you can not do a hard reaction to receiving such a situation
						QString message =
						    "WARNING! Bad walk history state if move next. History max index " +
						    QString::number(_history_ids.length() - 1) + ", History point " +
						    QString::number(_history_iterator) + ", History id " +
						    _history_ids.at(_history_iterator) + ", Current id " + _record_id;
						qDebug() << message;

						clear_invalid_from_tree(_record_id);
						clear_invalid_from_tree(id_value(_history_ids.at(_history_iterator)));

						// critical_error(message); // return;
					}

					//                                        switch_to_next(); //		// The pointer moves forward // Происходит перемещение указателя вперед
					//					if (_history_iterator < (_history_ids.length() - 1))
					_history_iterator++;
					if (_history_iterator > _leave_mark_point) {
						_history_iterator = _history_ids.length() - 1;
						_leave_mark_point = -1;
					}
				} else {

					// Если добавляемый идентификатор не равен последнему в истории
					// Его нужно добавить
					append(_record_id);
					// print(); return;
				}
				// }		// The condition that there is a memorization with movement forward has come to the end // Закончилось условие что происходит запоминание с движением вперед
				// On history // по истории
			} else {
				append(_record_id);
				//				// // The identifier is simply added to the story  // Идентификатор просто добавляется в историю
				//				_history_ids << _record_id;
				if (_history_iterator != 0) _history_iterator = 0;

				//				// print(); return;
			}
		}
	}
}

// void WalkHistory::add(const id_value &_record_id
// , int _cursor_position
// , int _scrollbar_position
// , int mode){
//// qDebug() << "WalkHistory::add() : id " << id;
//// qDebug() << "WalkHistory::add() : mode " << mode;
//// qDebug() << "WalkHistory::add() start status:";
//// print();
// if(static_cast<QString>(_record_id).length() == 0)return;
// _data[_record_id]._cursor_position		= _cursor_position;
// _data[_record_id]._scrollbar_position	= _scrollbar_position;
// if(_drop_flag){
//// qDebug() << "WalkHistory::add() : Dropping adding.";
// return;
// }
//// Если происходит просто запоминание без движения по истории
// if(mode == WALK_HISTORY_GO_NONE){
//// Если указатель истории не указывает на конец истории (на вершину стека)
// if(_history_point != (history_id.length() - 1) && _history_point != (- 1)){
//// Значит указатель находится где-то в середине истории

//// Нужно удалить все старшие записи от указателя истории
// int deleteStart	= _history_point + 1;
// int deleteEnd	= history_id.length();
// int deleteCount	= deleteEnd - deleteStart;
// for(int i = 0; i < deleteCount; i ++)history_id.removeLast();
// }
//// Повторяющийся идентификатор не запоминается
// if(history_id.size() > 0)
// if(history_id.last() == _record_id)return;	// print();

//// Идентификатор добавляется в историю
// history_id << _record_id;
// _history_point = history_id.length() - 1;
//// Если история слишком большая
// if(history_id.length() > WALK_HISTORY_MAX){
// history_id.removeFirst();
// _history_point = history_id.length() - 1;
// }
// }	// Закончилось условие что происходит запоминание без движения по
// истории
//// Если происходит запоминание с движением назад по истории
// if(mode == WALK_HISTORY_GO_PREVIOUS){
//// Если история была пуста
// if(_history_point == - 1){
//// Идентификатор просто добавляется в историю
// history_id << _record_id;
// _history_point = 0;

//// print();
// return;
// }
//// Если указатель находится в конце истории
// if(_history_point == (history_id.length() - 1)){
//// Если добавляемый идентификатор не равен последнему в истории
//// Его нужно добавить
// if(_record_id != history_id.last()){
//// Идентификатор добавляется в историю
// history_id << _record_id;
// _history_point = history_id.length() - 1;
//// Если история слишком большая
// if(history_id.length() > WALK_HISTORY_MAX){
// history_id.removeFirst();
// _history_point = history_id.length() - 1;
// }
// }
//// Происходит перемещение указателя назад
// if(_history_point > 0)_history_point --;
// }	// Закрылось условие что указатель находился в конце истории
// else{
//// Иначе указатель находится где-то в середине истории
// if(_record_id != history_id.at(_history_point)){
//// Невозможная ситуация. При правильной работе механизма выбора и кликов
//// по записям, она не может возникнуть. Но так как возможны баги в разных
//// версиях Qt, жесткой реации на получение такой ситуации делать нельзя
// QString message = "WARNING! Bad walk history state if move previous. History
// max index " + QString::number(history_id.length() - 1) + ", History point " +
// QString::number(_history_point) + ", History id " +
// history_id.at(_history_point) + ", Current id " + _record_id;
// qDebug() << message;

// check_id(_record_id);
// check_id(id_value(history_id.at(_history_point)));

//// critical_error(message);
//// return;
// }
//// Происходит перемещение указателя назад
// if(_history_point > 0)_history_point --;
// }
// }	// Закончилось условие что происходит запоминание с движением назад по
// истории
//// Если происходит запоминание с движением вперед по истории
// if(mode == WALK_HISTORY_GO_NEXT){
//// Если история была пуста
// if(_history_point == - 1){
//// Идентификатор просто добавляется в историю
// history_id << _record_id;
// _history_point = 0;

//// print();
// return;
// }
//// Если указатель находится в конце истории
// if(_history_point == (history_id.length() - 1)){
//// Если добавляемый идентификатор не равен последнему в истории
//// Его нужно добавить
// if(_record_id != history_id.last()){
//// Идентификатор добавляется в историю
// history_id << _record_id;
// _history_point = history_id.length() - 1;
//// Если история слишком большая
// if(history_id.length() > WALK_HISTORY_MAX){
// history_id.removeFirst();
// _history_point = history_id.length() - 1;
// }
// }
//// print();
// return;
// }else{
//// Иначе указатель находится где-то в середине истории
// if(_record_id != history_id.at(_history_point)){
//// Невозможная ситуация. При правильной работе механизма выбора и кликов
//// по записям, она не может возникнуть. Но так как возможны баги в разных
//// версиях Qt, жесткой реации на получение такой ситуации делать нельзя
// QString message = "WARNING! Bad walk history state if move next. History max
// index " + QString::number(history_id.length() - 1) + ", History point " +
// QString::number(_history_point) + ", History id " +
// history_id.at(_history_point) + ", Current id " + _record_id;
// qDebug() << message;

// check_id(_record_id);
// check_id(id_value(history_id.at(_history_point)));

//// critical_error(message);
//// return;
// }
//// Происходит перемещение указателя вперед
// if(_history_point < (history_id.length() - 1))_history_point ++;
// }
// }	// Закончилось условие что происходит запоминание с движением вперед по
// истории

//// print();
// }

//void WalkHistory::switch_to_previous(void)
//{
//	if (_history_iterator > 0)
//		_history_iterator--;
//	qDebug() << "WalkHistory::switchToPrevious() :";
//	// print();
//}

//void WalkHistory::switch_to_next(void)
//{
//	if (_history_iterator < (_history_ids.length() - 1))
//		_history_iterator++;
//	qDebug() << "WalkHistory::switchToNext() :";
//	// print();
//}

id_value WalkHistory::record_at_iterator() const
{
	if (_history_iterator >= 0 && _history_iterator <= (_history_ids.length() - 1))
		return id_value(_history_ids[_history_iterator]);
	else
		return id_value(QString());
}

int WalkHistory::cursor_position(const id_value& id)
{
	if (_data.contains(id))
		return _data[id]._cursor_position;
	else
		return 0;
}

int WalkHistory::scrollbar_position(const id_value& id)
{
	if (_data.contains(id))
		return _data[id]._scrollbar_position;
	else
		return 0;
}

void WalkHistory::remove_history_data(const id_value& id)
{
	if (_data.contains(id))
		_data.remove(id);
}

void WalkHistory::print(void)
{
	qDebug() << "WalkHistory table ---v";
	qDebug() << "WalkHistory pointer: " << _history_iterator;
	for (int i = 0; i < _history_ids.size(); i++)
		qDebug() << "WalkHistory " << i << ":" << _history_ids.at(i);
	qDebug() << "WalkHistory table ---^";
}

bool WalkHistory::drop_flag() const
{
	return _drop_flag;
}


void WalkHistory::drop_flag(bool flag)
{
	if (flag != _drop_flag) _drop_flag = flag;
}

// Проверка наличия идентификатора в базе, и его исключение из истории, если
// идентификатор не обнаружен
void WalkHistory::clear_invalid_from_tree(const id_value& id)
{
	// Выясняется ссылка на модель дерева данных
	tkm_t* dataModel =
	    static_cast<tkm_t*>( // find_object<TreeKnowView>("knowTreeView")
		gl_paras->tree_screen()->view()->model());
	// Если запись с указанным идентификатором существует
	if (dataModel->path_list(id).length() <= 0) { // return; // Ничего делать не нужно

		// Здесь запись с указанным идентификатором не существует, и его надо
		// удалить из истории

		// Выясняется, сколько раз встречается несуществущий идентификатор в истории
		// и удаляется из истории
		int idRemoveCount = _history_ids.removeAll(id);
		// Происходит перемещение указателя назад на столько ячеек, сколько
		// было удалено идентификторов
		for (int i = 0; i < idRemoveCount; i++) {
			if (_history_iterator > 0) {
				_history_iterator--;
			}
		}
	}
}
