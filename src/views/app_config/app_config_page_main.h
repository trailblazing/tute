#ifndef _APPCONFIGPAGE_MAIN_H_
#define	_APPCONFIGPAGE_MAIN_H_

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>


class FlatToolButton;

#include "config_page.h"
#include "libraries/flat_control.h"


class AppConfigPageMain : public ConfigPage {
    Q_OBJECT

public:
    AppConfigPageMain(QWidget *parent = 0);
    ~AppConfigPageMain(void);

    void setup_ui(void);
    void setup_signals(void);
    void assembly(void);

    int apply_changes(void);

private slots:
    void open_tetradir_select_dialog(void);
    void open_trashdir_select_dialog(void);

    void onDisableCustomDateTimeFormatToggle(bool checked);
    void onEnableCustomDateTimeFormatToggle(bool checked);
    void onDateTimeFormatHelpButton(void);

private:

    QLabel *application_mode_label;
    MtComboBox *application_mode_option;

    QLabel *tetradirLabel;
    QLineEdit *tetradirInput;
    FlatToolButton *tetradirButton;

    QLabel *trashdirLabel;
    QLineEdit *trashdirInput;
    FlatToolButton *trashdirButton;

    QLabel *trashsizeLabel;
    QSpinBox  *trashsizeInput;
    QLabel *trashsizeFlexion;

    QLabel *trashmaxfilecountLabel;
    QSpinBox  *trashmaxfilecountInput;
    QLabel *trashmaxfilecountFlexion;

    QLabel *interfaceLanguageLabel;
    MtComboBox *interfaceLanguage;

    // Настройки отображения даты и времени
    QGroupBox *dateTimeFormatBox;
    QRadioButton *disableCustomDateTimeFormat;
    QRadioButton *enableCustomDateTimeFormat;
    QLineEdit *customDateTimeFormat;
    FlatToolButton *dateTimeFormatHelpButton;
};


#endif	// _APPCONFIGPAGE_MAIN_H_

