#ifndef RECORDLISTSCREEN_H_
#define RECORDLISTSCREEN_H_


#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>


#include <QWidget>
#include <QTableView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStringList>
#include <QItemSelection>
#include <QMenu>
#include <QTapAndHoldGesture>
#include <QEvent>
#include <QGestureEvent>

#include <QStyledItemDelegate> // #include <QItemDelegate>
#include "libraries/FlatControl.h"
#include "libraries/FixedParameters.h"

extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;


class ClipboardRecords;
class RecordController;
class RecordScreen;
class TreeItem;
struct pos_proxy;
struct pos_source;
struct index_proxy;
struct index_source;
struct id_value;
class RecordView;
class QStyleOptionButton;


#include <QMetaType>
#include <QPointF>
#include <QVector>



//! [0]
class StarRating {
    public:
        enum EditMode {Editable, ReadOnly};

        explicit StarRating(int star_count = 1, int max_star_count = 5);

        void paint(QPainter *painter, const QRect &rect, const QPalette &palette, EditMode mode) const;
        QSize sizeHint() const;
        int star_count() const {return _star_count; }
        int max_star_count() const {return _max_star_count; }
        void star_count(int starCount){_star_count = starCount; }
        void max_star_count(int maxStarCount){_max_star_count = maxStarCount; }

    private:
        QPolygonF star_polygon;
        QPolygonF diamond_polygon;
        int _star_count;
        int _max_star_count;
};
//! [0]

//! [1]
Q_DECLARE_METATYPE(StarRating)


class FlatToolButtonRating : public FlatToolButton {
    Q_OBJECT
    public:
        FlatToolButtonRating(QWidget *parent = nullptr);
        void star_rating(const StarRating &starRating){_star_rating = starRating; }
        StarRating star_rating(){return _star_rating; }

    signals:
        void editingFinished();
    protected:
        void paintEvent(QPaintEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
    private:

        StarRating _star_rating;
};


class ViewDelegation : public QStyledItemDelegate // QItemDelegate
{
    Q_OBJECT
    public:
        explicit ViewDelegation(RecordView *view = 0);

        // basic function for a read-only delegate, you can draw anything with the painter
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
        // for a editable delegate
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    protected:
        bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

    private slots:
        void commitAndCloseEditor();

    private:
        RecordView  *_view;
};

Q_DECLARE_METATYPE(QStyleOptionButton)

class RecordView : public QTableView {
    Q_OBJECT

    public:
        RecordView(RecordScreen         *record_screen // QString screen_name,
                  , RecordController    *_record_controller);

        virtual ~RecordView();

        // void record_controller(RecordController *_record_controller);
        // void init(void);

        void restore_header_state(void);
        void restore_column_width(void);

        template<typename return_type>inline return_type selection_first() const;


        // PosProxy selection_first_pos(void) const;
        // IdType selection_first_id(void) const;
        // IndexProxy selection_first_proxy_index(void) const;
        // IndexSource selection_first_source_index(void) const;


        boost::intrusive_ptr<TreeItem> current_item()const;

        bool is_selected_set_to_top(void);
        bool is_selected_set_to_bottom(void);

        ClipboardRecords *get_selected_records(void);
        RecordController *record_controller();

        // void on_parent_resizevent(QResizeEvent *e);
        template <typename field_type>
        bool is_field_type_column(int index){
            return _is_field_type_column(boost::mpl::c_str<field_type>::value, index);
        }
    signals:

        // void list_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);
        void tap_and_hold_gesture_finished(const QPoint &);


    public slots:

        // Открытие контекстного меню
        void on_custom_context_menu_requested(const QPoint &pos);

        // Слот, срабатывающий после перетаскивания колонки
        void on_section_moved(int logicalIndex, int oldVisualIndex, int newVisualIndex);
        void on_section_resized(int logicalIndex, int oldSize, int newSize);

        // Вызов действий из контекстного меню или из контроллера для редактирования инфополей записи
        void edit_field_context(void);
        void on_doubleclick(const QModelIndex &index);
        QModelIndex previous_index() const;


    protected slots:

        //// Реакия на сдвиг засветки клавишами или мышкой
        // void on_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);

        // Слот, который автоматически срабатыват при изменении selection в списке
        // Этот слот нигде не надо прописывать через connect(), так как он
        // является переопределенным, так как его тип virtual protected slot
        virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
        bool eventFilter(QObject *obj, QEvent *event);
        void on_click(const QModelIndex &proxy_index);

    protected:

        QMenu               *_context_menu;
        RecordScreen        *_record_screen;
        RecordController    *_record_controller;
        QVBoxLayout         *_layout;
        QPoint _mouse_start_position;
        bool _enable_move_section;
        QModelIndex _previous_index;
        int x = 0;  // test

        // void setup_signals(void);
        // void assembly_context_menu(void);

        // void editField(int pos, QString name, QString author, QString url, QString tags);
        // void deleteRecords(void);

        // еакия на выбор записи мышкой или клавишами
        // The response to the record selection with the mouse or keys
        // void click_record(const IndexProxy &proxy_index);

        bool event(QEvent *event);
        bool gesture_event(QGestureEvent *event);
        void tap_and_hold_gesture_triggered(QTapAndHoldGesture *gesture);

        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
#endif
        void save_column_width(void);

        void resizeEvent(QResizeEvent *e);

    private:
        std::function<bool (const QString &, int)> _is_field_type_column;
        void start_drag();
        friend class RecordScreen;
        friend class VerticalScrollArea;
        friend class RecordController;
};


template<>pos_proxy RecordView::selection_first<pos_proxy>() const;
template<>pos_source RecordView::selection_first<pos_source>() const;
template<>id_value RecordView::selection_first<id_value>() const;
template<>index_proxy RecordView::selection_first<index_proxy>() const;
template<>index_source RecordView::selection_first<index_source>() const;
template<>boost::intrusive_ptr<TreeItem> RecordView::selection_first<boost::intrusive_ptr<TreeItem> >() const;

#endif // RECORDLISTSCREEN_H_
