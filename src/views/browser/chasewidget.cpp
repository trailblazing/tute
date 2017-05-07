/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include "chasewidget.h"

#include <QtCore/QPoint>

#include <QtGui/QHideEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QShowEvent>
#include <QtWidgets/QApplication>

namespace web {
#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(ChaseWidget)
#endif

	ChaseWidget::ChaseWidget(const QSize& size, QWidget* parent, QPixmap pixmap, bool pixmapEnabled)
	    : QAbstractButton(parent) //QWidget(parent)
	    , _segment(0)
	    , _delay(100)
	    , _step(40)
	    , _timerid(-1)
	    , _size(size.height() - 1, size.height() - 1)
	    , _animated(false)
	    , _pixmap(pixmap)
	    , _pixmapenabled(pixmapEnabled)
	{
		// adjustSize();
		resize(_size);
	}

	void ChaseWidget::setAnimated(bool value)
	{
		if (_animated == value)
			return;
		_animated = value;
		if (_timerid != -1) {
			killTimer(_timerid);
			_timerid = -1;
		}
		if (_animated) {
			_segment = 0;
			_timerid = startTimer(_delay);
		}
		update();
	}

	void ChaseWidget::paintEvent(QPaintEvent* event)
	{
		Q_UNUSED(event);
		QPainter p(this);
		if (_pixmapenabled && !_pixmap.isNull()) {
			p.drawPixmap(0, 0, _pixmap);

			return;
		}
		// int w = width();    // 37
		// int h = height();   // 27

		const int extent = _size.height(); // qMin(width() - 8, height() - 8);
		const int displ = extent / 3;      //extent / 4;
		const int ext = extent / 4 - 1;    // extent / 4 - 1;

		p.setRenderHint(QPainter::Antialiasing, true);
		if (_animated)
			p.setPen(Qt::gray);
		else
			p.setPen(QPen(palette().dark().color()));
		p.translate(                    //_size.height() - 7, _size.height() - 2 // 4
		    width() / 2, height() / 2); // center
		for (int segment = 0; segment < segmentCount(); ++segment) {
			p.rotate(QApplication::isRightToLeft() ? _step : -_step);
			if (_animated)
				p.setBrush(colorForSegment(segment));
			else
				p.setBrush(palette().background());
			p.drawEllipse(QRect(
			    displ,
			    -ext / 2, //-ext / 2
			    ext,
			    ext));
		}
	}

	QSize ChaseWidget::sizeHint() const
	{
		return // _size;   //
		    QSize(_size.height() * 2.7, _size.height());
	}

	void ChaseWidget::timerEvent(QTimerEvent* event)
	{
		if (event->timerId() == _timerid) {
			++_segment;
			update();
		}
		QWidget::timerEvent(event);
	}

	QColor ChaseWidget::colorForSegment(int seg) const
	{
		int index = ((seg + _segment) % segmentCount());
		int comp = qMax(0, 255 - (index * (255 / segmentCount())));

		return QColor(comp, comp, comp, 255);
	}

	int ChaseWidget::segmentCount() const
	{
		return 360 / _step;
	}

	void ChaseWidget::setPixmapEnabled(bool enable)
	{
		_pixmapenabled = enable;
	}
}
