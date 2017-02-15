
#include <QDateTime>
#include <QFileInfo>
#include <QWidget>
#include <cassert>
#include <chrono>

#include "libraries/disk_helper.h"
#include "libraries/global_parameters.h"
#include "main.h"
#include "models/app_config/app_config.h"
#include "trash_monitoring.h"

extern AppConfig appconfig;
extern gl_para gl_paras;

TrashMonitoring::TrashMonitoring(void)
{
}

TrashMonitoring::~TrashMonitoring(void)
{
}

std::shared_ptr<QFileInfo> TrashMonitoring::recover_from_trash(std::shared_ptr<QFileInfo> target_file, bool force_remove)
{
    std::shared_ptr<QFileInfo> result(nullptr); //				= false;
    auto need = false;
    auto _file_name = target_file->fileName(); // "index.xml";
    auto file_name_fullpath = target_file->filePath(); // appconfig.tetra_dir() + "/" + _file_name;
    //    if(QFile::exists(appconfig.tetra_dir() + "/" + _file_name))remove_oldest_file(_file_name);
    if (QFile::exists(file_name_fullpath)) {
        //		QFileInfo currentFile(_trashmonitoring->_path + "/" + _name);
        //		_file_size = currentFile.size();

        //	unsigned int _file_size = 0;
        //	QFile		file_from(file_name_fullpath);
        //	if(file_from.open(QIODevice::ReadOnly)){
        auto _file_size = filesize(file_name_fullpath.toStdString().c_str()); // file_from.size();		// when file does open.
        //	    file_from.close();
        //	}
        if (0 == _file_size) {
            if (!QFile::remove(file_name_fullpath)) { // Файл физически удаляется
                //	}else{
                critical_error("In recover_from_trash. Can not delete file " + file_name_fullpath);
                exit(0);
            } else
                need = true;
        }
    } else
        need = true;
    if (need || force_remove) {
        //    auto	_main_program_file = globalparameters.main_program_file();
        //    QFileInfo	main_program_file_info(_main_program_file);
        //    QString	full_current_path = main_program_file_info.absolutePath();
        if (_files_table.size() == 0) {
            if (!QDir(gl_paras.root_path() + "/" + QDir(appconfig.trash_dir()).dirName()).exists())
                QDir(gl_paras.root_path()).mkdir(QDir(appconfig.trash_dir()).dirName());
            if (!QFile::copy(QString(":/resource/standarddata/") + _file_name, gl_paras.root_path() + "/" + QDir(appconfig.trash_dir()).dirName() + "/" + _file_name))
                throw std::runtime_error("Can not copy index.xml");
            else
                QFile::setPermissions(gl_paras.root_path() + "/" + QDir(appconfig.trash_dir()).dirName() + "/" + _file_name, QFile::ReadUser | QFile::WriteUser);
            //	bool succedded = DiskHelper::save_strings_to_directory(full_current_path + "/trash", globalparameters.index_xml());
            //	assert(succedded);
            add_file(_file_name); // globalparameters.index_xml().keys()[0]
            result = std::make_shared<QFileInfo>(file_name_fullpath); // target_file;
        }
        //    else{
        auto file_data = [&] {
            decltype(_files_table) left;
            for (auto f : _files_table)
                //                                    if(f->size() >= r->size()) r = f;
                //                                    else
                if (f->size() > 0)
                    left.append(f); // r = f;
            std::shared_ptr<FileData> r(nullptr);
            if (left.size() > 0) {
                r = std::make_shared<FileData>(this, left[0]->_name);
                for (auto f : left)
                    if (f->_time >= r->_time)
                        r = f;
            }
            return r;
        }(); // _files_table.first();
        if (file_data)
            result = DiskHelper::copy_file_to_data_folder(file_name_fullpath, gl_paras.root_path() + "/" + QDir(appconfig.trash_dir()).dirName() + "/" + file_data->_name);
        //        result = target_file;
        //    }
    }
    return result;
}

void TrashMonitoring::init(QString _trash_path)
{
    //    auto		_main_program_file = globalparameters.main_program_file();
    //    QFileInfo	main_program_file_info(_main_program_file);
    //    QString		full_current_path = main_program_file_info.absolutePath();
    // Инит объекта директории с указанным путем
    _dir.setPath(_trash_path);
    if (!_dir.exists()) {
        DiskHelper::create_directory(gl_paras.root_path() // full_current_path
            ,
            "trash");
        //	critical_error("Can not open trash directory " + _trash_path);
        //	exit(1);
    }
    _path = _trash_path; // Имя директории запоминается

    // Размер директории
    //    _dir_size = 0;

    // Получение списка информации о файлах
    QFileInfoList fileInfoList = _dir.entryInfoList(QDir::Files, QDir::Time);
    // Перебор всех файлов в полученном списке
    for (int i = 0; i < fileInfoList.size(); i++) {
        QString _file_name = fileInfoList.at(i).fileName();
        //		unsigned int	_file_time	= fileInfoList.at(i).created().toTime_t();
        auto _file_size = filesize(_file_name.toStdString().c_str()); // fileInfoList.at(i).size();
        // Директории с именами "." и ".." обрабатывать не нужно
        if (_file_name == "." || _file_name == "..")
            continue;
        // Увеличивается подсчитываемый размер директории
        _dir_size += _file_size;

        // Информация о файле добавляется в таблицу
        auto currentFileData = std::make_shared<FileData>(this, _file_name);
        //        currentFileData._name	= _file_name;
        //        currentFileData._time	= _file_time;
        //        currentFileData._size	= _file_size;
        _files_table << currentFileData;
    }
    // qDebug() << "In init trash " << filesTable.size() << "files";
    _is_inited = true;
}

// Функция должна вызываться после фактического добавления файла в корзину
// принимает имя файла без пути к директории
void TrashMonitoring::add_file(QString _file_name)
{
    // Выясняется время создания файла берется текущее, особой точности не нужно
    //    unsigned int _file_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());	// (QDateTime::currentDateTime()).toTime_t();

    // Выясняется размер файла
    //    QFile			currentFile(_path + "/" + _file_name);
    auto _file_size = filesize(QString(_path + "/" + _file_name).toStdString().c_str());
    ; // currentFile.size();

    // Увеличивается подсчитываемый размер директории
    _dir_size += _file_size;

    // Информация о файле добавляется в таблицу
    auto currentfiledata = std::make_shared<FileData>(this, _file_name);
    //    currentfiledata._name	= _file_name;
    //    currentfiledata._time	= _file_time;
    //    currentfiledata._size	= _file_size;
    _files_table.insert(0, currentfiledata);

    update();
}

void TrashMonitoring::update(void)
{
    // Исключается наиболее старый файл пока выполняется
    // условие что количество файлов слишком велико или
    // суммарный размер файлов превышает предельно допустимый размер корзины
    while (_files_table.size() > appconfig.trash_max_file_count() || _dir_size > appconfig.trash_size() * 1000000) {
        if (_files_table.size() <= 10) // Оставляется последний файл, какого бы размера он не был
            break;
        else
            remove_oldest_file();
    }
}

void TrashMonitoring::remove_file_fullpath(QString file_name_)
{
    //    QString _file_name = got ? _path + "/" + _files_table.last()._file_name : _path + "/" + found._file_name;

    qDebug() << "Remove file " << file_name_;
    if (QFile::exists(file_name_)) {
        if (!QFile::remove(file_name_)) { // Файл физически удаляется
            critical_error("In remove_file_fullpath. Can not delete file " + file_name_);
            exit(0);
        }
    }
}

void TrashMonitoring::remove_oldest_file(QString file_name_)
{
    std::shared_ptr<FileData> found(nullptr);
    bool got = false;
    if (file_name_ != "") {
        for (auto f : _files_table) {
            if (file_name_ == f->_name) {
                found = f;
                got = true;
                break;
            }
        }
    }
    QString _file_found = got ? _path + "/" + found->_name : _path + "/" + _files_table.last()->_name;

    qDebug() << "Remove file " << _file_found << " from trash";
    if (QFile::exists(_file_found)) {
        if (QFile::remove(_file_found)) { // Файл физически удаляется
            // Расчетный размер директории уменьшается на размер файла
            _dir_size = _dir_size - _files_table.last()->size();
            //
            QMutableListIterator<std::shared_ptr<FileData> > iter(_files_table);
            while (iter.hasNext()) {
                auto it = iter.next();
                if (it->_name == _file_found)
                    _files_table.removeOne(it);
            }
            // Файл удаляется из списка
            //	    _files_table.removeLast();
        } else {
            critical_error("In remove_oldest_file. Can not delete file " + _file_found);
            exit(0);
        }
    } else if (_files_table.size() > 10)
        _files_table.removeLast();
}

TrashMonitoring::FileData::FileData(TrashMonitoring* tm, const QString& n)
    : _trashmonitoring(tm)
    , _name(n)
    , _time([&] {
        unsigned int _file_time = 0;
        if (_name != "") {
            QFileInfo currentFile(_trashmonitoring->_path + "/" + _name);

            _file_time = currentFile.created().toTime_t();
        } else
            _file_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return _file_time;
    }())
    , _size([&] {
        std::ifstream::pos_type _file_size = 0;
        if (_name != "") {
            //                  QFileInfo	currentFile(_trashmonitoring->_path + "/" + _name);
            //                  _file_size = currentFile.size();

            //                  QFile file_from(_trashmonitoring->_path + "/" + _name);
            //                  if(file_from.open(QIODevice::ReadOnly)){
            //                      _file_size = file_from.size();	// when file does open.
            _file_size = filesize(QString(_trashmonitoring->_path + "/" + _name).toStdString().c_str());
            //                      file_from.close();
            //                  }
        }
        return _file_size;
    }())
{
}

std::ifstream::pos_type TrashMonitoring::FileData::size() const
{
    //                    QFile			currentFile(_trashmonitoring->_path + "/" + _name);
    //                    unsigned int	_file_size = currentFile.size();
    return const_cast<FileData*>(this)->_size = filesize(QString(_trashmonitoring->_path + "/" + _name).toStdString().c_str()); // const_cast<FileData *>(this)->_size = _file_size;
}
