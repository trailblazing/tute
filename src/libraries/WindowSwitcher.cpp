#include "WindowSwitcher.h"

#include "main.h"
#include "models/appConfig/AppConfig.h"
#include "libraries/GlobalParameters.h"
#include "views/tree/TreeScreen.h"
#include "views/record/MetaEditor.h"
#include "views/recordTable/RecordTableScreen.h"
#include "views/findInBaseScreen/FindScreen.h"

extern AppConfig appconfig;
extern GlobalParameters globalparameters;


WindowSwitcher::WindowSwitcher(QObject *parent) : QObject(parent)
{
    enableSwitch();

    // Редактор является встраиваемым, поэтому работа кнопки Back у него идет через callback функцию
    // MetaEditor *edView=find_object<MetaEditor>("editorScreen"); // Выясняется указатель на объект редактирования текста записи
    MetaEditor *metaEditor = globalparameters.getMetaEditor();
    metaEditor->set_back_callback(this->switchFromRecordToRecordtable);   // Устанавливается функция обратного вызова при клике на кнопку Back
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
    if(!enableSwitcher)
        return;

    // Скрываются все прочие области
    globalparameters.getTreeScreen()->hide();
    globalparameters.getMetaEditor()->hide();
    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.getRecordTableScreen());
    object->show();
    appconfig.setFocusWidget(object->objectName());
}


void WindowSwitcher::switchFromTreeToFindInBase(void)
{
    if(!enableSwitcher)
        return;

}


// Статическая функция, используется редактором как callback функция при нажатии кнопки back в редакторе конечной записи
void WindowSwitcher::switchFromRecordToRecordtable(void)
{
    if(globalparameters.getWindowSwitcher() == NULL)
        return;

    // Если переключение запрещено
    if(!globalparameters.getWindowSwitcher()->getSwitchStatus())
        return;

    if(appconfig.getInterfaceMode() != "mobile") // В статическом методе использовать нестатическую переменну enableSwitcher нельзя
        return;

    // Скрываются все прочие области
    globalparameters.getTreeScreen()->hide();
    globalparameters.getMetaEditor()->hide();
    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.getRecordTableScreen());
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
    if(!enableSwitcher)
        return;

    // Скрываются все прочие области
    globalparameters.getTreeScreen()->hide();
    globalparameters.getRecordTableScreen()->hide();
    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.getMetaEditor());
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
    globalparameters.getMetaEditor()->hide();
    globalparameters.getRecordTableScreen()->hide();
    // globalParameters.getFindScreen()->hide();

    QWidget *object = static_cast<QWidget *>(globalparameters.getTreeScreen());
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
    FindScreen *findScreen = find_object<FindScreen>("findScreenDisp");

    // Если виджет не показан, он выводится на экран, и наоборот
    if(findScreen->isVisible() == false)
        findScreen->widgetShow();
    else
        findScreen->widgetHide();
}


void WindowSwitcher::restoreFocusWidget()
{
    if(!enableSwitcher)
        return;

    // Скрываются все прочие области
    globalparameters.getTreeScreen()->hide();
    globalparameters.getRecordTableScreen()->hide();
    globalparameters.getMetaEditor()->hide();
    // globalParameters.getFindScreen()->hide();

    QString widgetName = appconfig.getFocusWidget();

    // Оформить через case
    if(widgetName == "treeScreen") {
        globalparameters.getTreeScreen()->show();
        globalparameters.getTreeScreen()->setFocus();
        return;
    }

    if(widgetName == "recordTableScreen") {
        globalparameters.getRecordTableScreen()->show();
        globalparameters.getRecordTableScreen()->setFocus();
        return;
    }

    if(widgetName == "editorScreen") {
        globalparameters.getMetaEditor()->show();
        globalparameters.getMetaEditor()->setFocus();
        return;
    }

    // Доработать
    if(widgetName == "findTableView") {
        globalparameters.getFindScreen()->show();
        globalparameters.getFindScreen()->setFocus();
        return;
    }


    // Остальные случаи - активируется дерево записей
    globalparameters.getTreeScreen()->show();
    globalparameters.getTreeScreen()->setFocus();
}
