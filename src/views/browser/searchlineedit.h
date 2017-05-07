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

#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include "urllineedit.h"

#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QLineEdit>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

// QT_BEGIN_NAMESPACE
class FindScreen;
namespace web {
	class UrlLineEdit;
	class SearchButton;
	class WebView;

	//            Clear button on the right hand side of the search widget.
	//            Hidden by default
	//            "A circle with an X in it"

	class ClearButton : public QAbstractButton {
#if QT_VERSION == 0x050600
		W_OBJECT(ClearButton)
#else
		Q_OBJECT
#endif

	    public:
		ClearButton(QWidget* parent = 0);
		void paintEvent(QPaintEvent* event);

	    public slots:
		void textChanged(const QString& text);

	    protected:
		//		ChaseWidget* _chasewidget;
	};


	//	Search icon on the left hand side of the search widget. When a menu is set a down arrow appears

	class SearchButton : public QAbstractButton {
	    public:
		SearchButton(QWidget* parent = 0);
		void paintEvent(QPaintEvent* event);
		QMenu* _menu;

	    protected:
		void mousePressEvent(QMouseEvent* event);
	};


	class SearchLineEdit : public UrlLineEdit //ExLineEdit
	{
		typedef UrlLineEdit super;
#if QT_VERSION == 0x050600
		W_OBJECT(SearchLineEdit)
#else
		Q_OBJECT
		Q_PROPERTY(QString inactiveText READ inactiveText WRITE setInactiveText)
#endif

	    signals:

#if QT_VERSION == 0x050600
		void textChanged(const QString& text) W_SIGNAL(textChanged, (const QString&), text);
#else
		void textChanged(const QString& text);
#endif

	    public:
		SearchLineEdit(QWidget* view = nullptr);

		QString inactiveText() const;
		void setInactiveText(const QString& text);

		QMenu* menu() const;
		void setMenu(QMenu* menu);
		static constexpr char _default_tip[] = "Search";
		SearchButton* searchbutton() const;

	    protected:
		void resizeEvent(QResizeEvent* event);
		void paintEvent(QPaintEvent* event);

	    private:
		void updateGeometries();


		SearchButton* _searchbutton;
		//		FindScreen* _find_screen;
		web::WebView* _web_view;
		QString _inactivetext;
#if QT_VERSION == 0x050600
		W_PROPERTY(QString, inactiveText, &SearchLineEdit::inactiveText, &SearchLineEdit::setInactiveText)
#endif
	};
}

// QT_END_NAMESPACE

#endif // SEARCHLINEEDIT_H
