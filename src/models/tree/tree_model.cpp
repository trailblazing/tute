
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

//#include "models/tree/tree_item.dec"

#include "libraries/global_parameters.h"
#include "main.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/tree/tree_know_model.h"
#include "tree_item.h"
#include "tree_model.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

#include "controllers/record_table/record_controller.h"
#include "libraries/clipboard_branch.h"
#include "libraries/clipboard_records.h"
#include "libraries/crypt/password.h"
#include "libraries/global_parameters.h"
#include "main.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "views/browser/docker.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"

extern std::shared_ptr<gl_para> gl_paras;
extern std::shared_ptr<AppConfig> appconfig;
extern const char* clipboard_items_root;

// TreeModel - Это модель данных, которая работает с видом TreeScreen
// От него наследуется класс KnowTreeModel

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(tm_t)
#endif

tm_t::tm_t(tv_t* parent)
    : QAbstractItemModel(parent)
{
	return;
}

tm_t::tm_t(boost::intrusive_ptr<i_t> root_item, tv_t* parent)
    : QAbstractItemModel(parent)
    , _root_item(root_item)
{
}

tm_t::~tm_t(void)
{
	return;
}

int tm_t::columnCount(const QModelIndex& parent) const
{
	// Q_UNUSED(itemIndex);

	//// Ранее число столбцов вычислялось исходя из
	//// количества полей в корневом элементе
	// Previously, the number of columns is calculated from
	// Number of fields in the root element
	//// return rootItem->fieldCount();

	// return 1;

	int result = 1;
	int root_path_size = _root_item->path_list<id_key>().size();
	int currnet_index_path_size =
	    parent.isValid() ? static_cast<i_t*>(parent.internalPointer())->path_list<id_key>().size() : root_path_size;

	// if(parent.isValid()) {

	// currnet_index_path_size = static_cast<TreeItem
	// *>(parent.internalPointer())->path_list().size();
	// int columncount = currnet_index_path_size - root_path_size;
	// assert(columncount > 0);
	// result = columncount;   // static_cast<TreeItem
	// *>(parent.internalPointer())->path_list().size() -
	// _root_item->path_list().size();
	// } else {
	// result = _root_item->path_list().size() - _root_item->path_list().size();
	// }
	result = currnet_index_path_size - root_path_size + 1;

	return result;
}

QVariant tm_t::data(const QModelIndex& _index, int role) const
{
	// Если индекс невалиден, возвращается несуществующий элемент
	if (!_index.isValid())
		return QVariant();
	// Если запрашивается окраска текста элемента
	if (role == Qt::ForegroundRole) {
		boost::intrusive_ptr<i_t> it = item(_index);
		if (it->id() == _session_id &&
		    _index != gl_paras->tree_screen()->view()->current_index())
			return QColor(Qt::red);
		else if (it->id() == _session_id && _index == gl_paras->tree_screen()->view()->current_index())
			return QColor(Qt::cyan);
		else if (it->count_direct() > 0)
			return QColor(Qt::black); // Если узел содержит таблицу конечных записей
		else
			return QColor(Qt::darkGray); // Ветка без таблицы конечных записей
	}
	if (role == Qt::BackgroundRole)
		if (_index == _cursor_over_index)
			return QColor(Qt::gray);
	// Если запрашивается содержимое текста элемента
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		boost::intrusive_ptr<i_t> it = item(_index);

		return QVariant(it->field<dynamic_name_key>()); // "dynamicname"  //
								// Запрашивается строка
								// имени с количеством
								// элементов
	}
	// Если запрашиваются элементы оформления
	if (role == Qt::DecorationRole) {
		boost::intrusive_ptr<i_t> it = item(_index);
		// Если ветка зашифрована
		if (it->field<crypt_key>() == crypt_value(true)) {
			// Если пароль не введен, доступа к ветке нет
			if (gl_paras->crypt_key().length() == 0)
				return QIcon(":/resource/pic/branch_closed.svg");
			else
				return QIcon(":/resource/pic/branch_opened.svg");
		}
	}
	return QVariant();
}

Qt::ItemFlags tm_t::flags(const QModelIndex& _index) const
{
	if (!_index.isValid())
		return Qt::ItemIsEnabled;
	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

// Получение индекса элемента, который задан через parent и
// номер строки и столбца относительно parent (нумерация с нуля)
// Загадочный метод, надо еще подумать что он делает на самом деле

// Get the index of the element that is defined with parent and Row and column
// number relative to parent (numbering from zero)
// Mysterious method, we must also think that he does in fact
QModelIndex tm_t::index(int row, int column, const QModelIndex& current_index) const
{
	QModelIndex result;
	// if(current_index.isValid() && current_index.column() != 0) {
	// return QModelIndex();
	// }
	if (0 <= current_index.column())
		assert(current_index.isValid());
	// if(0 == current_index.column())assert(!current_index.isValid());
	if (!current_index.isValid() ||
	    (0 <= current_index.column())) { // if(!current_index.isValid() || (0 ==
		// current_index.column())) {
		boost::intrusive_ptr<i_t> current_item =
		    current_index.isValid() ? this->item(current_index) : _root_item; // || 0 < current_index.column()
		if (row >= 0 && row < current_item->count_direct()) {
			boost::intrusive_ptr<i_t> child_item = current_item->child_direct(row);
			if (child_item)
				result =
				    createIndex(row, column, static_cast<void*>(child_item.get()));
			// else {
			// return QModelIndex();
			// }
		}
	}
	return result;
}

// does not must be succeeded
index_tree tm_t::index(
    const std::function<bool(boost::intrusive_ptr<const Linker>)>& _equal)
    const
{
	// QModelIndex result;

	std::function<QModelIndex(
	    QModelIndex, std::function<bool(boost::intrusive_ptr<const Linker>)>,
	    const int)>
	    index_recursive = [&](
		QModelIndex _index,
		std::function<bool(boost::intrusive_ptr<const Linker>)> _equal,
		const int mode) {
		    static QModelIndex find_index;
		    if (0 != mode) {
			    auto it = _index.isValid() ? this->item(_index) : _root_item; // how about _index itself?
											  // as a child of root, it
											  // will ever checked
			    for (int i = 0; i < it->count_direct(); i++) {
				    QModelIndex _index_child = createIndex(
					i, 0, static_cast<void*>(
						  it->child_direct(i)
						      .get())); // index(i, 0, _index);		//
				    if (_equal(it->linker_direct(i))) {
					    find_index = _index_child;
					    break;
				    } else {
					    index_recursive(_index_child, _equal, 1);
					    if (find_index.isValid())
						    if (_equal(item(find_index)->linker()))
							    break;
				    }
			    }
		    } else
			    find_index = QModelIndex();
		    return find_index;
	    };

	assert(_root_item);

	index_recursive(QModelIndex(), _equal, 0);
	// result = index_recursive(QModelIndex(), _equal);

	return index_tree(index_recursive(QModelIndex(), _equal, 1));
}

//// does not must be succeeded
// QModelIndex TreeModel::index(delegater _del) const
// {
// QModelIndex result;
////    assert(!result.isValid());

// std::function<QModelIndex(QModelIndex, delegater)>            // , int
// index_recursive = [&](QModelIndex _index, delegater _del    // , int mode
// ) {
////static
// QModelIndex find_index;
//// static
////        bool is_find = false;

////        if(!is_find) { //return find_index;

////            if(mode == 0) {
////                is_find = false;    // return QModelIndex();
////            } else  //        assert(_index.isValid());
////                if(mode == 1) { // is the _index valid?
////            // If the node pointer coincides with the given item    // Если
///указатель узла совпадает с заданным item
////            TreeItem *index_item = static_cast<TreeItem
///*>(_index.internalPointer());

////            if(_del._equal(index_item)  //_del.get() == index_item
////              ) {
////                is_find = true;
////                find_index = _index;
////                return find_index;
////            } else if(_index.isValid()) {
//// Иначе указатель узла не совпадает с заданным
//// и нужно рекурсивно искать далее
//// Otherwise, the node pointer does not match with the specified
//// Need to recursively search for and further

////                    int rows = _index.row();

////                    for(int i = 0; i < _index.row(); i++) {
////                        index_recursive(_index.child(i, 0), item, 1);
////                    }

////                QModelIndex first_child = index(0, 0, _index);  // _index
///may be invalid

////                //                if(first_child.isValid()) {
////                for(int i = 0; i < item->child_count(); i++) {
////                    index_recursive(
////                        index(i, 0, _index) // first_child.sibling(i, 0)
////                        , item
////                        , 1
////                    );
////                }

// auto it = this->item(_index);   // how about _index itself?

// for(int i = 0; i < it->count_direct(); i++) {
// auto _index_child = index(i, 0, _index);    // createIndex(j, 0,
// static_cast<void *>(_root_item->child(j).get()));

// if(_del._equal(it->item_direct(i).get())) {       //_del.get() ==
// it->child(i).get()
//// is_find = true;
// find_index = _index_child;
// break;  //return find_index;
// } else {
// find_index = index_recursive(_index_child, _del);    // , 1

//// is_find = _del._equal(item(find_index).get());    // find_index.isValid();

// if(_del._equal(item(find_index).get())) {
// break;
// }
// }
// }

////                for(int i = 0; i < _index.row(); i++) {
////                    index_recursive(_index.child(i, 0), _item, 1);
////                }
////            }

////                }
////        }

// return find_index;  // QModelIndex();
// };

// assert(_root_item);
////    //    // Initialize a recursive function  // Инициализация рекурсивной
///функции
////    //    index_recursive(QModelIndex(), _del, 0);

////    //    QModelIndex _index_root;

////    //    _index_root = index(0, 0, QModelIndex());

////    for(int i = 0; i < _root_item->current_count(); i++) {
////        auto _index_child = index(i, 0, QModelIndex());  // createIndex(j,
///0, static_cast<void *>(_root_item->child(j).get())); //
///index_recursive(_idx, item, 1);

////        assert(_index_child.isValid());

////        if(_del._equal(_root_item->child(i).get())) {  //_del.get() ==
///_root_item->child(j).get()
////            // index_recursive(_idx, _item, 1);
////            result = _index_child;
////            break;
////        } else {
////            result = index_recursive(_index_child, _del, 1);

////            if(_del._equal(item(result).get())) { //result.isValid()
////                break;
////            }
////        }
////    }

// result = index_recursive(QModelIndex(), _del);

// return result;
// }

// boost::intrusive_ptr<TreeItem> TreeModel::item(const delegater &_del)const
// {
////    boost::intrusive_ptr<TreeItem> result = _root_item; // (nullptr);
////    QModelIndex idx = index(_del);

////    if(idx.isValid()) {
////        result = item(idx);
////    }

// return item(index(_del));  // result;
// }

index_tree tm_t::index(boost::intrusive_ptr<const i_t> _item) const
{
	index_tree result;
	// assert(!result.isValid());

	std::function<QModelIndex(boost::intrusive_ptr<const i_t>, boost::intrusive_ptr<const i_t>, const int)> // QModelIndex, int
	    index_recursive = [&](boost::intrusive_ptr<const i_t> _parent,                                      // QModelIndex _index
		boost::intrusive_ptr<const i_t> _item,
		const int mode) {
		    static QModelIndex find_index;
		    // static
		    // bool is_find = false;
		    // if(!is_find) { // return find_index;
		    // if(mode == 0) {
		    // is_find = false;    // return QModelIndex();
		    // } else  //        assert(_index.isValid());
		    // if(mode == 1) { // is the _index valid?
		    // If the node pointer coincides with the given item    // Если
		    // указатель узла совпадает с заданным item
		    // Иначе указатель узла не совпадает с заданным
		    // и нужно рекурсивно искать далее
		    // Otherwise, the node pointer does not match with the specified
		    // Need to recursively search for and further
		    // int rows = _index.row();
		    // for(int i = 0; i < _index.row(); i++) {
		    // index_recursive(_index.child(i, 0), item, 1);
		    // }
		    // QModelIndex first_child = index(0, 0, _index);  // _index may be
		    // invalid
		    ////                if(first_child.isValid()) {
		    // for(int i = 0; i < item->child_count(); i++) {
		    // index_recursive(
		    // index(i, 0, _index) // first_child.sibling(i, 0)
		    // , item
		    // , 1
		    // );
		    // }
		    if (0 != mode) {
			    auto it = _parent ? _parent : _root_item; // _index.isValid() ?
								      // this->item(_index) : _root_item;
								      // // TreeItem *index_item =
								      // static_cast<TreeItem
								      // *>(_index.internalPointer());    //
								      // same
			    // assert(_index.row() == it->current_count());  // wrong
			    for (int i = 0; i < it->count_direct(); i++) { // _index.row()    //
				    auto child = it->child_direct(i);
				    auto _index_child = createIndex(
					i, 0, static_cast<void*>(child.get())); // index(i, 0, _index);
				    //// index_recursive(_idx, item, 1);
				    // assert(static_cast<TreeItem *>(_index_child.internalPointer()) ==
				    // it->child_direct(i).get());
				    if (child == _item) { // if(_item.get() == static_cast<TreeItem
					    // *>(_index_child.internalPointer())){
					    // || _item->id() == static_cast<TreeItem
					    // *>(_index_child.internalPointer())->id() // it->child(i).get()
					    // index_recursive(_idx, _item, 1);
					    find_index = _index_child;
					    // is_find = true;
					    // return find_index;
					    break;
				    } else {
					    // return
					    index_recursive(child, _item, 1); // _index_child , 1
					    // is_find = find_index.isValid();
					    if (find_index.isValid())
						    if (_item.get() ==
							static_cast<i_t*>(find_index.internalPointer()))
							    break; // same as find_index.isValid()
				    }
			    }
		    } else
			    find_index = QModelIndex();
		    // for(int i = 0; i < _index.row(); i++) {
		    // index_recursive(_index.child(i, 0), _item, 1);
		    // }
		    // }

		    // }
		    // }

		    return find_index; // QModelIndex();
	    };

	// auto index_no_recursive = [&](QModelIndex _index,
	// boost::intrusive_ptr<const TreeItem> _item){
	// QModelIndex find_index;

	// return find_index;
	// };

	assert(_root_item);
	//// Initialize a recursive function  // Инициализация рекурсивной функции
	// index_recursive(QModelIndex(), _item, 0);
	// QModelIndex _index_root;
	// _index_root = index(0, 0, QModelIndex());
	// for(int i = 0; i < _root_item->current_count(); i++) {
	// auto _index_child = index(i, 0, QModelIndex());  // createIndex(j, 0,
	// static_cast<void *>(_root_item->child(j).get())); // same as current
	////        index_recursive(_idx, item, 1);
	// assert(_index_child.isValid());
	// assert(_root_item->child(i).get() == static_cast<TreeItem
	// *>(_index_child.internalPointer()));
	// if(_item.get() == static_cast<TreeItem *>(_index_child.internalPointer())
	// || _item->id() == static_cast<TreeItem
	// *>(_index_child.internalPointer())->id()   // _root_item->child(i).get()
	// ) { // index_recursive(_idx, _item, 1);
	// result = _index_child;
	// break;
	// } else {
	// result = index_recursive(_index_child, _item, 1);
	// if(item(result).get() == _item.get() //result.isValid()
	// )break;
	// }
	// }
	index_recursive(_root_item, _item, 0);
	if (_item)
		result = index_tree(index_recursive(_root_item, _item, 1)); // QModelIndex()
	// assert(result.isValid());

	// return index_recursive(QModelIndex(), _item, 1); // from default index?
	return result;
}

// index_tree tm_t::fake_index(boost::intrusive_ptr<TreeItem> it) const {
// index_tree result;
// if(it) result = createIndex(0, 0, static_cast<void *>(it.get()));
// return result;
// }

// Обновление на экране ветки и подветок
void tm_t::update_index(const index_tree& _index)
{
	if (static_cast<QModelIndex>(_index).isValid()) {
		// auto _source_model = _tree_view->source_model();
		// Для корневой ветки дается команда чтобы модель сообщила о своем изменении
		emit_datachanged_signal(_index);

		// По модельному индексу выясняется указатель на ветку
		auto _item = item(_index);

		// Перебираются подветки
		QList<QStringList> update_paths = _item->path_children_all<id_key>();

		foreach (QStringList current_path, update_paths) {
			auto current_item = item(current_path);

			QModelIndex _current_index = index(current_item);

			// Для подветки дается команда чтобы модель сообщила о своем изменении
			emit_datachanged_signal(_current_index);
		}
	}
}

// QModelIndex TreeModel::index_recursive(QModelIndex _index,
// boost::intrusive_ptr<TreeItem> item, int mode)
// {
// static QModelIndex find_index;
// static bool is_find = false;

// if(is_find) return find_index;

// if(mode == 0) {
// is_find = false;
// return QModelIndex();
// }

// if(mode == 1) {
//// Если указатель узла совпадает с заданным item
// if(item.get() == static_cast<TreeItem *const>(_index.internalPointer())) {
// is_find = true;
// find_index = _index;
// return find_index;
// } else {
//// Иначе указатель узла не совпадает с заданным
//// и нужно рекурсивно искать далее
// for(int i = 0; i < _index.row(); i++)
// index_recursive(_index.child(i, 0), item, 1);
// }
// }

// return QModelIndex();
// }

// Get a pointer to the Item-element associated with the specified QModelIndex
// // Получение указателя на Item-злемент связанный с заданным QModelIndex
boost::intrusive_ptr<i_t> tm_t::item(const QModelIndex& _index) const // ???
{
	boost::intrusive_ptr<i_t> _result = (nullptr); // _root_item; //
	if (_index.isValid()) {
		boost::intrusive_ptr<i_t> _item = boost::intrusive_ptr<
		    i_t>(static_cast<i_t*>(_index.internalPointer())); // item(static_cast<TreeItem
								       // *>(_index.internalPointer())->path_absolute());
		////boost::const_pointer_cast<TreeItem>(
		// boost::intrusive_ptr<TreeItem>(static_cast<TreeItem
		// *>(_index.internalPointer()))
		////)
		// ;
		if (_item)
			_result = _item; // qDebug() << "Get tree item " <<
					 // item->data("name").toString();

		else {
			assert(!_index.internalPointer());
			qDebug() << "Detect bad castind to TreeItem in item() method ";
			_result = nullptr;
		}
	}
	// qDebug() << "Detect bad QModelIndex in getItem() method ";
	return _result; // _root_item;
}

// Получение указателя на Item-злемент с указанным путем
boost::intrusive_ptr<i_t> tm_t::item(QStringList path) const
{
	boost::intrusive_ptr<i_t> result(nullptr);
	boost::intrusive_ptr<i_t> curritem = _root_item;
	// boost::intrusive_ptr<TreeItem> result(nullptr);
	// int found = 0;
	// Перебор идентификаторов пути
	for (int i = 1; i < path.size(); i++) {
		// int found = 0;
		// Поиск нужного идентификатора в подчиненных узлах текущего узла
		for (int j = 0; j < curritem->count_direct(); j++) {
			if ((curritem->child_direct(j))->id() == path.at(i)) {
				// Узел найден, он становится текущим
				curritem = curritem->child_direct(j);
				// found = 1;
				break;
			}
		}
		//// Если очередной идентификатор пути не был найден
		// if(found == 0)
		// critical_error("Detect bad path in getItem() method " + path.join(","));
	}
	if (curritem != _root_item || (path.size() == 1 && path[0] == global_root_id))
		result = curritem;
	//// Если очередной идентификатор пути не был найден
	// if(found == 0) {
	// critical_error("Detect bad path in getItem() method " + path.join(","));
	// }

	return result; // curritem;
}

boost::intrusive_ptr<i_t> tm_t::item(
    const std::function<bool(boost::intrusive_ptr<const i_t>)>& _equal) const
{
	std::function<boost::intrusive_ptr<i_t>(
	    boost::intrusive_ptr<i_t>,
	    const std::function<bool(boost::intrusive_ptr<const i_t>)>&, int)>
	    item_recursive // boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem>
			 // item, QString name, int mode);
	    = [&](boost::intrusive_ptr<i_t> _it, const std::function<bool(boost::intrusive_ptr<const i_t>)>& _equal, int mode) {
		      static boost::intrusive_ptr<i_t> found_item;
		      if (mode == 0) {
			      found_item = nullptr;
			      // return find_item;	// nullptr;
		      } else if (!found_item) { // return find_item;
			      if (_equal(_it)) {
				      found_item = _it;
				      // return find_item;
			      } else {
				      for (auto il : _it->child_linkers()) {
					      if (!found_item)
						      item_recursive(il->host(), _equal, 1);
					      else
						      break;
				      }
			      }
		      }
		      // assert(found_item != _root_item);

		      return found_item;
	      };

	// Инициализация поиска
	item_recursive(_root_item, _equal, 0); // QUrl()

	// Запуск поиска и возврат результата
	return item_recursive(_root_item, _equal, 1); // _find_url
}

// boost::intrusive_ptr<TreeItem> TreeModel::find_recursive(const QUrl
// &find_url)const
// {
// QModelIndex idx = index(find_url);
// return item(idx);
// }

// boost::intrusive_ptr<TreeItem> TreeModel::find_recursive(const QString
// &id)const
// {
// QModelIndex idx = index(id);
// return item(idx);
// }

// bool TreeModel::is_item_valid(QStringList path) const
// {
// if(path.count() == 1 && path[0] == global_root_id)
// return false;

// boost::intrusive_ptr<TreeItem> curritem = _root_item;

// int found = 0;

//// Перебор идентификаторов пути
// for(int i = 1; i < path.size(); i++) {
////        int found = 0;

//// Поиск нужного идентификатора в подчиненных узлах текущего узла
// for(int j = 0; j < curritem->count_direct(); j++)
// if((curritem->item_direct(j))->id() == path.at(i)) {
//// Узел найден, он становится текущим
// curritem = curritem->item_direct(j);
// found = 1;
// break;
// }

////        // Если очередной идентификатор пути не был найден
////        if(found == 0)
////            return false;
// }

//// Если очередной идентификатор пути не был найден
// if(found == 0)return false;

// return true;
// }

boost::intrusive_ptr<i_t> tm_t::root_item() const
{
	return _root_item;
}

void tm_t::emit_datachanged_signal(const QModelIndex& _index)
{
	emit dataChanged(_index, _index);
}

// Получение заголовка столбца
// Заголовки хранятся в корневом Item элементе просто в виде значений
// для каждого столбца
// section - для какого столбца возвращается заголовок
QVariant tm_t::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);

	// Для всех столбцов возвращается одинаковое значение

	// фактически используется только один столбец
	return "";

	// if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
	// return rootItem->data(section);

	// return QVariant();
}

// Вставка пустых строк с позиции position в количестве rows
bool tm_t::insertRows(int position, int rows, const QModelIndex& _index_parent)
{
	boost::intrusive_ptr<i_t> parent_item = item(_index_parent);
	bool success = false;

	beginInsertRows(_index_parent, position, position + rows - 1);

	// Добавляются строки начиная с указанной позиции, в количестве rows
	// с числом столбцов равным единице
	success = parent_item->children_insert_new(position, rows, 1).size() == rows;

	endInsertRows();

	return success;
}

QModelIndex tm_t::parent(const QModelIndex& _index) const
{
	QModelIndex _result = QModelIndex();
	// if(!_index.isValid())
	// return QModelIndex();
	if (_index.isValid()) {
		boost::intrusive_ptr<i_t> child_item = item(_index);
		boost::intrusive_ptr<i_t> parent_item;
		if (child_item) {
			parent_item = child_item->parent();
			if (parent_item || (parent_item != _root_item)) {
				// return QModelIndex();
				// } else {
				if (parent_item) {
					auto parent_parent = parent_item->parent();
					if (parent_parent) {
						_result = createIndex(parent_parent->sibling_order([&](boost::intrusive_ptr<const Linker> il) {
							return il == parent_item->linker() && il->host() == parent_item &&
							    parent_item->parent() == il->host_parent();
						}),
						    0, static_cast<void*>(parent_item.get()));
					} else
						_result = QModelIndex(); // index(0, 0, index(parent_item));
				} else
					_result = QModelIndex();
			}
		}
	}
	return _result; // createIndex(parent_item->sibling_order(), 0,
			// static_cast<void *>(parent_item.get()));
}

bool tm_t::removeRows(int position, int rows, const QModelIndex& parent)
{
	boost::intrusive_ptr<i_t> parent_item = item(parent);
	bool success = false;

	beginRemoveRows(parent, position, position + rows - 1);
	success = parent_item->delete_permanent_recursive(position, rows).size() > 0 ? true : false;
	endRemoveRows();

	return success;
}

int tm_t::rowCount(const QModelIndex& _index) const
{
	int count = 0;
	if (_index.isValid()) {
		boost::intrusive_ptr<i_t> it = item(_index);
		count = it->count_direct();
	} else
		count = _root_item->count_direct();
	return // item(_index)->count_direct();    //
	    count;
}

void tm_t::session_id(
    boost::intrusive_ptr<TreeIndex> modelindex // const QString &id
    )
{
	// assert(item([&](boost::intrusive_ptr<const TreeItem> it) {return it->id()
	// == id;}));
	_session_id = modelindex->host()->id();
}

// Set the values ​​in the Item element associated with the specified
// QModelIndex   // Установка значений в Item элементе, связанного с указанным
// QModelIndex
bool tm_t::setData(const QModelIndex& _index, const QVariant& value, int role)
{
	// оль UserRole в настоящий момент используется для задания флага, сообщающего
	// что курсор неаходится над элементом при Drag and Drop
	// The Role UserRole currently used to set a flag informing
	// The cursor over an element in the neahoditsya Drag and Drop
	if (role == Qt::UserRole) {
		QModelIndex previousIndex = _cursor_over_index;
		if (value.toBool())
			_cursor_over_index = _index;
		else
			_cursor_over_index = QModelIndex();
		if (previousIndex.isValid())
			emit_datachanged_signal(previousIndex);
		if (_cursor_over_index.isValid())
			emit_datachanged_signal(_cursor_over_index);
		return true;
	}
	if (role == Qt::EditRole) {
		// Вычисляется указатель на Item элемент по QModelIndex
		// в визуальной модели дерева
		boost::intrusive_ptr<i_t> it = item(_index);

		// Устанавливаются данные в Item элемент
		it->field<name_key>(name_value(value.toString())); // QString("name"),

		return true;
	}
	return false;
}

bool tm_t::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	if (role != Qt::EditRole || orientation != Qt::Horizontal)
		return false;
	Q_UNUSED(section);
	_root_item->field<name_key>(
	    name_value(value.toString())); // QString("name"),
	return true;
}

// TreeIndex::ModelIndex(const std::function<KnowModel *()> &_current_model
// , const QModelIndex &_current_index): _current_model(_current_model),
// _current_index(_current_index)
// {
// assert(_current_model()->item(_current_index));
// }

// void TreeModel::session_id(const QString &id)
// {
// assert(item([&](boost::intrusive_ptr<const TreeItem> it) {return it->id() ==
// id;}));
// _session_id = id;
// }
