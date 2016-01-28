#ifndef __ATTACHTABLECONTROLLER_H__
#define __ATTACHTABLECONTROLLER_H__

#include <QObject>
#include <QModelIndexList>

#include "models/attach_table/Attach.h"
#include "views/attach_table/AttachTableView.h"
#include "models/attach_table/AttachTableModel.h"
#include "models/attach_table/AttachTableData.h"

// class Attach;
// class AttachTableView;
// class AttachTableModel;
// class AttachTableData;

class AttachTableController : public QObject {
    Q_OBJECT

public:

    AttachTableController(QObject *parent = 0);
    virtual ~AttachTableController();

    AttachTableView *view(void);
    void attach_table_data(std::shared_ptr<AttachTableData> attach_table_data);
    AttachTableData *attach_table_data();

    QList<QString> selected_id(void);

public slots:

    void on_add_attach(void);
    void on_add_link(void);
    void on_edit_file_name(void);
    void on_delete_attach(void);
    void on_open_attach(void);
    void on_show_attach_info(void);
    void on_save_as_attach(void);

    void on_switch_to_editor(void);


protected:

    void add_smart(QString attach_type);
    QStringList select_files_for_adding(QString attach_type);
    void save_attach_to_user_place(QString from_full_file_name, QString to_full_file_name, QString attach_type, bool is_attach_crypt);

    AttachTableView     *_view;
    AttachTableModel    *_model;

};

#endif // __ATTACHTABLECONTROLLER_H__
