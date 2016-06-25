#ifndef _PREVIEWVIEW_H_
#define _PREVIEWVIEW_H_

#include <QAbstractScrollArea>
#include <QTextDocument>


class PreviewView : public QAbstractScrollArea {
    Q_OBJECT

public:
    PreviewView(QTextDocument *document);

    inline void updateLayout()
    {
        resizeEvent(0);
        viewport()->update();
    }

public slots:
    void zoom_in();
    void zoom_out();

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void resizeEvent(QResizeEvent *);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    void paint_page(QPainter *painter, int page);
    QTextDocument   *_doc;
    qreal           _scale;
    int             _inter_page_spacing;
    QPoint          _mouse_press_pos;
    QPoint          _scroll_bar_values_on_mouse_press;
};

#endif // _PREVIEWVIEW_H_
