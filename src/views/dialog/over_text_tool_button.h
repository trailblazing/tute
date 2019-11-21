#ifndef __OVERTEXTTOOLBUTTON_H__
#define __OVERTEXTTOOLBUTTON_H__

#include "libraries/flat_control.h"
#include <QPaintEvent>

#include <QtGlobal>
#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class FlatToolButton;

// Виджет, похожий на FlatToolButton, но позволяющий писать поверх иконки

class OverTextToolButton : public FlatToolButton {
#if QT_VERSION == 0x050600
	W_OBJECT(OverTextToolButton)
#else
	Q_OBJECT
#endif

	public:
	OverTextToolButton(QWidget *parent = 0);

	void setOverText(QString iText);

	protected:
	void paintEvent(QPaintEvent *event);

	QString overText;
};

#endif  // __OVERTEXTTOOLBUTTON_H__
