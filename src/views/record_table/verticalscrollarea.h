#ifndef VERTICALSCROLLAREA_H
#define VERTICALSCROLLAREA_H



#include <QScrollArea>
#include "utility/delegate.h"



class VerticalScrollArea : public QScrollArea {
    Q_OBJECT

    std::shared_ptr<sd::_interface<sd::meta_info<void *>, void, QResizeEvent *>> _interface;

public:


    explicit VerticalScrollArea(
        std::shared_ptr<sd::_interface<sd::meta_info<void *>, void, QResizeEvent *>> interface
        , QWidget *parent = 0
    );

    virtual bool eventFilter(QObject *o, QEvent *e);

};


#endif // VERTICALSCROLLAREA_H
