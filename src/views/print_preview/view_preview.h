#ifndef _PREVIEWVIEW_H_
#define _PREVIEWVIEW_H_


#include <QAbstractScrollArea>
#include <QTextDocument>




#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif



class PreviewView : public QAbstractScrollArea {
#if QT_VERSION == 0x050600
    W_OBJECT(PreviewView)
#else
    Q_OBJECT
#endif

    public:
	PreviewView(QTextDocument *document);

	inline void updateLayout(){
	    resizeEvent(0);
	    viewport()->update();
	}
    public slots:
	void	zoom_in();
	void	zoom_out();

    protected:
	virtual void	paintEvent(QPaintEvent *e);
	virtual void	resizeEvent(QResizeEvent *);
	virtual void	mousePressEvent(QMouseEvent *e);
	virtual void	mouseMoveEvent(QMouseEvent *e);
	virtual void	mouseReleaseEvent(QMouseEvent *e);

    private:
	void		paint_page(QPainter *painter, int page);
	QTextDocument	*_doc;
	qreal		_scale			= 1.0;
	int		_inter_page_spacing	= 30;
	QPoint		_mouse_press_pos;
	QPoint		_scroll_bar_values_on_mouse_press;
};

#endif	// _PREVIEWVIEW_H_
