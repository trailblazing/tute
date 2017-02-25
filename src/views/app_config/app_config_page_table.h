#ifndef APPCONFIGPAGE_RECORDTABLE_H
#define APPCONFIGPAGE_RECORDTABLE_H

#include <QCheckBox>
#include <QGroupBox>
#include <QMap>
#include <QWidget>

#include "config_page.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class rctrl_t;

class AppConfigPageTable : public ConfigPage {
#if QT_VERSION == 0x050600
W_OBJECT(AppConfigPageTable)
#else
Q_OBJECT
#endif

public:
	AppConfigPageTable(QWidget *parent = 0);//rctrl_t *_record_controller,
	int apply_changes(void);

signals:
	void record_table_config_change(void)
#if QT_VERSION == 0x050600
	W_SIGNAL(record_table_config_change) // ;	// Сигнал, испускающийся когда изменились настройки таблицы конечных записей
#else
	;
#endif
private slots: void on_field_toggle(bool);

protected:
	QMap<QString, QCheckBox *> _fields;

	QCheckBox *_show_horizontal_header;
	QCheckBox *_show_vertical_header;

	// Объединяющая рамка
	QGroupBox *_show_fields_box;
	QGroupBox *_show_headers_box;

	void setupSignals(void);

//	rctrl_t *_record_controller;
};

#endif // APPCONFIGPAGE_RECORDTABLE_H
