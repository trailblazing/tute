
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include <QStackedWidget>
#include <QDialog>
#include <QDebug>

#include "main.h"
#include "config_dialog.h"
#include "app_config_dialog.h"
#include "app_config_page_main.h"
#include "app_config_page_crypt.h"
#include "app_config_page_misc.h"
#include "app_config_page_synchro.h"
#include "app_config_page_table.h"
#include "models/app_config/app_config.h"
#include "libraries/disk_helper.h"


extern AppConfig appconfig;


#if QT_VERSION == 0x050600
W_OBJECT_IMPL(AppConfigDialog)
#endif

AppConfigDialog::AppConfigDialog(rctrl_t *_record_controller, QString firstPageName) : QWidget(), _record_controller(_record_controller){
    if(appconfig.interface_mode() == "mobile"){	// if(true)
	qDebug() << "Screen size X Y: " << screen_size_x() << screen_size_y();
	this->setMinimumSize(screen_size_x(), screen_size_y());
	this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    }
    configDialog = new ConfigDialog(this);
    configDialog->set_window_title(tr("MyTetra settings"));

    pageMain		= configDialog->add_widget(new AppConfigPageMain(this), tr("Main"));
    pageCrypt		= configDialog->add_widget(new AppConfigPageCrypt(this), tr("Crypt"));
    pageSynchro		= configDialog->add_widget(new AppConfigPageSynchro(this), tr("Synchro"));
    pageRecordTable	= configDialog->add_widget(new AppConfigPageTable(_record_controller, this), tr("Note area"));
    pageMisc		= configDialog->add_widget(new AppConfigPageMisc(this), tr("Misc"));

    configDialog->updateListWidth();
    if(firstPageName.size() > 0)changePage(firstPageName);
    configDialog->exec();
}

// Переход на нужную страницу настроек (переход совершается кодом извне)
void AppConfigDialog::changePage(QString name){
    QListWidgetItem *item = nullptr;
    if(name == "pageMain")item = pageMain;
    if(name == "pageCrypt")item = pageCrypt;
    if(name == "pageSynchro")item = pageSynchro;
    if(name == "pageRecordTable")item = pageRecordTable;
    if(name == "pageMisc")item = pageMisc;
    if(item != nullptr)configDialog->externalChangePage(item);
    else qDebug() << "AppConfigDialog::changePage cant find item for name: " << name;
}

