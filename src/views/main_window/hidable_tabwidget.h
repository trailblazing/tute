#ifndef HIDABLETABWIDGET_H
#define HIDABLETABWIDGET_H

#include <QTabWidget>
#include <QAction>
#include <QStackedLayout>

extern const char *custom_hidabletabwidget_style;

class HidableTabWidget : public QTabWidget {
    Q_OBJECT
public:
    explicit HidableTabWidget(QString style_source, QWidget *parent = 0);
    QAction *_hide_action;
//    HidableTabWidget *delegate_tab();
private slots:
    void onHideAction(bool checked);
    void onTabBarClicked();
private:
//    HidableTabWidget *_delegate_tab;
    QStackedLayout *_layout;
    QString _style;
};


#endif // HIDABLETABWIDGET_H
