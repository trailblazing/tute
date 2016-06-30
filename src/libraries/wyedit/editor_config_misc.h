#ifndef _EDITORCONFIGMISC_H_
#define	_EDITORCONFIGMISC_H_


#include <wobjectdefs.h>
#include <QObject>

#include <QWidget>
#include <QLineEdit>
#include <QFontComboBox>
#include <QSpinBox>

#include "views/app_config/config_page.h"

class EditorConfig;


class EditorConfigMisc : public ConfigPage {
    W_OBJECT(EditorConfigMisc)

public:
    EditorConfigMisc(QWidget *parent = 0);

    int apply_changes(void);

private slots:

private:
    QSpinBox *indentStep;

    EditorConfig *conf;

};


#endif	// _EDITORCONFIGMISC_H_

