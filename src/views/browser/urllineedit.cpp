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


#include <QtCore/QEvent>
#include <QtCore/QMimeData>

#include <QtGui/QDrag>
#include <QtGui/QFocusEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleOptionFrame> // #include <QtWidgets/QStyleOptionFrameV2>

#include <QCompleter>
#include <QLineEdit>
#include <QtCore/QDebug>

#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "searchlineedit.h"
#include "urllineedit.h"
#include "views/browser/chasewidget.h"
#include "views/find_in_base_screen/find_screen.h"
#include "webview.h"

namespace web {

#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(ExLineEdit)
#endif

	ExLineEdit::ExLineEdit(QWidget* view)
	    : QWidget(view)
#ifdef USE_CLEAR_BUTTON
	    , _clearbutton(new ClearButton(this))
	    , _left_widget(_clearbutton) //
#else

	    , _chasewidget(new ChaseWidget(QSize(16, 16), this)) //
	    , _left_widget(_chasewidget)
#endif // USE_CLEAR_BUTTON
	    , _right_widget(nullptr)
	    , _lineedit(new QLineEdit(this))
	    , _web_view(dynamic_cast<web::WebView*>(view))
	{
#ifndef USE_CLEAR_BUTTON
		_chasewidget->setMaximumWidth(17);
#endif
		setFocusPolicy(_lineedit->focusPolicy());
		setAttribute(Qt::WA_InputMethodEnabled);
		setSizePolicy(_lineedit->sizePolicy());
		setBackgroundRole(_lineedit->backgroundRole());
		setMouseTracking(true);
		setAcceptDrops(true);
		setAttribute(Qt::WA_MacShowFocusRect, true);
		QPalette p = _lineedit->palette();
		setPalette(p);

		// line edit
		_lineedit->setFrame(false);
		_lineedit->setFocusProxy(this);
		_lineedit->setAttribute(Qt::WA_MacShowFocusRect, false);
		QPalette clearPalette = _lineedit->palette();
		clearPalette.setBrush(QPalette::Base, QBrush(Qt::transparent));
		_lineedit->setPalette(clearPalette);

// clearButton
// _clearbutton = new ClearButton(this);
#ifdef USE_CLEAR_BUTTON
		connect(_lineedit, &QLineEdit::textChanged, _clearbutton, &ClearButton::textChanged);
//		_right_widget = _clearbutton;
#endif // USE_CLEAR_BUTTON
	}

	QLineEdit* ExLineEdit::lineEdit() const
	{
		return _lineedit;
	}

	void ExLineEdit::left_widget(QWidget* widget)
	{
		_left_widget = widget;
	}

	QWidget* ExLineEdit::left_widget() const
	{
		return _left_widget;
	}
	void ExLineEdit::right_widget(QWidget* widget)
	{
		_right_widget = widget;
	}

	QWidget* ExLineEdit::right_widget() const
	{
		return _right_widget;
	}
	void ExLineEdit::resizeEvent(QResizeEvent* event)
	{
		Q_ASSERT(_left_widget);
		assert(_right_widget);
		updateGeometries();
		QWidget::resizeEvent(event);
	}

	void ExLineEdit::updateGeometries()
	{
		QStyleOptionFrame panel; // QStyleOptionFrameV2
		initStyleOption(&panel);
		QRect rect =
		    style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

		int height = rect.height(); //20
		int width = rect.width();

		int leftwidgetheight_ = _left_widget->height(); //15
		_left_widget->setGeometry(rect.x() + 2, rect.y() + (height - leftwidgetheight_) / 2, _left_widget->width(), _left_widget->height());

		int right_widget_width = this->height(); //22
		_lineedit->setGeometry(_left_widget->x() + _left_widget->width() + 2, 0, width - right_widget_width - _left_widget->width() - 2, this->height());
		auto this_width = this->width();
		//		_clearbutton
		_right_widget->setGeometry(this_width - right_widget_width, 0, right_widget_width, this->height());
	}

	void ExLineEdit::initStyleOption(
	    QStyleOptionFrame* option) const // QStyleOptionFrameV2
	{
		option->initFrom(this);
		option->rect = contentsRect();
		option->lineWidth =
		    style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
		option->midLineWidth = 0;
		option->state |= QStyle::State_Sunken;
		if (_lineedit->isReadOnly())
			option->state |= QStyle::State_ReadOnly;
#ifdef QT_KEYPAD_NAVIGATION
		if (hasEditFocus())
			option->state |= QStyle::State_HasEditFocus;
#endif
		option->features = QStyleOptionFrame::None; // QStyleOptionFrameV2
	}

	QSize ExLineEdit::sizeHint() const
	{
		_lineedit->setFrame(true);
		QSize size = _lineedit->sizeHint();
		_lineedit->setFrame(false);

		return size;
	}

	void ExLineEdit::focusInEvent(QFocusEvent* event)
	{
		_lineedit->event(event);
		QWidget::focusInEvent(event);
	}

	void ExLineEdit::focusOutEvent(QFocusEvent* event)
	{
		_lineedit->event(event);

		void (QCompleter::*_activated)(const QString& text) = &QCompleter::activated;
		if (_lineedit->completer()) {
			connect(_lineedit->completer(), _activated, _lineedit, &QLineEdit::setText);
#ifndef QT_NO_COMPLETER
// connect(_lineedit->completer(), &QCompleter::highlighted, _lineedit,
// &QLineEdit::_q_completionHighlighted);
#endif
		}
		QWidget::focusOutEvent(event);
	}

	void ExLineEdit::keyPressEvent(QKeyEvent* event)
	{
		_lineedit->event(event);
	}

	bool ExLineEdit::event(QEvent* event)
	{
		bool result = false;
		if (event->type() == QEvent::ShortcutOverride)
			result = _lineedit->event(event); // return _lineedit->event(event);
		else
			result = QWidget::event(event);
		return result; // QWidget::event(event);
	}

	void ExLineEdit::paintEvent(QPaintEvent*)
	{
		QPainter p(this);
		QStyleOptionFrame panel; // QStyleOptionFrameV2
		initStyleOption(&panel);
		style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &p, this);
	}

	QVariant ExLineEdit::inputMethodQuery(Qt::InputMethodQuery property) const
	{
		return _lineedit->inputMethodQuery(property);
	}

	void ExLineEdit::inputMethodEvent(QInputMethodEvent* e)
	{
		_lineedit->event(e);
	}
#ifndef USE_CLEAR_BUTTON
	ChaseWidget* ExLineEdit::chasewidget() const
	{
		return static_cast<ChaseWidget*>(_chasewidget);
	}
#endif // USE_CLEAR_BUTTON
	UrlIconLabel::UrlIconLabel(QWidget* parent)
	    : QLabel(parent)
	    , _browserview(0)
	{
		setMinimumWidth(16);
		setMinimumHeight(16);
	}

	void UrlIconLabel::mousePressEvent(QMouseEvent* event)
	{
		if (event->button() == Qt::LeftButton)
			_dragstartpos = event->pos();
		QLabel::mousePressEvent(event);
	}

	void UrlIconLabel::mouseMoveEvent(QMouseEvent* event)
	{
		if (event->buttons() == Qt::LeftButton &&
		    (event->pos() - _dragstartpos).manhattanLength() >
			QApplication::startDragDistance() &&
		    _browserview) {
			if (_browserview->page()) {
				QDrag* drag = new QDrag(this);
				QMimeData* mimeData = new QMimeData;

				mimeData->setText(
				    QString::fromUtf8(_browserview->page()->url().toEncoded()));
				QList<QUrl> urls;
				urls.append(_browserview->page()->url());
				mimeData->setUrls(urls);

				drag->setMimeData(mimeData);
				drag->exec();
			}
		}
	}

#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(UrlLineEdit)
#endif

	UrlLineEdit::UrlLineEdit(QWidget* view)
	    : ExLineEdit(view)
	    //	    , _find_screen(gl_paras->find_screen()) //dynamic_cast<FindScreen*>(find_screen_)
	    //	    , _web_view(view)
	    , _iconlabel(new UrlIconLabel(this))
	{
// icon
// _iconlabel = new UrlIconLabel(this);
#ifdef USE_URL_ICON
		_iconlabel->resize(16, 16);
		left_widget(_iconlabel); // replaced with searchbutton
#else


		if (_web_view) {
#ifdef USE_CLEAR_BUTTON
			_clearbutton->resize(16, 16 //_clearbutton->height(), _clearbutton->height()
			    );

			connect(_web_view, &web::WebView::loadFinished, [&](bool) { _clearbutton->setIcon(_web_view->icon()); });
#else
			connect(_web_view, &web::WebView::loadFinished, [&](bool) { _chasewidget->setIcon(_web_view->icon()); });
#endif //USE_CLEAR_BUTTON
		}


#endif //USE_URL_ICON
		_defaultbasecolor = palette().color(QPalette::Base);
		setWebView(dynamic_cast<web::WebView*>(view));
	}

	void UrlLineEdit::setWebView(WebView* webView)
	{
		// Q_ASSERT(!_web_view);
		_web_view = webView;
		_iconlabel->_browserview = _web_view;
		connect(_web_view, &WebView::urlChanged, this, &UrlLineEdit::webViewUrlChanged);
		connect(_web_view, &WebView::iconChanged, this, &UrlLineEdit::webViewIconChanged);
		connect(_web_view, &WebView::loadProgress, this, [&](int i) {
			Q_UNUSED(i);
			update();
		});
	}

	void UrlLineEdit::webViewUrlChanged(const QUrl& url)
	{
		_lineedit->setText(QString::fromUtf8(url.toEncoded()));
		qDebug() << "_lineedit->text()=\t" << _lineedit->text();
		_lineedit->setCursorPosition(0);
	}

	void UrlLineEdit::webViewIconChanged()
	{
		Q_ASSERT(_web_view);
#ifdef USE_URL_ICON
		QPixmap pixmap = _web_view->icon().pixmap(16, 16);
		_iconlabel->setPixmap(pixmap);
#endif // USE_URL_ICON
	}

	QLinearGradient UrlLineEdit::generateGradient(const QColor& color) const
	{
		QLinearGradient gradient(0, 0, 0, height());
		gradient.setColorAt(0, _defaultbasecolor);
		gradient.setColorAt(0.15, color.lighter(120));
		gradient.setColorAt(0.5, color);
		gradient.setColorAt(0.85, color.lighter(120));
		gradient.setColorAt(1, _defaultbasecolor);

		return gradient;
	}

	void UrlLineEdit::focusOutEvent(QFocusEvent* event)
	{
		// if(_lineedit->text().isEmpty() && _web_view){
		// if(_web_view->load_finished())
		// if(_web_view->page())_lineedit->setText(QString::fromUtf8(_web_view->page()->url().toEncoded()));
		// // _web_view maybe not accessible, page() may be invalid
		// }
		ExLineEdit::focusOutEvent(event);
	}

	void UrlLineEdit::paintEvent(QPaintEvent* event)
	{
		QPalette p = palette();
		if (_web_view) {
			if (_web_view->page() &&
			    _web_view->page()->url().scheme() == QLatin1String("https")) {
				QColor lightYellow(248, 248, 210);
				p.setBrush(QPalette::Base, generateGradient(lightYellow));
			}
		} else
			p.setBrush(QPalette::Base, _defaultbasecolor);
		setPalette(p);
		ExLineEdit::paintEvent(event);

		QPainter painter(this);
		QStyleOptionFrame panel; // QStyleOptionFrameV2
		initStyleOption(&panel);
		QRect backgroundRect =
		    style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
		if (_web_view && !hasFocus()) {
			int progress = _web_view->progress();
			QColor loadingColor = QColor(116, 192, 250);
			painter.setBrush(generateGradient(loadingColor));
			painter.setPen(Qt::transparent);
			int mid = backgroundRect.width() / 100 * progress;
			QRect progressRect(backgroundRect.x(), backgroundRect.y(), mid, backgroundRect.height());
			painter.drawRect(progressRect);
		}
	}

	WebView* UrlLineEdit::webview() { return _web_view; }

	UrlIconLabel* UrlLineEdit::iconlabel() const
	{
		return _iconlabel;
	}
}
