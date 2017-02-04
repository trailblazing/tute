#ifndef __OVERTEXTTOOLBUTTON_H__
#define __OVERTEXTTOOLBUTTON_H__



#include <QPaintEvent>
#include "libraries/flat_control.h"


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
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

#endif	// __OVERTEXTTOOLBUTTON_H__
