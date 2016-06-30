#ifndef _EDITORCONFIGDIALOG_H_
#define _EDITORCONFIGDIALOG_H_

#include <wobjectdefs.h>
#include <QObject>

#include <QWidget>
#include "views/app_config/config_dialog.h"

class EditorConfigDialog : public QWidget {
    W_OBJECT(EditorConfigDialog)

public:
    EditorConfigDialog(QWidget *parent=0);

private:
    ConfigDialog *configDialog;
};

#endif // _EDITORCONFIGDIALOG_H_
