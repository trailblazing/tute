#include "main.h"

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QWebEngineView>   // #include <QWebView> //#include <QtWebKitWidgets/QWebView>

// #include <QWebElement>
#include <QWebEnginePage>   // #include <QWebPage>
#include <QtWebEngineWidgets>

// #include <QWebFrame>
#include <QAction>
#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QDockWidget>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QMessageBox>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QSettings>
#include <QSslCertificate>
#include <QSslConfiguration>
#include <QSslError>
#include <QUrl>
#include <QVBoxLayout>
#include <cassert>

#ifdef Q_WS_QWS
#include <QWSServer>
#endif

#include "browser.h"
#include "controllers/record_table/record_controller.h"
#include "editentry.h"
#include "entranceinfo.h"
#include "libraries/global_parameters.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "libraries/qtm/editing_window.h"
#include "libraries/qtm/super_menu.h"
#include "libraries/window_switcher.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/record.h"
#include "models/record_table/record_index.hxx"
#include "models/record_table/record_model.h"
#include "models/tree/binder.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "tabwidget.h"
#include "toolbarsearch.h"
#include "views/browser/webview.h"
#include "views/find_in_base_screen/find_screen.h"
#include "views/main_window/main_window.h"
#include "views/record/meta_editor.h"
#include "views/record_table/record_screen.h"
#include "views/record_table/record_view.h"
#include "views/tree/tree_screen.h"
#include "webview.h"
#include <utility>


extern AppConfig appconfig;
// namespace browser {
// struct BrowserViewPrivate {
////Q_OBJECT
// public:
// QWebEngineView *view;
// QNetworkAccessManager *nam;
// QMap<QPair<QString, int>, QByteArray> exceptions;
// bool msgShown;
////~BrowserViewPrivate() = default;
// };

////deprecated
// void BrowserManager::loadUrl(const int pos)
// {
// RecordTableController *recordTableController =
// globalParameters.getRecordTableScreen()->getRecordTableController();
////int pos = recordTableController->getFirstSelectionPos();
// RecordTableModel *recordTableModel =
// recordTableController->getRecordTableModel();
// RecordTableData *recordTableData = recordTableModel->getRecordTableData();
// Record *record = recordTableData->getRecord(pos);

////    QUrl _url;
// if(record) {
////this->current_record = record;
////_url = record->getField("url");
// main_window()->currentTab()->record(record);
////mainWindow()->currentTab()->bind_recordtabcontroller(recordTableController);
// loadUrl(record);
// }
// }

// Record *BrowserView::findRecord()
// {
// Record *_record = nullptr;
// RecordTableController *recordTableController =
// globalParameters.getRecordTableScreen()->getRecordTableController();
// int pos = recordTableController->getFirstSelectionPos();

// if(pos != -1) {
////RecordTableView *view = recordTableController->getView();
////view->loadUrl();

// RecordTableModel *recordTableModel =
// recordTableController->getRecordTableModel();
// RecordTableData *recordTableData = recordTableModel->getRecordTableData();

// Record *record = recordTableData->getRecord(pos);

////if(record->isLite())record->switchToFat();  // no such a value

////QVariant qVariant = recordTableModel->data(pos, Qt::DisplayRole);

////    if(current_record.isLite())current_record.switchToFat();

////    QString dir = globalParameters.getWorkDirectory();
////    QString name = this->d->view->title();

////    if(0 == name.size())name = "default";

////    this->current_record.setNaturalFieldSource("id", url.toString());

////    //this->current_record.setField("name", name);  //
////    this->current_record.setNaturalFieldSource("name", name);
////    //this->current_record.setField(boost::mpl::c_str < crypt_type >
/// ::value, "0");

////    QString author = "default";
////    QString tags = "default";

////    QString text = this->d->view->page()->mainFrame()->toHtml();

////    if(0 == text.size())text = "<html>no content</html>";

// if(record) {
////this->current_record
// _record = record;
// }

////    this->current_record.setNaturalFieldSource("author", author);
////    this->current_record.setNaturalFieldSource("tags", tags);
////    this->current_record.setText(text);
////    this->current_record.setNaturalFieldSource("url", url.toString());
////    this->current_record.setNaturalFieldSource("dir", dir);

// }

// return _record;
// }

void Editentry::init_setting(void){
	// QUrl _url;

	// if(record)
	// _url = record->getField("url");

	QUrl url;
	// initialize

	QFileInfo settingsFile;

	// QString sb(QApplication::applicationDirPath());
	// sb.append(QDir::separator());
	// sb.append("browserview.ini");

	QString configFileName = globalparameters.root_path() + "/" +
	    globalparameters.target_os() + "/browserview.ini";
	// check to see if we have a settings file where we started from
	// if not fall back to system hard coded path
	QFileInfo file(configFileName.toLatin1());  // file(sb.toLatin1());
	if(file.exists()){
		qDebug() << "using local settings file";
		settingsFile.setFile(file.filePath());
	}else{
		qDebug() << "using system defined settings file";
		settingsFile.setFile(SETTINGS_FILE);
	}
	QSettings settings(settingsFile.filePath(), QSettings::NativeFormat);

	settings.beginGroup(SETTINGS_SECTION);

	// if(_url.isEmpty()) {
	url =
	    (settings.value("browser_view")
	    .toString());   // QUrl url(settings.value("browser_view").toString());

	// } else {
	// url = _url;
	// }

	// if(browser->currentTab()) {
	// RecordTableController *recordtablecontroller =
	// globalParameters.getRecordTableScreen()->getRecordTableController();
	////int pos = recordTableController->getFirstSelectionPos();
	// Record *record = nullptr;

	////if(pos != -1) {
	// RecordTableModel *recordtablemodel =
	// recordtablecontroller->getRecordTableModel();
	////RecordTableView *recordtableview = recordtablecontroller->getView();
	// RecordTableData *recordtabledata = recordtablemodel->getRecordTableData();
	// record = recordtabledata->getRecordByUrl(url);
	////}

	// if(record)browser->currentTab()->bind_recordtabcontroller(recordtablecontroller);

	// if((browser->currentTab()->url() != url)) {
	// this->setUrl(url);
	// }
	// }

	qDebug() << "loading url: " << settings.value("main_view").toString();
	if(settings.value("full_screen", false).toBool()) this->showFullScreen();
	if(settings.value("hide_cursor", false).toBool()){
		this->setCursor(QCursor(Qt::BlankCursor));
#ifdef Q_WS_QWS
		QWSServer::setCursorVisible(false);
#endif
	}
	this->set_scrollbars(settings.value("hide_scrollbars", false).toBool());
	this->set_cache(settings.value("enable_cache", false).toBool()
		       , settings.value("cache_size_megabytes", 5).toInt());

	settings.endGroup();

	// this->setMinimumSize(QSize(0, 0));
	// setMinimumHeight(0);

	this->show();

	// RecordTableController *recordTableController =
	// globalParameters.getRecordTableScreen()->getRecordTableController();
	// int pos = recordTableController->getFirstSelectionPos();

	// if(pos != -1) {
	////RecordTableView *view = recordTableController->getView();
	////view->loadUrl();

	// RecordTableModel *recordTableModel =
	// recordTableController->getRecordTableModel();
	// RecordTableData *recordTableData = recordTableModel->getRecordTableData();

	// Record *record = recordTableData->getRecord(pos);

	////if(record->isLite())record->switchToFat();  // no such a value

	////QVariant qVariant = recordTableModel->data(pos, Qt::DisplayRole);

	////    if(current_record.isLite())current_record.switchToFat();

	////    QString dir = globalParameters.getWorkDirectory();
	////    QString name = this->d->view->title();

	////    if(0 == name.size())name = "default";

	////    this->current_record.setNaturalFieldSource("id", url.toString());

	////    //this->current_record.setField("name", name);  //
	////    this->current_record.setNaturalFieldSource("name", name);
	////    //this->current_record.setField(boost::mpl::c_str < crypt_type >
	/// ::value, "0");

	////    QString author = "default";
	////    QString tags = "default";

	////    QString text = this->d->view->page()->mainFrame()->toHtml();

	////    if(0 == text.size())text = "<html>no content</html>";

	// if(record) this->current_record = record;

	////    this->current_record.setNaturalFieldSource("author", author);
	////    this->current_record.setNaturalFieldSource("tags", tags);
	////    this->current_record.setText(text);
	////    this->current_record.setNaturalFieldSource("url", url.toString());
	////    this->current_record.setNaturalFieldSource("dir", dir);

	// }

	// this->current_record = findRecord();
}

// void BrowserManager::loadUrl(Record *record)
// {
// QUrl _url = record->getField("url");

// if(!_url.isEmpty() && _url.isValid()) {

// if(main_window()->currentTab()->url() != _url) {

////if(mainWindow())
////mainWindow()->currentTab()->bind_recordtabcontroller(globalParameters.getRecordTableScreen()->getRecordTableController());
// main_window()->currentTab()->record(record);
// this->setUrl(_url);
// }

// this->show();

// main_window()->show();
// main_window()->currentTab()->show();
// }
// }

Editentry *Editentry::prepend(browser::Browser *browser){
	browser->setParent(this);
	setWidget(browser);
	browser->show();

	//    // adjustSize();
	//    // setAutoFillBackground(true);
	//    // setFeatures(QDockWidget::NoDockWidgetFeatures);
	//    // _browser = browser;
	//    bool found = false;
	//    for(auto i = _browsers.begin(); i != _browsers.end(); i ++){
	//        if(*i == browser){
	//        found = true;
	//        break;
	//        }
	//    }

	//    if(! found)_browsers.insert(browser);
	return this;
}

// void Editentry::on_splitter_moved(int pos, int index)
// {
// Q_UNUSED(index)
////        resize(size());
// QRect rect = geometry();
// int new_width = rect.width() + (pos - rect.left());
////move(pos,rect.top());
// resize(new_width, rect.height());
////setGeometry(pos, rect.top(), new_width, rect.height());
////        setMaximumWidth(16777215);
// adjustSize();
// repaint();
// }

void Editentry::on_activate_window(){
	FindScreen *findscreen = globalparameters.find_screen();

	assert(findscreen);
	assert(findscreen->historyhome());

	QObject::disconnect(_home_connection);

	_home_connection = QObject::connect(
		findscreen->historyhome(), &QAction::triggered, this
					       , [this](bool checked = true) -> void {
		Q_UNUSED(checked)
		assert(_main_window->vtab_record()->activated_browser());
		auto view = _main_window->vtab_record()
		->activated_browser()
		->tabmanager()
		->currentWebView();
		assert(view);
		if(view){
		    browser::WebPage *page = view->page();
		    if(page){
			boost::intrusive_ptr<TreeItem> _item = page->host();
			assert(_item);
			QString home = _item->field<home_type>();
			QUrl homeurl = QUrl(home);
			if(homeurl.isValid() && homeurl != page->url()){
			    _item->field<url_type>(home);       // "url",
			    // boost::intrusive_ptr<RecordModel::ModelIndex> record_index;

			    // try {
			    // record_index = new RecordModel::ModelIndex([&] {return
			    // page->record_controller()->source_model();},
			    // page->record_controller()->source_model()->sibling(_item),
			    // _item);
			    // } catch(std::exception &e) {throw e;}
			    page->bind(_item)->activate(std::bind(
						&HidableTabWidget::find
										 , globalparameters.main_window()->vtab_record()
										 , std::placeholders::_1));     // page->load(record, true);
				}
			}
		}
	});
}

void Editentry::resizeEvent(QResizeEvent *e){
	//    for(auto i : _editing_list)
	//            if(i) i->resizeEvent(e);
	if(this->widget()) static_cast<EditingWindow *>(this->widget())->resizeEvent(e);
	QDockWidget::resizeEvent(e);
}

// Browser *Editentry::new_browser(const QByteArray &state)
// {

// Browser *browser = new Browser(_tree_screen
// , _find_screen
// , _editor_screen
// , _vtabwidget
// , _main_window
// , this
// , _style_source
// , Qt::MaximizeUsingFullscreenGeometryHint
// ); //, dock_widget

// browser->restore_state(state);

////        _dockwidget->setWidget(browser);
////        browser->setParent(_dockwidget);

////        //        _dockwidget->adjustSize();
////        //        _dockwidget->setAutoFillBackground(true);
////        _dockwidget->setFeatures(QDockWidget::NoDockWidgetFeatures);

////        browser->restore_state(state);
////        _mainWindows.prepend(browser);

// return browser;     // BrowserView::QDockWidget::BrowserWindow*
// }

//    Browser *Editentry::new_browser(){
////    // DockedWindow *browser =
////    return new Browser(_tree_screen
////              , _find_screen
////              , _editor_screen  // , _vtab_tree
////              , _main_window
////              , this
////              , _style_source
////              , _profile
////              , Qt::MaximizeUsingFullscreenGeometryHint
////           );   // , dock_widget

//  auto rs = new rs_t(_tree_screen, _find_screen, _editor_screen, this,
// _main_window, _style_source, _profile);

//  return rs->browser();

//  // return find(url);   // std::make_pair(browser, find(url).second);
//// BrowserView::QDockWidget::BrowserWindow*
//    }

// Browser *Editentry::new_browser(QUrl const &url)
// {

////        DockedWindow *browser =
// return new Browser(url
// , _tree_screen
// , _find_screen
// , _editor_screen
// , _vtabwidget
// , _main_window
// , this
// , _style_source
// , Qt::MaximizeUsingFullscreenGeometryHint
// ); //, dock_widget

////        return find(url);   // std::make_pair(browser, find(url).second);
///// BrowserView::QDockWidget::BrowserWindow*
// }

// Browser *Editentry::new_browser(boost::intrusive_ptr<TreeItem> item)
// {

////        DockedWindow *browser =
// return new Browser(item
// , _tree_screen
// , _find_screen
// , _editor_screen
// , _vtabwidget
// , _main_window
// , this
// , _style_source
// , Qt::MaximizeUsingFullscreenGeometryHint
// ); //, dock_widget

////        return find(item);   // std::make_pair(browser, find(url).second);
///// BrowserView::QDockWidget::BrowserWindow*
// }

// WebView *Editentry::new_view(QUrl const &_url)
// {
////BrowserView *browser_view = globalParameters.getBrowserView();
////BrowserWindow *browser = nullptr;

////if(browser_view) {
////        browser = browser_view->getBrowserWindow();     //
////auto dock_widget = new QDockWidget(this,
/// Qt::MaximizeUsingFullscreenGeometryHint);
////dock_widget->setParent(this);

////        DockedWindow *browser =
// new Browser(_url
// , _tree_screen
// , _find_screen
// , _editor_screen
// , _vtabwidget
// , _main_window
// , this
// , _style_source
// , Qt::MaximizeUsingFullscreenGeometryHint
// ); //, dock_widget

////        _dockwidget->setWidget(browser);
////        browser->setParent(_dockwidget);

////        //    QVBoxLayout *layout = new QVBoxLayout(this);
////        //    //QDockWidget *dock_widget = new QDockWidget(this,
/// Qt::MaximizeUsingFullscreenGeometryHint);
////        //    //when change dock_widget to member variable, you need rebuild
/// the project! else the behavior will be weird.
////        //    layout->setMargin(0);

////        //        _dockwidget->adjustSize();
////        //        _dockwidget->setAutoFillBackground(true);
////        _dockwidget->setFeatures(QDockWidget::NoDockWidgetFeatures);   //
/// AllDockWidgetFeatures

////        //Qt::MaximizeUsingFullscreenGeometryHint

////        //        if(!browser) {
////        //            browser = new BrowserWindow(browser_view);
/////incomplete
////        //        }
////        _mainWindows.prepend(browser);
////        //browser->show();
////        //}

////        //this->exec();
////        assert(record->binded_only_page());
////        assert(record->binded_only_page()->view());

// return find([&](boost::intrusive_ptr<const TreeItem> it) {return
// it->field("url") == _url.toString();});   // std::make_pair(browser,
// find(url).second);     // BrowserView::QDockWidget::BrowserWindow*
// }

// WebView *Editentry::new_dockedwindow(Record *const record)
// {
////BrowserView *browser_view = globalParameters.getBrowserView();
////BrowserWindow *browser = nullptr;

////if(browser_view) {
////        browser = browser_view->getBrowserWindow();     //
////auto dock_widget = new QDockWidget(this,
/// Qt::MaximizeUsingFullscreenGeometryHint);
////dock_widget->setParent(this);
// DockedWindow *browser = new DockedWindow(record->getNaturalFieldSource("url")
// , _record_ontroller
// , this
// , _style_source
// , Qt::MaximizeUsingFullscreenGeometryHint
// ); //, dock_widget

////        _dockwidget->setWidget(browser);
////        browser->setParent(_dockwidget);

////        //    QVBoxLayout *layout = new QVBoxLayout(this);
////        //    //QDockWidget *dock_widget = new QDockWidget(this,
/// Qt::MaximizeUsingFullscreenGeometryHint);
////        //    //when change dock_widget to member variable, you need rebuild
/// the project! else the behavior will be weird.
////        //    layout->setMargin(0);

////        //        _dockwidget->adjustSize();
////        //        _dockwidget->setAutoFillBackground(true);
////        _dockwidget->setFeatures(QDockWidget::NoDockWidgetFeatures);   //
/// AllDockWidgetFeatures

////        //Qt::MaximizeUsingFullscreenGeometryHint

////        //        if(!browser) {
////        //            browser = new BrowserWindow(browser_view);
/////incomplete
////        //        }
////        _mainWindows.prepend(browser);
////        //browser->show();
////        //}

////        //this->exec();
// assert(record->binded_only_page());
// assert(record->binded_only_page()->view());
// return record->binded_only_page()->view();  // std::make_pair(browser,
// record->binded_only_page()->view());     //
// BrowserView::QDockWidget::BrowserWindow*
// }

// DockWidget::DockWidget(QWidget *parent, DockedWindow *browser,
// Qt::WindowFlags flags)
// : QDockWidget(parent, flags)
// {
// this->setWidget(browser);
// browser->setParent(this);

////    QVBoxLayout *layout = new QVBoxLayout(this);
////    //QDockWidget *dock_widget = new QDockWidget(this,
/// Qt::MaximizeUsingFullscreenGeometryHint);
////    //when change dock_widget to member variable, you need rebuild the
/// project! else the behavior will be weird.
////    layout->setMargin(0);

////        adjustSize();
////        setAutoFillBackground(true);
// setFeatures(QDockWidget::NoDockWidgetFeatures);   // AllDockWidgetFeatures

////Qt::MaximizeUsingFullscreenGeometryHint

// qobject_cast<Editentry *>(parent)->window_list().prepend(browser);
// }

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(Editentry)
#endif

Editentry::Editentry(QString object_name, ts_t *tree_screen, FindScreen *find_screen    // browser::ToolbarSearch *toolbarsearch
		    , browser::Entrance *entrance, wn_t *main_window, AppConfig &appconfig
		    , const QString &style_source
		    , browser::Profile *profile
		    , Qt::WindowFlags flags)
	: QDockWidget(main_window, flags)   // , _application(application)
	  //      , _browsers(std::set<Browser * >())   // ,
	  // _shadow_branch(_record_controller->source_model()->_browser_pages)
	  , _tree_screen(tree_screen), _find_screen(find_screen)
	  //      , _editor_screen(_editor_screen)      // ,
	  //      _record_controller(_record_controller)
	  , _entrance(entrance), _main_window(main_window), _appconfig(appconfig)
	  , _style_source(style_source), _profile(profile), _flags(flags)
	  , _hidetitlebar(new QWidget(this, Qt::FramelessWindowHint | Qt::CustomizeWindowHint))
	  , _super_menu(new SuperMenu(find_screen, this)){
//	  , _splitter(_splitter)
//	  , _splitter_config(_splitter_config)
//	  , _splitter_groupname(_splitter_groupname)
//	  , _splitter_sizelist(_splitter_sizelist)
	//      , _editor(_blog_editor->editor())
	// _find_screen -> for find_text
	// | Qt::SplashScreen
	// , _dockwidget(new DockWidget(
	// this
	// , _mainWindows[0].data()
	// ,  flags  //Qt::Widget   //Qt::WindowMaximizeButtonHint
	// //Qt::MaximizeUsingFullscreenGeometryHint
	// )
	// )
	// , _browser(new
	// DockedWindow(register_record(QUrl(DockedWindow::_defaulthome))
	// , recordtablecontroller
	// , this, style_source, flags    //Qt::Widget
	// //Qt::WindowMaximizeButtonHint  // Qt::MaximizeUsingFullscreenGeometryHint
	// ))

	setObjectName(object_name);
	// invoke_ptr = &Editentry::active_url;

	// _mainWindows.prepend(browser);

	// d->view = new QWebEngineView(this);
	// d->view->page()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	// d->view->page()->setScrollBarPolicy(Qt::Horizontal,
	// Qt::ScrollBarAlwaysOff);

	// browser->setWebAttribute(QWebEngineSettings::JavascriptEnabled, true);
	// browser->setWebAttribute(QWebEngineSettings::JavascriptCanOpenWindows,
	// true);

	// browser->msgShown = false;

	// QVBoxLayout *layout = new QVBoxLayout(this);
	// QDockWidget *dock_widget = new QDockWidget(this,
	// Qt::MaximizeUsingFullscreenGeometryHint);
	// when change dock_widget to member variable, you need rebuild the project!
	// else the behavior will be weird.
	// layout->setMargin(0);

	// _dockwidget->setWidget(_mainWindows[0].data());
	////    //    layout->addWidget(browser);
	////    //    browser->setLayout(layout);
	// _mainWindows[0]->setParent(_dockwidget);

	setWindowFlags(     // Qt::Window |
		Qt::FramelessWindowHint
		// |Qt::Popup
		| Qt::CustomizeWindowHint
		// | Qt::SplashScreen  //
		// http://www.qtforum.org/article/20174/how-to-create-borderless-windows-with-no-title-bar.html?s=86e2c5a6509f28a482adbb7d9f3654bb2058a301#post75829
		// | Qt::DockWidgetArea::NoDockWidgetArea
		| Qt::MaximizeUsingFullscreenGeometryHint);

	setAutoFillBackground(true);
	adjustSize();

	setFeatures(QDockWidget::NoDockWidgetFeatures |
	    QDockWidget::DockWidgetVerticalTitleBar
	    // | Qt::DockWidgetArea::NoDockWidgetArea
	    // | Qt::MaximizeUsingFullscreenGeometryHint
		   ); // AllDockWidgetFeatures

	// this->titleBarWidget()->hide();

	QWidget *titleBar = titleBarWidget();

	// QVBoxLayout *main = new QVBoxLayout;
	////main -> addLayout(0);
	////main -> addLayout(0);
	// main->setGeometry(QRect());
	// main->setSizeConstraint(QLayout::SetMinimumSize);
	////setWindowTitle(tr("Find"));
	////setLayout(main);

	// _hidetitlebar->setMinimumWidth(0);
	// _hidetitlebar->setMaximumWidth(0);
	// _hidetitlebar->setMinimumHeight(0);
	// _hidetitlebar->setMaximumHeight(
	// 0
	//// parent->height() // for QDockWidget::DockWidgetVerticalTitleBar
	// );
	// _hidetitlebar->setMinimumSize(QSize(0, 0));
	// _hidetitlebar->resize(QSize(0, 0));

	// _hidetitlebar->setLayout(main);

	setTitleBarWidget(_hidetitlebar);
	_hidetitlebar->setGeometry(0, 0, 0, 0);
	// _hidetitlebar->hide();
	_hidetitlebar->setVisible(false);

	// _hidetitlebar->setMaximumWidth(0);
	_hidetitlebar->close();
	// _hidetitlebar->setCollapsible(true);
	// _hidetitlebar->setMaximumHeight();
	// _hidetitlebar->setMaximumSize(0, 0);

	delete titleBar;

	//    _blog_editor = new EditingWindow();
	blog_editor()->setSTI(0);       // No STI
	blog_editor()->setWindowTitle(QObject::tr((program_title_string + " - new entry [*]").c_str()));

	blog_editor()->setParent(this);
	setWidget(blog_editor());
	if(blog_editor()->handleArguments()) blog_editor()->show();
	else blog_editor()->close();
	blog_editor()->raise();

	setup_actions();

	setup_ui();
	assembly();

	connect(this, &Editentry::editing_activated, blog_editor()->_super_menu
	       , &SuperMenu::editing_win);
	connect(this, &Editentry::editing_activated
	       , [&](EditingWindow *){blog_editor()->adjustSize();});

	init_setting();

	// setup_signals(_find_screen->toolbarsearch());

	// new_mainwindow(register_record(QUrl(DockedWindow::_defaulthome)));  //
	// main_window() will never fail

	// d->nam = d->view->page()->networkAccessManager();

	// connect(d->nam, SIGNAL(finished(QNetworkReply *)), this,
	// SLOT(finished(QNetworkReply *)));

	// connect(d->nam, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError>
	// &)), this, SLOT(sslErrors(QNetworkReply *, const QList<QSslError> &)));

	// browser->show();
}

Editentry::~Editentry(){
	//    if(_browsers.size() > 0){
	//        for(auto i = _browsers.begin(); i != _browsers.end(); i ++){
	//        if(*i && *i != widget()){
	//            // _browsers.erase(i);
	//            (*i)->deleteLater();// delete *i;
	//            // *i = nullptr;
	//        }
	//        }
	//    }
	//    // if(isselfcreated())delete current_record;   // no, do not apply
	// memory by this class for record, from the original source
	//    // if(_actionFreeze)delete _actionFreeze;
	//    // if(_dockwidget)delete _dockwidget;
	//    // if(browser)delete browser;  // I can't destroy?
	//    // delete _shadow_branch;
	if(_blog_editor) _blog_editor->deleteLater();
	if(_hidetitlebar){
		delete _hidetitlebar;
		_hidetitlebar = nullptr;
	}
}

void Editentry::setup_actions(){
	// _actionFreeze = new QAction(tr("Pin / freeze browser view"), this);
	// _actionFreeze->setStatusTip(tr("Pin / freeze browser view"));
	// _actionFreeze->setIcon(QIcon(":/resource/pic/pentalpha.svg"));
}

void Editentry::setup_ui(void){}

// void BrowserView::setupDynamicSignals(void)
// {

////    //Connect a signal to a pointer to qobject member function
////    template <typename Func1, typename Func2>
////    static inline QMetaObject::Connection connect(
////        const typename QtPrivate::FunctionPointer<Func1>::Object *sender,
/// Func1 signal,
////        const typename QtPrivate::FunctionPointer<Func2>::Object *receiver,
/// Func2 slot,
////        Qt::ConnectionType type = Qt::AutoConnection);

// static QMetaObject::Connection _connection_loadfinished;    // for disconnect
// static QMetaObject::Connection _connection_openlinkinnewtab;

// if(_connection_loadfinished.operator void *QMetaObject::Connection:: * () !=
// nullptr) {
// QObject::disconnect(_connection_loadfinished);
// }

// if(_connection_openlinkinnewtab.operator void *QMetaObject::Connection:: * ()
// != nullptr) {
// QObject::disconnect(_connection_openlinkinnewtab);
// }

////…
////    _connection = QObject::connect(…);
////…
////    QObject::disconnect(_connection);
// if(browser->currentTab() != nullptr) {
// _connection_loadfinished =  //QWebView::
// connect
////                   <
////                   void (QWebView::*)(bool)
////                   , void (RecordTableController::*)(void)
////                   >
// (
// browser->currentTab()
////, &BrowserView::urlChanged//, &QWebView::urlChanged  //
// , &WebView::loadFinished
// , this//globalParameters.getRecordTableScreen()->getRecordTableController()
////, &BrowserView::onUrlChanged//&RecordTableController::autoAddNewAfterContext
// , &BrowserView::onLoadFinished//
// );

////    _connection = connect(this
////                          , &BrowserView::loadChanged
////                          ,
/// globalParameters.getRecordTableScreen()->getRecordTableController()
////                          , &RecordTableController::autoAddNewAfterContext);

////void (BrowserView::*lU)(const QUrl &) = &BrowserView::loadUrl;
// _connection_openlinkinnewtab = connect
////                  <
////                  void (QWebEngineView::*)(const QUrl &)
////                  , void (BrowserView::*)(const QUrl &)
////                  >
// (browser->currentTab()
// , &WebView::openLinkInNewTab
// , this
////, &BrowserView::loadUrl //
// , &BrowserView::openLinkInNewTab
// );
// }
// }

// void Editentry::activate_search_fragment(const QUrl &url)
// {
// clean();

// WebView *v = nullptr;

// if(_browsers.size() > 0) {
// for(auto &browser : _browsers) {
// v = browser->tabmanager()->find<url_fragment>(url);

// if(v) {
// v->page()->activate();
// }
// }
// }

// if(v == nullptr) {

// Browser *browser = activated_browser();

// auto r = browser->tabmanager()->item_request_from_tree<url_fragment>(url);

// r->activate();

// }
// }

// void Editentry::activate(const QUrl &url)
// {
// clean();

// WebView *v = nullptr;

// if(_browsers.size() > 0) {
// for(auto &browser : _browsers) {
// v = browser->tabmanager()->find(url);

// if(v) {
// v->page()->activate();
// }
// }
// }

// if(v == nullptr) {
////        if(activiated_browser()) {
// Browser *browser = activated_browser();
////            auto ara =
/// boost::make_shared<TabWidget::ActiveRecordBinder>(browser->tabWidget()); //
/// boost::make_shared<Editentry::ActiveRecordBinder>(this);

// auto r = browser->tabmanager()->item_request_from_tree(url);
////            r->self_bind();
// r->activate();
////        } else {
////            new_browser(url);
////        }

////        r->active_immediately(true);
////        return active_record(r);
// }
// }

//  // not sure to succeeded
//    Browser *Editentry::activated_browser(){
//  // clean();

//  // std::pair<Browser *, WebView *> dp = std::make_pair(nullptr,
// nullptr);
//  Browser *_browser = nullptr;
//  // if(_mainWindows.isEmpty()) {
//  // dp = new_dockedwindow(
//  // QUrl(DockedWindow::_defaulthome)
//  // );
//  // } else { //
//  // if(_browsers.size() > 0) {
//  for(auto i : _main_window->vtab_record()->browsers()){
//      if(i->isVisible() || i->isActiveWindow()){
//      // assert(i);
//      // dp.first
//      _browser = i;   // .data();
//      // assert(_browser);
//      // dp.second = i->tabWidget()->currentWebView();
//      break;
//      }
//  }
//  if(! _browser){
//      // _browser = _browsers[0];
//      // }
//      // } else {
//      _browser = new_browser();
//      // assert(_browser);
//      // return _browser;
//      // dp.second = dp.first->tabWidget()->currentWebView();
//  }
//  // assert(dp.first);
//  // assert(dp.second);
//  assert(_browser);

//  return _browser;// qobject_cast<DockedWindow *>(widget()); //
//  // _mainWindows[0];
//    }

// void Editentry::activate(const QUrl &_find_url
// , const TreeScreen::paste_strategy &_view_paste_strategy
// , equal_url_t _equal
// )
// {
////        clean();
// Browser *_current_browser = nullptr;
// WebView *v = nullptr;

// if(_browsers.size() > 0) {
// for(auto browser : _browsers) {
// v = browser->tabmanager()->find([&](boost::intrusive_ptr<const TreeItem> it)
// { return _equal(it, _find_url);});

// if(v) {
// _current_browser = browser;
// v->page()->activate();
// }
// }
// }

// if(v == nullptr) {

// _current_browser = activated_browser();

// auto r =    // _current_browser->tabmanager()
// _tree_screen->item_bind(_find_url, _view_paste_strategy, _equal);

// r->activate();

// }

// auto vtab = globalparameters.vtab();

// if(vtab->currentWidget() != _current_browser->record_screen()) {
// vtab->setCurrentWidget(_current_browser->record_screen());
// }
// }

// void Editentry::activate(boost::intrusive_ptr<TreeItem> item
// , const TreeScreen::paste_strategy &_view_paste_strategy
// , equal_t _equal
// )
// {
////        clean();
// Browser *_current_browser = nullptr;

// if(item->page_valid() && _browsers.size() > 0) {
// for(auto browser : _browsers) {
// if(browser->tabmanager()->indexOf(item->page_link()->view()) != -1) {
// _current_browser = browser;
// item->activate();
// }
// }
// } else {
////        if(activiated_browser().first) {
// _current_browser = activated_browser();

// auto r =    // _current_browser->tabmanager()
// _tree_screen->item_bind(item, _view_paste_strategy, _equal);

// r->activate();
// }

// auto vtab = globalparameters.vtab();

// if(vtab->currentWidget() != _current_browser->record_screen()) {
// vtab->setCurrentWidget(_current_browser->record_screen());
// }
// }

// void Editentry::setup_signals(browser::ToolbarSearch *toolbarsearch)
// {
////        auto _toolbarsearch =
/// globalparameters.getFindScreen()->toolbarsearch();
// void(Editentry::*_activate)(const QUrl &, const TreeScreen::paste_strategy &,
// equal_url_t) = &Editentry::activate;  // <url_fragment>;
// connect(toolbarsearch, &ToolbarSearch::search, this, _activate);
////        connect(this->_actionFreeze, &QAction::triggered,
/// globalparameters.getWindowSwitcher(), &WindowSwitcher::findInBaseClick);

// }

void Editentry::assembly(void){}

// void BrowserManager::setUrl(const QUrl &_url)
// {
////if(browser)
// main_window()->loadPage(_url.toString());
// }

void Editentry::set_scrollbars(bool hide){
	if(! hide){
		// d->view->page()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAsNeeded);
		// d->view->page()->setScrollBarPolicy(Qt::Horizontal,
		// Qt::ScrollBarAsNeeded);
	}
}

void Editentry::set_cache(bool cache, int cache_size){
	if(cache){
		QNetworkDiskCache	*diskCache	= new QNetworkDiskCache(this);
		QString				location	=
		    QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
		diskCache->setCacheDirectory(location);
		diskCache->setMaximumCacheSize(cache_size * 1024 * 1024);   // in MB's
		// d->nam->setCache(diskCache);
		// browser->setCache(diskCache);
		qDebug() << QString("Cache location: %1").arg(location);
		qDebug() << QString("Cache maximum size: %1MB").arg(cache_size);
	}
}

////deprecated
// void BrowserManager::onLoadFinished(bool finished)
// {
// if(finished) {

////        //QWebFrame *frame = d->view->page()->mainFrame();
////        //QWebElement dom_title(frame->findFirstElement("title"));
////        QString title = mainWindow()->currentTab()->title();
/////dom_title.evaluateJavaScript("this.text").toString();

////        RecordTableController *recordTableController =
/// globalParameters.getRecordTableScreen()->getRecordTableController();

////        //RecordTableData *recordTableData =
/// recordTableController->getRecordTableModel()->getRecordTableData();

////        int pos = recordTableController->getFirstSelectionPos();
////        Record *_record =
/// recordTableController->getRecordTableModel()->getRecordTableData()->getRecord(pos);

////        if(_record)_record->setNaturalFieldSource("name", title);

// main_window()->currentTab()->synchronize_title_to_record();
// }
// }

//// deprecated, move to class WebView
// void BrowserManager::openLinkInNewTab()
// {
////QString url(QString(""));
// QUrl _url = main_window()->currentTab()->url();

// RecordTableController *recordTableController =
// globalParameters.getRecordTableScreen()->getRecordTableController();

// RecordTableData *recordTableData =
// recordTableController->getRecordTableModel()->getRecordTableData();

// if(!recordTableData->isRecordExists(_url)) {

// int pos = recordTableController->getFirstSelectionPos();
// Record *previous_record =
// recordTableController->getRecordTableModel()->getRecordTableData()->getRecord(pos);

// if(previous_record) {
// Record record;

// if(record.isLite())record.switchToFat();

////QString title = d->view->title(); // not ready yet
// record.setNaturalFieldSource("id",
// previous_record->getNaturalFieldSource("id"));
// record.setNaturalFieldSource("name",
// previous_record->getNaturalFieldSource("name"));
// record.setNaturalFieldSource("author",
// previous_record->getNaturalFieldSource("author"));
// record.setNaturalFieldSource("url",    _url.toString());    // only changed
// record.setNaturalFieldSource("tags",
// previous_record->getNaturalFieldSource("tags"));

// recordTableController->addNew(ADD_NEW_RECORD_AFTER, record);
// //recordTableController->autoAddNewAfterContext();
// }
// }

////    Record *previous_record =
/// recordTableController->getRecordTableModel()->getRecordTableData()->getRecord(pos);
////    //if(current_record)
////    url = previous_record->getNaturalFieldSource("url");

////    std::string url_compare_stored = _url.toString().toStdString();
////    std::string url_compare_get = url.toStdString();
////    std::string compare = getDifference(url_compare_stored,
/// url_compare_get);
////    //QUrl qurl(url);

////    //if(qurl != _url) {
////    if(compare.size() != 0 && compare != "/") { // really changed!

////        //if(this->current_record->getNaturalFieldSource("url") !=
/// _url.toString()) {

////        //this->current_record = new Record();    // should from framework

////        Record record;

////        if(record.isLite())record.switchToFat();

////        record.setNaturalFieldSource("id",
/// previous_record->getNaturalFieldSource("id"));
////        record.setNaturalFieldSource("name",
/// previous_record->getNaturalFieldSource("name"));
////        record.setNaturalFieldSource("author",
/// previous_record->getNaturalFieldSource("author"));
////        record.setNaturalFieldSource("url",    _url.toString());    // only
/// changed
////        record.setNaturalFieldSource("tags",
/// previous_record->getNaturalFieldSource("tags"));

////        //record.setText(browser_view->getText());
////        if(!recordTableData->isRecordExists(_url)) {
////            //this->current_record->setNaturalFieldSource("url",
/// _url.toString());
////            recordTableController->addNew(ADD_NEW_RECORD_AFTER, record);
/////recordTableController->autoAddNewAfterContext();

////            //loadUrl(_url);

////            //this->current_record = findRecord();
////            //assert(this->current_record->getNaturalFieldSource("url") ==
/// _url.toString());
////        }

////    }
// }

void Editentry::finished(QNetworkReply *reply){
	if(reply->error() != QNetworkReply::NoError) qDebug() << QString("Network Error: %1").arg(reply->errorString());
	if(reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool() ==
	    true){
		QVariant contentVar = reply->header(QNetworkRequest::ContentTypeHeader);
		qDebug() << QString("Cache Used: %1").arg(contentVar.toString());
	}
	// QUrl url = reply->url();
	// QString _url = current_record->getNaturalFieldSource("url");
	// std::string url_compare_stored = _url.toStdString();
	// std::string url_compare_get = url.toString().toStdString();
	// std::string compare = "";

	// if(url_compare_stored.size() >= url_compare_get.size()) {
	// url_compare_stored.erase(url_compare_get.begin(), url_compare_get.end());
	// compare = url_compare_stored;
	// } else {
	// url_compare_get.erase(url_compare_stored.begin(),
	// url_compare_stored.end());
	// compare = url_compare_get;
	// }

	// std::string::size_type pos;

	// while((pos = compare.find_first_of(" ")) != compare.npos)compare.erase(pos,
	// 1);

	// if(compare == "" || compare == "/") {
	////if(this->current_record->getNaturalFieldSource("url") != url.toString()) {
	// this->current_record->setNaturalFieldSource("url", url.toString());
	// this->loadChanged();
	// }
}

void Editentry::ssl_errors(QNetworkReply *reply, const QList<QSslError> &errors){
	foreach(const QSslError &error, errors){
		qDebug() << QString("SSL Error: %1").arg(error.errorString());
	}

	reply->ignoreSslErrors(errors);
}

// void Editentry::clean()
// {
////        if(_browsers.size() > 0) {
////            // cleanup any deleted main windows first
////            //            for(int i = _browsers.count() - 1; i >= 0; --i) {
////            //                if(!_browsers.at(i)) {
////            //                    _browsers.removeAt(i);
////            //                }
////            //            }
////            for(auto i = _browsers.begin(); i != _browsers.end(); i++) {
////                if(!*i)_browsers.erase(i);
////            }
////        }
// }

// WebView *Editentry::active_record_alternative(Record *const record) {return
// active_record(record).second;}

//// prepare active chain but not load them
// boost::intrusive_ptr<TreeItem> Editentry::item_bind(RecordModel::ModelIndex
// modelindex)   // boost::intrusive_ptr<TreeItem> tab_brother,
// boost::intrusive_ptr<TreeItem> _it
// {
// boost::intrusive_ptr<TreeItem> tab_brother = modelindex.sibling();
// boost::intrusive_ptr<TreeItem> _it = modelindex.target();
// assert(_it);
// assert(tab_brother != _it);

// if(_it->is_lite())_it->to_fat();

// boost::intrusive_ptr<TreeItem> result(nullptr);
////        assert(_it->is_registered_to_browser() || (_it->field("url") ==
/// browser::Browser::_defaulthome));
////        clean();

// Browser *_browser = nullptr;    // DockedWindow *w = nullptr;
// WebView *_view = nullptr;

////        auto tree_view = _tree_screen->tree_view();
////        TreeIndex modelindex([&] {return tree_view->source_model();},
/// tree_view->current_item()->parent(),
/// tree_view->current_item()->parent()->sibling_order([&](boost::intrusive_ptr<const
/// Linker> il) {return il == tree_view->current_item()->linker() && il->host()
/// == tree_view->current_item() && tree_view->current_item()->parent() ==
/// il->host_parent();}));

// if(_it) {   // && !_it->record_binder()

////            if(QUrl(_it->field("url")).isValid())_it->field("url",
/// Browser::_defaulthome);  // terrify!!! delete all URL
// if(_it->field("url") == "")_it->field("url", browser::Browser::_defaulthome);

// assert(QUrl(_it->field("url")).isValid());

// if(QUrl(_it->field("url")).isValid()) {

// if(_browsers.size() == 0) {

// _browser = new_browser();
// result = _browser->item_bind(modelindex);  //
// _tree_screen->item_register(_it, std::bind(&TreeScreen::view_paste_as_child,
// _tree_screen, std::placeholders::_1, std::placeholders::_2,
// std::placeholders::_3));
// _view = result->binder()->page()->view();

// } else {

// for(auto i : _browsers) {

// _view = i->tabWidget()->find([&](boost::intrusive_ptr<const TreeItem> it)
// {return it->field("url") == _it->field("url");});

// if(_view != nullptr) {
// _browser = i;
// break;
// } else if(i->isVisible() || i->isActiveWindow()) {
// assert(i);
// _browser = i;
// }
// }

// assert(_browser);

// if(_browser) {
// if(_view == nullptr) {
////                            // assert(_browser->tabmanager()->count() > 0);

////                            if(_browser->tabmanager()->count() > 0) {
////                                //
/// assert(_browser->tabmanager()->currentWebView()->page()->binder()->item() ==
/// tab_brother);
// result = _browser->item_bind(modelindex);    //
// _browser->tabmanager()->currentWebView()->page()->binder()->item()
////                            }
////                            else {
////                                result = _browser->item_bind(nullptr, _it);
////                            }

// _view = result->binder()->page()->view();
// } else {
// result = _view->page()->item_bind(_it);
// }
// }
// }

// assert(_browser);
// assert(_view);
// assert(result);
// setWidget(_browser);
// }
// }

// assert(_browser);
// assert(_view);
// assert(_view->page()->binder());
// assert(_it->binder());
// assert(_view->page()->binder() == _it->binder());

// auto vtab = globalparameters.vtab();

// if(vtab->currentWidget() != _browser->record_screen()) {
// vtab->setCurrentWidget(_browser->record_screen());
// }

// assert(result->binder()->integrity_external(result, _view->page()));
// return result; // _view;
// }

// std::pair<DockedWindow *, WebView *> Editentry::invoke_page(Record *const
// record)
// {
// clean();

// std::pair<DockedWindow *, WebView *> dp = std::make_pair(nullptr, nullptr);

// if(_mainWindows.isEmpty()) {
// dp = new_dockedwindow(record);
// } else {
// for(auto &i : _mainWindows) {
// dp.second = i->tabWidget()->find(record->getNaturalFieldSource("url"));

// if(dp.second != nullptr) {
// dp.first = i.data();

// if(!i->isVisible()) {
// i->raise();
// i->activateWindow();
// }

// break;
// } else if(i->isVisible()) {
// dp.first = i.data();
// }
// }

// if(dp.first == nullptr) {
// dp.first = _mainWindows[0].data();
// }

// assert(dp.first);
// }

// const DockedWindow *w = dp.first;
// TabWidget *const tab = w->tabWidget();

// if(dp.second == nullptr && w != nullptr) {

//// Record *blank_url =
/// check_register_record(QUrl(DockedWindow::_defaulthome));

////            if(blank.isLite())blank.switchToFat();
////            blank.setNaturalFieldSource("url", DockedWindow::_defaulthome);

// WebView *blankview = nullptr;
// blankview = tab->find(QUrl(DockedWindow::_defaulthome));
////PageView *no_pin = nullptr;
// dp.second = w->tabWidget()->find_nopin();
//// assert(dp.first);

// if(blankview != nullptr) {
// dp.second = blankview;
// blankview->page()->load(record);
// } else if(dp.second != nullptr) {   // no_pin
// if(dp.second->page()->url().toString() !=
// record->getNaturalFieldSource("url"))dp.second->page()->load(record);
// } else {
// dp.second = tab->newTab(record);  // , false
//// auto load
// }

////            tab->setCurrentWidget(dp.second);   //
/// tab->setCurrentIndex(tab->webViewIndex(dp.second));
////            dp.second->show();
// }

////        tab->setCurrentWidget(dp.second);
////        dp.second->show();
// assert(dp.first);
// assert(dp.second);
// return dp;  //_mainWindows[0];
// }

// QList<DockedWindow *> Editentry::window_raw_list()
// {
// clean();
// QList<DockedWindow *> list;

// for(int i = 0; i < _mainWindows.count(); ++i)
// list.append(_mainWindows.at(i));

// return list;
// }

//    std::set<Browser *> &Editentry::browsers(){
//  // clean();

//  // QList<DockedWindow *> list;

//  // for(int i = 0; i < _mainWindows.count(); ++i)
//  // list.append(_mainWindows.at(i));

//  return _browsers;   // list;
//    }

// void Editentry::open_url(const QUrl &url)
// {
// active_url(url); // active_record()->loadPage(url.toString());
// }

////    std::pair<Browser *, WebView *>
// WebView *Editentry::find(boost::intrusive_ptr<const TreeItem> item)const
// {
////        clean();
////        std::pair<Browser *, WebView *> dp{nullptr, nullptr};
// WebView *v = nullptr;
////        if(_mainWindows.isEmpty())dp.first = activebrowser();

////        if(!_mainWindows.isEmpty()) {
////new_dockedwindow(record);
// for(auto &i : _browsers) {
// if(!i.isNull()) {
// v = i->tabWidget()->find(item); // ->field("url")

// if(v != nullptr) {  // setWidget(i.data());
////                dp.first = i.data();
// break;
// }
// }

////            else if(i->isVisible() || i->isActiveWindow()) {
////                dp.first = i.data();
////            }
// }

////        }

////        if(dp.first == nullptr)dp.first = _mainWindows[0].data();

////        if(!dp.first->isActiveWindow()) {
////            dp.first->raise();
////            dp.first->activateWindow();
////        }
////        assert(dp.first);
// return v;
// }

// BrowserView *BrowserManager::create_view(Record *record, BrowserWindow
// *window)
// {
// BrowserView *bv = nullptr;
// bv = window->tabWidget()->newTabFull(record, _record_ontroller, true);
// return bv;
// }

//    bool Editentry::restore_state(const QByteArray &state){
//  return
// _main_window->vtab_record()->activated_browser()->restore_state(state);
//    }

// std::pair<DockedWindow *, WebView *> Editentry::active_record(Record *const
// record)
// {

// std::pair<DockedWindow *, WebView *> dp;
////        DockedWindow *w = nullptr;
////        PageView *bv = nullptr;
// dp = active_record(record);   // assert(record->page() == nullptr);  // maybe
// come from other tree branches

// assert(dp.first);
// assert(dp.second);

////        if(dp.second == nullptr && dp.first) {
////            //            dp = invoke_page(record);
/////->tabWidget()->find_view(record);    // create_view(record,
/// main_window(record));
////            dp.second = dp.first->invoke_page(record);
////        }

////        assert(dp.second);

// setWidget(dp.first);

////        if(!dp.first->isActiveWindow()) {
////            dp.first->raise();
////            dp.first->activateWindow();
////        }

////        // should this after loadfinished()? a global knoeledge based
/// strategy needed?
////        dp.first->tabWidget()->setCurrentWidget(dp.second);
////        dp.second->show();

////        bv->window()->raise();
////        dp.second->switch_show();

// return dp;
// }

// void BrowserManager::runScriptOnOpenViews(const QString &source)
// {
// for(auto &w : main_windows()) {
// w->runScriptOnOpenViews(source);

////            for(int i = 0; i < w->tabWidget()->count(); ++i) {
////                auto browserview = w->tabWidget()->webView(i);

////                if(browserview != nullptr)
////                    browserview->page()->runJavaScript(source);
////            }
// }
// }

//    WebView *Editentry::find(const std::function<bool
//    (boost::intrusive_ptr<const ::Binder>)> &_equal) const {
//  // clean();
//  WebView *v = nullptr;
//  // new_dockedwindow(record);
//  for(auto i : _main_window->vtab_record()->browsers()){
//      if(i){
//      v = i->tabWidget()->find(_equal);
//      if(v != nullptr)break;
//      }
//  }
//  boost::intrusive_ptr<const TreeItem> found_myself(nullptr);
//  if(v){
//      if(v->page()){
//      boost::intrusive_ptr<::Binder> binder = v->page()->binder();
//      if(binder){
//          auto _this_item = v->page()->host();//
// globalparameters.Editentry()->find(_equal);
//          if(_this_item){
//          if(binder->integrity_external(_this_item, v->page())){
//// assert(_this_item == v->page()->binder()->item());

//// if(_this_item && v->load_finished()) {
///// && (v->tabmanager()->currentWebView() == v)
//              found_myself = _this_item;
//// }
//          }
//          }
//      }
//      }
//  }
//// }
//  if(! found_myself)v = nullptr;
//  return v;
//    }

void Editentry::style_source(const QString &style_source){
	_style_source = style_source;
}

QString Editentry::style_source(){return _style_source;}

void Editentry::name(const QString &nm){blog_editor()->editor()->name(nm);}

//void Editentry::tree_path(const QString &path){
//	blog_editor()->editor()->tree_path(path);
//}
void Editentry::pin(const QString &pin_){blog_editor()->editor()->pin(pin_);}

// void Editentry::switch_pin(bool checked){blog_editor()->editor()->switch_pin(checked);}
void Editentry::author(const QString &author_){
	blog_editor()->editor()->author(author_);
}
void Editentry::home(const QString &home_){
	blog_editor()->editor()->home(home_);
}
void Editentry::url(const QString &url_){blog_editor()->editor()->url(url_);}
void Editentry::tags(const QString &tags_){
	blog_editor()->editor()->tags(tags_);
}

boost::intrusive_ptr<TreeItem> Editentry::item(){
	return blog_editor()->editor()->item();
}
void Editentry::bind(boost::intrusive_ptr<TreeItem> item_to_be_bound){
	blog_editor()->editor()->bind(item_to_be_bound);
}

const std::function<void (QObject *editor, QString saveString)>Editentry::save_callback() const {
	return blog_editor()->editor()->save_callback();
}
void Editentry::save_callback(const std::function<void (QObject *editor, QString saveString)> &func){
	blog_editor()->editor()->save_callback(func);
}

std::function<void (QObject *editor, QString &String)>Editentry::load_callback() const {
	return blog_editor()->editor()->load_callback();
}
void Editentry::load_callback(const std::function<void (QObject *editor, QString &String)> &func){
	blog_editor()->editor()->load_callback(func);
}

void Editentry::editor_load_callback(QObject *editor, QString &loadText){
	Editor::editor_load_callback(editor, loadText);
}
void Editentry::editor_save_callback(QObject *editor, const QString &saveText){
	Editor::editor_save_callback(editor, saveText);
}

bool Editentry::work_directory(QString dir_name){
	return blog_editor()->editor()->work_directory(dir_name);
}

QString Editentry::work_directory(){
	return blog_editor()->editor()->work_directory();
}

void Editentry::file_name(QString file_name_){
	blog_editor()->editor()->file_name(file_name_);
}

QString Editentry::file_name(){return blog_editor()->editor()->file_name();}

void Editentry::save_textarea(){blog_editor()->editor()->save_textarea();}

void Editentry::dir_file_empty_reaction(int mode){
	blog_editor()->editor()->dir_file_empty_reaction(mode);
}

int Editentry::dir_file_empty_reaction(){
	return blog_editor()->editor()->dir_file_empty_reaction();
}

void Editentry::misc_field(QString name, QString value){
	blog_editor()->editor()->misc_field(name, value);
}

QString Editentry::misc_field(QString name){
	return blog_editor()->editor()->misc_field(name);
}

bool Editentry::load_textarea(){
	return blog_editor()->editor()->load_textarea();
}

int Editentry::cursor_position(){
	return blog_editor()->editor()->cursor_position();
}

void Editentry::cursor_position(int n){
	blog_editor()->editor()->cursor_position(n);
}

int Editentry::scrollbar_position(){
	return blog_editor()->editor()->scrollbar_position();
}

void Editentry::scrollbar_position(int n){
	blog_editor()->editor()->scrollbar_position(n);
}

QTextDocument *Editentry::textarea_document(){
	return blog_editor()->editor()->document();
}

void Editentry::textarea_editable(bool editable){
	blog_editor()->editor()->textarea_editable(editable);
}

void Editentry::back_callback(void (*func)()){
	blog_editor()->editor()->back_callback(func);
}

void Editentry::clear_all(){blog_editor()->editor()->clear_all();}

FlatToolButton *Editentry::to_attach() const {
	return blog_editor()->editor()->_to_attach;
}

QIcon Editentry::icon_attach_exists() const {
	return blog_editor()->editor()->_icon_attach_exists;
}

QIcon Editentry::icon_attach_not_exists() const {
	return blog_editor()->editor()->_icon_attach_not_exists;
}

QVBoxLayout *Editentry::textformat_buttons_layout() const {
	return blog_editor()->editor()->_textformat_buttons_layout;
}

EditorTextArea *Editentry::text_area() const {
	return blog_editor()->editor()->_text_area;
}

void Editentry::to_editor_layout(){
	blog_editor()->editor()->to_editor_layout();
}

void Editentry::to_attach_layout(){
	blog_editor()->editor()->to_attach_layout();
}

MetaEditor *Editentry::editor(){return blog_editor()->editor();}

EditingWindow *Editentry::blog_editor() const {return _blog_editor;}

EditingWindow *Editentry::blog_editor(){
	if(! _blog_editor)
		_blog_editor = add_blog_editor(
			new EditingWindow(
				_tree_screen
					     , _entrance
					     , _profile
					     , _find_screen
					     , this
					     , _flags
					     , _style_source
					 ));
	return _blog_editor;
}

// WebView *Editentry::find(const std::function<bool(boost::intrusive_ptr<const
// TreeItem>)> &_equal) const
// {

// WebView * v = nullptr;
////        if(_binder) {
// for(auto i : _browsers) {
// if(i) {
// v = i->tabWidget()->find(_equal);

// if(v != nullptr) {  // setWidget(i.data());
////                dp.first = i.data();
// break;
// }
// }

////            else if(i->isVisible() || i->isActiveWindow()) {
////                dp.first = i.data();
////            }
// }

// boost::intrusive_ptr<const TreeItem> found_myself(nullptr);
// if(v) {
// if(v->page()) {
// if(v->page()->binder()) {
// auto _this = v->page()->host(); //
// globalparameters.Editentry()->find(_equal);
// if(_this) {
// assert(_this == v->page()->binder()->item());

// if(_this && v->load_finished()) {
// // && (v->tabmanager()->currentWebView() == v)
// found_myself = _this;
// }
// }
// }
// }
// }
////        }
// if(!found_myself) v = nullptr;
// return v;   // found_myself;
// }

// boost::intrusive_ptr<const TreeItem>
// Editentry::is_registered_to_browser(const
// std::function<bool(boost::intrusive_ptr<const TreeItem>)> &_equal) const
// {
// boost::intrusive_ptr<const TreeItem> found(nullptr);

////    bool re = false;
// if(_binder) {
////            auto _Editentry = globalparameters.Editentry();
// auto view = // _Editentry->
// find(_equal
////                [&] (boost::intrusive_ptr<const TreeItem> it) {
////                return it->id() == this->id();
////            }
// );

// if(view) {
// auto binder = view->page()->binder();

// if(binder) {
// auto page = binder->page();

// if(page) {
// if(binder == _binder
// && binder->item().get() == this // boost::intrusive_ptr<const TreeItem>(this)
// ) {
// found = this; // page_binder->bounded_item();
// }
// }
// }
// }
// }

////    if(page_valid()) {
////        auto p = _record_binder->bounded_page();
////        auto v = p->view();

////        if(v) {
////            auto _tabmanager = p->tabmanager();

////            if(_tabmanager) {
////                if(_tabmanager->indexOf(v) != -1) {
////                    re = true;
////                    found = p->record_binder()->bounded_item();
////                    assert(found);
////                }
////            }

////            //
/// if(_record_binder->bounded_page()->view()->record_controller()) {
////            //                auto _record_controller =
/// _record_binder->bounded_page()->view()->record_controller();
////            //                found =
/// _record_controller->source_model()->find_current_bound(boost::intrusive_ptr<TreeItem>(this));
////            //            }
////        }
////    }

////    //    else {
////    //        browser::Editentry *_Editentry = globalparameters.Editentry();

////    //        for(int w = 0; w < _Editentry->browsers().size(); w++) {
////    //            auto tabmanager =
/// _Editentry->browsers().at(w)->record_screen()->tabmanager();  //
/// record_controller()->source_model();  // ->record_table();

////    //            for(int i = 0; i < tabmanager->count(); i++) {
////    //                auto item =
/// tabmanager->webView(i)->page()->current_item();

////    //                if(item->field("id") == id()) {
////    //                    found = item;
////    //                }
////    //            }
////    //        }
////    //    }

////    assert((found && re) || (!found && !re));
//// assert(found);

////    if(found && found != boost::intrusive_ptr<TreeItem>(this)) {
////        found =
/// globalparameters.tree_screen()->duplicated_remove(boost::intrusive_ptr<TreeItem>(this),
/// found);
////    }

// return found;
// }
// }

EditingWindow *Editentry::add_blog_editor(EditingWindow *ew){
	if(! _editing_list.contains(ew)) _editing_list << ew;
	_blog_editor = ew;
	return ew;
}

SuperMenu *Editentry::super_menu(){return _super_menu;}

void Editentry::editor_switch(void){
//    auto *_editenty = globalparameters.edit_entry();			// find_object<MetaEditor>(meta_editor_singleton_name);
//	auto _app = qobject_cast<sapp_t *>(qApp);
	if(! (this->isVisible())){
		this->show();
//		emit _app->editing_win_changed(_editenty->blog_editor());
		emit this->editing_activated(blog_editor());
//		_app->sendEvent(_editenty->blog_editor(), new QEvent(QEvent::WindowActivate));

		appconfig.editor_show(true);
	}else{
		this->hide();
//		_app->sendEvent(_editenty->blog_editor(), new QEvent(QEvent::WindowActivate));

		appconfig.editor_show(false);
	}
}
