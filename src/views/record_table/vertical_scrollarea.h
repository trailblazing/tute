#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H

#include <wobjectdefs.h>
#include <QObject>

#include <QScrollArea>
#include "utility/delegate.h"
#include "models/tree/binder.hxx"


class rv_t;

class VerticalScrollArea : public QScrollArea {
    W_OBJECT(VerticalScrollArea)

    //    std::shared_ptr<sd::_interface<void(QResizeEvent *), sd::meta_info<void *> > > _interface;
    rv_t *_record_view;
    public:


        explicit VerticalScrollArea(rv_t *_record_view, QWidget *parent = 0);

        virtual bool eventFilter(QObject *obj, QEvent *ev);
    protected:
        void wheelEvent(QWheelEvent *);
        int x = 0;
};


#endif // VERTICALSCROLLAREA_H
