#ifndef _EDITORCONFIGDIALOG_H_
#define _EDITORCONFIGDIALOG_H_

#include "views/app_config/config_dialog.h"
#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class EditorConfigDialog : public QWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(EditorConfigDialog)
#else
	Q_OBJECT
#endif

	public:
	EditorConfigDialog(QWidget *parent = 0);

	private:
	ConfigDialog *configDialog;
};

#endif  // _EDITORCONFIGDIALOG_H_
