#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include <QtXml>

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
// #include "TreeItem.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

// TreeModel - Это вспомогательный класс! От него наследуется KnowTreeModel

extern const char* global_root_id;

class vector_t;
class i_t;
class tkm_t;
class ts_t;
class tv_t;
struct Binder;
struct Linker;
struct TreeIndex;
struct index_tree;

class tm_t : public QAbstractItemModel {
#if QT_VERSION == 0x050600
	W_OBJECT(tm_t)
#else
	Q_OBJECT
#endif

    public:
	// struct  delegater {
	// private:
	// boost::intrusive_ptr<TreeItem> _item;
	// QUrl _find_url;
	// QString _id = "";
	// public:
	// explicit delegater(boost::intrusive_ptr<TreeItem> _item): _item(_item)
	// {_equal = [&](TreeItem * it) {return _item.get() == it;};}
	// explicit delegater(const QUrl &_find_url):  _find_url(_find_url) {_equal =
	// [&](TreeItem * it) {return _find_url.toString() == it->field("url");};}
	// explicit delegater(const QString &_id): _id(_id) {_equal = [&](TreeItem *
	// it) {return _id == it->field("id");};}

	////        bool (*equal)(TreeItem * it);
	// std::function<bool(TreeItem *)> _equal;

	// };

	// QModelIndex index(delegater _del) const;
	// boost::intrusive_ptr<TreeItem> item(const delegater &_del)const;

	tm_t(tv_t* parent);
	tm_t(boost::intrusive_ptr<i_t> root_item, tv_t* parent);
	~tm_t(void);

	QVariant data(const QModelIndex& _index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	QModelIndex index(int row, int column, const QModelIndex& current_index = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& _index) const;

	int rowCount(const QModelIndex& _index = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex& _index) const;
	bool setData(const QModelIndex& _index, const QVariant& value, int role = Qt::EditRole);
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole);

	bool insertRows(int position, int rows, const QModelIndex& _index_parent = QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex());

	// bool is_item_valid(QStringList path) const;
	index_tree index(const std::function<bool(boost::intrusive_ptr<const Linker>)>& _equal) const;
	index_tree index(boost::intrusive_ptr<const i_t> _item) const;
	// index_tree	fake_index(boost::intrusive_ptr<TreeItem> it) const;
	void update_index(const index_tree& _index);
	// QModelIndex index(const QUrl &find_url)const;
	// QModelIndex index(const QString &id)const;

	// Возвращение указателя на Item-элемент с указанным index
	// Где index - это индекс объекта в терминах структуры модель-вид
	boost::intrusive_ptr<i_t> item(const QModelIndex& _index) const;
	// Возвращение указателя на Item-элемент с указанным путем
	// в виде последовательности идентификаторов
	boost::intrusive_ptr<i_t> item(QStringList path) const;

	boost::intrusive_ptr<i_t> item(const std::function<bool(boost::intrusive_ptr<const i_t>)>& _equal) const;

	// boost::intrusive_ptr<TreeItem> find_recursive(const QUrl &find_url) const;
	// boost::intrusive_ptr<TreeItem> find_recursive(const QString &id)const;

	void emit_datachanged_signal(const QModelIndex& _index);

	// void root_item(boost::intrusive_ptr<TreeItem> it) {_root_item = it;}
	boost::intrusive_ptr<i_t> root_item() const; // {return _root_item;}
	QString session_id()
	{
		return _session_id;
	}

	void session_id(boost::intrusive_ptr<TreeIndex> modelindex);

    protected:
	boost::intrusive_ptr<i_t> _root_item; // Ссылка на первый (корневой) item-объект

    private:
	// QModelIndex index_recursive(QModelIndex _index,
	// boost::intrusive_ptr<TreeItem> item, int mode);

	// Element over which the cursor will carry. Used in the Drag And Drop.    //
	// Элемент, над которым проносят курсор. Используется при Drag And Drop.
	QModelIndex _cursor_over_index;
	QString _session_id = global_root_id;
};

#endif
