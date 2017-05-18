#ifndef RECORDLISTSCREEN_H_
#define RECORDLISTSCREEN_H_

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>


#include <QEvent>
#include <QGestureEvent>
#include <QHBoxLayout>
#include <QItemSelection>
#include <QMenu>
#include <QStringList>
#include <QTableView>
#include <QTapAndHoldGesture>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include <QStyledItemDelegate> // #include <QItemDelegate>

//#include "libraries/fixed_parameters.h"
#include "libraries/flat_control.h"
#include "libraries/global_parameters.h"
#include "utility/lease.h"

extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;

extern enum QItemSelectionModel::SelectionFlag current_record_selection_mode;
extern enum QItemSelectionModel::SelectionFlag current_record_current_index_mode;

class ClipboardRecords;
class rctrl_t;
class rs_t;
class i_t;
struct pos_proxy;
struct pos_source;
struct index_proxy;
struct index_source;
struct id_value;
class rv_t;
class QStyleOptionButton;
class FlatToolButton;
class VerticalScrollArea;

#include <QMetaType>
#include <QPointF>
#include <QVector>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

// #ifndef USE_BUTTON
// #define USE_BUTTON
// #endif

#ifndef USE_TEXT_AS_BUTTON
#define USE_TEXT_AS_BUTTON
#endif

#ifdef USE_STAR_RATING

// ! [0]
class StarRating {
    public:
	enum EditMode { Editable,
		ReadOnly };

	explicit StarRating(int star_count = 1, int max_star_count = 5);

	void paint(QPainter* painter, const QRect& rect, const QPalette& palette, EditMode mode) const;
	QSize sizeHint() const;
	int star_count() const
	{
		return _star_count;
	}

	int max_star_count() const
	{
		return _max_star_count;
	}

	void star_count(int star_count_)
	{
		_star_count = star_count_;
	}

	void max_star_count(int max_star_count_)
	{
		_max_star_count = max_star_count_;
	}

    private:
	QPolygonF _star_polygon;
	QPolygonF _diamond_polygon;
	int _star_count;
	int _max_star_count;
};
// ! [0]

// ! [1]
Q_DECLARE_METATYPE(StarRating)

class FlatToolButtonRating : public FlatToolButton {
#if QT_VERSION == 0x050600
	W_OBJECT(FlatToolButtonRating)
#else
	Q_OBJECT
#endif

    public:
	FlatToolButtonRating(QWidget* parent = nullptr);
	void star_rating(const StarRating& star_rating_)
	{
		_star_rating = star_rating_;
	}

	StarRating star_rating()
	{
		return _star_rating;
	}

    signals:
	void editingFinished()
#if QT_VERSION == 0x050600
	    W_SIGNAL(editingFinished) //
#else
	    ;
#endif
	    protected : void paintEvent(QPaintEvent* e);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

    private:
	StarRating _star_rating;
};

#endif

#ifdef USE_BUTTON_COLUMN
// #include <QStyledItemDelegate>
// #include <QWidget>
// #include <QPushButton>
// #include <QTableView>
class QPushButton;

class ButtonColumnDelegate : public QStyledItemDelegate {
	Q_OBJECT

    public:
	explicit ButtonColumnDelegate(rv_t* view = 0); // (QObject *parent = 0);
	~ButtonColumnDelegate();

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

	void setEditorData(QWidget* editor, const QModelIndex& index) const;

	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    public slots:
	void cellEntered(const QModelIndex& index);

    private:
	// QTableView		*_view;
	QPushButton* _button;
	rv_t* _view;
	const int _rating_width = 30;

	bool _is_one_cell_in_edit_mode;
	QPersistentModelIndex _current_edited_cell_index;
	friend class rv_t;
};

#endif

class rv_t;

class ViewDelegation : public QStyledItemDelegate // QItemDelegate
{
#if QT_VERSION == 0x050600
	W_OBJECT(ViewDelegation)
#else
	Q_OBJECT
#endif
    public:
	explicit ViewDelegation(rv_t* view = 0);

	// basic function for a read-only delegate, you can draw anything with the
	// painter
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	// for a editable delegate
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    protected:
	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);

    private slots:
	void commitAndCloseEditor();

    private:
	rv_t* _view;
	const int _scroll_bar_width = 10;
	const int _rating_width = 30;
	int _x_offset;
	friend class rv_t;
};

Q_DECLARE_METATYPE(QStyleOptionButton)

// W_REGISTER_ARGTYPE(rv_t)
// W_REGISTER_ARGTYPE(rs_t)
// W_REGISTER_ARGTYPE(rctl_t)

class rv_t : public QTableView
#ifdef USE_SIGNAL_CLOSE
	     ,
	     public boost::intrusive_ref_counter<rctrl_t, boost::thread_safe_counter>,
	     public sd::renter
#endif // USE_SIGNAL_CLOSE
{
#if QT_VERSION == 0x050600
	W_OBJECT(rv_t)
#else
	Q_OBJECT
#endif
    public:
	typedef QTableView super;
	rv_t(rs_t* record_screen_, rctrl_t* record_ctrl_); // W_CONSTRUCTOR(rs_t   *, rctl_t  *)
	// // QString screen_name,

	virtual ~rv_t();

	// void record_controller(RecordController *_record_controller);
	// void init(void);

	void restore_header_state(void);
	void restore_column_width(void);

	template <typename return_type>
	inline return_type selection_first() const;

	// PosProxy selection_first_pos(void) const;
	// IdType selection_first_id(void) const;
	// IndexProxy selection_first_proxy_index(void) const;
	// IndexSource selection_first_source_index(void) const;

	boost::intrusive_ptr<i_t> current_item() const;

	bool is_selected_set_to_top(void);
	bool is_selected_set_to_bottom(void);

	ClipboardRecords* get_selected_records(void);
	rctrl_t* record_ctrl();

	// void on_parent_resizevent(QResizeEvent *e);
	//	template <typename field_type>
	//	bool is_field_type_column(int index){return
	//_is_field_type_column(boost::mpl::c_str<field_type>::value, index);}
	std::function<bool(const QString&, int)> is_field_type_column;

	std::function<int()> rating_width;
	VerticalScrollArea* vertical_scrollarea() const;
    signals:
	void tabMoved(int from, int to);
// void list_selection_changed(const QItemSelection &selected, const
// QItemSelection &deselected);

#if QT_VERSION == 0x050600
	void tap_and_hold_gesture_finished(const QPoint& p) W_SIGNAL(tap_and_hold_gesture_finished, (const QPoint&), p); //
#else
	void tap_and_hold_gesture_finished(const QPoint& p);
#endif

	// int	vertical_scroll_bar_width() const;
	// void	vertical_scroll_bar_width(int w);
    public slots:

	// Открытие контекстного меню
	void on_custom_context_menu_requested(const QPoint& pos); // W_SLOT(on_custom_context_menu_requested)

	// Слот, срабатывающий после перетаскивания колонки
	void on_section_moved(int logicalIndex, int oldVisualIndex,
	    int newVisualIndex); // W_SLOT(on_section_moved)
	void on_section_resized(int logicalIndex, int oldSize,
	    int newSize); // W_SLOT(on_section_resized)

	// Вызов действий из контекстного меню или из контроллера для редактирования
	// инфополей записи
	void edit_field_context(void);                 // W_SLOT(edit_field_context)
	void on_doubleclick(const QModelIndex& index_proxy_); // W_SLOT(on_doubleclick)
						       //	QModelIndex previous_index() const;            // W_SLOT(previous_index)

	void assembly_context_menu(const std::map<std::string, QMenu*>& main_menu_map_);
    protected slots:

	//// Реакия на сдвиг засветки клавишами или мышкой
	// void on_selection_changed(const QItemSelection &selected, const
	// QItemSelection &deselected);

	// Слот, который автоматически срабатыват при изменении selection в списке
	// Этот слот нигде не надо прописывать через connect(), так как он
	// является переопределенным, так как его тип virtual protected slot
	virtual void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected); // W_SLOT(selectionChanged, W_Access::Protected)	// ;
	bool eventFilter(QObject* obj, QEvent* event);                                                   // W_SLOT(eventFilter, W_Access::Protected)// ;
	void on_click(const QModelIndex& index_proxy_);                                                  // W_SLOT(on_click, W_Access::Protected)	// ;

    protected:
	QMenu* _context_menu;
	rs_ref _record_screen;
	rctrl_ref _rctrl;

	QVBoxLayout* _layout;
	ViewDelegation* _delegate; // ButtonColumnDelegate	*_delegate;
	QPoint _mouse_start_position;
	bool _enable_move_section = true;
	QModelIndex _previous_index;

	// int			_vertical_scroll_bar_width	= 0;
	int x = 0; // test
	const int _scroll_bar_width = 10;
	const int _pin_width = 25;
	const int _rating_width = 30;

	VerticalScrollArea* _vertical_scrollarea = nullptr;
	// void setup_signals(void);
	// void assembly_context_menu(void);

	// void editField(int pos, QString name, QString author, QString url, QString
	// tags);
	// void deleteRecords(void);

	// еакия на выбор записи мышкой или клавишами
	// The response to the record selection with the mouse or keys
	// void click_record(const IndexProxy &proxy_index);

	bool event(QEvent* event);
	bool gesture_event(QGestureEvent* event);
	void tap_and_hold_gesture_triggered(QTapAndHoldGesture* gesture);

	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	// #ifndef QT_NO_WHEELEVENT
	// void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
	// #endif
	void save_column_width(void);
	// bool	is_vertical_scrollbar_visible() const;
	void resizeEvent(QResizeEvent* e);

    private:
	void start_drag();
	friend class rs_t;
	friend class VerticalScrollArea;
	friend class rctrl_t;
	friend class ViewDelegation;
};

template <>
pos_proxy rv_t::selection_first<pos_proxy>() const;
template <>
pos_source rv_t::selection_first<pos_source>() const;
template <>
id_value rv_t::selection_first<id_value>() const;
template <>
index_proxy rv_t::selection_first<index_proxy>() const;
template <>
index_source rv_t::selection_first<index_source>() const;
template <>
boost::intrusive_ptr<i_t>
rv_t::selection_first<boost::intrusive_ptr<i_t>>() const;

#endif // RECORDLISTSCREEN_H_
