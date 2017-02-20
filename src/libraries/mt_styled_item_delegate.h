#ifndef __MTSTYLEDITEMDELEGATE_H__
#define __MTSTYLEDITEMDELEGATE_H__

#include <QStyledItemDelegate>

#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#endif
#include <QObject>

// Этот класс не используется, скорее всего будет удален

class MtStyledItemDelegate : public QStyledItemDelegate {
#if QT_VERSION == 0x050600
W_OBJECT(MtStyledItemDelegate) //
#else
Q_OBJECT
#endif

public:
	explicit MtStyledItemDelegate(QObject *parent = 0);

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

public slots:
};

#endif // __MTSTYLEDITEMDELEGATE_H__
