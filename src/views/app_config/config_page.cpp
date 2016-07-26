
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif


#include <QWidget>
#include <QBoxLayout>
#include <QDir>
#include <QLineEdit>

#include "config_page.h"


#if QT_VERSION == 0x050600
W_OBJECT_IMPL(ConfigPage)
#endif


ConfigPage::ConfigPage(QWidget *parent) : QWidget(parent)
{}

ConfigPage::~ConfigPage()
{}

int ConfigPage::apply_changes(void){
    return 0;
}

