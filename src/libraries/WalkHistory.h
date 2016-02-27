#ifndef _WALKHISTORY_H_
#define _WALKHISTORY_H_

#include <QtGlobal>
#include <QObject>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QMap>

#define WALK_HISTORY_MAX 100

#define WALK_HISTORY_GO_NONE     0
#define WALK_HISTORY_GO_PREVIOUS 1
#define WALK_HISTORY_GO_NEXT     2

struct WALK_HISTORY_DATA {
    int _cursor_position;
    int _scrollbar_position;
};


class WalkHistory : public QObject {
    Q_OBJECT

public:
    WalkHistory(void);
    ~WalkHistory(void);

    void clear(void);

    void add(QString _record_id, int _cursor_position, int _scrollbar_position, int mode = WALK_HISTORY_GO_NONE);

    //    QString tree_root_id();
    QString record_id();
    int cursor_position(QString id);
    int scrollbar_position(QString id);
    void remove_history_data(QString id);

    void set_drop(bool flag);

protected:

    bool _drop_flag;

    void print(void);

    // Эти методы видимо ненужны, подумать
    void switch_to_previous(void);
    void switch_to_next(void);

    void check_id(QString id);

    // Указатель в списке истории посещения записей
    // Обычно указывает на последнюю в списке запись
    int _history_point;

    // Список идентификаторов посещаемых записей
    QStringList history_id;

    // Для каждого идентификатора запоминается позиция курсора и прокрутки
    QMap<QString, WALK_HISTORY_DATA> _data;

    //    QString _tree_root_id = "";
};

#endif // _WALKHISTORY_H_
