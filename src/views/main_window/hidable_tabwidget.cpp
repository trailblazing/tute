

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif




#include "hidable_tabwidget.h"
#include <QTabBar>
#include <QToolButton>
#include <QLayout>
#include "libraries/global_parameters.h"
#include "views/tree/tree_view.h"
#include "views/browser/entrance.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "views/main_window/main_window.h"


extern GlobalParameters globalparameters;
extern const char	*record_screen_multi_instance_name;
const char		*custom_hidabletabwidget_style =
    "QTabWidget::pane {"
//    "width: 250px;"	// content space, can not change easily if set here
    "border: 0 px;"
    "}"
    "QTabBar::tab {"
//    "left 3 px;"
//    "background: transparent;"
//    "background-color: transparent;"
    "border-color: white;"
    "border-width: 0 px;"
    "border-style: solid;"
    "alignment: bottom;"
    "max-height: 100px;"
    "width: 26 px;"	// 16
    "max-width: 28px;"
    "qproperty-alignment: AlignBottom;"			//  AlignCenter | AlignVCenter;*/
    "qproperty-wordWrap: false;"
    "padding-bottom: 5px;"
//    "padding: 0 px;"
    "margin-left: 2 px;"
    "margin-right: 0 px;"
    "}"

    "QTabWidget::tab-bar {"
//    "background: transparent;"
//    "border-color: transparent;"
    "border-width: 0 px;"
    "border-style: solid;"
    "background-color: transparent;"
    "alignment: bottom;"
    "width: 28px;"	// 16
    "max-width : 30 px;"
    "text-align: left;"
    "qproperty-alignment: AlignBottom;"
    "qproperty-wordWrap: false;"
    "padding : 0 px;"
    "padding-bottom: 0px;"
//    "padding-left: 0px;"
    "margin-left : 0 px;"	// 0
    "margin-right : 0 px;"	// 0
    "}"
    "QTabWidget{"
//    "width: 250px;" // nake fiexed size
    "background: transparent;"
    "border-color: transparent;"
    "}"
    "QTabBar::tab:selected, QTabBar::tab:hover {"
    "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fafafa, stop: 0.4 #f4f4f4, stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);"
    "font-color: black;"
    "}"
    "QTabBar::tab:selected {"
    "border-color: #9B9B9B;"
    "border-bottom-color: white;"	// #C2C7CB         /* same as pane color */
    "background-color: white;"
    "}"
    "QTabBar::tab:!selected {"
    "margin-left: 0px;"		/* make non-selected tabs look smaller */
    "background-color: transparent;"
    "border-bottom-color: transparent;"
    "}"
;


#if QT_VERSION == 0x050600
W_OBJECT_IMPL(HidableTabWidget)
#endif


HidableTabWidget::HidableTabWidget(ts_t *_tree_screen
				  , FindScreen *_find_screen
				  , MetaEditor *_editor_screen
				  , browser::Entrance *_entrance
				  , wn_t *_main_window
				  , browser::Profile *_profile
				  , QString _style_source)
    : QTabWidget(_main_window)
      , _hide_action(new QAction(tr("▾"), this))
      , _layout(new QStackedLayout(this))
      , _tree_screen(_tree_screen)
      , _find_screen(_find_screen)
      , _editor_screen(_editor_screen)
      , _entrance(_entrance)
      , _main_window(_main_window)
      , _profile(_profile)
      , _style_source(_style_source){
//    , _delegate_tab(_delegate_tab)
    _hide_action->setCheckable(true);
    _hide_action->setToolTip("Hide Panels");
    QToolButton *hide_button = new QToolButton();
    hide_button->setDefaultAction(_hide_action);
    hide_button->setAutoRaise(true);
    this->setCornerWidget(hide_button);


    setWindowFlags(	// Qt::Window |
	Qt::FramelessWindowHint
	// |Qt::Popup
	| Qt::CustomizeWindowHint
//        | Qt::SplashScreen   // http://www.qtforum.org/article/20174/how-to-create-borderless-windows-with-no-title-bar.html?s=86e2c5a6509f28a482adbb7d9f3654bb2058a301#post75829
	// | Qt::DockWidgetArea::NoDockWidgetArea
	| Qt::MaximizeUsingFullscreenGeometryHint
	);

    setAutoFillBackground(true);
    adjustSize();


    setTabPosition(TabPosition::West);		// South
    setTabShape(TabShape::Triangular);
    setTabsClosable(true);
    connect(this, &::HidableTabWidget::tabCloseRequested, [&](int index){
	    auto w = widget(index);
	    this->removeTab(index);
	    if(w->objectName() == record_screen_multi_instance_name){
//		auto rs = dynamic_cast<rs_t *>(w);
		auto _browser = dynamic_cast<rs_t *>(w)->browser();
		if(_browser){
		    _browser->close();
//		    if(_record_screens.find(rs) != _record_screens.end())_record_screens.erase(rs);
		    _browser->deleteLater();
		}
	    }else{
		w->close();
		w->deleteLater();
	    }
	    w = nullptr;
	});

    connect(this, &::HidableTabWidget::currentChanged, [&](int index){
	    auto w = widget(index);
	    if(w->objectName() == record_screen_multi_instance_name){
//		auto rs = dynamic_cast<rs_t *>(w);
		auto _browser = dynamic_cast<rs_t *>(w)->browser();
		if(_browser){
//		    if(_record_screens.find(rs) != _record_screens.end())_record_screens.erase(rs);
		    auto v = _browser->currentTab();
		    auto it = v->page()->host();
		    if(_tree_screen->view()->current_item()->id() != it->id())_tree_screen->view()->select_as_current(TreeIndex::create_treeindex_from_item([&] {return _tree_screen->view()->source_model();}, it));
		}
	    }
	    w = nullptr;
	});
    setUsesScrollButtons(true);
	//    setStyleSheet("QTabBar::tab { max-width: 200px; padding: 2px; margin-left: 2px; }");
//    setStyleSheet("QTabWidget::pane { border: 0 px; } QTabBar::tab { max-width: 200px; padding: 0 px; margin-left: 2 px; margin-right: 0 px;} QTabWidget::tab-bar { max-width: 200px; align: left; text-align: left; margin-left: 2 px; padding: 0 px; margin-right: 0 px;}");    // QWidget{border: 0px;}
//    setStyleSheet("QTabWidget::pane { border: 0 px; }");

//    QSettings settings;
//    settings.beginGroup(QLatin1String("websettings"));
//    QString style_source = settings.value(QLatin1String("userStyleSheet")).toString();
//    settings.endGroup();
//    QString style_source = globalparameters.entrance()->style_source();

    setStyleSheet(custom_hidabletabwidget_style);	// _style

    connect(_hide_action, &QAction::toggled, this, &HidableTabWidget::onHideAction);
    connect(this, &HidableTabWidget::tabBarClicked, this, &HidableTabWidget::onTabBarClicked);


    QFont font;
    font.setFamily("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);

//        QTextEdit* editor = new QTextEdit();
//        editor->setFont(font);
    this->setFont(font);


//    _layout->widget()->setContentsMargins(0, 0, 0, 0);
//    _layout->widget()->setStyleSheet("border:0px;");



    _layout->setMargin(0);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
    setLayout(_layout);
}

HidableTabWidget::~HidableTabWidget(){
//    if(_record_screens.size() > 0){
    for(int i = 0; i < count(); i ++){	// for(auto i = _record_screens.begin(); i != _record_screens.end(); i ++){
	auto w = widget(i);
	if(w->objectName() == record_screen_multi_instance_name){
		//	    auto	rs		= dynamic_cast<rs_t *>(w);
	    auto browser_ = dynamic_cast<rs_t *>(w)->browser();
	    if(browser_){// if(*i){	// && *i != widget()=>for entrance
//		_record_screens.erase(i);
		browser_->deleteLater();// (*i)->deleteLater();	// delete *i;
		// *i = nullptr;
	    }
	}
    }
//    }
}

std::set<rs_t *> HidableTabWidget::record_screens() const {
//    int browser_size_ = 0;
//    for(int i = 0; i < count(); i ++)
//		if(widget(i)->objectName() == record_screen_multi_instance_name)browser_size_ ++;

//    return browser_size_;
    std::set<rs_t *> result;
    for(int i = 0; i < count(); i ++){		// for(auto i = _record_screens.begin(); i != _record_screens.end(); i ++){
	auto w = widget(i);
	if(w->objectName() == record_screen_multi_instance_name){
	    auto rs = dynamic_cast<rs_t *>(w);
//	    auto	browser_	= dynamic_cast<rs_t *>(w)->browser();
	    if(rs)if(result.find(rs) == result.end())result.insert(rs);	// if(*i){	// && *i != widget()=>for entrance
	}
    }
    return result;	// _record_screens;
}

void HidableTabWidget::onHideAction(bool checked){
    if(checked){
	if(this->tabPosition() == North || tabPosition() == South)	// , West, East
		this->setMaximumHeight(this->tabBar()->height());
	else this->setMaximumWidth(this->tabBar()->width());
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    }else{
	if(this->tabPosition() == North || tabPosition() == South){	// , West, East
	    this->setMaximumHeight(std::numeric_limits<int>::max());	// 100000// just a very big number
		//            setContentsMargins(0, 0, 0, 0);
	}else{
	    this->setMaximumWidth(std::numeric_limits<int>::max());	// 100000// just a very big number
		//            setContentsMargins(0, 0, 0, 0);
	}
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

void HidableTabWidget::onTabBarClicked(){_hide_action->setChecked(false);}

// HidableTabWidget *HidableTabWidget::delegate_tab(){
//    HidableTabWidget *r = nullptr;
//    if(_delegate_tab) r = _delegate_tab;
//    return r;
// }

bool HidableTabWidget::eventFilter(QObject *obj, QEvent *event){
    if(event->type() == QEvent::MouseButtonDblClick && static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton)emit tabBarDoubleClicked(indexOf(childAt(static_cast<QMouseEvent *>(event)->pos())));
    return QTabWidget::eventFilter(obj, event);
}

browser::WebView *HidableTabWidget::find(const std::function<bool (boost::intrusive_ptr<const ::Binder>)> &_equal) const {
	// clean();
    browser::WebView *v = nullptr;
//	// new_dockedwindow(record);
//    for(auto i : _record_screens){
//	if(i){
//	    v = i->browser()->tabWidget()->find(_equal);
//	    if(v != nullptr)break;
//	}
//    }
    for(int i = 0; i < count(); i ++){
	auto w = widget(i);
	if(w->objectName() == record_screen_multi_instance_name){
//	    auto	rs		= dynamic_cast<rs_t *>(w);
	    auto browser_ = dynamic_cast<rs_t *>(w)->browser();
	    if(browser_){
		v = browser_->tabWidget()->find(_equal);
		if(v != nullptr)break;
	    }
	}
    }
    boost::intrusive_ptr<const TreeItem> found_myself(nullptr);
    if(v){
	if(v->page()){
	    boost::intrusive_ptr<::Binder> binder = v->page()->binder();
	    if(binder){
		auto _this_item = v->page()->host();	// globalparameters.entrance()->find(_equal);
		if(_this_item){
		    if(binder->integrity_external(_this_item, v->page())){
//			assert(_this_item == v->page()->binder()->item());
//			if(_this_item && v->load_finished())																			// && (v->tabmanager()->currentWebView() == v)
			found_myself = _this_item;
		    }
		}
	    }
	}
    }
    if(! found_myself)v = nullptr;
    return v;
}

browser::Browser *HidableTabWidget::new_browser(){
//	// DockedWindow *browser =
//	return new Browser(_tree_screen
//			  , _find_screen
//			  , _editor_screen	// , _vtab_tree
//			  , _main_window
//			  , this
//			  , _style_source
//			  , _profile
//			  , Qt::MaximizeUsingFullscreenGeometryHint
//		   );	// , dock_widget

    auto rs = new rs_t(_tree_screen, _find_screen, _editor_screen, _entrance, _main_window, _style_source, _profile);

    setUpdatesEnabled(false);
    addTab(rs, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Browser"));	// QString("Browser ") + QString::number(vtab_record->count())
    bool found = false;
    for(int i = 0; i < count(); i ++){
	auto r = widget(i);
	if(r == rs)found = true;
    }
    assert(found);
////    bool found = false;
////    for(auto i = _record_screens.begin(); i != _record_screens.end(); i ++){
////	if(*i == rs){
////	    found = true;
////	    break;
////	}
////    }
////    if(! found) _record_screens.insert(rs);
//    _record_screens.insert(rs);
    setUpdatesEnabled(true);
    rs->adjustSize();


    return rs->browser();

	// return find(url);   // std::make_pair(browser, find(url).second);     // BrowserView::QDockWidget::BrowserWindow*
}

// not sure to succeeded
browser::Browser *HidableTabWidget::activated_browser(){
// clean();

// std::pair<Browser *, WebView *> dp = std::make_pair(nullptr, nullptr);
    browser::Browser *_browser = nullptr;
// if(_mainWindows.isEmpty()) {
// dp = new_dockedwindow(
// QUrl(DockedWindow::_defaulthome)
// );
// } else { //
//    if(count() > 0)
    for(int i = 0; i < count(); i ++){		// for(auto i : _record_screens){
	auto w = widget(i);
	if(w->objectName() == record_screen_multi_instance_name){
//	    auto	rs		= dynamic_cast<rs_t *>(w);
	    auto browser_ = dynamic_cast<rs_t *>(w)->browser();
	    if(browser_){
		if(browser_->isVisible() || browser_->isActiveWindow()){
		    _browser = browser_;		// .data();

		    break;
		}
	    }
	}
    }
    if(! _browser){
	// _browser = _browsers[0];
	// }
	// } else {
	_browser = new_browser();
	// assert(_browser);
	// return _browser;
	// dp.second = dp.first->tabWidget()->currentWebView();
    }
// assert(dp.first);
// assert(dp.second);
    assert(_browser);

    return _browser;	// qobject_cast<DockedWindow *>(widget()); //
// _mainWindows[0];
}

