#ifndef _WALKHISTORY_H_
#define _WALKHISTORY_H_

#include <QDebug>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtGlobal>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

#define WALK_HISTORY_MAX 100

#define WALK_HISTORY_GO_NONE 0
#define WALK_HISTORY_GO_PREVIOUS 1
#define WALK_HISTORY_GO_NEXT 2

struct WALK_HISTORY_DATA {
	int _cursor_position;
	int _scrollbar_position;
};

struct id_value;

class WalkHistory : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(WalkHistory)
#else
	Q_OBJECT
#endif

	public:
	WalkHistory(void);
	~WalkHistory(void);

	void clear(void);

	template <int direction>
	void add(const id_value &_record_id, int _cursor_position,
	         int _scrollbar_position);  // , int mode = WALK_HISTORY_GO_NONE

	// QString tree_root_id();
	id_value record_id() const;
	int cursor_position(const id_value &id);
	int scrollbar_position(const id_value &id);
	void remove_history_data(const id_value &id);
	void set_drop(bool flag);

	protected:
	bool _drop_flag;

	void print(void);

	// Эти методы видимо ненужны, подумать
	void switch_to_previous(void);
	void switch_to_next(void);

	void check_id(const id_value &id);

	// Указатель в списке истории посещения записей
	// Обычно указывает на последнюю в списке запись
	int _history_point;

	// Список идентификаторов посещаемых записей
	QStringList history_id;

	// Для каждого идентификатора запоминается позиция курсора и прокрутки
	QMap<id_value, WALK_HISTORY_DATA> _data;

	// QString _tree_root_id = "";
};

template <>
void WalkHistory::add<WALK_HISTORY_GO_NONE>(const id_value &_record_id, int _cursor_position, int _scrollbar_position);
template <>
void WalkHistory::add<WALK_HISTORY_GO_PREVIOUS>(const id_value &_record_id, int _cursor_position, int _scrollbar_position);
template <>
void WalkHistory::add<WALK_HISTORY_GO_NEXT>(const id_value &_record_id, int _cursor_position, int _scrollbar_position);

#endif  // _WALKHISTORY_H_
