#include <cassert>
#include <chrono>
#include <QWidget>
#include <QDateTime>
#include <QFileInfo>

#include "main.h"
#include "trash_monitoring.h"
#include "libraries/global_parameters.h"
#include "models/app_config/app_config.h"
#include "libraries/disk_helper.h"

extern AppConfig		appconfig;
extern GlobalParameters globalparameters;

TrashMonitoring::TrashMonitoring(void)
{}

TrashMonitoring::~TrashMonitoring(void)
{}

void TrashMonitoring::recover_from_trash(){
    auto	_file_name			= "mytetra.xml";
    auto	file_name_fullpath	= appconfig.tetra_dir() + "/" + _file_name;
//    if(QFile::exists(appconfig.tetra_dir() + "/" + _file_name))remove_oldest_file(_file_name);
    if(QFile::exists(file_name_fullpath)){
        if(! QFile::remove(file_name_fullpath)){// Файл физически удаляется
//	}else{
            critical_error("Can not delete file " + file_name_fullpath);
            exit(0);
        }
	}
//    auto	_main_program_file = globalparameters.main_program_file();
//    QFileInfo	main_program_file_info(_main_program_file);
//    QString	full_current_path = main_program_file_info.absolutePath();
    if(_files_table.size() == 0){
        if(! QFile::copy(QString(":/resource/standartdata/") + _file_name, appconfig.trash_dir() + "/" + _file_name)) throw std::runtime_error("Can not copy mytetra.xml");
        else QFile::setPermissions(appconfig.trash_dir() + "/" + _file_name, QFile::ReadUser | QFile::WriteUser);
//	bool succedded = DiskHelper::save_strings_to_directory(full_current_path + "/trash", globalparameters.mytetra_xml());
//	assert(succedded);
        add_file(_file_name);	// globalparameters.mytetra_xml().keys()[0]
    }else{
        auto file_data = [&] {FileData r;for(auto f : _files_table) if(f._size > r._size) r = f;return r;} ();		// _files_table.first();
        DiskHelper::copy_file_to_data(appconfig.trash_dir() + '/' + file_data._name);
    }
}

void TrashMonitoring::init(QString _trash_path){
    auto		_main_program_file = globalparameters.main_program_file();
    QFileInfo	main_program_file_info(_main_program_file);
    QString		full_current_path = main_program_file_info.absolutePath();
	// Инит объекта директории с указанным путем
    _dir.setPath(_trash_path);
    if(! _dir.exists()){
        DiskHelper::create_directory(full_current_path, "trash");
//	critical_error("Can not open trash directory " + _trash_path);
//	exit(1);
    }
    _path = _trash_path;// Имя директории запоминается

	// Размер директории
    _dir_size = 0;

	// Получение списка информации о файлах
    QFileInfoList fileInfoList = _dir.entryInfoList(QDir::Files, QDir::Time);
	// Перебор всех файлов в полученном списке
    for(int i = 0; i < fileInfoList.size(); i ++){
        QString			_file_name	= fileInfoList.at(i).fileName();
        unsigned int	_file_time	= fileInfoList.at(i).created().toTime_t();
        unsigned int	_file_size	= fileInfoList.at(i).size();
        // Директории с именами "." и ".." обрабатывать не нужно
        if(_file_name == "." || _file_name == "..") continue;
        // Увеличивается подсчитываемый размер директории
        _dir_size = _dir_size + _file_size;

        // Информация о файле добавляется в таблицу
        FileData currentFileData;
        currentFileData._name	= _file_name;
        currentFileData._time	= _file_time;
        currentFileData._size	= _file_size;
        _files_table << currentFileData;
    }
	// qDebug() << "In init trash " << filesTable.size() << "files";
    _is_inited = true;
}

// Функция должна вызываться после фактического добавления файла в корзину
// принимает имя файла без пути к директории
void TrashMonitoring::add_file(QString _file_name){
	// Выясняется время создания файла берется текущее, особой точности не нужно
    unsigned int _file_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());	// (QDateTime::currentDateTime()).toTime_t();

	// Выясняется размер файла
    QFile			currentFile(_path + "/" + _file_name);
    unsigned int	_file_size = currentFile.size();

	// Увеличивается подсчитываемый размер директории
    _dir_size = _dir_size + _file_size;

	// Информация о файле добавляется в таблицу
    FileData currentfiledata;
    currentfiledata._name	= _file_name;
    currentfiledata._time	= _file_time;
    currentfiledata._size	= _file_size;
    _files_table.insert(0, currentfiledata);

    update();
}

void TrashMonitoring::update(void){
	// Исключается наиболее старый файл пока выполняется
	// условие что количество файлов слишком велико или
	// суммарный размер файлов превышает предельно допустимый размер корзины
    while(_files_table.size() > appconfig.trash_max_file_count() ||
        _dir_size > appconfig.trash_size() * 1000000
        ){
        if(_files_table.size() <= 10)															// Оставляется последний файл, какого бы размера он не был
                break;
        else remove_oldest_file();
    }
}

void TrashMonitoring::remove_file_fullpath(QString file_name_){
//    QString _file_name = got ? _path + "/" + _files_table.last()._file_name : _path + "/" + found._file_name;

    qDebug() << "Remove file " << file_name_;
    if(QFile::exists(file_name_)){
        if(! QFile::remove(file_name_)){// Файл физически удаляется
            critical_error("Can not delete file " + file_name_);
            exit(0);
        }
	}
}


void TrashMonitoring::remove_oldest_file(QString file_name_){
    FileData	found;
    bool		got = false;
    if(file_name_ != ""){
        for(auto f : _files_table){
            if(file_name_ == f._name){
                found	= f;
                got		= true;
                break;
            }
        }
	}
    QString _file_found = got ? _path + "/" + found._name : _path + "/" + _files_table.last()._name;

    qDebug() << "Remove file " << _file_found << " from trash";
    if(QFile::exists(_file_found)){
        if(QFile::remove(_file_found)){	// Файл физически удаляется
            // Расчетный размер директории уменьшается на размер файла
            _dir_size = _dir_size - _files_table.last()._size;
            //
            QMutableListIterator<FileData> iter(_files_table);
            while(iter.hasNext()){
                auto it = iter.next();
                if(it._name == _file_found) _files_table.removeOne(it);
            }
            // Файл удаляется из списка
//	    _files_table.removeLast();
        }else{
            critical_error("In trash monitoring can not delete file " + _file_found);
            exit(0);
        }
    }else if(_files_table.size() > 10) _files_table.removeLast();
}

