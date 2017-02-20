
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include "mt_styled_item_delegate.h"

// Этот класс не используется, скорее всего будет удален

// Делегат, использующийся для QTableView чтобы стилизовать отступы,
// так как через QSS отступы для QTableView не стилизуются

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(MtStyledItemDelegate)
#endif

MtStyledItemDelegate::MtStyledItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{}
QSize MtStyledItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QSize size = QStyledItemDelegate::sizeHint(option, index);

	size.rheight() = 45;

	return size;
}
