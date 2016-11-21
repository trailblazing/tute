#ifndef HIDABLETABWIDGET_H
#define HIDABLETABWIDGET_H

#include <set>
#include <functional>

#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>


#include <QTabWidget>
#include <QAction>
#include <QStackedLayout>



#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif


extern const char *custom_hidabletabwidget_style;
namespace browser {
    class WebView;
    class Browser;
    class Profile;
    class Entrance;
}
struct Binder;
class ts_t;
class FindScreen;
class MetaEditor;
class wn_t;
class rs_t;


class HidableTabWidget : public QTabWidget {
#if QT_VERSION == 0x050600
    W_OBJECT(HidableTabWidget)
#else
    Q_OBJECT
#endif

    public:
	explicit HidableTabWidget(ts_t *_tree_screen
				 , FindScreen *_find_screen
				 , MetaEditor *_editor_screen
				 , browser::Entrance *_entrance
				 , wn_t *_main_window
				 , browser::Profile *_profile, QString style_source_);
	~HidableTabWidget();
	QAction				*_hide_action;
	std::set<rs_t *>		record_screens()const;
	browser::WebView		*find(const std::function<bool (boost::intrusive_ptr<const ::Binder>)> &_equal) const;
	browser::Browser		*new_browser();
	browser::Browser		*activated_browser();
//    HidableTabWidget *delegate_tab();
    protected slots:
	bool eventFilter(QObject *obj, QEvent *event);
    private slots:
	void	onHideAction(bool checked);
	void	onTabBarClicked();
    private:
//    HidableTabWidget *_delegate_tab;
	QStackedLayout					*_layout;
	ts_t						*_tree_screen;
	FindScreen					*_find_screen;
	MetaEditor					*_editor_screen;
	browser::Entrance				*_entrance;
	wn_t						*_main_window;
	browser::Profile				*_profile;
	QString						_style_source;
//	std::set<browser::Browser *>			_browsers;
//	std::set<rs_t *>				_record_screens;
};


#endif	// HIDABLETABWIDGET_H
