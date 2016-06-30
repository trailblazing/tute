#ifndef __MTSTYLEDITEMDELEGATE_H__
#define __MTSTYLEDITEMDELEGATE_H__


#include <wobjectdefs.h>
#include <QObject>

#include <QStyledItemDelegate>

// Этот класс не используется, скорее всего будет удален

class MtStyledItemDelegate : public QStyledItemDelegate {
    W_OBJECT(MtStyledItemDelegate)	//

    public:
	explicit MtStyledItemDelegate(QObject *parent = 0);

	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    signals:

    public slots:
};

#endif	// __MTSTYLEDITEMDELEGATE_H__
