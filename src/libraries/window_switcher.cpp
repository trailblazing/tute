
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include "window_switcher.h"

#include <QSplitter>
#include "main.h"
#include "models/app_config/app_config.h"
#include "libraries/global_parameters.h"
#include "views/tree/tree_screen.h"
#include "views/record/meta_editor.h"
#include "views/record_table/record_screen.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/browser/entrance.h"
#include "views/record/meta_editor.h"
#include "views/browser/tabwidget.h"
#include "views/tree/tree_view.h"
#include "models/tree/tree_know_model.h"
#include "views/main_window/hidable_tabwidget.h"


extern const char *record_screen_multi_instance_name;

extern AppConfig	appconfig;
extern GlobalParameters globalparameters;


#if QT_VERSION == 0x050600
W_OBJECT_IMPL(WindowSwitcher)
#endif


WindowSwitcher::WindowSwitcher(QString object_name, MetaEditor *meta_editor, QObject *parent) : QObject(parent){
    setObjectName(object_name);
    enable();

	// Редактор является встраиваемым, поэтому работа кнопки Back у него идет через callback функцию
	// MetaEditor *edView=find_object<MetaEditor>(meta_editor_singleton_name); // Выясняется указатель на объект редактирования текста записи

	//    MetaEditor *metaEditor = globalparameters.getMetaEditor();
    meta_editor->back_callback(this->record_to_recordtable);	// Устанавливается функция обратного вызова при клике на кнопку Back
}

void WindowSwitcher::enable(void){
	// Для десктопа переключение окон виджетов вообще не должно включаться
    if(appconfig.interface_mode() == "desktop"){
	enableSwitcher = false;

	return;
    }
    enableSwitcher = true;
}

void WindowSwitcher::disable(void){enableSwitcher = false;}

bool WindowSwitcher::status(void){return enableSwitcher;}

void WindowSwitcher::tree_to_record_screen(void){
    if(! enableSwitcher)return;	// on desktop, default false

	// Скрываются все прочие области
    globalparameters.tree_screen()->hide();
    globalparameters.meta_editor()->hide();
	// globalParameters.getFindScreen()->hide();

    auto _index = globalparameters.tree_screen()->view()->current_index();
    if(_index.isValid()){
	auto item = globalparameters.tree_screen()->view()->source_model()->child(_index);
	if(item != globalparameters.tree_screen()->view()->source_model()->root_item()){
//	    auto	view		= globalparameters.entrance()->find([&](boost::intrusive_ptr<const Binder> b){return b == item->binder();});
	    rs_t	*_record_screen = item->page()->tabmanager()->browser()->record_screen();
	    ;		// item->page()->view()->record_controller()->tabmanager()->browser()->record_screen();
	    QWidget	*object	= static_cast<QWidget *>(_record_screen);				// globalparameters.record_screens()[0]
		// temporary setting
	    object->show();
	    appconfig.focus_widget(object->objectName());

	    globalparameters.vtab_record()->setCurrentWidget(_record_screen);		// globalparameters.record_screens()[0]
		// temporary setting
	}
    }
}

void WindowSwitcher::tree_to_find_in_base(void){if(! enableSwitcher)return;}

// Статическая функция, используется редактором как callback функция при нажатии кнопки back в редакторе конечной записи
void WindowSwitcher::record_to_recordtable(void){
    if(globalparameters.window_switcher() == nullptr)return;
	// Если переключение запрещено
    if(! globalparameters.window_switcher()->status())return;
    if(appconfig.interface_mode() != "mobile")	// В статическом методе использовать нестатическую переменну enableSwitcher нельзя
		return;
	// Скрываются все прочие области
    globalparameters.tree_screen()->hide();
    globalparameters.meta_editor()->hide();
	// globalParameters.getFindScreen()->hide();
    auto _index = globalparameters.tree_screen()->view()->current_index();
    if(_index.isValid()){
	auto item = globalparameters.tree_screen()->view()->source_model()->child(_index);
	if(item != globalparameters.tree_screen()->view()->source_model()->root_item()){
	    rs_t	*_record_screen = item->page()->tabmanager()->browser()->record_screen();
	    QWidget	*object		= static_cast<QWidget *>(_record_screen);	// temporary setting					// globalparameters.record_screens()[0]

	    object->show();

	    globalparameters.vtab_record()->setCurrentWidget(_record_screen);	// temporary setting		// globalparameters.record_screens()[0]

	    appconfig.focus_widget(object->objectName());
	}
    }
}

void WindowSwitcher::record_to_find_in_base(void){if(! enableSwitcher)return;}

void WindowSwitcher::recordtable_ro_record_editor(void){
    if(! enableSwitcher)// on desktop, default false
		return;
	// Скрываются все прочие области
    globalparameters.tree_screen()->hide();
    for(auto &i : globalparameters.record_screens())i->hide();
	// globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.meta_editor());
    object->show();
    appconfig.focus_widget(object->objectName());
}

void WindowSwitcher::recordtable_to_find_in_base(void){if(! enableSwitcher)return;}

void WindowSwitcher::recordtable_to_tree(void){
    if(! enableSwitcher)return;
	// Скрываются все прочие области
    globalparameters.meta_editor()->hide();
    for(auto &i : globalparameters.record_screens())i->hide();
	// globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.tree_screen());
    object->show();
    appconfig.focus_widget(object->objectName());
}

// Закрытие окна FindInBase с переходом на окно, откуда оно было открыто
void WindowSwitcher::close_find_in_base(void){if(! enableSwitcher)return;}

// Слот, срабатывающий при нажатии на кнопку перехода к поиску по базе в любом виджете
void WindowSwitcher::find_in_base_click(void){
	// Определяется ссылка на виджет поиска
    FindScreen	*_find_screen		= globalparameters.find_screen();	// find_object<FindScreen>(find_screen_singleton_name);
    QSplitter	*_v_right_splitter	= globalparameters.v_right_splitter();		// find_object<QSplitter>("v_right_splitter");
	//    browser::Entrance *entrance = globalparameters.entrance();
    if(_find_screen && _v_right_splitter){
	//        auto dp = entrance->activiated_registered();
	int	height		= _find_screen->height();
	QSize	right_size	= _v_right_splitter->size();
	// Если виджет не показан, он выводится на экран, и наоборот
	if(_find_screen->isVisible() == false){
	    _find_screen->widget_show();
	    _v_right_splitter->resize(QSize(right_size.width(), _v_right_splitter->height() - height));	//            dp.first->resize(entrance->size()); // ? dp.second->resize(dp.first->size());
	}else{
	    _find_screen->widget_hide();
	    _v_right_splitter->resize(QSize(right_size.width(), _v_right_splitter->height() + height));	//            dp.first->resize(entrance->size()); // dp.second->resize(dp.first->size());
	}
    }
}

void WindowSwitcher::restore_focus_widget(){
    if(! enableSwitcher)return;
	// Скрываются все прочие области
    globalparameters.tree_screen()->hide();
    for(auto &i : globalparameters.record_screens())i->hide();
    globalparameters.meta_editor()->hide();
	// globalParameters.getFindScreen()->hide();

    QString widgetName = appconfig.focus_widget();
	// Оформить через case
    if(widgetName == tree_screen_singleton_name){
	globalparameters.tree_screen()->show();
	globalparameters.tree_screen()->setFocus();

	return;
    }
    if(widgetName == record_screen_multi_instance_name){
	auto _index = globalparameters.tree_screen()->view()->current_index();
	if(_index.isValid()){
	    auto item = globalparameters.tree_screen()->view()->source_model()->child(_index);
	    if(item != globalparameters.tree_screen()->view()->source_model()->root_item()){
		rs_t *_record_screen = item->page()->tabmanager()->browser()->record_screen();


		QWidget *object = static_cast<QWidget *>(_record_screen);		// globalparameters.record_screens()[0]
		// temporary setting
		object->show();

		globalparameters.vtab_record()->setCurrentWidget(_record_screen);		// globalparameters.record_screens()[0]
		// temporary setting

		appconfig.focus_widget(object->objectName());

		//        for(auto &i : globalparameters.record_screens())i->show();

		//        for(auto &i : globalparameters.record_screens())i->setFocus(); // temporary setting
	    }
	}
	return;
    }
    if(widgetName == meta_editor_singleton_name){
	globalparameters.meta_editor()->show();
	globalparameters.meta_editor()->setFocus();

	return;
    }
	// Доработать
    if(widgetName == "findTableView"){
	globalparameters.find_screen()->show();
	globalparameters.find_screen()->setFocus();

	return;
    }
	// Остальные случаи - активируется дерево записей
    globalparameters.tree_screen()->show();
    globalparameters.tree_screen()->setFocus();
}

