#include "WindowSwitcher.h"

#include <QSplitter>
#include "main.h"
#include "models/app_config/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/record_table/TableScreen.h"
#include "views/find_in_base_screen/FindScreen.h"
#include "views/browser/entrance.h"
#include "views/record/MetaEditor.h"

extern AppConfig appconfig;
extern GlobalParameters globalparameters;


WindowSwitcher::WindowSwitcher(QString object_name, MetaEditor *meta_editor, QObject *parent) : QObject(parent)
{
    setObjectName(object_name);
    enableSwitch();

    // Редактор является встраиваемым, поэтому работа кнопки Back у него идет через callback функцию
    // MetaEditor *edView=find_object<MetaEditor>("editor_screen"); // Выясняется указатель на объект редактирования текста записи

    //    MetaEditor *metaEditor = globalparameters.getMetaEditor();
    meta_editor->set_back_callback(this->switchFromRecordToRecordtable);   // Устанавливается функция обратного вызова при клике на кнопку Back
}


void WindowSwitcher::enableSwitch(void)
{
    // Для десктопа переключение окон виджетов вообще не должно включаться
    if(appconfig.getInterfaceMode() == "desktop") {
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


void WindowSwitcher::switchFromTreeToRecordtable(void)
{
    if(!enableSwitcher) // on desktop, default false
        return;

    // Скрываются все прочие области
    globalparameters.tree_screen()->hide();
    globalparameters.meta_editor()->hide();
    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.table_screen());
    object->show();
    appconfig.setFocusWidget(object->objectName());

    globalparameters.vtab()->setCurrentWidget(globalparameters.table_screen());
}


void WindowSwitcher::switchFromTreeToFindInBase(void)
{
    if(!enableSwitcher)
        return;

}


// Статическая функция, используется редактором как callback функция при нажатии кнопки back в редакторе конечной записи
void WindowSwitcher::switchFromRecordToRecordtable(void)
{
    if(globalparameters.window_switcher() == NULL)
        return;

    // Если переключение запрещено
    if(!globalparameters.window_switcher()->getSwitchStatus())
        return;

    if(appconfig.getInterfaceMode() != "mobile") // В статическом методе использовать нестатическую переменну enableSwitcher нельзя
        return;

    // Скрываются все прочие области
    globalparameters.tree_screen()->hide();
    globalparameters.meta_editor()->hide();
    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.table_screen());
    object->show();
    appconfig.setFocusWidget(object->objectName());
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
    globalparameters.table_screen()->hide();
    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.meta_editor());
    object->show();
    appconfig.setFocusWidget(object->objectName());
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
    globalparameters.table_screen()->hide();
    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.tree_screen());
    object->show();
    appconfig.setFocusWidget(object->objectName());
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
    FindScreen *findScreen = find_object<FindScreen>("find_screen");
    QSplitter *vrs = find_object<QSplitter>("v_right_splitter");
    //    browser::Entrance *entrance = globalparameters.entrance();

    if(findScreen && vrs) {
        //        auto dp = entrance->activiated_registered();
        int height = findScreen->height();
        QSize vrs_size = vrs->size();

        // Если виджет не показан, он выводится на экран, и наоборот
        if(findScreen->isVisible() == false) {
            findScreen->widgetShow();
            vrs->resize(QSize(vrs_size.width(), vrs->height() - height)); //            dp.first->resize(entrance->size()); // ? dp.second->resize(dp.first->size());
        } else {
            findScreen->widgetHide();
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
    globalparameters.table_screen()->hide();
    globalparameters.meta_editor()->hide();
    // globalParameters.getFindScreen()->hide();

    QString widgetName = appconfig.getFocusWidget();

    // Оформить через case
    if(widgetName == "tree_screen") {
        globalparameters.tree_screen()->show();
        globalparameters.tree_screen()->setFocus();
        return;
    }

    if(widgetName == "table_screen") {
        globalparameters.table_screen()->show();
        globalparameters.table_screen()->setFocus();
        return;
    }

    if(widgetName == "editor_screen") {
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
