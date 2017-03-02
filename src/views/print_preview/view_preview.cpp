

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QAbstractScrollArea>
#include <QAbstractTextDocumentLayout>
#include <QColor>
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QTextDocument>

#include "view_preview.h"

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(PreviewView)
#endif

PreviewView::PreviewView(QTextDocument* document)
    : _doc(document)
{
    verticalScrollBar()->setSingleStep(20);
    horizontalScrollBar()->setSingleStep(20);

    viewport()->setBackgroundRole(QPalette::Dark);

    // _doc		= document;
    // _scale		= 1.0;
    // _inter_page_spacing = 30;
}

void PreviewView::zoom_in()
{
    _scale += 0.2;
    resizeEvent(0);
    viewport()->update();
}

void PreviewView::zoom_out()
{
    _scale -= 0.2;
    resizeEvent(0);
    viewport()->update();
}

void PreviewView::paintEvent(QPaintEvent*)
{
    QPainter p(viewport());

    p.translate(-horizontalScrollBar()->value(), -verticalScrollBar()->value());
    p.translate(_inter_page_spacing, _inter_page_spacing);

    const int pages = _doc->pageCount();
    for (int i = 0; i < pages; ++i) {
        p.save();
        p.scale(_scale, _scale);

        paint_page(&p, i);

        p.restore();
        p.translate(0, _inter_page_spacing + _doc->pageSize().height() * _scale);
    }
}

void PreviewView::paint_page(QPainter* painter, int page)
{
    const QSizeF pgSize = _doc->pageSize();

    QColor col(Qt::black);

    painter->setPen(col);
    painter->setBrush(Qt::white);
    painter->drawRect(QRectF(QPointF(0, 0), pgSize));
    painter->setBrush(Qt::NoBrush);

    col = col.light();
    painter->drawLine(QLineF(pgSize.width(), 1, pgSize.width(), pgSize.height() - 1));

    col = col.light();
    painter->drawLine(QLineF(pgSize.width(), 2, pgSize.width(), pgSize.height() - 2));

    QRectF docRect(QPointF(0, page * pgSize.height()), pgSize);
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = docRect;

    // don't use the system palette text as default text color, on HP/UX
    // for example that's white, and white text on white paper doesn't
    // look that nice
    ctx.palette.setColor(QPalette::Text, Qt::black);

    painter->translate(0, -page * pgSize.height());
    painter->setClipRect(docRect);
    _doc->documentLayout()->draw(painter, ctx);
}

void PreviewView::resizeEvent(QResizeEvent*)
{
    const QSize viewportSize = viewport()->size();

    QSize docSize;
    docSize.setWidth(qRound(_doc->pageSize().width() * _scale + 2 * _inter_page_spacing));
    const int pageCount = _doc->pageCount();
    docSize.setHeight(qRound(pageCount * _doc->pageSize().height() * _scale + (pageCount + 1) * _inter_page_spacing));

    horizontalScrollBar()->setRange(0, docSize.width() - viewportSize.width());
    horizontalScrollBar()->setPageStep(viewportSize.width());

    verticalScrollBar()->setRange(0, docSize.height() - viewportSize.height());
    verticalScrollBar()->setPageStep(viewportSize.height());
}

void PreviewView::mousePressEvent(QMouseEvent* e)
{
    _mouse_press_pos = e->pos();
    _scroll_bar_values_on_mouse_press.rx() = horizontalScrollBar()->value();
    _scroll_bar_values_on_mouse_press.ry() = verticalScrollBar()->value();
    e->accept();
}

void PreviewView::mouseMoveEvent(QMouseEvent* e)
{
    if (_mouse_press_pos.isNull()) {
        e->ignore();

        return;
    }
    horizontalScrollBar()->setValue(_scroll_bar_values_on_mouse_press.x() - e->pos().x() + _mouse_press_pos.x());
    verticalScrollBar()->setValue(_scroll_bar_values_on_mouse_press.y() - e->pos().y() + _mouse_press_pos.y());
    horizontalScrollBar()->update();
    verticalScrollBar()->update();
    e->accept();
}

void PreviewView::mouseReleaseEvent(QMouseEvent* e)
{
    _mouse_press_pos = QPoint();
    e->accept();
}
