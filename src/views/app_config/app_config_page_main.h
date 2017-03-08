#ifndef _APPCONFIGPAGE_MAIN_H_
#define _APPCONFIGPAGE_MAIN_H_

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

extern const char *standardItem;
extern const char *portableItem;

class FlatToolButton;

#include "config_page.h"
#include "libraries/flat_control.h"

class AppConfigPageMain : public ConfigPage {
#if QT_VERSION == 0x050600
	W_OBJECT(AppConfigPageMain)
#else
	Q_OBJECT
#endif

	public:
	AppConfigPageMain(QWidget *parent = 0);
	~AppConfigPageMain(void);

	void setup_ui(void);
	void setup_signals(void);
	void assembly(void);

	int apply_changes(void);

	static QString get_root_path(const QString &initia_path);
	private slots:
	void open_rootdir_select_dialog(void);
	void open_datadir_select_dialog(void);
	void open_trashdir_select_dialog(void);

	void on_disable_custom_datetime_format_toggle(bool checked);
	void on_enable_custom_datetime_format_toggle(bool checked);
	void on_datetime_format_help_button(void);

	private:
	QLabel *_application_current_path_label;
	// MtComboBox	*_application_mode_option;

	QLabel *_rootdir_label;
	QLineEdit *_rootdir_input;
	FlatToolButton *_rootdir_button;

	QLabel *_datadir_label;
	QLineEdit *_datadir_input;
	FlatToolButton *_datadir_button;

	QLabel *_trashdir_label;
	QLineEdit *_trashdir_input;
	FlatToolButton *_trashdir_button;

	QLabel *_trashsize_label;
	QSpinBox *_trashsize_input;
	QLabel *_trashsize_flexion;

	QLabel *_trashmaxfilecount_label;
	QSpinBox *_trashmaxfilecount_input;
	QLabel *_trashmaxfilecount_flexion;

	QLabel *_interface_language_label;
	FlatComboBox *_interface_language;

	// Настройки отображения даты и времени
	QGroupBox *_datetime_format_box;
	QRadioButton *_disable_custom_datetime_format;
	QRadioButton *_enable_custom_datetime_format;
	QLineEdit *custom_datetime_format;
	FlatToolButton *_datetime_format_help_button;
	std::tuple<bool, QString> _original_root_state{true, "./"};
};

#endif  // _APPCONFIGPAGE_MAIN_H_
