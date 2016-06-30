#ifndef _CONFIGPAGE_MISC_H_
#define	_CONFIGPAGE_MISC_H_


#include <wobjectdefs.h>
#include <QObject>

#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>

#include "config_page.h"


class AppConfigPageMisc : public ConfigPage {
    W_OBJECT(AppConfigPageMisc)

    public:
	AppConfigPageMisc(QWidget *parent = 0);
	int apply_changes(void);

    private slots:

    protected:

	QCheckBox	*cutBranchConfirm;
	QCheckBox	*printDebugMessages;
	QCheckBox	*runInMinimizedWindow;

	// Объединяющая рамка
	QGroupBox *historyBox;

	QCheckBox	*rememberAtHistoryNavigationCheckBox;
	QCheckBox	*rememberAtOrdinarySelectionCheckBox;
};


#endif	// _CONFIGPAGE_MISC_H_

