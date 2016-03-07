#include <QWidget>
#include <QDateTime>
#include <QFileInfo>

#include "main.h"
#include "TrashMonitoring.h"
#include "models/app_config/AppConfig.h"

extern AppConfig appconfig;


TrashMonitoring::TrashMonitoring(void)
{

}


TrashMonitoring::~TrashMonitoring(void)
{

}


void TrashMonitoring::init(QString _trash_path)
{
    // Инит объекта директории с указанным путем
    _dir.setPath(_trash_path);

    if(!_dir.exists()) {
        critical_error("Can not open trash directory " + _trash_path);
        exit(1);
    }

    _path = _trash_path; // Имя директории запоминается

    // Размер директории
    _dir_size = 0;

    // Получение списка информации о файлах
    QFileInfoList fileInfoList = _dir.entryInfoList(QDir::Files, QDir::Time);

    // Перебор всех файлов в полученном списке
    for(int i = 0; i < fileInfoList.size(); i++) {
        QString      _file_name = fileInfoList.at(i).fileName();
        unsigned int _file_time = fileInfoList.at(i).created().toTime_t();
        unsigned int _file_size = fileInfoList.at(i).size();

        // Директории с именами "." и ".." обрабатывать не нужно
        if(_file_name == "." || _file_name == "..")continue;

        // Увеличивается подсчитываемый размер директории
        _dir_size = _dir_size + _file_size;

        // Информация о файле добавляется в таблицу
        FileData currentFileData;
        currentFileData._file_name = _file_name;
        currentFileData._file_time = _file_time;
        currentFileData._file_size = _file_size;
        _files_table << currentFileData;
    }

    // qDebug() << "In init trash " << filesTable.size() << "files";
}


// Функция должна вызываться после фактического добавления файла в корзину
// принимает имя файла без пути к директории
void TrashMonitoring::add_file(QString _file_name)
{
    // Выясняется время создания файла берется текущее, особой точности не нужно
    unsigned int fileTime = (QDateTime::currentDateTime()).toTime_t();

    // Выясняется размер файла
    QFile currentFile(_path + "/" + _file_name);
    unsigned int fileSize = currentFile.size();

    // Увеличивается подсчитываемый размер директории
    _dir_size = _dir_size + fileSize;

    // Информация о файле добавляется в таблицу
    FileData currentfiledata;
    currentfiledata._file_name = _file_name;
    currentfiledata._file_time = fileTime;
    currentfiledata._file_size = fileSize;
    _files_table.insert(0, currentfiledata);

    update();
}


void TrashMonitoring::update(void)
{
    // Исключается наиболее старый файл пока выполняется
    // условие что количество файлов слишком велико или
    // суммарный размер файлов превышает предельно допустимый размер корзины
    while(_files_table.size() > appconfig.get_trashmaxfilecount() ||
          _dir_size > appconfig.get_trashsize() * 1000000) {
        if(_files_table.size() == 1) { // Оставляется последний файл, какого бы размера он не был
            break;
        } else {
            remove_oldest_file();
        }
    }
}


void TrashMonitoring::remove_oldest_file(void)
{
    QString _file_name = _path + "/" + _files_table.last()._file_name;

    qDebug() << "Remove file " << _file_name << " from trash";

    if(QFile::remove(_file_name)) { // Файл физически удаляется
        // Расчетный размер директории уменьшается на размер файла
        _dir_size = _dir_size - _files_table.last()._file_size;

        // Файл удаляется из списка
        _files_table.removeLast();
    } else {
        critical_error("In trash monitoring can not delete file " + _file_name);
        exit(0);
    }
}

