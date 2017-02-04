#ifndef __MTTABLEWIDGET_H__
#define __MTTABLEWIDGET_H__




#include <QWidget>
#include <QTableWidget>
#include <QStyledItemDelegate>



#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif


// Этот класс не используется, скорее всего будет удален

// Класс, исправляющий QTableWidget, чтобы правильно применялись QSS-стили

class MtTableWidget : public QTableWidget {
#if QT_VERSION == 0x050600
    W_OBJECT(MtTableWidget)	//
#else
    Q_OBJECT
#endif

    public:
	MtTableWidget(QWidget *parent = 0);
	virtual ~MtTableWidget();

    private:
	QStyledItemDelegate *itemDelegate;
};

#endif	// __MTTABLEWIDGET_H__

