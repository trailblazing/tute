#ifndef __OVERTEXTTOOLBUTTON_H__
#define __OVERTEXTTOOLBUTTON_H__

#include <wobjectdefs.h>
#include <QObject>

#include <QPaintEvent>
#include "libraries/flat_control.h"

class FlatToolButton;

// Виджет, похожий на FlatToolButton, но позволяющий писать поверх иконки


class OverTextToolButton : public FlatToolButton {
    W_OBJECT(OverTextToolButton)

    public:
	OverTextToolButton(QWidget *parent = 0);

	void setOverText(QString iText);


    protected:

	void paintEvent(QPaintEvent *event);

	QString overText;
};

#endif	// __OVERTEXTTOOLBUTTON_H__
