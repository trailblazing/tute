
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include "window_switcher.h"

#include "libraries/global_parameters.h"
#include "main.h"
#include "models/app_config/app_config.h"
#include "models/tree/tree_know_model.h"
#include "views/browser/docker.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/hidable_tab.h"

#include "views/browser/browser.h"
#include "views/record/editor_wrap.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_screen.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"
#include <QSplitter>

extern const char* record_screen_multi_instance_name;

extern std::shared_ptr<AppConfig> appconfig;
extern std::shared_ptr<gl_para> gl_paras;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(WindowSwitcher)
#endif

WindowSwitcher::WindowSwitcher(QString object_name, QObject* parent)
    : QObject(parent)
{
	setObjectName(object_name);
	enable();

	// едактор является встраиваемым, поэтому работа кнопки Back у него идет через
	// callback функцию
	// MetaEditor *edView=find_object<MetaEditor>(meta_editor_singleton_name); //
	// Выясняется указатель на объект редактирования текста записи

	// MetaEditor *metaEditor = globalparameters.getMetaEditor();
	//	editor_docker->back_callback(&WindowSwitcher::record_to_recordtable); //
	//Устанавливается функция обратного вызова при клике на кнопку Back
}

void WindowSwitcher::enable(void)
{
	// Для десктопа переключение окон виджетов вообще не должно включаться
	if (appconfig->interface_mode() == "desktop") {
		enableSwitcher = false;

		return;
	}
	enableSwitcher = true;
}

void WindowSwitcher::disable(void)
{
	enableSwitcher = false;
}

bool WindowSwitcher::status(void)
{
	return enableSwitcher;
}

void WindowSwitcher::tree_screen_to_record_screen(void)
{
	if (!enableSwitcher)
		return; // on desktop, default false

	// Скрываются все прочие области
	gl_paras->tree_screen()->hide();
	gl_paras->editor_docker()->hide();
	// globalParameters.getFindScreen()->hide();

	auto _index = gl_paras->tree_screen()->view()->current_index();
	if (_index.isValid()) {
		auto item = gl_paras->tree_screen()->view()->source_model()->child(_index);
		if (item != gl_paras->tree_screen()->view()->source_model()->root_item()) {
			// auto	view		=
			// globalparameters.entrance()->find([&](boost::intrusive_ptr<const
			// Binder> b){return b == item->binder();});
			auto _record_screen = item->page()->browser()->record_screen();
			if (_record_screen) {
				//				// item->page()->view()->record_ctrl()->tabmanager()->browser()->record_screen();
				//				QWidget* object = static_cast<QWidget*>(_record_screen); // globalparameters.record_screens()[0]
				//				// temporary setting
				//				object
				_record_screen->show();
				appconfig->focus_widget( //object
				    _record_screen->objectName());

				gl_paras->vtab_record()->setCurrentWidget(_record_screen); // globalparameters.record_screens()[0]
											   // temporary setting
			}
		}
	}
}

void WindowSwitcher::tree_screen_to_find_in_base(void)
{
	if (!enableSwitcher)
		return;
}

// Статическая функция, используется редактором как callback функция при нажатии
// кнопки back в редакторе конечной записи
void WindowSwitcher::record_to_record_screen(void)
{
	if (gl_paras->window_switcher() == nullptr)
		return;
	// Если переключение запрещено
	if (!gl_paras->window_switcher()->status())
		return;
	if (appconfig->interface_mode() != "mobile")
		return; // В статическом методе использовать нестатическую переменну
			// enableSwitcher нельзя
	auto _tree_screen = gl_paras->tree_screen();
	if (_tree_screen) {

		// Скрываются все прочие области
		_tree_screen->hide();
		gl_paras->editor_docker()->hide();
		// globalParameters.getFindScreen()->hide();
		auto _tree_view = _tree_screen->view();
		if (_tree_view) {
			auto _index = _tree_view->current_index();
			if (_index.isValid()) {
				auto item = _tree_view->source_model()->child(_index);
				if (item != _tree_view->source_model()->root_item()) {
					auto _record_screen = item->page()->browser()->record_screen();
					if (_record_screen) {
						//						QWidget* object = static_cast<QWidget*>(_record_screen); // temporary setting
						//						// // globalparameters.record_screens()[0]
						//						object
						_record_screen->show();
						gl_paras->vtab_record()->setCurrentWidget(_record_screen); // temporary setting		//
													   // globalparameters.record_screens()[0]
						appconfig->focus_widget(                                   //object
						    _record_screen->objectName());
					}
				}
			}
		}
	}
}

void WindowSwitcher::record_to_find_in_base(void)
{
	if (!enableSwitcher)
		return;
}

void WindowSwitcher::record_screen_to_editor_docker(void)
{
	if (!enableSwitcher) // on desktop, default false
		return;
	// Скрываются все прочие области
	gl_paras->tree_screen()->hide();
	for (auto& i : gl_paras->record_screens())
		i->hide();
	// globalParameters.getFindScreen()->hide();

	//	QWidget* object = static_cast<QWidget*>(gl_paras->editor_docker());
	//	object
	gl_paras->editor_docker()->show();
	appconfig->focus_widget( //object
	    gl_paras->editor_docker()->objectName());
}

void WindowSwitcher::record_screen_to_find_in_base(void)
{
	if (!enableSwitcher)
		return;
}

void WindowSwitcher::record_screen_to_tree_screen(void)
{
	if (!enableSwitcher)
		return;
	// Скрываются все прочие области
	gl_paras->editor_docker()->hide();
	for (auto& i : gl_paras->record_screens())
		i->hide();
	// globalParameters.getFindScreen()->hide();

	//	QWidget* object = static_cast<QWidget*>(gl_paras->tree_screen());
	//	object
	gl_paras->tree_screen()->show();
	appconfig->focus_widget( //object
	    gl_paras->tree_screen()->objectName());
}

// Закрытие окна FindInBase с переходом на окно, откуда оно было открыто
void WindowSwitcher::close_find_in_base(void)
{
	if (!enableSwitcher)
		return;
}

// Слот, срабатывающий при нажатии на кнопку перехода к поиску по базе в любом
// виджете
void WindowSwitcher::find_in_base_click(void)
{
	// Определяется ссылка на виджет поиска
	FindScreen* _find_screen = gl_paras->find_screen();          // find_object<FindScreen>(find_screen_singleton_name);
	QSplitter* _v_right_splitter = gl_paras->v_right_splitter(); // find_object<QSplitter>("v_right_splitter");
	// web::Entrance *entrance = globalparameters.entrance();
	if (_find_screen && _v_right_splitter) {
		// auto dp = entrance->activiated_registered();
		int height = _find_screen->height();
		QSize right_size = _v_right_splitter->size();
		// Если виджет не показан, он выводится на экран, и наоборот
		if (_find_screen->isVisible() == false) {
			_find_screen->widget_show();
			_v_right_splitter->resize(
			    QSize(right_size.width(),
				_v_right_splitter->height() - height)); // dp.first->resize(entrance->size()); // ?
									// dp.second->resize(dp.first->size());
		} else {
			_find_screen->widget_hide();
			_v_right_splitter->resize(
			    QSize(right_size.width(),
				_v_right_splitter->height() + height)); // dp.first->resize(entrance->size()); //
									// dp.second->resize(dp.first->size());
		}
	}
}

void WindowSwitcher::restore_focus_widget()
{
	if (!enableSwitcher)
		return;
	// Скрываются все прочие области
	gl_paras->tree_screen()->hide();
	for (auto& i : gl_paras->record_screens())
		i->hide();
	gl_paras->editor_docker()->hide();
	// globalParameters.getFindScreen()->hide();

	QString widgetName = appconfig->focus_widget();
	// Оформить через case
	if (widgetName == tree_screen_singleton_name) {
		gl_paras->tree_screen()->show();
		gl_paras->tree_screen()->setFocus();

		return;
	}
	if (widgetName == record_screen_multi_instance_name) {
		auto _index = gl_paras->tree_screen()->view()->current_index();
		if (_index.isValid()) {
			auto item =
			    gl_paras->tree_screen()->view()->source_model()->child(_index);
			if (item !=
			    gl_paras->tree_screen()->view()->source_model()->root_item()) {
				auto _record_screen = item->page()->browser()->record_screen();
				if (_record_screen) {
					QWidget* object = static_cast<QWidget*>(_record_screen); // globalparameters.record_screens()[0]
					object->show();
					gl_paras->vtab_record()->setCurrentWidget(_record_screen); // globalparameters.record_screens()[0]
					appconfig->focus_widget(object->objectName());
					// for(auto &i : globalparameters.record_screens())i->show();
					// for(auto &i : globalparameters.record_screens())i->setFocus(); //
				}
			}
		}
		return;
	}
	if (widgetName == meta_editor_singleton_name) {
		gl_paras->editor_docker()->show();
		gl_paras->editor_docker()->setFocus();

		return;
	}
	// Доработать
	if (widgetName == "findTableView") {
		gl_paras->find_screen()->show();
		gl_paras->find_screen()->setFocus();

		return;
	}
	// Остальные случаи - активируется дерево записей
	gl_paras->tree_screen()->show();
	gl_paras->tree_screen()->setFocus();
}
