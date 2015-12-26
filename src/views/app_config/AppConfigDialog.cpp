#include <QStackedWidget>
#include <QDialog>
#include <QDebug>

#include "main.h"
#include "ConfigDialog.h"
#include "AppConfigDialog.h"
#include "AppConfigPageMain.h"
#include "AppConfigPageCrypt.h"
#include "AppConfigPageMisc.h"
#include "AppConfigPageSynchro.h"
#include "AppConfigPageTable.h"
#include "models/app_config/AppConfig.h"

extern AppConfig appconfig;


AppConfigDialog::AppConfigDialog(QString firstPageName = "") : QWidget()
{
    if(appconfig.getInterfaceMode() == "mobile") { // if(true)
        qDebug() << "Screen size X Y: " << getScreenSizeX() << getScreenSizeY();
        this->setMinimumSize(getScreenSizeX(), getScreenSizeY());
        this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    }

    configDialog = new ConfigDialog();
    configDialog->set_window_title(tr("MyTetra settings"));

    pageMain        = configDialog->add_widget(new AppConfigPageMain(this),        tr("Main"));
    pageCrypt       = configDialog->add_widget(new AppConfigPageCrypt(this),       tr("Crypt"));
    pageSynchro     = configDialog->add_widget(new AppConfigPageSynchro(this),     tr("Synchro"));
    pageRecordTable = configDialog->add_widget(new AppConfigPageTable(this), tr("Note area"));
    pageMisc        = configDialog->add_widget(new AppConfigPageMisc(this),        tr("Misc"));

    configDialog->updateListWidth();

    if(firstPageName.size() > 0)
        changePage(firstPageName);

    configDialog->exec();
}


// Переход на нужную страницу настроек (переход совершается кодом извне)
void AppConfigDialog::changePage(QString name)
{

    QListWidgetItem *item = NULL;

    if(name == "pageMain")          item = pageMain;

    if(name == "pageCrypt")         item = pageCrypt;

    if(name == "pageSynchro")       item = pageSynchro;

    if(name == "pageRecordTable")   item = pageRecordTable;

    if(name == "pageMisc")          item = pageMisc;

    if(item != NULL)
        configDialog->externalChangePage(item);
    else
        qDebug() << "AppConfigDialog::changePage cant find item for name: " << name;
}
