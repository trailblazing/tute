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


#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QMenu>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleOptionFrame> // #include <QtWidgets/QStyleOptionFrameV2>

#include "libraries/fixed_parameters.h"
#include "libraries/qtm/blogger.h"
#include "searchlineedit.h"
#include "urllineedit.h"
#include "views/browser/chasewidget.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/find_screen.h"

namespace web {
#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(ClearButton)
#endif

    ClearButton::ClearButton(QWidget* parent)
        : QAbstractButton(parent)
    //	    , _chasewidget(new ChaseWidget(QSize(16, 16), this))
    {
#ifndef QT_NO_CURSOR
        setCursor(Qt::ArrowCursor);
#endif                                          // QT_NO_CURSOR
        setToolTip(tr("Switch topic and URL")); //"Clear"
        setVisible(false);
        setFocusPolicy(Qt::NoFocus);
        //		_chasewidget->setMaximumWidth(17);
    }

    void ClearButton::paintEvent(QPaintEvent* event)
    {
        Q_UNUSED(event);
        QPainter painter(this);
        int height = this->height();

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(isDown() ? palette().color(QPalette::Dark) : palette().color(QPalette::Mid));
        painter.setPen(painter.brush().color());
        int size = width();
        int offset = size / 5;
        int radius = size - offset * 2;
        painter.drawEllipse(offset, offset, radius, radius);

        painter.setPen(palette().color(QPalette::Base));
        int border = offset * 2;
        painter.drawLine(border, border, width() - border, height - border);
        painter.drawLine(border, height - border, width() - border, border);
    }

    void ClearButton::textChanged(const QString& text)
    {
        setVisible(!text.isEmpty());
    }


    SearchButton::SearchButton(QWidget* parent)
        : QAbstractButton(parent)
        , _menu(new QMenu(this))
    {
        setObjectName(QLatin1String("SearchButton"));
#ifndef QT_NO_CURSOR
        setCursor(Qt::ArrowCursor);
#endif // QT_NO_CURSOR
        setFocusPolicy(Qt::NoFocus);
    }

    void SearchButton::mousePressEvent(QMouseEvent* event)
    {
        if (_menu && event->button() == Qt::LeftButton) {
            QWidget* p = parentWidget();
            if (p) {
                QPoint r = p->mapToGlobal(QPoint(0, p->height()));
                _menu->exec(QPoint(r.x() + height() / 2, r.y()));
            }
            event->accept();
        }
        QAbstractButton::mousePressEvent(event);
    }

    void SearchButton::paintEvent(QPaintEvent* event)
    {
        Q_UNUSED(event);
        QPainterPath myPath;

        int radius = (height() / 5) * 2;
        QRect circle(height() / 3 - 1, height() / 4, radius, radius);
        myPath.addEllipse(circle);

        myPath.arcMoveTo(circle, 300);
        QPointF c = myPath.currentPosition();
        int diff = height() / 7;
        myPath.lineTo(qMin(width() - 2, (int)c.x() + diff), c.y() + diff);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(QPen(Qt::darkGray, 2));
        painter.drawPath(myPath);
        if (_menu) {
            QPainterPath dropPath;
            dropPath.arcMoveTo(circle, 320);
            QPointF c = dropPath.currentPosition();
            c = QPointF(c.x() + 3.5, c.y() + 0.5);
            dropPath.moveTo(c);
            dropPath.lineTo(c.x() + 4, c.y());
            dropPath.lineTo(c.x() + 2, c.y() + 2);
            dropPath.closeSubpath();
            painter.setPen(Qt::darkGray);
            painter.setBrush(Qt::darkGray);
            painter.setRenderHint(QPainter::Antialiasing, false);
            painter.drawPath(dropPath);
        }
        painter.end();
    }

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(SearchLineEdit)
#endif

    /*
            SearchLineEdit is an enhanced QLineEdit
            - A Search icon on the left with optional menu
            - When there is no text and doesn't have focus an "inactive text" is
   displayed
            - When there is text a clear button is displayed on the right hand
   side
 */
    constexpr char SearchLineEdit::_default_tip[];

    SearchLineEdit::SearchLineEdit(QWidget* view)
        : super( //find_screen_,
              //		    [this] { _searchbutton = new SearchButton(this); return _searchbutton;}(),
              view)
        //	    , _find_screen(gl_paras->find_screen()) //dynamic_cast<FindScreen*>(find_screen_)
        , _searchbutton(new SearchButton(this)) //dynamic_cast<SearchButton*>(super::right_widget()) //
                                                //        , _web_view(dynamic_cast<web::WebView*>(view))
    {


        connect(_lineedit, //lineEdit(),
            &QLineEdit::textChanged, this, &SearchLineEdit::textChanged);

        right_widget(_searchbutton);
        _inactivetext = SearchLineEdit::_default_tip; //tr("Search");

        QSizePolicy policy = sizePolicy();
        setSizePolicy(QSizePolicy::Preferred, policy.verticalPolicy());

        connect(
#ifdef USE_CLEAR_BUTTON
            _clearbutton,
#else
            _chasewidget,
#endif // USE_CLEAR_BUTTON

            &ChaseWidget::clicked, // _lineedit, &QLineEdit::clear
            [&](bool checked) {

                (void)checked;
                if (_web_view) {
                    auto _browser = _web_view->browser();
                    if (_browser) {
                        auto blogger_ = _browser->blogger();
                        if (blogger_) {
                            auto topic = blogger_->topic();
                            //					int count_ = _find_screen->_stack_layout->count();
                            //					assert(count_ > 0);
                            //					auto stack = static_cast<QStackedWidget*>(_find_screen->_stack_layout->itemAt(0)->widget());
                            //					if (stack) {
                            //						auto this = static_cast<web::ToolbarSearch*>(stack->currentWidget());
                            //						if (search_edit) {
                            //							auto _lineedit = search_edit->lineEdit();
                            if (_web_view) {
                                auto page = _web_view->page();
                                if (page) {
                                    auto url_ = page->url().toString(); //_browser->tab_widget()->current_item()->field<url_key>();
                                                                        //					auto real_url = to_be_url(_lineedit->text());
                                    if (                                //real_url != QUrl() //
                                        _lineedit->text() == url_) {
                                        //									    if (this->inactiveText() == web::SearchLineEdit::_default_tip) {
                                        this->setInactiveText(topic);
                                        //									    }
                                        synchronize_text(this->inactiveText()); // _lineedit->setText(this->inactiveText());
                                        //						search_edit->left_widget(search_edit->searchbutton());
                                        //						search_edit->left_widget()->show();
                                        //						search_edit->iconlabel()->hide();
                                    } else {
                                        synchronize_text(url_); //_lineedit->setText(url_);
                                        //						search_edit->left_widget(search_edit->iconlabel());
                                        //						search_edit->left_widget()->show();
                                        //						search_edit->searchbutton()->hide();
                                    }
                                    //						}
                                    //					}
                                }
                            }
                        }
                    }
                }

            });
    }

    void SearchLineEdit::paintEvent(QPaintEvent* event)
    {
        if ( //lineEdit()
            _lineedit->text().isEmpty() && !hasFocus() && !_inactivetext.isEmpty()) {
            ExLineEdit::paintEvent(event);
            QStyleOptionFrame panel; // QStyleOptionFrameV2
            initStyleOption(&panel);
            QRect r =
                style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
            QFontMetrics fm = fontMetrics();
            int horizontalMargin = //lineEdit()
                _lineedit->x();
            QRect lineRect(horizontalMargin + r.x(), r.y() + (r.height() - fm.height() + 1) / 2, r.width() - 2 * horizontalMargin, fm.height());
            QPainter painter(this);
            painter.setPen(palette().brush(QPalette::Disabled, QPalette::Text).color());
            painter.drawText(lineRect, Qt::AlignLeft | Qt::AlignVCenter, _inactivetext);
        } else
            ExLineEdit::paintEvent(event);
    }

    void SearchLineEdit::resizeEvent(QResizeEvent* event)
    {
        updateGeometries();
        ExLineEdit::resizeEvent(event);
    }

    void SearchLineEdit::updateGeometries()
    {
        int menuHeight = height();
        int menuWidth = menuHeight + 1;
        if (!_searchbutton->_menu)
            menuWidth = (menuHeight / 5) * 4;
        _searchbutton->resize(QSize(menuWidth, menuHeight));
    }

    QString SearchLineEdit::inactiveText() const
    {
        return _inactivetext;
    }

    void SearchLineEdit::setInactiveText(const QString& text)
    {
        _inactivetext = text;
    }

    void SearchLineEdit::setMenu(QMenu* menu)
    {
        if (_searchbutton->_menu)
            _searchbutton->_menu->deleteLater();
        _searchbutton->_menu = menu;
        updateGeometries();
    }

    SearchButton* SearchLineEdit::searchbutton() const
    {
        return _searchbutton;
    }

    QMenu* SearchLineEdit::menu() const
    {
        if (!_searchbutton->_menu) {
            _searchbutton->_menu = new QMenu(_searchbutton);
            if (isVisible())
                (const_cast<SearchLineEdit*>(this))->updateGeometries();
        }
        return _searchbutton->_menu;
    }
}
