
#include <QEvent>
#include <QScrollBar>
#include <QBoxLayout>

#include "verticalscrollarea.h"
#include "views/record_table/TableView.h"

VerticalScrollArea::VerticalScrollArea(
    std::shared_ptr<sd::_interface<sd::meta_info<void *>, void, QResizeEvent *>> interface
    , QWidget *parent
) :
    QScrollArea(parent)
    , _interface(interface)
{


    setWidgetResizable(true);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //    // how to use VerticalScrollArea class:
    //    auto vertical_scroll_widget = new QWidget(this);
    //    vertical_scroll_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //    QVBoxLayout *baseLayout = new QVBoxLayout(vertical_scroll_widget);
    //    setWidget(vertical_scroll_widget);
    //    vertical_scroll_widget->installEventFilter(this);



    //    setWidgetResizable(true);
    //    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

}

bool VerticalScrollArea::eventFilter(QObject *o, QEvent *e)
{
    // This works because QScrollArea::setWidget installs an eventFilter on the widget
    if(o && o == widget() && e->type() == QEvent::Resize) {

        // deprecated:
        //        setMinimumWidth(widget()->minimumSizeHint().width() + verticalScrollBar()->width());


        //        widget()->setMaximumWidth(width() + verticalScrollBar()->width());
        //        widget()->repaint();

        //        static_cast<RecordTableView *>(o)
        //        //        widget()
        //        ->resizeEvent(static_cast<QResizeEvent *>(e));

        (*_interface)(static_cast<QResizeEvent *>(e));
    }

    return QScrollArea::eventFilter(o, e);
}
