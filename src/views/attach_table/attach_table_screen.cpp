#include <wobjectimpl.h>
#include "main.h"
#include <QToolButton>
#include "attach_table_screen.h"
#include "attach_table_view.h"
#include "controllers/attach_table/attach_table_controller.h"
#include "libraries/global_parameters.h"

extern GlobalParameters globalparameters;
W_OBJECT_IMPL(AttachTableScreen)
AttachTableScreen::AttachTableScreen(QWidget *parent) : QWidget(parent){
	// По факту этот класс - синглтон. Синглтон сам задает себе имя
    this->setObjectName("attachTableScreen");

	// Создаются действия. Они используются как в данном классе (на кнопках), так и в контекстном меню в AttachTableView
    setupActions();

	// Инициализируется контроллер списка файлов
    attachTableController = new AttachTableController(this);
    attachTableController->setObjectName("attachTableController");

    globalparameters.attachtable_controller(attachTableController);

    setupUI();
    setupSignals();
    assembly();
}
AttachTableScreen::~AttachTableScreen()
{}
// Настройка возможных действий
void AttachTableScreen:: setupActions(void){
	// Добавление файла
    actionAddAttach = new QAction(tr("Attach file"), this);
    actionAddAttach->setStatusTip(tr("Attach file"));
    actionAddAttach->setIcon(QIcon(":/resource/pic/attach_add.svg"));

	// Добавление линка
    actionAddLink = new QAction(tr("Add link"), this);
    actionAddLink->setStatusTip(tr("Add link without file copying"));
    actionAddLink->setIcon(QIcon(":/resource/pic/attach_add_link.svg"));

	// Редактирование информации о файле (имени файла)
    actionEditFileName = new QAction(tr("Edit file name"), this);
    actionEditFileName->setStatusTip(tr("Edit file name"));
    actionEditFileName->setIcon(QIcon(":/resource/pic/attach_edit.svg"));

	// Удаление файла
    actionDeleteAttach = new QAction(tr("Delete file"), this);
    actionDeleteAttach->setStatusTip(tr("Delete file"));
    actionDeleteAttach->setIcon(QIcon(":/resource/pic/attach_delete.svg"));

	// Просмотр файла
    actionOpenAttach = new QAction(tr("Preview file"), this);
    actionOpenAttach->setStatusTip(tr("Preview file"));
    actionOpenAttach->setIcon(QIcon(":/resource/pic/attach_preview.svg"));

	// Сохранить как... файл
    actionSaveAsAttach = new QAction(tr("Save as..."), this);
    actionSaveAsAttach->setStatusTip(tr("Save as..."));
    actionSaveAsAttach->setIcon(QIcon(":/resource/pic/attach_save_as.svg"));

	// Информация об аттаче
    actionShowAttachInfo = new QAction(tr("Attach info"), this);
    actionShowAttachInfo->setStatusTip(tr("Attach info"));
    actionShowAttachInfo->setIcon(QIcon(":/resource/pic/attach_info.svg"));

	// Переключение на редактор
    actionSwitchToEditor = new QAction(tr("Return to editor"), this);
    actionSwitchToEditor->setStatusTip(tr("Return to editor"));
    actionSwitchToEditor->setIcon(QIcon(":/resource/pic/attach_switch_to_editor.svg"));
}
void AttachTableScreen:: setupUI(void){
	// Создание тулбара
    toolsLine = new QToolBar(this);

	// Создание кнопок на тулбаре
    insert_action_as_button<QToolButton>(toolsLine, actionAddAttach);
    insert_action_as_button<QToolButton>(toolsLine, actionAddLink);
    insert_action_as_button<QToolButton>(toolsLine, actionEditFileName);
    insert_action_as_button<QToolButton>(toolsLine, actionDeleteAttach);
    insert_action_as_button<QToolButton>(toolsLine, actionOpenAttach);
    insert_action_as_button<QToolButton>(toolsLine, actionSaveAsAttach);
    insert_action_as_button<QToolButton>(toolsLine, actionShowAttachInfo);

    toolsLine->addSeparator();

    insert_action_as_button<QToolButton>(toolsLine, actionSwitchToEditor);
}
void AttachTableScreen:: setupSignals(void){
	// Связывание действий
    connect(actionAddAttach, &QAction::triggered, attachTableController, &AttachTableController::on_add_attach);
    connect(actionAddLink, &QAction::triggered, attachTableController, &AttachTableController::on_add_link);
    connect(actionEditFileName, &QAction::triggered, attachTableController, &AttachTableController::on_edit_file_name);
    connect(actionDeleteAttach, &QAction::triggered, attachTableController, &AttachTableController::on_delete_attach);
    connect(actionOpenAttach, &QAction::triggered, attachTableController, &AttachTableController::on_open_attach);
    connect(actionSaveAsAttach, &QAction::triggered, attachTableController, &AttachTableController::on_save_as_attach);

    connect(actionShowAttachInfo, &QAction::triggered, attachTableController, &AttachTableController::on_show_attach_info);

    connect(actionSwitchToEditor, &QAction::triggered, attachTableController, &AttachTableController::on_switch_to_editor);
}
void AttachTableScreen:: assembly(void){
    screenLayout = new QVBoxLayout();	// todo: Добавить this?

    screenLayout->addWidget(toolsLine);
    screenLayout->addWidget(attachTableController->view());

    setLayout(screenLayout);

	// Границы убираются, так как данный объект будет использоваться как виджет
    QLayout *lt;
    lt = layout();
    lt->setContentsMargins(0, 0, 0, 0);
}
// Очистка таблицы приаттаченных файлов
// Список на экране должен быть пустой
// Вызывается в моменты, когда ни одна запись не выбрана или содержимое записи недоступно
void AttachTableScreen:: clear(){
    attachTableController->attach_table_data(nullptr);
}
