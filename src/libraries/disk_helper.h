#ifndef DISKHELPER_H
#define DISKHELPER_H

#include <QByteArray>
#include <QFileInfo>
#include <QMap>
#include <QString>
#include <QStringList>
#include <memory>

// Класс, помогающий работать с дисковой подсистемой

class DiskHelper {
public:
	DiskHelper();

	static void remove_directory_to_trash(QString nameDirFrom);
	static void backup(QString file_name_from);
	static std::shared_ptr<QFileInfo> copy_file_to_data_folder(const QString &file_name_to, const QString &file_name_from);
	static QString copy_file_to_trash(QString file_name_from);
	static QString create_directory(QString path_name, QString dir_name);
	static QString create_temp_directory(void);
	static bool remove_directory(const QString &dirName);
	static bool copy_directory(const QString &fromName, const QString &toName);
	static QMap<QString, QString> get_strings_from_directory(QString dirName, QString fileMask);
	static QMap<QString, QByteArray> get_files_from_directory(QString dirName, QString fileMask);
	static bool save_strings_to_directory(QString dirName, QMap<QString, QString> fileList);
	static bool save_files_to_directory(QString dirName, QMap<QString, QByteArray> fileList);
	static bool copy_file_force(QString source, QString target);
	static std::shared_ptr<QFileInfo> recover_ini_file(std::shared_ptr<QFileInfo> target_file, bool force_remove = false);
};

#endif // DISKHELPER_H
