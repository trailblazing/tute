#ifndef __MTTABLEWIDGET_H__
#define __MTTABLEWIDGET_H__

#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

// Этот класс не используется, скорее всего будет удален

// Класс, исправляющий QTableWidget, чтобы правильно применялись QSS-стили

class MtTableWidget : public QTableWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(MtTableWidget)  //
#else
	Q_OBJECT
#endif

	public:
	MtTableWidget(QWidget *parent = 0);
	virtual ~MtTableWidget();

	private:
	QStyledItemDelegate *itemDelegate;
};

#endif  // __MTTABLEWIDGET_H__
