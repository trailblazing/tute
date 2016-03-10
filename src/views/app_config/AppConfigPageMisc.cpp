#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>

#include "AppConfigPageMisc.h"
#include "models/app_config/AppConfig.h"

extern AppConfig appconfig;


AppConfigPageMisc::AppConfigPageMisc(QWidget *parent) : ConfigPage(parent)
{
    qDebug() << "Create misc config page";

    // Блок настройки подтверждения для действия "cut" на ветке
    cutBranchConfirm=new QCheckBox(this);
    cutBranchConfirm->setText(tr("Confirm item cut"));
    cutBranchConfirm->setChecked(appconfig.cut_branch_confirm());

    // Блок настройки отображения отладочных сообщений в консоли
    printDebugMessages=new QCheckBox(this);
    printDebugMessages->setText(tr("Print debug messages to console"));
    printDebugMessages->setChecked(appconfig.get_printdebugmessages());

    // Настройка запуска MyTetra в свернутом окне
    runInMinimizedWindow=new QCheckBox(this);
    runInMinimizedWindow->setText(tr("Run MyTetra in a minimized window"));
    runInMinimizedWindow->setChecked(appconfig.get_runinminimizedwindow());


    // Группировщик виджетов для настройки автоматического старта синхронизации
    historyBox=new QGroupBox(this);
    historyBox->setTitle(tr("History of visited notes"));

    rememberAtHistoryNavigationCheckBox=new QCheckBox(this);
    rememberAtHistoryNavigationCheckBox->setText(tr("Remember cursor position at history navigation"));
    rememberAtHistoryNavigationCheckBox->setChecked(appconfig.getRememberCursorAtHistoryNavigation());

    rememberAtOrdinarySelectionCheckBox=new QCheckBox(this);
    rememberAtOrdinarySelectionCheckBox->setText(tr("Try remember cursor position at ordinary selection"));
    rememberAtOrdinarySelectionCheckBox->setChecked(appconfig.getRememberCursorAtOrdinarySelection());

    // Виджеты вставляются в группировщик
    QVBoxLayout *historyLayout = new QVBoxLayout;
    historyLayout->addWidget(rememberAtHistoryNavigationCheckBox);
    historyLayout->addWidget(rememberAtOrdinarySelectionCheckBox);
    historyBox->setLayout(historyLayout);


    // Собирается основной слой
    QVBoxLayout *central_layout=new QVBoxLayout();
    central_layout->addWidget(cutBranchConfirm);
    central_layout->addWidget(printDebugMessages);
    central_layout->addWidget(runInMinimizedWindow);
    central_layout->addWidget(historyBox);
    central_layout->addStretch();

    // Основной слой устанавливается
    setLayout(central_layout);
}


// Метод должен возвращать уровень сложности сделанных изменений
// 0 - изменения не требуют перезапуска программы
// 1 - изменения требуют перезапуска программы
int AppConfigPageMisc::apply_changes(void)
{
    qDebug() << "Apply changes misc";

// Сохраняется настройка подтверждения для действия "cut" на ветке
    if(appconfig.cut_branch_confirm()!=cutBranchConfirm->isChecked())
        appconfig.cut_branch_confirm(cutBranchConfirm->isChecked());

// Сохраняется настройка отображения отладочных сообщений в консоли
    if(appconfig.get_printdebugmessages()!=printDebugMessages->isChecked())
        appconfig.set_printdebugmessages(printDebugMessages->isChecked());

// Сохраняется настройка режима запуска MyTetra - обычный или свернутый
    if(appconfig.get_runinminimizedwindow()!=runInMinimizedWindow->isChecked())
        appconfig.set_runinminimizedwindow(runInMinimizedWindow->isChecked());

// Сохраняется настройка нужно ли вспоминать позицию курсора при перемещении
// по истории
    if(appconfig.getRememberCursorAtHistoryNavigation()!=rememberAtHistoryNavigationCheckBox->isChecked())
        appconfig.setRememberCursorAtHistoryNavigation(rememberAtHistoryNavigationCheckBox->isChecked());

// Сохраняется настройка нужно ли пытаться вспоминать позицию курсора при
// обычном выборе записи
    if(appconfig.getRememberCursorAtOrdinarySelection()!=rememberAtOrdinarySelectionCheckBox->isChecked())
        appconfig.setRememberCursorAtOrdinarySelection(rememberAtOrdinarySelectionCheckBox->isChecked());

    return 0;
}
