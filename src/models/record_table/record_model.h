#ifndef __RECORDTABLEMODEL_H__
#define __RECORDTABLEMODEL_H__

#include <memory>

#include <QAbstractTableModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <QtXml>

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <boost/serialization/strong_typedef.hpp>

#include "models/record_table/record_index.hxx"

// #include "models/tree/TreeItem.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

#define TABLE_DATA_ROLE Qt::UserRole + 10
#define ONE_RECORD_ROLE Qt::UserRole + 11
#define RECORD_ID_ROLE Qt::UserRole + 12
#define SORT_ROLE Qt::UserRole + 13

extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;

class Record;
class vector_t;
class i_t;
class tkm_t;
class rctrl_t;
class wn_t;
class ts_t;
class FindScreen;
class EditorWrap;
class AppConfig;
class rs_t;

struct pos_proxy;
struct pos_source;
struct index_proxy;
struct index_source;
struct id_value;

namespace web {
	class Docker;
	class Browser;
	class TabWidget;
	class WebView;
}

// struct pages_container {
// public:
// explicit pages_container(web::TabWidget *_tabmanager);
// ~pages_container();
// web::TabWidget      *tabmanager(){return _tabmanager;}

//// void browser_pages(ItemsFlat *_browser_pages);
//// ItemsFlat *browser_pages();
//// ItemsFlat *browser_pages()const;

// protected:
// web::TabWidget  *_tabmanager;
//// ItemsFlat           *_browser_pages;
// };

class RecordModel : public QAbstractTableModel // , public pages_container
{
#if QT_VERSION == 0x050600
	W_OBJECT(RecordModel)
#else
	Q_OBJECT
#endif
	// By the closed (private) function models can have access controller   // К
	// закрытым (private) функциям модели может иметь доступ контроллер
	friend class rctrl_t;
	friend class web::Docker;

    public:
	RecordModel(rctrl_t* record_controller); // , RecordScreen *_record_screen

	~RecordModel();

	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

	// Интерфейс модели, предоставление данных по указанному индексу
	// Interface models to provide data at the specified index
	QVariant data(const QModelIndex& index, int role) const;

	// Интерфейс модели, сохранение вводимых данных по указанному индексу
	// Interface model, saving data entry at the specified index
	bool setData(const QModelIndex& index, const QVariant& value, int role);

	// std::shared_ptr<RecordTable> table_data() {return _table;}

	// Интерфейс модели, сколько записей в таблице
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int size() const; // {return rowCount();}
	// Интерфейс модели, сколько столбцов в таблице
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	// QString field(int pos, QString name);
	void fields(int pos, QMap<QString, QString> data);

	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	index_source index(boost::intrusive_ptr<i_t> it) const;
	index_source fake_index(boost::intrusive_ptr<i_t> it) const;
	// for multi items link with unique page
	// boost::intrusive_ptr<TreeItem>
	// item_bounded(boost::intrusive_ptr<TreeItem> it) const;
	boost::intrusive_ptr<i_t>
	item(const std::function<bool(const id_value)> _equal) const;
	// boost::intrusive_ptr<TreeItem> item(boost::intrusive_ptr<TreeItem> it)
	// const;

	// int item_current(QString find_id);
	// bool remove_child(QString find_id);
	// bool remove_child(int index);
	// boost::intrusive_ptr<TreeItem> tree_item() {return _shadow_branch_root;}
	// boost::intrusive_ptr<TreeItem>	item(const pos_source _index);
	boost::intrusive_ptr<i_t> item(const pos_source _index) const;
	// boost::intrusive_ptr<TreeItem>	item(const id_value &id);
	// boost::intrusive_ptr<TreeItem>	item(const id_value &id) const;
	// boost::intrusive_ptr<TreeItem>	item(const QUrl &_url) const;	// lead
	// implicit construct
	// boost::intrusive_ptr<TreeItem>	item_fat(const pos_source index);
	// boost::intrusive_ptr<TreeItem> item(int pos) {return item(pos);}
	// boost::intrusive_ptr<TreeItem> item(int pos)const {return item(pos);}

	// void init_source_model(boost::intrusive_ptr<TreeItem> item,
	// RecordController *_record_controller, RecordScreen *_record_screen,
	// MainWindow *main_window, MetaEditor *_editor_screen);

	// void init_source_model(ItemsFlat *_browser_pages
	// , RecordController *_record_controller
	// , RecordScreen *_record_screen
	// , MainWindow *main_window
	// , MetaEditor *_editor_screen
	// );

	// void init_source_model(RecordController *_record_controller, RecordScreen
	// *_record_screen, MainWindow *main_window, MetaEditor *_editor_screen);

	// ItemsFlat *browser_pages()const {return pages_container::browser_pages();}
	rctrl_t* reocrd_controller() const;

	int count() const; // {return _tabmanager->count();}

	// boost::intrusive_ptr<TreeItem>	sibling_s(boost::intrusive_ptr<TreeItem>
	// it) const;// override
	boost::intrusive_ptr<i_t> current_item() const;
	index_source current_index() const;

	void position(pos_source _index);
	// PosSource position()const;
	pos_source position(id_value id) const;
	// PosSource position(boost::intrusive_ptr<TreeItem> item)const;

	Qt::ItemFlags flags(const QModelIndex& index) const;

    public slots:

    private:
	void reset_internal_data();

	//// Установка указателя на таблицу данных, с которой нужно работать модели
	// void reset_tabledata(std::shared_ptr<RecordTable> record_table);
	//// Ссылка на данные, с которыми работает модель
	// boost::intrusive_ptr<TreeItem> tree_item(void);

	// void tree_item(boost::intrusive_ptr<TreeItem> item);
	// void browser_pages(ItemsFlat *_browser_pages);
	// void root(boost::intrusive_ptr<TreeItem> item);
	void on_table_config_changed(void);

	// Добавление записей
	web::WebView*
	insert_new_item(boost::intrusive_ptr<i_t> _target_item); // , int mode = add_new_record_after

	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	void remove_child(boost::intrusive_ptr<i_t> it);

	int move(const pos_source pos, const pos_source target = pos_source(-1));
	// int	move_dn(const pos_source pos);

	// protected:

	// Указатель на таблицу конечных записей
	// std::shared_ptr<RecordTable>  _table; // flat one
	// TreeModelKnow *_browser_pages;  //
	// boost::intrusive_ptr<TreeItem> _shadow_branch_root;	// keep it flat

	rctrl_t* _record_controller;
	friend class rs_t;
	friend class web::TabWidget;
	friend struct RecordIndex;
};

#endif // __RECORDTABLEMODEL_H__
