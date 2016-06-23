#ifndef _APPCONFIGDIALOG_H_
#define _APPCONFIGDIALOG_H_

#include <QWidget>

class ConfigDialog;
class QListWidgetItem;
class rctl_t;

class AppConfigDialog : public QWidget {
    Q_OBJECT

public:
    AppConfigDialog(rctl_t *_record_controller, QString firstPageName);

private:
    ConfigDialog *configDialog;

    QListWidgetItem *pageMain;
    QListWidgetItem *pageCrypt;
    QListWidgetItem *pageSynchro;
    QListWidgetItem *pageRecordTable;
    QListWidgetItem *pageMisc;
    rctl_t *_record_controller;
    void changePage(QString name);
};

#endif // _APPCONFIGDIALOG_H_
