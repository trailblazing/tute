#include <QWidget>
#include <QBoxLayout>
#include <QLabel>

#include "AppConfigPageSynchro.h"
#include "models/app_config/AppConfig.h"

extern AppConfig appconfig;


AppConfigPageSynchro::AppConfigPageSynchro(QWidget *parent) : ConfigPage(parent)
{
    qDebug() << "Create synchro config page";

    synchroCommand=new QLineEdit(this);
    synchroCommand->setText(appconfig.get_synchrocommand());
    synchroCommand->setCursorPosition(0);


    synchroOnStartup=new QCheckBox(this);
    synchroOnStartup->setText(tr("Synchronize at MyTetra startup"));
    synchroOnStartup->setChecked(appconfig.get_synchroonstartup());

    synchroOnExit=new QCheckBox(this);
    synchroOnExit->setText(tr("Synchronize when exit from MyTetra"));
    synchroOnExit->setChecked(appconfig.get_synchroonexit());


    // Собирается основной слой
    QVBoxLayout *central_layout=new QVBoxLayout();

    // Область ввода команды синхронизации
    QLabel *commandText=new QLabel(this);
    commandText->setText(tr("Synchronization command"));

    QLabel *commandAboutText=new QLabel(this);
    commandAboutText->setText(tr("Use <b>%a</b> macro for get database directory path"));
    commandAboutText->setWordWrap(true);

    central_layout->addWidget(commandText);
    central_layout->addWidget(synchroCommand);
    central_layout->addWidget(commandAboutText);



    // Группировщик виджетов для настройки автоматического старта синхронизации
    synchroOnBox=new QGroupBox(this);
    synchroOnBox->setTitle(tr("Automatic start synchronization"));

    // Виджеты вставляются в группировщик
    QVBoxLayout *synchroOnLayout = new QVBoxLayout;
    synchroOnLayout->addWidget(synchroOnStartup);
    synchroOnLayout->addWidget(synchroOnExit);
    synchroOnBox->setLayout(synchroOnLayout);

    central_layout->addWidget(synchroOnBox);

    central_layout->addStretch();

    // Основной слой устанавливается
    setLayout(central_layout);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPageSynchro::apply_changes(void)
{
    qDebug() << "Apply changes synchro";

// Сохраняется строка с командой синхронизации
    if(appconfig.get_synchrocommand()!=synchroCommand->text())
        appconfig.set_synchrocommand(synchroCommand->text());

// Сохраняется настройка запуска синхронизации при старте
    if(appconfig.get_synchroonstartup()!=synchroOnStartup->isChecked())
        appconfig.set_synchroonstartup(synchroOnStartup->isChecked());

// Сохраняется настройка запуска синхронизации при выходе
    if(appconfig.get_synchroonexit()!=synchroOnExit->isChecked())
        appconfig.set_synchroonexit(synchroOnExit->isChecked());

    return 0;
}
