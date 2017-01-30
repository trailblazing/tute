#ifndef _EDITORCONFIGMISC_H_
#define	_EDITORCONFIGMISC_H_


#include <QWidget>
#include <QLineEdit>
#include <QFontComboBox>
#include <QSpinBox>

#include "views/app_config/config_page.h"



#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif




class EditorConfig;


class EditorConfigMisc : public ConfigPage {
#if QT_VERSION == 0x050600
    W_OBJECT(EditorConfigMisc)
#else
    Q_OBJECT
#endif

public:
    EditorConfigMisc(QWidget *parent = 0);

    int apply_changes(void);

private slots:

private:
    QSpinBox *indentStep;

    EditorConfig *conf;

};


#endif	// _EDITORCONFIGMISC_H_

