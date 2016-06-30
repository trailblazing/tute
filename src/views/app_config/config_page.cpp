#include <wobjectimpl.h>
#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>

#include "config_page.h"

W_OBJECT_IMPL(ConfigPage)
ConfigPage::ConfigPage(QWidget *parent) : QWidget(parent)
{}
ConfigPage::~ConfigPage()
{}
int ConfigPage:: apply_changes(void){
    return 0;
}
