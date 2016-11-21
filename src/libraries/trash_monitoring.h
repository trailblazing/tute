#ifndef _TRASHMONITORING_H_
#define _TRASHMONITORING_H_

#include <memory>
#include <QWidget>
#include <QDir>

class DiskHelper;

class TrashMonitoring {
    public:
        TrashMonitoring(void);
        virtual ~TrashMonitoring(void);

        void init(QString _trash_path);

        // Функция, вызываемая после фактического добавления файла в корзину
        // принимает имя файла без пути к директории
        void add_file(QString _file_name);

        void update(void);
        std::shared_ptr<QFileInfo> recover_from_trash(std::shared_ptr<QFileInfo> target_file, bool force_remove = false);
        bool is_inited(){return _is_inited;}
    private:

        void remove_file_fullpath(QString file_name_);
        void remove_oldest_file(QString file_name_ = "");

        QString	_path;
        QDir _dir;
        unsigned int _dir_size;

        struct FileData {
            private:
                TrashMonitoring *_trashmonitoring;
            public:
                FileData(TrashMonitoring *tm, const QString &n);

                QString					_name	= "";
                unsigned int			_time	= 0;
                std::ifstream::pos_type	size() const {
//                    QFile			currentFile(_trashmonitoring->_path + "/" + _name);
//                    unsigned int	_file_size = currentFile.size();
                    return const_cast<FileData *>(this)->_size = filesize(QString(_trashmonitoring->_path + "/" + _name).toStdString().c_str());		// const_cast<FileData *>(this)->_size = _file_size;
                }
                friend bool operator ==(const FileData &fd0, const FileData &fd1){return fd0._name == fd1._name && fd0._time == fd1._time && fd0._size == fd1._size;}
                FileData(const FileData &fd){_name = fd._name;_time = fd._time;_size = fd.size();}
                const FileData &operator =(const FileData &fd){if(&fd != this){_name = fd._name;_time = fd._time;_size = fd.size();} return *this;}
            private:
                FileData();
                std::ifstream::pos_type _size = 0;
        };

        QList<std::shared_ptr<FileData> >        _files_table;
        bool _is_inited = false;
};

#endif /* _TRASHMONITORING_H_ */

