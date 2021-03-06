

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QPainter>
#include <QPen>

#include "libraries/flat_control.h"
#include "over_text_tool_button.h"

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(OverTextToolButton)
#endif

OverTextToolButton::OverTextToolButton(QWidget *parent)
    : FlatToolButton("", parent) {
	overText = "";
}

void OverTextToolButton::setOverText(QString iText) {
	overText = iText;

	this->update();
}

void OverTextToolButton::paintEvent(QPaintEvent *event) {
	FlatToolButton::paintEvent(event);

	QPainter painter(this);  // Создаём новый объект рисовальщика

	/*
          // Проверочная диагональная линия
          painter.setPen(QPen(Qt::red,1,Qt::SolidLine)); // Настройки рисования
          painter.drawLine(0, 0, width(), height()); // Рисование линии
       */

	painter.drawText(0, 0, width(), height(), Qt::AlignCenter, overText);
}
