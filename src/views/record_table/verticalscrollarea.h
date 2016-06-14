#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H



#include <QScrollArea>
#include "utility/delegate.h"
#include "models/tree/binder.hxx"


class RecordView;

class VerticalScrollArea : public QScrollArea {
    Q_OBJECT

    //    std::shared_ptr<sd::_interface<void(QResizeEvent *), sd::meta_info<void *> > > _interface;
    RecordView *_record_view;
    public:


        explicit VerticalScrollArea(RecordView *_record_view, QWidget *parent = 0);

        virtual bool eventFilter(QObject *obj, QEvent *ev);
    protected:
        void wheelEvent(QWheelEvent *);
        int x = 0;
};


#endif // VERTICALSCROLLAREA_H
