#include <QStackedWidget>
#include <QDialog>

#include "main.h"
#include "views/app_config/ConfigDialog.h"

#include "EditorConfigDialog.h"
#include "EditorConfigFont.h"
#include "EditorConfigMisc.h"
#include "libraries/GlobalParameters.h"



extern GlobalParameters globalparameters;

EditorConfigDialog::EditorConfigDialog(QWidget *parent) : QWidget(parent)
{
    configDialog = new ConfigDialog();

    configDialog->set_window_title(tr("Editor settings"));

    configDialog->add_widget(new EditorConfigFont(this), tr("Fonts"));
    configDialog->add_widget(new EditorConfigMisc(this), tr("Misc"));

    configDialog->exec();
}

