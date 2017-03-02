
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

extern std::shared_ptr<AppConfig> appconfig;
extern std::shared_ptr<gl_para> gl_paras;

TrashMonitoring::TrashMonitoring(void)
{
}

TrashMonitoring::~TrashMonitoring(void)
{
}

std::shared_ptr<QFileInfo> TrashMonitoring::recover_from_trash(std::shared_ptr<QFileInfo> target_file, bool force_remove)
{
    std::shared_ptr<QFileInfo> result(nullptr); // = false;
    auto need = false;
    auto _file_name = target_file->fileName(); // "index.xml";
    auto target_file_full_name = target_file->filePath(); // appconfig->tetra_dir() + "/" + _file_name;
    // if(QFile::exists(appconfig->tetra_dir() + "/" + _file_name))remove_oldest_file(_file_name);
    if (QFile::exists(target_file_full_name)) {
        // QFileInfo currentFile(_trashmonitoring->_path + "/" + _name);
        // _file_size = currentFile.size();

        // unsigned int _file_size = 0;
        // QFile		file_from(file_name_fullpath);
        // if(file_from.open(QIODevice::ReadOnly)){
        auto _file_size = filesize(target_file_full_name.toStdString().c_str()); // file_from.size();		// when file does open.
        // file_from.close();
        // }
        if (0 >= _file_size) {
            if (!QFile::remove(target_file_full_name)) { // Файл физически удаляется
                // }else{
                critical_error("In recover_from_trash. Can not delete file " + target_file_full_name);
                exit(0);
            } else
                need = true;
        }
    } else
        need = true;
    if (need || force_remove) {
        // auto	_main_program_file = globalparameters.main_program_file();
        // QFileInfo	main_program_file_info(_main_program_file);
        // QString	full_current_path = main_program_file_info.absolutePath();
        auto recover_from_source = [&] {
            auto trash_file_full_name = gl_paras->root_path() + "/" + QDir(appconfig->trash_dir()).dirName() + "/" + _file_name;
            if (!QDir(gl_paras->root_path() + "/" + QDir(appconfig->trash_dir()).dirName()).exists())
                QDir(gl_paras->root_path()).mkdir(QDir(appconfig->trash_dir()).dirName());
            if (!QFile::copy(QString(":/resource/standarddata/") + _file_name, trash_file_full_name))
                throw std::runtime_error("Can not copy index.xml");
            else
                QFile::setPermissions(gl_paras->root_path() + "/" + QDir(appconfig->trash_dir()).dirName() + "/" + _file_name, QFile::ReadUser | QFile::WriteUser);
            // bool succedded = DiskHelper::save_strings_to_directory(full_current_path + "/trash", globalparameters.index_xml());
            // assert(succedded);
            add_file(_file_name); // globalparameters.index_xml().keys()[0]
            //			result = std::make_shared<QFileInfo>(target_file_full_name); // target_file;
            _files_table << std::make_shared<FileData>(this, trash_file_full_name);
        };
        if (_files_table.size() == 0)
            recover_from_source();
        // else{
        std::function<std::shared_ptr<FileData>()> get_most_recent_file_data = [&] {
            decltype(_files_table) left;
            for (auto f : _files_table) {
                // if(f->size() >= r->size()) r = f;
                // else
                if (f->size() > 0)
                    left.append(f); // r = f;
            }
            std::shared_ptr<FileData> r(nullptr);
            if (left.size() > 0) {
                r = std::make_shared<FileData>(this, left[0]->_name);
                for (auto f : left)
                    if (f->_time >= r->_time)
                        r = f;
            } else {
                recover_from_source();
                r = get_most_recent_file_data();
            }
            return r;
        };
        auto file_data = get_most_recent_file_data(); // _files_table.first();
        auto s = gl_paras->root_path() + "/" + QDir(appconfig->trash_dir()).dirName() + "/" + file_data->_name;
        if (file_data)
            result = DiskHelper::copy_file_force(s, target_file_full_name);
        // result = target_file;
        // }
    }
    return result;
}

void TrashMonitoring::init(QString _trash_path)
{
    // auto		_main_program_file = globalparameters.main_program_file();
    // QFileInfo	main_program_file_info(_main_program_file);
    // QString		full_current_path = main_program_file_info.absolutePath();
    // Инит объекта директории с указанным путем
    _dir.setPath(_trash_path);
    if (!_dir.exists()) {
        DiskHelper::create_directory(gl_paras->root_path() // full_current_path
            ,
            "trash");
        // critical_error("Can not open trash directory " + _trash_path);
        // exit(1);
    }
    _path = _trash_path; // Имя директории запоминается

    // азмер директории
    // _dir_size = 0;

    // Получение списка информации о файлах
    QFileInfoList fileInfoList = _dir.entryInfoList(QDir::Files, QDir::Time);
    // Перебор всех файлов в полученном списке
    for (int i = 0; i < fileInfoList.size(); i++) {
        QString _file_name = fileInfoList.at(i).fileName();
        // unsigned int	_file_time	= fileInfoList.at(i).created().toTime_t();
        auto _file_size = filesize(_file_name.toStdString().c_str()); // fileInfoList.at(i).size();
        // Директории с именами "." и ".." обрабатывать не нужно
        if (_file_name == "." || _file_name == "..")
            continue;
        // Увеличивается подсчитываемый размер директории
        _dir_size += _file_size;

        // Информация о файле добавляется в таблицу
        auto currentFileData = std::make_shared<FileData>(this, _file_name);
        // currentFileData._name	= _file_name;
        // currentFileData._time	= _file_time;
        // currentFileData._size	= _file_size;
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
    // unsigned int _file_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());	// (QDateTime::currentDateTime()).toTime_t();

    // Выясняется размер файла
    // QFile			currentFile(_path + "/" + _file_name);
    auto _file_size = filesize(QString(_path + "/" + _file_name).toStdString().c_str());
    ; // currentFile.size();

    // Увеличивается подсчитываемый размер директории
    _dir_size += _file_size;

    // Информация о файле добавляется в таблицу
    auto currentfiledata = std::make_shared<FileData>(this, _file_name);
    // currentfiledata._name	= _file_name;
    // currentfiledata._time	= _file_time;
    // currentfiledata._size	= _file_size;
    _files_table.insert(0, currentfiledata);

    update();
}

void TrashMonitoring::update(void)
{
    // Исключается наиболее старый файл пока выполняется
    // условие что количество файлов слишком велико или
    // суммарный размер файлов превышает предельно допустимый размер корзины
    while (_files_table.size() > appconfig->trash_max_file_count() || _dir_size > appconfig->trash_size() * 1000000) {
        if (_files_table.size() <= 10) // Оставляется последний файл, какого бы размера он не был
            break;
        else
            remove_oldest_file();
    }
}

void TrashMonitoring::remove_file_fullpath(QString file_name_)
{
    // QString _file_name = got ? _path + "/" + _files_table.last()._file_name : _path + "/" + found._file_name;

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
    if (_files_table.size() > 10) {
        std::shared_ptr<FileData> found(_files_table.last());
        if (file_name_ != "") {
            for (auto f : _files_table) {
                if (file_name_ == f->_name) {
                    found = f;
                    break;
                }
            }
        } else {
            for (auto f : _files_table) {
                if (0 == f->size()) {
                    found = f;
                    break;
                }
            }
        }
        QString _file_found = _path + "/" + found->_name; //got ? _path + "/" + found->_name : _path + "/" + _files_table.last()->_name;

        qDebug() << "Remove file " << _file_found << " from trash";
        //		if(0 == found->size()){
        //			if(QFile::exists(_file_found)){
        //				if(QFile::remove(_file_found)){  // Файл физически удаляется
        //					if(_files_table.removeOne(found)) _dir_size = _dir_size - found->size();
        //				}else{
        //					critical_error("In remove_oldest_file. Can not delete file \"" + _file_found + "\" which file size is zero");
        ////				exit(0);
        //				}
        //			}
        //		}else
        if (QFile::exists(_file_found)) {
            if (QFile::remove(_file_found)) { // Файл физически удаляется
                // асчетный размер директории уменьшается на размер файла
                //				//
                //				QMutableListIterator<std::shared_ptr<FileData> > iter(_files_table);
                //				while(iter.hasNext()){
                //					auto it = iter.next();
                //					if(it->_name == _file_found) _files_table.removeOne(it);
                //				}
                if (_files_table.removeOne(found))
                    _dir_size = _dir_size - found->size();
                // Файл удаляется из списка
                // _files_table.removeLast();
            } else {
                critical_error("In remove_oldest_file. Can not delete file \"" + _file_found + "\"");
                exit(0);
            }
        } //else if(_files_table.size() > 10) _files_table.removeLast();
    }
}

TrashMonitoring::FileData::FileData(TrashMonitoring* tm, const QString& name_)
    : _trashmonitoring(tm)
    , _name(name_)
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
            // QFileInfo	currentFile(_trashmonitoring->_path + "/" + _name);
            // _file_size = currentFile.size();

            // QFile file_from(_trashmonitoring->_path + "/" + _name);
            // if(file_from.open(QIODevice::ReadOnly)){
            // _file_size = file_from.size();	// when file does open.
            _file_size = filesize(QString(_trashmonitoring->_path + "/" + _name).toStdString().c_str());
            // file_from.close();
            // }
        }
        return _file_size;
    }())
{
}

std::ifstream::pos_type TrashMonitoring::FileData::size() const
{
    // QFile			currentFile(_trashmonitoring->_path + "/" + _name);
    // unsigned int	_file_size = currentFile.size();
    return const_cast<FileData*>(this)->_size = filesize(QString(_trashmonitoring->_path + "/" + _name).toStdString().c_str()); // const_cast<FileData *>(this)->_size = _file_size;
}

bool operator==(const TrashMonitoring::FileData& fd0, const TrashMonitoring::FileData& fd1)
{
    return fd0._name == fd1._name && fd0._time == fd1._time && fd0._size == fd1._size && fd0._trashmonitoring == fd1._trashmonitoring;
}

TrashMonitoring::FileData::FileData(const TrashMonitoring::FileData& fd)
{
    _name = fd._name;
    _time = fd._time;
    _size = fd.size();
    _trashmonitoring = fd._trashmonitoring;
}

const TrashMonitoring::FileData& TrashMonitoring::FileData::operator=(const TrashMonitoring::FileData& fd)
{
    if (&fd != this) {
        _name = fd._name;
        _time = fd._time;
        _size = fd.size();
        _trashmonitoring = fd._trashmonitoring;
    }
    return *this;
}
