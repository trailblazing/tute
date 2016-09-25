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

    private:
        ConfigDialog *configDialog;

        QListWidgetItem *pageMain;
        QListWidgetItem *pageCrypt;
        QListWidgetItem *pageSynchro;
        QListWidgetItem *pageRecordTable;
        QListWidgetItem *pageMisc;
        rctrl_t		*_record_controller;
        void changePage(QString name);
        friend class XmlTree;
};

#endif	// _APPCONFIGDIALOG_H_
