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

#ifndef URLLINEEDIT_H
#define URLLINEEDIT_H

#include <QtCore/QUrl>
#include <QtWidgets/QLabel>
#include <QtWidgets/QStyleOptionFrame>
#include <QtWidgets/QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

// QT_BEGIN_NAMESPACE

namespace web {
	class TabWidget;
	class ClearButton;
	class WebView;

	// addressbar
	class ExLineEdit : public QWidget {
#if QT_VERSION == 0x050600
		W_OBJECT(ExLineEdit)
#else
		Q_OBJECT
#endif

	    public:
		ExLineEdit(QWidget* parent = 0);

		QLineEdit* lineEdit() const;

		void left_widget(QWidget* widget);
		QWidget* left_widget() const;

		QSize sizeHint() const;

		QVariant inputMethodQuery(Qt::InputMethodQuery property) const;

	    protected:
		void focusInEvent(QFocusEvent* event);
		void focusOutEvent(QFocusEvent* event);
		void keyPressEvent(QKeyEvent* event);
		void paintEvent(QPaintEvent* event);
		void resizeEvent(QResizeEvent* event);
		void inputMethodEvent(QInputMethodEvent* e);
		bool event(QEvent* event);

	    protected:
		void updateGeometries();
		void initStyleOption(QStyleOptionFrame* option) const; // QStyleOptionFrameV2

		QWidget* _left_widget;
		QLineEdit* _lineedit;
		ClearButton* _clearbutton;
	};

	class UrlIconLabel;
	class WebView;

	class UrlIconLabel : public QLabel {
	    public:
		UrlIconLabel(QWidget* parent);

		WebView* _browserview;

	    protected:
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);

	    private:
		QPoint _dragstartpos;
	};

	class UrlLineEdit : public ExLineEdit {
#if QT_VERSION == 0x050600
		W_OBJECT(UrlLineEdit)
#else
		Q_OBJECT
#endif

	    public:
		UrlLineEdit(WebView* view = nullptr, QWidget* parent = 0);
		void setWebView(WebView* webView);

		WebView* webview();
		UrlIconLabel* iconlabel() const;

	    protected:
		void paintEvent(QPaintEvent* event);
		void focusOutEvent(QFocusEvent* event);

	    private slots:
		void webViewUrlChanged(const QUrl& url);
		void webViewIconChanged();

	    private:
		QLinearGradient generateGradient(const QColor& color) const;
		WebView* _webview;
		UrlIconLabel* _iconlabel;
		QColor _defaultbasecolor;
	};
}

// QT_END_NAMESPACE

#endif // URLLINEEDIT_H
