

#include <cassert>
// #include <iostream>
#include <algorithm>
#include <vector>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QHeaderView>
#include <QObject>

//#include "models/tree/tree_item.dec"

#include "controllers/attach_table/attach_table_controller.h"
#include "libraries/clipboard_records.h"
#include "libraries/disk_helper.h"
#include "libraries/fixed_parameters.h"
#include "libraries/flat_control.h"
#include "libraries/global_parameters.h"
#include "libraries/qtm/blogger.h"
#include "libraries/qtm/side_tabwidget.h"
#include "libraries/walk_history.h"
#include "libraries/window_switcher.h"
#include "main.h"
#include "models/app_config/app_config.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/record_table/record_model_proxy.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "models/tree/tree_model.h"
#include "record_controller.h"
#include "views/app_config//app_config_dialog.h"
#include "views/attach_table/attach_table_screen.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/browser/tabwidget.h"
#include "views/browser/toolbar_search.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/main_window.h"
#include "views/record/add_new_record.h"
#include "views/record/editor_wrap.h"
#include "views/record/record_info_fields_editor.h"
#include "views/record_table/record_print.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
#include "views/record_table/record_view.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

extern std::shared_ptr<gl_para> gl_paras;
extern FixedParameters fixedparameters;
extern std::shared_ptr<AppConfig> appconfig;
extern WalkHistory walkhistory;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(rctl_t)
#endif

rctrl_t::rctrl_t(
    Blogger* blogger_, // TreeScreen *_tree_screen, FindScreen *_find_screen // ,
    web::TabWidget* tabmanager_,
    rs_t* record_screen_)
#ifdef USE_SIGNAL_CLOSE
    : super()
    , sd::renter() //sd::renter<rctrl_t>()
#else
    : super(record_screen_)
#endif                                     //USE_SIGNAL_CLOSE
    , _tab_widget(tabmanager_)             // nullptr
    , _source_model(new RecordModel(this)) // so, you need boost::intursive_ptr
    , _proxy_model(new RecordProxyModel(this))
    , _record_screen(record_screen_)
    , _blogger(blogger_)
    , _view(new rv_t(record_screen_, this), true //sd::make_intrusive<rv_t>(_view, record_screen_, this)
	  //	  [&] {
	  //	    _view.detach(); // = nullptr; // = sd::intrusive_ptr<rv_t>(nullptr);
	  //	    assert(record_screen_);
	  //	    return std::forward<sd::intrusive_ptr<rv_t>>(sd::make_intrusive<rv_t>(record_screen_, this));
	  ////	    return std::forward<sd::intrusive_ptr<rv_t>>(r);
	  //    }()
	  )
{
	//	_view =  std::forward<sd::intrusive_ptr<rv_t>>(sd::make_intrusive<rv_t>(record_screen_, this));

	//	  , _main_window(main_window_)
	// setObjectName(screen_name + "_controller");

	// _tree_screen->reocrd_controller =
	// std::make_shared<sd::_interface<sd::meta_info<void >, RecordController
	// *>>("", &RecordController::reocrd_controller, this);
	// _find_screen->reocrd_controller =
	// std::make_shared<sd::_interface<sd::meta_info<void >, RecordController
	// *>>("", &RecordController::reocrd_controller, this);

	// _find_screen->toolbarsearch()->lineedits(_tab_widget->lineEditStack());

	// Инициализируется область со списком записей
	// view = new RecordTableView(qobject_cast<QWidget *>(parent));   // Вид
	// размещается внутри виджета Screen
	// _view->setObjectName("recordTableView");
	// view->setController(this);

	// Создание модели данных
	// recordSourceModel = new RecordTableModel(this);
	// _source_model->setObjectName("recordSourceModel");

	// recordProxyModel = new RecordTableProxyModel(this);
	_proxy_model->setSourceModel(_source_model);
	// _proxy_model->setObjectName("recordProxyModel");

	// Модель данных задается для вида
	_view->setModel(_proxy_model);
	// init();
	// _no_view = false;
	connect(_view, &rv_t::tabMoved, _tab_widget, &web::TabWidget::moveTab);
#ifdef USE_SIGNAL_CLOSE

	//	close_connect(std::bind(&web::TabWidget::close_requested_from_others, _tab_widget, static_cast<sd::renter* const>(this)));


	destroy_transfer([&](sd::renter* const r) {
		(void)r;
		if (r != this) { //&& !this->_destroyed_request_sent
			this->_destroy_request_sent = true;
			//			delete this; //this->deleteLater();
		}
	});


#endif //USE_SIGNAL_CLOSE
}

rctrl_t::~rctrl_t()
{
	//	this->_destroyed_request_triggered = true;
	//	_renter.trigger();
	// delete
	// _view->deleteLater();
	// delete
	_proxy_model->deleteLater();
	// delete
	_source_model->deleteLater();
#ifdef USE_SIGNAL_CLOSE
	if (_record_screen) {
		if (!_record_screen->close_request_sent() && !_record_screen->destroy_request_sent())
			//                        destroy_transfer(
			_record_screen->destroy_trigger_from_others()(this);
		//				    );
	}

	if (_view) {
		if (!_view->close_request_sent() && !_view->destroy_request_sent())
			//                        destroy_transfer(
			_view->destroy_trigger_from_others()(this);
		//					    );
	}
#endif //USE_SIGNAL_CLOSE
}

// void RecordController::init(void)
// {
// _view->init();
// }

rv_t* rctrl_t::view(void) const
{
	return _view;
}

// Установка засветки в нужную строку на экране
void rctrl_t::select_as_current(pos_proxy pos_proxy_)
{ // , const int mode
	////    IdType id;
	////    PosSource pos_source_ = _source_model->position(id);
	////    PosProxy pos_proxy_ = index<PosProxy>(pos_source_);

	//// В QTableView некорректно работает установка на только что созданную
	///строку
	//// Это как-то связано с отрисовкой виджета QTableView
	//// Прокрутка к только что созданной строке через selectRow() показывает
	///только
	//// верхнюю часть новой строки. Чтобы этого избежать, при добавлении в конец
	//// таблицы конечных записей, установка прокрутки делается через
	///scrollToBottom()
	// if(mode == add_new_record_to_end
	// || (mode == add_new_record_after && pos_proxy_ >=
	// (_view->model()->rowCount() - 1))
	// ) {
	// _view->scrollToBottom();
	// }

	// PosProxy pos_proxy_ = _record_controller->pos_proxy(pos_proxy_);
	index_proxy index_proxy_ =
	    index<index_proxy>(pos_proxy_); // Модельный индекс в Proxy модели
	index_source index_source_ = index<index_source>(pos_proxy_);
	auto qindex_proxy = static_cast<QModelIndex>(index_proxy_);
	pos_proxy pos_proxy_real(qindex_proxy.row());
	// todo: Если это условие ни разу не сработает, значит преобразование ipos -
	// pos надо просто убрать
	// Todo: If this condition is never going to work, then ipos transformation -
	// pos should simply remove
	if (static_cast<int>(pos_proxy_real) != static_cast<int>(pos_proxy_)) {
		QMessageBox msg_box;
		msg_box.setText(
		    "In RecordView::cursor_to_index() input pos not equal model pos");
		msg_box.exec();
	}
	int tab_count = row_count();
	if ((int)pos_proxy_real >= 0 && (int)pos_proxy_real < tab_count) {
		if (index_source_ !=
		    _source_model->current_index()) { // if(pos_real > (rowCount - 1))return;
			// Простой механизм выбора строки. Похоже, что его использовать не
			// получится
			_view->selectRow((int)pos_proxy_real);

			// auto recordSourceModel = controller->getRecordTableModel();
			// QModelIndex selIdx = recordSourceModel->index(pos, 0);

			_view->selectionModel()->select(qindex_proxy, current_tree_selection_mode);
			// Установка засветки на нужный индекс
			// Set the backlight to the desired index
			_view->selectionModel()->setCurrentIndex(qindex_proxy, current_tree_current_index_mode); // selIdx   //
			// QItemSelectionModel::Select    //
			// ClearAndSelect
			_view->setCurrentIndex(qindex_proxy);
			_view->edit(qindex_proxy);

			_view->scrollTo(qindex_proxy); // QAbstractItemView::PositionAtCenter
			_view->update(qindex_proxy);
			//
			auto real_index_source_ = _source_model->current_index();
			auto real_index_proxy_ = index<index_proxy>(real_index_source_);
			assert(real_index_proxy_ == index_proxy_);
			// В мобильной версии реакции на выбор записи нет (не обрабатывается
			// сигнал смены строки в модели выбора)
			// Поэтому по записи должен быть сделан виртуальный клик, чтобы
			// заполнилась таблица конечных записей
			// In response to the mobile version of the record is no choice (not
			// processed signal line change to the selection model)
			// Therefore, the recording must be made a virtual click to fill the final
			// table of records
			if (appconfig->interface_mode() == "mobile") emit _view->clicked(qindex_proxy);
			// QModelIndex selIdx=recordSourceModel->index(pos, 0);

			// emit _view->clicked(qindex_proxy);	//
			// segment error?

			// this->setFocus();   // ?
			pos_source pos_source_ = index<pos_source>(pos_proxy_);
			auto it = index<boost::intrusive_ptr<i_t>>(pos_source_);
			if (this->_view->hasFocus()) {                                                                                                // view is the curretn controller
				if (_tab_widget->currentIndex() != static_cast<int>(pos_source_)) _tab_widget->select_as_current(it->page()->view()); // setCurrentIndex(static_cast<int>(pos_source_));
				auto tree_screen = gl_paras->main_window()->tree_screen();
				if (tree_screen->view()->current_item() != it) tree_screen->view()->select_as_current(TreeIndex::item_require_treeindex([&] { return tree_screen->view()->source_model(); }, it));
			}
			if (it)
				//				if (it->page()) it->page()
				_blogger->metaeditor_sychronize();
		}
		//		_view->setFocus();
	}
	//	_record_screen->tools_update();
}

// Принимает индекс Proxy модели
// Accepts index Proxy models
boost::intrusive_ptr<i_t> rctrl_t::index_invoke(const index_proxy& index_proxy_, bool force_update)
{
	boost::intrusive_ptr<i_t> result;
	// Так как, возможно, включена сортировка, индекс на экране преобразуется в
	// обычный индекс
	index_source source_index = index<index_source>(index_proxy_);

	// Позиция записи в списке
	pos_source pos_source_ = index<pos_source>(index_proxy_); // (((QModelIndex)source_index).row());
	qDebug() << "rctrl_t::index_invoke() : current item num " << pos_source_;

	_view->setFocus(); //    select_as_current(index<pos_proxy>(index_proxy_));

	result = source_model()->item(pos_source_);
	// auto	ov	= result->page()->view();
	// auto v =
	//	force_update ? result->binder()->activate() :
	result->activate(std::bind(&web::TabWidget::find, &*_tab_widget, std::placeholders::_1), force_update);
	//        QItemSelectionModel* item_selection_model = _view->selectionModel();
	//	bool has_selection = item_selection_model->hasSelection();
	//	if (!has_selection) {
	select_as_current(index<pos_proxy>(index_proxy_));
	//	}
	// assert(v == ov);
	// assert(v->page()->host() == result);
	// v->recovery_global_consistency();
	_record_screen->tools_update();
	// sychronize_metaeditor_to_record(source_pos);  // means update
	// editor(source_pos);
	if (static_cast<QModelIndex>(source_index).isValid()) {
		sychronize_attachtable_to_item(pos_source_);
		// browser_update(pos_source_); // if new one, create it? no, you can't
		// click a record which does not exist.
	}
	gl_paras->window_switcher()->record_screen_to_editor_docker();

	return result;
}

// void RecordController::url_load(IndexProxy proxyIndex)
// {
// qDebug() << "RecordTableController::editFieldContext()";

// IndexSource source_index = index<IndexSource>(proxyIndex);
// auto _index = PosSource(((QModelIndex)source_index).row()); // Номер строки в
// базе

// browser_update(_index);

////    // Create input window after exiting the function window should retire
///// Создается окно ввода данных, после выхода из этой функции окно должно
///удалиться
////    //RecordInfoFieldsEditor editRecordWin;
////    auto entrance = globalparameters.entrance();
////    // Выясняется ссылка на таблицу конечных данных
////    //    RecordTableData *table = recordSourceModel->getTableData();

////    // Fields box filled with initial values    // Поля окна заполняются
///начальными значениями
////    //editRecordWin.setField("name",  table->getField("name",   pos) );
////    //editRecordWin.setField("author",table->getField("author", pos) );
////    //editRecordWin.setField("url",   table->getField("url",    pos) );
////    //editRecordWin.setField("tags",  table->getField("tags",   pos) );

////    //    browser_view->loadUrl(pos);   //table->getField("url", pos)

////    boost::intrusive_ptr<Record> record =
///this->table_model()->table_data()->record(pos);
////    assert(record->is_registered());

////    //    if(record->getNaturalFieldSource("url") !=
///web::DockedWindow::_defaulthome)
////    if(entrance && (!record->binder() || record->activator()))
///entrance->equip_registered(record);

////    if(record->binder() && record->activator())record->activate();

////    //    int i = editRecordWin.exec();
////    //    if(i == QDialog::Rejected)
////    //        return; // Была нажата отмена, ничего ненужно делать

////    //// Измененные данные записываются
////    //    editField(pos,
////    //              editRecordWin.getField("name"),
////    //              editRecordWin.getField("author"),
////    //              editRecordWin.getField("url"),
////    //              editRecordWin.getField("tags"));

// }

//// you can't click a record which does not exist.
//// you can switch between two already existing record from this
// void RecordController::browser_update(const PosSource pos_source_)
// {
////    //RecordTableData *table = recordSourceModel->getTableData();

////    web::Entrance *entrance = globalparameters.entrance();

////    //Record *current_record = browser_view->getCurrentRecord();

////    //if(current_record) {

////    //if(current_record->isLite())current_record->switchToFat();

////    //bool existing =
///table->isRecordExists(current_record->getNaturalFieldSource("id"));

////    //    QUrl _url = table->getField("url", pos); //reply->url();
////    //    QString url = current_record->getNaturalFieldSource("url");
////    //    std::string url_compare_stored = url.toStdString();
////    //    std::string url_compare_get = _url.toString().toStdString();
////    //    std::string compare = getDifference(url_compare_stored,
///url_compare_get);

////    //    if(//!existing &&
////    //        compare.size() != 0 && compare != "/") {
////    //        //if(this->current_record->getNaturalFieldSource("url") !=
///url.toString()) {
////    //        Record record;

////    //        //if(record.isLite())record.switchToFat();

////    //        record.setNaturalFieldSource("id",
///current_record->getNaturalFieldSource("id"));
////    //        record.setNaturalFieldSource("name",
///current_record->getNaturalFieldSource("name"));
////    //        record.setNaturalFieldSource("author",
///current_record->getNaturalFieldSource("author"));
////    //        record.setNaturalFieldSource("url",    _url.toString());    //
///only changed
////    //        record.setNaturalFieldSource("tags",
///current_record->getNaturalFieldSource("tags"));
////    //        //record.setText(browser_view->getText());

////    //        //this->current_record->setNaturalFieldSource("url",
///_url.toString());
////    //        addNew(ADD_NEW_RECORD_AFTER, record);
/////recordTableController->autoAddNewAfterContext();

////    //        browser_view->loadUrl(_url);

////    //        //current_record = table->getRecordFat(pos); // wrong pos !
///new position created //findRecord();
////    //        assert(current_record->getNaturalFieldSource("url") ==
///_url.toString());

////    //        //browser_view->loadUrl(pos);

////    //        //this->current_record->setNaturalFieldSource("url",
///url.toString());
////    //        //this->loadChanged();
////    //    } else {

// boost::intrusive_ptr<TreeItem> item =
// this->source_model()->item(pos_source_);

// if(item->is_lite())item->to_fat();

////    //    assert(record->is_registered());
////    item->active_request(source_pos, 0);

////    //    if(record->generator())record->generate();
////    //    else if(entrance) entrance->active_record(record);

////    //    entrance->activebrowser()->tabWidget()->view_index();
////    //    if(record->page() == nullptr || record->page()->view() == nullptr)

// QString url = item->field("url");

////    web::WebPage *page = record->binded_only_page();
////    Record *old_record = nullptr;
////    if(page)old_record = page->current_record();

//// assert(url != "");

////    if(record != old_record) {

// if(url != "" && url != web::Browser::_defaulthome) {   //       &&
// record->active_immediately()

////        //        // back compatible
////        //        //record->setNaturalFieldSource("url",
///web::DockedWindow::_defaulthome);
////        //        if(// (!page && url != "")
////        //            // || (page && url != "" && url !=
///page->url().toString())  // page redirect to new record already!
////        //            // || (page && !page->view()->hasFocus())
////        //            (old_record &&
///_state_check[old_record->getNaturalFieldSource("pin")] == Qt::Unchecked)
////        //            || (page && !old_record)
////        //        ) {
////        //            page->load(record);
////        //        } else

////        if(entrance && !item->page_valid()) {    // unique_page()
////            // !record->binder() || !record->activator())) {
////            entrance->item_bind(item);

////            //        assert(record->unique_page());    // not sure
////            assert(item->binder());
////            //            assert(item->activator());

////            //        if(record->binder() &&
///!record->unique_page())record->bind();
////            //        else if(record->activator() &&
///record->unique_page())record->activate();  // if(entrance)
///entrance->active_record(record);
////            if(item->binder())item->activate();

////            //        else if(entrance)
////        } else {
////            if(!item->is_registered_to_browser())
// item->activate();
////        }
// }

////    } else {
////        web::TabWidget *tabwidget = entrance->activebrowser()->tabWidget();

////        if(tabwidget->currentWebView() != page->view()) {
//// tabwidget->setCurrentIndex(tabwidget->webViewIndex(page->view()));
////            // page->load(record);
////            page->view()->show();
////        }

////    }

////    loadUrl(pos);

////    }

////    if(browser_view->getCurrentRecord()->getNaturalFieldSource("url") !=
///table->getField("url", pos))
////        browser_view->loadUrl(QUrl(table->getField("url", pos)));
////} else {
////    browser_view->loadUrl(pos);
////}

////    Record *_record =
///getRecordTableModel()->getRecordTableData()->getRecord(pos);
////    WebView *view =
///globalParameters.getBrowserView()->getBrowserWindow()->currentTab();
////    view->bindRecord(_record);

// }

//// from this function,we would have found that without a full-stack
///controller, we can't synchronize editor content.
//// so if our records are come from different tree path, we must switch the
///parent node, our give them a sharing parent.
//// that's why we need a page_controller,  or we should implement a multi table
///screen architecture -- but with this design,
//// we can not settle the medium results easily.
// void RecordController::sychronize_metaeditor_to_item(const PosSource
// pos_source_)
// {

// boost::intrusive_ptr<TreeItem> item =
// this->source_model()->item(PosSource(pos_source_));
// assert(item);
//// Внимание! Наверно, всю эту логику следует перенести в MetaEditor. А здесь
///только получить данные из таблицы

//// Выясняется указатель на объект редактирования текста записи
// MetaEditor *meta_editor = globalparameters.meta_editor();   //
// find_object<MetaEditor>(meta_editor_singleton_name);
// assert(meta_editor);
// meta_editor->bind(item);

////    // Выясняется ссылка на таблицу конечных данных
////    auto item = _source_model->tree_item();

//// В таблице конечных данных запоминается какая запись была выбрана
//// чтобы затем при выборе этой же подветки засветка автоматически
//// установилась на последнюю рабочую запись
// this->source_model()->position(pos_source_);   // item->work_pos(pos);

//// Устанавливается функция обратного вызова для записи данных
// meta_editor->save_callback(item->editor_save_callback);

//// Сохраняется текст и картинки в окне редактирования
////    find_object<MainWindow>("mainwindow")
// globalparameters.mainwindow()->save_text_area();

// auto it = item->item_direct(pos_source_);
//// Для новой выбраной записи выясняется директория и основной файл
// QString currentDir      = it->field("dir");
// QString currentFile     = it->field("file");
// QString fullDir         = appconfig->tetra_dir() + "/base/" + currentDir;
// QString fullFileName    = fullDir + "/" + currentFile;
// qDebug() << " File " << fullFileName << "\n";

//// If the window contents of the record is already selected record  // Если в
///окне содержимого записи уже находится выбираемая запись
// if(meta_editor->work_directory() == fullDir
// && meta_editor->file_name() == currentFile
// ) {
// globalparameters.window_switcher()->switchFromRecordtableToRecord();
// return;
// }

//// Перед открытием редактора происходит попытка получения текста записи
//// Этот вызов создаст файл с текстом записи, если он еще не создан (подумать,
///переделать)
//// Before the opening of the editor it attempts to get the text records
//// This call will create a text file with the record if it is not already
///created (think remake)
// item->text(pos_source_);

//// Редактору задаются имя файла и директории
//// И дается команда загрузки файла
// meta_editor->work_directory(fullDir);
// meta_editor->file_name(currentFile);

//// Если идет работа с зашифрованной записью
//// И если имя директории или имя файла пусты, то это означает что
//// запись не была расшифрована, и редактор должен просто показывать пустой
///текст
//// ничего не сохранять и не считывать
// qDebug() << "RecordTableView::onClickToRecord() : id " << it->field("id");
// qDebug() << "RecordTableView::onClickToRecord() : name " <<
// it->field("name");
// qDebug() << "RecordTableView::onClickToRecord() : crypt " <<
// it->field(boost::mpl::c_str < crypt_type > ::value);

// if(it->field(boost::mpl::c_str < crypt_type > ::value) == "1")
// if(fullDir.length() == 0 || currentFile.length() == 0)
// meta_editor->dir_file_empty_reaction(MetaEditor::DIRFILEEMPTY_REACTION_SUPPRESS_ERROR);

//// В редактор заносится информация, идет ли работа с зашифрованным текстом
// meta_editor->misc_field(boost::mpl::c_str < crypt_type > ::value,
// it->field(boost::mpl::c_str < crypt_type > ::value));

//// В редакторе устанавливается функция обратного вызова для чтения данных
// meta_editor->load_callback(item->editor_load_callback);

// meta_editor->load_textarea();
//// edView->set_textarea(table->get_text(index.row()));

//// Заполняются прочие инфо-поля
// meta_editor->pin(it->field("pin"));
// meta_editor->name(it->field("name"));
// meta_editor->author(it->field("author"));
// meta_editor->home(it->field("home"));
// meta_editor->url(it->field("url"));
// meta_editor->tags(it->field("tags"));

// QString id = it->field("id");
// meta_editor->misc_field("id", id);

// meta_editor->misc_field("title", it->field("name"));

//// Устанавливается путь до ветки в которой лежит запись (в виде названий
///веток)
// QString path = qobject_cast<RecordScreen *>(parent())->tree_path();

//// В мобильном интерфейсе редактор должен показывать путь до записи
// if(appconfig->interface_mode() == "mobile")
// meta_editor->tree_path(path);

//// В редакторе восстанавливается позиция курсора и прокрутки если это
///необходимо
// if(appconfig->remember_cursor_at_ordinary_selection()) {
// meta_editor->cursor_position(walkhistory.cursor_position(id));
// meta_editor->scrollbar_position(walkhistory.scrollbar_position(id));
// }

//// Обновление иконки аттачей
// if(it->attach_table()->size() == 0)
// meta_editor->_to_attach->setIcon(meta_editor->_icon_attach_not_exists);   //
// Если нет приаттаченных файлов
// else
// meta_editor->_to_attach->setIcon(meta_editor->_icon_attach_exists);   // Есть
// приаттаченные файлы
// }

void rctrl_t::sychronize_attachtable_to_item(const pos_source pos)
{
	// Выясняется ссылка на таблицу конечных данных
	// auto table = _source_model->tree_item();

	// Устанавливается таблица приаттаченных файлов
	AttachTableController* attachTableController = _blogger->control_tab()->attachtable_screen()->attach_table_ctrl(); //gl_paras->attachtable_controller(); // find_object<AttachTableController>("attachTableController");
	attachTableController->attach_table_data(_source_model->item(pos)->attach_table());
}

// bool RecordController::is_tree_item_exists(void)
// {
////    if(_source_model->table_data() == nullptr)
////        return true;
////    else
////        return false;
// return _source_model->tree_item() ? true : false;
// }

//// Set a new set of data to the list of recordings  // Установка нового набора
///данных для списка записей
// void TableController::reset_tabledata_test(TableData *rtData)
// {

////    _view->deleteLater();
// _view->reset();

// recordSourceModel->deleteLater();

// recordSourceModel = new TableModel(this);
////    recordProxyModel->deleteLater();
////    recordProxyModel= new RecordTableProxyModel(this);
// recordProxyModel->clear();
// recordProxyModel->setSourceModel(recordSourceModel);

////    // Pointer to the data reported to the data source    // Указатель на
///данные сообщается источнику данных
////    recordSourceModel->reset_tabledata(rtData);

// _view->setModel(recordProxyModel);

// reset_tabledata(rtData);
// }

RecordModel* rctrl_t::source_model()
{
	return _source_model;
}

RecordProxyModel* rctrl_t::proxy_model()
{
	return _proxy_model;
}

web::TabWidget* rctrl_t::tab_widget()
{
	return _tab_widget;
}

//void rctrl_t::tab_widget(web::TabWidget* tab_)
//{
//	_tab_widget = tab_;
//}

// void RecordController::init_source_model(TreeModelKnow *_shadow_branch,
// MainWindow *main_window, MetaEditor *_editor_screen)
// {
// qDebug() << "In RecordController init_source_model() start";
// _source_model->shadow_branch(_shadow_branch);

//// Обновление набора данных с последующим выбором первой строки
//// может быть очень длительным, поэтому показывается что процесс выполняется
//// QCursor cursor_wait=QCursor(Qt::BusyCursor);
//// sapp_t::instance()->setOverrideCursor(cursor_wait);
////    find_object<MainWindow>("mainwindow")
// main_window->setCursor(Qt::BusyCursor);

//// Pointer to the data reported to the data source    // Указатель на данные
///сообщается источнику данных
////    _source_model->root(tree_item);    // reset_tabledata(table_data);

//// Надо обязательно сбросить selection model
// _view->selectionModel()->clear();

//// Если список конечных записей не пуст
// bool removeSelection = true;

// if(_source_model->size() > 0) {
//// Нужно выяснить, на какой записи ранее стояло выделение
// int workPos = _shadow_branch->root()->work_pos();

//// Если номер записи допустимый
// if(workPos > 0 && workPos < _source_model->rowCount()) {
//// Выделение устанавливается на нужную запись
//// selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) ,
///current_tree_selection_mode);
// _view->selectRow(workPos);
// _view->scrollTo(_view->currentIndex());   //
// QAbstractItemView::PositionAtCenter

// removeSelection = false;
// }
// }

//// If the selection does not need to install    // Если выделение
///устанавливать ненужно
// if(removeSelection) {
//// Надо очистить поля области редактировния
////        find_object<MetaEditor>(meta_editor_singleton_name)
// _editor_screen->clear_all();

//// При выборе записи обновление инструментов было бы вызвано автоматически
//// в альтернативной ветке (там "эмулируется" клик по записи)
//// А так как записей нет, то обновление инструментов надо
//// вызвать принудительно
// qobject_cast<RecordScreen *>(parent())->tools_update();
// }

//// sapp_t::instance()->restoreOverrideCursor();
////    find_object<MainWindow>("mainwindow")
// main_window->unsetCursor();

////
///globalparameters.entrance()->activiated_registered().first->tabWidget()->tree_item(tree_item);

// qDebug() << "In RecordTableView set new model stop";

// }

////boost::intrusive_ptr<TreeItem> RecordController::tree_item()
////{
////    return _source_model->tree_item();
////}

////void RecordController::init_source_model(boost::intrusive_ptr<TreeItem>
///item)
////{
////    _source_model->root(item);
////}

// void RecordController::init_source_model(boost::intrusive_ptr<TreeItem>
// tree_item)
// {
// qDebug() << "In RecordController init_source_model() start";

//// Обновление набора данных с последующим выбором первой строки
//// может быть очень длительным, поэтому показывается что процесс выполняется
//// QCursor cursor_wait=QCursor(Qt::BusyCursor);
//// sapp_t::instance()->setOverrideCursor(cursor_wait);
// find_object<MainWindow>("mainwindow")->setCursor(Qt::BusyCursor);

//// Pointer to the data reported to the data source    // Указатель на данные
///сообщается источнику данных
// _source_model->root(tree_item);    // reset_tabledata(table_data);

//// Надо обязательно сбросить selection model
// _view->selectionModel()->clear();

//// Если список конечных записей не пуст
// bool removeSelection = true;

// if(_source_model->size() > 0) {
//// Нужно выяснить, на какой записи ранее стояло выделение
// int workPos = tree_item->work_pos();

//// Если номер записи допустимый
// if(workPos > 0 && workPos < _source_model->rowCount()) {
//// Выделение устанавливается на нужную запись
//// selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) ,
///current_tree_selection_mode);
// _view->selectRow(workPos);
// _view->scrollTo(_view->currentIndex());   //
// QAbstractItemView::PositionAtCenter

// removeSelection = false;
// }
// }

//// If the selection does not need to install    // Если выделение
///устанавливать ненужно
// if(removeSelection) {
//// Надо очистить поля области редактировния
// find_object<MetaEditor>(meta_editor_singleton_name)->clear_all();

//// При выборе записи обновление инструментов было бы вызвано автоматически
//// в альтернативной ветке (там "эмулируется" клик по записи)
//// А так как записей нет, то обновление инструментов надо
//// вызвать принудительно
// qobject_cast<RecordScreen *>(parent())->tools_update();
// }

//// sapp_t::instance()->restoreOverrideCursor();
// find_object<MainWindow>("mainwindow")->unsetCursor();

////
///globalparameters.entrance()->activiated_registered().first->tabWidget()->tree_item(tree_item);

// qDebug() << "In RecordTableView set new model stop";
// }

//// Set a new set of data to the list of recordings  // Установка нового набора
///данных для списка записей
// void RecordController::reset_tabledata(std::shared_ptr<RecordTable>
// table_data)
// {
// qDebug() << "In RecordTableView reset_tabledata() start";

//// Обновление набора данных с последующим выбором первой строки
//// может быть очень длительным, поэтому показывается что процесс выполняется
//// QCursor cursor_wait=QCursor(Qt::BusyCursor);
//// sapp_t::instance()->setOverrideCursor(cursor_wait);
// find_object<MainWindow>("mainwindow")->setCursor(Qt::BusyCursor);

//// Pointer to the data reported to the data source    // Указатель на данные
///сообщается источнику данных
// _source_model->reset_tabledata(table_data);

//// Надо обязательно сбросить selection model
// _view->selectionModel()->clear();

//// Если список конечных записей не пуст
// bool removeSelection = true;

// if(_source_model->rowCount() > 0) {
//// Нужно выяснить, на какой записи ранее стояло выделение
// int workPos = table_data->work_pos();

//// Если номер записи допустимый
// if(workPos > 0 && workPos < _source_model->rowCount()) {
//// Выделение устанавливается на нужную запись
//// selectionModel()->setCurrentIndex( model()->index( workPos, 0 ) ,
///current_tree_selection_mode);
// _view->selectRow(workPos);
// _view->scrollTo(_view->currentIndex());   //
// QAbstractItemView::PositionAtCenter

// removeSelection = false;
// }
// }

//// If the selection does not need to install    // Если выделение
///устанавливать ненужно
// if(removeSelection) {
//// Надо очистить поля области редактировния
// find_object<MetaEditor>(meta_editor_singleton_name)->clear_all();

//// При выборе записи обновление инструментов было бы вызвано автоматически
//// в альтернативной ветке (там "эмулируется" клик по записи)
//// А так как записей нет, то обновление инструментов надо
//// вызвать принудительно
// qobject_cast<RecordScreen *>(parent())->tools_update();
// }

//// sapp_t::instance()->restoreOverrideCursor();
// find_object<MainWindow>("mainwindow")->unsetCursor();

// qDebug() << "In RecordTableView set new model stop";
// }

// Заполнение переданного объекта буфера обмена данными из указанных записей
// Индексы QModelIndexList передаются от Proxy модели
// Fill the object passed clipboard data from these records
// Index QModelIndexList transferred from the Proxy pattern
void rctrl_t::add_items_to_clipboard(ClipboardRecords* clipboardRecords, QModelIndexList items_copy)
{
	// Выясняется ссылка на таблицу конечных данных
	// auto table = _source_model->tree_item();
	// Перебираются записи и вносятся в буфер обмена
	for (int i = 0; i < items_copy.size(); ++i) {
		index_source index_ = index<index_source>(index_proxy(items_copy.at(i)));

		// The image recording, including all text data (text records, property
		// records list an attached file)        // Образ записи, включающий все
		// текстовые данные (текст записи, свойства записи, перечень приаттаченных
		// файлов)
		boost::intrusive_ptr<i_t> record =
		    _source_model->item(pos_source(((QModelIndex)index_).row()));
		if (record->is_lite()) record->to_fat();
		clipboardRecords->add_record(record);
	}
}

int rctrl_t::row_count(void) const
{
	return _tab_widget->count(); //_proxy_model->rowCount();
}

//// Get the number of the first highlighted item on the screen   // Получение
///номера первого выделенного элемента на экране
// PosProxy RecordController::first_selectionpos(void)const
// {
// return _view->selection_first_pos();
// }

//// Получение ID первого выделенного элемента на экране
// IdType RecordController::first_selectionid(void) const
// {
// return _view->selection_first_id();
// }

// void RecordController::cursor_to_index(boost::intrusive_ptr<TreeItem> it)
// {
// PosSource pos_source_ = _source_model->position(it->id());
// cursor_to_index(index<PosProxy>(pos_source_));
// }

// void RecordController::select(PosProxy pos_proxy_)
// {
// cursor_to_index(pos_proxy_);
////    emit _view->clicked((QModelIndex)index_proxy(pos_proxy_));

// }

// void RecordController::select(IdType id)
// {
//// Выясняется ссылка на таблицу конечных данных
////    auto table = _source_model->tree_item();

//// Если таблица конечных данных задана
//// (Не задана таблица может быть по причине если ветка зашифрована и введен
///неверный пароль, или при вводе пароля была нажата отмена)
////    if(table) {
//// Номер записи в Source данных
// PosSource pos_source_ = _source_model->position(id);

////    if(_view->selection_first_pos()
////       != sourcepos_to_proxypos(pos)
////      ) {

// PosProxy pos_proxy_ = index<PosProxy>(pos_source_);

// cursor_to_index(pos_proxy_);

////    emit _view->clicked((QModelIndex)index_proxy(pos_proxy_));

////    }

////    }

////    PosSource tab_index = _source_model->position(id);

////    if(_tab_widget->currentIndex() != (int)pos_source_)
///_tab_widget->setCurrentIndex((int)pos_source_);
// }

// IndexSource RecordController::index_source(IdType id) const
// {
//// Выясняется ссылка на таблицу конечных данных
////    auto table = _source_model->tree_item();

//// Номер записи в Source данных
// PosSource pos_source_ = _source_model->position(id);
// PosProxy proxy_pos_ = pos_proxy(pos_source_);
// return index<IndexSource>(proxy_pos_);
// }

// IndexSource RecordController::index_source(PosSource pos_source_)const
// {
// PosProxy proxy_pos_ = pos_proxy(pos_source_);
// return index<IndexSource>(proxy_pos_);
// }

// IndexSource RecordController::index_source(PosProxy pos_proxy_) const
// {
// if(pos_proxy_ < 0 || pos_proxy_ >= _proxy_model->rowCount())
// return IndexSource(QModelIndex());

// IndexProxy proxyIndex = index_proxy(pos_proxy_);
// IndexSource index(_proxy_model->mapToSource((QModelIndex)proxyIndex));

// return index;
// }

// IndexSource RecordController::index_source(IndexProxy proxyIndex) const
// {
// if(!((QModelIndex)proxyIndex).isValid())
// return IndexSource(QModelIndex());

// QModelIndex index = _proxy_model->mapToSource((QModelIndex)proxyIndex);

// return IndexSource(index);
// }

// IndexProxy RecordController::index_proxy(PosSource pos_source_) const
// {
// return index<IndexProxy>(index<PosProxy>(pos_source_));
// }

// IndexProxy RecordController::index_proxy(IdType id) const
// {
//// Выясняется ссылка на таблицу конечных данных
////    auto table = _source_model->tree_item();

//// Номер записи в Source данных
// PosSource source_pos = _source_model->position(id);
// PosProxy proxy_pos = pos_proxy(source_pos);

// return index<IndexProxy>(proxy_pos);
// }

// IndexProxy RecordController::index_proxy(PosProxy pos) const
// {
// if(pos < 0 || pos >= _proxy_model->rowCount())
// return IndexProxy(QModelIndex());

// QModelIndex index = _proxy_model->index(pos, 0);

// return IndexProxy(index);
// }

// IndexProxy RecordController::index_proxy(IndexSource sourceIndex) const
// {
// if(!((QModelIndex)sourceIndex).isValid())
// return IndexProxy(QModelIndex());

// QModelIndex index = _proxy_model->mapFromSource((QModelIndex)sourceIndex);

// return IndexProxy(index);
// }

//// Преобразование Proxy индекса в позицию на экране (так, как это будет
///выглядеть при Proxy модели)
// PosProxy RecordController::pos_proxy(IndexProxy index) const
// {
// if(!((QModelIndex)index).isValid())
// return PosProxy(-1);

// return PosProxy(((QModelIndex)index).row());
// }

// PosProxy RecordController::pos_proxy(PosSource source_pos) const
// {
// assert((int)source_pos != -1);
// QModelIndex proxyIndex =
// _proxy_model->mapFromSource(_source_model->index(source_pos, 0));
// assert(((QModelIndex)proxyIndex).row() != -1);
// return PosProxy(proxyIndex.row());
// }

//// Преобразование Source индекса в позицию на экране (так, как это будет
///выглядеть при Source модели)
// PosSource RecordController::pos_source(IndexSource index) const
// {
// if(!((QModelIndex)index).isValid())
// return PosSource(-1);

// return PosSource(((QModelIndex)index).row());
// }

// PosSource RecordController::pos_source(PosProxy pos_proxy_) const
// {
// QModelIndex sourceIndex =
// _proxy_model->mapToSource(_proxy_model->index((int)pos_proxy_, 0));
// return PosSource(sourceIndex.row());
// }

// Копирование отмеченных записей в буфер обмена с удалением
// из таблицы конечных записей
void rctrl_t::cut(void)
{
	// Надо сохранить запись, так как перед копированием в буфер обмена запись
	// обязательно должна быть сохранена, иначе редактирование,
	// которое было после открытия записи и до нажатия Cut, потеряется
	// find_object<MetaEditor>(meta_editor_singleton_name)
	_blogger->save_textarea();

	copy();

	QModelIndexList items_for_delete = _view->selectionModel()->selectedIndexes();
	// Проверка, выбраны ли записи
	if (items_for_delete.count() == 0) {
		qDebug() << "Records for delete not selected.";

		QMessageBox msgBox;
		msgBox.setText("Please select at least one record for delete.");
		msgBox.exec();

		return;
	}
	QVector<id_value> delete_ids;
	// QVector<int>        del_rows;
	for (auto it = items_for_delete.begin(); it != items_for_delete.end(); it++) {
		QModelIndex curr_idx;
		curr_idx = *it;

		id_value append_id(curr_idx.data(RECORD_ID_ROLE).toString());
		// Если идентификатор не содержится в перечне удаляемых идентификаторов
		// это может произойти если видно несколько столбцов - у каждой ячейки будет
		// один и тот же идентификатор записи
		if (!delete_ids.contains(append_id)) {
			qDebug() << "Mark for delete item id " << append_id;
			delete_ids.append(append_id);
			// del_rows.append(curr_idx.row());
		}
	}
	remove(delete_ids);
}

// Копирование отмеченных записей в буфер обмена
void rctrl_t::copy(void)
{
	// Объект с записями помещается в буфер обмена
	QApplication::clipboard()->setMimeData(_view->get_selected_records());
}

// Insert records from the clipboard    // Вставка записей из буфера обмена
void rctrl_t::paste(void)
{
	// Проверяется, содержит ли буфер обмена данные нужного формата
	const QMimeData* mimeData = QApplication::clipboard()->mimeData();
	if (mimeData == nullptr) return;
	if (!(mimeData->hasFormat(QString(gl_para::_program_instance_name) + "/records"))) return;
	// Создается ссылка на буфер обмена
	QClipboard* clipboardBuf = QApplication::clipboard();

	// Извлечение объекта из буфера обмена
	// const clipboardrecords *rcd=new clipboardrecords();
	const ClipboardRecords* clipboard_records;
	clipboard_records = qobject_cast<const ClipboardRecords*>(clipboardBuf->mimeData());
	clipboard_records->print();

	// Выясняется количество записей в буфере
	int nList = clipboard_records->size();
	// Пробегаются все записи в буфере
	for (int i = 0; i < nList; i++) {
		auto it = clipboard_records->record(i);
		// it->field<id_type>(get_unical_id());
		addnew_item(RecordIndex::instance([&] { return _source_model; }, it), true); // , add_new_record_to_end
	}
	// Обновление на экране ветки, на которой стоит засветка,
	// так как количество хранимых в ветке записей поменялось
	// find_object<TreeScreen>(tree_screen_singleton_name)
	gl_paras->tree_screen()->view()->update_selected_indexes();
}

//// Слот для добавления новой записи в конец таблицы
// void RecordController::addnew_to_end(void)
// {
// qDebug() << "In slot add_new_toend_context()";

// addnew_blank(add_new_record_to_end);
// }

//// Слот для добавления новой записи перед выделенной строкой
// void RecordController::addnew_before(void)
// {
// qDebug() << "In slot add_new_before_context()";

// addnew_blank(add_new_record_before);
// }

//// Слот для добавления новой записи после выделенной строки
//// Slot to add a new record after the highlighted row
// void RecordController::addnew_after(void)
// {
// qDebug() << "In slot add_new_after_context()";

// addnew_blank(add_new_record_after);
// }

//// Слот для добавления новой записи после выделенной строки
//// Slot to add a new record after the highlighted row
// void RecordTableController::autoAddNewAfterContext(void)
// {
// qDebug() << "In slot add_new_after_context()";

// autoAddNewRecord(ADD_NEW_RECORD_AFTER);
// }

//// Вызов окна добавления данных в таблицу конечных записей
//// Call window to add data to a table of final entries
// void RecordTableController::autoAddNewRecord(int mode)
// {
// qDebug() << "In add_new_record()";

////// Создается окно ввода данных
////// При клике Ok внутри этого окна, будет создана временная директория
////// с картинками, содержащимися в тексте
////    AddNewRecord addNewRecordWin;
////    int i=addNewRecordWin.exec();
////    if(i==QDialog::Rejected)
////        return; // Была нажата отмена, ничего ненужно делать

////// Имя директории, в которой расположены файлы картинок, используемые в
///тексте и приаттаченные файлы
////    QString directory=addNewRecordWin.getImagesDirectory();

//// todo: сделать заполнение таблицы приаттаченных файлов
// BrowserView *browser_view = globalParameters.getBrowserView();
// Record *current_view = browser_view->getCurrentRecord();

// if(browser_view->getCurrentRecord()) {
// if(!browser_view->getCurrentRecord()->isEmpty()) {

// RecordTableController *recordTableController =
// globalParameters.getRecordTableScreen()->getRecordTableController();
// int pos = recordTableController->getFirstSelectionPos();

////RecordTableView *view = recordTableController->getView();
////view->loadUrl();

// RecordTableModel *recordTableModel =
// recordTableController->getRecordTableModel();
// RecordTableData *recordTableData = recordTableModel->getRecordTableData();
// Record *record = recordTableData->getRecord(pos);

// if(record->getNaturalFieldSource("url") !=
// current_view->getNaturalFieldSource("url")) {
// Record record;

////if(record.isLite())record.switchToFat();

// record.setNaturalFieldSource("id",
// browser_view->getNaturalFieldSource("id"));
// record.setNaturalFieldSource("name",
// browser_view->getNaturalFieldSource("name"));
// record.setNaturalFieldSource("author",
// browser_view->getNaturalFieldSource("author"));
// record.setNaturalFieldSource("url",
// browser_view->getNaturalFieldSource("url"));
// record.setNaturalFieldSource("tags",
// browser_view->getNaturalFieldSource("tags"));
////record.setText(browser_view->getText());

////record.setPictureFiles( DiskHelper::getFilesFromDirectory(directory,
///"*.png") );

//// Пока что принята концепция, что файлы нельзя приаттачить в момент создания
///записи
//// Запись должна быть создана, потом можно аттачить файлы.
//// Это ограничение для "ленивого" программинга, но пока так
//// record.setAttachFiles( DiskHelper::getFilesFromDirectory(directory,
///"*.bin") );

////// Временная директория с картинками и приаттаченными файлами удаляется
////    DiskHelper::removeDirectory(directory);

//// Введенные данные добавляются (все только что введенные данные передаются в
///функцию addNew() незашифрованными)
// addNew(mode, record);
// }
// }
// }
// }

// Вызов окна добавления данных в таблицу конечных записей
// Call window to add data to a table of final entries
web::WebView* rctrl_t::addnew_blank()
{ // int mode
	qDebug() << "In addnew_blank()";

	//// Создается окно ввода данных
	//// При клике Ok внутри этого окна, будет создана временная директория
	//// с картинками, содержащимися в тексте
	// AddNewRecord addNewRecordWin;

	// int i = addNewRecordWin.exec();

	// if(i == QDialog::Rejected)
	// return; // Была нажата отмена, ничего ненужно делать

	// Имя директории, в которой расположены файлы картинок, используемые в тексте
	// и приаттаченные файлы
	QString directory = DiskHelper::create_temp_directory(); //
	// addNewRecordWin.getImagesDirectory();

	// todo: сделать заполнение таблицы приаттаченных файлов

	full_fields_map data;
	boost::fusion::at_key<id_key>(data) = get_unical_id();
	boost::fusion::at_key<pin_key>(data) = pin_value(bool_from_check_state[Qt::Unchecked]);
	boost::fusion::at_key<name_key>(data) = "";
	boost::fusion::at_key<author_key>(data) = "";
	boost::fusion::at_key<home_key>(data) = web::Browser::_defaulthome;
	boost::fusion::at_key<url_key>(data) = web::Browser::_defaulthome;
	boost::fusion::at_key<tags_key>(data) = tags_value(QStringList());

	boost::intrusive_ptr<i_t> it = boost::intrusive_ptr<i_t>(new i_t(_view->current_item(), data));

	it->to_fat();
	// record.setText(addNewRecordWin.getField("text"));
	// record.setField("pin",   addNewRecordWin.getField("pin"));
	// record.setField("name",   addNewRecordWin.getField("name"));
	// record.setField("author", addNewRecordWin.getField("author"));
	// record.setField("url",    addNewRecordWin.getField("url"));
	// record.setField("tags",   addNewRecordWin.getField("tags"));
	it->text_to_fat("");
	// item->field("pin",   _check_state[Qt::Unchecked]);
	// item->field("name",   "");
	// item->field("author", "");
	// item->field("home",   web::Browser::_defaulthome);
	// item->field("url",    web::Browser::_defaulthome);
	// item->field("tags",   "");

	it->picture_files(DiskHelper::get_files_from_directory(directory, "*.png"));

	// Пока что принята концепция, что файлы нельзя приаттачить в момент создания
	// записи
	// Запись должна быть создана, потом можно аттачить файлы.
	// Это ограничение для "ленивого" программинга, но пока так
	// record.setAttachFiles( DiskHelper::getFilesFromDirectory(directory,
	// "*.bin") );

	// Временная директория с картинками и приаттаченными файлами удаляется
	DiskHelper::remove_directory(directory);

	// Введенные данные добавляются (все только что введенные данные передаются в
	// функцию addNew() незашифрованными)
	return addnew_item(RecordIndex::instance([&] { return _source_model; }, it), true); // , mode
}

rs_t* rctrl_t::record_screen()
{
	return _record_screen;
}

//// Вызов окна добавления данных в таблицу конечных записей
//// Call window to add data to a table of final entries
// web::WebView *rctrl_t::addnew_item_fat(boost::intrusive_ptr<RecordIndex>
// record_index_, bool make_current){	// , const int mode
// boost::intrusive_ptr<TreeItem> item = record_index_->host();
// qDebug() << "In add_new_record()";

////// Создается окно ввода данных
////// При клике Ok внутри этого окна, будет создана временная директория
////// с картинками, содержащимися в тексте
//// AddNewRecord addNewRecordWin;

//// int i = addNewRecordWin.exec();

//// if(i == QDialog::Rejected)
//// return; // Была нажата отмена, ничего ненужно делать

//// Имя директории, в которой расположены файлы картинок, используемые в тексте
///и приаттаченные файлы
// QString directory = DiskHelper::create_temp_directory();	//
//// addNewRecordWin.getImagesDirectory();
//// todo: сделать заполнение таблицы приаттаченных файлов
//// Record record;
//// if(record.isLite())record.switchToFat();
// if(item->is_lite()) item->to_fat();
// // I met this!!! but before in, I am sure I called to_fat() already. just at
// delete?
// assert(! item->is_lite());

//// record.setText(addNewRecordWin.getField("text"));
//// record.setField("pin",   addNewRecordWin.getField("pin"));
//// record.setField("name",   addNewRecordWin.getField("name"));
//// record.setField("author", addNewRecordWin.getField("author"));
//// record.setField("url",    addNewRecordWin.getField("url"));
//// record.setField("tags",   addNewRecordWin.getField("tags"));

//// record.setText("");
//// record.setField("pin",   _check_state[Qt::Unchecked]);
//// record.setField("name",   "");
//// record.setField("author", "");
//// record.setField("home",   url.toString());
//// record.setField("url",    url.toString());
//// record.setField("tags",   "");

// item->picture_files(DiskHelper::get_files_from_directory(directory,
// "*.png"));

//// record->generator(generator);

//// Пока что принята концепция, что файлы нельзя приаттачить в момент создания
///записи
//// Запись должна быть создана, потом можно аттачить файлы.
//// Это ограничение для "ленивого" программинга, но пока так
//// record->setAttachFiles(DiskHelper::getFilesFromDirectory(directory,
///"*.bin"));

//// Временная директория с картинками и приаттаченными файлами удаляется
// DiskHelper::remove_directory(directory);

//// Введенные данные добавляются (все только что введенные данные передаются в
///функцию addNew() незашифрованными)
// return addnew_item(record_index_, make_current);	// , mode
// }

// Функция добавления новой записи в таблицу конечных записей
// Принимает полный формат записи
web::WebView* rctrl_t::addnew_item(boost::intrusive_ptr<RecordIndex> record_index_, bool make_current)
{ // , const int mode
	// boost::intrusive_ptr<TreeItem> item_target = record_index_->host();

	boost::intrusive_ptr<i_t> item_target = record_index_->host();
	qDebug() << "In add_new_record()";

	//// Создается окно ввода данных
	//// При клике Ok внутри этого окна, будет создана временная директория
	//// с картинками, содержащимися в тексте
	// AddNewRecord addNewRecordWin;

	// int i = addNewRecordWin.exec();

	// if(i == QDialog::Rejected)
	// return; // Была нажата отмена, ничего ненужно делать

	// Имя директории, в которой расположены файлы картинок, используемые в тексте
	// и приаттаченные файлы
	QString directory = DiskHelper::create_temp_directory(); //
	// addNewRecordWin.getImagesDirectory();
	// todo: сделать заполнение таблицы приаттаченных файлов
	// Record record;
	// if(record.isLite())record.switchToFat();
	if (item_target->is_lite()) item_target->to_fat(); // I met this!!! but before in, I am sure I called
	// to_fat() already. just at delete?
	assert(!item_target->is_lite());

	// record.setText(addNewRecordWin.getField("text"));
	// record.setField("pin",   addNewRecordWin.getField("pin"));
	// record.setField("name",   addNewRecordWin.getField("name"));
	// record.setField("author", addNewRecordWin.getField("author"));
	// record.setField("url",    addNewRecordWin.getField("url"));
	// record.setField("tags",   addNewRecordWin.getField("tags"));

	// record.setText("");
	// record.setField("pin",   _check_state[Qt::Unchecked]);
	// record.setField("name",   "");
	// record.setField("author", "");
	// record.setField("home",   url.toString());
	// record.setField("url",    url.toString());
	// record.setField("tags",   "");

	item_target->picture_files(
	    DiskHelper::get_files_from_directory(directory, "*.png"));

	// record->generator(generator);

	// Пока что принята концепция, что файлы нельзя приаттачить в момент создания
	// записи
	// Запись должна быть создана, потом можно аттачить файлы.
	// Это ограничение для "ленивого" программинга, но пока так
	// record->setAttachFiles(DiskHelper::getFilesFromDirectory(directory,
	// "*.bin"));

	// Временная директория с картинками и приаттаченными файлами удаляется
	DiskHelper::remove_directory(directory);

	qDebug() << "In addnew_item()";
	web::WebView* v = nullptr;

	//// Получение Source-индекса первой выделенной строки
	// index_source source_position_index =
	// index<index_source>(_view->current_item());	//
	// selection_first<IndexSource>();
	//// if(!position_index.isValid()) {
	//// position_index = _view->currentIndex();   // very wrong!
	//// }
	// if(! ((QModelIndex) source_position_index).isValid() &&
	// _source_model->size() > 0){
	//// if(0 == _source_model->tree_item()->size()) {
	////
	///_source_model->tree_item(globalparameters.entrance()->shadow_branch()->root());
	//// }

	// source_position_index = _source_model->createIndex(_source_model->size() -
	// 1
	// , 0
	// , static_cast<void *>(_source_model->item(pos_source(_source_model->size()
	// - 1)).get())
	// );
	// }
	// assert(((QModelIndex) source_position_index).row() <
	// _source_model->size());
	//// assert(position_index.isValid());

	//// if(!position_index.isValid()) {
	//// position_index = view->getFirstSelectionProxyIndex();
	//// }

	//// if(position_index.isValid() //   // do not need? but if it is invalid,
	///the sequence will be changed. hughvonyoung@gmail.com
	//////       && item.getNaturalFieldSource("url") !=
	///web::DockedWindow::_defaulthome
	//// ) {

	pos_source selected_source_position(-1);
	// assert(_source_model->item(item_target->field<id_type>()));
	// Вставка новых данных, возвращаемая позиция - это позиция в Source данных
	if (!_source_model->item(
		[&](const id_value id) { return id == item_target->id(); })) {
		v = _source_model->insert_new_item(
		    item_target); // , mode	// source_position_index,
		selected_source_position = _tab_widget->webViewIndex(v);
	} else {
		selected_source_position = _source_model->position(item_target->id());
		v = _tab_widget->webView(static_cast<int>(selected_source_position));
	}
	assert(selected_source_position != -1);
	assert(_source_model->item(selected_source_position) == item_target || item_target->field<url_key>() == url_value("") || item_target->field<url_key>() == web::Browser::_defaulthome);
	assert(_source_model->position(item_target->id()) == selected_source_position || item_target->field<url_key>() == url_value("") || item_target->field<url_key>() == web::Browser::_defaulthome);
	// assert(_source_model->child(selected_position) == item);

	_view->reset();
	//	_source_model->insert_new_item(item_target);//repeated with above!
	_source_model->on_table_config_changed();
	_proxy_model->setSourceModel(_source_model);
	_view->setModel(_proxy_model);
	if (make_current) select_as_current(index<pos_proxy>(selected_source_position)); // , mode // modify _source_model? yeah

	// Сохранение дерева веток
	// find_object<TreeScreen>(tree_screen_singleton_name)

	gl_paras->tree_screen()->view()->know_model_save();

	// }

	// else {
	//// Add a record in the database // Добавление записи в базу
	// RecordTableData *table = nullptr;
	// table = recordSourceModel->getRecordTableData();

	////        if(table == nullptr) {}

	// int selPos = table->insertNewRecord(ADD_NEW_RECORD_TO_END, 0, record);

	// view->moveCursorToNewRecord(mode, convertSourcePosToProxyPos(selPos));

	//// Сохранение дерева веток
	// find_object<TreeScreen>(tree_screen_singleton_name)->saveKnowTree();
	// }

	selected_source_position = _source_model->position(item_target->id());
	assert(selected_source_position != -1 || item_target->field<id_key>() == id_value(global_root_id));
	assert(_source_model->item(selected_source_position) == item_target || item_target->field<id_key>() == id_value(global_root_id));
	assert(v);

	return v; // selected_source_position;
}

//// Функция добавления новой записи в таблицу конечных записей
//// Принимает полный формат записи
// int TableController::addnew_page_record(boost::intrusive_ptr<Record> record,
// int mode)
// {
// qDebug() << "In add_new()";

//// Получение Source-индекса первой выделенной строки
// QModelIndex posIndex = _view->getFirstSelectionSourceIndex();

////    //    assert(posIndex.isValid());
////    if(!posIndex.isValid()) {
////        posIndex = view->getFirstSelectionProxyIndex();
////    }

////    if(posIndex.isValid() //   // do not need? yeah, I am sure.
///hughvonyoung@gmail.com
////       //       && record.getNaturalFieldSource("url") !=
///web::DockedWindow::_defaulthome
////      ) {

//// Вставка новых данных, возвращаемая позиция - это позиция в Source данных
// int selected_position = _source_model->add_record(mode, posIndex, record);

// _view->moveCursorToNewRecord(mode, sourcepos_to_proxypos(selected_position));

//// Сохранение дерева веток
// find_object<TreeScreen>(tree_screen_singleton_name)->save_knowtree();

////    }

////    else {
////        // Add a record in the database // Добавление записи в базу
////        RecordTableData *table = nullptr;
////        table = recordSourceModel->getRecordTableData();

////        //        if(table == nullptr) {}

////        int selPos = table->insertNewRecord(ADD_NEW_RECORD_TO_END, 0,
///record);

////        view->moveCursorToNewRecord(mode,
///convertSourcePosToProxyPos(selPos));

////        // Сохранение дерева веток
////        find_object<TreeScreen>(tree_screen_singleton_name)->saveKnowTree();
////    }

// return selected_position;
// }

//// Вызов окна добавления данных в таблицу конечных записей
//// Call window to add data to a table of final entries
// int TableController::new_record_from_url(   // move to request_record(Url...
// const QUrl &url
// , const int mode
////    , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
///web::WebView *, Record *const>> generator
// )
// {
// qDebug() << "In add_new_record()";

////    // Создается окно ввода данных
////    // При клике Ok внутри этого окна, будет создана временная директория
////    // с картинками, содержащимися в тексте
////    AddNewRecord addNewRecordWin;

////    int i = addNewRecordWin.exec();

////    if(i == QDialog::Rejected)
////        return; // Была нажата отмена, ничего ненужно делать

//// Имя директории, в которой расположены файлы картинок, используемые в тексте
///и приаттаченные файлы
// QString directory = DiskHelper::createTempDirectory();  //
//// addNewRecordWin.getImagesDirectory();

//// todo: сделать заполнение таблицы приаттаченных файлов

// boost::intrusive_ptr<Record> record = boost::intrusive_ptr<Record>(new
// Record());
// record->switchToFat();
////    record.setText(addNewRecordWin.getField("text"));
////    record.setField("pin",   addNewRecordWin.getField("pin"));
////    record.setField("name",   addNewRecordWin.getField("name"));
////    record.setField("author", addNewRecordWin.getField("author"));
////    record.setField("url",    addNewRecordWin.getField("url"));
////    record.setField("tags",   addNewRecordWin.getField("tags"));
// record->setTextToFat("");
// record->setField("pin",   _check_state[Qt::Unchecked]);
// record->setField("name",   "");
// record->setField("author", "");
// record->setField("home",   url.toString());
// record->setField("url",    url.toString());
// record->setField("tags",   "");

// record->setPictureFiles(DiskHelper::getFilesFromDirectory(directory,
// "*.png"));

////    record.generator(generator);

//// Пока что принята концепция, что файлы нельзя приаттачить в момент создания
///записи
//// Запись должна быть создана, потом можно аттачить файлы.
//// Это ограничение для "ленивого" программинга, но пока так
//// record->setAttachFiles( DiskHelper::getFilesFromDirectory(directory,
///"*.bin") );

//// Временная директория с картинками и приаттаченными файлами удаляется
// DiskHelper::removeDirectory(directory);

//// Введенные данные добавляются (все только что введенные данные передаются в
///функцию addNew() незашифрованными)
// return addnew_record(record, mode);
// }

void rctrl_t::on_edit_fieldcontext(void)
{
	_view->edit_field_context();
	// view->loadUrl();
}

// Действия при нажатии кнопки редактирования записи
bool rctrl_t::edit_field_context(index_proxy proxyIndex)
{
	qDebug() << "RecordController::edit_field_context()";
	bool result = false;
	index_source sourceIndex = index<index_source>(proxyIndex);
	pos_source pos_source_(
	    ((QModelIndex)sourceIndex).row()); // Номер строки в базе

	// Создается окно ввода данных, после выхода из этой функции окно должно
	// удалиться
	InfoFieldsEditor edit_record_dialog;

	// Выясняется ссылка на таблицу конечных данных
	auto item = _source_model->item(pos_source_);
	//	auto debug_tags = detail::to_qstring(item->field<tags_key>());
	// Поля окна заполняются начальными значениями
	edit_record_dialog.setField("pin", detail::to_qstring(item->field<pin_key>()));       // "pin"
	edit_record_dialog.setField("name", detail::to_qstring(item->field<name_key>()));     // "name"
	edit_record_dialog.setField("author", detail::to_qstring(item->field<author_key>())); // "author"
	edit_record_dialog.setField("home", detail::to_qstring(item->field<home_key>()));     // "home"
	edit_record_dialog.setField("url", detail::to_qstring(item->field<url_key>()));       // "url"
	edit_record_dialog.setField("tags", detail::to_qstring(item->field<tags_key>()));     // "tags"

	int i = edit_record_dialog.exec();
	if (i != QDialog::Rejected) {
		// result; // Была нажата отмена, ничего ненужно делать

		// Измененные данные записываются
		edit_field(pos_source_, detail::from_qstring<pin_key>(edit_record_dialog.getField("pin")), detail::from_qstring<name_key>(edit_record_dialog.getField("name")), detail::from_qstring<author_key>(edit_record_dialog.getField("author")), detail::from_qstring<home_key>(edit_record_dialog.getField("home")), detail::from_qstring<url_key>(edit_record_dialog.getField("url")), detail::from_qstring<tags_key>(edit_record_dialog.getField("tags")));
		// auto editor = globalparameters.meta_editor();
		// if(! editor->isHidden())
		//		item->page()->metaeditor_sychronize();
		result = true;
	}
	return result;
}

// Функция сохранения отредактированных полей записи в таблицу конечных записей
void rctrl_t::edit_field(pos_source pos, const pin_value& pin, const name_value& name, const author_value& author, const home_value& home, const url_value& url, const tags_value& tags)
{
	qDebug() << "In edit_field()";

	// Выясняется ссылка на таблицу конечных данных
	// auto pages = _source_model->browser_pages();

	//// Переданные отредактированные поля преобразуются в вид имя-значение
	// QMap<QString, QString> edit_data;
	// edit_data["pin"] = pin;
	// edit_data["name"] = name;
	// edit_data["author"] = author;
	// edit_data["home"] = home;
	// edit_data["url"] = url;
	// edit_data["tags"] = tags;

	// Обновление новых данных в таблице конечных записей
	auto it = _source_model->item(pos);
	it->field<pin_key>(pin);
	it->field<name_key>(name);
	it->field<author_key>(author);
	it->field<home_key>(home);
	it->field<url_key>(url);
	it->field<tags_key>(tags);
//	assert(it->field<tags_key>() == tags);
#ifdef USE_EDITOR_WRAP
	// Обновление инфополей в области редактирования записи
	// auto *_editing_window = globalparameters.editor_dock(); //
	// find_object<MetaEditor>(meta_editor_singleton_name);
	_editing_window->pin(pin);
	_editing_window->name(name);
	_editing_window->author(author);
	_editing_window->home(home);
	_editing_window->url(url);
	_editing_window->tags(tags);
#endif // USE_EDITOR_WRAP
	// Сохранение дерева веток
	// find_object<TreeScreen>(tree_screen_singleton_name)
	gl_paras->tree_screen()->view()->know_model_save();
}

// Обработка клика по удалению записи в контекстном меню и по кнопке на панели
void rctrl_t::close_context(void)
{
	//// Создается окно с вопросом нужно удалять запись (записи) или нет
	// QMessageBox messageBox(_view);
	// messageBox.setWindowTitle("Delete");
	// messageBox.setText(tr("Are you sure to delete this record(s)?"));
	// QAbstractButton *cancelButton = messageBox.addButton(tr("Cancel"),
	// QMessageBox::RejectRole);
	// QAbstractButton *deleteButton = messageBox.addButton(tr("Delete"),
	// QMessageBox::AcceptRole);

	// Q_UNUSED(cancelButton);

	// messageBox.exec();

	// if(messageBox.clickedButton() == deleteButton) {
	//// Выбранные данные удаляются

	QModelIndexList items_for_delete = _view->selectionModel()->selectedIndexes();
	// Проверка, выбраны ли записи
	if (items_for_delete.count() == 0) {
		qDebug() << "Records for delete not selected.";

		QMessageBox msgBox;
		msgBox.setText("Please select at least one record for delete.");
		msgBox.exec();

		return;
	}
	QVector<id_value> delete_ids;
	// QVector<int>        del_rows;
	for (auto it = items_for_delete.begin(); it != items_for_delete.end(); it++) {
		QModelIndex curr_idx;
		curr_idx = *it;

		id_value append_id(curr_idx.data(RECORD_ID_ROLE).toString());
		// Если идентификатор не содержится в перечне удаляемых идентификаторов
		// это может произойти если видно несколько столбцов - у каждой ячейки будет
		// один и тот же идентификатор записи
		if (!delete_ids.contains(append_id)) {
			qDebug() << "Mark for delete item id " << append_id;
			delete_ids.append(append_id);
			// del_rows.append(curr_idx.row());
		}
	}
	remove(delete_ids);
	if (_view->currentIndex().row() != _tab_widget->currentIndex()) select_as_current(pos_proxy(_tab_widget->currentIndex())); // }
}

void rctrl_t::remove(id_value delete_id)
{
	QVector<id_value> delete_ids;
	delete_ids.append(delete_id);
	remove(delete_ids);
}

//// Удаление отмеченных записей
// void RecordController::pages_remove(void)
// {
// qDebug() << "RecordController::delete_items_selected()";

//// Получение списка Item-элементов, подлежащих удалению. Индексы Proxy модели
// QModelIndexList items_for_delete =
// _view->selectionModel()->selectedIndexes();

//// Проверка, выбраны ли записи
// if(items_for_delete.count() == 0) {
// qDebug() << "Records for delete not selected.";

// QMessageBox msgBox;
// msgBox.setText("Please select at least one record for delete.");
// msgBox.exec();

// return;
// }

//// Сбор в массив всех идентификаторов, которые нужно удалить
//// Напрямую пробегать массив item-элементов и удалять из него нельзя
//// так как итератор начинает указывать на несуществующие элементы
// QVector<QString>    del_ids;
// QVector<int>        del_rows;
////    QModelIndexList::iterator it;

// for(auto it = items_for_delete.begin(); it != items_for_delete.end(); it++) {
// QModelIndex curr_idx;
// curr_idx = *it;

// QString append_id = curr_idx.data(RECORD_ID_ROLE).toString();

//// Если идентификатор не содержится в перечне удаляемых идентификаторов
//// это может произойти если видно несколько столбцов - у каждой ячейки будет
///один и тот же идентификатор записи
// if(!del_ids.contains(append_id)) {
// qDebug() << "Mark for delete item id " << append_id;
// del_ids.append(append_id);
// del_rows.append(curr_idx.row());
// }
// }

//// Массив удаляемых номеров строк (в Proxy-нумерации) сортируется так чтоб
///вначале были индексы с наибольшим номером
// qSort(del_rows.begin(), del_rows.end(), qGreater<int>());
// int lastRowNum = del_rows[0]; // Максимальный номер удаляемой строки

//// Номер строки на который надо установить засветку после удаления
//// Засветка устанавливается на запись, следующую после последней удаляемой
// int selection_row_num = lastRowNum + 1 - del_rows.count();
// qDebug() << "After delete cursor set to" << selection_row_num << "row";

//// Надо очистить поля области редактировния, чтобы редактор не пытался
///сохранить текущую открытую, но удаленную запись
////    find_object<MetaEditor>(meta_editor_singleton_name)
// globalparameters.meta_editor()->clear_all();

//// Вызывается удаление отмеченных записей
// pages_remove_from_browser(del_ids);

////    // Сохранение дерева веток
////    //    find_object<TreeScreen>(tree_screen_singleton_name)
////    globalparameters.tree_screen()->save_knowtree();

////    // Обновление на экране ветки, на которой стоит засветка,
////    // так как количество хранимых в ветке записей поменялось
////    //    find_object<TreeScreen>(tree_screen_singleton_name)
////    globalparameters.tree_screen()->update_selected();

//// Установка курсора на нужную позицию
// if(selection_row_num >= 0 && selection_row_num < _proxy_model->rowCount())
// _view->selectRow(selection_row_num);

//// Если таблица конечных записей пуста
// if(_proxy_model->rowCount() == 0) {
//// Нужно очистить поле редактирования чтобы невидно было текста
//// последней удаленной записи
////        find_object<MetaEditor>(meta_editor_singleton_name)
// globalparameters.meta_editor()->clear_all();
// }

// qobject_cast<RecordScreen *>(parent())->tools_update();
// }

// Удаление одной записи по идентификатору
void rctrl_t::remove(QVector<id_value> delete_ids)
{
	// Remove records for the specified list of identifiers // Удаление записей по
	// указанному списку идентификаторов
	auto pages_remove_from_browser =
	    [&](std::vector<id_value> pre) -> web::WebView* {
		web::WebView* _new_view = nullptr;

		qDebug() << "Remove rows by ID list: " << pre;
		QVector<id_value> sorted_delete_ids;

		auto sort_delete = [&](std::vector<id_value>& y) -> std::vector<id_value> {
			std::sort(std::begin(y), std::end(y), [&](id_value i1, id_value i2) {
				auto item1 =
				    _source_model->item([&](const id_value id) { return id == i1; });
				auto item2 =
				    _source_model->item([&](const id_value id) { return id == i2; });
				bool result = false;
				if (item1 && item2) {
					web::WebView* v1 = nullptr;
					web::WebView* v2 = nullptr;
					if ((v1 = _tab_widget->find([&](boost::intrusive_ptr<const ::Binder> b) {
						    return url_equal(b->host()->field<url_key>(), item1->field<url_key>()) && b->host()->id() == item1->id();
					    })) &&
					    (v2 = _tab_widget->find([&](boost::intrusive_ptr<const ::Binder> b) {
						    return url_equal(b->host()->field<url_key>(), item2->field<url_key>()) && b->host()->id() == item1->id();
					    }))) {
						assert(v1->page()->binder()->host() == item1);
						assert(v2->page()->binder()->host() == item2);
						auto index1 = _tab_widget->webViewIndex(v1);
						assert(index1 != -1);
						auto index2 = _tab_widget->webViewIndex(v2);
						assert(index2 != -1);
						result = index1 < index2;
					}
				}
				return result;
			});
			// for(auto v : y)std::cout << v << ' ';
			return y;
		};
		// Выясняется ссылка на таблицу конечных данных
		// auto _browser_pages = _source_model->browser_pages();
		if (_source_model->count() >
		    0) { // return nullptr;	// if(!_browser_pages)
			bool changed = false;

			pre = sort_delete(pre);
			// web::WebView	*_first_delete	= nullptr;
			int _new_index = 0;
			for (size_t i = 0; i < pre.size(); i++) {
				id_value id = pre[i];
				// QModelIndex idx = id_to_proxyindex(id);
				auto item =
				    _source_model->item([&](const id_value id_) { return id_ == id; });
				if (item) {
					web::WebView* v = nullptr;
					if ((v = _tab_widget->find([&](boost::intrusive_ptr<const ::Binder> b) {
						    return url_equal(b->host()->field<url_key>(), item->field<url_key>()) && b->host()->id() == item->id();
					    }))) { // "url"
						   // item_to_be_deleted->unique_page()
						   ////            int index =
						   ///_tab_widget->indexOf(item->unique_page()->view());
						   ////            if(index != -1)_tab_widget->closeTab(index);
						assert(v->page()->binder()->host() == item);
						auto index = _tab_widget->webViewIndex(v);
						assert(index != -1);
						// _source_model->remove_child(item);  // doing nothing
						_tab_widget->closeTab(index); // _tab_widget->indexOf(item->bounded_page()->view())
						_new_index = _tab_widget->currentIndex();
						changed = true;
						// if(0 == i){
						////				_first_delete	= v;
						// _new_index = (0 >= index) ? 0 : index - 1;
						// }
						if (-1 == _new_index) {
							//
							_new_index = 0;
						}
						sorted_delete_ids << id;
					}
				}
				// globalparameters.find_screen()->remove_id(id);  // ?
			}
			(void)changed;
			// _new_view = _tab_widget->sibling(_first_delete);
		}
		// if(_tab_widget->count() > 0)_new_view = _tab_widget->currentWebView();
		return _new_view;
	};

	// QVector<QString> del_ids;
	// del_ids.append(del_id);
	// remove_children(del_ids);

	// Сбор в массив всех идентификаторов, которые нужно удалить
	// Напрямую пробегать массив item-элементов и удалять из него нельзя
	// так как итератор начинает указывать на несуществующие элементы
	// QVector<QString>    del_ids;
	// del_ids.append(del_id);
	QVector<int> del_rows;
	// QModelIndexList::iterator it;
	for (auto del_id : delete_ids) { // for(it = items_for_delete.begin(); it !=
		// items_for_delete.end(); it++) {
		QModelIndex curr_idx;
		curr_idx = index<index_source>(del_id); // index<IndexProxy>(del_id); //*it;

		// QString append_id = curr_idx.data(RECORD_ID_ROLE).toString();

		// Если идентификатор не содержится в перечне удаляемых идентификаторов
		// это может произойти если видно несколько столбцов - у каждой ячейки будет
		// один и тот же идентификатор записи
		// if(!del_ids.contains(append_id)) {
		qDebug() << "Mark for delete item id " << del_id;
		// del_ids.append(append_id);
		del_rows.append(curr_idx.row());
		// }
	}
//// Массив удаляемых номеров строк (в Proxy-нумерации) сортируется так чтоб
///вначале были индексы с наибольшим номером
// qSort(del_rows.begin(), del_rows.end(), qGreater<int>());
// int lastRowNum = del_rows[0];	// Максимальный номер удаляемой строки

//// Номер строки на который надо установить засветку после удаления
//// Засветка устанавливается на запись, следующую после последней удаляемой
// int selection_row_num = lastRowNum + 1 - del_rows.count();
// qDebug() << "After delete cursor set to" << selection_row_num << "row";

//// Надо очистить поля области редактировния, чтобы редактор не пытался
///сохранить текущую открытую, но удаленную запись
//// find_object<MetaEditor>(meta_editor_singleton_name)
#ifdef USE_FILE_PER_TREEITEM
	_editing_window->clear_all();
#endif // USE_FILE_PER_TREEITEM
	// Вызывается удаление отмеченных записей
	// web::WebView *v =
	std::vector<id_value> pre;
	for (int i = 0; i < delete_ids.count(); i++) {
		id_value id = delete_ids[i];
		auto item =
		    _source_model->item([&](const id_value id_) { return id_ == id; });
		if (item) pre.push_back(id);
	}
	bool changed = false;
	pages_remove_from_browser(pre);
	// if(v){
	// auto binder = v->page()->binder();
	// if(binder)
	// if(binder->host() !=
	// _view->current_item())this->select_as_current(index<pos_proxy>(binder->host()));
	// }
	for (auto row : del_rows) {
		_proxy_model->removeRow(row);
		changed = true;
	}
	if (changed) {
		// Удаляется строка в Proxy модели
		// Proxy модель сама должна уведомить вид о своем изменении, так как именно
		// она подключена к виду
		// _proxy_model->removeRow(idx.row()); // ? is this still needed after
		// source changed?
		_view->reset();
		_proxy_model->setSourceModel(_source_model);
		_view->setModel(_proxy_model);
		if (_tab_widget->count() > 0) {
			//			web::WebView *_new_view = nullptr;
			auto _new_index = _tab_widget->currentIndex();
			web::WebView* _new_view = _tab_widget->webView(_new_index);
			if (_new_view != _tab_widget->currentWebView()) _tab_widget->select_as_current(_new_view);
			// _new_view = _tab_widget->currentWebView();
			auto _binder = _new_view->page()->binder();
			if (_new_view && _binder) {
				auto it = _binder->host();
				auto index_ = index<pos_proxy>(it->id());
				if (_view->current_item() != it) this->select_as_current(index_);
				// for(auto id : real_delete_ids){
				// IndexProxy index_ = index<IndexProxy>(id);	// invalid
				// emit _view->dataChanged(index_, index_);
				// }
			}
		}
	}
	////        //// Сохранение дерева веток
	////        ////    find_object<TreeScreen>(tree_screen_singleton_name)
	////        // globalparameters.tree_screen()->save_knowtree();
	////        //// Обновление на экране ветки, на которой стоит засветка,
	////        //// так как количество хранимых в ветке записей поменялось
	////        ////    find_object<TreeScreen>(tree_screen_singleton_name)
	////        // globalparameters.tree_screen()->update_selected();
	////        // Установка курсора на нужную позицию
	////    if(selection_row_num >= 0 && selection_row_num <
	///_proxy_model->rowCount())_view->selectRow(selection_row_num);
	////        // Если таблица конечных записей пуста
	if (_proxy_model->rowCount() == 0) {
// Нужно очистить поле редактирования чтобы невидно было текста
// последней удаленной записи
// find_object<MetaEditor>(meta_editor_singleton_name)
#ifdef USE_FILE_PER_TREEITEM
		if (_blogger) _blogger->clear_all();
#else
		if (_blogger) _blogger->close();
#endif // USE_FILE_PER_TREEITEM
	}
	// qobject_cast<rs_t *>(parent())
	_record_screen->tools_update();
}

//// Удаление одной записи по идентификатору
// void RecordController::remove_child(int index)
// {

// if(_source_model->count() == 0) // if(!_browser_pages)
// return;

////        QModelIndex idx = id_to_proxyindex(id);
// remove_child(_source_model->child(index)->id());
////        for(int j = 0; j < _browser_pages->size(); j++) {
////            if(_browser_pages->item(j)->id() == id) {
////                _browser_pages->delete_item_by_position(j); //
///remove_child(_browser_pages->item(j));
////            }
////        }

//// Удаляется строка в Proxy модели
//// Proxy модель сама должна уведомить вид о своем изменении, так как именно
///она подключена к виду
////        _proxy_model->removeRow(idx.row()); // ? is this still needed after
///source changed?
// _view->reset();
// _proxy_model->setSourceModel(_source_model);
// _view->setModel(_proxy_model);
////        globalparameters.find_screen()->remove_id(id);  // ?

// }

//// Перемещение записи вверх
// void RecordController::move_up(void)
// {
// qDebug() << "In moveup()";

//// Получение номера первой выделенной строки
// PosProxy pos_proxy_ = _view->selection_first<PosProxy>();

//// Выясняется ссылка на таблицу конечных данных
////    auto item = _source_model->browser_pages();

//// Перемещение текущей записи вверх
// _source_model->move_up(index<PosSource>(pos_proxy_));

//// Установка засветки на перемещенную запись
// cursor_to_index(PosProxy((int)pos_proxy_ - 1));

//// Сохранение дерева веток
////    find_object<TreeScreen>(tree_screen_singleton_name)
// globalparameters.tree_screen()->tree_view()->know_model_save();
// }

//// Перемещение записи вниз
// void RecordController::move_dn(void)
// {
// qDebug() << "In movedn()";

//// Получение номера первой выделенной строки
// PosProxy pos_proxy_ = _view->selection_first<PosProxy>();

//// Выясняется ссылка на таблицу конечных данных
////    auto item = _source_model->browser_pages();

//// Перемещение текущей записи вниз
// _source_model->move_dn(index<PosSource>(pos_proxy_));

//// Установка засветки на перемещенную запись
// cursor_to_index(PosProxy((int)pos_proxy_ + 1));

//// Сохранение дерева веток
////    find_object<TreeScreen>(tree_screen_singleton_name)
// globalparameters.tree_screen()->tree_view()->know_model_save();
// }

void rctrl_t::on_sort_requested(int logicalIndex, Qt::SortOrder order)
{
	// _proxy_model->setDynamicSortFilter(true);
	_proxy_model->setFilterKeyColumn(logicalIndex);
	_proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);

	auto current = _view->current_item(); // selection_first<boost::intrusive_ptr<TreeItem>>();

	// _record_controller->proxy_model()->sort(logicalIndex, order);
	auto header_title =
	    _source_model->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole)
		.toString();
	auto rating_field_description = fixedparameters.record_field_description(
	    QStringList() << boost::mpl::c_str<
		rating_key>::value)[boost::mpl::c_str<rating_key>::value];
	auto pin_field_description = fixedparameters.record_field_description(
	    QStringList() << boost::mpl::c_str<
		pin_key>::value)[boost::mpl::c_str<pin_key>::value];
	auto title_field_description = fixedparameters.record_field_description(
	    QStringList() << boost::mpl::c_str<
		name_key>::value)[boost::mpl::c_str<name_key>::value];

	_view->setSortingEnabled(true);

	_proxy_model->setSortRole(SORT_ROLE);

	std::vector<web::WebView*> v_list;
	for (int index_ = 0; index_ < _tab_widget->count(); index_++) v_list.push_back(_tab_widget->webView(index_));
	if (order == Qt::AscendingOrder) {
		if (header_title == pin_field_description) {
			// std::sort(v_list.begin(), v_list.end(), [&](web::WebView *v0,
			// web::WebView *v1){
			// return v0->page()->host()->field<pin_type>() == "" ?
			// v1->page()->host()->field<pin_type>() != "" ? true : false : false;
			// });
			// int t = 0;
			for (auto v : v_list) {
				if (v->page()->host()->field<pin_key>() != pin_value(bool_from_check_state[Qt::Unchecked])) _source_model->move(pos_source(_tab_widget->webViewIndex(v)), pos_source(0)); // , index<pos_source>(pos_proxy(_tab_widget->count() -
																									  // 1))
																									  // t ++;
			}
		} else if (header_title == title_field_description) {
			std::sort(v_list.begin(), v_list.end(), [&](web::WebView* v0, web::WebView* v1) {
				return v0->page()->host()->field<name_key>() <
				    v1->page()->host()->field<name_key>();
			});
			int t = 0;
			for (auto v : v_list) {
				_source_model->move(pos_source(_tab_widget->webViewIndex(v)), pos_source(t));
				t++;
			}
		} else if (header_title == rating_field_description) {
			std::sort(v_list.begin(), v_list.end(), [&](web::WebView* v0, web::WebView* v1) {
				return detail::to_qstring(v0->page()->host()->field<rating_key>()).toULongLong() > detail::to_qstring(v1->page()->host()->field<rating_key>()).toULongLong();
			});
			int t = 0;
			for (auto v : v_list) {
				_source_model->move(pos_source(_tab_widget->webViewIndex(v)), pos_source(t));
				t++;
			}
		}
	} else {
		if (header_title == pin_field_description) {
			// std::sort(v_list.begin(), v_list.end(), [&](web::WebView *v0,
			// web::WebView *v1){
			// return v0->page()->host()->field<pin_type>() != "" ?
			// v1->page()->host()->field<pin_type>() == "" ? true : false : false;
			// });
			// int t = 0;
			for (auto v : v_list) {
				if (v->page()->host()->field<pin_key>() != pin_value(bool_from_check_state[Qt::Unchecked])) _source_model->move(pos_source(_tab_widget->webViewIndex(v)), pos_source(_tab_widget->count() - 1));
				// t ++;
			} //
		} else if (header_title == title_field_description) {
			std::sort(v_list.begin(), v_list.end(), [&](web::WebView* v0, web::WebView* v1) {
				return v0->page()->host()->field<name_key>() >
				    v1->page()->host()->field<name_key>();
			});
			int t = 0;
			for (auto v : v_list) {
				_source_model->move(pos_source(_tab_widget->webViewIndex(v)), pos_source(t));
				t++;
			}
		} else if (header_title == rating_field_description) {
			std::sort(v_list.begin(), v_list.end(), [&](web::WebView* v0, web::WebView* v1) {
				return detail::to_qstring(v0->page()->host()->field<rating_key>()).toULongLong() > detail::to_qstring(v1->page()->host()->field<rating_key>()).toULongLong();
			});
			int t = 0;
			for (auto v : v_list) {
				_source_model->move(pos_source(_tab_widget->webViewIndex(v)), pos_source(t));
				t++;
			}
		}
	}
	_view->reset();
	_proxy_model->setSourceModel(_source_model);
	_view->setModel(_proxy_model);
	_view->setSortingEnabled(false);
	_proxy_model->setSortRole(Qt::InitialSortOrderRole);
	_proxy_model->invalidate();
	select_as_current(index<pos_proxy>(_source_model->index(current)));
	// Сохранение дерева веток
	// find_object<TreeScreen>(tree_screen_singleton_name)
	gl_paras->tree_screen()->view()->know_model_save();
	_record_screen->tools_update();
}

// Клик по пункту "Сортировка" в контекстном меню
void rctrl_t::on_sort_click(void)
{
	// int	index_ = _record_screen->_sort->data().toInt();
	// if(_view->horizontalHeader()->sortIndicatorOrder())
	// Если сортировка еще не включена
	if (!_view->isSortingEnabled()) {
		// Включается сортировка
		_view->setSortingEnabled(true);

		_proxy_model->setSortRole(SORT_ROLE); // Qt::DisplayRole

		// Включается сортировка по нужному столбцу
		int n = _record_screen->_sort->data().toInt(); // В actionSort хранится
		// номер столбца, по которому
		// нужно сортировать
		qDebug() << "Sort column number " << n;
		_proxy_model->sort(n, Qt::DescendingOrder);

		// Треугольничек сортировки переставляется на нужный столбец
		_view->horizontalHeader()->setSortIndicator(
		    n, Qt::DescendingOrder); // Qt::AscendingOrder

		// Запрещается передвижение заголовков столбцов
		// так как после переноса неправильно устанавливается треугольничек
		// сортировки, он остается на том же по счету столбце
		// horizontalHeader()->setSectionsMovable(false);
	} else {
		// Оменяется сортировка
		_view->setSortingEnabled(false);
		_proxy_model->setSortRole(Qt::InitialSortOrderRole);
		_proxy_model->invalidate();

		// азрешается передвижение заголовков столбцов
		// horizontalHeader()->setSectionsMovable(true);
	}
	// qobject_cast<rs_t *>(parent())
	_record_screen->tools_update();
}

// Слот, срабатывающий при вызове настроек
void rctrl_t::settings(void)
{
	AppConfigDialog appconfigdialog("pageMain");
	// appconfigdialog.changePage("pageMain");
	appconfigdialog.show();

	// Todo: Возвращение фокуса почему-то не работает, надо разбираться
	// (а может просто не выделяется виджет, в Qt5 вделенный виджет не виден в
	// дефолтной схеме)
	// qDebug() << "Set focus to RecordTableView";
	// this->setFocus();
}

void rctrl_t::on_recordtable_configchange(void)
{
	_source_model->on_table_config_changed();
	_view->restore_column_width();
	_view->restore_header_state();
}

void rctrl_t::on_print_click(void)
{
	// RecordScreen *parentPointer = qobject_cast<RecordScreen *>(parent());

	RecordPrint print_dialog(static_cast<rs_t*>(_record_screen)); // parentPointer
	print_dialog.setModel(_proxy_model);
	print_dialog.generateHtmlTableFromModel();
	print_dialog.setTitleToHtml(
	    record_screen()
		->objectName()); // _source_model->_browser_pages->path_as_name_with_delimiter("
	// / ")

	print_dialog.exec();
}

// record url may be empty or web::Browser::_defaulthome
boost::intrusive_ptr<i_t> rctrl_t::synchronize(boost::intrusive_ptr<RecordIndex> record_index_)
{
	boost::intrusive_ptr<i_t> it = record_index_->host();
	boost::intrusive_ptr<i_t> _found_item =
	    _source_model->item([&](const id_value id) { return id == it->id(); });
	web::WebView* v = nullptr;
	pos_source source_position(-1);
	// if(! _found_item){
	//////    assert(record_controller);
	//// auto browser_pages = this->_source_model->browser_pages();
	//// assert(browser_pages);
	//// if(!_item) {
	//// Record record;
	//// if(record.isLite())record.switchToFat();
	//// assert(!item->is_lite());
	// if(it->is_lite())it->to_fat();
	//// item->is_registered_to_record_controller_and_tab_widget(true);
	// v		= this->addnew_item_fat(record_index_, add_new_record_after);
	// source_position = _tab_widget->webViewIndex(v);	//
	// recordTableController->autoAddNewAfterContext();
	// assert(source_position != - 1);

	//// _source_model->on_table_config_changed();
	// }else

	source_position = _source_model->position(_found_item->id());
	v = _tab_widget->webView(static_cast<int>(source_position));

	_found_item = v->page()->binder()->host();
	assert(_found_item == _source_model->item(source_position));
	assert(_found_item);

	pos_source pos = _source_model->position(_found_item->id());

	assert(pos == source_position); // maybe duplicated
	// _item = _source_model->item(source_position);

	assert(_found_item.get() == it.get());
	// assert(_item_.get() == item.get());
	// assert(record == _record);
	assert(url_equal(_found_item->field<url_key>(), it->field<url_key>()));
	// }
	// assert(_record);
	if (_found_item->is_lite()) _found_item->to_fat();
	// }
	// if(_view->current_item() != _item) {
	// auto current_item = _view->current_item();
	//
	// _source_model->on_table_config_changed();

	index_proxy proxy_index =
	    index<index_proxy>(index<pos_proxy>(source_position));
	auto qindex_proxy = static_cast<QModelIndex>(proxy_index);
	_view->dataChanged(qindex_proxy, qindex_proxy);

	_view->restore_column_width();
	_view->restore_header_state();

	select_as_current(index<pos_proxy>(_found_item));
	//// }item

	return _found_item; // _record;
}

// Record *register_record(const QUrl &_url
// , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, Record *const>> generator
// , std::shared_ptr<sd::_interface<sd::meta_info<boost::shared_ptr<void>>,
// web::WebView *, Record *const>> activator
// , RecordTableController *_record_ontroller)
// {

// assert(_record_ontroller);
// RecordTableData *recordtabledata =
// _record_ontroller->getRecordTableModel()->getRecordTableData();
// assert(recordtabledata);

// Record record;

// if(record.isLite())record.switchToFat();

////                QString title = _url.toString(); // not ready yet

// record.setNaturalFieldSource("pin",     "");
// record.setNaturalFieldSource("name",    "");
// record.setNaturalFieldSource("author",  "");
// record.setNaturalFieldSource("home",    _url.toString());
// record.setNaturalFieldSource("url",     _url.toString());    // only changed
// record.setNaturalFieldSource("tags",    "");

// int source_position = _record_ontroller->new_record(_url,
// ADD_NEW_RECORD_AFTER, generator);
// //recordTableController->autoAddNewAfterContext();

////    Record *_record = nullptr;
////    _record = recordtabledata->record(_url);    // does not work every time?
///still not update now?

////                int pos = _record_ontroller->getFirstSelectionPos();
// auto record_ =
// _record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(source_position);

////assert(record_ == _record);
// assert(record_->getNaturalFieldSource("url") == _url.toString());
////            }
////assert(_record);
// return record_; //_record;
// }

// boost::intrusive_ptr<TreeItem> rctrl_t::find(const QUrl &_url){
// boost::intrusive_ptr<TreeItem> item = nullptr;

//// TableController *_record_controller =
///globalparameters.table_screen()->table_controller();
//// assert(_record_controller);

//// if(_record_controller) {
//// auto browser_pages = this->_source_model->browser_pages();
// assert(_source_model->count() > 0);

//// if(browser_pages) {
// item = _source_model->item(_url);
//// }

//// }

// return item;
// }

// namespace web {
// class Browser;
// class WebView;
// }

// boost::intrusive_ptr<TreeItem> RecordController::item_request_from_tree(
// boost::intrusive_ptr<TreeItem> item
////    , std::shared_ptr<CouplerDelegation> _record_binder
// )
// {
// TreeScreen *_tree_screen = globalparameters.tree_screen();
////    auto _know_model_root = tree_screen->know_root();
// auto _know_model_board = _tree_screen->know_model_board();
// auto _current_view_model = _tree_screen->tree_view()->source_model();
// boost::intrusive_ptr<TreeItem> _result(nullptr);    // =
// _know_model_board->root_item();

// if(item->is_lite())item->to_fat();

////    item->record_binder(_record_binder);
////    //    item->activator(activator);

////    //    item->is_registered_to_record_controller_and_tab_widget(false);

// if(!item->record_binder()) {

////        if(item->is_lite())item->to_fat();

////        item->binder(generator);
////        item->activator(activator);
////        item->is_registered_to_record_controller(true);

////    TableController *_record_controller =
///globalparameters.table_screen()->table_controller();
////    assert(_record_controller);

// QUrl find_url = QUrl(item->field("url"));

////    boost::intrusive_ptr<TreeItem> _source_root_item =
///tree_screen->know_branch()->item(TreeModel::delegater(find_url));        //
///on know_root semantic    // item won't work, for it is not inside
///_know_model_root if it is come from _know_model_branch
// boost::intrusive_ptr<TreeItem> _source_item =
// _know_model_board->item(TreeModel::delegater(find_url));

////    if(_source_root_item && !_source_item) {
////        auto result = tree_screen->cut_from_root(_source_root_item);

////        if(result)_source_item = tree_screen->paste_to_branch(result,
///_know_model_branch);

////        assert(result);
////        assert(_source_item);
////        assert((_source_item == result) && (result == _source_root_item));
////    }

// bool item_is_brand_new = false;
////    if(_record_controller) {
////    auto browser_pages = this->_source_model->browser_pages();
////    assert(browser_pages);

////    if(_source_model->count() > 0) {
// _result = _source_model->find_current(item);

// if(_source_item != _know_model_board->root_item()) {
// if(!_result) {

// assert(item == _source_item);

////                //                record->binder(generator);
////                //                record->activator(activator);
// if(_source_item->is_lite())_source_item->to_fat();

////                _source_item->binder(generator);
////                _source_item->activator(activator);

////                //            _item =
///register_item_to_browser_source_model(_source_item);
////                _source_item->is_registered_to_record_controller(true);
////                // _source_item->self_bind();
////                //                assert(_record);

////                // _record->active_immediately(active_immediately);
////                //                _record->generator(generator);

// _result = _source_item; // assert(_item.get() == _source_item.get());

// } else {
// assert(_result == _source_item);
// assert(_result->is_registered_to_browser() || _result->field("url") ==
// web::Browser::_defaulthome);
// }

// assert(!_result->is_lite());
////            assert(_result->is_registered_to_browser());

// } else {

// item_is_brand_new = true;

// if(!_result) {

////                if(item->is_lite())item->to_fat();

// if(item->field("id") == "")item->field("id", get_unical_id());

////                item->binder(generator);
////                item->activator(activator);

////                //            _item =
///register_item_to_browser_source_model(item);
////                item->is_registered_to_record_controller(true);
////                // item->self_bind();
// _result = item; // assert(_item.get() == item.get());
// } else {
// assert(_result == item);

// if(_result->is_lite())_result->to_fat();

// if(_result->field("id") == "")_result->field("id", get_unical_id());

// assert(_result->is_registered_to_browser() || _result->field("url") ==
// web::Browser::_defaulthome);
////                _item->binder(generator);
////                _item->activator(activator);
////                _item->is_registered_to_record_controller(true);
////                // _item->self_bind();
// }

// auto idx = _tree_screen->tree_view()->index_current();

//// if(idx.isValid()) {

// auto it = _current_view_model->item(idx);
// assert(it);

////        if(_item->is_lite())_item->to_fat();

// if(it != _result && item_is_brand_new) {
//// int pos
// _result = _tree_screen->branch_add(_result, true, _current_view_model); //
// it->insert_new_item(it->current_count() - 1, _result);
// _tree_screen->synchronized(false);
//// assert(_result == it->child(pos));
// }

// _tree_screen->knowtree_save();
//// }
// }

// if(_result->is_lite())_result->to_fat();

////        //            else {
////        _item->binder(generator);
////        _item->activator(activator);
////        //                _record->generate();
////        //            }

// assert(_result != _know_model_board->root_item());

////        assert(_result->is_registered_to_browser());

// assert(_result->field("url") == item->field("url"));
////    }

////    }

////    }

////    assert(_record);
// } else {
// _result = item;
// }

// if(_result->field("dir") == "")_result->field("dir", _result->field("id"));

// if(_result->field("file") == "")_result->field("file", "text.html");

// return _result;

// }

// boost::intrusive_ptr<TreeItem> RecordController::item_request_from_tree(
// const QUrl &_url
////    , std::shared_ptr<CouplerDelegation> _record_binder
// )
// {
// TreeScreen *_tree_screen = globalparameters.tree_screen();
////    auto _know_model_root = tree_screen->know_root();
// auto _know_model_board = _tree_screen->know_model_board();
// auto _current_view_model = _tree_screen->tree_view()->source_model();

// boost::intrusive_ptr<TreeItem> _result(nullptr);    // =
// _know_model_board->root_item();
////    boost::intrusive_ptr<TreeItem> _source_root_item =
///tree_screen->know_branch()->item(TreeModel::delegater(_url));    // on
///know_root semantic
// boost::intrusive_ptr<TreeItem> _source_item =
// _know_model_board->item(TreeModel::delegater(_url));

////    if(_source_root_item && !_source_item) {
////        auto result = tree_screen->cut_from_root(_source_root_item);

////        if(result)_source_item = tree_screen->paste_to_branch(result,
///_know_model_branch);

////        assert(result);
////        assert(_source_item);
////        assert((_source_item == result) && (result == _source_root_item));
////    }

// bool item_is_brand_new = false;
////    //    if(_record_controller) {
////    auto browser_pages = this->_source_model->browser_pages();
////    assert(browser_pages);

////    if(browser_pages) {
// _result = _source_model->find_current(_url);

// if(_source_item != _know_model_board->root_item()) {
// if(!_result) {

// if(_source_item->is_lite())_source_item->to_fat();

////            //            _source_item->binder(generator);
////            //            _source_item->activator(activator);

////            //            _item =
///register_item_to_browser_source_model(_source_item);

//// _source_item->is_registered_to_record_controller_and_tab_widget(false);
////            // _source_item->self_bind();
// _result = _source_item;
// } else {
// assert(_result == _source_item);

// if(_result->is_lite())_result->to_fat();

// if(_result->field("id") == "")_result->field("id", get_unical_id());

// assert(_result->is_registered_to_browser() || _result->field("url") ==
// web::Browser::_defaulthome);

////            //            _result->binder(generator);
////            //            _result->activator(activator);

//// _result->is_registered_to_record_controller_and_tab_widget(false);
////            // _item->self_bind();
// }

// assert(!_result->is_lite());
//// assert(_result->is_registered_to_record_controller_and_tab_widget());

// } else {
// item_is_brand_new = true;

// if(!_result) {

////                int pos = _record_ontroller->getFirstSelectionPos();
////                Record *previous_record =
///_record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

////                if(previous_record) {

////                    Record record;

////                    if(record.isLite())record.switchToFat();

////                    //QString title = d->view->title(); // not ready yet
////                    //record.setNaturalFieldSource("id",
///previous_record->getNaturalFieldSource("id"));   // id concept?
////                    record.setNaturalFieldSource("pin",   "");
////                    record.setNaturalFieldSource("name",
///previous_record->getNaturalFieldSource("name"));
////                    record.setNaturalFieldSource("author",
///previous_record->getNaturalFieldSource("author"));
////                    record.setNaturalFieldSource("url",    _url.toString());
///// only changed
////                    record.setNaturalFieldSource("tags",
///previous_record->getNaturalFieldSource("tags"));

////                    _record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);
/////recordTableController->autoAddNewAfterContext();
////                    _record = recordtabledata->getRecordByUrl(_url);
////                    //                int pos =
///_record_ontroller->getFirstSelectionPos();
////                    //                _record =
///_record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);
////                } else {

////    record.generator(generator);

//// Имя директории, в которой расположены файлы картинок, используемые в тексте
///и приаттаченные файлы
// QString directory = DiskHelper::create_temp_directory();  //

// QMap<QString, QString> data;
// data["id"]      = get_unical_id();
// data["pin"]     = _string_from_check_state[Qt::Unchecked];
// data["name"]    = "";
// data["author"]  = "";
// data["home"]    = _url.toString();
// data["url"]     = _url.toString();
// data["tags"]    = "";
// data["dir"]     = data["id"];
// data["file"]    = "text.html";

// boost::intrusive_ptr<TreeItem> item
// = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr, data));

////                if(record.isLite())
// item->to_fat();
// item->text_to_fat("");
////            //                QString title = _url.toString(); // not ready
///yet
////            item->field("id",       get_unical_id());
////            item->field("pin",      _check_state[Qt::Unchecked]);
////            item->field("name",     "");
////            item->field("author",   "");
////            item->field("home",     _url.toString());    // only changed
////            item->field("url",      _url.toString());    // only changed
////            item->field("tags",     "");

////            //
///_record_ontroller->addNew(ADD_NEW_RECORD_AFTER, record);
/////recordTableController->autoAddNewAfterContext();
////            //                _record =
///recordtabledata->getRecordByUrl(_url);
////            //                //                int pos =
///_record_ontroller->getFirstSelectionPos();
////            //                //                _record =
///_record_ontroller->getRecordTableModel()->getRecordTableData()->getRecord(pos);

////            //                //            }

////            //                record->binder(generator);
////            //                record->activator(activator);

// item->picture_files(DiskHelper::get_files_from_directory(directory,
// "*.png"));

//// Пока что принята концепция, что файлы нельзя приаттачить в момент создания
///записи
//// Запись должна быть создана, потом можно аттачить файлы.
//// Это ограничение для "ленивого" программинга, но пока так
//// record->setAttachFiles( DiskHelper::getFilesFromDirectory(directory,
///"*.bin") );

//// Временная директория с картинками и приаттаченными файлами удаляется
// DiskHelper::remove_directory(directory);

// if(item->is_lite())item->to_fat();

////            //            item->binder(generator);
////            //            item->activator(activator);

////            //            _item =
///register_item_to_browser_source_model(item);
////            item->is_registered_to_record_controller_and_tab_widget(true);
////            // item->self_bind();
////            //                assert(_record);
////            //                assert(_record->is_registered());
////            // _record->active_immediately(active_immediately);
////            //                _record->generator(generator);

// _result = item; // assert(_item.get() == item.get());
// } else {
// if(_result->is_lite())_result->to_fat();

// if(_result->field("id") == "")_result->field("id", get_unical_id());

// assert(_result->is_registered_to_browser() || _result->field("url") ==
// web::Browser::_defaulthome);

////            //            _result->binder(generator);
////            //            _result->activator(activator);

//// _result->is_registered_to_record_controller_and_tab_widget(true);
////            // _item->self_bind();
// }

// auto idx = _tree_screen->tree_view()->index_current();

//// if(idx.isValid()) {
// auto it = _current_view_model->item(idx);
// assert(it);

////        if(_item->is_lite())_item->to_fat();

// if(it != _result && item_is_brand_new) {
//// int pos
// _result = _tree_screen->branch_add(_result, true, _current_view_model); //
// it->insert_new_item(it->current_count() - 1, _result);
// assert(_result);
// _tree_screen->synchronized(false);
//// assert(_result == it->child(pos));
// }

// _tree_screen->knowtree_save();
//// }
// }

// if(_result->is_lite())_result->to_fat();

////    //        //            else {
////    //        //                // assert(_record->is_registered());
////    //        //                _record->binder(generator);
////    //        //                _record->activator(activator);
////    //        //                //                _record->generate();    //
///why?
////    //        //            }

////    _result->record_binder(_record_binder);
////    //    _result->activator(activator);

// assert(_result != _know_model_board->root_item());
////    // assert(_result->is_registered_to_record_controller_and_tab_widget());
////    assert(_result->field("url") == _url.toString());   // maybe other url
///loaded !
////    //    } // browser_pages

////    //    }

////    //    }

////    //    assert(_record);

// return _result;
// }

template <>
pos_proxy rctrl_t::index<pos_proxy>(const pos_source& source_pos) const
{
	// assert((int) source_pos != - 1);
	QModelIndex source_index = _source_model->index(
	    _source_model->item(source_pos) // (int) source_pos, 0, QModelIndex()
	    );
	// auto it = _source_model->item(source_pos);
	// QModelIndex source_index = _source_model->createIndex((int)source_pos, 0,
	// static_cast<void *>(it.get()));
	// QModelIndex source_index =
	// (QModelIndex)_source_model->index(_source_model->item(source_pos));
	QModelIndex proxy_index = _proxy_model->mapFromSource(source_index);

	// assert(proxy_index.row() != - 1);
	return pos_proxy(proxy_index.row());
}

template <>
pos_proxy rctrl_t::index<pos_proxy>(const index_proxy& index_) const
{
	if (!static_cast<QModelIndex>(index_).isValid()) return pos_proxy(-1);
	return pos_proxy(static_cast<QModelIndex>(index_).row());
}

template <>
pos_proxy rctrl_t::index<pos_proxy>(const index_source& is) const
{
	pos_source ps(static_cast<QModelIndex>(is).row());
	return index<pos_proxy>(ps);
}

template <>
pos_proxy rctrl_t::index<pos_proxy>(const id_value& id) const
{
	pos_source source_pos = _source_model->position(id);

	return index<pos_proxy>(source_pos);
}

template <>
pos_proxy rctrl_t::index<pos_proxy>(const boost::intrusive_ptr<i_t>& it) const
{
	assert(it);
	return index<pos_proxy>(it->id());
}

template <>
pos_source rctrl_t::index<pos_source>(const pos_proxy& pos_proxy_) const
{
	if (-1 == pos_proxy_) return pos_source(-1);
	QModelIndex fake_source_index = _proxy_model->mapToSource(_proxy_model->index(static_cast<int>(pos_proxy_), 0));

	return pos_source(fake_source_index.row());
}

template <>
pos_source rctrl_t::index<pos_source>(const index_proxy& ip) const
{
	pos_proxy pp(QModelIndex(ip).row());

	return index<pos_source>(pp);
}

template <>
pos_source rctrl_t::index<pos_source>(const index_source& index_) const
{
	if (!static_cast<QModelIndex>(index_).isValid()) return pos_source(-1);
	return pos_source(static_cast<QModelIndex>(index_).row());
}

template <>
pos_source rctrl_t::index<pos_source>(const id_value& id) const
{
	return _source_model->position(id);
}

template <>
pos_source rctrl_t::index<pos_source>(const boost::intrusive_ptr<i_t>& it) const
{
	assert(it);
	return index<pos_source>(it->id());
}

template <>
index_proxy rctrl_t::index<index_proxy>(const pos_source& pos_source_) const
{
	return index<index_proxy>(index<pos_proxy>(pos_source_));
}

template <>
index_proxy rctrl_t::index<index_proxy>(const pos_proxy& pos_proxy_) const
{
	if (pos_proxy_ < 0 || pos_proxy_ >= _proxy_model->rowCount()) return index_proxy(QModelIndex());
	auto pos_source_ = index<pos_source>(pos_proxy_);
	auto it = _source_model->item(pos_source_);
	auto index_source_ = _source_model->index(it);
	QModelIndex index = _proxy_model->mapFromSource(index_source_); // _proxy_model->index((int) pos_proxy_, 0);

	return index_proxy(index);
}

template <>
index_proxy rctrl_t::index<index_proxy>(const index_source& index_source_) const
{
	if (!static_cast<QModelIndex>(index_source_).isValid()) return index_proxy(QModelIndex());
	QModelIndex index_ = _proxy_model->mapFromSource(index_source_); // ((QModelIndex) sourceIndex).row(), 0)// (QModelIndex)sourceIndex

	return index_proxy(index_);
}

template <>
index_proxy rctrl_t::index<index_proxy>(const id_value& id) const // Выясняется ссылка на таблицу конечных данных
{
	// auto table = _source_model->tree_item();

	// Номер записи в Source данных
	pos_source source_pos = _source_model->position(id);
	pos_proxy proxy_pos = index<pos_proxy>(source_pos);

	return index<index_proxy>(proxy_pos);
}

template <>
index_proxy rctrl_t::index<index_proxy>(const boost::intrusive_ptr<i_t>& it) const
{
	assert(it);
	return index<index_proxy>(it->id());
}

template <>
index_source rctrl_t::index<index_source>(const pos_source& pos_source_) const
{
	// pos_proxy	proxy_pos_	= index<pos_proxy>(pos_source_);
	auto it = _source_model->item(pos_source_);
	return _source_model->index(it);
}

template <>
index_source rctrl_t::index<index_source>(const index_proxy& index_proxy_) const
{
	if (!static_cast<QModelIndex>(index_proxy_).isValid()) return index_source(QModelIndex());
	// QModelIndex index		=
	// _proxy_model->mapToSource(_proxy_model->index(((QModelIndex)
	// proxyIndex).row(), 0));
	pos_source pos_source_ = index<pos_source>(index_proxy_);
	auto it = _source_model->item(pos_source_);
	return _source_model->index(it);
}

template <>
index_source rctrl_t::index<index_source>(const pos_proxy& pos_proxy_) const
{
	if (pos_proxy_ < 0 || pos_proxy_ >= _proxy_model->rowCount()) return index_source(QModelIndex());
	// IndexProxy proxyIndex = index<IndexProxy>(pos_proxy_);
	// index_source
	// index_(_proxy_model->mapToSource(_proxy_model->index((int)pos_proxy_, 0)));

	pos_source pos_source_ = index<pos_source>(pos_proxy_);
	auto it = _source_model->item(pos_source_);
	return _source_model->index(it);
}

template <>
index_source rctrl_t::index<index_source>(const id_value& id) const // Выясняется ссылка на таблицу конечных данных
{
	// Номер записи в Source данных
	pos_source pos_source_ = _source_model->position(id);

	// PosProxy proxy_pos_ = index<PosProxy>(pos_source_);
	// return index_source(_source_model->index((int) pos_source_, 0));
	auto it = _source_model->item(pos_source_);
	return _source_model->index(it);
}

template <>
index_source rctrl_t::index<index_source>(const boost::intrusive_ptr<i_t>& it) const
{
	assert(it);
	// return index<index_source>(it->id());
	return _source_model->index(it);
}

template <>
id_value rctrl_t::index<id_value>(const pos_source& ps) const
{
	return _source_model->item(ps)->id();
}

template <>
id_value rctrl_t::index<id_value>(const index_proxy& ip) const
{
	return _source_model->item(index<pos_source>(ip))->id();
}

template <>
id_value rctrl_t::index<id_value>(const pos_proxy& pp) const
{
	return _source_model->item(index<pos_source>(pp))->id();
}

template <>
id_value rctrl_t::index<id_value>(const index_source& is) const
{
	return _source_model->item(index<pos_source>(is))->id();
}

template <>
id_value rctrl_t::index<id_value>(const boost::intrusive_ptr<i_t>& it) const
{
	assert(it);
	return index<id_value>(index<pos_source>(it->id()));
}

template <>
boost::intrusive_ptr<i_t> rctrl_t::index<boost::intrusive_ptr<i_t>>(const pos_source& ps) const
{
	return _source_model->item(ps);
}

template <>
boost::intrusive_ptr<i_t> rctrl_t::index<boost::intrusive_ptr<i_t>>(const index_proxy& ip) const
{
	return _source_model->item(index<pos_source>(ip));
}

template <>
boost::intrusive_ptr<i_t> rctrl_t::index<boost::intrusive_ptr<i_t>>(const pos_proxy& pp) const
{
	return _source_model->item(index<pos_source>(pp));
}

template <>
boost::intrusive_ptr<i_t> rctrl_t::index<boost::intrusive_ptr<i_t>>(const index_source& is) const
{
	return _source_model->item(index<pos_source>(is));
}

template <>
boost::intrusive_ptr<i_t> rctrl_t::index<boost::intrusive_ptr<i_t>>(const id_value& id) const
{
	return _source_model->item(index<pos_source>(id));
}

Blogger* rctrl_t::blogger() const
{
	return _blogger;
}

//sd::shared_ptr<rctrl_t> rctrl_t::renter() const { return _renter; }
