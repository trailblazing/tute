#include <QDir>

#include "DiskHelper.h"
#include "main.h"
#include "models/app_config/AppConfig.h"
#include "libraries/TrashMonitoring.h"
#include "libraries/GlobalParameters.h"



extern AppConfig appconfig;
extern GlobalParameters globalparameters;
extern TrashMonitoring trashmonitoring;
extern const char *index_xml_file_name;

DiskHelper::DiskHelper()
{

}


// Удаление директории с копированием содержимого в корзину
void DiskHelper::remove_directory_to_trash(QString nameDirFrom)
{
    QDir dirfrom(nameDirFrom);
    QStringList fileList = dirfrom.entryList();

    QString nameDirTo = appconfig.trash_dir();

    // Перебор всех файлов в удаляемой директории
    for(int i = 0; i < fileList.size(); i++) {
        // Директории с именами "." и ".." обрабатывать не нужно
        if(fileList.at(i) == "." || fileList.at(i) == "..")continue;

        // Исходный файл, который будет перенесен в корзину
        QString fileNameFrom = nameDirFrom + "/" + fileList.at(i);

        // Конечный файл, который должен лежать в корзине
        QString fileNameToShort;
        QString fileNameTo;
        bool targetFileFree = false;

        do {
            fileNameToShort = get_unical_id() + "_" + fileList.at(i);
            fileNameTo       = nameDirTo + "/" + fileNameToShort;

            if(QFile::exists(fileNameTo)) targetFileFree = false;
            else targetFileFree = true;
        } while(!targetFileFree);

        qDebug() << "Move file from " << fileNameFrom << " to " << fileNameTo;

        if(QFile::exists(fileNameFrom)) {
            // Перенос файла в корзину
            if(QFile::rename(fileNameFrom, fileNameTo) == true)
                trashmonitoring.add_file(fileNameToShort); // Оповещение что в корзину добавлен файл
            else {
                //            critical_error("Can not remove file\n" + fileNameFrom + "\nto directory\n" + nameDirTo + "\nwith new name\n" + fileNameTo);
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

    if(!applicationdir.rmdir(nameDirFrom))
        qDebug() << "Directory " << nameDirFrom << " NOT deleted";
    else
        qDebug() << "Directory " << nameDirFrom << " delete succesfull";
}


// Удаление файла с копированием его копии в корзину
void DiskHelper::remove_file_to_trash(QString fileNameFrom)
{
    // Получение короткого имени исходного файла
    QFileInfo fileInfo(fileNameFrom);
    QString fileNameFromShort = fileInfo.fileName();

    // Получение имени файла для сохранения в корзине
    QString fileNameToShort = get_unical_id() + "_" + fileNameFromShort;
    QString fileNameTo     = appconfig.trash_dir() + "/" + fileNameToShort;

    qDebug() << "Move file from " << fileNameFrom << " to " << fileNameTo;

    if(QFile::exists(fileNameFrom)) {
        // Файл перемещается в корзину
        if(QFile::rename(fileNameFrom, fileNameTo) == true)
            trashmonitoring.add_file(fileNameToShort); // Оповещение что в корзину добавлен файл
        else {
            //        critical_error("Can not remove file\n" + fileNameFrom + "\nto reserve file\n" + fileNameTo);
        }
    }
}


// Копирование файла в корзину
// Функция возвращает полное имя файла копии
QString DiskHelper::copy_file_to_data(QString file_name_from)
{
    // Получение короткого имени исходного файла
    //    QFileInfo fileInfo(file_name_from);
    //    QString file_name_from_short = fileInfo.fileName();

    // Получение имени файла для сохранения в корзине
    //    QString file_name_to_short = file_name_from_short.remove(0, file_name_from_short.lastIndexOf('_') + 1);
    QString file_name_to = appconfig.tetra_dir()    //
                           + "/"
                           + index_xml_file_name  // + globalparameters.main_program_file() + ".xml"
                           ; //appconfig.get_tetradir() + "/" + file_name_to_short;

    qDebug() << "Copy file from " << file_name_from << " to " << file_name_to;

    // Файл копируется в корзину
    if(QFile::copy(file_name_from, file_name_to) != true) {
        //        trashmonitoring.add_file(file_name_to_short); // Оповещение что в корзину добавлен файл
        //        }else {
        critical_error("Can not remove file\n" + file_name_from + "\nto reserve file\n" + file_name_to);
    }

    QFileInfo fileInfoTo(file_name_to);

    return fileInfoTo.absoluteFilePath();
}

// Копирование файла в корзину
// Функция возвращает полное имя файла копии
QString DiskHelper::copy_file_to_trash(QString file_name_from)
{
    // Получение короткого имени исходного файла
    QFileInfo fileInfo(file_name_from);
    QString file_name_from_short = fileInfo.fileName();

    // Получение имени файла для сохранения в корзине
    QString file_name_to_short = get_unical_id() + "_" + file_name_from_short;
    QString file_name_to     = appconfig.trash_dir() + "/" + file_name_to_short;

    qDebug() << "Copy file from " << file_name_from << " to " << file_name_to;

    if(QFile::exists(file_name_from)) {
        // Файл копируется в корзину
        if(QFile::copy(file_name_from, file_name_to) == true)
            trashmonitoring.add_file(file_name_to_short); // Оповещение что в корзину добавлен файл
        else {
            //        critical_error("Can not remove file\n" + fileNameFrom + "\nto reserve file\n" + fileNameTo);
        }
    }

    QFileInfo fileInfoTo(file_name_to);
    return fileInfoTo.absoluteFilePath();
}


// Создание временной директории
QString DiskHelper::create_temp_directory(void)
{
    QDir dir;
    QString systemTempDirName = dir.tempPath();

    QString temp_dir_name = "mytetra" + get_unical_id();

    // Создается директория
    dir.setPath(systemTempDirName);
    dir.mkdir(temp_dir_name);

    QString createTempDirName = systemTempDirName + "/" + temp_dir_name;

    qDebug() << "Create temporary directory " + createTempDirName;

    return createTempDirName;
}


// Удаление директории вместе со всеми поддиректориями и файлами
bool DiskHelper::remove_directory(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if(dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if(info.isDir())
                result = remove_directory(info.absoluteFilePath());
            else
                result = QFile::remove(info.absoluteFilePath());

            if(!result)
                return result;
        }

        result = dir.rmdir(dirName);
    }

    return result;
}


// Копирование содержимого директории
// Копируются только файлы
bool DiskHelper::copy_directory(const QString &fromName, const QString &toName)
{
    QDir fromDir(fromName);
    QDir toDir(toName);

    if(fromDir.exists() && toDir.exists()) {
        Q_FOREACH(QFileInfo info, fromDir.entryInfoList(QDir::Files)) {
            QFile::copy(info.absoluteFilePath(), toName + "/" + info.fileName());
        }

        return true;
    }

    return false;
}


// Получение списка файлов с их содержимым в указанной директории
QMap<QString, QByteArray> DiskHelper::get_files_from_directory(QString dirName, QString fileMask)
{
    QMap<QString, QByteArray> result;
    QDir directory(dirName);

    if(directory.exists()) {
        QStringList filter;
        filter << fileMask;

        foreach(QFileInfo info, directory.entryInfoList(filter, QDir::Files)) {
            QFile f(info.absoluteFilePath());

            if(!f.open(QIODevice::ReadOnly))
                critical_error("DiskHelper::getFilesFromDirectory() : File '" + info.absoluteFilePath() + "' open error");

            // Содержимое файла
            QByteArray b = f.readAll();

            // Содержимое файла сохраняется с ключем в виде имени файла
            result.insert(info.fileName(), b);
        }
    } else
        qDebug() << "DiskHelper::getFilesFromDirectory() : Can not find directory" << dirName;

    return result;
}


bool DiskHelper::save_files_to_directory(QString dirName, QMap<QString, QByteArray> fileList)
{
    qDebug() << "DiskHelper::saveFilesToDirectory() : Directory name " << dirName;

    QDir directory(dirName);

    // Если директория существует
    if(directory.exists()) {
        foreach(QString filename, fileList.keys()) {
            qDebug() << "DiskHelper::saveFilesToDirectory() : Save file " << filename;

            QFile file(dirName + "/" + filename);

            // Файл открывается для записи
            if(!file.open(QIODevice::WriteOnly)) {
                qDebug() << "DiskHelper::saveFilesToDirectory() : Can not save file '" << filename << "' to directory '" << dirName << "'";
                return false;
            }

            // Данные сохраняются в файл
            file.write(fileList.value(filename));
        }

        return true;
    } else {
        qDebug() << "DiskHelper::saveFilesToDirectory() : Can not find directory" << dirName;
        return false;
    }
}


