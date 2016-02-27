#ifndef APPCONFIGPAGE_RECORDTABLE_H
#define APPCONFIGPAGE_RECORDTABLE_H

#include <QWidget>
#include <QCheckBox>
#include <QMap>
#include <QGroupBox>

#include "ConfigPage.h"
class  RecordController;

class AppConfigPageTable : public ConfigPage {
    Q_OBJECT

public:
    AppConfigPageTable(RecordController *_record_controller, QWidget *parent = 0);
    int apply_changes(void);

signals:
    void recordTableConfigChange(void); // Сигнал, испускающийся когда изменились настройки таблицы конечных записей

private slots:
    void onFieldToggle(bool);

protected:

    QMap<QString, QCheckBox *> fields;

    QCheckBox *showHorizontalHeader;
    QCheckBox *showVerticalHeader;

    // Объединяющая рамка
    QGroupBox *showFieldsBox;
    QGroupBox *showHeadersBox;

    void setupSignals(void);

    RecordController *_record_controller;
};


#endif // APPCONFIGPAGE_RECORDTABLE_H
