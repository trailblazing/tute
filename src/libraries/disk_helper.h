#ifndef DISKHELPER_H
#define DISKHELPER_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QByteArray>

// Класс, помогающий работать с дисковой подсистемой

class DiskHelper {
public:
    DiskHelper();

    static void remove_directory_to_trash(QString nameDirFrom);
    static void remove_file_to_trash(QString fileNameFrom);
    static QString copy_file_to_data(QString file_name_from);
    static QString copy_file_to_trash(QString file_name_from);
    static QString create_temp_directory(void);
    static bool remove_directory(const QString &dirName);
    static bool copy_directory(const QString &fromName, const QString &toName);
    static QMap<QString, QByteArray> get_files_from_directory(QString dirName, QString fileMask);
    static bool save_files_to_directory(QString dirName, QMap<QString, QByteArray> fileList);

};

#endif // DISKHELPER_H
