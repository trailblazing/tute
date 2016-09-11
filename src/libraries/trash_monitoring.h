#ifndef _TRASHMONITORING_H_
#define _TRASHMONITORING_H_

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

	void	update(void);
	void	recover_from_trash();
	bool is_inited(){return _is_inited;}
    private:

	void	remove_file_fullpath(QString file_name_);
	void	remove_oldest_file(QString file_name_ = "");

	QString		_path;
	QDir		_dir;
	unsigned int	_dir_size;

	struct FileData {
	    QString		_name;
	    unsigned int	_time;
	    unsigned int	_size;
	    friend bool operator ==(const FileData &fd0, const FileData &fd1){return fd0._name == fd1._name && fd0._time == fd1._time && fd0._size == fd1._size;}
	};

	QList<FileData>		_files_table;
	bool			_is_inited = false;
};

#endif /* _TRASHMONITORING_H_ */

