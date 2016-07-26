
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>

#include "app_config_page_misc.h"
#include "models/app_config/app_config.h"

extern AppConfig appconfig;


#if QT_VERSION == 0x050600
W_OBJECT_IMPL(AppConfigPageMisc)
#endif


AppConfigPageMisc::AppConfigPageMisc(QWidget *parent) : ConfigPage(parent){
    qDebug() << "Create misc config page";

	// Блок настройки подтверждения для действия "cut" на ветке
    cutBranchConfirm = new QCheckBox(this);
    cutBranchConfirm->setText(tr("Confirm item cut"));
    cutBranchConfirm->setChecked(appconfig.cut_branch_confirm());

	// Блок настройки отображения отладочных сообщений в консоли
    printDebugMessages = new QCheckBox(this);
    printDebugMessages->setText(tr("Print debug messages to console"));
    printDebugMessages->setChecked(appconfig.print_debug_messages());

	// Настройка запуска MyTetra в свернутом окне
    runInMinimizedWindow = new QCheckBox(this);
    runInMinimizedWindow->setText(tr("Run MyTetra in a minimized window"));
    runInMinimizedWindow->setChecked(appconfig.run_in_minimized_window());


	// Группировщик виджетов для настройки автоматического старта синхронизации
    historyBox = new QGroupBox(this);
    historyBox->setTitle(tr("History of visited notes"));

    rememberAtHistoryNavigationCheckBox = new QCheckBox(this);
    rememberAtHistoryNavigationCheckBox->setText(tr("Remember cursor position at history navigation"));
    rememberAtHistoryNavigationCheckBox->setChecked(appconfig.remember_cursor_at_history_navigation());

    rememberAtOrdinarySelectionCheckBox = new QCheckBox(this);
    rememberAtOrdinarySelectionCheckBox->setText(tr("Try remember cursor position at ordinary selection"));
    rememberAtOrdinarySelectionCheckBox->setChecked(appconfig.remember_cursor_at_ordinary_selection());

	// Виджеты вставляются в группировщик
    QVBoxLayout *historyLayout = new QVBoxLayout;
    historyLayout->addWidget(rememberAtHistoryNavigationCheckBox);
    historyLayout->addWidget(rememberAtOrdinarySelectionCheckBox);
    historyBox->setLayout(historyLayout);


	// Собирается основной слой
    QVBoxLayout *central_layout = new QVBoxLayout();
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
int AppConfigPageMisc::apply_changes(void){
    qDebug() << "Apply changes misc";
// Сохраняется настройка подтверждения для действия "cut" на ветке
    if(appconfig.cut_branch_confirm() != cutBranchConfirm->isChecked())appconfig.cut_branch_confirm(cutBranchConfirm->isChecked());
// Сохраняется настройка отображения отладочных сообщений в консоли
    if(appconfig.print_debug_messages() != printDebugMessages->isChecked())appconfig.print_debug_messages(printDebugMessages->isChecked());
// Сохраняется настройка режима запуска MyTetra - обычный или свернутый
    if(appconfig.run_in_minimized_window() != runInMinimizedWindow->isChecked())appconfig.run_in_minimized_window(runInMinimizedWindow->isChecked());
// Сохраняется настройка нужно ли вспоминать позицию курсора при перемещении
// по истории
    if(appconfig.remember_cursor_at_history_navigation() != rememberAtHistoryNavigationCheckBox->isChecked())appconfig.remember_cursor_at_history_navigation(rememberAtHistoryNavigationCheckBox->isChecked());
// Сохраняется настройка нужно ли пытаться вспоминать позицию курсора при
// обычном выборе записи
    if(appconfig.remember_cursor_at_ordinary_selection() != rememberAtOrdinarySelectionCheckBox->isChecked())appconfig.remember_cursor_at_ordinary_selection(rememberAtOrdinarySelectionCheckBox->isChecked());
    return 0;
}

