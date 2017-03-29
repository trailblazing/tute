#ifndef ATTACHTABLEDATA_H
#define ATTACHTABLEDATA_H

#include <QDomElement>
#include <QList>
#include <QMetaType>

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include "models/attach_table/attach.h"
#include "models/record_table/record.h"

class r_t;
class AttachTableModel;

class AttachTableData {
	friend class Attach;

    public:
	AttachTableData(r_t* record_ = nullptr);
	AttachTableData(const AttachTableData& obj);
	AttachTableData& operator=(const AttachTableData& obj);
	// AttachTableData();
	virtual ~AttachTableData();

	void dom_to_data(QDomElement iDomElement);
	QDomElement dom_from_data(std::shared_ptr<QDomDocument> doc) const;

	void record(r_t* record); //boost::intrusive_ptr<r_t>
	void related_attach_table_model(AttachTableModel* model);
	void related_attach_table_model_only(AttachTableModel* model);

	void clear();
	int size() const;

	Attach attach(QString id); // Получение объекта аттача
	void
	add_attach(Attach attach);               // Добавление аттача в таблицу приаттаченных файлов
	void attach(QString id, Attach iAttach); // Изменение данных аттача
	void delete_attach(QString id);          // Удаление аттача по идентификатору

	int row_by_id(QString id);
	QString id_by_row(int row);

	QString file_name(int row);
	QString file_name_by_id(QString id);

	QString inner_file_name(int row);
	QString inner_file_name_by_id(QString id);

	QString full_inner_file_name(int row);
	QString full_inner_file_name_by_id(QString id);

	QString absolute_inner_file_name(int row);
	QString absolute_inner_file_name_by_id(QString id);

	qint64 file_size(int row);

	bool is_empty() const;
	bool is_lite() const;

	void switch_to_lite();
	void switch_to_fat();

	void print();

	bool is_record_crypt();
	void encrypt(unsigned int area);
	void decrypt(unsigned int area);

	void save_attach_files_to_directory(QString dirName);

	// Обновление ссылок на таблицу аттачей внутри аттачей
	void update_attach_table_back_link();
	std::shared_ptr<AttachTableData> merge(std::shared_ptr<AttachTableData> cut);

    protected:
	bool _lite_flag;

	// Перечень файлов и их свойств
	QList<Attach> _attach_table;

	// Какой записи принадлежит таблица файлов
	r_t* _record;

	AttachTableModel* _related_attach_table_model;
};

// егистрация в QVariant типа AttachTableData
Q_DECLARE_METATYPE(AttachTableData);

// егистрация в QVariant типа *AttachTableData
typedef AttachTableData* AttachTableDataPointer;
Q_DECLARE_METATYPE(AttachTableDataPointer);

#endif // ATTACHTABLEDATA_H
