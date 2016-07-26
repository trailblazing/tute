
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include <QStackedWidget>
#include <QDialog>

#include "main.h"
#include "views/app_config/config_dialog.h"

#include "editor_config_dialog.h"
#include "editor_config_font.h"
#include "editor_config_misc.h"
#include "libraries/global_parameters.h"



extern GlobalParameters globalparameters;


#if QT_VERSION == 0x050600
W_OBJECT_IMPL(EditorConfigDialog)
#endif


EditorConfigDialog::EditorConfigDialog(QWidget *parent) : QWidget(parent){
    configDialog = new ConfigDialog(this);

    configDialog->set_window_title(tr("Editor settings"));

    configDialog->add_widget(new EditorConfigFont(this), tr("Fonts"));
    configDialog->add_widget(new EditorConfigMisc(this), tr("Misc"));

    configDialog->exec();
}
