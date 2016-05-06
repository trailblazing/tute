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


extern const int add_new_record_to_end;
extern const int add_new_record_before;
extern const int add_new_record_after;


class ClipboardRecords;
class RecordController;
class RecordScreen;
class TreeItem;
struct PosProxy;
struct PosSource;
struct IndexProxy;
struct IndexSource;
struct IdType;

class RecordView : public QTableView {
    Q_OBJECT

public:
    RecordView(// QString screen_name,
        RecordScreen        *_record_screen
        , RecordController  *_record_controller);

    virtual ~RecordView();

    //    void record_controller(RecordController *_record_controller);

    void init(void);

    void restore_header_state(void);
    void restore_column_width(void);

    template<typename return_type>inline return_type selection_first()const;


    //    PosProxy selection_first_pos(void) const;
    //    IdType selection_first_id(void) const;
    //    IndexProxy selection_first_proxy_index(void) const;
    //    IndexSource selection_first_source_index(void) const;


    void cursor_to_index(boost::intrusive_ptr<TreeItem> it);
    void cursor_to_index(PosProxy pos_proxy_, const int mode = add_new_record_after);
    //    void cursor_to_index(PosProxy pos_proxy_);

    boost::intrusive_ptr<TreeItem> current_item();

    bool is_selected_set_to_top(void);
    bool is_selected_set_to_bottom(void);

    ClipboardRecords *get_selected_records(void);


    //    void on_parent_resizevent(QResizeEvent *e);


signals:

    //    void list_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);
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
    QModelIndex previous_index()const;


protected slots:

    //    // Реакия на сдвиг засветки клавишами или мышкой
    //    void on_selection_changed(const QItemSelection &selected, const QItemSelection &deselected);

    // Слот, который автоматически срабатыват при изменении selection в списке
    // Этот слот нигде не надо прописывать через connect(), так как он
    // является переопределенным, так как его тип virtual protected slot
    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void on_click_to_record(const QModelIndex &proxy_index);

protected:

    QMenu               *_context_menu;
    RecordScreen        *_record_screen;
    RecordController    *_record_controller;
    QVBoxLayout         *_layout;
    QPoint              _mouse_start_position;
    bool                _enable_move_section;
    QModelIndex         _previous_index;

    void setup_signals(void);
    void assembly_context_menu(void);

    //    void editField(int pos, QString name, QString author, QString url, QString tags);
    //    void deleteRecords(void);

    // Реакия на выбор записи мышкой или клавишами
    // The response to the record selection with the mouse or keys
    void click_record(const IndexProxy &proxy_index);

    bool event(QEvent *event);
    bool gesture_event(QGestureEvent *event);
    void tap_and_hold_gesture_triggered(QTapAndHoldGesture *gesture);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void save_column_width(void);

    void resizeEvent(QResizeEvent *e);

private:

    void start_drag();
    friend class RecordScreen;
    friend class VerticalScrollArea;
    friend class RecordController;
};


template<>PosProxy RecordView::selection_first<PosProxy>()const;
template<>IdType RecordView::selection_first<IdType>()const;
template<>IndexProxy RecordView::selection_first<IndexProxy>()const;
template<>IndexSource RecordView::selection_first<IndexSource>()const;


#endif // RECORDLISTSCREEN_H_
