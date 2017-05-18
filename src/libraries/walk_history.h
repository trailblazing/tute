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
	void add(const id_value& _record_id, int _cursor_position, int _scrollbar_position); // , int mode = WALK_HISTORY_GO_NONE

	// QString tree_root_id();
	id_value record_at_iterator() const;
	int cursor_position(const id_value& id);
	int scrollbar_position(const id_value& id);
	void remove_history_data(const id_value& id);
	void drop_flag(bool flag);

	bool drop_flag() const;

	void append(const id_value& _record_id);

    protected:
	bool _drop_flag = false;

	void print(void);

	//	// Эти методы видимо ненужны, подумать
	//	void switch_to_previous(void);
	//	void switch_to_next(void);

	void clear_invalid_from_tree(const id_value& id);

	// The pointer in the list of records visited history, usually indicates the last entry in the list.
	// But if there is a movement backward-forward in the history, it is accordingly shifted
	int _history_iterator = -1;

	// Pointer to the point in the history in which the movement began
	// If the user has moved back, and then forward, then when the "_history_iterator" reaches this point,
	// the "_history_iterator" pointer immediately moves to the last record, and "_leave_mark_point" becomes unused
	// (set to -1)
	int _leave_mark_point = -1;

	// Список идентификаторов посещаемых записей
	QStringList _history_ids;

	// Для каждого идентификатора запоминается позиция курсора и прокрутки
	QMap<id_value, WALK_HISTORY_DATA> _data;

	// QString _tree_root_id = "";
};

template <>
void WalkHistory::add<WALK_HISTORY_GO_NONE>(const id_value& _record_id, int _cursor_position, int _scrollbar_position);
template <>
void WalkHistory::add<WALK_HISTORY_GO_PREVIOUS>(const id_value& _record_id, int _cursor_position, int _scrollbar_position);
template <>
void WalkHistory::add<WALK_HISTORY_GO_NEXT>(const id_value& _record_id, int _cursor_position, int _scrollbar_position);

#endif // _WALKHISTORY_H_
