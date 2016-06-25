#ifndef _CONFIGPAGE_SYNCHRO_H_
#define	_CONFIGPAGE_SYNCHRO_H_

#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>

#include "config_page.h"


class AppConfigPageSynchro : public ConfigPage {
    Q_OBJECT

public:
    AppConfigPageSynchro(QWidget *parent = 0);
    int apply_changes(void);

private slots:

protected:

    QLineEdit *synchroCommand;

    QCheckBox *synchroOnStartup;
    QCheckBox *synchroOnExit;

    // Объединяющая рамка
    QGroupBox *synchroOnBox;
};


#endif	// _CONFIGPAGE_SYNCHRO_H_

