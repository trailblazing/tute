
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QDialog>
#include <QStackedWidget>

#include "main.h"
#include "views/app_config/config_dialog.h"

#include "editor_config_dialog.h"
#include "editor_config_font.h"
#include "editor_config_misc.h"
#include "libraries/global_parameters.h"
#include "libraries/wyedit/editor.h"
#include "libraries/wyedit/editor_config.h"

extern std::shared_ptr<gl_para> gl_paras;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(EditorConfigDialog)
#endif

EditorConfigDialog::EditorConfigDialog(std::shared_ptr<EditorConfig> _editor_config, Editor* const parent)
    : QWidget(parent)
{
	configDialog = new ConfigDialog(this);

	configDialog->set_window_title(tr("Editor settings"));

	configDialog->add_widget(new EditorConfigFont(_editor_config, this), tr("Fonts"));
	configDialog->add_widget(new EditorConfigMisc(_editor_config, this), tr("Misc"));

	configDialog->exec();
}
