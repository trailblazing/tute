#include "WindowSwitcher.h"

#include <QSplitter>
#include "main.h"
#include "models/app_config/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/record_table/RecordScreen.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "views/browser/entrance.h"
#include "views/record/MetaEditor.h"
#include "views/browser/tabwidget.h"
#include "views/tree/KnowView.h"
#include "models/tree/KnowModel.h"

extern const char *record_screen_multi_instance_name;

extern AppConfig appconfig;
extern GlobalParameters globalparameters;


WindowSwitcher::WindowSwitcher(QString object_name, MetaEditor *meta_editor, QObject *parent) : QObject(parent)
{
    setObjectName(object_name);
    enableSwitch();

    // Редактор является встраиваемым, поэтому работа кнопки Back у него идет через callback функцию
    // MetaEditor *edView=find_object<MetaEditor>(meta_editor_singleton_name); // Выясняется указатель на объект редактирования текста записи

    //    MetaEditor *metaEditor = globalparameters.getMetaEditor();
    meta_editor->back_callback(this->switchFromRecordToRecordtable);   // Устанавливается функция обратного вызова при клике на кнопку Back
}


void WindowSwitcher::enableSwitch(void)
{
    // Для десктопа переключение окон виджетов вообще не должно включаться
    if(appconfig.interface_mode() == "desktop") {
        enableSwitcher = false;
        return;
    }

    enableSwitcher = true;
}


void WindowSwitcher::disableSwitch(void)
{
    enableSwitcher = false;
}


bool WindowSwitcher::getSwitchStatus(void)
{
    return enableSwitcher;
}


void WindowSwitcher::switch_from_tree_to_record_screen(void)
{
    if(!enableSwitcher) // on desktop, default false
        return;

    // Скрываются все прочие области
    globalparameters.tree_screen()->hide();
    globalparameters.meta_editor()->hide();
    // globalParameters.getFindScreen()->hide();

    auto _index = globalparameters.tree_screen()->tree_view()->current_index();

    if(_index.isValid()) {
        auto item = globalparameters.tree_screen()->tree_view()->source_model()->item(_index);

        if(item != globalparameters.tree_screen()->tree_view()->source_model()->root_item()) {
            RecordScreen *record_screen = item->page()->view()->record_controller()->tabmanager()->browser()->record_screen();
            QWidget *object = static_cast<QWidget *>(record_screen);                           // globalparameters.record_screens()[0]
            // temporary setting
            object->show();
            appconfig.focus_widget(object->objectName());

            globalparameters.vtab()->setCurrentWidget(record_screen);         // globalparameters.record_screens()[0]
            // temporary setting
        }
    }
}


void WindowSwitcher::switchFromTreeToFindInBase(void)
{
    if(!enableSwitcher)
        return;

}


// Статическая функция, используется редактором как callback функция при нажатии кнопки back в редакторе конечной записи
void WindowSwitcher::switchFromRecordToRecordtable(void)
{
    if(globalparameters.window_switcher() == nullptr)
        return;

    // Если переключение запрещено
    if(!globalparameters.window_switcher()->getSwitchStatus())
        return;

    if(appconfig.interface_mode() != "mobile") // В статическом методе использовать нестатическую переменну enableSwitcher нельзя
        return;

    // Скрываются все прочие области
    globalparameters.tree_screen()->hide();
    globalparameters.meta_editor()->hide();
    // globalParameters.getFindScreen()->hide();
    auto _index = globalparameters.tree_screen()->tree_view()->current_index();

    if(_index.isValid()) {
        auto item = globalparameters.tree_screen()->tree_view()->source_model()->item(_index);

        if(item != globalparameters.tree_screen()->tree_view()->source_model()->root_item()) {
            RecordScreen *record_screen = item->page()->view()->record_controller()->tabmanager()->browser()->record_screen();
            QWidget *object = static_cast<QWidget *>(record_screen                           // globalparameters.record_screens()[0]
                                                    ); // temporary setting
            object->show();

            globalparameters.vtab()->setCurrentWidget(record_screen);         // globalparameters.record_screens()[0]
            // temporary setting

            appconfig.focus_widget(object->objectName());
        }
    }
}


void WindowSwitcher::switchFromRecordToFindInBase(void)
{
    if(!enableSwitcher)
        return;

}


void WindowSwitcher::switchFromRecordtableToRecord(void)
{
    if(!enableSwitcher) // on desktop, default false
        return;

    // Скрываются все прочие области
    globalparameters.tree_screen()->hide();

    for(auto &i : globalparameters.record_screens())i->hide();

    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.meta_editor());
    object->show();
    appconfig.focus_widget(object->objectName());
}


void WindowSwitcher::switchFromRecordtableToFindInBase(void)
{
    if(!enableSwitcher)
        return;

}


void WindowSwitcher::switchFromRecordtableToTree(void)
{
    if(!enableSwitcher)
        return;

    // Скрываются все прочие области
    globalparameters.meta_editor()->hide();

    for(auto &i : globalparameters.record_screens())i->hide();

    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.tree_screen());
    object->show();
    appconfig.focus_widget(object->objectName());
}


// Закрытие окна FindInBase с переходом на окно, откуда оно было открыто
void WindowSwitcher::closeFindInBase(void)
{
    if(!enableSwitcher)
        return;

}


// Слот, срабатывающий при нажатии на кнопку перехода к поиску по базе в любом виджете
void WindowSwitcher::findInBaseClick(void)
{
    // Определяется ссылка на виджет поиска
    FindScreen *findScreen = globalparameters.find_screen();    // find_object<FindScreen>(find_screen_singleton_name);
    QSplitter *vrs = globalparameters.v_right_splitter();        // find_object<QSplitter>("v_right_splitter");
    //    browser::Entrance *entrance = globalparameters.entrance();

    if(findScreen && vrs) {
        //        auto dp = entrance->activiated_registered();
        int height = findScreen->height();
        QSize vrs_size = vrs->size();

        // Если виджет не показан, он выводится на экран, и наоборот
        if(findScreen->isVisible() == false) {
            findScreen->widget_show();
            vrs->resize(QSize(vrs_size.width(), vrs->height() - height)); //            dp.first->resize(entrance->size()); // ? dp.second->resize(dp.first->size());
        } else {
            findScreen->widget_hide();
            vrs->resize(QSize(vrs_size.width(), vrs->height() + height));  //            dp.first->resize(entrance->size()); // dp.second->resize(dp.first->size());
        }
    }
}


void WindowSwitcher::restoreFocusWidget()
{
    if(!enableSwitcher)
        return;

    // Скрываются все прочие области
    globalparameters.tree_screen()->hide();

    for(auto &i : globalparameters.record_screens())i->hide();

    globalparameters.meta_editor()->hide();
    // globalParameters.getFindScreen()->hide();

    QString widgetName = appconfig.focus_widget();

    // Оформить через case
    if(widgetName == tree_screen_singleton_name) {
        globalparameters.tree_screen()->show();
        globalparameters.tree_screen()->setFocus();
        return;
    }

    if(widgetName == record_screen_multi_instance_name) {
        auto _index = globalparameters.tree_screen()->tree_view()->current_index();

        if(_index.isValid()) {
            auto item = globalparameters.tree_screen()->tree_view()->source_model()->item(_index);

            if(item != globalparameters.tree_screen()->tree_view()->source_model()->root_item()) {
                RecordScreen *_record_screen = item->page()->view()->record_controller()->tabmanager()->browser()->record_screen();


                QWidget *object = static_cast<QWidget *>(_record_screen);              // globalparameters.record_screens()[0]
                // temporary setting
                object->show();

                globalparameters.vtab()->setCurrentWidget(_record_screen);             // globalparameters.record_screens()[0]
                // temporary setting

                appconfig.focus_widget(object->objectName());

                //        for(auto &i : globalparameters.record_screens())i->show();

                //        for(auto &i : globalparameters.record_screens())i->setFocus(); // temporary setting
            }
        }

        return;
    }

    if(widgetName == meta_editor_singleton_name) {
        globalparameters.meta_editor()->show();
        globalparameters.meta_editor()->setFocus();
        return;
    }

    // Доработать
    if(widgetName == "findTableView") {
        globalparameters.find_screen()->show();
        globalparameters.find_screen()->setFocus();
        return;
    }


    // Остальные случаи - активируется дерево записей
    globalparameters.tree_screen()->show();
    globalparameters.tree_screen()->setFocus();
}
