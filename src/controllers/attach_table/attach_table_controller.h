#ifndef __ATTACHTABLECONTROLLER_H__
#define __ATTACHTABLECONTROLLER_H__

#include <QModelIndexList>

#include "libraries/global_parameters.h"
#include "models/attach_table/attach.h"
#include "models/attach_table/attach_table_data.h"
#include "models/attach_table/attach_table_model.h"
#include "views/attach_table/attach_table_view.h"

#include <QtGlobal>
#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

// class Attach;
// class AttachTableView;
// class AttachTableModel;
// class AttachTableData;
class EditorWrap;
class AttachTableScreen;
class Blogger;

class AttachTableController : public QObject {
#if QT_VERSION == 0x050600
	W_OBJECT(AttachTableController)
#else
	Q_OBJECT
#endif

    public:
	AttachTableController(Blogger *blogger_, AttachTableScreen* parent = 0);
	virtual ~AttachTableController();

	AttachTableView* view(void);
	void attach_table_data(std::shared_ptr<AttachTableData> attach_table_data);
	AttachTableData* attach_table_data();

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

	AttachTableView* _view;
	AttachTableModel* _model;
	blogger_ref _blogger;
};

#endif // __ATTACHTABLECONTROLLER_H__
