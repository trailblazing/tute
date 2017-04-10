#ifndef _EDITORCONFIGMISC_H_
#define _EDITORCONFIGMISC_H_

#include <QFontComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>
#include <memory>

#include "views/app_config/config_page.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class EditorConfig;

class EditorConfigMisc : public ConfigPage {
#if QT_VERSION == 0x050600
	W_OBJECT(EditorConfigMisc)
#else
	Q_OBJECT
#endif

	public:
	EditorConfigMisc(std::shared_ptr<EditorConfig> editor_config_, QWidget *parent = 0);

	int apply_changes(void);

	private slots:

	private:
	QSpinBox *indentStep;

	std::shared_ptr<EditorConfig> _editor_config;
};

#endif  // _EDITORCONFIGMISC_H_
