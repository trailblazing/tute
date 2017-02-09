
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include <QObject>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDialogButtonBox>

#include "main.h"
#include "attach_table_controller.h"
#include "views/attach_table/attach_table_view.h"
#include "models/attach_table/attach_table_model.h"
#include "models/attach_table/attach_table_data.h"
#include "models/app_config/app_config.h"
#include "libraries/global_parameters.h"
#include "libraries/crypt/crypt_service.h"
#include "libraries/disk_helper.h"
#include "views/record/meta_editor.h"
#include "views/tree/tree_screen.h"
#include "views/dialog/reduce_message_box.h"
#include "libraries/flat_control.h"
#include "views/tree/tree_view.h"
#include "views/record/editentry.h"


extern gl_para globalparameters;
extern AppConfig	appconfig;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(AttachTableController)
#endif

AttachTableController::AttachTableController(QObject *parent)
    : QObject(parent)
      , _view(new AttachTableView(qobject_cast<QWidget * >(parent)))
      , _model(new AttachTableModel(this)){
	// Создается область со списком файлов
	//    _view = new AttachTableView(qobject_cast<QWidget *>(parent)); // Вид размещается внутри виджета Screen
    _view->setObjectName("attachTableView");
    _view->setController(this);
    _view->init();	// Инициализация запускается только после установки контроллера

	// Создание модели данных (тонкой обертки над AttachTableData)
	//    _model = new AttachTableModel(this);
    _model->setObjectName("attachTableModel");

	// Модель данных задается для вида
    _view->setModel(_model);
}
AttachTableController::~AttachTableController(){
	// delete
    _view->deleteLater();
	// delete
    _model->deleteLater();
}
AttachTableView *AttachTableController::view(void){
    return _view;
}
// Основной метод установки указателя на данные
void AttachTableController::attach_table_data(std::shared_ptr<AttachTableData> attach_table_data){
    if(attach_table_data) _model->setData(QModelIndex(), QVariant::fromValue(*attach_table_data), ATTACHTABLE_ROLE_TABLE_DATA);
}
AttachTableData *AttachTableController::attach_table_data(){
    return (_model->data(QModelIndex(), ATTACHTABLE_ROLE_TABLE_DATA)).value<AttachTableData *>();
}
void AttachTableController::on_add_attach(void){
    add_smart("file");
}
void AttachTableController::on_add_link(){
    add_smart("link");
}
void AttachTableController::add_smart(QString attach_type){
    QStringList files = select_files_for_adding(attach_type);
    if(files.size() == 0) return;// Если ни один файл не выбран

	// Указатель на данные таблицы приаттаченных файлов
    AttachTableData *attachTableData = attach_table_data();
    if(attachTableData == nullptr) critical_error("Unset attach table data in AttachTableController::addSmart()");
	// Перебираются выбранные в диалоге файлы
    for(int i = 0; i < files.size(); ++ i){
	// Текущее полное имя файла
	QString currFullFileName = files.at(i);

	QFileInfo currFileInfo(currFullFileName);
	// Если пользователь выбрал директорию (директорию выбирать нельзя, пока что можно выбирать только файлы)
	if(currFileInfo.isDir()){
	    show_message_box(tr("Can't add directory. Please, select files."));
	    break;
	}
	qDebug() << "Select file from disk: " << currFullFileName;

	// Текущее короткое имя файла
	QString currShortFileName = currFileInfo.fileName();

	// Идентификатор аттача
	QString id = get_unical_id();

	// Конструируется Attach, который нужно добавить
	Attach attach(attach_type, attachTableData);
	attach.setField("id", id);
	attach.setField("fileName", currShortFileName);

	bool result = false;
	if(attach_type == "file") result = attach.copyFileToBase(currFullFileName);																																											// Файл аттача копируется в базу
	else if(attach_type == "link"){
	    attach.setField("link", currFullFileName);	// Запоминается куда указывает линк
	    result = true;
	}else critical_error("Unsupport adding mode");
	// Если запись, к которой добавляется аттач, зашифрована
	if(attachTableData->is_record_crypt()) attach.encrypt();
	if(result){
		// Данные аттача добавляются в таблицу приаттаченных файлов
	    attachTableData->add_attach(attach);
	}else{
	    show_message_box(tr("An error occurred while copying file(s). File(s) can't attach."));
	    break;
	}
    }	// Закончился цикл перебора файлов

	// Сохранение дерева веток
	//    find_object<TreeScreen>(tree_screen_singleton_name)
    globalparameters.tree_screen()->view()->know_model_save();
	// Обновление иконки аттачей в редакторе
    if(attachTableData->size() > 0){
	auto *editor_ = globalparameters.edit_entry();	// find_object<MetaEditor>(meta_editor_singleton_name);
	editor_->to_attach()->setIcon(editor_->icon_attach_exists());
    }
}
QStringList AttachTableController::select_files_for_adding(QString attach_type){
	// Диалог выбора файлов
    QFileDialog fileSelectDialog;

    QString title;
    if(attach_type == "file") title = tr("Attach file");
    if(attach_type == "link") title = tr("Add link to file");
    fileSelectDialog.setWindowTitle(title);

    fileSelectDialog.setFileMode(QFileDialog::ExistingFiles);	// QFileDialog::ExistingFile
    fileSelectDialog.setNameFilter("*");
    fileSelectDialog.setDirectory(QDir::homePath());

	// Если существует каталог, открытый при предыдущем выборе файла
    QDir appendDir(appconfig.attach_append_dir());
    if(appendDir.exists()) fileSelectDialog.setDirectory(appendDir);
	// Отрисовка диалога выбора
    int dialogResult = fileSelectDialog.exec();
	// Если была нажата отмена
    if(dialogResult == QDialog::Rejected) return QStringList();
	// Выясняется список выбранных файлов
    QStringList files = fileSelectDialog.selectedFiles();
	// Запоминается директория, в которой был сделан выбор
    if(files.size() > 0) appconfig.attach_append_dir(fileSelectDialog.directory().absolutePath());
    return files;
}
// Выбрано действие "Сохранить как..."
void AttachTableController::on_save_as_attach(void){
    QList<QString> selectedId = selected_id();
	// Если ни один аттач не выбран
    if(selectedId.size() == 0){
	show_message_box(tr("Please select any attach(es) for save to your directory."));

	return;
    }
	// Если выбран только один аттач
    if(selectedId.size() == 1){
	// Диалог выбора имени файла
	QFileDialog fileSelectDialog;
	fileSelectDialog.setFileMode(QFileDialog::ExistingFile);// Один файл
	fileSelectDialog.setWindowTitle(tr("Save as..."));
	fileSelectDialog.setDirectory(QDir::homePath());
	fileSelectDialog.setAcceptMode(QFileDialog::AcceptSave);// Чтобы была кнока "Сохранить" а не "Выбрать"
	fileSelectDialog.setFileMode(QFileDialog::AnyFile);	// Чтобы кнопка "Сохранить" была активной

	// Если существует каталог, открытый при предыдущем сохранении
	QDir saveAsDir(appconfig.attach_save_as_dir());
	if(saveAsDir.exists()) fileSelectDialog.setDirectory(saveAsDir);
	// В диалоге устанавливается имя файла выбранного аттача
	QString		id			= selectedId.at(0);
	AttachTableData *attachTableData	= attach_table_data();
	QString		attachType		= attachTableData->attach(id).getField("type");
	QString		fileName		= attachTableData->file_name_by_id(id);
	QString		fullFileName		= attachTableData->absolute_inner_file_name_by_id(id);
	fileSelectDialog.selectFile(fileName);

	// Отрисовка диалога выбора
	int dialogResult = fileSelectDialog.exec();
	if(dialogResult == QDialog::Rejected) return;
	// Запоминается директория, в которой был сделан выбор
	appconfig.attach_save_as_dir(fileSelectDialog.directory().absolutePath());


	// Выясняется список выбранных файлов
	QStringList selectFiles = fileSelectDialog.selectedFiles();
	// Должен быть выбран только один файл
	if(selectFiles.size() != 1){
	    show_message_box(tr("For save sigle file you must set single result file name."));

	    return;
	}
	// Указанное пользователем имя файла для сохранения аттача, взятое из формы ввода
	QString targetFileName = selectFiles.at(0);

	// Непосредственное сохранение файла
	save_attach_to_user_place(fullFileName, targetFileName, attachType, attachTableData->is_record_crypt());
    }else if(selectedId.size() > 1){	// Если выбрано несколько аттачей
	// Диалог выбора имени директории
	QFileDialog fileSelectDialog;
	fileSelectDialog.setFileMode(QFileDialog::DirectoryOnly);	// Выбор директории
	fileSelectDialog.setWindowTitle(tr("Save attaches to directory..."));
	fileSelectDialog.setDirectory(QDir::homePath());

	// Если существует каталог, открытый при предыдущем сохранении
	QDir saveAsDir(appconfig.attach_save_as_dir());
	if(saveAsDir.exists()) fileSelectDialog.setDirectory(saveAsDir);
	// Отрисовка диалога выбора
	int dialogResult = fileSelectDialog.exec();
	if(dialogResult == QDialog::Rejected) return;
	// Запоминается директория, в которой был сделан выбор
	appconfig.attach_save_as_dir(fileSelectDialog.directory().absolutePath());


	// Выбранная директория
	QString toDir = fileSelectDialog.directory().absolutePath();

	AttachTableData *attachTableData = attach_table_data();

	// Перебор выбранных для сохранения аттачей
	foreach(QString id, selectedId){
	    QString	attachType		= attachTableData->attach(id).getField("type");
	    QString	fileName		= attachTableData->file_name_by_id(id);
	    QString	fromFileName		= attachTableData->inner_file_name_by_id(id);
	    QString	fromFullFileName	= attachTableData->absolute_inner_file_name_by_id(id);
	    QString	toFullFileName		= toDir + "/" + fileName;

		// Непосредственное сохранение файла
	    save_attach_to_user_place(fromFullFileName, toFullFileName, attachType, attachTableData->is_record_crypt());
	}
    }
}
void AttachTableController::save_attach_to_user_place(QString from_full_file_name, QString to_full_file_name, QString attach_type, bool is_attach_crypt){
	// Проверка наличия исходного файла (ведь по каким-то причинам его может не быть, например после какого-нибудь сбоя)
    QFile file(from_full_file_name);
    if(file.exists() == false){
	show_message_box(tr("Can't save file. File %1 not exists in database.").arg(from_full_file_name));

	return;
    }
	// Сохранение
    bool result = file.copy(to_full_file_name);
    if(! result){
	show_message_box(tr("Can't save file %1. Any i/o problem.").arg(to_full_file_name));

	return;
    }
	// Расшифровка файла, если он был зашифрован и данные хранились в базе (то есть, это именно тип file, а не линк на файл)
    if(is_attach_crypt && attach_type == "file") CryptService::decryptFile(globalparameters.crypt_key(), to_full_file_name);
}
void AttachTableController::on_edit_file_name(void){
    QList<QString> selectedId = selected_id();
	// Если ни один аттач не выбран
    if(selectedId.size() == 0) return;
	// Если выбрано больше одного аттача
    if(selectedId.size() > 1){
	show_message_box(tr("Please select single attach for edit."));

	return;
    }
	// Имя файла выбранного аттача
    QString		id			= selectedId.at(0);
    AttachTableData	*attachTableData	= attach_table_data();
    QString		fileName		= attachTableData->file_name_by_id(id);

	// Запрос нового имени файла
    bool	isOk;
    QString	newFileName = QInputDialog::getText(_view
						   , tr("File name editing")
						   , tr("File name:")
						   , QLineEdit::Normal
						   , fileName
						   , &isOk);
    if(! isOk) return;													// Была нажата кнопка Cancel
    if(newFileName.size() == 0){
	show_message_box(tr("Cant save file with empty name."));

	return;
    }
	// Данные изменяются
    Attach tempAttach = attachTableData->attach(id);
    tempAttach.setField("fileName", newFileName);
    attachTableData->attach(id, tempAttach);

	// Сохранение дерева веток
	//    find_object<TreeScreen>(tree_screen_singleton_name)
    globalparameters.tree_screen()->view()->know_model_save();
}
void AttachTableController::on_delete_attach(void){
    QList<QString> selectedId = selected_id();
	// Если ни один аттач не выбран
    if(selectedId.size() == 0){
	show_message_box(tr("Please select any attach(es) for delete."));

	return;
    }
	// Запрос подтверждения об удалении
    QMessageBox msgBox;
    msgBox.setText(tr("Do you want to delete attach file(s)?"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if(ret != QMessageBox::Ok) return;
	// Удаление выбранных аттачей
    AttachTableData *attachTableData = attach_table_data();

    foreach(QString id, selectedId)
    attachTableData->delete_attach(id);


	// Сохранение дерева веток
	//    find_object<TreeScreen>(tree_screen_singleton_name)
    globalparameters.tree_screen()->view()->know_model_save();
	// Обновление иконки аттачей в редакторе
    if(attachTableData->size() == 0){
	auto *editor_ = globalparameters.edit_entry();	// find_object<MetaEditor>(meta_editor_singleton_name);
	editor_->to_attach()->setIcon(editor_->icon_attach_not_exists());
    }
}
// Открытие аттача (аттачей) на просмотр
void AttachTableController::on_open_attach(void){
    AttachTableData *attachTableData = attach_table_data();

    QList<QString> selectedId = selected_id();

    foreach(QString id, selectedId){
	QString fullFileName = attachTableData->absolute_inner_file_name_by_id(id);
	// Если запись зашифрована и открывается файл (не линк), аттач надо скопировать в директорию корзины и расшифровать
	if(attachTableData->is_record_crypt() && attachTableData->attach(id).getField("type") == "file"){
	    if(appconfig.enable_decrypt_file_to_trash_directory()){
		fullFileName = DiskHelper::copy_file_to_trash(fullFileName);	// Копирование
		CryptService::decryptFile(globalparameters.crypt_key(), fullFileName);	// Расшифровка
	    }else{
		show_message_box(tr("Can't preview encrypted attach file %1.\n\nUse \"Save As...\" button or enable decrypt to temporary file in settings.").arg(fullFileName));
		continue;
	    }
	}
	qDebug() << "Open file: " + fullFileName;

	// QUrl urlFile;
	// urlFile.fromLocalFile(fullFileName);
	QUrl urlFile("file:" + fullFileName);

	// Открытие файла средствами операционной системы
	QDesktopServices::openUrl(urlFile);
    }
}
void AttachTableController::on_show_attach_info(void){
    QList<QString> selectedId = selected_id();
	// Если ни один аттач не выбран
    if(selectedId.size() == 0) return;
	// Если выбрано больше одного аттача
    if(selectedId.size() > 1){
	show_message_box(tr("Please select single attach for see info."));

	return;
    }
    QString		id			= selectedId.at(0);
    AttachTableData	*attachTableData	= attach_table_data();

    ReduceMessageBox messageBox;
    messageBox.setText("Attach file info");
    messageBox.setDetailedText(QString("<pre><p style='font-family:monospace'>") +			 \
	"<b>Attach:</b> " + attachTableData->file_name_by_id(id) + "\n" +				 \
	"<b>Attach ID:</b> " + id + "\n" +								 \
	"<b>Attach type:</b> " + attachTableData->attach(id).getField("type") + "\n" +			 \
	"<b>File size:</b> " + QString::number(attachTableData->attach(id).getFileSize()) + " bytes\n" + \
	"<b>Full path:</b> " + attachTableData->absolute_inner_file_name_by_id(id) +			 \
	"</p></pre>"
	);
    messageBox.setDetailedTextReadOnly(true);
    messageBox.setStandardButtons(QDialogButtonBox::Ok);// Для двух кнопок можно так: QDialogButtonBox::Ok | QDialogButtonBox::Cancel

	// int result=messageBox.exec();
	// qDebug() << "Result code: " << result;
    messageBox.exec();
}
void AttachTableController::on_switch_to_editor(void){
    auto *editor_ = globalparameters.edit_entry();	// find_object<MetaEditor>(meta_editor_singleton_name);
    editor_->to_editor_layout();
}
// Получение списка идентификаторов аттачей, выделенных в представлении
QList<QString> AttachTableController::selected_id(void){
    QList<QString> selectedId;

	// Получение списка выделенных в таблице на экране Item-элементов
    QModelIndexList selectItems = _view->selectionModel()->selectedIndexes();

	// Перебор выделенных элементов. Так как имеем несколько столбцов, то для одной строки будет несколько QModelIndex
    int previousRow = - 1;
    for(int i = 0; i < selectItems.size(); i ++){
	int row = selectItems.at(i).row();
	// Строка обратабывается только один раз (из-за того что для одной строки несколько QModelIndex)
	if(row != previousRow){
	    previousRow = row;

	    selectedId.append(_model->data(selectItems.at(i), ATTACHTABLE_ROLE_ID).toString());
	}
    }
    return selectedId;
}
