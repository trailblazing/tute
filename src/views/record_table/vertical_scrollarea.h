#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H


#include <QScrollArea>
#include "utility/delegate.h"
#include "models/tree/binder.hxx"


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif



class rv_t;

class VerticalScrollArea : public QScrollArea {
#if QT_VERSION == 0x050600
    W_OBJECT(VerticalScrollArea)
#else
    Q_OBJECT
#endif

	//    std::shared_ptr<sd::_interface<void(QResizeEvent *), sd::meta_info<void *> > > _interface;
    rv_t * _record_view;
    public:


	explicit VerticalScrollArea(rv_t *_record_view, QWidget *parent = 0);

	virtual bool eventFilter(QObject *obj, QEvent *ev);

    protected:
#ifndef QT_NO_WHEELEVENT
	void wheelEvent(QWheelEvent *);
#endif
	int x = 0;
};


#endif	// VERTICALSCROLLAREA_H
