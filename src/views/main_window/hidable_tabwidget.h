#ifndef HIDABLETABWIDGET_H
#define HIDABLETABWIDGET_H

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <functional>
#include <memory>
#include <set>

#include <QAction>
#include <QSettings>
#include <QStackedLayout>
#include <QTabWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

extern const char* custom_hidabletabwidget_style;
namespace browser {
class WebView;
class Browser;
class Profile;
class BrowserDock;
}
struct Binder;
class ts_t;
class FindScreen;
class EditorDock;
class wn_t;
class rs_t;
class QSplitter;
class EditingWindow;
class QMainWindow;

class HidableTabWidget : public QTabWidget {
#if QT_VERSION == 0x050600
    W_OBJECT(HidableTabWidget)
#else
    Q_OBJECT
#endif

public:
    explicit HidableTabWidget(ts_t* _tree_screen, FindScreen* _find_screen, browser::BrowserDock* _entrance, QMainWindow* _main_window, browser::Profile* _profile, QString style_source_, QSplitter* splitter, std::shared_ptr<QSettings> splitter_config, QString splitter_group_name, QString splitter_sizelist_name, QString collapsed_status_name, QWidget* parent);
    ~HidableTabWidget();
    QAction* _hide_action;

    //    HidableTabWidget *delegate_tab();
    std::tuple<int, QList<int> > inner_rebuild_on_splitter_moved(int record_pos, int index);
protected slots:
    bool eventFilter(QObject* obj, QEvent* event);
    void inner_and_spiltter_on_click(int index);
public slots:
    void collapse_when_true(bool checked);
    //	void onTabBarClicked(int index);
protected:
    //    HidableTabWidget *_delegate_tab;
    //		QStackedLayout					*_layout;
    QVBoxLayout* _layout;
    ts_t* _tree_screen;
    FindScreen* _find_screen;
    browser::BrowserDock* _entrance;
    QMainWindow* _main_window;
    browser::Profile* _profile;
    QString _style_source;
    //	std::set<browser::Browser *>			_browsers;
    //	std::set<rs_t *>				_record_screens;
    QSplitter* _splitter;
    std::shared_ptr<QSettings> _splitter_config;
    QString _splitter_group_name;
    QString _splitter_sizelist_name;
    QString _collapsed_status_name;
    friend class SideWidget;
};

#endif // HIDABLETABWIDGET_H
