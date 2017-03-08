
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include "mt_table_widget.h"

// Этот класс не используется, скорее всего будет удален

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(MtTableWidget)
#endif

MtTableWidget::MtTableWidget(QWidget *parent)
    : QTableWidget(parent) {
	Q_UNUSED(parent);

	itemDelegate = new QStyledItemDelegate();
	this->setItemDelegate(itemDelegate);
}
MtTableWidget::~MtTableWidget() {
	delete itemDelegate;
}
