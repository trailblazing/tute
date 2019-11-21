#ifndef _CONFIGPAGE_MISC_H_
#define _CONFIGPAGE_MISC_H_

#include <QCheckBox>
#include <QGroupBox>
#include <QWidget>

#include "config_page.h"

#include <QtGlobal>
#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class AppConfigPageMisc : public ConfigPage {
#if QT_VERSION == 0x050600
	W_OBJECT(AppConfigPageMisc)
#else
	Q_OBJECT
#endif

	public:
	AppConfigPageMisc(QWidget *parent = 0);
	int apply_changes(void);

	private slots:

	protected:
	QCheckBox *cutBranchConfirm;
	QCheckBox *printDebugMessages;
	QCheckBox *runInMinimizedWindow;

	// Объединяющая рамка
	QGroupBox *historyBox;

	QCheckBox *rememberAtHistoryNavigationCheckBox;
	QCheckBox *rememberAtOrdinarySelectionCheckBox;
};

#endif  // _CONFIGPAGE_MISC_H_
