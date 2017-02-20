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

#include "urllineedit.h"

#include "libraries/qt_single_application5/qtsingleapplication.h"

#include "searchlineedit.h"
#include "webview.h"

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

namespace browser {
#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(ExLineEdit)
#endif

	ExLineEdit::ExLineEdit(QWidget *parent)
		: QWidget(parent)
		  , _leftwidget(nullptr)
		  , _lineedit(new QLineEdit(this))
		  , _clearbutton(new ClearButton(this)){
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
		connect(_clearbutton, &ClearButton::clicked, _lineedit, &QLineEdit::clear);
		connect(_lineedit, &QLineEdit::textChanged, _clearbutton, &ClearButton::textChanged);
	}

	void ExLineEdit::setLeftWidget(QWidget *widget){
		_leftwidget = widget;
	}

	QWidget *ExLineEdit::leftWidget() const {
		return _leftwidget;
	}

	void ExLineEdit::resizeEvent(QResizeEvent *event){
		Q_ASSERT(_leftwidget);
		updateGeometries();
		QWidget::resizeEvent(event);
	}

	void ExLineEdit::updateGeometries(){
		QStyleOptionFrame panel; // QStyleOptionFrameV2
		initStyleOption(&panel);
		QRect rect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

		int height	= rect.height();
		int width	= rect.width();

		int leftwidgetheight_ = _leftwidget->height();
		_leftwidget->setGeometry(rect.x() + 2, rect.y() + (height - leftwidgetheight_) / 2, _leftwidget->width(), _leftwidget->height());

		int clearButtonWidth = this->height();
		_lineedit->setGeometry(_leftwidget->x() + _leftwidget->width(), 0, width - clearButtonWidth - _leftwidget->width(), this->height());

		_clearbutton->setGeometry(this->width() - clearButtonWidth, 0, clearButtonWidth, this->height());
	}

	void ExLineEdit::initStyleOption(QStyleOptionFrame *option) const // QStyleOptionFrameV2
	{
		option->initFrom(this);
		option->rect = contentsRect();
		option->lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, this);
		option->midLineWidth = 0;
		option->state |= QStyle::State_Sunken;
		if(_lineedit->isReadOnly()) option->state |= QStyle::State_ReadOnly;
#ifdef QT_KEYPAD_NAVIGATION
		if(hasEditFocus()) option->state |= QStyle::State_HasEditFocus;
#endif
		option->features = QStyleOptionFrame::None; // QStyleOptionFrameV2
	}

	QSize ExLineEdit::sizeHint() const {
		_lineedit->setFrame(true);
		QSize size = _lineedit->sizeHint();
		_lineedit->setFrame(false);

		return size;
	}

	void ExLineEdit::focusInEvent(QFocusEvent *event){
		_lineedit->event(event);
		QWidget::focusInEvent(event);
	}

	void ExLineEdit::focusOutEvent(QFocusEvent *event){
		_lineedit->event(event);

		void (QCompleter::*_activated)(const QString &text) = &QCompleter::activated;
		if(_lineedit->completer()){
			connect(_lineedit->completer(), _activated, _lineedit, &QLineEdit::setText);
#ifndef QT_NO_COMPLETER
// connect(_lineedit->completer(), &QCompleter::highlighted, _lineedit, &QLineEdit::_q_completionHighlighted);
#endif
		}
		QWidget::focusOutEvent(event);
	}

	void ExLineEdit::keyPressEvent(QKeyEvent *event){
		_lineedit->event(event);
	}

	bool ExLineEdit::event(QEvent *event){
		bool result = false;
		if(event->type() == QEvent::ShortcutOverride) result = _lineedit->event(event); // return _lineedit->event(event);
		else result = QWidget::event(event);
		return result; // QWidget::event(event);
	}

	void ExLineEdit::paintEvent(QPaintEvent *){
		QPainter p(this);
		QStyleOptionFrame panel; // QStyleOptionFrameV2
		initStyleOption(&panel);
		style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &p, this);
	}

	QVariant ExLineEdit::inputMethodQuery(Qt::InputMethodQuery property) const {
		return _lineedit->inputMethodQuery(property);
	}

	void ExLineEdit::inputMethodEvent(QInputMethodEvent *e){
		_lineedit->event(e);
	}

	class UrlIconLabel : public QLabel {
	public:
		UrlIconLabel(QWidget *parent);

		WebView *_browserview;

	protected:
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);

	private:
		QPoint _dragstartpos;
	};

	UrlIconLabel::UrlIconLabel(QWidget *parent)
		: QLabel(parent)
		  , _browserview(0){
		setMinimumWidth(16);
		setMinimumHeight(16);
	}

	void UrlIconLabel::mousePressEvent(QMouseEvent *event){
		if(event->button() == Qt::LeftButton) _dragstartpos = event->pos();
		QLabel::mousePressEvent(event);
	}

	void UrlIconLabel::mouseMoveEvent(QMouseEvent *event){
		if(  event->buttons() == Qt::LeftButton
		  && (event->pos() - _dragstartpos).manhattanLength() > QApplication::startDragDistance()
		  && _browserview){
			if(_browserview->page()){
				QDrag *drag = new QDrag(this);
				QMimeData *mimeData = new QMimeData;

				mimeData->setText(QString::fromUtf8(_browserview->page()->url().toEncoded()));
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

	UrlLineEdit::UrlLineEdit(WebView *view, QWidget *parent)
		: ExLineEdit(parent)
		  , _webview(view)
		  , _iconlabel(new UrlIconLabel(this)){
		// icon
		// _iconlabel = new UrlIconLabel(this);
		_iconlabel->resize(16, 16);
		setLeftWidget(_iconlabel);
		_defaultbasecolor = palette().color(QPalette::Base);
		setWebView(view);
	}

	void UrlLineEdit::setWebView(WebView *webView){
		// Q_ASSERT(!_webview);
		_webview = webView;
		_iconlabel->_browserview = webView;
		connect(webView, &WebView::urlChanged, this, &UrlLineEdit::webViewUrlChanged);
		connect(webView, &WebView::iconChanged, this, &UrlLineEdit::webViewIconChanged);
		connect(webView, &WebView::loadProgress, this, [&](int i){
				Q_UNUSED(i);
				update();
			});
	}

	void UrlLineEdit::webViewUrlChanged(const QUrl &url){
		_lineedit->setText(QString::fromUtf8(url.toEncoded()));
		_lineedit->setCursorPosition(0);
	}

	void UrlLineEdit::webViewIconChanged(){
		Q_ASSERT(_webview);
		QPixmap pixmap = _webview->icon().pixmap(16, 16);
		_iconlabel->setPixmap(pixmap);
	}

	QLinearGradient UrlLineEdit::generateGradient(const QColor &color) const {
		QLinearGradient gradient(0, 0, 0, height());
		gradient.setColorAt(0, _defaultbasecolor);
		gradient.setColorAt(0.15, color.lighter(120));
		gradient.setColorAt(0.5, color);
		gradient.setColorAt(0.85, color.lighter(120));
		gradient.setColorAt(1, _defaultbasecolor);

		return gradient;
	}

	void UrlLineEdit::focusOutEvent(QFocusEvent *event){
		// if(_lineedit->text().isEmpty() && _webview){
		// if(_webview->load_finished())
		// if(_webview->page())_lineedit->setText(QString::fromUtf8(_webview->page()->url().toEncoded()));	// _webview maybe not accessible, page() may be invalid
		// }
		ExLineEdit::focusOutEvent(event);
	}

	void UrlLineEdit::paintEvent(QPaintEvent *event){
		QPalette p = palette();
		if(_webview){
			if(_webview->page() && _webview->page()->url().scheme() == QLatin1String("https")){
				QColor lightYellow(248, 248, 210);
				p.setBrush(QPalette::Base, generateGradient(lightYellow));
			}
		}else p.setBrush(QPalette::Base, _defaultbasecolor);
		setPalette(p);
		ExLineEdit::paintEvent(event);

		QPainter painter(this);
		QStyleOptionFrame panel; // QStyleOptionFrameV2
		initStyleOption(&panel);
		QRect backgroundRect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
		if(_webview && !hasFocus()){
			int progress = _webview->progress();
			QColor loadingColor = QColor(116, 192, 250);
			painter.setBrush(generateGradient(loadingColor));
			painter.setPen(Qt::transparent);
			int mid = backgroundRect.width() / 100 * progress;
			QRect progressRect(backgroundRect.x(), backgroundRect.y(), mid, backgroundRect.height());
			painter.drawRect(progressRect);
		}
	}
}
