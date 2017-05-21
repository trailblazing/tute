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

#ifndef CHASEWIDGET_H
#define CHASEWIDGET_H

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include <QtCore/QSize>
#include <QtGui/QColor>
#include <QtGui/QPixmap>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QWidget>


#include "libraries/global_parameters.h"
#include "utility/lease.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

QT_BEGIN_NAMESPACE
class QHideEvent;
class QShowEvent;
class QPaintEvent;
class QTimerEvent;
QT_END_NAMESPACE

// QT_BEGIN_NAMESPACE

namespace web {
	class ChaseWidget : public QAbstractButton //QWidget
			    ,
//			    public boost::intrusive_ref_counter<ChaseWidget, boost::thread_safe_counter>,
			    public sd::renter {
#if QT_VERSION == 0x050600
		W_OBJECT(ChaseWidget)
#else
		Q_OBJECT
#endif

	    public:
		ChaseWidget(const QSize& size, QWidget* parent = 0, QPixmap pixmap = QPixmap(), bool pixmapEnabled = false);

		void setAnimated(bool value);
		void setPixmapEnabled(bool enable);
		QSize sizeHint() const;

	    protected:
		void paintEvent(QPaintEvent* event);
		void timerEvent(QTimerEvent* event);

	    private:
		int segmentCount() const;
		QColor colorForSegment(int segment) const;

		int _segment;
		int _delay;
		int _step;
		int _timerid;
		QSize _size;
		bool _animated;
		QPixmap _pixmap;
		bool _pixmapenabled;
	};
}

// QT_END_NAMESPACE

#endif
