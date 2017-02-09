

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif




#include "hidable_tabwidget.h"
#include <QTabBar>
#include <QToolButton>
#include <QLayout>
#include <QMainWindow>
#include "libraries/global_parameters.h"
#include "views/tree/tree_view.h"
#include "views/browser/entrance.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_index.hxx"
#include "views/main_window/main_window.h"
#include "libraries/qtm/editing_window.h"


extern AppConfig	appconfig;
extern gl_para		globalparameters;
extern const char	*record_screen_multi_instance_name;
const char			*custom_hidabletabwidget_style =
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


HidableTabWidget::HidableTabWidget(ts_t *tree_screen_
								  , FindScreen *find_screen_
								  , Editentry *_editentry
								  , browser::Entrance *entrance_
								  , QMainWindow *main_window_
								  , browser::Profile *profile_
								  , QString style_source_
								  , QSplitter *splitter
								  , std::shared_ptr<QSettings> splitter_config
								  , QString splitter_group_name
								  , QString splitter_sizelist_name
								  , QString collapsed_status_name, QWidget *parent)
	: QTabWidget(parent)
	  , _hide_action(new QAction(tr("▾"), this))
	  , _layout(new QVBoxLayout(this))	// (new QStackedLayout(this))
	  , _tree_screen(tree_screen_)
	  , _find_screen(find_screen_)
	  , _editentry(_editentry)
	  , _entrance(entrance_)
	  , _main_window(main_window_)
	  , _profile(profile_)
	  , _style_source(style_source_)
	  , _splitter(splitter)
	  , _splitter_config(splitter_config)
	  , _splitter_group_name(splitter_group_name)
	  , _splitter_sizelist_name(splitter_sizelist_name)
	  , _collapsed_status_name(collapsed_status_name){
//    , _delegate_tab(_delegate_tab)
	_hide_action->setCheckable(true);
	_hide_action->setToolTip("Hide Panels");
	QToolButton *hide_button = new QToolButton();
	hide_button->setDefaultAction(_hide_action);
	hide_button->setAutoRaise(true);
	this->setCornerWidget(hide_button);



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
			QWidget *w = nullptr;
			w = widget(index);
			if(w){
				if(w->objectName() == record_screen_multi_instance_name){
//		auto rs = dynamic_cast<rs_t *>(w);
					auto _browser = dynamic_cast<rs_t *>(w)->browser();
					if(_browser){
//		    if(_record_screens.find(rs) != _record_screens.end())_record_screens.erase(rs);
						if(_browser->tabmanager()->count() > 0){
							auto v = _browser->currentTab();
							if(v){
								auto p = v->page();
								if(p){
									auto it = p->host();
									if(_tree_screen->view()->current_item()->id() != it->id()) _tree_screen->view()->select_as_current(TreeIndex::create_treeindex_from_item([&] {return _tree_screen->view()->source_model();}, it));
								}
							}
						}
					}
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


	_layout->addWidget(this);
	setLayout(_layout);

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

	connect(_hide_action, &QAction::toggled, this, &HidableTabWidget::collapse_when_true);


	connect(this, &HidableTabWidget::tabBarClicked, this, &HidableTabWidget::inner_and_spiltter_on_click);
	connect(_splitter, &QSplitter::splitterMoved, this, &HidableTabWidget::inner_rebuild_on_splitter_moved);
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
			if(rs) if(result.find(rs) == result.end()) result.insert(rs);// if(*i){	// && *i != widget()=>for entrance
		}
	}
	return result;	// _record_screens;
}



// void HidableTabWidget::onTabBarClicked(int index){
//    if(currentIndex() == index){
//	if(this->tabBar()->geometry().width() >= this->geometry().width()) onHideAction(false);	// emit _hide_action->toggle(false);
//	else onHideAction(true);// emit _hide_action->toggle(true);
////    _hide_action->setChecked(false);
//    }else if(this->tabBar()->geometry().width() >= this->geometry().width()) onHideAction(false); // emit _hide_action->toggle(false);
// }

// HidableTabWidget *HidableTabWidget::delegate_tab(){
//    HidableTabWidget *r = nullptr;
//    if(_delegate_tab) r = _delegate_tab;
//    return r;
// }

bool HidableTabWidget::eventFilter(QObject *obj, QEvent *event){
	if(event->type() == QEvent::MouseButtonDblClick && static_cast<QMouseEvent *>(event)->button() == Qt::LeftButton) emit tabBarDoubleClicked(indexOf(childAt(static_cast<QMouseEvent *>(event)->pos())));
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
				if(v != nullptr) break;
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
	if(! found_myself) v = nullptr;
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

	auto rs = new rs_t(_tree_screen, _find_screen, _editentry, _entrance, this, _style_source, _profile);

	setUpdatesEnabled(false);
	addTab(rs, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Browser"));	// QString("Browser ") + QString::number(vtab_record->count())
	bool found = false;
	for(int i = 0; i < count(); i ++){
		auto r = widget(i);
		if(r == rs){
			rs->browser()->activateWindow();
			rs->browser()->setVisible(true);
			this->setCurrentWidget(r);
			found = true;
		}
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
	int count_browser = 0;
	for(int i = 0; i < count(); i ++){		// for(auto i : _record_screens){
		auto rs = widget(i);
		if(rs->objectName() == record_screen_multi_instance_name){
			count_browser ++;
//	    auto	rs		= dynamic_cast<rs_t *>(w);
			auto browser_ = dynamic_cast<rs_t *>(rs)->browser();
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


std::tuple<int, QList<int> > HidableTabWidget::inner_rebuild_on_splitter_moved(int record_pos, int index){
	(void) record_pos;
	(void) index;
//	QSplitter	*_splitter	= _splitter;
//	QMainWindow *_main_window	= _main_window;

	auto sizes = _splitter->sizes();

	auto bar_width = this->tabBar()->sizeHint().width();//
	if(0 == bar_width){
		bar_width = sizeHint().width();	// tabBar()->geometry().width();//
//		auto bar_width_ = minimumSizeHint().width();
	}
	assert(bar_width > 0);
	auto win_width = _main_window->width();
	if(0 == win_width) win_width = _main_window->geometry().width();
	assert(win_width > 0 && win_width >= bar_width);
	if(record_pos <= bar_width){
		this->setMinimumWidth(bar_width);
		this->setMaximumWidth(bar_width);
		sizes[1]	= win_width - bar_width;
		sizes[0]	= bar_width;
////		if((sizes[1] <= 0) || (sizes[1] > win_width - bar_width)) sizes[1] = win_width - bar_width;
//		emit this->_hide_action->setChecked(true);// redundant

		this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

		_splitter->setSizes(sizes);
	}else{	// if(record_pos > bar_width)
		this->setMaximumWidth(win_width);
		if(record_pos < 0) record_pos = bar_width;
		if(record_pos < win_width){
			sizes[1]	= win_width - record_pos;
			sizes[0]	= record_pos;
		}else{
			sizes[0]	= win_width * 20 / 100;
			sizes[1]	= win_width - sizes[0];
		}
		if((sizes[1] <= bar_width)) sizes[0] = record_pos;
		if(sizes[1] > (win_width - bar_width)) sizes[1] = win_width - bar_width;
		if(sizes[1] < 0) sizes[1] = std::abs(win_width - record_pos);
//		emit this->_hide_action->setChecked(false);	// redundant

		this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		_splitter->setSizes(sizes);
	}
	if(sizes[0] <= 0) sizes[0] = bar_width;																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																// tabBar()->sizeHint().width();
	if((sizes[1] > win_width - sizes[0]) || (sizes[1] <= 0)) sizes[1] = std::abs(win_width - sizes[0]);
	assert(sizes[1] >= 0);
	// If you build main stack and edtor too early before the container has been built, you will need following jobs.
	auto	_main_stack = _splitter->widget(1);
	auto	msw			= _main_stack->width();	// 50
	if(msw < sizes[1]) _main_stack->resize(sizes[1], height());
////			assert(msw == sizes[1]);
//	auto ew = _editor->width();	// 0
////			assert(ew == sizes[1]);
//	if(ew < sizes[1]) _editor->resize(sizes[1], height());
	//
//	QSettings _splitter_config(_config_file_name, QSettings::IniFormat);

	//
	_splitter_config->beginGroup(_splitter_group_name);
	//
	auto modify_sizes_when_expanded = [&] {
		QStringList line_list_;
		QList<int>	list_;
		line_list_ = (_splitter_config->value(_splitter_sizelist_name, "100,100")).toString().split(",");
		for(int i = 0; i < line_list_.size(); ++ i) list_.append(line_list_.at(i).toInt());
		if(sizes != list_){
			QStringList line_list;
			for(int i = 0; i < sizes.size(); ++ i) line_list.append(QString::number(sizes.at(i)));
			_splitter_config->setValue(_splitter_sizelist_name, line_list.join(","));
		}
	};
	//
	auto cllapsed_original = _splitter_config->value(_collapsed_status_name, 0).toBool();
	if(! cllapsed_original){
		if(sizes[0] <= bar_width) _splitter_config->setValue(_collapsed_status_name, true);
		else modify_sizes_when_expanded();
	}else{
		if(sizes[0] > bar_width){
			_splitter_config->setValue(_collapsed_status_name, false);
			modify_sizes_when_expanded();
		}
	}
	_splitter_config->endGroup();
	//
	_splitter_config->sync();
	return	// std::make_tuple(bar_width, h_record_sizes);	//
		   std::make_tuple<int, QList<int> > (std::move(bar_width), std::move(sizes));
}

void HidableTabWidget::collapse_when_true(bool checked){
//	auto	bar_width_	= tabBar()->width();//100
	auto bar_width = tabBar()->sizeHint().width();
	if(0 == bar_width) bar_width = sizeHint().width();
//	assert(bar_width_ == bar_width);
//	auto	bar_height_	= tabBar()->height();
	auto bar_height	= tabBar()->sizeHint().height();
	if(0 == bar_height) bar_height = sizeHint().height();
//	assert(bar_height_ == bar_height);

	auto	sizes		= _splitter->sizes();
	auto	win_width	= _main_window->width();
	auto	win_height	= _main_window->height();
//	auto	bar_width	= tabBar()->sizeHint().width();	// geometry().width();
	//
	auto open_tab_widgets_west_east = [&] {
//		QSettings	_splitter_config(_config_file_name, QSettings::IniFormat);
		QStringList line_list_;
		QList<int>	list_;
		_splitter_config->beginGroup(_splitter_group_name);	// "geometry"
		line_list_ = (_splitter_config->value(_splitter_sizelist_name, "100,100")).toString().split(",");
		for(int i = 0; i < line_list_.size(); ++ i) list_.append(line_list_.at(i).toInt());
		auto win_width = _main_window->width();	// sizes[0] + sizes[1];
		if(list_[0] <= bar_width){
			sizes[0]	= win_width * 20 / 100;
			sizes[1]	= win_width - sizes[0];
//			QStringList line_list;
//			for(int i = 0; i < sizes.size(); ++ i) line_list.append(QString::number(sizes.at(i)));
//			_splitter_config->setValue(_splitter_sizelist_name, line_list.join(","));
		}else{
			sizes[0]	= list_[0];
			sizes[1]	= win_width - sizes[0];
		}
		_splitter_config->endGroup();
		_splitter_config->sync();
		_splitter->setSizes(sizes);
		emit _splitter->splitterMoved(sizes[0], 0);
	};
	//
	if(checked){// close, collapse
		if(this->tabPosition() == North || tabPosition() == South){	// , West, East
			this->setMaximumHeight(bar_height);
			for(int i = 0; i < this->count(); i ++){
				auto w = this->widget(0);
				w->setMaximumSize(QSize(widget(0)->geometry().width(), 0));
				w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
			}
			sizes[1]	= win_height - bar_height;
			sizes[0]	= bar_height;
			_splitter->setSizes(sizes);
//	    this->setMaximumWidth([&] {auto p = this->parentWidget();return p ? p->width() : this->geometry().width();} ());
			this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
		}else{
			this->setMaximumWidth(bar_width);
			for(int i = 0; i < this->count(); i ++){
				auto w = this->widget(0);
				w->setMaximumSize(QSize(0, widget(0)->geometry().height()));
				w->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
			}
			sizes[1]	= win_width - bar_width;
			sizes[0]	= bar_width;
			_splitter->setSizes(sizes);
			emit _splitter->splitterMoved(sizes[0], 0);
//	    this->setMaximumHeight([&] {auto p = this->parentWidget();return p ? p->height() : this->geometry().height();} ());
			this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
		}
	}else{	// open, expand
		if(this->tabPosition() == North || tabPosition() == South){	// , West, East
			this->setMaximumHeight(std::numeric_limits<int>::max());	// 100000// just a very big number
			for(int i = 0; i < this->count(); i ++){
				auto w = this->widget(0);
				w->setMaximumSize(QSize(widget(0)->geometry().width(), std::numeric_limits<int>::max()));
				w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			}
			//            setContentsMargins(0, 0, 0, 0);
//	    this->setMaximumWidth([&] {auto p = this->parentWidget();return p ? p->width() : this->geometry().width();} ());
		}else{
			this->setMaximumWidth(std::numeric_limits<int>::max());	// 100000// just a very big number
			for(int i = 0; i < this->count(); i ++){
				auto w = this->widget(0);
				w->setMaximumSize(QSize(std::numeric_limits<int>::max(), widget(0)->geometry().height()));
				w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			}
			open_tab_widgets_west_east();
			//            setContentsMargins(0, 0, 0, 0);
//	    this->setMaximumHeight([&] {auto p = this->parentWidget();return p ? p->height() : this->geometry().height();} ());
		}
		this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	}
}

void HidableTabWidget::inner_and_spiltter_on_click(int index){
//    [&](int index){	// deal with spiltter
	(void) index;
	auto	sizes		= _splitter->sizes();
	auto	bar_width	= tabBar()->sizeHint().width();	// geometry().width();
	//
	//
	//
	if(index == currentIndex()){
		if(sizes[0] <= bar_width){
			// expand
			collapse_when_true(false);
		}else{
			// collapse
			collapse_when_true(true);
		}
	}else if(sizes[0] <= bar_width)	collapse_when_true(false);
//    });
}
