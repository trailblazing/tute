#ifndef _EDITORTEXTAREA_H_
#define _EDITORTEXTAREA_H_

#include <QEvent>
#include <QGestureEvent>
#include <QMimeData>
#include <QPaintEvent>
#include <QTapAndHoldGesture>
#include <QTextEdit>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class EditorTextArea : public QTextEdit {
#if QT_VERSION == 0x050600
	W_OBJECT(EditorTextArea)
#else
	Q_OBJECT
#endif

	public:
	EditorTextArea(QWidget *parent = 0);
	~EditorTextArea(void);

	virtual void paintEvent(QPaintEvent *event);
	virtual void resizeEvent(QResizeEvent *event);

	virtual bool canInsertFromMimeData(const QMimeData *source) const;
	virtual void insertFromMimeData(const QMimeData *source);

	bool get_showformatting(void);
	void set_showformatting(bool i);

	int get_indent_started_left(void);
	int get_indent_started_right(void);

	signals:
	void tap_and_hold_gesture_finished(const QPoint &p)
#if QT_VERSION == 0x050600
	    W_SIGNAL(tap_and_hold_gesture_finished, (const QPoint &), p)  //
#else
	    ;
#endif
	    void update_indentline_geometry_signal(void)
#if QT_VERSION == 0x050600
	        W_SIGNAL(update_indentline_geometry_signal)  //
#else
	    ;
#endif

	    public slots : void show_indetedge(bool i);
	void set_indentedge_pos(int i);

	private:
	bool flagShowFormatting;  // исовать ли символы форматирования

	bool flagShowIndentEdge;  // исовать ли линию отступа
	int posIndentEdge;        // По какой координате рисовать линию отступа

	bool event(QEvent *event);
	bool gesture_event(QGestureEvent *event);
	void tap_and_hold_gesture_triggered(QTapAndHoldGesture *gesture);

	virtual bool eventFilter(QObject *o, QEvent *e);
};

#endif  // _EDITORTEXTAREA_H_
