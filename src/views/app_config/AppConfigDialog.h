#ifndef _APPCONFIGDIALOG_H_
#define _APPCONFIGDIALOG_H_

#include <QWidget>

class ConfigDialog;
class QListWidgetItem;
class RecordController;

class AppConfigDialog : public QWidget {
    Q_OBJECT

public:
    AppConfigDialog(RecordController *_record_controller, QString firstPageName);

private:
    ConfigDialog *configDialog;

    QListWidgetItem *pageMain;
    QListWidgetItem *pageCrypt;
    QListWidgetItem *pageSynchro;
    QListWidgetItem *pageRecordTable;
    QListWidgetItem *pageMisc;
    RecordController *_record_controller;
    void changePage(QString name);
};

#endif // _APPCONFIGDIALOG_H_
