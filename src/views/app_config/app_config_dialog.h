#ifndef _APPCONFIGDIALOG_H_
#define _APPCONFIGDIALOG_H_

#include <wobjectdefs.h>
#include <QObject>

#include <QWidget>

class ConfigDialog;
class QListWidgetItem;
class rctl_t;

class AppConfigDialog : public QWidget {
    W_OBJECT(AppConfigDialog)

    public:
	AppConfigDialog(rctl_t *_record_controller, QString firstPageName);

    private:
	ConfigDialog *configDialog;

	QListWidgetItem *pageMain;
	QListWidgetItem *pageCrypt;
	QListWidgetItem *pageSynchro;
	QListWidgetItem *pageRecordTable;
	QListWidgetItem *pageMisc;
	rctl_t		*_record_controller;
	void		changePage(QString name);
};

#endif	// _APPCONFIGDIALOG_H_
