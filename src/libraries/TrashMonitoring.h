#ifndef _TRASHMONITORING_H_
#define	_TRASHMONITORING_H_

#include <QWidget>
#include <QDir>


class TrashMonitoring {

public:
    TrashMonitoring(void);
    virtual ~TrashMonitoring(void);

    void init(QString _trash_path);

// Функция, вызываемая после фактического добавления файла в корзину
// принимает имя файла без пути к директории
    void add_file(QString _file_name);

    void update(void);

private:

    void remove_oldest_file(void);

    QString         _path;
    QDir            _dir;
    unsigned int    _dir_size;

    struct FileData {
        QString         _file_name;
        unsigned int    _file_time;
        unsigned int    _file_size;
    };

    QList<FileData>     _files_table;

};

#endif	/* _TRASHMONITORING_H_ */

