#ifndef __ATTACH_H__
#define __ATTACH_H__

#include <QByteArray>
#include <QDomElement>
#include <QMap>
#include <QString>
#include <memory>

class AttachTableData;

class Attach {
	friend class AttachTableData;

	public:
	enum EncryptDecryptArea { areaMemory = 0x1,
		                      areaFile = 0x2,
		                      areaAll = 0xFF };

	Attach(AttachTableData *iParentTable);
	Attach(QString iType, AttachTableData *iParentTable);
	virtual ~Attach();

	void dom_to_data(QDomElement iDomElement);
	QDomElement dom_from_data(std::shared_ptr<QDomDocument> doc) const;

	QString getField(QString name) const;
	void setField(QString name, QString value);

	// абота с именем файла
	void setFileName(QString iFileName);
	QString getInnerFileName() const;
	QString getFullInnerFileName() const;
	QString getFullInnerDirName() const;
	QString getAbsoluteInnerFileName() const;

	bool setLink(QString iLink);

	qint64 getFileSize() const;

	// todo: подумать, может унаследовать Attach и Record от общего класса
	// LiteFatObject
	bool isEmpty() const;
	bool isLite() const;
	void switchToLite();
	void switchToFat();

	void pushFatDataToDisk();
	void pushFatDataToDirectory(QString dirName);
	void popFatDataFromDisk();

	bool copyFileToBase(QString iFileName);
	void removeFile();

	void encrypt(unsigned int area = areaAll);
	void decrypt(unsigned int area = areaAll);

	protected:
	void init(AttachTableData *iParentTable);
	void setParentTable(AttachTableData *iParentTable);  // Защищенный метод,
	                                                     // который может вызвать
	                                                     // только этот класс и
	                                                     // AttachTableData

	QStringList fieldAvailableList(void) const;
	QStringList fieldCryptedList(void) const;
	QStringList typeAvailableList(void) const;

	void setFieldSource(QString name, QString value);

	bool _lite_flag;

	AttachTableData *_parent_table;  // Указатель на таблицу приаттаченных файлов,
	                                 // которой принадлежит данный аттач

	QMap<QString, QString> _fields;

	QByteArray
	    _file_content;  // Содержимое файла, используется в режиме полных данных
	friend inline bool operator==(const Attach &left, const Attach &right);
};

inline bool operator==(const Attach &left, const Attach &right) {
	// bool fields_has_defferences = false;

	// for(auto i : right._fields.keys()) {
	////        for(auto j : left._fields.keys()) {
	// if(left._fields[i].isEmpty()) {fields_has_defferences = true; break;}

	////        }
	// }

	return left._parent_table == right._parent_table &&
	    left._lite_flag == right._lite_flag &&
	    left._fields == right._fields  // !fields_has_defferences
	    && left._file_content == right._file_content;
}

#endif  // __ATTACH_H__
