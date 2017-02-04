#ifndef _EDITORCONFIGDIALOG_H_
#define _EDITORCONFIGDIALOG_H_



#include <QWidget>
#include "views/app_config/config_dialog.h"


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif


class EditorConfigDialog : public QWidget {
#if QT_VERSION == 0x050600
    W_OBJECT(EditorConfigDialog)
#else
    Q_OBJECT
#endif

public:
    EditorConfigDialog(QWidget *parent=0);

private:
    ConfigDialog *configDialog;
};

#endif // _EDITORCONFIGDIALOG_H_
