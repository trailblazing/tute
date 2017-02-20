#include <QDir>
#include <sstream>

#include "disk_helper.h"
#include "libraries/global_parameters.h"
#include "libraries/trash_monitoring.h"
#include "main.h"
#include "models/app_config/app_config.h"

extern std::shared_ptr<AppConfig> appconfig;
extern std::shared_ptr<gl_para> gl_paras;
extern TrashMonitoring trashmonitoring;
extern const char *index_xml_file_name;

DiskHelper::DiskHelper()
{}

// Удаление директории с копированием содержимого в корзину
void DiskHelper::remove_directory_to_trash(QString nameDirFrom){
	QDir dirfrom(nameDirFrom);
	QStringList fileList = dirfrom.entryList();

	QString nameDirTo = gl_paras->root_path() + "/" + QDir(appconfig->trash_dir()).dirName();
	// Перебор всех файлов в удаляемой директории
	for(int i = 0; i < fileList.size(); i++){
		// Директории с именами "." и ".." обрабатывать не нужно
		if(fileList.at(i) == "." || fileList.at(i) == "..") continue;
		// Исходный файл, который будет перенесен в корзину
		QString fileNameFrom = nameDirFrom + "/" + fileList.at(i);

		// Конечный файл, который должен лежать в корзине
		QString fileNameToShort;
		QString fileNameTo;
		bool targetFileFree = false;
		do {
			fileNameToShort = get_unical_id() + "_" + fileList.at(i);
			fileNameTo = nameDirTo + "/" + fileNameToShort;
			if(QFile::exists(fileNameTo)) targetFileFree = false;
			else targetFileFree = true;
		} while(!targetFileFree);
		qDebug() << "Move file from " << fileNameFrom << " to " << fileNameTo;
		if(QFile::exists(fileNameFrom)){
			// Перенос файла в корзину
			if(QFile::rename(fileNameFrom, fileNameTo) == true) trashmonitoring.add_file(fileNameToShort); // Оповещение что в корзину добавлен файл
			else{
				// critical_error("Can not remove file\n" + fileNameFrom + "\nto directory\n" + nameDirTo + "\nwith new name\n" + fileNameTo);
			}
		}
	}
	// Удаление директории
	// Из-за проблем с синтаксисом метода rmdir(), нельзя удалить ту
	// директорию, на которую указывает объект, поэтому удаление происходит
	// через дополнительный QDir объект, который указывает на директорию
	// где лежит бинарник.
	// Если в rmdir() передать относительный путь, то будет удалена директория
	// относительно директории бинарника.
	// Если в rmdir() передать асолютный путь, то будет удалена директория
	// по абсолютному пути
	QDir applicationdir(QCoreApplication::applicationDirPath());
	qDebug() << "Try delete directory " << nameDirFrom;
	if(!applicationdir.rmdir(nameDirFrom)) qDebug() << "Directory " << nameDirFrom << " NOT deleted";
	else qDebug() << "Directory " << nameDirFrom << " delete succesfull";
}

// Удаление файла с копированием его копии в корзину
void DiskHelper::remove_file_to_trash(QString file_name_from){
	// Получение короткого имени исходного файла
	QFileInfo file_info(file_name_from);
	QString file_name_from_short = file_info.fileName();

	// Получение имени файла для сохранения в корзине
	QString file_name_to_short = get_unical_id() + "_" + file_name_from_short;
	QString file_name_to = gl_paras->root_path() + "/" + QDir(appconfig->trash_dir()).dirName() + "/" + file_name_to_short;

	qDebug() << "Move file from " << file_name_from << " to " << file_name_to;
	if(QFile::exists(file_name_from)){
		// Файл перемещается в корзину
		if(QFile::rename(file_name_from, file_name_to) == true) trashmonitoring.add_file(file_name_to_short); // Оповещение что в корзину добавлен файл
		else{
			// critical_error("Can not remove file\n" + fileNameFrom + "\nto reserve file\n" + fileNameTo);
		}
	}
}

// Копирование файла в корзину
// Функция возвращает полное имя файла копии
std::shared_ptr<QFileInfo> DiskHelper::copy_file_to_data_folder(const QString &file_name_to, const QString &file_name_from){
	std::shared_ptr<QFileInfo> result(nullptr);
	// Получение короткого имени исходного файла
	// QFileInfo fileInfo(file_name_from);
	// QString file_name_from_short = fileInfo.fileName();

	//// Получение имени файла для сохранения в корзине
	////    QString file_name_to_short = file_name_from_short.remove(0, file_name_from_short.lastIndexOf('_') + 1);
	// QString file_name_to = appconfig->tetra_dir()	//
	// + "/"
	// + index_xml_file_name	// "index.xml"// + globalparameters.main_program_file() + ".xml"
	// ;				// appconfig->datadir() + "/" + file_name_to_short;

	qDebug() << "Copy file from " << file_name_from << " to " << file_name_to;
	QFileInfo fileInfoFrom(file_name_from);

	// QFile file_from(file_name_from);
	// if(file_from.open(QIODevice::ReadOnly)){
	// auto	file_from_size		= file_from.size();	// when file does open.
	auto file_from_size_again = filesize(file_name_from.toStdString().c_str());
	// file_from.close();
	if( // file_from_size > 0 ||
		file_from_size_again > 0){  // if(fileInfoFrom.size() != 0){	// prevent to erase file target
		// Файл копируется в корзину
		if(!QFile::copy(file_name_from, file_name_to)){
			// trashmonitoring.add_file(file_name_to_short); // Оповещение что в корзину добавлен файл
			// }else {
			critical_error("Can not copy file\n" + file_name_from + "\nto file\n" + file_name_to);
		}else result = std::make_shared<QFileInfo>(file_name_to);
	}
	// }
	// QFileInfo fileInfoTo(file_name_to);

	return result; // fileInfoTo.absoluteFilePath();
}

// Копирование файла в корзину
// Функция возвращает полное имя файла копии
QString DiskHelper::copy_file_to_trash(QString file_name_from){
	// Получение короткого имени исходного файла
	QFileInfo fileInfo(file_name_from);
	QString file_name_from_short = fileInfo.fileName();

	// Получение имени файла для сохранения в корзине
	QString file_name_to_short = get_unical_id() + "_" + file_name_from_short;
	QString file_name_to = gl_paras->root_path() + "/" + QDir(appconfig->trash_dir()).dirName() + "/" + file_name_to_short;

	qDebug() << "Copy file from " << file_name_from << " to " << file_name_to;
	if(QFile::exists(file_name_from)){
		// Файл копируется в корзину
		if(QFile::copy(file_name_from, file_name_to)) trashmonitoring.add_file(file_name_from_short); // file_name_to_short // Оповещение что в корзину добавлен файл
		else critical_error("Can not remove file\n" + file_name_from + "\nto reserve file\n" + file_name_to);
	}
	QFileInfo fileInfoTo(file_name_to);

	return fileInfoTo.absoluteFilePath();
}

// Создание временной директории
QString DiskHelper::create_directory(QString path_name, QString dir_name){
	QDir dir;
	// QString systemTempDirName = dir.tempPath();

	// QString temp_dir_name = globalparameters.main_program_file() + get_unical_id();

	// Создается директория
	dir.setPath(path_name);
	QString createTempDirName = path_name + "/" + dir_name;
	if(!dir.mkdir(dir_name)) critical_error("Can\'t create path \"" + createTempDirName + "\"");
	qDebug() << "Create directory " + createTempDirName;

	return createTempDirName;
}

// Создание временной директории
QString DiskHelper::create_temp_directory(void){
	QDir dir;
	QString systemTempDirName = dir.tempPath();

	QString temp_dir_name = gl_paras->application_name() + get_unical_id();

	// Создается директория
	dir.setPath(systemTempDirName);
	QString createTempDirName = systemTempDirName + "/" + temp_dir_name;
	if(!dir.mkdir(temp_dir_name)) critical_error("Can\'t create path \"" + createTempDirName + "\"");
	qDebug() << "Create temporary directory " + createTempDirName;

	return createTempDirName;
}

// Удаление директории вместе со всеми поддиректориями и файлами
bool DiskHelper::remove_directory(const QString &dirName){
	bool result = true;
	QDir dir(dirName);
	if(dir.exists(dirName)){
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst)){
			if(info.isDir()) result = remove_directory(info.absoluteFilePath());
			else result = QFile::remove(info.absoluteFilePath());
			if(!result) return result;
		}
		result = dir.rmdir(dirName);
	}
	return result;
}

// Копирование содержимого директории
// Копируются только файлы
bool DiskHelper::copy_directory(const QString &fromName, const QString &toName){
	QDir fromDir(fromName);
	QDir toDir(toName);
	if(fromDir.exists() && toDir.exists()){
		Q_FOREACH(QFileInfo info, fromDir.entryInfoList(QDir::Files)) QFile::copy(info.absoluteFilePath(), toName + "/" + info.fileName());
		return true;
	}
	return false;
}

QMap<QString, QString> DiskHelper::get_strings_from_directory(QString dirName, QString fileMask){
	QMap<QString, QString> result;
	QDir directory(dirName);
	if(directory.exists()){
		QStringList filter;
		filter << fileMask;

		foreach(QFileInfo info, directory.entryInfoList(filter, QDir::Files)){
			QFile f(info.absoluteFilePath());
			if(!f.open(QIODevice::ReadOnly)) critical_error("DiskHelper::get_strings_from_directory() : File '" + info.absoluteFilePath() + "' open error");
			// Содержимое файла
			char buf[1024 * 1024];
			auto result_size = f.read(buf, 1024 * 1024);

			std::ostringstream ss;
			for(int i = 0; i < result_size; i++) ss << buf[i];
			QString content = QString::fromStdString(ss.str());

			// Содержимое файла сохраняется с ключем в виде имени файла
			result.insert(info.fileName(), content);
		}
	}else qDebug() << "DiskHelper::get_strings_from_directory() : Can not find directory" << dirName;
	return result;
}

// Получение списка файлов с их содержимым в указанной директории
QMap<QString, QByteArray> DiskHelper::get_files_from_directory(QString dirName, QString fileMask){
	QMap<QString, QByteArray> result;
	QDir directory(dirName);
	if(directory.exists()){
		QStringList filter;
		filter << fileMask;

		foreach(QFileInfo info, directory.entryInfoList(filter, QDir::Files)){
			QFile f(info.absoluteFilePath());
			if(!f.open(QIODevice::ReadOnly)) critical_error("DiskHelper::get_files_from_directory() : File '" + info.absoluteFilePath() + "' open error");
			// Содержимое файла
			QByteArray b = f.readAll();

			// Содержимое файла сохраняется с ключем в виде имени файла
			result.insert(info.fileName(), b);
		}
	}else qDebug() << "DiskHelper::get_files_from_directory() : Can not find directory" << dirName;
	return result;
}

bool DiskHelper::save_strings_to_directory(QString dirName, QMap<QString, QString> fileList){
	qDebug() << "DiskHelper::save_files_to_directory() : Directory name " << dirName;

	QDir directory(dirName);
	// Если директория существует
	if(directory.exists()){
		foreach(QString filename, fileList.keys()){
			qDebug() << "DiskHelper::save_files_to_directory() : Save file " << filename;

			QFile file(dirName + "/" + filename);
			// Файл открывается для записи
			if(!file.open(QIODevice::WriteOnly)){
				qDebug() << "DiskHelper::save_files_to_directory() : Can not save file '" << filename << "' to directory '" << dirName << "'";

				return false;
			}
			// Данные сохраняются в файл
			file.write(fileList.value(filename).toStdString().c_str());
		}

		return true;
	}else{
		qDebug() << "DiskHelper::save_files_to_directory() : Can not find directory" << dirName;

		return false;
	}
}

bool DiskHelper::save_files_to_directory(QString dirName, QMap<QString, QByteArray> fileList){
	qDebug() << "DiskHelper::save_files_to_directory() : Directory name " << dirName;

	QDir directory(dirName);
	// Если директория существует
	if(directory.exists()){
		foreach(QString filename, fileList.keys()){
			qDebug() << "DiskHelper::save_files_to_directory() : Save file " << filename;

			QFile file(dirName + "/" + filename);
			// Файл открывается для записи
			if(!file.open(QIODevice::WriteOnly)){
				qDebug() << "DiskHelper::save_files_to_directory() : Can not save file '" << filename << "' to directory '" << dirName << "'";

				return false;
			}
			// Данные сохраняются в файл
			file.write(fileList.value(filename));
		}

		return true;
	}else{
		qDebug() << "DiskHelper::save_files_to_directory() : Can not find directory" << dirName;

		return false;
	}
}

bool DiskHelper::copy_file_force(QString source, QString target){
	bool result = false;
	QFileInfo traget_info(target);
	if(!QDir(traget_info.path()).exists()) QDir::root().mkpath(traget_info.path());
	if(QFile(target).exists())
		if(!QFile::remove(target)) critical_error("Can\'t remove file \"" + target + "\"");
	if(!QFile::copy(source, target)) critical_error("Can\'t copy \"" + source + "\" to " + target + "\"");
	if((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(target) & (QFile::ReadUser | QFile::WriteUser))){
		if(!QFile::setPermissions(target, QFile::ReadUser | QFile::WriteUser)) critical_error("Can\'t set permissions to file \"" + target + "\"");
		else result = true;
	}
	return result;
}
