#ifndef _CONFIGPAGE_SYNCHRO_H_
#define	_CONFIGPAGE_SYNCHRO_H_



#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>

#include "config_page.h"


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif

class AppConfigPageSynchro : public ConfigPage {
#if QT_VERSION == 0x050600
    W_OBJECT(AppConfigPageSynchro)
#else
    Q_OBJECT
#endif

    public:
	AppConfigPageSynchro(QWidget *parent = 0);
	int apply_changes(void);

    private slots:

    protected:

	QLineEdit *synchroCommand;

	QCheckBox	*synchroOnStartup;
	QCheckBox	*synchroOnExit;

	// Объединяющая рамка
	QGroupBox *synchroOnBox;
};


#endif	// _CONFIGPAGE_SYNCHRO_H_

