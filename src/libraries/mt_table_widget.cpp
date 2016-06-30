#include <wobjectimpl.h>
#include "mt_table_widget.h"

// Этот класс не используется, скорее всего будет удален

W_OBJECT_IMPL(MtTableWidget)

MtTableWidget::MtTableWidget(QWidget *parent) : QTableWidget(parent){
    Q_UNUSED(parent);

    itemDelegate = new QStyledItemDelegate();
    this->setItemDelegate(itemDelegate);
}
MtTableWidget::~MtTableWidget(){
    delete itemDelegate;
}
