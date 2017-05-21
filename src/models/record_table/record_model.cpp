

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

//#include "models/tree/tree_item.dec"

#include "items_flat.h"
#include "main.h"
#include "record.h"
#include "record_model.h"

#include "controllers/record_table/record_controller.h"
#include "libraries/fixed_parameters.h"
#include "libraries/global_parameters.h"
#include "models/app_config/app_config.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model_proxy.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "models/tree/tree_model.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

extern FixedParameters fixedparameters;
extern std::shared_ptr<AppConfig> appconfig;
extern std::shared_ptr<gl_para> gl_paras;

// pages_container::pages_container(web::TabWidget *_tab_widget)
// : _tab_widget(_tab_widget)	// new web::TabWidget(_browser,
// _record_controller)
//// , _browser_pages(new ItemsFlat())      //    , _table(new
///RecordTable(_tree_item, QDomElement()))
// {}

// pages_container::~pages_container(){
// _tab_widget = nullptr;	// delete _browser_pages;
// }

// void pages_container::browser_pages(ItemsFlat *_browser_pages)
// {
////    QList<boost::intrusive_ptr<TreeItem>> vw;

////    for(int i = 0; i < _tab_widget->count(); i++) {
////        vw.push_back(_tab_widget->webView(i)->page()->current_item());
////    }

// for(int j = 0; j < _browser_pages->count(); j++) {
// if(-1 ==
// _tab_widget->indexOf(_browser_pages->child(j)->unique_page()->view())) {
// _reocrd_controller->request_item(
// _browser_pages->child(j)
// , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, boost::intrusive_ptr<TreeItem>,
// boost::intrusive_ptr<TreeItem>(TreeItem::*)(web::WebPage *)>>(
// ""
// , &web::TabWidget::ActiveRecordBinder::binder
// , _tab_widget
// )
// , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, boost::intrusive_ptr<TreeItem>>>(
// ""
// , &web::TabWidget::ActiveRecordBinder::activator
// , _tab_widget
// )
// );
// _browser_pages->child(j)->activate();
//// _tab_widget->newTab(_browser_pages->child(j), false);
// }
// }

// }

// ItemsFlat *pages_container::browser_pages() // use as a signal?
// {
// if(_browser_pages->count() != _tab_widget->count()) {
// _browser_pages->clear();

// for(int i = 0; i < _tab_widget->count(); i++) {
// _browser_pages->insert_new_item(i,
// _tab_widget->webView(i)->page()->current_item());
// }
// }

// return _browser_pages;
// }

// ItemsFlat *pages_container::browser_pages()const
// {
// if(_browser_pages->count() != _tab_widget->count()) {
// _browser_pages->clear();

// for(int i = 0; i < _tab_widget->count(); i++) {
// _browser_pages->insert_new_item(i,
// _tab_widget->webView(i)->page()->current_item());
// }
// }

// return _browser_pages;
// }

// void RecordModel::init_source_model(RecordController    *_record_controller
// , RecordScreen      *_record_screen
// , MainWindow        *main_window
// , MetaEditor        *_editor_screen)
// {
// qDebug() << "In RecordController init_source_model() start";
////    shadow_branch(_shadow_branch);

//// Обновление набора данных с последующим выбором первой строки
//// может быть очень длительным, поэтому показывается что процесс выполняется
//// QCursor cursor_wait=QCursor(Qt::BusyCursor);
//// sapp_t::instance()->setOverrideCursor(cursor_wait);
////    find_object<MainWindow>("mainwindow")
// main_window->setCursor(Qt::BusyCursor);

//// Pointer to the data reported to the data source    // Указатель на данные
///сообщается источнику данных
////    _source_model->root(tree_item);    // reset_tabledata(table_data);

// RecordView *_view = nullptr; // _record_controller->view();

// if(!_record_controller->no_view()) {
// _view = _record_controller->view();
//// Надо обязательно сбросить selection model
// _view->selectionModel()->clear();
// }

//// Если список конечных записей не пуст
// bool remove_selection = true;

// if(size() > 0) {
//// Нужно выяснить, на какой записи ранее стояло выделение
// int _work_pos = work_pos();

//// Если номер записи допустимый
// if(!_record_controller->no_view() && _work_pos > 0 && _work_pos < rowCount())
// {
//// Выделение устанавливается на нужную запись
//// selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) ,
///current_tree_selection_mode);
// _view->selectRow(_work_pos);
// _view->scrollTo(_view->currentIndex());   //
// QAbstractItemView::PositionAtCenter

// remove_selection = false;
// }
// }

//// If the selection does not need to install    // Если выделение
///устанавливать ненужно
// if(remove_selection) {
//// Надо очистить поля области редактировния
////        find_object<MetaEditor>(meta_editor_singleton_name)
// _editor_screen->clear_all();

//// При выборе записи обновление инструментов было бы вызвано автоматически
//// в альтернативной ветке (там "эмулируется" клик по записи)
//// А так как записей нет, то обновление инструментов надо
//// вызвать принудительно
// if(_record_screen && _record_screen->inited()) //
// if(qobject_cast<RecordScreen *>(parent())->inited())
// _record_screen->tools_update();
// }

//// sapp_t::instance()->restoreOverrideCursor();
////    find_object<MainWindow>("mainwindow")
// main_window->unsetCursor();

////
///globalparameters.entrance()->activiated_registered().first->tabWidget()->tree_item(tree_item);

// qDebug() << "In RecordTableView set new model stop";

// }

// void RecordModel::init_source_model(ItemsFlat *_browser_pages
// , RecordController *_record_controller
// , RecordScreen *_record_screen
// , MainWindow *main_window
// , MetaEditor *_editor_screen
// )
// {
// qDebug() << "In RecordController init_source_model() start";

//// Обновление набора данных с последующим выбором первой строки
//// может быть очень длительным, поэтому показывается что процесс выполняется
//// QCursor cursor_wait=QCursor(Qt::BusyCursor);
//// sapp_t::instance()->setOverrideCursor(cursor_wait);
////    find_object<MainWindow>("mainwindow")
// main_window->setCursor(Qt::BusyCursor);

//// Pointer to the data reported to the data source    // Указатель на данные
///сообщается источнику данных
// this->browser_pages(_browser_pages);   // ?

// RecordView *_view = nullptr; // _record_controller->view();

// if(!_record_controller->no_view()) {
// _view = _record_controller->view();
//// Надо обязательно сбросить selection model
// _view->selectionModel()->clear();
// }

//// Если список конечных записей не пуст
// bool remove_selection = true;

// if(size() > 0) {
//// Нужно выяснить, на какой записи ранее стояло выделение
// int _work_pos = work_pos();

//// Если номер записи допустимый
// if(!_record_controller->no_view() && _work_pos > 0 && _work_pos < rowCount())
// {
//// Выделение устанавливается на нужную запись
//// selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) ,
///current_tree_selection_mode);
// _view->selectRow(_work_pos);
// _view->scrollTo(_view->currentIndex());   //
// QAbstractItemView::PositionAtCenter

// remove_selection = false;
// }
// }

//// If the selection does not need to install    // Если выделение
///устанавливать ненужно
// if(remove_selection) {
//// Надо очистить поля области редактировния
////        find_object<MetaEditor>(meta_editor_singleton_name)
// _editor_screen->clear_all();

//// При выборе записи обновление инструментов было бы вызвано автоматически
//// в альтернативной ветке (там "эмулируется" клик по записи)
//// А так как записей нет, то обновление инструментов надо
//// вызвать принудительно
// if(_record_screen && _record_screen->inited()) //
// if(qobject_cast<RecordScreen *>(parent())->inited())
// _record_screen->tools_update();
// }

//// sapp_t::instance()->restoreOverrideCursor();
////    find_object<MainWindow>("mainwindow")
// main_window->unsetCursor();

////
///globalparameters.entrance()->activiated_registered().first->tabWidget()->tree_item(tree_item);

// qDebug() << "In RecordTableView set new model stop";
// }

// void RecordModel::init_source_model(boost::intrusive_ptr<TreeItem> tree_item
// , RecordController *_record_controller
// , RecordScreen *_record_screen
// , MainWindow *main_window
// , MetaEditor *_editor_screen
// )
// {
// init_source_model(tree_item.get(), _record_controller, _record_screen,
// main_window, _editor_screen);
// }

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(RecordModel)
#endif

//void RecordModel::on_rctrl_close_requested()
//{
//	_record_controller = nullptr;
//	deleteLater();
//}
// Конструктор модели
// TreeScreen *_tree_screen//, FindScreen *_find_screen//, RecordScreen *_record_screen, web::TabWidget *_tab_widget
RecordModel::RecordModel(rctrl_t* record_ctrl_)
    : QAbstractTableModel(record_ctrl_) // _record_controller, pages_container(_tab_widget)
    , _rctrl(record_ctrl_
	  //          [&] {
	  //                  connect(record_controller, &rctrl_t::close_request, this, &RecordModel::on_rctrl_close_requested);
	  //                  return record_controller;
	  //          }()
	  )
{
	// _browser_pages->init_from_xml(appconfig->datadir() + "/default_page.xml");
	// _browser_pages->root_item()->field("id", "0");  // get_unical_id()
	// _browser_pages->root_item()->field("name", "_shadow_branch");

	// init_source_model(_record_controller, _record_screen, _main_window,
	// _editor_screen);

	setObjectName("recordSourceModel"); // setObjectName(screen_name + "_source_model");

	// При создании модели она должна брать данные как минимум из
	// пустой таблицы данных
	// When you create a model, it has to take the data from at least
	// Empty data table
	// table = new RecordTableData();

	// return;
}

// Деструктор модели
RecordModel::~RecordModel()
{
	// delete _browser_pages;   // current it created from tabmanager
	return;
}

// Предоставление данных табличной модели
// Provide tabular data model
QVariant RecordModel::data(const QModelIndex& index, int role) const
{
	QVariant result;

	//// Если таблица данных не создана
	// if(count() == 0)    // if(!browser_pages())    // if(!_table)
	// return QVariant();
	// Если таблица пустая
	if (0 != count()) { // if(_table->size() == 0)
			    //		return QVariant();
		// Если индекс недопустимый, возвращается пустой объект
		if (index.isValid()) {
			//			return QVariant();
			// qDebug() << "RecordTableModel::data(), row:" << index.row() << " column "
			// << index.column();
			auto pos = pos_source(index.row());
			auto it = item(pos);
			// Если запрашивается текст строки для отрисовки или для редактирования
			if (role == Qt::DisplayRole || role == Qt::EditRole || role == SORT_ROLE || role == Qt::UserRole || role == Qt::CheckStateRole) {
				QStringList show_fields = appconfig->record_table_show_fields();

				// Если длина списка показываемых столбцов меньше или равна номеру
				// запрашиваемого столбца
				if (index.column() < show_fields.size()) {
					QString key_name = show_fields.value(index.column());

					//			auto it = item(pos_source(index.row()));
					QString field = it->field_dynamic(key_name); //item(pos_source(index.row()))->_field_data[key_name];
					// Некоторые данные при отрисовке в таблице преобразуются в "экранные"
					// представления
					// Преобразование возможно только для отображаемой в таблице информации
					// Some data when drawing in the table are converted to "screen" representation
					// Convert only possible to display the information in the table
					if (role == Qt::DisplayRole && key_name == boost::mpl::c_str<ctime_key>::value) {
						// Преобразование временного штампа в дату и время
						auto time = detail::to_qstring(boost::fusion::at_key<ctime_key>(it->_fields_data_map));
						QDateTime fieldDateTime = QDateTime::fromString(time, "yyyyMMddhhmmss");

						if (appconfig->enable_custom_datetime_format() == false)
							result = fieldDateTime.toString(Qt::SystemLocaleDate);
						else
							result = fieldDateTime.toString(appconfig->custom_datetime_format());
					} else if (role == Qt::DisplayRole && key_name == boost::mpl::c_str<name_key>::value) {
						return "<b>" + detail::to_qstring(it->field<name_key>()) + "</b>";
					} else if (role == Qt::DisplayRole && key_name == boost::mpl::c_str<has_attach_key>::value) { // "hasAttach"// Наличие аттачей
						if (boost::fusion::at_key<has_attach_key>(it->_fields_data_map) == has_attach_value(false))
							result = ""; //Attachey If not, an empty string is displayed. This improves readability // Если аттачей нет, выводится пустая строка. Это повышает читабельность
						else
							result = tr("Yes");                                                            // На русский перевести как "Есть"
					} else if (role == Qt::DisplayRole && key_name == boost::mpl::c_str<attach_size_key>::value) { // "attachCount"   // Количество аттачей
						if (boost::fusion::at_key<attach_size_key>(it->_fields_data_map) == attach_size_value(0))
							result = ""; // Если количество аттачей нуливое, выводится пустая
								     // строка. Это повышает читабельность
						else
							result = detail::to_qstring(boost::fusion::at_key<attach_size_key>(it->_fields_data_map));
					} else if (role == Qt::UserRole || role == Qt::EditRole) { // just a test
#ifdef USE_STAR_RATING
						StarRating star_rating = qvariant_cast<StarRating>(index.data());
						// pixmap.load(":/resource/pic/butterfly-right.svg");
						return QVariant::fromValue(star_rating); // pixmap.scaled(16, 16, Qt::KeepAspectRatio, Qt::FastTransformation);
#endif
						result = field;
					} else if (role == SORT_ROLE) {
						if (key_name == boost::mpl::c_str<rating_key>::value)
							return detail::to_qstring(boost::fusion::at_key<rating_key>(it->_fields_data_map)); //field.toULongLong();
						result = field;
					} else
						result = field;
				}
			}
			if (role == RECORD_ID_ROLE)
				result = detail::to_qstring(it->field<id_key>());
			// Если происходит запрос ссылки на таблицу данных
			// if(role == TABLE_DATA_ROLE) {
			// QVariant var;
			// var.setValue<RecordTableDataPointer>(this->getTableData());
			// return var;
			// }
		}
	}
	// Во всех остальных случаях
	return result;
}

// Save the input data at the specified index   // Сохранение вводимых данных по
// указанному индексу
bool RecordModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	bool result = false;
	//// Если таблица данных не создана
	// if(!browser_pages())    // if(!_table)//
	// return false;
	// Если таблица пустая
	if (0 != count()) { // if(_table->size() == 0)
			    //		result= false;
		// Если индекс недопустимый
		if (index.isValid()) {
			//		result= false;
			// Если происходит редактирование
			if (role == Qt::EditRole) {
				// QStringList showFields=fixedParameters.recordFieldAvailableList(); //
				// TODO: Заменить на показываемые поля
				QStringList showFields = appconfig->record_table_show_fields();
				// Если длина списка показываемых столбцов меньше или равна номеру
				// запрашиваемого столбца
				if (index.column() < showFields.size()) {
					QString field_name = showFields.value(index.column());

					// Новое значение ячейки записывается в строковую переменную
					QString cell_value;
					cell_value = value.value<QString>();

					// Изменяется поле в таблице конечных записей
					// _table
					item(pos_source(index.row()))->field_dynamic(field_name, cell_value);
					if (_rctrl) {
						rv_t* view = _rctrl->view();
						if (view) {
							if (view->is_field_type_column(boost::mpl::c_str<rating_key>::value, index.column())) {
								// _record_controller->view()->edit(index);
								// _record_controller->close_context();
							} else
								emit dataChanged(index, index); // Посылается сигнал что данные были изменены
						}
					}
					result = true;
				}
			}
//// Если происходит запись во всю таблицу данных
// if(role == TABLE_DATA_ROLE){
// this->setTableData(qVariantFromValue(value) );
// return true;
// }
//// Если происходит запись одной строки
// if(role == ONE_RECORD_ROLE){
// this->setTableData(qVariantFromValue(value) );
// return true;
// }

#ifdef USE_STAR_RATING
			if (role == Qt::UserRole) { // just a test
				StarRating star_rating = qvariant_cast<StarRating>(index.data());
				// pixmap.load(":/resource/pic/butterfly-right.svg");
				star_rating.star_count(qvariant_cast<int>(value));

				return true; // pixmap.scaled(16, 16, Qt::KeepAspectRatio,
					     // Qt::FastTransformation);
			}
#endif
		}
	}
	// Во всех остальных случаях
	return result;
}

// Получение заголовков столбцов и строк
QVariant RecordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// QStringList showFields=fixedParameters.recordFieldAvailableList(); // TODO:
	// Заменить на показываемые поля
	QStringList showFields = appconfig->record_table_show_fields();

	QMap<QString, QString> descriptionFields = fixedparameters.record_field_description(showFields);
	// Если ни один столбец не показывается (чего, впринципе не может быть)
	if (showFields.size() == 0)
		return QVariant();
	// Если запрашивается заголовок столбца
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		// Если запрашиваемый номер столбца больше количества показываемых
		if (section > showFields.size())
			return QVariant();
		QString fieldName = showFields.value(section);

		return descriptionFields.value(fieldName);
	}
	// Если запрашивается заголовок строки
	if (orientation == Qt::Vertical && role == Qt::DisplayRole)
		return section + 1;
	return QAbstractTableModel::headerData(section, orientation, role);
}

// Сколько записей в таблице
int RecordModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	int r = 0;
	// if(!browser_pages())    // if(!_table)
	// return 0;
	if (_rctrl) {
		auto tab = _rctrl->tab_widget();
		//	_rctrl(tab, &rctrl_t::tabmanager);
		if (tab)
			r = tab->count();
	}
	return r;
}

// Сколько столбцов в таблице
int RecordModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);

	static int previousColumnCount = 0;

	int currentColumnCount = appconfig->record_table_show_fields().size();
	if (currentColumnCount != previousColumnCount) {
		qDebug() << "Column count change. New column count: " << currentColumnCount;
		previousColumnCount = currentColumnCount;
	}
	return currentColumnCount;
}

void RecordModel::remove_child(boost::intrusive_ptr<i_t> it)
{
	for (int i = 0; i < size(); i++)
		if (item(pos_source(i)) == it)
			removeRows(i, 1);
}

// Удаление строк в таблице
// note: Переопределение метода removeRows() влияет и на метод removeRow(),
// так как он просто вызывает removeRows() для удаления одной строки
// Delete rows in the table
// Note: Override removeRows () method affects the removeRow (),
// Because it simply calls removeRows () to remove a single line
bool RecordModel::removeRows(int row, int count, const QModelIndex& parent)
{
	Q_UNUSED(parent);
	if (row < 0 || row >= rowCount() || (row + count - 1) < 0 || (row + count - 1) >= rowCount()) {
		critical_error("Bad arguments in RecordModel::removeRows(). row: " + QString::number(row) + " count: " + QString::number(count));

		return false;
	}
	// QModelIndex index = createIndex(row, 0);

	// auto view = _reocrd_controller->view();

	beginRemoveRows(QModelIndex(), row, row + count - 1); // index   //

	//	// QVector<QString> ids;

	//	//// Удаляются строки непосредственно в таблице
	//	// for(int i = row; i < row + count; ++i) {
	//	// ids.append(child(i)->id());

	//	// }
	//	QAbstractTableModel::removeRow(row, parent);
	//	// _record_controller->remove_children(ids); // recursive called

	//	// view->reset();
	//	// view->setModel(this);   // wrong operation! this is not the proxy model
	endRemoveRows();

	return true;
}

// void RecordModel::tree_item(boost::intrusive_ptr<TreeItem> item)
// {
// beginResetModel();
// _shadow_branch_root = item;
// endResetModel();
// }

// void RecordModel::browser_pages(ItemsFlat *_browser_pages)
// {
// beginResetModel();
////    pages_container::browser_pages(_shadow_branch);   //
///this->_browser_pages = _shadow_branch;

// for(int j = 0; j < _browser_pages->count(); j++) {
// if(-1 ==
// _tab_widget->indexOf(_browser_pages->child(j)->unique_page()->view())) {
// boost::shared_ptr<web::TabWidget> _tab_widget_ptr =
// boost::make_shared<web::TabWidget>(_tab_widget);
// _reocrd_controller->request_item(
// _browser_pages->child(j)
// , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, boost::intrusive_ptr<TreeItem>,
// boost::intrusive_ptr<TreeItem>(TreeItem::*)(web::WebPage *)>>(
// ""
// , &web::TabWidget::ActiveRecordBinder::binder
// , _tab_widget_ptr
// )
// , std::make_shared<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, boost::intrusive_ptr<TreeItem>>>(
// ""
// , &web::TabWidget::ActiveRecordBinder::activator
// , _tab_widget_ptr
// )
// );
// _browser_pages->child(j)->activate();
//// _tab_widget->newTab(_browser_pages->child(j), false);
// }
// }

// endResetModel();
// }

// void RecordModel::root(boost::intrusive_ptr<TreeItem> item)
// {
////void RecordModel::shadow_branch(TreeModelKnow *_shadow_branch)

// beginResetModel();
// _shadow_branch->root_item(item);
// endResetModel();
// }

//// Установка данных в таблицу данных
// void RecordModel::reset_tabledata(std::shared_ptr<RecordTable> record_table)
// {
// beginResetModel();
////    _table
// _tree_item->record_table(record_table);

// endResetModel();
// }

//// Получение ссылки на таблицу данных
// boost::intrusive_ptr<TreeItem> RecordModel::tree_item(void)
// {
//// Возвращается ссылка на данные, с которыми в данный момент работает модель
// return _tree_item;  // ->record_table(); // _table;
// }

// Добавление данных
// Функция возвращает позицию нового добавленного элемента
web::WebView* RecordModel::insert_new_item(boost::intrusive_ptr<i_t> _target_item)
{ // , int mode
	// const index_source source_pos_index,
	pos_source returned_position(-1);
	web::WebView* view = nullptr;
	auto insert_new_tab = [&](
	    pos_source& returned_position,
	    boost::intrusive_ptr<i_t> _item) { // , const pos_source source_insert_pos
		// if(selected_position == -1) {
		boost::intrusive_ptr<RecordIndex> record_index = RecordIndex::instance([&] { return this; }, _item); // , _record_controller->tabmanager()->count() > 0 ?
														     // _record_controller->tabmanager()->webView((int)
														     // source_insert_pos)->page()->binder()->host() : nullptr
		// if(record_index)
		if (_rctrl) {
			auto tab = _rctrl->tab_widget();
			if (tab)
				view = tab->newTab(record_index); // , _item->field("name")
			// else{
			// view = _record_controller->tabmanager()->webView((int)source_insert_pos);
			// view->page()->binder()->host()->activate(std::bind(&web::Entrance::find,
			// globalparameters.entrance(), std::placeholders::_1));
			//// addTab()-> wrong design, demage the function TabWidget::newTab and the
			///function QTabWidget::addTab
			// }
		}
		assert(view);
		returned_position = position(id_value(_item->field<id_key>())); // pos_source(_record_controller->tabmanager()->indexOf(view));
		insertRow(returned_position, QModelIndex());
		return view;
	};
	if (_target_item) {
		// pos_source source_insert_pos =
		// _record_controller->index<pos_source>(source_pos_index);	//
		// Q_UNUSED(pos_index) // to be used
		// Q_UNUSED(mode)	// to be used
		// if(- 1 == (int) source_insert_pos)source_insert_pos = 0;
		auto brother = current_item(); // item(source_insert_pos);
		beginResetModel();             // Подумать, возможно нужно заменить на beginInsertRows
		if (_target_item->binder()) {
			// if(_item->binder()->page()){
			// view = _item->binder()->page()->view();	// activate();
			auto v = gl_paras->main_window()->find([&](boost::intrusive_ptr<const ::Binder> b) {
				return url_equal(detail::to_string(b->host()->field<home_key>()), detail::to_string(_target_item->field<home_key>())) && b->host()->field<id_key>() == _target_item->field<id_key>();
			});
			if (v) {
				if (_rctrl) {
					auto tab = _rctrl->tab_widget();

					if (tab && v->tabmanager() != tab) {
						v->tabmanager()->closeTab(v->tabmanager()->indexOf(v));
						// if(selected_position == - 1)
						// selected_position =
						view = insert_new_tab(returned_position, _target_item); // , source_insert_pos
					} else {
						returned_position = position(id_value(_target_item->field<id_key>())); // pos_source(_record_controller->tabmanager()->indexOf(view));
														       // // _tab_widget->insertTab(pos_index.row(), _item, mode);   // _table
						view = v;
					}
				}

			} else {
				// selected_position =
				view = insert_new_tab(returned_position, _target_item); // , source_insert_pos
			}
			// Вставка новых данных в таблицу конечных записей
			// accomplished by TabWidget::addTab in TabWidget::newTab?

			// }
			assert(returned_position != -1);
			assert(view);
		} else {
			auto _tree_view = gl_paras->tree_screen()->view(); // auto	this_index		=
			// TreeIndex::instance([&] {return
			// _tree_view->source_model();}, brother);
			auto target_url = _target_item->field<url_key>();
			auto _target_item = // this_index->
			    real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(target_url,
				[&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
					return TreeIndex::url_require_item_from_tree(
					    real_target_url_, //std::placeholders::_1,
					    std::bind(&tv_t::move, _tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
					    [&](boost::intrusive_ptr<const i_t> it_) -> bool {
						    return url_equal(detail::to_string(it_->field<home_key>()), detail::to_string(target_url)) || url_equal(detail::to_string(it_->field<url_key>()), detail::to_string(target_url)); // return it_->field<url_type>() == target_url.toString();
					    });
				});

			//			    TreeIndex::url_require_item_from_tree(
			//				real_url_t<url_value>::instance(target_url),
			//				std::bind(&tv_t::move, _tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
			//				[&](boost::intrusive_ptr<const i_t> it_) -> bool {
			//					return url_equal(detail::to_string(it_->field<home_key>()), detail::to_string(target_url)) || url_equal(detail::to_string(it_->field<url_key>()), detail::to_string(target_url)); // return it_->field<url_type>() == target_url.toString();
			//				});

			view = insert_new_tab(returned_position, _target_item); // , source_insert_pos
			assert(returned_position != -1);
			assert(view);
		}
		assert(item(returned_position) == _target_item || _target_item->field<url_key>() == web::Browser::_defaulthome || _target_item->field<url_key>() == url_value(""));
		assert(view);
		endResetModel(); // Подумать, возможно нужно заменить на endInsertRows
	}
	assert(view);

	return view; // selected_position;
}

void RecordModel::on_table_config_changed(void)
{
	beginResetModel();
	endResetModel();
}

void RecordModel::reset_internal_data()
{
	QAbstractTableModel::resetInternalData(); // clear
}

// QString RecordModel::field(int pos, QString name){
// QString result = "";
// if(pos >= 0 && pos < count()){
// result = item(PosSource(pos))->field_dynamic(name);
// }
// return result;
// }

void RecordModel::fields(int pos, QMap<QString, QString> data)
{
	if (pos >= 0 && pos < count()) {
		// QMap<QString, QString>::iterator
		for (auto i = data.begin(); i != data.end(); i++)
			item(pos_source(pos))->field_dynamic(i.key(), i.value());
	}
}

QModelIndex RecordModel::index(int row, int column, const QModelIndex& parent) const
{
	// (void) parent;
	QModelIndex result;
	// auto	tab =
	// _record_controller->record_screen()->browser()->tabmanager();
	// assert(tab);// this will encounter failures under debug buid when copy
	// paste codes
	// assert(_record_controller->tabmanager());
	if (_rctrl) {
		auto tab = _rctrl->tab_widget();
		if (tab) {
			if (column >= 0 && column < tab->count()) {
				auto it = item(pos_source(column));
				result = createIndex(row, column, static_cast<void*>(it.get()));
			}
		} else
			result = QAbstractTableModel::index(row, column, parent);
	}
	return result;
	// return QAbstractTableModel::index(row, column, parent);
}

index_source RecordModel::index(boost::intrusive_ptr<i_t> it) const
{
	index_source result;
	if (it) {
		for (int i = 0; i < count(); i++) {
			if (item(pos_source(i)) == it) {
				result = createIndex(i, 0, static_cast<void*>(it.get()));
				break;
			}
		}
	}
	// else{
	// result = QAbstractTableModel::index(0, 0, QModelIndex());

	// }
	return result;
}

index_source RecordModel::fake_index(boost::intrusive_ptr<i_t> it) const
{
	index_source result;
	if (it)
		result = createIndex(0, 0, static_cast<void*>(it.get()));
	return result;
}

//// for multi items link with unique page
// boost::intrusive_ptr<TreeItem>
// RecordModel::item_bounded(boost::intrusive_ptr<TreeItem> it) const {
// boost::intrusive_ptr<TreeItem> result = nullptr;
// for(int i = 0; i < count(); i ++){
// if(it->page_valid() && item(pos_source(i))->page() == it->page()){
// result = it;break;
// }
// }
// return result;
// }

boost::intrusive_ptr<i_t>
RecordModel::item(const std::function<bool(const id_value)> _equal) const
{
	boost::intrusive_ptr<i_t> result = nullptr;
	for (int _index = 0; _index < count(); _index++) {
		if (_rctrl) {
			auto tab = _rctrl->tab_widget();
			if (tab) {
				auto it = tab->webView(_index)->page()->binder()->host(); // item(pos_source(i));
				if (_equal(it->id())) {
					result = it;
					break;
				}
			}
		}
	}
	return result;
}

// boost::intrusive_ptr<TreeItem>
// RecordModel::item(boost::intrusive_ptr<TreeItem> it) const {
// boost::intrusive_ptr<TreeItem> result = nullptr;
// for(int i = 0; i < count(); i ++){
// if(item(pos_source(i)) == it){
// result = it;break;
// }
// }
// return result;
// }

// boost::intrusive_ptr<TreeItem> RecordModel::item(const QUrl &_url) const {
// // lead implicit construct
// boost::intrusive_ptr<TreeItem> result(nullptr);
// for(int i = 0; i < count(); i ++){
// auto it	= item(pos_source(i));
////	std::string	difference	=
///url_difference((it->field<url_type>()).toStdString(),
///_url.toString().toStdString());
// if(url_equal((it->field<url_type>()).toStdString(),
// _url.toString().toStdString())){	// if(difference.size() == 0 ||
// difference == "/"){
// result = it;
// break;
// }
// }
// return result;
// }

// int RecordModel::item_current(QString find_id)
// {
// int pos = -1;

// for(int i = 0; i < count(); i++) {
// if(item(i)->id() == find_id) {
// pos = i;
// break;
// }
// }

// return pos;
// }

// boost::intrusive_ptr<TreeItem> RecordModel::item(const id_value &id){
// boost::intrusive_ptr<TreeItem> r(nullptr);
// for(int pos = 0; pos < _record_controller->tabmanager()->count(); pos ++){
// auto it = _record_controller->tabmanager()->webView(pos)->page()->host();
// if(it->id() == id){
// r = it;
// break;
// }
// }
// return r;
// }

// boost::intrusive_ptr<TreeItem> RecordModel::item(const id_value &id) const {
// boost::intrusive_ptr<TreeItem> r(nullptr);
// for(int pos = 0; pos < _record_controller->tabmanager()->count(); pos ++){
// auto it = _record_controller->tabmanager()->webView(pos)->page()->host();
// if(it->id() == id){
// r = it;
// break;
// }
// }
// return r;
// }

// boost::intrusive_ptr<TreeItem> RecordModel::item(const pos_source _index){
// boost::intrusive_ptr<TreeItem> r(nullptr);
// if(_index >= 0 && _index < size()) r =
// _record_controller->tabmanager()->webView((int) _index)->page()->host();
// return r;
// }

boost::intrusive_ptr<i_t> RecordModel::item(const pos_source _index) const
{
	boost::intrusive_ptr<i_t> r(nullptr);
	if (_index >= 0 && _index < size()) {
		if (_rctrl) {
			auto tab = _rctrl->tab_widget();
			if (tab) {
				assert(tab->webView(static_cast<int>(_index))->page()->binder());
				r = tab->webView(static_cast<int>(_index))->page()->binder()->host();

				assert(r); // if find_recursive get new item from tree, there will be no
					   // item_link? because I move it?
			}
		}
	}
	return r;
}

// boost::intrusive_ptr<TreeItem> RecordModel::item_fat(const pos_source index){
// boost::intrusive_ptr<TreeItem> item =
// _record_controller->tabmanager()->webView((int) index)->page()->host();
// if(item->is_lite()) item->to_fat();
// return item;
// }

rctrl_t* RecordModel::record_ctrl() const
{
	return _rctrl;
}

// bool RecordModel::remove_child(int index)
// {
// bool r = false;

// if(index > 0 && index < count()) {
// beginRemoveRows(QModelIndex(), index, index);

////    if(index > 0 && index < count()) {
////        _tab_widget->closeTab(index) ;
////        //        _reocrd_controller->view()->reset();
////        //
///_reocrd_controller->view()->setModel(_reocrd_controller->proxy_model());
////        r = true;

////    }
// r = true;
// endRemoveRows();
// }

// return r;
// }

// bool RecordModel::remove_child(QString find_id)
// {
// bool r = false;
// int index = is_item_exists(find_id);

// if(index != -1) {
// beginRemoveRows(QModelIndex(), index, index);

////    if(index != -1) {
////        _tab_widget->closeTab(position(find_id)) ;
////        //        _reocrd_controller->view()->reset();
////        //
///_reocrd_controller->view()->setModel(_reocrd_controller->proxy_model());
////        r = true;

////    }

// endRemoveRows();
// }

// return r;
// }

// boost::intrusive_ptr<TreeItem>
// RecordModel::sibling_s(boost::intrusive_ptr<TreeItem> it) const {
//// #ifdef USE_LOAD_ON_FOUND
// return _record_controller->tabmanager()->sibling(it);
//// #else
////    index_source	cur	= index(it);
////    QModelIndex		cur_	= static_cast<QModelIndex>(cur);
////    cur_ = createIndex(cur_.row() > 0 ? cur_.row() - 1 : 0, cur_.column(),
///cur_.internalPointer());
////    index_source tar(cur_);
////    return item(_record_controller->index<pos_source>(tar));
//// #endif
// }

boost::intrusive_ptr<i_t> RecordModel::current_item() const
{
	boost::intrusive_ptr<i_t> result(nullptr);
	// #ifdef USE_LOAD_ON_FOUND


	if (_rctrl) {
		auto tab_widget = _rctrl->tab_widget();
		if (tab_widget) {
			auto v = tab_widget->currentWebView();
			auto page = v ? v->page() : nullptr;
			if (page) {
				auto binder = page->binder();
				if (binder) {
					result = binder->host();
					//			assert(result);
				}
			}
			// #else
			// result = item(_record_controller->index<pos_source>(current_index()));

			// #endif
			// assert(result);
		}
	}
	return result; // _tab_widget->currentWebView()->page()->binder()->host();
}

index_source RecordModel::current_index() const
{
	index_source result;
	rv_t* view = nullptr;
	if (_rctrl) {
		view = _rctrl->view();
		if (view) {
			auto current = current_item();
#ifdef USE_HAS_SELECTION
			const index_proxy index_proxy_(view->selectionModel()->currentIndex());

			if (static_cast<QModelIndex>(index_proxy_).isValid()) {
				index_source is = _rctrl->index<index_source>(index_proxy_);
				//			index_source (rctrl_t::*index_function)(const index_proxy&) const = &rctrl_t::index<index_source>;
				//				_rctrl(is, &rctrl_t::index<index_source>, index_proxy_);
				if (static_cast<QModelIndex>(is).isValid()) {
					result = is;
				}
			} else
#endif // USE_HAS_SELECTION
			    if (current)
				result = index(current);
		}
	}
	return result;
	//// #ifdef USE_LOAD_ON_FOUND
	// index(current_item());
	//// #else
	////	index_source(_record_controller->view()->selectionModel()->currentIndex());	//
	//// #endif
}

void RecordModel::position(pos_source _index)
{
	if (_rctrl) {
		auto tab = _rctrl->tab_widget();
		if (tab)
			tab->setCurrentIndex(static_cast<int>(_index));
	}
}

// PosSource RecordModel::position()const
// {
// return PosSource(_tab_widget->currentIndex());
// }

pos_source RecordModel::position(id_value id) const
{
	pos_source result(-1);
	if (_rctrl) {
		auto tab = _rctrl->tab_widget();
		if (tab) {
			for (int i = 0; i < tab->count(); i++) {
				if (tab->webView(i)->page()->host()->id() == id) {
					result = pos_source(i);
					break;
				}
			}
		}
	}
	return result;
}

Qt::ItemFlags RecordModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

// PosSource RecordModel::position(boost::intrusive_ptr<TreeItem> item) const
// {
// PosSource result(-1);

// for(int i = 0; i < _tab_widget->count(); i++) {
// if(_tab_widget->webView(i)->page()->host() == item) {
// result = PosSource(i);
// break;
// }
// }

// return result;
// }

int RecordModel::count() const
{
	int r = -1;
	if (_rctrl) {
		auto tab = _rctrl->tab_widget();
		if (tab)
			r = tab->count();
	}
	return r;
}

int RecordModel::size() const
{
	return count();
}

int RecordModel::move(const pos_source pos, const pos_source target)
{
	pos_source new_pos = pos;
	beginResetModel();
	if (_rctrl) {
		auto tab = _rctrl->tab_widget();


		if (target < pos) { // move up
			if (pos > 0) {
				new_pos = (-1 == target) ? pos_source(static_cast<int>(pos) - 1) : target;
				if (tab)
					tab->tabbar()->moveTab(static_cast<int>(pos), static_cast<int>(new_pos)); // moveTab(pos, new_pos);
														  // auto source_index =
														  // static_cast<QModelIndex>(_record_controller->index<index_source>(pos));
														  // auto target_index =
														  // static_cast<QModelIndex>(_record_controller->index<index_source>(new_pos));
														  // this->moveRow(source_index.parent(), source_index.row(),
														  // target_index.parent(), target_index.row());
														  ////	    emit
														  ///_record_controller->view()->tabMoved(static_cast<int>(pos),
														  ///static_cast<int>(new_pos));
			}
		} else if (target > pos) { // move down
			if (pos < count() - 1) {
				new_pos = (target == 1 && pos != 0) ? pos_source(static_cast<int>(pos) + 1) : target;
				if (tab)
					tab->tabbar()->moveTab(static_cast<int>(pos), static_cast<int>(new_pos)); // moveTab(pos, new_pos);
														  // emit _record_controller->view()->tabMoved(static_cast<int>(pos),
														  // static_cast<int>(new_pos));
			}
		}
		endResetModel();
	}
	return new_pos;
}

// int RecordModel::move_dn(const pos_source pos){
// beginResetModel();

// pos_source new_pos = pos;
// if(pos < count() - 1){
// new_pos = pos_source((int) pos + 1);
// _record_controller->tabmanager()->tabbar()->moveTab((int) pos, (int)
// new_pos);		// moveTab(pos, new_pos);
// emit _record_controller->view()->tabMoved(static_cast<int>(pos),
// static_cast<int>(new_pos));
// }
// endResetModel();

// return new_pos;
// }
