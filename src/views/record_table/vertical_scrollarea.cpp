

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QBoxLayout>
#include <QEvent>
#include <QScrollBar>

#include "vertical_scrollarea.h"
#include "views/record_table/record_view.h"

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(VerticalScrollArea)
#endif

VerticalScrollArea::VerticalScrollArea(
    rv_t* _record_view, // std::shared_ptr<sd::_interface<void(QResizeEvent *),// sd::meta_info<void >>> interface
    QWidget* parent)
    : QScrollArea(parent)
    , _record_view(_record_view)
{ // _interface(interface)
	setWidgetResizable(true);
	setFrameStyle(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	//// how to use VerticalScrollArea class:
	// auto vertical_scroll_widget = new QWidget(this);
	// vertical_scroll_widget->setSizePolicy(QSizePolicy::Preferred,
	// QSizePolicy::Preferred);
	// QVBoxLayout *baseLayout = new QVBoxLayout(vertical_scroll_widget);
	// setWidget(vertical_scroll_widget);
	// vertical_scroll_widget->installEventFilter(this);

	// RecordView *_record_view = _record_controller->view(); //    auto
	// record_view = new QWidget(this);

	_record_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

	setWidget(_record_view);
	_record_view->viewport()->installEventFilter(this);
	//	// _record_view->_vertical_scroll_bar_width	=
	//	// this->verticalScrollBar()->width();
	//	_record_view->_vertical_scrollarea = this;
	//	// setWidgetResizable(true);
	//	// setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//	// setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setContentsMargins(0, 0, 0, 0);
}

bool VerticalScrollArea::eventFilter(QObject* obj, QEvent* ev)
{
	// This works because QScrollArea::setWidget installs an eventFilter on the
	// widget
	if (obj && obj == widget() && ev->type() == QEvent::Resize) {
		// deprecated:
		// setMinimumWidth(widget()->minimumSizeHint().width() +
		// verticalScrollBar()->width());

		// widget()->setMaximumWidth(width() + verticalScrollBar()->width());
		// widget()->repaint();

		// static_cast<RecordTableView *>(o)
		////        widget()
		// ->resizeEvent(static_cast<QResizeEvent *>(e));

		_record_view->resizeEvent(static_cast<QResizeEvent*>(ev)); // (*_interface)(static_cast<QResizeEvent *>(ev));
	}
	return QScrollArea::eventFilter(obj, ev);
}

void VerticalScrollArea::wheelEvent(QWheelEvent* event)
{ // does not work
	x -= (event->delta() / 120) * 10;
	verticalScrollBar()->setValue(x);
}
