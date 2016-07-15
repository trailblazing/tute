#ifndef HIDABLETABWIDGET_H
#define HIDABLETABWIDGET_H

#include <wobjectdefs.h>
#include <QObject>

#include <QTabWidget>
#include <QAction>
#include <QStackedLayout>

extern const char *custom_hidabletabwidget_style;

class HidableTabWidget : public QTabWidget {
    W_OBJECT(HidableTabWidget)
    public:
	explicit HidableTabWidget(QString style_source, QWidget *parent = 0);
	QAction *_hide_action;
//    HidableTabWidget *delegate_tab();
    protected slots:
	bool eventFilter(QObject *obj, QEvent *event);
    private slots:
	void	onHideAction(bool checked);
	void	onTabBarClicked();
    private:
//    HidableTabWidget *_delegate_tab;
	QStackedLayout	*_layout;
	QString		_style;
};


#endif	// HIDABLETABWIDGET_H
