
// http://stackoverflow.com/questions/12988010/right-way-to-write-to-file-from-different-threads-using-qt-c
#include <mutex>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QAbstractItemModel>
#include <QDomNamedNodeMap>
#include <QMap>

//#include "models/tree/tree_item.dec"

#include "main.h"
#include "tree_item.h"
#include "tree_know_model.h"
#include "tree_model.h"
#include "tree_xml.h"

#include "libraries/clipboard_branch.h"
#include "libraries/clipboard_records.h"
#include "libraries/crypt/password.h"
#include "libraries/disk_helper.h"
#include "libraries/disk_helper.h"
#include "libraries/global_parameters.h"
#include "libraries/trash_monitoring.h"
#include "models/app_config/app_config.h"
#include "models/record_table/linker.hxx"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "views/app_config/app_config_dialog.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/main_window/hidable_tab.h"
#include "views/main_window/main_window.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

extern std::shared_ptr<gl_para> gl_paras;
extern const char* clipboard_items_root;
extern std::shared_ptr<AppConfig> appconfig;
extern TrashMonitoring trashmonitoring;
const char* global_root_id = "0";
// const char *global_root_parent_id = "-1";

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(tkm_t)
#endif

// Конструктор модели дерева, состоящего из Item элементов
tkm_t::tkm_t(tv_t* parent) // const QString &index_xml_file_name_,
    : tm_t(parent),
      _xml_file_path(gl_paras->root_path() + "/" + QDir(appconfig->data_dir()).dirName() + "/" + gl_para::_index_xml_file_name),
      _xml_tree(std::make_shared<XmlTree>())
{ // , _root_item(nullptr)
	// xmlFileName = "";
	// rootItem = nullptr;

	// QMap<QString, QString> root_parent_data;
	// root_parent_data["id"] = global_root_parent_id;
	// root_parent_data["name"] = "";
	// auto _root_item_parent = boost::intrusive_ptr<TreeItem>(new
	// TreeItem(nullptr, root_parent_data));
	full_fields_map data;

	// Определяется одно поле в корневом объекте
	// то есть на экране будет один столбец
	boost::fusion::at_key<id_key>(data) = global_root_id;
	boost::fusion::at_key<name_key>(data) = "";
	// Создание корневого Item объекта
	if (_root_item)
		_root_item.reset();
	_root_item = i_t::dangle_instance(data); // boost::intrusive_ptr<TreeItem>(new
						 // TreeItem(_root_item_parent,
						 // root_data));		// ?

	// assert(_root_item->linker()->host().get() == _root_item.get());
	// assert(_root_item->linker()->host_parent().get() != _root_item.get());
	//	_xml_file_path = gl_paras->root_path() + "/" +
	//QDir(appconfig->data_dir()).dirName() + "/" + index_xml_file_name_;
	init_from_xml(); // _know_branch->intercept(know_root_holder::know_root()->root_item());
			 // // init_from_xml(xml);  //
	//	int all_count = count_records_all();
	while (_xml_tree->dom_model()->isNull()) { //(all_count <= 0){
		//		assert(trashmonitoring.is_inited());
		//		auto target_file = std::make_shared<QFileInfo>(_xml_file_path);
		//		if(trashmonitoring.recover_from_trash(target_file, true)){
		//			////	throw std::runtime_error("database load
		//failure");

		init_from_xml(true);
		//			all_count = count_records_all();
		//		}else{
		//			AppConfigDialog appconfigdialog("pageMain"); //
		//globalparameters.main_window()->vtab_record()->activated_browser()->record_screen()->record_ctrl()
		//			appconfigdialog.show();
		//			//// if(all_count <= 0)
		//			// throw std::runtime_error("database load failure");
		//		}
	}
	synchronized(true);
}

tkm_t::tkm_t(boost::intrusive_ptr<i_t> root_item, tv_t* parent)
    : tm_t(root_item, parent)
    , _xml_file_path(gl_paras->root_path() + "/" + QDir(appconfig->data_dir()).dirName() + "/" + gl_para::_index_xml_file_name)
    , _xml_tree(std::make_shared<XmlTree>())
{
} // intercept(_root_item);

// Деструктор Item модели.
// По-хорошему деструктор перед удалением корневого элемента должен пробежать по
// дереву элементов и удалить их
tkm_t::~tkm_t()
{
} // delete rootItem;

std::shared_ptr<XmlTree> tkm_t::init_from_xml(bool force_recover)
{
	auto init = [&](QDomDocument* dom_model) -> int {
		// Проверка формата XML-файла
		if (!format_check(dom_model->documentElement().firstChildElement("format"))) {
			critical_error(tr(std::string("Unsupported format version for data base.\nPlease upgrade your " + program_title_string + " application.").c_str()));

			return 1;
		}
		assert(_root_item);
		// QMap<QString, QString> root_data;
		//// Определяется одно поле в корневом объекте
		//// то есть на экране будет один столбец
		// root_data["id"] = global_root_id;
		// root_data["name"] = "";
		if (gl_paras->main_window())
			gl_paras->main_window()->setDisabled(true);
		beginResetModel();

		//// Создание корневого Item объекта
		// if(_root_item) _root_item.reset();

		// _root_item = boost::intrusive_ptr<TreeItem>(new TreeItem(root_data,
		// nullptr)); // ?
		//// , std::make_shared<RecordTable>(QDomElement())

		//// Динамическое создание дерева из Item объектов на основе DOM модели
		// setup_modeldata(dom_model, _root_item);
		assert(dom_model);
		QDomElement dom_content_root_as_record = dom_model->documentElement().firstChildElement("content").firstChildElement("record"); // "node"
		if (dom_content_root_as_record.isNull()) {
			qDebug() << "Unable load xml tree, first content node not found.";

			return 1;
		}
		// assert(_root_item->linker()->host().get() == _root_item.get());
		// assert(_root_item->linker()->host_parent().get() != _root_item.get());
		_root_item->dom_to_records(dom_content_root_as_record);

		endResetModel();
		if (gl_paras->main_window())
			gl_paras->main_window()->setEnabled(true);
		// save(); // temporary
		return 0;
	};
	//	QString _xml_file_path;
	auto target_file = std::make_shared<QFileInfo>(_xml_file_path);

	bool succeeded = false;
	if (!QFile::exists(_xml_file_path) || 0 >= filesize(_xml_file_path.toStdString().c_str()) || force_recover) {
		//		bool existance = QFile::exists(_xml_file_path);

		// AppConfigDialog appconfigdialog(nullptr	//
		// globalparameters.main_window()->vtab_record()->activated_browser()->record_screen()->record_ctrl()
		// , "pageMain"	// "pageRecordTable"
		// );
		////	appconfigdialog.changePage("pageMain");
		// appconfigdialog.show();

		assert(trashmonitoring.is_inited());
		if (!trashmonitoring.recover_from_trash(target_file, force_recover)) {
			// _file_name		=;
			//			_xml_file_path = _xml_file_path;
			//		}else{
			AppConfigDialog appconfigdialog("pageMain"); // globalparameters.main_window()->vtab_record()->activated_browser()->record_screen()->record_ctrl()
			appconfigdialog.show();
			// _file_name =;
			_xml_file_path = gl_paras->root_path() + "/" + QDir(appconfig->data_dir()).dirName() + "/" + gl_para::_index_xml_file_name;
		}
	}
	if (_xml_tree->dom_model()->isNull() || force_recover) {
		if (!_xml_tree->dom_model()->isNull())
			_xml_tree->dom_model()->clear();
		succeeded = _xml_tree->load(_xml_file_path);
	}
	//	_xml_file_path = _xml_file_path;
	// Загрузка файла и преобразование его в DOM модель
	//	std::shared_ptr<XmlTree> _xml_tree = std::make_shared<XmlTree>();
	if (!succeeded)
		init_from_xml(true);
	else
		assert(0 == init(_xml_tree->dom_model())); // return xmlt;

	return _xml_tree;
}

// std::shared_ptr<XmlTree> tkm_t::init_from_xml(std::shared_ptr<XmlTree> xmlt){
//	init(xmlt->dom_model());

//	return xmlt;
//}

//// Разбор DOM модели и преобразование ее в Item модель
// void TreeKnowModel::setup_modeldata(QDomDocument *dommodel,
// boost::intrusive_ptr<TreeItem> self)
// {
// assert(dommodel);
// QDomElement content_root_record =
// dommodel->documentElement().firstChildElement("content").firstChildElement("record");
// // "node"

// if(content_root_record.isNull()) {
// qDebug() << "Unable load xml tree, first content node not found.";
// return;
// }

// dom_to_records(content_root_record, self);

// return;
// }

//// Рекурсивный обход DOM дерева и извлечение из него узлов
// void KnowModel::dom_to_records(QDomElement _record_dom_element,
// boost::intrusive_ptr<TreeItem> _parent_item)
// {
// assert(_parent_item->up_linker()->host().get() == _parent_item.get());
// assert(_parent_item->up_linker()->host_parent().get() != _parent_item.get());

// auto // std::function<void(const QDomElement &,
// boost::intrusive_ptr<TreeItem>)>
// assembly_record_and_table_to_parent = [&](const QDomElement & _dom_record,
// boost::intrusive_ptr<TreeItem> current_parent)->void {

//// Определяются атрибуты узла дерева разделов
// QDomNamedNodeMap attribute_map = _dom_record.attributes();

////        // Перебираются атрибуты узла дерева разделов
////        for(int i = 0; i < attribute_map.count(); ++i) {
////            QDomNode attribute = attribute_map.item(i);

////            QString name = attribute.nodeName();
////            QString value = attribute.nodeValue();

////            if(name == "id") {
////                id = value;
////                break;
////            }

////            //                        // В дерево разделов устанавливаются
///считанные атрибуты
////            //                        //
///parent->child(parent->current_count() - 1)
////            //                        parent->field(name , value);
////            //                        //
///parent->child(parent->child_count() - 1)->record_to_item(); // temporary
////        }

// QString id = attribute_map.namedItem("id").nodeValue();

// assert(id != "");

// int index = current_parent->sibling_order([&](boost::intrusive_ptr<const
// Linker> it) {return it->host()->id() == id;}); // int index =
// current_parent->item_direct(id)->sibling_order();

// boost::intrusive_ptr<TreeItem> child_item(nullptr);

// if(index != -1)
// {
// child_item = current_parent->item_direct(index)->host();
// } else {
// child_item = current_parent->child_add_new(current_parent->count_direct(),
// id, "");
////            child_item->field("id", id);
// }

// child_item->dom_to_records(_dom_record);   //
// item->self_equipment_from_dom(record);

////        QDomElement recordtable = record.firstChildElement();
////        assert(recordtable.tagName() == "recordtable" ||
///recordtable.isNull());

////        if(recordtable.tagName() == "recordtable") {
////            //                    auto record =
///recordtable.firstChildElement();
////            //                    assert(record.tagName() == "record" ||
///record.isNull());
////            //                    if(record.tagName() ==
///"record")model_from_dom(record, parent);    // get recordtable == ItemsFlat
////            model_from_dom(recordtable, item);
////        }
// };
////    boost::intrusive_ptr<TreeItem> parent = iParent;

// if(!_record_dom_element.isNull()) {
// assert(_record_dom_element.tagName() != "recordtable");
// assert(_record_dom_element.tagName() == "record");

// assert(_parent_item->up_linker()->host().get() == _parent_item.get());
// assert(_parent_item->up_linker()->host_parent().get() != _parent_item.get());
//// У данного Dom-элемента ищется таблица конечных записей
//// и данные заполняются в Item-таблицу конечных записей
//// At this Dom-end table element is searched for records and the data filled
///in the Item-end table entries
// _parent_item->dom_to_direct(_record_dom_element);    // take ground from the
// nearest level children
//// up line duplicated with
///below!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// assert(_parent_item->up_linker()->host().get() == _parent_item.get());
// assert(_parent_item->up_linker()->host_parent().get() != _parent_item.get());

// QDomElement _dom_record_table = _record_dom_element.firstChildElement();

// assert(_dom_record_table.tagName() == "recordtable" ||
// _dom_record_table.isNull());

// if(_dom_record_table.tagName() == "recordtable") {

// QDomElement _dom_record = _dom_record_table.firstChildElement();
// assert(_dom_record.tagName() == "record");

// assert(_parent_item->up_linker()->host().get() == _parent_item.get());
// assert(_parent_item->up_linker()->host_parent().get() != _parent_item.get());

//// Пробегаются все DOM элементы текущего уровня
//// и рекурсивно вызывается обработка подуровней
// while(!_dom_record.isNull()) { // traverse brothers

////            if(_dom_element.tagName() == "node") { // "node"
////                assert(_dom_element.firstChildElement().tagName() ==
///"record");
////                QDomElement record =
///_dom_element.firstChildElement("record");

////                if(!record.isNull()) {

////                    //                    //                // Обнаруженый
///подузел прикрепляется к текущему элементу
////                    //                    //                auto item =
///parent->add_child();  // insert_children(parent->current_count(), 1, 1);

////                    //                    //
///if(!_dom_element.firstChildElement("record").isNull())
////                    //                    //
///boost::static_pointer_cast<Record>(item)->record_from_dom(_dom_element.firstChildElement("record"));

////                    //                    //                //
///QString line1, line_name, line_id;
////                    //                    //                //
///line1 = n.tagName();
////                    //                    //                //
///line_name = n.attribute("name");
////                    //                    //                //
///line_id = n.attribute("id");
////                    //                    //                //
///qDebug() << "Read node " << line1 << " " << line_id << " " << line_name;

////                    //                    QString id = "";

////                    //                    // Определяются атрибуты узла
///дерева разделов
////                    //                    QDomNamedNodeMap attributeMap =
///record.attributes();

////                    //                    // Перебираются атрибуты узла
///дерева разделов
////                    //                    for(int i = 0; i <
///attributeMap.count(); ++i) {
////                    //                        QDomNode attribute =
///attributeMap.item(i);

////                    //                        QString name =
///attribute.nodeName();
////                    //                        QString value =
///attribute.nodeValue();

////                    //                        if(name == "id") {
////                    //                            id = value;
////                    //                            break;
////                    //                        }

////                    //                        //                    // В
///дерево разделов устанавливаются считанные атрибуты
////                    //                        //                    //
///parent->child(parent->current_count() - 1)
////                    //                        // item->field(name , value);
////                    //                        //                    //
///parent->child(parent->child_count() - 1)->record_to_item(); // temporary
////                    //                    }

////                    //                    assert(id != "");
////                    //                    int index =
///parent->is_item_exists(id);

////                    //                    boost::intrusive_ptr<TreeItem>
///item = nullptr;

////                    //                    if(index != -1) {
////                    //                        item = parent->child(index);

////                    //                        item->record_from_dom(record);
////                    //                    } else {
////                    //                        item = parent->add_child();
////                    //                        item->record_from_dom(record);
////                    //                    }

////                    //                    // Вызов перебора оставшегося DOM
///дерева с прикреплением обнаруженных объектов
////                    //                    // к только что созданному
///элементу
////                    //
///model_from_dom(record.firstChildElement(), item);    //
////                    record_and_recordtable(record);
////                }

////            } else

// if(_dom_record.tagName() == "record") {

////                if(!_dom_element.isNull()) {
////                    //
///boost::static_pointer_cast<Record>(parent)->record_from_dom(_dom_element);
////                    //                    boost::intrusive_ptr<TreeItem>
///item = boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString, QString>(),
///parent));
////                    // item->record_from_dom(_dom_element);
////                    //
///parent->find(item->id())->record_from_dom(_dom_element);

////                    QString id = "";
////                    // Определяются атрибуты узла дерева разделов
////                    QDomNamedNodeMap attributeMap =
///_dom_element.attributes();

////                    // Перебираются атрибуты узла дерева разделов
////                    for(int i = 0; i < attributeMap.count(); ++i) {
////                        QDomNode attribute = attributeMap.item(i);

////                        QString name = attribute.nodeName();
////                        QString value = attribute.nodeValue();

////                        if(name == "id") {
////                            id = value;
////                            break;
////                        }

////                        //                        // В дерево разделов
///устанавливаются считанные атрибуты
////                        //                        //
///parent->child(parent->current_count() - 1)
////                        //                        parent->field(name ,
///value);
////                        //                        //
///parent->child(parent->child_count() - 1)->record_to_item(); // temporary
////                    }

////                    assert(id != "");
////                    int index = parent->is_item_exists(id);
////                    boost::intrusive_ptr<TreeItem> item = nullptr;

////                    if(index != -1) {
////                        item = parent->child(index);
////                        item->record_from_dom(_dom_element);
////                    } else {
////                        item = parent->add_child();
////                        item->record_from_dom(_dom_element);
////                    }

////                    QDomElement recordtable =
///_dom_element.firstChildElement();
////                    assert(recordtable.tagName() == "recordtable" ||
///recordtable.isNull());

////                    if(recordtable.tagName() == "recordtable") {
////                        //                    auto record =
///recordtable.firstChildElement();
////                        //                    assert(record.tagName() ==
///"record" || record.isNull());

////                        //                    if(record.tagName() ==
///"record")model_from_dom(record, parent);    // get recordtable == ItemsFlat
////                        model_from_dom(recordtable, item);
////                    }

//// model_from_dom(_child, self); //
// assert(_parent_item->up_linker()->host().get() == _parent_item.get());
// assert(_parent_item->up_linker()->host_parent().get() != _parent_item.get());
// assembly_record_and_table_to_parent(_dom_record, _parent_item);
////            }

// }

////                else {

////                    assert(_child.tagName() == "recordtable");

////                    //
///if(!_dom_element.isNull())static_cast<ItemsFlat
///*>(parent.get())->items_from_dom(_dom_element, parent->parent());

////                    QDomElement record = _child.firstChildElement();
////                    assert(record.tagName() == "record" || record.isNull());

////                    if(record.tagName() == "record") {
////                        assembly_record_and_table_to_parent(record, self);
///// model_from_dom(record, parent);    // get recordtable == ItemsFlat
////                    }
////                }

// _dom_record = _dom_record.nextSiblingElement();   // brother record
// assert(_dom_record.tagName() == "record" || _dom_record.isNull());
// }
// }
// }

// }

bool tkm_t::format_check(QDomElement elementFormat)
{
	int baseVersion = 0;
	int baseSubVersion = 0;
	// Если DOM-элемент с версией и подверсией существует
	if (!elementFormat.isNull()) {
		baseVersion = elementFormat.attribute("version", "0")
				  .toInt(); // Считывается номер версии
		baseSubVersion = elementFormat.attribute("subversion", "0")
				     .toInt(); // Считывается номер подверсии
	}
	// Если номер версии или подверсии выше чем поддерживаемые программой
	if (baseVersion > CURRENT_FORMAT_VERSION || baseSubVersion > CURRENT_FORMAT_SUBVERSION)
		return false;
	// В настоящий момент поддерживается формат 1.2
	// В настоящий момент предполагается, что номер версии всегда 1, поэтому вся
	// работа идет по номеру подверсии
	if (baseSubVersion <= 1)
		if (update_sub_version_from_1_to_2() == false) // Смена формата с 1.1 на 1.2
			return false;
	// На будущее, для перехода с подверии 2 на подверсию 3, эти строки надо
	// добавлять к существующим (а не заменять)
	// if(baseSubVersion<=2)
	// if(updateSubVersionFrom2To3()==false)
	// return false;

	return true;
}

bool tkm_t::update_sub_version_from_1_to_2(void)
{
	return true;
}

void tkm_t::reload(void)
{
	gl_paras->main_window()->setDisabled(true);
	//	if(_xml_file_path == gl_para::_index_xml_file_name) _xml_file_path =
	//gl_paras->root_path() + "/" + QDir(appconfig->data_dir()).dirName() + "/" +
	//gl_para::_index_xml_file_name;
	init_from_xml();
	gl_paras->main_window()->setEnabled(true);
}


// Запись всех данных в XML файл
void tkm_t::save()
{
	std::mutex m;
	std::lock_guard<std::mutex> lock(m);
	auto check_integrity = [&](const QString& fake_path) {
		QFile xmlFile(fake_path); //_xml_file_path
		if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qDebug() << "Cant open file " << _xml_file_path << " for read.";
			exit(1);
		}
		QDomDocument doc((std::string(gl_para::_program_instance_name) + "doc").c_str());
		bool result = false;
		QString errorStr;
		int errorLine;
		int errorColumn;
		if (!doc.setContent(&xmlFile, true, &errorStr, &errorLine, &errorColumn)) {
			QMessageBox::information(
			    0, tr("Error converting to DOM"), tr("Parse error at line %1, column %2:\n%3\nIt\'s a good time for you to correct it manually when leave the warning open :)").arg(errorLine).arg(errorColumn).arg(errorStr));

			//		result = false;
			exit(1);
		} else
			result = true;
		return result;
	};
#ifdef _with_record_table
	record_to_item();
#endif
	// Если имя файла небыло проинициализировано
	if (_xml_file_path == "")
		critical_error(tr("In KnowTreeModel can't set file name for XML file"));
	// Коструирование DOM документа для записи в файл
	QDomDocument doc(
	    (std::string(gl_para::_program_instance_name) + "doc").c_str());

	// Установка заголовка
	doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

	// Создание корневого элемента
	QDomElement rootelement = doc.createElement("root");

	// Добавление формата версии к корневому элементу
	QDomElement formvers = doc.createElement("format");
	formvers.setAttribute("version", CURRENT_FORMAT_VERSION);
	formvers.setAttribute("subversion", CURRENT_FORMAT_SUBVERSION);
	rootelement.appendChild(formvers);

	QDomElement content = doc.createElement("content");

	// Получение полного DOM дерева хранимых данных
	QDomElement elmdomtree = _root_item->dom_from_treeitem();

	content.appendChild(elmdomtree);
	if (content.hasChildNodes()) {
		// Добавление полного дерева DOM хранимых данных к корневому элементу
		rootelement.appendChild(content); //
		// elmdomtree

		// Добавление корневого элемента в DOM документ
		doc.appendChild(rootelement);
		if (doc.hasChildNodes()) {
			auto write_to = [&](const QString& file_name_, bool backup_ = true) {
				// аспечатка на экран, что будет выводиться в XML файл
				// qDebug() << "Doc document for write " << doc.toString();
				// Перенос текущего файла дерева в корзину
				if (0 < filesize(file_name_.toStdString().c_str()) && backup_)
					DiskHelper::backup(file_name_);
				// Запись DOM данных в файл
				QSaveFile xmlFile(file_name_);
				if (!xmlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
					qDebug() << "Cant open file " << file_name_ << " for write.";
					return; //exit(1);
				}
				QTextStream out(&xmlFile);
				out.setCodec("UTF-8");
				auto str = doc.toString();
				if (str != "") {

					out << str;
				}
				xmlFile.commit();
			};
			do
				write_to(_xml_file_path + ".fake", false);
			while (!check_integrity(_xml_file_path + ".fake"));
			write_to(_xml_file_path);
		}
	}
}

//// Генерирование полного DOM дерева хранимых данных
// QDomElement KnowModel::dom_from_treeitem(boost::intrusive_ptr<TreeItem> root)
// // full modeldata to dom
// {
// std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
////    QDomElement elm = doc->createElement("content");

//// qDebug() << "New element for export" << xmlNodeToString(elm);
// QDomElement records_all_in_root = root->dom_from_treeitem(doc);
////    dom_from_record(doc, elm, root);

//// qDebug() << "In export_fullmodeldata_to_dom stop element " <<
///xmlNodeToString(elm);
////    elm.appendChild(record);

// return records_all_in_root;  // elm;
// }

// Recursive conversion Item-elements in the Dom tree   // Рекурсивное
// преобразование Item-элементов в Dom дерево
void tkm_t::dom_from_treeitem(std::shared_ptr<QDomDocument> doc, QDomElement& xml_data, boost::intrusive_ptr<i_t> curr_item)
{
	// Если в ветке присутсвует таблица конечных записей
	// В первую очередь добавляется она
	// If there is a branch in the final table of records // First of all, she
	// added
	// if(curr_item->count_direct() > 0) {
	// Обработка таблицы конечных записей

	// Получение Dom дерева таблицы конечных записей
	// В метод передается QDomDocument, на основе кторого будут создаваться
	// элементы
	QDomElement item_flat_dom = curr_item->dom_from_treeitem_impl(doc); //
	// Dom дерево таблицы конечных записей добавляется
	// как подчиненный элемент к текущему элементу
	if (!item_flat_dom.isNull())
		xml_data.appendChild(item_flat_dom); // .cloneNode()
						     // }

	// I don't think these codes is needed:

	////    // Обработка каждой подчиненной ветки
	////    int i;

	// for(int i = 0; i < curr_item->count_direct(); i++) {
	////        assert(curr_item->child(i).get() != curr_item);

	// if(curr_item->item_direct(i)->host() != curr_item) {
	////            // Временный элемент, куда будет внесена текущая перебираемая
	///ветка
	////            QDomElement  tempElement = doc->createElement("node");

	////            // Получение всех полей для данной ветки
	////            QMap<QString, QString> fields =
	///curr_item->child(i)->all_fields_direct();

	////            // Перебираются поля элемента ветки
	////            QMapIterator<QString, QString> fields_iterator(fields);

	////            while(fields_iterator.hasNext()) {
	////                fields_iterator.next();

	////                // Установка для временного элемента значения
	///перебираемого поля как атрибута
	////                tempElement.setAttribute(fields_iterator.key(),
	///fields_iterator.value());
	////            }

	////            //
	///tempElement.appendChild(curr_item->export_local_to_dom(doc));

	////            // Добавление временного элемента к основному документу
	////            xml_data.appendChild(tempElement);

	////            // qDebug() << "In parsetreetodom() current construct doc " <<
	///xmlNodeToString(*xmldata);

	//// Рекурсивная обработка
	// QDomElement workElement = xml_data.lastChildElement();
	// dom_from_treeitem(doc,
	// workElement, curr_item->item_direct(i)->host());
	// }
	// }
}

//// Добавление новой ветки после указанной ветки
// boost::intrusive_ptr<TreeItem> KnowModel::model_new_sibling(const QModelIndex
// &_index, QString id, QString name)
// {
//// Получение ссылки на родительский Item элемент по QModelIndex
// boost::intrusive_ptr<TreeItem> current = item(_index);
// boost::intrusive_ptr<TreeItem> parent = current->parent();
// assert(parent);
// boost::intrusive_ptr<TreeItem> _result(nullptr);

// if(parent) {
////        beginInsertRows(_index.parent()
////                        , _index  // parent->count_direct()
////                        , _index  // parent->count_direct()
////                       );
// _result = model_new_child(TreeIndex(parent, _index.row()), id, name);
////        endInsertRows();
// }

// return _result;
// }

//// Добавление новой подветки к указанной ветке
// boost::intrusive_ptr<TreeItem> KnowModel::model_new_child(const QModelIndex
// &_index, QString id, QString name)
// {
//// Получение ссылки на Item элемент по QModelIndex
// boost::intrusive_ptr<TreeItem> parent = item(_index);
// boost::intrusive_ptr<TreeItem> _result(nullptr);
////    beginInsertRows(_index
////                    , _index.row()  // parent->count_direct()
////                    , _index.row()  // parent->count_direct()
////                   );
// _result = model_new_child(TreeIndex(parent
// , 0 // _index.row() =>this row number is semantically wrong!!!
// ), id, name);
////    endInsertRows();
// return _result;
// }

//// Добавление новой подветки к Item элементу
// boost::intrusive_ptr<TreeItem>
// KnowModel::model_new_child(boost::intrusive_ptr<TreeItem> parent, int pos,
// QString id, QString name)
// {
// beginInsertRows(index(parent)
// , pos   // parent->count_direct()
// , pos   // parent->count_direct()
// );

// QMap<QString, QString> data;
// data["id"]      = id;
// data["name"]    = name;
// data["url"]     = web::Browser::_defaulthome;
//// Подузел прикрепляется к указанному элементу
//// в конец списка подчиненных элементов
// boost::intrusive_ptr<TreeItem> current = new TreeItem(parent, data,
// QDomElement(), pos); // parent->child_add_new(pos, id, name);

////    //    // Определяется ссылка на только что созданную ветку
////    //    boost::intrusive_ptr<TreeItem> current =
///parent->child(parent->child_count() - 1);

////    // Устанавливается идентификатор узла
////    current->field("id", id);

////    // Устанавливается навание узла
////    current->field("name", name);

////    //    // Инициализируется таблица конечных записей
////    //    current->tabledata(std::make_shared<RecordTable>(current));

//// Определяется, является ли родительская ветка зашифрованной
// if(parent->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
//// Новая ветка превращается в зашифрованную
// current->to_encrypt();
// }

// endInsertRows();
// return current;
// }

// Добавление новой подветки к Item элементу
boost::intrusive_ptr<i_t>
tkm_t::new_child(boost::intrusive_ptr<TreeIndex> _modelindex, QString id, QString name)
{
	boost::intrusive_ptr<i_t> parent = _modelindex->host();
	int pos = _modelindex->sibling_order();
	auto index_parent = index(parent);
	beginInsertRows(index_parent, pos // parent->count_direct()
	    ,
	    pos // parent->count_direct()
	    );

	full_fields_map data;
	boost::fusion::at_key<id_key>(data) = id;
	boost::fusion::at_key<name_key>(data) = name;
	boost::fusion::at_key<url_key>(data) = web::Browser::_defaulthome;
	// Подузел прикрепляется к указанному элементу
	// в конец списка подчиненных элементов
	boost::intrusive_ptr<i_t> current = new i_t(parent, data, QDomElement(),
	    pos); // parent->child_add_new(pos, id, name);
	// auto				view	= static_cast<tv_t *>(static_cast<QObject
	// *>(this)->parent());
	// update_index(index_parent);
	// view->update(index_parent);
	////    // Определяется ссылка на только что созданную ветку
	////    boost::intrusive_ptr<TreeItem> current =
	///parent->child(parent->child_count() - 1);
	//// Устанавливается идентификатор узла
	// current->field("id", id);
	//// Устанавливается навание узла
	// current->field("name", name);
	////    // Инициализируется таблица конечных записей
	////    current->tabledata(std::make_shared<RecordTable>(current));
	// Определяется, является ли родительская ветка зашифрованной
	if (parent->field<crypt_key>() == crypt_value(true)) {
		// Новая ветка превращается в зашифрованную
		current->to_encrypt();
	}
	endInsertRows();

	return current;
}

//// Добавление новой ветки после указанной ветки
// boost::intrusive_ptr<TreeItem> KnowModel::model_move_as_sibling(const
// QModelIndex &_index, boost::intrusive_ptr<TreeItem> _source_item)
// {
//// Получение ссылки на родительский Item элемент по QModelIndex
// auto current_item       = item(_index);
// auto current_parent    = item(_index.parent());  // current->parent();
// auto current_parent_    = current_item->parent();
// assert(current_parent == current_parent_);
// assert(current_parent->id() != "");

// bool current_parent_valid = [&]() {
// if(!item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() ==
// current_parent->id();}) && current_parent->id() != "") {
// intercept_self(current_parent);
// }
// return (index(current_parent).isValid())
// && (item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id() ==
// current_parent->id();}));
// }();

// assert(current_parent_valid);
////
///assert(globalparameters.tree_screen()->know_model_board()->item_by_id(current->id()));
////
///assert(globalparameters.tree_screen()->know_model_board()->item_by_id(parent->id()));
////    assert(item_by_id(current->id()));
////    assert(item_by_id(parent->id()));
////    assert(is_id_exists(current->id()));
// assert(item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id()
// == current_parent->id();}));    // what if parent_id is _know_model_board but
// current model is not?
// boost::intrusive_ptr<TreeItem> _result(nullptr);
// assert(current_parent != _source_item);

// if(current_parent_valid) {
////        beginInsertRows(_index.parent()
////                        , _index  // parent->count_direct()
////                        , _index  // parent->count_direct()
////                       );
// _result = model_move_as_child_impl(current_parent, _source_item,
// _index.row());
////        endInsertRows();
// }

// assert(item([ = ](boost::intrusive_ptr<const TreeItem> it) {return it->id()
// == _result->id();}));
// return _result;
// }

//// Добавление новой подветки к указанной ветке
// boost::intrusive_ptr<TreeItem> KnowModel::model_move_as_child(const
// QModelIndex &_index, boost::intrusive_ptr<TreeItem> _source_item)
// {
//// Получение ссылки на Item элемент по QModelIndex
// boost::intrusive_ptr<TreeItem> current = item(_index);
// boost::intrusive_ptr<TreeItem> _result(nullptr);

////    beginInsertRows(_index
////                    , _index.row()  // parent->count_direct()
////                    , _index.row()  // parent->count_direct()
////                   );
// _result = model_move_as_child_impl(current, _source_item, 0);  //
// _index.row()

////    endInsertRows();
// return _result;
// }

// boost::intrusive_ptr<TreeItem>
// KnowModel::clipboard_move_as_child_impl(boost::intrusive_ptr<TreeItem>
// _parent, boost::intrusive_ptr<TreeItem> _source_item, int _pos, int _mode)
// {

////    auto item_from_model = item([&](boost::intrusive_ptr<const TreeItem>
///it)->bool{return it->id() == _source_item->id();});  // clipboard
///implementation have no model

////    if(item_from_model) {
////        _source_item = item_from_model;
////    }

// boost::intrusive_ptr<TreeItem> result(_source_item);

// auto _index_parent = index(_parent);

// auto _index_origin = index(_source_item);

// assert(!_index_origin.isValid());

// _parent->child_remove(_source_item);

// assert(!_index_parent.isValid());

// result = _parent->child_insert(_pos, result, _mode);

// if(_parent->field(boost::mpl::c_str < crypt_type > ::value) == "1" && result)
// {
//// Новая ветка превращается в зашифрованную
// result->to_encrypt();
// }

////    assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() ==
///_parent->id();}));

////    if(result)
////        assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return
///it->id() == result->id();})); // may be parent is not exist?

////    // Подузел прикрепляется к указанному элементу
////    // в конец списка подчиненных элементов
////    //    boost::intrusive_ptr<TreeItem> current =

// return result;  // return model_move_as_child_impl(_parent, _source, _pos,
// _mode);
// }

// Add a new highlight to the Item element  // Добавление новой подветки к Item
// элементу
boost::intrusive_ptr<i_t>
tkm_t::move(boost::intrusive_ptr<TreeLevel> _tree_level, int mode)
{ // , int _pos
	// , QString id, QString name

	boost::intrusive_ptr<TreeIndex> _tree_index = _tree_level->tree_index(); // boost::intrusive_ptr<TreeItem> _parent
	boost::intrusive_ptr<i_t> _to_be_operated = _tree_level->to_be_operated();
	boost::intrusive_ptr<i_t> to_be_host = _tree_index->host();
	//
	auto move_impl = [&](const int pos, const int mode) -> boost::intrusive_ptr<i_t> {
		// auto	_parent = _tree_index->host_parent();
		auto _parent = _tree_index->host();
		if (_parent) {
			auto _linker = _to_be_operated->linker(); // _parent->linker();
			if (!_linker) {
				_to_be_operated->linker(boost::intrusive_ptr<Linker>(new Linker(_parent, _to_be_operated))); // , pos, mode
				_linker = _to_be_operated->linker();
				auto link_result = _linker->parent(_parent, pos, mode);
				assert(link_result == _linker);
				assert(_linker->integrity_external(_to_be_operated, _parent));
			} else if (_linker && _linker->host_parent() != _parent) {
				auto old_parent = _linker->host_parent();
				if (old_parent && old_parent != _parent)
					old_parent->release([&](boost::intrusive_ptr<const Linker> il) {
						return il->host()->id() == _to_be_operated->id() && il == _linker;
					});
				if (_linker->host() != _to_be_operated)
					_linker->host(std::forward<boost::intrusive_ptr<i_t>>(_to_be_operated)); // std::move(boost::intrusive_ptr<TreeItem>(this))
				auto link_result = _linker->parent(_parent, pos, mode);                          // _linker->host_parent()->release(this->linker());
				assert(link_result == _linker);
				assert(_linker->integrity_external(_to_be_operated, _parent));
			}
		} else
			throw std::runtime_error("target host is nullptr");
		// auto current_model_ = _tree_index->current_model();
		// if(!
		// static_cast<QModelIndex>(current_model_()->index(_to_be_operated)).isValid())throw
		// std::runtime_error("move_child index invalid");
		// assert(static_cast<QModelIndex>(current_model_()->index(_to_be_operated)).isValid());
		return _to_be_operated;
	};
	assert(to_be_host);
	int pos = _tree_index->sibling_order();

	// auto _to_be_operated(_to_be_operated);		// 1-1
	auto index_operated = index(_to_be_operated);
	auto operate_parent = _to_be_operated->parent();
	// assert(original_parent);
	auto index_old_parent = operate_parent ? index(operate_parent) : index_tree();
	if (!(operate_parent == to_be_host && to_be_host->contains_direct(_to_be_operated->linker()))) {
		auto _index_new_parent = index(to_be_host);
		////        auto _index_origin = index(source_item);
		// auto view = static_cast<tv_t *>(static_cast<QObject *>(this)->parent());
		////        if(source_item->parent() != host){  // why do this?
		///original_parent->delete_permanent
		////            boost::intrusive_ptr<TreeItem> deleted_item;
		////                //            bool remove_result = false;
		////            if(_index_origin.isValid()){
		////                // auto original_parent = source_item->parent();
		////                int sibling_order = - 1;
		////                if(original_parent){
		////                    sibling_order =
		///original_parent->sibling_order([&](boost::intrusive_ptr<const Linker>
		///il){return il == source_item->linker() && il->host() == source_item &&
		///source_item->parent() == il->host_parent();});
		////                    assert(_index_origin.row() == sibling_order);
		////                    beginRemoveRows(_index_parent, _index_origin.row(),
		///_index_origin.row());
		////                    deleted_item =
		///original_parent->delete_permanent([&](boost::intrusive_ptr<const Linker>
		///il){
		////                                return il == source_item->linker() &&
		///il->host() == source_item && source_item->parent() == original_parent &&
		///source_item->parent() == il->host_parent();
		////                            });																																			//
		///model_remove(_source_item->up_linker());
		////                    if(_index_original_parent.isValid()){
		////                        update_index(_index_origin.parent());							//
		///emit_datachanged_signal(_index_origin.parent());
		////                        view->update(_index_origin.parent());
		////                        emit
		///layoutChanged(QList<QPersistentModelIndex>() << _index_original_parent);
		////                    }
		////                    endRemoveRows();
		////                }					// else sibling_order =
		///_index_origin.row();
		////            }
		////                //            else
		////                //                remove_result = true;
		////                //            else if(_source_item->parent()) {   //
		///should not use
		////                //                //
		///_source_item->self_remove_from_parent();    //
		////                //                deleted_linker =
		///_source_item->parent()->remove(_source_item->up_linker()); // -> this
		///comment content is not must a logic error  1-2?
		////                //            }
		////                //            assert(!deleted_linker);  // means always
		///does not work!? no, eg. _source_item->parent() == nullptr, so some time
		///it is nullptr, not always
		//////            assert(!deleted_item);
		//////            assert(!deleted_item->linker());
		////        }
		if (static_cast<QModelIndex>(_index_new_parent).isValid() || to_be_host == _root_item) {
			beginInsertRows(_index_new_parent, pos, // parent->count_direct()
			    pos                                 // (pos + 1 < parent->count_direct()) ? pos + 1 :
			    // parent->count_direct()
			    );

			_to_be_operated = move_impl(pos, mode); // source_item->parent(host, pos, mode)->host();
								// // add_new_branch(parent, id, name);  //
								// parent->count_direct()
			endInsertRows();
			if (_to_be_operated && child([&](boost::intrusive_ptr<const i_t> it) -> bool {
				    return it->id() == _to_be_operated->id();
			    })) {
				emit_datachanged_signal(
				    index(to_be_host->sibling_order([&](boost::intrusive_ptr<const Linker> il) {
					    return il->host()->id() == _to_be_operated->id() && _to_be_operated->linker() == il && il->host_parent() == _to_be_operated->parent();
				    }),
					0, _index_new_parent));
			}
			// update_index(_index_parent);
			// view->update(_index_parent);

			emit layoutChanged(QList<QPersistentModelIndex>() << static_cast<QModelIndex>(_index_new_parent));
			if (static_cast<QModelIndex>(index_old_parent).isValid()) {
				// update_index(index_operate_parent);
				// view->update(index_operate_parent);
				emit layoutChanged(QList<QPersistentModelIndex>() << static_cast<QModelIndex>(index_old_parent));
			} else if (static_cast<QModelIndex>(index_operated).isValid()) {
				// update_index(index_operated);
				// view->update(index_operated);
				emit layoutChanged(QList<QPersistentModelIndex>() << static_cast<QModelIndex>(index_operated));
			}
			////	    if(!
			///static_cast<QModelIndex>(index(result)).isValid())throw
			///std::runtime_error("move_child index invalid");
			// assert(static_cast<QModelIndex>(index(_to_be_operated)).isValid());
			// // maybe merged? not need a real tree member?second one
		} else
			_to_be_operated = move_impl(pos, mode); // should not use?	//
								// source_item->parent(host, pos,
								// mode)->host();
								// // 1-3
		assert(_to_be_operated->linker()->integrity_external(_to_be_operated, to_be_host));
		if (to_be_host->field<crypt_key>() == crypt_value(true) && _to_be_operated) {
			// Новая ветка превращается в зашифрованную
			_to_be_operated->to_encrypt();
		}
		// assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() ==
		// _parent->id();}));

		// if(result)
		// assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() ==
		// result->id();})); // may be parent is not exist?

		//// Подузел прикрепляется к указанному элементу
		//// в конец списка подчиненных элементов
		////    boost::intrusive_ptr<TreeItem> current =
	}
	return _to_be_operated; // parent->add_child(item);

	//// Определяется ссылка на только что созданную ветку
	// boost::intrusive_ptr<TreeItem> current =
	// parent->child(parent->child_count() - 1);

	//// Устанавливается идентификатор узла
	// current->field("id", id);

	//// Устанавливается навание узла
	// current->field("name", name);

	//// Инициализируется таблица конечных записей
	// current->tabledata(std::make_shared<RecordTable>(current));

	//// Определяется, является ли родительская ветка зашифрованной
	// if(parent->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
	//// Новая ветка превращается в зашифрованную
	// current->to_encrypt();
	// }

	// return current;
}

// boost::intrusive_ptr<TreeItem>
// KnowModel::add_new_branch(boost::intrusive_ptr<TreeItem> parent,
// boost::intrusive_ptr<TreeItem> item)
// {
// boost::intrusive_ptr<TreeItem> current;

// if(item->field("url") != web::Browser::_defaulthome) {
//// Подузел прикрепляется к указанному элементу
//// в конец списка подчиненных элементов
// current = parent->add_child(item);

////    // Определяется ссылка на только что созданную ветку
////    boost::intrusive_ptr<TreeItem> current =
///parent->child(parent->child_count() - 1);

////    // Инициализируется таблица конечных записей
////    current->tabledata(std::make_shared<RecordTable>(current));

//// Определяется, является ли родительская ветка зашифрованной
// if(parent->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
//// Новая ветка превращается в зашифрованную
// current->to_encrypt();
// }
// }

// return current;
// }

#ifdef _with_record_table

// Add a new highlight to the Item element  // Добавление новой подветки к Item
// элементу
boost::intrusive_ptr<TreeItem> KnowModel::lock_child_add(
    boost::intrusive_ptr<Record> record,
    boost::intrusive_ptr<TreeItem> parent)
{ // , QString id, QString name
	beginInsertRows(index(parent), parent->current_count(), parent->current_count());
	boost::intrusive_ptr<TreeItem> result = parent->add_child(record); // add_new_branch(parent, id, name);
	endInsertRows();

	// Подузел прикрепляется к указанному элементу
	// в конец списка подчиненных элементов

	// boost::intrusive_ptr<TreeItem> current =
	return result; // parent->add_child(record);

	//// Определяется ссылка на только что созданную ветку
	// boost::intrusive_ptr<TreeItem> current =
	// parent->child(parent->child_count() - 1);

	//// Устанавливается идентификатор узла
	// current->field("id", id);

	//// Устанавливается навание узла
	// current->field("name", name);

	//// Инициализируется таблица конечных записей
	// current->tabledata(std::make_shared<RecordTable>(current));

	//// Определяется, является ли родительская ветка зашифрованной
	// if(parent->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
	//// Новая ветка превращается в зашифрованную
	// current->to_encrypt();
	// }

	// return current;
}

#endif

// QString KnowModel::model_paste_sibling_from_clipboard(const QModelIndex
// &_index, ClipboardBranch *subbranch)
// {
// QString pasted_branch_id;

//// Получение ссылки на родительский Item элемент по QModelIndex
// boost::intrusive_ptr<TreeItem> current = item(_index);
// boost::intrusive_ptr<TreeItem> parent = current->parent();
// assert(parent);

// if(parent) {
// assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() ==
// parent->id();}));
////        beginInsertRows(_index.parent(), parent->count_direct(),
///parent->count_direct());
// pasted_branch_id = paste_children_from_clipboard(parent, (ClipboardBranch
// *)subbranch, current->sibling_order());
////        endInsertRows();
// }

// return pasted_branch_id;
// }

//// Добавление новой подветки к указанной ветке из буфера обмена
// QString KnowModel::model_paste_child_from_clipboard(const QModelIndex
// &_index, ClipboardBranch *subbranch)
// {
// QString pasted_branch_id;

//// Получение ссылки на Item элемент по QModelIndex
// boost::intrusive_ptr<TreeItem> parent = item(_index);
// assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() ==
// parent->id();}));
////    beginInsertRows(_index, parent->count_direct(), parent->count_direct());
// pasted_branch_id = paste_children_from_clipboard(parent, (ClipboardBranch
// *)subbranch, parent->count_direct());
////    endInsertRows();

// return pasted_branch_id;
// }

//// Добавление подветки из буфера обмена относительно указанного элемента
//// Функция возвращает новый идентификатор стартовой добавленной подветки
// boost::intrusive_ptr<TreeItem>
// KnowModel::paste_children_from_clipboard(boost::intrusive_ptr<TreeItem>
// _target_item, ClipboardBranch *_sub_branch, int _sibling_order)
// {
// qDebug() << "In paste_subbranch()";
// boost::intrusive_ptr<TreeItem> _result(nullptr);
////    assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() ==
///_target_item->id();}));  // maybe a dangling pointer, for further operations

// auto clipboard_move_as_child_impl = [](boost::intrusive_ptr<TreeItem>
// _parent, boost::intrusive_ptr<TreeItem> _source_item, int _pos, int _mode =
// ADD_NEW_RECORD_AFTER)->boost::intrusive_ptr<TreeItem> {

////    auto item_from_model = item([&](boost::intrusive_ptr<const TreeItem>
///it)->bool{return it->id() == _source_item->id();});  // clipboard
///implementation have no model

////    if(item_from_model) {
////        _source_item = item_from_model;
////    }

// boost::intrusive_ptr<TreeItem> result(_source_item);

////        auto _index_parent = index(_parent);

////        auto _index_origin = index(_source_item);

////        assert(!_index_origin.isValid());

// _parent->child_remove(_source_item);

////        assert(!_index_parent.isValid());

// result = _parent->child_insert(_pos, result, _mode);

// if(_parent->field(boost::mpl::c_str < crypt_type > ::value) == "1" && result)
// {
//// Новая ветка превращается в зашифрованную
// result->to_encrypt();
// }

////    assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return it->id() ==
///_parent->id();}));

////    if(result)
////        assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return
///it->id() == result->id();})); // may be parent is not exist?

////    // Подузел прикрепляется к указанному элементу
////    // в конец списка подчиненных элементов
////    //    boost::intrusive_ptr<TreeItem> current =

// return result;  // return model_move_as_child_impl(_parent, _source, _pos,
// _mode);
// };

// std::function<QString(boost::intrusive_ptr<TreeItem>, const QString &,
// ClipboardBranch *, const int)>
// child_paste_recursive
// = [&](boost::intrusive_ptr<TreeItem>    _clip_parent
// , const QString                   & _clip_root_items_parent_id
// , ClipboardBranch                 * _sub_branch
// , const int                       _sibling_order
// )->QString {
// QString id = _clip_parent->id();

////        if(_clip_parent->parent()) {
// qDebug() << "In paste_subbranch_recurse()";

//// Выясняются данные конечных записей
// QList< boost::intrusive_ptr<TreeItem> > records =
// _sub_branch->records_by_parent_id(_clip_root_items_parent_id);

// for(int i = 0; i < records.size(); i++)
// //foreach(boost::intrusive_ptr<TreeItem> record, records) {
// {
// auto record = records.at(i);
// qDebug() << "Add table record " + record->field("name");

// if(record && _clip_parent != record) {
//// _new_item
// auto record_shadow = clipboard_move_as_child_impl(_clip_parent, record,
// _sibling_order + i); // _clip_parent->count_direct()
// assert(record_shadow->id() == record->id());
//// _clip_parent->child_move(_clip_parent->count_direct(), record,
///ADD_NEW_RECORD_TO_END);    // _new_item

////                assert(item([ = ](boost::intrusive_ptr<TreeItem> it) {return
///it->id() == record_shadow->id();})); // dangling parent lead all do not exist
///in model

// if(record_shadow) {    // you can not use result->count_direct() because
// there are no children there
////                    QList< boost::intrusive_ptr<TreeItem> > record_children
///= _sub_branch->records_by_parent_id(record_shadow->id());

////                    for(int i = 0; i < record_children.size(); i++) {
/////foreach(QString current_subbranch_id, subbranch_id_list) {

////                    assert(record_children[i]->parent_id() ==
///record_shadow->id());

////                    assert(0 == record_shadow->count_direct());
// child_paste_recursive(record_shadow, record_shadow->id(), _sub_branch, 0);

////                    }
// }
// }
// }

////        }

// assert(id != "");
// return id;
// };

////    // Выясняется линейная структура добавляемого дерева
////    QList<CLIPB_TREE_ONE_LINE> tree = subbranch->id_tree_get();

////    // Идентификатор стартовой ветки лежит в первом элементе списка
////    QString start_branch_id = tree[0]._branch_id;

////    auto _clip_root_items_parent_id_list =
///subbranch->clip_root_items_parent_id_list();
// auto id_list = _sub_branch->clip_root_items_parent_id_list();

////    int offset = 0;
// if(id_list.size() > 0) {
// for(auto it = id_list.begin(); it != id_list.end(); it++) {

// child_paste_recursive(_target_item
// , *it
// , _sub_branch
// , _sibling_order  // + offset
// );
////        offset++;
// }

// _result = _target_item;
// }

// return _result;  // _target_item->id();
// }

//// Перемещение ветки вверх
// QModelIndex KnowModel::branch_move_up(const QModelIndex &_index)
// {
// return branch_move_up_dn(_index
// , &TreeItem::move_up   //, 1
// );
// }

//// Перемещение ветки вниз
// QModelIndex KnowModel::branch_move_dn(const QModelIndex &_index)
// {
// return branch_move_up_dn(_index
// , &TreeItem::move_dn   //-1
// );
// }

// Перемещение ветки вверх или вниз
QModelIndex tkm_t::move_up_dn(const QModelIndex& _index,
    int (i_t::*_move)() // int direction
    )
{
	QModelIndex _new_index;
	// Получение ссылки на Item элемент по QModelIndex
	boost::intrusive_ptr<i_t> current = child(_index);

	// Перемещается ветка
	emit layoutAboutToBeChanged();

	int move_distance = 0;

	// if(direction == 1) {
	// moveok = current->move_up(); // Перемещение в Item представлении
	// } else {
	// moveok = current->move_dn();
	// }
	move_distance = ((*current).*(_move))();
	if (move_distance != 0) {
		// Получение QModelIndex расположенного над или под элементом index
		QModelIndex swap_index = _index.sibling(_index.row() + move_distance,
		    0); // _index.sibling(_index.row() - direction, 0);
		// Проверяется допустимость индекса элемента, куда будет сделано перемещение
		if (!swap_index.isValid())
			return QModelIndex(); // Возвращается пустой указатель

		// Запоминаются параметры для абсолютного позиционирования засветки
		// после перемещения ветки
		int swpidx_row = swap_index.row();
		int swpidx_column = swap_index.column();
		QModelIndex swpidx_parent = swap_index.parent();

		changePersistentIndex(swap_index, _index); // (from, to)
		changePersistentIndex(_index, swap_index); // (from, to)

		emit layoutChanged();
		// Возвращается указатель на перемещенную ветку
		if (move_distance != 0)
			_new_index = tm_t::index(swpidx_row, swpidx_column, swpidx_parent);
		else
			_new_index = _index; // QModelIndex(); // Возвращается пустой указатель
	}
	return _new_index;
}

// boost::intrusive_ptr<TreeItem> KnowModel::model_child_remove(//
// boost::intrusive_ptr<TreeItem> parent_item,
// boost::intrusive_ptr<TreeItem> target
//// , int position, int rows, const QModelIndex &parent
// )
// {
// boost::intrusive_ptr<TreeItem> _result(nullptr);
// boost::intrusive_ptr<TreeItem> parent_item = target->parent();  //
// item(parent);
// QModelIndex parent_index = index(parent_item);
////    bool success = false;
// int position = target->sibling_order();
// beginRemoveRows(parent_index, position, position);    // + rows - 1

////    success
// _result = parent_item->child_remove(target);
// endRemoveRows();

// return _result;
// }

boost::intrusive_ptr<i_t>
tkm_t::delete_permanent(boost::intrusive_ptr<Linker> delete_linker)
{
	boost::intrusive_ptr<i_t> deleted_item;
	auto host_parent = delete_linker->host_parent();
	auto host = delete_linker->host();

	auto model_delete_permantent_impl =
	    [&](                                                  // boost::intrusive_ptr<TreeItem> parent_item,
		boost::intrusive_ptr<Linker> delete_target_linker // , int position,
		// int rows, const
		// QModelIndex
		// &parent
		) -> boost::intrusive_ptr<i_t> {
		boost::intrusive_ptr<i_t> result(nullptr);
		// boost::intrusive_ptr<TreeItem> host = delete_target_linker->host();  //
		// item(parent);
		boost::intrusive_ptr<i_t> parent_of_delete = delete_target_linker->host_parent(); // item(parent);
		if (parent_of_delete) {
			auto parent_index = index(parent_of_delete);
			// bool success = false;
			auto equal_linker = [&](boost::intrusive_ptr<const Linker> il) {
				return il->host()->id() == delete_target_linker->host()->id() && il == delete_target_linker;
			};
			int position = parent_of_delete->sibling_order(equal_linker);
			gl_paras->main_window()->setDisabled(true);
			beginRemoveRows(parent_index, position, position); // + rows - 1

			// remove_target->self_remove_from_parent();
			result = parent_of_delete->delete_permanent(equal_linker);
			web::WebView* v = gl_paras->main_window()->find(
			    [&](boost::intrusive_ptr<const ::Binder> b) {
				    return b->host() == result;
			    });
			if (v)
				v->page()->tabmanager()->closeTab(v->page()->tabmanager()->webViewIndex(v));
			auto view = static_cast<tv_t*>(static_cast<QObject*>(this)->parent());

			// update_index(parent_index);
			layoutChanged(QList<QPersistentModelIndex>() << static_cast<QModelIndex>(parent_index));

			view->update(parent_index);
			endRemoveRows();
			gl_paras->main_window()->setEnabled(true);
		}
		return result;
	};
	if (host && static_cast<QModelIndex>(index(host)).isValid()) {
		if (host && host->count_direct() > 0) {
			auto linker_first = host->child_linkers().at(0);
			auto view = static_cast<tv_t*>(static_cast<QObject*>(this)->parent());
			// boost::intrusive_ptr<TreeIndex> tree_index;
			// try {tree_index = new TreeIndex([&] {return view->source_model(); },
			// host_parent, delete_linker->sibling_order()); } catch(std::exception
			// &e) {}

			// assert(tree_index);

			// if(tree_index) {
			auto result = view->move(TreeIndex::item_require_treeindex([&] { return view->source_model(); }, host_parent), linker_first->host(), [&](boost::intrusive_ptr<const Linker> il) -> bool {
				return il->host()->id() == linker_first->host()->id();
			});
			assert(result->id() == linker_first->host()->id());
			if (result != linker_first->host()) {
				tv_t* tree_view = static_cast<tv_t*>(static_cast<QObject*>(this)->parent());
				assert(tree_view);
				result = merge(
				    TreeLevel::instance(
					TreeIndex::item_require_treeindex([&] { return this; }, result),
					linker_first->host()),
				    std::bind(&tv_t::delete_permanent, tree_view, [&] { return this; }, QList<boost::intrusive_ptr<i_t>>() << linker_first->host(), &tkm_t::delete_permanent, "cut", false));
			}
			assert(result->linker()->integrity_external(result, delete_linker->host_parent()));
			// bTreeIndex([&] {return view->source_model(); },
			// result)oost::intrusive_ptr<TreeIndex> tree_index_first;
			// try {tree_index_first = new TreeIndex([&] {return view->source_model();
			// }, result); } catch(std::exception &e) {}
			// assert(tree_index_first);
			// if(tree_index_first) {
			for (auto& il : host->child_linkers()) {
				view->move(TreeIndex::item_require_treeindex([&] { return view->source_model(); }, result), il->host(), [&](boost::intrusive_ptr<const Linker> link) -> bool {
					return il->host()->id() == link->host()->id();
				});
			}
			// }
			// }
		}
		assert(host->count_direct() == 0);
		deleted_item = model_delete_permantent_impl(
		    delete_linker);                                                                       // if(removeRows(_index_delete.row(), 1,
													  // _index_delete.parent()))
		if (host_parent && host_parent->count_direct() == 0 && root_item()->count_direct() > 0) { // (index.row() > 0) == index.isValid()
			tv_t* tree_view = static_cast<tv_t*>(static_cast<QObject*>(this)->parent());
			while ((deleted_item = host_parent->delete_permanent_recursive([&](boost::intrusive_ptr<const i_t> it) {
				return it->is_empty();
			}))) {
				host_parent = host_parent->parent();
				tree_view->cursor_focus();
				if (!host_parent)
					break;
			}
		}
	}
	return deleted_item;
}

// Удаление одной ветки и её подветок
// Delete one branch and its podvetok / garter
boost::intrusive_ptr<i_t>
tkm_t::delete_permanent_recursive(boost::intrusive_ptr<Linker> delete_linker)
{
	auto parent = delete_linker->host_parent();
	auto host = delete_linker->host();
	boost::intrusive_ptr<i_t> deleted_root_item;

	// std::function<void(boost::intrusive_ptr<TreeItem>)>
	// close_tab_recursive = [&](boost::intrusive_ptr<TreeItem> it)->void {
	// if(it->is_registered_to_browser())
	// it->page()->record_ctrl()->remove(it->id());

	// if(it->count_direct() > 0)
	// {
	// for(int i = 0; i < it->count_direct(); i++) {
	// close_tab_recursive(it->item_direct(i));
	// }
	// }
	// };

	std::function<boost::intrusive_ptr<i_t>(boost::intrusive_ptr<Linker>)>
	    model_delete_permantent_impl =
		[&](                                                  // boost::intrusive_ptr<TreeItem> parent_item,
		    boost::intrusive_ptr<Linker> delete_target_linker // , int
		    // position, int
		    // rows, const
		    // QModelIndex
		    // &parent
		    ) mutable -> boost::intrusive_ptr<i_t> {
		boost::intrusive_ptr<i_t> result(nullptr);
		auto host = [&] { return delete_target_linker->host(); };
		auto host_parent = [&] {
			return delete_target_linker->host_parent();
		}; // item(parent);
		if (host() && static_cast<QModelIndex>(index(host())).isValid()) {
			if (host()->count_direct() > 0)
				for (auto& il : host()->child_linkers())
					model_delete_permantent_impl(il);
			// boost::intrusive_ptr<TreeItem> host = delete_target_linker->host();  //
			// item(parent);
			if (host() && host_parent()) {
				auto parent_index = index(host_parent());
				// bool success = false;
				auto equal_linker = [&](boost::intrusive_ptr<const Linker> il) {
					return il->host()->id() == delete_target_linker->host()->id() && il == delete_target_linker;
				};
				int position = host_parent()->sibling_order(equal_linker);
				gl_paras->main_window()->setDisabled(true);
				beginRemoveRows(parent_index, position, position); // + rows - 1

				// remove_target->self_remove_from_parent();
				result = host_parent()->delete_permanent(equal_linker);
				web::WebView* v = gl_paras->main_window()->find(
				    [&](boost::intrusive_ptr<const ::Binder> b) {
					    return b->host() == result;
				    });
				if (v)
					v->page()->tabmanager()->closeTab(v->page()->tabmanager()->webViewIndex(v));
				auto view = static_cast<tv_t*>(static_cast<QObject*>(this)->parent());

				update_index(parent_index);
				layoutChanged(QList<QPersistentModelIndex>() << static_cast<QModelIndex>(parent_index));

				view->update(parent_index);
				endRemoveRows();
				gl_paras->main_window()->setEnabled(true);
			}
		}
		return result;
	};
	if (host && static_cast<QModelIndex>(index(host)).isValid()) {
		// qDebug() << "Label 1";

		////        // Получение узла, который соответствует обрабатываемому индексу
		////        _item_deleted = _item_delete;

		////        int deleted_position = _item_to_be_deleted->sibling_order();
		/////item_to_be_deleted->parent()->sibling_order(item_to_be_deleted);

		// qDebug() << "Label 2, branch id:" << host->field("id") << "name:" <<
		// host->field("name");

		//// Получение пути к элементу
		// QStringList _delete_path = host->path_absolute();

		// qDebug() << "Label 3";

		//// Получение путей ко всем подветкам
		// QList<QStringList> sub_branches_path = host->path_children_all();

		// qDebug() << "Label 4";

		//// Сортировка массива веток по длине пути
		// qSort(sub_branches_path.begin(), sub_branches_path.end()
		// , [](const QStringList & list1, const QStringList & list2) {return
		// list1.size() < list2.size();}    // compare_qstringlist_length
		// );

		// qDebug() << "Label 5";

		//// Удаление всех таблиц конечных записей для нужных подветок
		//// Удаление всех подчиненных элементов для нужных подветок
		//// Вначале пробегаются самые длинные ветки а потом более короткие
		// for(int i = sub_branches_path.size() - 1; i >= 0; i--) {
		// qDebug() << "Delete subbranch, id:" <<
		// item(sub_branches_path.at(i))->field("id") << "name:" <<
		// item(sub_branches_path.at(i))->field("name");

		// item(sub_branches_path.at(i))->clear();
		//// ( knowTreeModel->getItem(subbranchespath.at(i)) )->removeAllChildren();
		// }

		// qDebug() << "Label 6";

		//// Удаление таблицы конечных записей для самой удаляемой ветки
		//// Удаление подчиненных элементов для самой удаляемой ветки
		// qDebug() << "Delete rootbranch, id:" << item(_delete_path)->field("id")
		// << "name:" << item(_delete_path)->field("name");

		// item(_delete_path)->clear();
		//// ( knowTreeModel->getItem(path) )->removeAllChildren();

		// qDebug() << "Label 7";

		//// Удаление ветки на экране, при этом удалятся все подветки
		// qDebug() << "This branch have row() as" << host->count_direct();
		// auto _index_delete = index(host);

		// if(_index_delete.isValid()) qDebug() << "Index valid";
		// else qDebug() << "Index non valid";

		// if( host->count_direct() > 0) {

		// for(auto &il : host->child_linkers()) {
		// model_delete_permantent_impl(il);
		// }

		// }

		// _source_model->beginRemoveRows(_index_to_be_delete.parent(),
		// _index_to_be_delete.row(), _index_to_be_delete.row());
		deleted_root_item = model_delete_permantent_impl(
		    delete_linker); // if(removeRows(_index_delete.row(), 1,
				    // _index_delete.parent()))

		////        if(!_item_deleted)_item_deleted = _item_delete; // temporary
		///radded before reimplement relation management of TreeItem!!!, 2016-04-07

		////        assert(_item_deleted);    // temporary removed before
		///reimplement relation management of TreeItem!!!, 2016-04-07

		////        _source_model->endRemoveRows();

		////        items_update_selected();

		////    // Удаление всех нужных подветок
		////    // Пробегаются самые длинные ветки а потом более короткие
		////    for(int i = subbranchespath.size() - 1; i >= 0; i--)
		////        if(knowTreeModel->isItemValid(subbranchespath.at(i))) {
		////            TreeItem *current_item =
		///knowTreeModel->getItem(subbranchespath.at(i));
		////            delete current_item;
		////        }

		////    if(!reocrd_controller) {
		////
		///globalparameters.entrance()->new_browser(QUrl(web::Browser::_defaulthome));
		////    }

		////        if(item_to_be_deleted->is_registered_to_browser())   //
		///item_to_be_deleted->unique_page()
		////
		///item_to_be_deleted->unique_page()->record_ctrl()->remove_child(item_to_be_deleted->id());
		///// (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

		// close_tab_recursive(host);

		qDebug() << "Label 8";
		//// items_update_selected();
		// QModelIndex setto;
		// auto _parent = _item_to_be_deleted->parent();
		// if(_parent) {
		// int new_count = _parent->count_direct();
		//// Одноранговая ветка
		// if(new_count > 0) {
		// int new_pos = (deleted_position > new_count - 1) ? new_count - 1 :
		// ((deleted_position - 1) >= 0) ? deleted_position : 0;
		// setto = _current_model->index(new_pos, 0, index.parent());
		// _tree_view->selectionModel()->select(setto, current_tree_selection_mode);
		// _tree_view->selectionModel()->setCurrentIndex(setto,
		// current_tree_selection_mode);   // ClearAndSelect
		// } else {
		////            //        setto = _root_model->index_child(_current_index,
		///current_item->direct_children_count() - 1);
		////            setto = static_cast<TreeModel
		///*>(_current_know_branch)->index(index.parent().row(), 0,
		///index.parent().parent());
		////            _tree_view->selectionModel()->setCurrentIndex(setto,
		///QItemSelectionModel::ClearAndSelect);
		// while(_parent != _know_model_board->root_item()) {
		// if(_parent != _current_model->root_item() &&
		// _current_model->index(_parent).isValid()) {
		// _tree_view->selectionModel()->select(_current_model->index(_parent),
		// current_tree_selection_mode);
		// _tree_view->selectionModel()->setCurrentIndex(_current_model->index(_parent),
		// current_tree_selection_mode);   // ClearAndSelect
		// break;
		// } else { // if(_parent != _know_model_board->root_item())
		// _parent = _parent->parent();
		// cursor_up_one_level();
		// }
		// }
		////            if(index.parent().parent().isValid()) {view_up_one_level();}
		////            else {view_return_to_root();}
		// }
		// }
		// assert(item_to_be_deleted == _current_know_branch->item(setto));
		// try to rewrite, but below is just memory operation which is not we want,
		// but but, the original code is also a memory ooperation too? why?
		if (parent && parent->count_direct() == 0 && root_item()->count_direct() > 0) { // (index.row() > 0) == index.isValid()
			// if(_parent)_parent->child_remove(_item_deleted);
			tv_t* tree_view = static_cast<tv_t*>(static_cast<QObject*>(this)->parent());
			while ((deleted_root_item = parent->delete_permanent_recursive([&](boost::intrusive_ptr<const i_t> it) {
				return it->is_empty();
			}))) {
				parent = parent->parent();
				tree_view->cursor_focus();
				if (!parent)
					break;
			}
		}
	}
	return deleted_root_item;
}

boost::intrusive_ptr<i_t>
tkm_t::merge(boost::intrusive_ptr<TreeLevel> _tree_merge, const view_delete_permantent_strategy& _view_delete_permantent)
{
	boost::intrusive_ptr<TreeIndex> tree_index = _tree_merge->tree_index();
	boost::intrusive_ptr<i_t> source = _tree_merge->to_be_operated();
	boost::intrusive_ptr<i_t> result = tree_index->host();

	auto view = static_cast<tv_t*>(static_cast<QObject*>(this)->parent());
	assert(view);
	// auto _current_model = tree_index->current_model();
	////    auto source = source;	//
	///_current_model()->item([=](boost::intrusive_ptr<const TreeItem> t){return
	///t->id() == source->id();});
	////    auto result = tree_index->host();	//
	///item([=](boost::intrusive_ptr<const TreeItem> t){return t->id() ==
	///tree_index->host()->id();});
	// if(source->is_ancestor_of([&](boost::intrusive_ptr<const TreeItem>
	// it){return it == result;})){
	// view->paste_sibling(TreeIndex::instance(_current_model, source,
	// source->parent()), result, [&](boost::intrusive_ptr<const Linker>
	// il){return il->host()->id() == result->id();});
	// }
	if (result && result != source) {

		auto old_source_parent = source->parent();
		auto _index_origin_source = index(source);
		auto _index_target = index(result);
		// auto _index_origin = index(source);

		beginInsertColumns(
		    _index_target,             // .parent()
		    0,                         // target->count_direct() - 1 // target->sibling_order()    // 0
		    result->count_direct() - 1 // target->sibling_order()    // target->count_direct() - 1
		    );

		result = result->merge(source); // not a pure insertion, move removerows to
		// below: _view_delete_permantent

		auto r = child([=](boost::intrusive_ptr<const i_t> t) { return t->id() == result->id(); });
		assert(r);
		assert(static_cast<QModelIndex>(tree_index->current_model()()->index(r)).isValid());
		// assert(item([ = ] (boost::intrusive_ptr<const TreeItem> t) {return
		// t->id() == result->id();}));
		assert(source->count_direct() == 0);

		// if(_index_origin.isValid())update_index(_index_origin.parent());
		auto old_source_parent_index = index(old_source_parent);
		if (static_cast<QModelIndex>(old_source_parent_index).isValid()) {
			// update_index(old_source_parent_index);
			// view->update(old_source_parent_index);
		}
		auto _index_result = index(result);
		if (static_cast<QModelIndex>(_index_result).isValid()) {
			// update_index(_index_result);
			// view->update(_index_result);
		}
		// if(_index_result.parent().isValid())update_index(_index_result.parent());
		if (result->parent()) {
			// emit_datachanged_signal(index(result->parent()->sibling_order([&](boost::intrusive_ptr<const
			// Linker> it) {return it->host()->id() == result->id();}), 0,
			// _index_result.parent()));
			emit_datachanged_signal(index(result->sibling_order([&](boost::intrusive_ptr<const Linker> il) {
				return il->host()->id() == result->id() && il == result->linker() && result->parent() == il->host_parent();
			}),
			    0, _index_result));
			emit layoutChanged(QList<QPersistentModelIndex>() << static_cast<QModelIndex>(_index_result));
		}
		endInsertColumns();
		assert(source->count_direct() == 0);
		if (static_cast<QModelIndex>(_index_origin_source).isValid()) {
			auto index_ = index(source);
			if (static_cast<QModelIndex>(index_).isValid()) { // source->parent()->field("name") !=
					      // clipboard_items_root
				if (source->count_direct() == 0) {
					// beginRemoveRows(_index.parent(), _index.row(), _index.row());
					if (source->linker()->host() && source->linker()->host_parent()) {
						QList<boost::intrusive_ptr<i_t>> delete_list = _view_delete_permantent(
						    [&]() -> tkm_t* { return this; },
						    QList<boost::intrusive_ptr<i_t>>()
							<< source); // , _delete_strategy, "cut", false

						// auto removed = record_remove(source);
						// assert(removed == source);
						// assert(removed->count_direct() == 0);

						assert(delete_list.size() <= 1); // ?

						// endRemoveRows();

						// auto model =
						// globalparameters.tree_screen()->tree_view()->source_model();

						// if(model->index(source).isValid() ||
						// model->index(target).isValid()) {
						// globalparameters.tree_screen()->tree_view()->reset();
						// globalparameters.tree_screen()->tree_view()->source_model(model->root_item());
						// }
					}
				}
				// else{
				// source->traverse();
				// source->parent()->delete_permanent([&](boost::intrusive_ptr<const
				// Linker> il){return il->host() == source && il == source->linker();});
				// }
			} else {
				source->parent()->delete_permanent(
				    [&](boost::intrusive_ptr<const Linker> il) {
					    return il->host() == source && il == source->linker();
				    });
			}
		}
	}
	//	else if (result)
	//		result = result;
	return result; // keep;
}

//// Удаление одной ветки и её подветок
//// Delete one branch and its podvetok / garter
// boost::intrusive_ptr<TreeItem> KnowModel::model_delete_index(QModelIndex
// _index_delete)
// {
// return model_delete_item(item(_index_delete));  // ?

////    boost::intrusive_ptr<TreeItem> _item_deleted(nullptr);

////    std::function<void(boost::intrusive_ptr<TreeItem>)> close_tab_recursive
///= [&](boost::intrusive_ptr<TreeItem> it) {
////        if(it->is_registered_to_browser())   //
///item_to_be_deleted->unique_page()
////            it->bounded_page()->record_ctrl()->page_remove(it->id());
///// (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

////        if(it->count_direct() > 0) {
////            for(int i = 0; i < it->count_direct(); i++) {
////                close_tab_recursive(it->item_direct(i));
////            }
////        }
////    };

////    if(_index_delete.isValid()) {

////        qDebug() << "Label 1";

////        // Получение узла, который соответствует обрабатываемому индексу
////        auto _item_delete = item(_index_delete);

////        //        int deleted_position =
///_item_to_be_deleted->sibling_order();
/////item_to_be_deleted->parent()->sibling_order(item_to_be_deleted);

////        qDebug() << "Label 2, branch id:" << _item_delete->field("id") <<
///"name:" << _item_delete->field("name");

////        // Получение пути к элементу
////        QStringList _delete_path = _item_delete->path_absolute();

////        qDebug() << "Label 3";

////        // Получение путей ко всем подветкам
////        QList<QStringList> sub_branches_path =
///_item_delete->path_children_all();

////        qDebug() << "Label 4";

////        // Сортировка массива веток по длине пути
////        qSort(sub_branches_path.begin(), sub_branches_path.end()
////        , [](const QStringList & list1, const QStringList & list2) {return
///list1.size() < list2.size();}    // compare_qstringlist_length
////             );

////        qDebug() << "Label 5";

////        // Удаление всех таблиц конечных записей для нужных подветок
////        // Удаление всех подчиненных элементов для нужных подветок
////        // Вначале пробегаются самые длинные ветки а потом более короткие
////        for(int i = sub_branches_path.size() - 1; i >= 0; i--) {
////            qDebug() << "Delete subbranch, id:" <<
///item(sub_branches_path.at(i))->field("id") << "name:" <<
///item(sub_branches_path.at(i))->field("name");

////            item(sub_branches_path.at(i))->children_clear();
////            // ( knowTreeModel->getItem(subbranchespath.at(i))
///)->removeAllChildren();
////        }

////        qDebug() << "Label 6";

////        // Удаление таблицы конечных записей для самой удаляемой ветки
////        // Удаление подчиненных элементов для самой удаляемой ветки
////        qDebug() << "Delete rootbranch, id:" <<
///item(_delete_path)->field("id") << "name:" <<
///item(_delete_path)->field("name");

////        item(_delete_path)->children_clear();
////        // ( knowTreeModel->getItem(path) )->removeAllChildren();

////        qDebug() << "Label 7";

////        // Удаление ветки на экране, при этом удалятся все подветки
////        qDebug() << "This branch have row() as" << _index_delete.row();

////        if(_index_delete.isValid()) qDebug() << "Index valid";
////        else qDebug() << "Index non valid";

////        //
///_source_model->beginRemoveRows(_index_to_be_delete.parent(),
///_index_to_be_delete.row(), _index_to_be_delete.row());
////        //        if(
////        _item_deleted = model_child_remove(_item_delete); // ,
///_index_delete.row(), 1, _index_delete.parent());
////        //        )
////        //        {
////        //            _item_deleted = _item_delete;
////        //        }

////        assert(_item_deleted);
////        //        _source_model->endRemoveRows();

////        //        items_update_selected();

////        //    // Удаление всех нужных подветок
////        //    // Пробегаются самые длинные ветки а потом более короткие
////        //    for(int i = subbranchespath.size() - 1; i >= 0; i--)
////        //        if(knowTreeModel->isItemValid(subbranchespath.at(i))) {
////        //            TreeItem *current_item =
///knowTreeModel->getItem(subbranchespath.at(i));
////        //            delete current_item;
////        //        }

////        //    if(!reocrd_controller) {
////        //
///globalparameters.entrance()->new_browser(QUrl(web::Browser::_defaulthome));
////        //    }

////        //        if(item_to_be_deleted->is_registered_to_browser())   //
///item_to_be_deleted->unique_page()
////        //
///item_to_be_deleted->unique_page()->record_ctrl()->remove_child(item_to_be_deleted->id());
///// (*reocrd_controller)()->remove_child(item_to_be_deleted->id());

////        close_tab_recursive(_item_deleted);

////        qDebug() << "Label 8";

////        //        // items_update_selected();
////        //        QModelIndex setto;
////        //        auto _parent = _item_to_be_deleted->parent();

////        //        if(_parent) {
////        //            int new_count = _parent->count_direct();

////        //            // Одноранговая ветка
////        //            if(new_count > 0) {
////        //                int new_pos = (deleted_position > new_count - 1) ?
///new_count - 1 : ((deleted_position - 1) >= 0) ? deleted_position : 0;

////        //                setto = _current_model->index(new_pos, 0,
///index.parent());
////        //                _tree_view->selectionModel()->select(setto,
///current_tree_selection_mode);
////        //
///_tree_view->selectionModel()->setCurrentIndex(setto,
///current_tree_selection_mode);   // ClearAndSelect

////        //            } else {

////        //                //            //        setto =
///_root_model->index_child(_current_index,
///current_item->direct_children_count() - 1);
////        //                //            setto = static_cast<TreeModel
///*>(_current_know_branch)->index(index.parent().row(), 0,
///index.parent().parent());
////        //                //
///_tree_view->selectionModel()->setCurrentIndex(setto,
///QItemSelectionModel::ClearAndSelect);

////        //                while(_parent != _know_model_board->root_item()) {

////        //                    if(_parent != _current_model->root_item() &&
///_current_model->index(_parent).isValid()) {
////        //
///_tree_view->selectionModel()->select(_current_model->index(_parent),
///current_tree_selection_mode);
////        //
///_tree_view->selectionModel()->setCurrentIndex(_current_model->index(_parent),
///current_tree_selection_mode);   // ClearAndSelect

////        //                        break;
////        //                    } else { // if(_parent !=
///_know_model_board->root_item())
////        //                        _parent = _parent->parent();
////        //                        cursor_up_one_level();
////        //                    }
////        //                }

////        //                //
///if(index.parent().parent().isValid()) {view_up_one_level();}
////        //                //            else {view_return_to_root();}
////        //            }

////        //        }

////        //        assert(item_to_be_deleted ==
///_current_know_branch->item(setto));

////        auto _parent = _item_deleted->parent();

////        // try to rewrite, but below is just memory operation which is not
///we want, but but, the original code is also a memory ooperation too? why?
////        if(_parent && _parent->count_direct() == 0 &&
///root_item()->count_direct() > 0) { // (index.row() > 0) == index.isValid()

////            // if(_parent)
////            //            _parent->child_remove(_item_deleted);

////            while(_parent->self_empty_remove()) {
////                _parent = _parent->parent();

////                if(!_parent)break;
////            }

////        }
////    }

// return _item_deleted;
// }

// Получение индекса подчиненного элемента с указанным номером
// Нумерация элементов считается что идет с нуля
QModelIndex tkm_t::index_direct(const QModelIndex& _parent_index, int n) const
{
	// Проверяется, передан ли правильный QModelIndex
	// Если он неправильный, возвращается пустой индекс
	if (!_parent_index.isValid()) {
		qDebug() << "In indexChildren() unavailable model index";

		return QModelIndex();
	}
	// Выясняется указатель на основной Item элемент
	boost::intrusive_ptr<i_t> it = child(_parent_index);
	// Если у основного Item элемента запрашивается индекс
	// несуществующего подэлемента, возвращается пустой индекс
	if (n < 0 || n >= it->count_direct()) {
		qDebug() << "In indexChildren() unavailable children number";

		return QModelIndex();
	}
	// Выясняется указатель на подчиненный Item элемент
	boost::intrusive_ptr<i_t> child_item = it->child_direct(n);
	// Если указатель на подчиненный Item элемент непустой
	if (child_item) {
		// return createIndex(0, 0, childitem); // Индекс подчиненного элемента
		// return createIndex(n, 0, parent.internalPointer());

		return tm_t::index(n, 0, _parent_index);
	} else {
		qDebug() << "In indexChildren() empty child element";

		return QModelIndex(); // Возвращается пустой индекс
	}
}

//// Get QModelIndex of the KnownTreeItem   // Получение QModelIndex по
///известному TreeItem
// QModelIndex KnowTreeModel::index(boost::intrusive_ptr<TreeItem> item)
// {
// int itemrow = item->child_index();

// return this->createIndex(itemrow, 0, static_cast<void *>(item.get()));
// }

// Возвращает общее количество записей, хранимых в дереве
int tkm_t::count_records_all(void) const
{
	//// Обнуление счетчика
	// get_all_record_count_recurse(_root_item, 0);

	// return get_all_record_count_recurse(_root_item, 1);

	return size_of(_root_item);
}

// Возвращает количество записей в ветке и всех подветках
int tkm_t::size_of(boost::intrusive_ptr<const i_t> it) const
{
	std::function<int(boost::intrusive_ptr<const i_t>, int)>
	    get_all_record_count_recurse =
		[&](boost::intrusive_ptr<const i_t> _it, int mode) {
			static int n = 0;
			if (mode == 0) {
				n = 0;

				return 0;
			}
			n = n + _it->count_direct();
			for (int i = 0; i < _it->count_direct(); i++)
				get_all_record_count_recurse(_it->child_direct(i), 1);
			return n;
		};

	// Обнуление счетчика
	get_all_record_count_recurse(_root_item, 0);

	return get_all_record_count_recurse(it, 1);
}

// int
// KnowTreeModel::get_all_record_count_recurse(boost::intrusive_ptr<TreeItem>
// item, int mode)
// {
// static int n = 0;

// if(mode == 0) {
// n = 0;
// return 0;
// }

// n = n + item->row_count();

// for(int i = 0; i < item->child_count(); i++)
// get_all_record_count_recurse(item->child(i), 1);

// return n;
// }

//// Проверка наличия идентификатора ветки во всем дереве
// bool KnowModel::is_item_exists(QString findId)
// {

////    std::function<bool (boost::intrusive_ptr<TreeItem>, QString, int)>
////    is_item_id_exists_recurse =
////        [&](boost::intrusive_ptr<TreeItem> item, QString id_to_find, int
///mode
////    ) {
////        static bool is_exists = false;

////        // Инициализация
////        if(mode == 0) {
////            return is_exists = false;
////            //            return false;
////        }

////        // Если ветка найдена, дальше проверять не имеет смысла. Это условие
///ускоряет возврат из рекурсии.
////        if(is_exists)
////            return true;

////        // Если текущая ветка содержит искомый идетнификатор
////        if(item->field("id") == id_to_find) {
////            return is_exists = true;
////            //            return true;
////        }

////        // Перебираются подветки
////        for(int i = 0; i < item->current_count(); i++)
////            is_item_id_exists_recurse(item->child(i), id_to_find, 1);

////        return is_exists;
////    };

//// Обнуление счетчика
// is_item_id_exists_recurse(_root_item, findId, 0);

// return is_item_id_exists_recurse(_root_item, findId, 1);
// }

//// Проверка наличия идентификатора записи во всем дереве
// bool KnowModel::is_id_exists(QString findId)const
// {

////    std::function<bool (boost::intrusive_ptr<TreeItem>, QString, int)>
////    is_record_id_exists_recurse
////        = [&](
////              boost::intrusive_ptr<TreeItem> item, QString find_id, int mode
////    ) {

////        static bool isExists = false;

////        // Инициализация
////        if(mode == 0) {
////            return isExists = false;
////            // return false;
////        }

////        // Если запись найдена, дальше проверять не имеет смысла. Это
///условие ускоряет возврат из рекурсии.
////        if(isExists)
////            return true;

////        // Если таблица записей текущей ветки содержит искомый идентификатор
////        if(item->is_item_exists(find_id)) {
////            return isExists = true;
////            // return true;
////        }

////        // Перебираются подветки
////        for(int i = 0; i < item->current_count(); i++)
////            is_record_id_exists_recurse(item->child(i), find_id, 1);

////        return isExists;
////    };

// std::function<bool (boost::intrusive_ptr<TreeItem>, QString, int)>
// is_item_id_exists_recurse =
// [&](boost::intrusive_ptr<TreeItem> _it, QString id_to_find, int mode) {
// static bool is_exists = false;

//// Инициализация
// if(mode == 0) {
// return is_exists = false;
////            return false;
// }

//// Если ветка найдена, дальше проверять не имеет смысла. Это условие ускоряет
///возврат из рекурсии.
// if(is_exists)
// return true;

//// Если текущая ветка содержит искомый идетнификатор
// if(_it->field("id") == id_to_find) {
// return is_exists = true;
////            return true;
// }

//// Перебираются подветки
// for(int i = 0; i < _it->count_direct(); i++)
// is_item_id_exists_recurse(_it->item_direct(i), id_to_find, 1);

// return is_exists;
// };

//// Обнуление счетчика
// is_item_id_exists_recurse(_root_item, findId, 0);

// return is_item_id_exists_recurse(_root_item, findId, 1);
// }

// bool KnowTreeModel::is_item_id_exists_recurse(boost::intrusive_ptr<TreeItem>
// item, QString findId, int mode)
// {
// static bool isExists = false;

//// Инициализация
// if(mode == 0) {
// isExists = false;
// return false;
// }

//// Если ветка найдена, дальше проверять не имеет смысла. Это условие ускоряет
///возврат из рекурсии.
// if(isExists)
// return true;

//// Если текущая ветка содержит искомый идетнификатор
// if(item->field("id") == findId) {
// isExists = true;
// return true;
// }

//// Перебираются подветки
// for(int i = 0; i < item->child_count(); i++)
// is_item_id_exists_recurse(item->child(i), findId, 1);

// return isExists;
// }

#ifdef _with_record_table
void TreeModelKnow::record_to_item()
{
	std::function<void(
	    boost::intrusive_ptr<TreeItem>)> // , boost::intrusive_ptr<TreeItem>
	    record_to_item_recurse = [&](boost::intrusive_ptr<TreeItem> item
		// , boost::intrusive_ptr<TreeItem> parent
		) {
		    // if(!is_item_id_exists(item->id())) {
		    // add_child(item, parent);
		    // }

		    item->records_to_children();
		    // Перебираются подветки
		    for (int i = 0; i < item->size(); i++)
			    record_to_item_recurse(item->child(i)); // , item
	    };

	_root_item->records_to_children();
	// if(_root_item->child_count() > 0) {
	for (int i = 0; i < _root_item->size(); i++)
		record_to_item_recurse(_root_item->child(i)); // , _root_item
							      // }
}

#endif

// bool
// KnowTreeModel::is_record_id_exists_recurse(boost::intrusive_ptr<TreeItem>
// item, QString findId, int mode)
// {
// static bool isExists = false;

//// Инициализация
// if(mode == 0) {
// isExists = false;
// return false;
// }

//// Если запись найдена, дальше проверять не имеет смысла. Это условие ускоряет
///возврат из рекурсии.
// if(isExists)
// return true;

//// Если таблица записей текущей ветки содержит искомый идентификатор
// if(item->is_item_exists(findId)) {
// isExists = true;
// return true;
// }

//// Перебираются подветки
// for(int i = 0; i < item->child_count(); i++)
// is_record_id_exists_recurse(item->child(i), findId, 1);

// return isExists;
// }

//// Рекурсивное добавление дерева
//// item - элемент главного дерева, к которому добавляется ветка
//// start_branch_id - идентификатор ветки в переданном линейном представлении
////                   добавляемого дерева
//// subbranch - добавляемое дерево
// QString
// KnowTreeModel::paste_sub_branch_recurse(boost::intrusive_ptr<TreeItem> item
// , QString startBranchId
// , ClipboardBranch *subbranch)
// {
// qDebug() << "In paste_subbranch_recurse()";

//// ---------------------------
//// Добавляется стартовая ветка
//// ---------------------------

//// Выясняются поля стартовой ветки
// QMap<QString, QString> subbranch_fields =
// subbranch->getBranchFieldsById(startBranchId);

//// Выясняется имя ветки
// QString subbranch_name = subbranch_fields["name"];

//// Получение уникального идентификатора, под которым будет добавляться ветка
// QString id = get_unical_id();

//// Стартовая ветка добавляется
// add_new_branch(item, id, subbranch_name);

//// Выясняется указатель на эту добавленную ветку
// auto newitem = item_by_id(id);

// qDebug() << "KnowTreeModel::paste_subbranch_recurse() : create branch with
// field" << newitem->all_fields();

//// -----------------------------------------------
//// Для стартовой ветки добавляются конечные записи
//// -----------------------------------------------

//// Выясняются данные конечных записей
// QList< boost::intrusive_ptr<Record> > records =
// subbranch->getBranchRecords(startBranchId);

// foreach(boost::intrusive_ptr<Record> record, records) {
// qDebug() << "Add table record " + record->field("name");
// newitem->record_table()->insert_new_record(0, record, ADD_NEW_RECORD_TO_END);
// }

//// --------------------
//// Добавляются подветки
//// --------------------

//// Выясняется линейная структура добавляемого дерева
// QList<CLIPB_TREE_ONE_LINE> tree = subbranch->getIdTree();

//// Выясняется список подветок для стартовой ветки
// QStringList subbranch_list;

// foreach(CLIPB_TREE_ONE_LINE one_line, tree)
// if(one_line.branch_id == startBranchId)
// subbranch_list = one_line.subbranches_id;

// foreach(QString current_subbranch, subbranch_list)
// paste_sub_branch_recurse(newitem, current_subbranch, subbranch);

// return id;
// }

// Перешифрование базы с новым паролем
void tkm_t::re_encrypt(QString previousPassword, QString currentPassword)
{
	// Получение путей ко всем подветкам дерева
	QList<QStringList> subbranchespath = _root_item->path_children_all<id_key>();

	// Перебираются подветки
	foreach (QStringList currPath, subbranchespath) {
		// Перешифровываются только те подветки, которые имеют
		// флаг шифрования, и у которых родительская ветка нешифрована
		auto currBranch = child(currPath);
		auto currBranchParent = currBranch->parent();
		if (currBranch->field<crypt_key>() == crypt_value(true) && currBranchParent->field<crypt_key>() != crypt_value(true)) {
			Password pwd;

			pwd.setCryptKeyToMemory(previousPassword);
			currBranch->to_decrypt();

			pwd.setCryptKeyToMemory(currentPassword);
			currBranch->to_encrypt();
		}
	} // Закончился перебор подветок

	// Сохранение дерева веток
	// find_object<TreeScreen>(tree_screen_singleton_name)
	static_cast<tv_t*>(static_cast<QObject* const>(this)->parent())
	    ->know_model_save();
}

//// Функция ищет ветку с указанным ID и возвращает ссылку не неё в виде
///TreeItem *
//// Если ветка с указанным ID не будет найдена, возвращается NULL
// boost::intrusive_ptr<TreeItem> KnowModel::item_by_url(QUrl find_url)const
// {
// std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeItem>,
// QUrl, int)>
// item_by_url_recurse    //
// boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem>
// item, QString name, int mode);
// = [&](boost::intrusive_ptr<TreeItem> item, QUrl find_url, int mode) {
// static boost::intrusive_ptr<TreeItem> find_item;

// if(mode == 0) {
// find_item = nullptr;
// return find_item;   // nullptr;
// }

// if(find_item) return find_item;

// if(QUrl(item->field("url")).fragment() == find_url.fragment()) {
// find_item = item;
// return find_item;
// } else {
// for(int i = 0; i < item->count_direct(); i++)
// item_by_url_recurse(item->child(i), find_url, 1);

// return find_item;
// }
// };

//// Инициализация поиска
// item_by_url_recurse(_root_item, QUrl(), 0);

//// Запуск поиска и возврат результата
// return item_by_url_recurse(_root_item, find_url, 1);
// }

QList<boost::intrusive_ptr<i_t>> tkm_t::children(
    const std::function<bool(boost::intrusive_ptr<const i_t>)>& _equal)
{
	std::function<QList<boost::intrusive_ptr<i_t>>(
	    boost::intrusive_ptr<i_t>,
	    int)> // , std::function<bool(boost::intrusive_ptr<TreeItem>)> _equal
	    item_by_equal_recurse = [&](boost::intrusive_ptr<i_t> it_, int mode)
	    -> QList<boost::intrusive_ptr<i_t>> { // boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem>
						  // item, QString name, int mode);
		    // , std::function<bool(boost::intrusive_ptr<TreeItem>)> _equal
		    static QList<boost::intrusive_ptr<i_t>>
			_items_list; // static boost::intrusive_ptr<TreeItem> _index_list;
		    if (mode == 0) {
			    _items_list.clear(); // find_item = nullptr;
			    return _items_list;  // nullptr;
		    }
		    // if(_index_list.size() > 0) return _index_list;
		    if (_equal(it_)) {
			    if (!_items_list.contains(it_))
				    _items_list.push_back(it_);
			    // return _index_list;
		    } else {
			    for (int i = 0; i < it_->count_direct(); i++)
				    item_by_equal_recurse(it_->child_direct(i), 1);
			    // return _index_list;
		    }
		    return _items_list;
	    };

	// Инициализация поиска
	item_by_equal_recurse(_root_item, 0);

	// Запуск поиска и возврат результата
	return item_by_equal_recurse(_root_item, 1);
}

QModelIndexList tkm_t::indexes(
    const std::function<bool(boost::intrusive_ptr<const i_t>)>& _equal)
{
	std::function<QModelIndexList(
	    boost::intrusive_ptr<i_t>,
	    int)> // , std::function<bool(boost::intrusive_ptr<TreeItem>)> _equal
	    item_by_equal_recurse = [&](
		boost::intrusive_ptr<const i_t> it_,
		int mode) { // boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem>
			    // item, QString name, int mode);
		    // , std::function<bool(boost::intrusive_ptr<TreeItem>)> _equal

		    static QModelIndexList
			_index_list; // static boost::intrusive_ptr<TreeItem> _index_list;
		    if (mode == 0) {
			    _index_list.clear(); // find_item = nullptr;
			    return _index_list;  // nullptr;
		    }
		    // if(_index_list.size() > 0) return _index_list;
		    if (_equal(it_)) {
			    _index_list.push_back(index(it_));
			    // return _index_list;
		    } else {
			    for (int i = 0; i < it_->count_direct(); i++)
				    item_by_equal_recurse(it_->child_direct(i), 1);
			    // return _index_list;
		    }
		    return _index_list;
	    };

	// Инициализация поиска
	item_by_equal_recurse(_root_item, 0);

	// Запуск поиска и возврат результата
	return item_by_equal_recurse(_root_item, 1);
}

boost::intrusive_ptr<i_t>
tkm_t::child(const std::function<bool(boost::intrusive_ptr<const i_t>)>& _equal)
    const // const QUrl &_find_url, equal_type _equal
{
	// std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeItem>,
	// std::function<bool(boost::intrusive_ptr<TreeItem>)>, int)>
	// item_by_url_recurse    //
	// boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem>
	// item, QString name, int mode);
	// = [&](boost::intrusive_ptr<TreeItem> _it
	// , std::function<bool(boost::intrusive_ptr<TreeItem>)> _equal    // const
	// QUrl & _find_url
	// , int mode
	// ) {
	// static boost::intrusive_ptr<TreeItem> find_item;

	// if(mode == 0) {
	// find_item = nullptr;
	// return find_item;   // nullptr;
	// }

	// if(find_item) return find_item;

	// if(_equal(_it)) {  //, _find_url
	//// if(item->url<url_type>() == url_type()(find_url)) {
	// find_item = _it;
	// return find_item;
	// } else {
	// for(int i = 0; i < _it->count_direct(); i++)
	// item_by_url_recurse(_it->item_direct(i), _equal, 1);    // _find_url

	// return find_item;
	// }
	// };

	//// Инициализация поиска
	// item_by_url_recurse(_root_item, _equal, 0);    // QUrl()

	//// Запуск поиска и возврат результата
	// return item_by_url_recurse(_root_item, _equal, 1); // _find_url

	return tm_t::item(_equal);
}

//// Функция ищет ветку с указанным ID и возвращает ссылку не неё в виде
///TreeItem *
//// Если ветка с указанным ID не будет найдена, возвращается NULL
// boost::intrusive_ptr<TreeItem> KnowModel::item_by_name(QString name)const
// {
// std::function<boost::intrusive_ptr<TreeItem>(boost::intrusive_ptr<TreeItem>,
// QString, int)>
// item_by_name_recurse    //
// boost::intrusive_ptr<TreeItem>(*item_by_name_recurse)(boost::intrusive_ptr<TreeItem>
// item, QString name, int mode);
// = [&](boost::intrusive_ptr<TreeItem> item, QString name, int mode) {
// static boost::intrusive_ptr<TreeItem> find_item;

// if(mode == 0) {
// find_item = nullptr;
// return find_item;   // nullptr;
// }

// if(find_item) return find_item;

// if(item->name() == name) {
// find_item = item;
// return find_item;
// } else {
// for(int i = 0; i < item->count_direct(); i++)
// item_by_name_recurse(item->item_direct(i), name, 1);

// return find_item;
// }
// };

//// Инициализация поиска
// item_by_name_recurse(_root_item, "", 0);

//// Запуск поиска и возврат результата
// return item_by_name_recurse(_root_item, name, 1);
// }

boost::intrusive_ptr<i_t> tkm_t::child(QStringList path) const
{
	return tm_t::item(path);
}

boost::intrusive_ptr<i_t> tkm_t::child(const QModelIndex& _index) const
{
	return tm_t::item(_index);
}

//// Функция ищет ветку с указанным ID и возвращает ссылку не неё в виде
///TreeItem *
//// Если ветка с указанным ID не будет найдена, возвращается NULL
// boost::intrusive_ptr<TreeItem> KnowModel::item(QString id)const
// {
// std::function<boost::intrusive_ptr<TreeItem> (boost::intrusive_ptr<TreeItem>,
// QString, int)>
// item_by_id_recurse
// = [&](boost::intrusive_ptr<TreeItem> _it, QString id, int mode) {
// static boost::intrusive_ptr<TreeItem> find_item;

// if(mode == 0) {
// find_item = nullptr;
// return find_item;
// }

// if(find_item) return find_item;

// if(_it->id() == id) {
// find_item = _it;
// return find_item;
// } else {
// for(int i = 0; i < _it->count_direct(); i++)
// item_by_id_recurse(_it->item_direct(i), id, 1);

// return find_item;
// }
// };
//// Initialize the search    // Инициализация поиска
// item_by_id_recurse(_root_item, "0", 0);

//// Запуск поиска и возврат результата
// return item_by_id_recurse(_root_item, id, 1);
// }

// boost::intrusive_ptr<TreeItem>
// KnowTreeModel::item_by_id_recurse(boost::intrusive_ptr<TreeItem> item,
// QString id, int mode)
// {
// static boost::intrusive_ptr<TreeItem> find_item;

// if(mode == 0) {
// find_item = nullptr;
// return nullptr;
// }

// if(find_item != nullptr) return find_item;

// if(item->id() == id) {
// find_item = item;
// return find_item;
// } else {
// for(int i = 0; i < item->child_count(); i++)
// item_by_id_recurse(item->child(i), id, 1);

// return find_item;
// }
// }

// Получение пути к ветке, где находится запись
QStringList tkm_t::path_list(const id_value& record_id) const
{
	std::function<QStringList(boost::intrusive_ptr<i_t>, QStringList, const id_value&, int)>
	    record_path_recurse = [&](boost::intrusive_ptr<i_t> _it, QStringList current_path, const id_value& record_id_, int mode) {
		    static QStringList found_path;
		    static bool is_found;
		    if (mode == 0) {
			    found_path.clear();
			    is_found = false;
			    // found_path	= QStringList();
		    } else if (!is_found) { // return found_path;
			    // Путь дополняется
			    current_path << _it->field<id_key>();
			    // auto r = _it->child_direct(record_id_);
			    // Если в данной ветке есть искомая запись
			    if (_it->field<id_key>() == record_id_) { // if(_it->child_direct(record_id_)){
				    found_path = current_path;
				    is_found = true;
			    } else {
				    // Иначе перебираются подветки
				    for (auto il : _it->child_linkers()) {
					    record_path_recurse(il->host(), current_path, record_id_, 1);
					    if (is_found)
						    break;
				    }
			    }
		    }
		    return found_path;
	    };

	record_path_recurse(_root_item, QStringList(), id_value(global_root_id),
	    0); // "0"

	return record_path_recurse(_root_item, QStringList(), record_id, 1);
}

// QStringList KnowTreeModel::record_path_recurse(boost::intrusive_ptr<TreeItem>
// item,
// QStringList currentPath,
// QString recordId,
// int mode)
// {
// static QStringList findPath;
// static bool isFind;

// if(mode == 0) {
// findPath.clear();
// isFind = false;
// return QStringList();
// }

// if(isFind)
// return findPath;

//// Путь дополняется
// currentPath << item->id();

//// Если в данной ветке есть искомая запись
// if(item->is_item_exists(recordId)) {
// isFind = true;
// findPath = currentPath;
// } else {
//// Иначе перебираются подветки
// for(int i = 0; i < item->child_count(); i++)
// record_path_recurse(item->child(i), currentPath, recordId, 1);
// }

// return findPath;
// }

// Метод определяющий есть ли в дереве зашифрованные ветки
bool tkm_t::is_contains_crypt_branches(void) const
{
	std::function<bool(boost::intrusive_ptr<i_t>, int)>
	    is_contains_crypt_branches_recurse =
		[&](boost::intrusive_ptr<i_t> _it, int mode) {
			static bool isCrypt = false;
			if (mode == 0) {
				isCrypt = false;

				return isCrypt;
			}
			if (_it->field<crypt_key>() == crypt_value(true)) {
				isCrypt = true;

				return isCrypt;
			}
			for (int i = 0; i < _it->count_direct(); i++)
				is_contains_crypt_branches_recurse(_it->child_direct(i), 1);
			return isCrypt;
		};

	is_contains_crypt_branches_recurse(_root_item, 0);

	return is_contains_crypt_branches_recurse(_root_item, 1);
}

// bool
// KnowTreeModel::is_contains_crypt_branches_recurse(boost::intrusive_ptr<TreeItem>
// item, int mode)
// {
// static bool isCrypt = false;

// if(mode == 0) {
// isCrypt = false;
// return isCrypt;
// }

// if(item->field(boost::mpl::c_str < crypt_type > ::value) == "1") {
// isCrypt = true;
// return isCrypt;
// }

// for(int i = 0; i < item->child_count(); i++)
// is_contains_crypt_branches_recurse(item->child(i), 1);

// return isCrypt;
// }

//// Старый вариант поиска QModelIndex по известному TreeItem закомментирован,
//// но алгоритм может пригодиться для поиска других данных

//// Получение QModelIndex по известному TreeItem
// QModelIndex knowtreemodel::get_item_index(TreeItem *item)
// {
//// Выясняется начальный QModelIndex дерева
// QModelIndex rootindex = index(0, 0);

//// Очищается флаг поиска внутри элементов
// get_item_index_recurse(rootindex, item, 0);

//// Перебираются элементы на одном уровне вложения с начальным элементом дерева
// for(int i = 0; rootindex.sibling(i, 0).isValid(); i++) {
//// qDebug() << "Sibling current " <<
///(find_object<KnowTreeView>(knowtreeview_singleton_name)->model()->data(rootindex.sibling(i,0),Qt::EditRole)).toString();

//// Перебираемый элемент проверяется на соответствие с искомым TreeItem
// if(item == static_cast<TreeItem *>(rootindex.sibling(i,
// 0).internalPointer()))
// return rootindex.sibling(i, 0);
// else {
//// Производится поиск внутри элемента
// QModelIndex idx = get_item_index_recurse(rootindex.sibling(i, 0), item, 1);

//// Если был найден элемент
// if(idx.isValid())return idx;
// }
// }

//// Если ничего небыло найдено, возвращается пустой индекс
// return QModelIndex();
// }

// QModelIndex knowtreemodel::get_item_index_recurse(QModelIndex currindex,
// TreeItem *finditem, int mode)
// {
// static QModelIndex findindex;
// static int findflag = 0;

//// Из QModelIndex можно всегда получить указатель TreeItem,
//// поэтому поиск можно вести по QModelIndex

//// Инициализация поиска
// if(mode == 0) {
// findflag = 0;
// return QModelIndex();
// }

//// qDebug() << "Recurse current " <<
///(find_object<KnowTreeView>(knowtreeview_singleton_name)->model()->data(currindex,Qt::EditRole)).toString();
//// qDebug() << "Current index have " << currindex.row() << "row";
//// qDebug() << "Find flag " << findflag;

//// Если был найден QModelIndex
// if(findflag == 1)return findindex;

// for(int i = 0; currindex.child(i, 0).isValid(); i++) {
//// Проверяется текущий элемент, не соответствует ли
//// его QModelIndex искомому TreeItem
// if(findflag == 0 &&
// finditem == static_cast<TreeItem *>(currindex.child(i, 0).internalPointer()))
// {
// findflag = 1;
// findindex = currindex.child(i, 0);
// return findindex;
// }

//// Рекурсивный вызов поиска в глубину дерева
// get_item_index_recurse(currindex.child(i, 0), finditem, 1);

//// Если был найден QModelIndex
// if(findflag == 1)return findindex;
// }

//// Сюда код доходит если на текущем уровне поиска элемент еще не найден
// return QModelIndex();
// }

void tkm_t::clear()
{
	beginResetModel();
	this->_root_item->clear(); // delete_list_items();   // =
				   // boost::intrusive_ptr<TreeItem>(new
				   // TreeItem(QMap<QString, QString>(), nullptr));
	endResetModel();
}

// if succeeded, id and name changed, deprecated, replaced by
// KnowView::setup_model(boost::intrusive_ptr<TreeItem> _item)
boost::intrusive_ptr<i_t>
tkm_t::intercept(boost::intrusive_ptr<i_t> _item)
{ // TreeIndex modelindex //
	// auto _item = modelindex.parent();
	auto absolute_source_model = [&]() {
		return static_cast<tv_t*>(static_cast<QObject* const>(this)->parent())
		    ->know_model_board();
	};
	assert(_item == absolute_source_model()->root_item() || absolute_source_model()->item([&](boost::intrusive_ptr<const i_t> it) {
		return it->id() == _item->id();
	}));
	boost::intrusive_ptr<i_t> result(nullptr);
	// QMap<QString, QString> root_data;

	//// Определяется одно поле в корневом объекте
	//// то есть на экране будет один столбец
	// root_data["id"] = "0";
	// root_data["name"] = "";
	gl_paras->main_window()->setDisabled(true);
	beginResetModel();
	// Создание корневого Item объекта
	if (_root_item)
		_root_item.reset();
	////    QMap<QString, QString> data;
	////    data["id"]      = _item->id();
	////    data["name"]    = _item->name();
	////    _root_item = boost::intrusive_ptr<TreeItem>(new TreeItem(nullptr,
	///data)); // ?

	////    //    _root_item->field("id", _item->id()); //
	///boost::intrusive_ptr<TreeItem>(new TreeItem(root_data, nullptr)); // ?
	////    //    _root_item->field("name", _item->name());

	////    //    //    if(item->id() == global_root_id) {_is_global_root = true;}
	///else {_is_global_root = false;}

	////    //    // , std::make_shared<RecordTable>(QDomElement())

	////    //    //    //    // Динамическое создание дерева из Item объектов на
	///основе DOM модели
	////    //    //    //    setup_modeldata(dom_model, _root_item);
	////    //    //    assert(dom_model);
	////    //    //    QDomElement content_root_record =
	///dom_model->documentElement().firstChildElement("content").firstChildElement("record");
	///// "node"

	////    //    //    std::shared_ptr<QDomDocument> doc =
	///std::make_shared<QDomDocument>();

	////    auto content_root_record_dom = _item->dom_from_treeitem();  //
	///std::make_shared<QDomDocument>()

	////    if(content_root_record_dom.isNull()) {
	////        qDebug() << "Unable load xml tree, first content node not found.";
	////        //        return;
	////    } else {
	////        result = _root_item;
	////    }

	////    _root_item->dom_to_records(content_root_record_dom);

	_root_item = _item;

	endResetModel();
	gl_paras->main_window()->setEnabled(true);
	////    QObject *(QObject::*_parent)() const = &QObject::parent;

	// KnowView *view = static_cast<KnowView *>(
	// static_cast<QObject *>(this)->parent() // (this->*_parent)()
	// );
	// view->reset();
	// view->source_model(_root_item);
	////    save(); // temporary
	_synchronized = false;

	return result;
}

boost::intrusive_ptr<i_t> tkm_t::synchronize(boost::intrusive_ptr<i_t> source)
{
	boost::intrusive_ptr<i_t> result(_root_item);
	result = child([=](boost::intrusive_ptr<const i_t> it) {
		return it->id() == source->id();
	});
	boost::intrusive_ptr<i_t> parent;
	if (result)
		parent = result->parent();
	// int pos = parent ? parent->list_position(result) : 0;
	gl_paras->main_window()->setDisabled(true);
	if (parent) {
		if (result)
			result.reset(); // ->clear_children();

		result = boost::intrusive_ptr<i_t>(new i_t(parent));
	}
	if (result->is_lite())
		result->to_fat();
	assert(!result->is_lite());
	assert(!result->attach_table()->is_lite());
	result->dom_to_records(
	    source->dom_from_treeitem()); // std::make_shared<QDomDocument>()
	assert(result);
	assert(!result->is_lite());
	assert(!result->attach_table()->is_lite());

	// if(parent)parent->insert_new_item(pos, result);
	gl_paras->main_window()->setEnabled(true);
	_synchronized = true;

	return result;
}

//// deprecated? for whitout recursive
// boost::intrusive_ptr<Linker>
// KnowModel::record_remove(boost::intrusive_ptr<TreeItem> _item)
// {
// assert(_item->count_direct() == 0);
// boost::intrusive_ptr<Linker> result(nullptr);
////    if(_item->count_direct() > 0) {
////        auto p = _item->parent();
////        int pos = _item->sibling_order();   // p->sibling_order(_item);
////        beginInsertRows(index(p), pos, _item->count_direct() - 1);

////        for(int i = 0; i < _item->count_direct(); i++) {
////            p->child_move(pos, _item->item_direct(i));  // ?
////        }

////        endInsertRows();
////    }

// QModelIndex _index = index(_item);

// beginRemoveRows(parent(_index), _index.row(), _index.row());

// result = _item->delete_permanent();
// assert(result == _item->up_linker());
// auto _index_origin = index(_item);

// if(_index_origin.isValid())update_index(_index_origin.parent());

// endRemoveRows();
// return result;
// }
