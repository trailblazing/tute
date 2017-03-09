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

//extern const char *custom_hidable_tab_style;
namespace web {
class WebView;
class Browser;
class Profile;
class Docker;
}
struct Binder;
class ts_t;
class FindScreen;
class EditorDock;
class wn_t;
class rs_t;
class QSplitter;
class Blogger;
class QMainWindow;

class HidableTab : public QTabWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(HidableTabWidget)
#else
	Q_OBJECT
#endif

	public:
	explicit HidableTab(QWidget *main_window, QSplitter *splitter, std::shared_ptr<QSettings> splitter_config, QString splitter_group_name, QString splitter_sizelist_name, QString collapsed_status_name, QWidget *parent);
	~HidableTab();
	QAction *_hide_action;

	// HidableTabWidget *delegate_tab();
	std::tuple<int, QList<int>> on_splitter_moved(int record_pos, int index);
	protected slots:
	bool eventFilter(QObject *obj, QEvent *event);
	void on_tabbar_clicked(int index);
	public slots:
	void collapse_when_true(bool checked);
	// void onTabBarClicked(int index);
	void on_child_self_close_requested(QWidget *child_);

	protected:
	// HidableTabWidget *_delegate_tab;
	// QStackedLayout					*_layout;
	QVBoxLayout *_layout;
	//	ts_t *_tree_screen;
	//	FindScreen *_find_screen;
	//	web::Docker *_browser_docker;
	QWidget *_main_window;
	//	web::Profile *_profile;
	//	QString _style_source;
	// std::set<web::Browser *>			_browsers;
	// std::set<rs_t *>				_record_screens;
	QSplitter *_splitter;
	std::shared_ptr<QSettings> _splitter_config;
	QString _splitter_group_name;
	QString _splitter_sizelist_name;
	QString _collapsed_status_name;
	friend class SideWidget;
};

#endif  // HIDABLETABWIDGET_H
