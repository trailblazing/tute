#ifndef _APPCONFIGDIALOG_H_
#define _APPCONFIGDIALOG_H_



#include <QWidget>


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif

class ConfigDialog;
class QListWidgetItem;
class rctrl_t;

class AppConfigDialog : public QWidget {
#if QT_VERSION == 0x050600
    W_OBJECT(AppConfigDialog)
#else
    Q_OBJECT
#endif

    public:
	AppConfigDialog(rctrl_t *_record_controller, QString firstPageName = "");
	void changePage(QString name);
    private:
	ConfigDialog	*configDialog;
	QListWidgetItem *pageMain;
	QListWidgetItem *pageCrypt;
	QListWidgetItem *pageSynchro;
	QListWidgetItem *pageRecordTable;
	QListWidgetItem *pageMisc;
	rctrl_t		*_record_controller;

//	friend class XmlTree;
//	friend class rctrl_t;
};

#endif	// _APPCONFIGDIALOG_H_
