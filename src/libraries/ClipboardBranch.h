#ifndef _CLIPBBRANCH_H_
#define _CLIPBBRANCH_H_

#include <QWidget>
#include <QMimeData>
#include <QMap>
#include <QByteArray>
#include <QMultiMap>
#include <QList>

#include "libraries/ClipboardRecords.h"

// Определяется структура данных
// которая будет передаваться через буфер обмена
struct CLIPB_BRANCH_STRUCT {
    // Список веток
    // Одна строка QList это набор нескольких элементов "ключ => значение"
    // например:
    // "branch_id" => "125"
    // "parent_id" => "100"
    // "name" => "Название ветки" и т.д.
    QList< QMap<QString, QString> >                         _branch;

    // Список записей
    //          +----------------------------- Membership of a branch (branch id)   // Принадлежность ветке (branch_id)
    //          |        +-------------------- record   // Запись
    //          |        |
    //          V        V
    QMultiMap< QString, boost::intrusive_ptr<TreeItem> >    _records;
};

Q_DECLARE_METATYPE(CLIPB_BRANCH_STRUCT);


struct CLIPB_TREE_ONE_LINE {
    QString     _branch_id;
    QStringList _subbranches_id;
};


class ClipboardBranch : public QMimeData {
    Q_OBJECT

public:
    ClipboardBranch(void);
    ~ClipboardBranch(void);

    void init(void);

    void branch_copy(boost::intrusive_ptr<TreeItem> it);

    void print(void) const;

    QList<CLIPB_TREE_ONE_LINE> id_tree_get(void) const;
    void id_tree_print(void) const;

    // Этот метод QMimeData надо переопределить, так как он виртуальный
    QStringList formats() const;

    // Получение полей для указанной ветки
    QMap<QString, QString> fields_by_branch_id(QString id) const;

    // Получение списка записей для указанной ветки
    QList<boost::intrusive_ptr<TreeItem> > records_by_branch_id(QString id) const;
    QString clip_root_item_parent_id() {return _clip_root_item_parent_id;}

protected:
    // Этот метод QMimeData надо переопределить, так как он виртуальный
    QVariant retrieveData(const QString &format, QVariant::Type preferredType) const;
    void branch_fields_add_parent_id(QString parent_id, QMap<QString, QString> _branch_fields);
    void record_duplicate_to_parent_id(QString parent_id, boost::intrusive_ptr<TreeItem> record);
private:
    QString             _clip_root_item_parent_id;
    CLIPB_BRANCH_STRUCT _branch_data; // Данные, которые передаются через буфер обмена
    QStringList         _clipboard_branch_format; // Строка с идентификатором формата

};


#endif // _CLIPBBRANCH_H_




