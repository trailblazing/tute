
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QDebug>
#include <QDialog>
#include <QStackedWidget>

#include "app_config_dialog.h"
#include "app_config_page_crypt.h"
#include "app_config_page_main.h"
#include "app_config_page_misc.h"
#include "app_config_page_synchro.h"
#include "app_config_page_table.h"
#include "config_dialog.h"
#include "libraries/disk_helper.h"
#include "main.h"
#include "models/app_config/app_config.h"

extern std::shared_ptr<AppConfig> appconfig;
extern const char* program_title;
extern const QString program_title_qstring;
extern const std::string program_title_string;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(AppConfigDialog)
#endif

AppConfigDialog::AppConfigDialog(QString first_page_name) //rctrl_t *record_controller_,
    : QWidget()
{
    //	  , _record_controller(record_controller_)
    if (appconfig->interface_mode() == "mobile") { // if(true)
        qDebug() << "Screen size X Y: " << screen_size_x() << screen_size_y();
        this->setMinimumSize(screen_size_x(), screen_size_y());
        this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    }
    config_dialog = new ConfigDialog(this);
    config_dialog->set_window_title(tr(std::string(program_title_string + " settings").c_str()));

    pageMain = config_dialog->add_widget(new AppConfigPageMain(this), tr("Main"));
    pageCrypt = config_dialog->add_widget(new AppConfigPageCrypt(this), tr("Crypt"));
    pageSynchro = config_dialog->add_widget(new AppConfigPageSynchro(this), tr("Synchro"));
    pageRecordTable = config_dialog->add_widget(new AppConfigPageTable(this), tr("Note area"));
    pageMisc = config_dialog->add_widget(new AppConfigPageMisc(this), tr("Misc"));

    config_dialog->updateListWidth();
    if (first_page_name.size() > 0)
        change_page(first_page_name);
    config_dialog->exec();
}

// Переход на нужную страницу настроек (переход совершается кодом извне)
void AppConfigDialog::change_page(QString name)
{
    QListWidgetItem* item = nullptr;
    if (name == "pageMain")
        item = pageMain;
    if (name == "pageCrypt")
        item = pageCrypt;
    if (name == "pageSynchro")
        item = pageSynchro;
    if (name == "pageRecordTable")
        item = pageRecordTable;
    if (name == "pageMisc")
        item = pageMisc;
    if (item != nullptr)
        config_dialog->externalChangePage(item);
    else
        qDebug() << "AppConfigDialog::changePage cant find item for name: " << name;
}
