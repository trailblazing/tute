/*******************************************************************************

   QTM - Qt-based blog manager
   Copyright (C) 2006-2011 Matthew J Smith

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License (version 2), as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*******************************************************************************/

// EditingWindow.cc - Main window class for QTM applicaiton

// #define QTM_DEBUG

#include <limits>
// #include <QApplication>
// #include <QCoreApplication>
#include <QAction>
#include <QChar>
#include <QCheckBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QCursor>
#include <QDataStream>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QFontDialog>
#include <QHBoxLayout>
#include <QHash>
#include <QHashIterator>
#include <QHostInfo>
#include <QIODevice>
#include <QIcon>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QPalette>
#include <QPixmap>
#include <QPoint>
#include <QProcess>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QRect>
#include <QRegExp>
#include <QRegExpValidator>
#include <QResizeEvent>
#include <QSettings>
#include <QSize>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTemporaryFile>
#include <QTextBrowser>
#include <QTextCharFormat>
#include <QTextCodec>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextEdit>
#include <QTextStream>
#include <QTimer>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWhatsThis>
#include <QWidget>

//#include "models/tree/tree_item.dec"

#if !defined DONT_USE_PTE

#include <QPlainTextEdit>
class QPlainTextEdit;

#else //DONT_USE_PTE

#ifdef USE_WYEDIT

#ifdef USE_EDITOR_WRAP
#include "views/record/editor_wrap.h"
class EditorWrap;
//#define TEXTEDIT_FOR_READ QTextEdit
class EditorWrap;
//#define TEXTEDIT EditorWrap
typedef EditorWrap TEXTEDIT;
#else //USE_EDITOR_WRAP

#include "libraries/wyedit/editor.h"
class Editor;
typedef Editor TEXTEDIT;

#endif // USE_EDITOR_WRAP

#else //USE_WYEDIT

#include <QTextEdit>

#endif //USE_WYEDIT

#endif //DONT_USE_PTE

#include "xml_rpc_handler.h"
// #include "libraries/flat_control.h"
#include "blogger.h"

#include "libraries/global_parameters.h"
#include "libraries/walk_history.h"
#include "libraries/wyedit/editor.h"
#include "views/tree/tree_view.h"

#include "libraries/disk_helper.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "models/attach_table/attach_table_data.h"
#include "views/attach_table/attach_table_screen.h"
#include "views/browser/autosaver.h"
#include "views/browser/browser.h"
#include "views/browser/docker.h"
#include "views/browser/tabwidget.h"
#include "views/browser/webview.h"
#include "views/main_window/hidable_tab.h"
#include "views/main_window/main_window.h"
#include "views/print_preview/print_preview.h"
#include "views/record_table/record_screen.h"
// extern std::shared_ptr<AppConfig> appconfig;

extern WalkHistory walkhistory;
extern std::shared_ptr<gl_para> gl_paras;
extern const char* program_title;
extern const std::string editor_prefix;

#ifdef USE_SYSTRAYICON
#include "sys_tray_icon.h"
#endif
#include "side_tabwidget.h"
#include "super_menu.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#if QT_VERSION >= 0x050000
#ifdef Q_OS_UNIX
#if !defined Q_OS_MAC && !defined Q_OS_CYGWIN
#define Q_WS_X11
#endif // !defined etc
#endif // Q_OS_UNIX
#ifdef Q_OS_WIN
#ifndef WIN_QTV
#define WIN_QTV
#endif // WIN_QTV
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#include <QWindow>
#endif // #Q_OS_WIN
#endif

// #ifdef Q_OS_MAC
// #include <CoreFoundation/CoreFoundation.h>
// #include <CoreFoundation/CFBundle.h>
// #endif

#include "qtm_version.h.in"
#if defined Q_WS_X11 || defined Q_OS_WIN32
#include "markdown_header.h.in"
#endif

#include <QtAlgorithms>
#include <QtNetwork>
#include <QtXml>
#include <cmath>

// #include "addimg.xpm"
// #include "addlink.xpm"
// #include "blog-this.xpm"
// #include "fileopen.xpm"
#include "filesave.xpm"
// #include "more.xpm"
// #include "paragraph.xpm"
// #include "preview.xpm"
// #include "textbold.xpm"
// #include "textital.xpm"
// #include "textul.xpm"
// #include "bquote.xpm"
#include "addctag.xpm"
#include "addtag.xpm"
#include "remtag.xpm"

#include "main.h"

#ifdef Q_OS_MAC
// #include "macFunctions.h"
#endif

//#define editor_object _editor
constexpr char Blogger::_default_filename[];

Blogger::Blogger(QString const& new_post_topic, QString const& new_post_content, QStringList hide_editor_tools_, const QByteArray& state_, Qt::WindowFlags flags)
    : super(nullptr, flags)
#ifdef USE_SIGNAL_CLOSE
    , sd::renter() //sd::renter<Blogger>()
#endif
    , _current_topic_name([&] {auto topic_copy = new_post_topic; if("" == new_post_topic) topic_copy = gl_para::_default_topic; return purify(topic_copy); }()) //(tr(new_post_topic.remove(QRegExp("[\"/\\\\<>\\?:\\*\\|]+")).toStdString().c_str()))
    , _current_topic_full_folder_name([&]() -> QString {
	    assert(gl_paras->editors_shared_full_path_name() != "");
	    auto fn = gl_paras->editors_shared_full_path_name() + "/" + _current_topic_name;
	    assert(fn != "");
	    return fn;
    }())
    , _current_topic_full_config_name(_current_topic_full_folder_name + "/" + gl_para::_editor_conf_filename)
    , _topic_editor_config([&]() -> std::shared_ptr<QSettings> {
	    if (!QDir(_current_topic_full_folder_name).exists())
		    if (!QDir::root().mkpath(_current_topic_full_folder_name)) critical_error("Can not create directory: \"" + _current_topic_full_folder_name + "\"");
	    auto conf_file_info = std::make_shared<QFileInfo>(_current_topic_full_config_name);
	    conf_file_info = DiskHelper::recover_ini_file(conf_file_info, true);
	    if ((QFile::ReadUser | QFile::WriteUser) != (QFile::permissions(_current_topic_full_config_name) & (QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(_current_topic_full_config_name, QFile::ReadUser | QFile::WriteUser);
	    return std::make_shared<QSettings>(_current_topic_full_config_name, QSettings::IniFormat);
    }())
    , _local_storage_file_extension(_topic_editor_config->value("local_storage_file_ext", "cqt").toString())
    , _filename(_current_topic_full_folder_name + "/" + _default_filename + "." + _local_storage_file_extension)
    , _dirty_indicator(new QLabel(this))
    , _current_reply(nullptr)
    , _vtab_record(gl_paras->vtab_record())
    , _central_widget([&]() -> QWidget* {_central_widget = nullptr; auto cw = new QWidget(this); return cw; }())
    , _splitter([&]() -> QSplitter* {_splitter = nullptr; auto sp = new QSplitter(Qt::Horizontal, _central_widget); return sp; }())
    , _main_stack([&]() -> QStackedWidget* {_main_stack = nullptr; auto ms = new QStackedWidget(_central_widget); return ms; }())
    , _console([&]() -> TEXTEDIT_FOR_READ* {_console = nullptr; auto e = new TEXTEDIT_FOR_READ(_main_stack); return e; }())
    // Set up editor widget
    , _editor(
#ifdef USE_WYEDIT

	  [&]() -> TEXTEDIT* {
		  _editor = nullptr;
		  auto e = new TEXTEDIT(
#ifdef USE_EDITOR_WRAP
		      find_screen, this, hide_editor_tools_, _main_stack, ""
#else
		      _main_stack, this, _topic_editor_config, (appconfig->interface_mode() == "desktop") ? Editor::WYEDIT_DESKTOP_MODE : Editor::WYEDIT_MOBILE_MODE, hide_editor_tools_, true, true
#endif // USE_EDITOR_WRAP
		      );
		  return e;
	  }()

#else
	  [&]() -> TEXTEDIT* {_editor = nullptr; auto e = new TEXTEDIT(_main_stack); return e; }()
#endif
	      )
    , _control_tab(
	  [&]() -> SideTabWidget* {
	  _control_tab = nullptr;
	  auto st = new SideTabWidget(gl_paras->editor_docker(), this, _topic_editor_config, _splitter, _central_widget);
	  st->topic(_current_topic_name);
	  st->title(_current_topic_name);
	  return st; }())
    , _browser(nullptr //new web::Browser(this, state_)
	  //	  [&] {
	  //	    _browser.detach(); // = nullptr; //= sd::intrusive_ptr<web::Browser>(nullptr);
	  //	    return             //auto bro =
	  //		sd::make_intrusive<web::Browser>(this, state_);
	  //	    //	    return bro;
	  //    }()
	  )
    //    , _record_screen(sd::make_intrusive<rs_t>(this, state_))
    , _editor_docker(gl_paras->editor_docker()->prepend(this, flags))
    , _super_menu([&]() -> SuperMenu* {_super_menu = nullptr; auto sm = new SuperMenu(this); return sm; }())
{
	//
	//	_editor =
	//#ifdef USE_WYEDIT

	//	    [&]() -> TEXTEDIT* {
	//		//		_editor = nullptr;
	//		auto e = new TEXTEDIT(
	//#ifdef USE_EDITOR_WRAP
	//		    find_screen, this, hide_editor_tools_, _main_stack, ""
	//#else
	//		    _main_stack, this, _topic_editor_config, (appconfig->interface_mode() == "desktop") ? Editor::WYEDIT_DESKTOP_MODE : Editor::WYEDIT_MOBILE_MODE, hide_editor_tools_, true, true
	//#endif // USE_EDITOR_WRAP
	//		    );
	//		return e;
	//	}()

	//#else
	//	    [&]() -> TEXTEDIT* {_editor = nullptr; auto e = new TEXTEDIT(_main_stack); return e; }()
	//#endif
	//	    ;
	//	_control_tab = [&]() -> SideTabWidget* {
	////		_control_tab = nullptr;
	//		auto st = new SideTabWidget(gl_paras->editor_docker(), this, _topic_editor_config, _splitter, _central_widget);
	//		st->topic(_current_topic_name);
	//		st->title(_current_topic_name);
	//		return st; }();
	_browser = new web::Browser(this, state_);

	auto id = this->misc_field<id_key>(); // static_cast<id_value>(_control_tab->topic());//
	if (detail::to_string(id) == "") {

		auto v = _browser->currentTab();
		if (v) {
			auto p = v->page();
			if (p) {
				auto h = p->host();
				if (h) {
					this->history_sychronize(h); //misc_field<id_key>(h->id());
					id = this->misc_field<id_key>();
				}
			}
		}
	}
	assert(detail::to_string(id) != "");

	QDomElement detailElem, attribElem, nameElem, serverElem, locElem, loginElem, pwdElem;
	// QSettings _topic_editor_config(_current_topic_config_name,
	// QSettings::IniFormat);

	_splitter->setHandleWidth(0);
	_splitter->addWidget(_control_tab);

	super::setWindowFlags(Qt::FramelessWindowHint);
	auto stack_layout = new QVBoxLayout();
	stack_layout->setMargin(0);
	stack_layout->setContentsMargins(0, 0, 0, 0);
	_main_stack->setLayout(stack_layout);
	_splitter->addWidget(_main_stack);
	_splitter->setCollapsible(0, false);
	_splitter->setCollapsible(1, false);

	QStringList line_list;
	QList<int> list;
	_topic_editor_config->beginGroup("geometry");
	line_list = (_topic_editor_config->value("splitter_sizelist", "100,100")).toString().split(",");
	for (int i = 0; i < line_list.size(); ++i) list.append(line_list.at(i).toInt());
	auto collapsed = _topic_editor_config->value("collapsed", 0).toBool();
	// HidableTabWidget::_editentry->_blog_editor->_splitter->setSizes(list);
	_topic_editor_config->endGroup();
	// if(_control_panel->tabBar()->sizeHint().width() >= list[0]){
	// _control_panel->collapse_when_true(true);
	//// emit _hide_action->toggle(false);
	// }else{
	// _control_panel->resize(list[0],
	// _control_panel->parentWidget()->geometry().height());        //
	// parentWidget()->geometry().width() * 38 / 100    // 284
	//// 428
	// }
	// _splitter->setSizes(list);
	// emit _splitter->splitterMoved(list[0], 0);
	//
	setAttribute(Qt::WA_DeleteOnClose);

//	_app = sapp_t::instance();//qobject_cast<sapp_t *>(sapp_t::instance());

#ifdef Q_OS_MAC
	SuperMenu* thisSM;
	Q_FOREACH (QWidget* w, qtm->topLevelWidgets()) {
		if (w->objectName() == "MacGlobalMenu") {
			thisSM = qobject_cast<SuperMenu*>(w);
			if (thisSM) superMenu = thisSM;
			break;
		}
	}
#endif

	doUiSetup();

	xmlEntities["quot"] = '"';
	xmlEntities["amp"] = '&';
	xmlEntities["gt"] = '>';
	xmlEntities["lt"] = '<';
	xmlEntities["apos"] = '\'';

	readSettings();
	setEditorColors();
	setTextFonts();

	_control_tab->chAllowComments->setEnabled(true);
	_control_tab->chAllowComments->setCheckState(allowComments ? Qt::Checked : Qt::Unchecked);
	_control_tab->chAllowTB->setCheckState(allowTB ? Qt::Checked : Qt::Unchecked);
	_control_tab->show();

	handleEnableCategories();

	QFile accountsXmlFile(PROPERSEPS(QString("%1/qtmaccounts2.xml").arg(gl_paras->editors_shared_full_path_name())));
	if (_accounts_dom.setContent(&accountsXmlFile)) {
		accountsXmlFile.close();
		setInitialAccount();
	} else {
		accountsXmlFile.close();
		accountsXmlFile.setFileName(
		    PROPERSEPS(QString("%1/qtmaccounts.xml").arg(gl_paras->editors_shared_full_path_name())));
		accountsXmlFile.open(QIODevice::ReadOnly | QIODevice::Text);
		if (_accounts_dom.setContent(&accountsXmlFile)) {
			accountsXmlFile.close();
			setInitialAccount();
		} else {
#ifndef NO_DEBUG_OUTPUT
// qDebug() << "Can't read the XML";
#endif
			accountsXmlFile.close();
			accountsElement =
			    _accounts_dom.createElement(program_title_qstring + "Accounts");

			readServerSettings();
			if (!server.isEmpty()) {
				// qDebug() << "copying details to new default element";
				currentAccountElement = _accounts_dom.createElement("account");
				currentAccountElement.setAttribute("id", "default");
				detailElem = _accounts_dom.createElement("details");
				nameElem = _accounts_dom.createElement("title");
				nameElem.appendChild(
				    _accounts_dom.createTextNode(tr("Default account")));
				serverElem = _accounts_dom.createElement("server");
				serverElem.appendChild(_accounts_dom.createTextNode(server));
				locElem = _accounts_dom.createElement("location");
				locElem.appendChild(_accounts_dom.createTextNode(location));
				loginElem = _accounts_dom.createElement("login");
				loginElem.appendChild(_accounts_dom.createTextNode(login));
				pwdElem = _accounts_dom.createElement("password");
				pwdElem.appendChild(_accounts_dom.createTextNode(password));
				detailElem.appendChild(nameElem);
				detailElem.appendChild(serverElem);
				detailElem.appendChild(locElem);
				detailElem.appendChild(loginElem);
				detailElem.appendChild(pwdElem);
				currentAccountElement.appendChild(detailElem);

				// Delete the old account from the settings
				_topic_editor_config->beginGroup("account");
				_topic_editor_config->remove("server");
				_topic_editor_config->remove("location");
				_topic_editor_config->remove("login");
				_topic_editor_config->remove("password");
				_topic_editor_config->endGroup();

				// Now transfer the attributes to the default accounts
				QStringList attribs(accountAttributes.keys());
				Q_FOREACH (QString s, attribs) {
					if (*(accountAttributes[s])) {
						attribElem = _accounts_dom.createElement("attribute");
						attribElem.setAttribute("name", s);
						detailElem.appendChild(attribElem);
					}
				}
			}
			extractAccountDetails();

			accountsElement.appendChild(currentAccountElement);
			_accounts_dom.appendChild(accountsElement);
			_accounts_dom.insertBefore(
			    _accounts_dom.createProcessingInstruction("xml", "version=\"1.0\""), _accounts_dom.firstChild());
			QHostInfo::lookupHost(server, this, SLOT(handleInitialLookup(QHostInfo)));
		}
	}
	checkForEmptySettings();

#ifdef USE_WYEDIT
	_editor->textarea(new_post_content);
#else
	editor_object->setPlainText(new_post_content);
#endif
	// _control_tab->title(new_post_topic);
	// _control_tab->topic(new_post_topic);
	//
	_main_stack->setCurrentIndex(_editor_id);

	// setPostClean();
	setDirtySignals(true);
	_dirty_indicator->hide();

	_editor_docker->setWidget(this);
	setParent(_editor_docker);
	//	emit _editor_docker->editing_activated(this);
	// _editor_dock->add_blog_editor(this);

	// _blog_editor = new EditingWindow();
	setSTI(0); // No STI
	setWindowTitle(QObject::tr((program_title_string + " - new entry [*]").c_str()));
	if (handleArguments()) {
		show();
		raise();
	} else
		close();
	//	connect(_editor_docker, &EditorDock::editing_activated, _super_menu,
	//&SuperMenu::editing_window);
	//	connect(_editor_docker, &EditorDock::editing_activated, [&](Bloger
	//*){adjustSize();});

	{
	    //		connect(_vtab_record, &HidableTab::currentChanged,
	    //[&](int index){
	    //				if(index < 0) index = 0;
	    //				auto cw = _vtab_record->widget(index);
	    //				auto crs = dynamic_cast<rs_t *>(cw);
	    //				if(crs){
	    //				        for(int i = 0; i <
	    //_vtab_record->count(); i++){
	    //				                auto w =
	    //_vtab_record->widget(i);

	    //				                auto rs =
	    //dynamic_cast<rs_t *>(w);
	    //				                if(rs == crs){
	    //				                        auto blogger_ =
	    //rs->blogger();
	    //				                        if(blogger_){
	    //				                                blogger_->show();
	    //				                                blogger_->activateWindow();
	    //				                                if(!blogger_->isTopLevel())
	    //blogger_->raise();
	    //				                                _editor_docker->setWidget(blogger_);
	    //				                                blogger_->setParent(_editor_docker);
	    //				                                blogger_->adjustSize();
	    //							}
	    //						}else{
	    //				                        auto blogger_ =
	    //rs->blogger();
	    //				                        if(blogger_){
	    //				                                blogger_->hide();
	    //				                                if(blogger_->isTopLevel())
	    //blogger_->lower();
	    //							}
	    //						}
	    //					}
	    //				}
	    //			});
	} ////    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	  ///
	{
		auto edh = _editor_docker->geometry().height();
		if (edh == 0) edh = _editor_docker->sizeHint().height();
		auto edw = _editor_docker->geometry().width();
		if (edw == 0) edw = _editor_docker->sizeHint().width();
		auto ch = _central_widget->geometry().height();
		if (ch == 0) ch = _central_widget->sizeHint().height();
		auto cw = _central_widget->geometry().width();
		if (cw == 0) cw = _central_widget->sizeHint().width();
		// _central_widget->resize(edw, edh);
		////    QSettings   qtm_settings(_config_file_name,
		/// QSettings::IniFormat);
		// auto    barwidth_geometry   =
		// _control_panel->tabBar()->geometry().width();
		// auto    whole_width         = _control_panel->width();
		// auto    bar_width           =
		// _control_panel->tabBar()->width();

		auto bar_width = _control_tab->tabBar()->sizeHint().width();
		if (list[0] <= bar_width || collapsed) {
			_control_tab->collapse_when_true(true);
			// auto list = _splitter->sizes();

			list[1] = width() - bar_width;
			list[0] = bar_width;
			// _splitter->setSizes(list);
			// emit _splitter->splitterMoved(list[0], 0);
		} else {
			_control_tab->collapse_when_true(false);
			// 284
			// 428
		}
		_control_tab->resize(list[0], _control_tab->parentWidget()->geometry().height());
		_splitter->setSizes(list);
		_main_stack->setMaximumWidth(std::numeric_limits<int>::max());
		_editor->setMaximumWidth(std::numeric_limits<int>::max());
		_main_stack->resize(list[1], ch);
		_editor->resize(list[1], ch);
		assert(list[1] == _editor->width());
		emit _splitter->splitterMoved(list[0], 0);
	}
// _main_stack->hide();// for debug
// _console->hide();status
// _main_stack->setCurrentIndex(_console_id);
#ifdef USE_SIGNAL_CLOSE
	//	bool _closed = false;
	destroy_transfer([&](renter* const r) {
		(void)r;
		if (r != this) { // && !this->_destroyed_request_sent
			assert(this->testAttribute(Qt::WA_DeleteOnClose));
			close_trigger_from_others()(r);
			//			this->_destroyed_request_sent =
			this->close();
			//			delete this;
		}
	}); //std::make_shared<sd::method<sd::meta_info<void>>>("", &Blogger::close, &_closed, this) //self_close_request.connect(std::bind(&Blogger::close, this));
#endif      //USE_SIGNAL_CLOSE

	if (_browser) {
		connect(this, &Blogger::topic_changed, _browser, &web::Browser::on_topic_changed);

//	connect(this, &Blogger::topic_changed, _record_screen, &rs_t::on_topic_changed);
#ifdef USE_SIGNAL_CLOSE
//		//		close_connect(std::make_shared<sd::method<sd::meta_info<void>>>("", &web::Browser::on_close_requested, _browser, static_cast<sd::renter* const>(this))); //self_close_request.connect(_browser->self_close_request); //(std::bind(&web::Browser::self_close_request, _browser));
//		destroy_transfer(_browser->destroy_trigger_from_others()); //
////		close_request_connect([=](renter* const r) {
////			(void)r;
////			if (_browser && r != _browser && !_browser->close_request_sent() && !_browser->destroy_request_sent()) {
////				_browser->close_requested_from_others(r);
////			}
////		}); //std::bind(&web::Browser::close_requested_from_others, _browser, static_cast<sd::renter* const>(this))
////                self_close_request.connect(std::bind(&web::Browser::test, _browser));//work
////                bool result = false;
////                close_connect(sd::method<sd::meta_info<void>>("", &web::Browser::close, &result, _browser));
////                //		assert(result);
#endif //USE_SIGNAL_CLOSE
	}
	connect(this, &Blogger::topic_changed, gl_paras->main_window(), &wn_t::on_topic_changed);
}

Blogger::~Blogger()
{
//	this->_destroyed_request_triggered = true;
#ifndef USE_SIGNAL_CLOSE
	_prepended(this);
#endif

//	//	close_requested(this);
//	//    _browser->save();
//	//	_browser->_autosaver->changeOccurred();
//	//	_browser->_autosaver->saveIfNeccessary();
//	// if(_editor_dock->_editing_list.contains(this))
//	// _editor_dock->_editing_list.removeOne(this);
//	//	deleteLater();
//	//	if (_record_screen) connect(this, &Blogger::close_request, _record_screen, &rs_t::on_blogger_close_requested);
//	if (_browser) {
//		//		self_close_request.connect(std::bind(&web::Browser::on_blogger_close_requested, _browser)); //connect(this, &Blogger::self_close_request, _browser, &web::Browser::on_blogger_close_requested);
//		//		//		auto rs = _browser->record_screen();
//		_browser = nullptr;
//	}
//	//	emit self_close_request(this);
//	delete _browser;
#ifdef USE_SIGNAL_CLOSE
	if (_browser) {
		if (!_browser->_close_request_sent && !_destroy_request_sent)
			_browser->destroy_trigger_from_others()(this);
	}
#endif // USE_SIGNAL_CLOSE
}

#ifdef USE_SYSTRAYICON
void Blogger::setSTI(SysTrayIcon* _sti)
{
	sti = _sti;
}
#endif
//
void Blogger::doUiSetup()
{
	sapp_t::instance()->setWindowIcon(QIcon(QLatin1String(":/resource/pic/logo.svg"))); // QPixmap(":/images/qtm-logo1.png")

#ifdef USE_SYSTRAYICON
	sti = 0;
#endif

#ifdef Q_OS_MAC
	setStatusBar(0);
	setMenuBar(0);
#else
// _super_menu = _editor->context_menu();  // new SuperMenu(_find_screen,
// _editentry, this);
// setMenuBar(superMenu);
// superMenu->setMainWindow(this);

#endif

	loadedEntryBlog = 999;
	_no_auto_save = false;
	_no_alpha_cats = false;
	_network_actions_enabled = false;
	setAttribute(Qt::WA_QuitOnClose);

	userAgentString =
	    QString(program_title_qstring + "/%1").arg(app_version); // QTM_VERSION
	userAgentArray = (program_title_string + "/").c_str();

	userAgentArray.append(app_version); // QTM_VERSION

	// QCoreApplication::setOrganizationName("Catkin Project");
	// QCoreApplication::setOrganizationDomain("catkin.blogistan.co.uk");
	// QCoreApplication::setApplicationName("QTM");
	// QCoreApplication::setApplicationVersion(QTM_VERSION);

	qtmaccounts_xml << "qtmaccounts2.xml"
			<< "qtmaccounts.xml";

	// ui.setupUi( this );
	_current_http_business = None;
	_doing_new_post = false;
	entryBlogged = false;
	_net_acess_manager = std::make_unique<QNetworkAccessManager>();

// Setup main signals and slots

/*ui.actionPrint->setEnabled( false );    // implemented
    ui.actionClose_and_Delete->setEnabled( false ); */

#ifndef USE_SYSTRAYICON
	if (superMenu) superMenu->qptAction->setVisible(false);
#endif

	// QHBoxLayout *toolsLine1Layout = new QHBoxLayout();
	// toolsLine1Layout->addWidget(toolBar);
	// toolsLine1Layout->addStretch();

	// toolsLine1Layout->addWidget(_close_button);

	// Actions related to Technorati tags
	auto default_icon = sapp_t::instance()->defaultIcon(); //qobject_cast<sapp_t*>(sapp_t::instance())
	addTechTagAction = add_action<QListWidget, QAction, SideTabWidget>(
	    _control_tab->lwTags, _control_tab, tr("Add Technorati tag"), tr("Add technorati tag"), tr(""), QIcon(QPixmap(addtag_xpm)), [&](bool) { addTechTag(); });
	addClipTechTagAction = add_action<QListWidget, QAction, SideTabWidget>(
	    _control_tab->lwTags, _control_tab, tr("Add Technorati tag from clipboard"), tr("Add Technorati tag from clipboard"), tr(""), QIcon(QPixmap(addctag_xpm)), [&](bool) { addClipTag(); });
	removeTechTagAction = add_action<QListWidget, QAction, SideTabWidget>(
	    _control_tab->lwTags, _control_tab, tr("Remove Technorati tag"), tr("Remove Technorati tag"), tr(""), QIcon(QPixmap(remtag_xpm)), [&](bool) { removeTechTag(); });
	// addTechTagAction->setIcon(QIcon(QPixmap(addtag_xpm)));
	// addClipTechTagAction->setIcon(QIcon(QPixmap(addctag_xpm)));
	// removeTechTagAction->setIcon(QIcon(QPixmap(remtag_xpm)));

	// Actions related to Trackback pings

	addTBPingAction = add_action<QListWidget, QAction, SideTabWidget>(
	    _control_tab->lwTBPings, _control_tab, tr("Add trackback ping"), tr("Add trackback ping"), tr(""), default_icon, [&](bool) { addTBPing(); });
	addClipPingAction = add_action<QListWidget, QAction, SideTabWidget>(
	    _control_tab->lwTBPings, _control_tab, tr("Add ping from clipboard"), tr("Add ping from clipboard"), tr(""), default_icon, [&](bool) { addClipTBPing(); });
	removeTBPingAction = add_action<QListWidget, QAction, SideTabWidget>(
	    _control_tab->lwTBPings, _control_tab, tr("Remove trackback ping"), tr("Remove trackback ping"), tr(""), default_icon, [&](bool) { removeTBPing(); });

	// Set up the main layout

	// cWidget = new QWidget(this);

	// leftWidget        = new QWidget(cWidget);
	_main_layout = new QHBoxLayout;
	// _main_layout->setMargin(0);
	// _main_layout->setMargin(5);
	// cw->setupUi(leftWidget);
	_main_layout->setSpacing(0);
	_main_layout->setMargin(0);

	connect(_control_tab->lwOtherCats, &QListWidget::itemSelectionChanged, this, &Blogger::changeOtherCatsHeading);
	_control_tab->cbBlogSelector->setMaxVisibleItems(10);
	_control_tab->cbMainCat->setMaxVisibleItems(10);
	// _control_panel->lwTags->addAction(addTechTagAction);
	// _control_panel->lwTags->addAction(addClipTechTagAction);
	// _control_panel->lwTags->addAction(removeTechTagAction);
	// _control_panel->lwTBPings->addAction(addTBPingAction);
	// _control_panel->lwTBPings->addAction(addClipPingAction);
	// _control_panel->lwTBPings->addAction(removeTBPingAction);

	addKeywordTag_forList = add_action<QListWidget, QAction, QWidget>(
	    _control_tab->lwAvailKeywordTags, nullptr, tr("Add this tag"), tr("Add this tag"), tr(""), default_icon, [&](bool) { addKeywordTagFromAvailTags(); });
	removeKeywordTag_forList = add_action<QListWidget, QAction, QWidget>(
	    _control_tab->lwKeywordTags, nullptr, tr("Remove this tag"), tr("Remove this tag"), tr(""), default_icon, [&](bool) { removeKeywordTag(); });
	_refreshKeywordTags = add_action<QListWidget, QAction, QWidget>(
	    _control_tab->lwAvailKeywordTags, nullptr, tr("Refresh"), tr("Refresh"), tr(""), default_icon, [&](bool) { refreshKeywordTags(); });
	// connect(addKeywordTag_forList, SIGNAL(triggered(bool)), this,
	// SLOT(addKeywordTagFromAvailTags()));
	// connect(removeKeywordTag_forList, SIGNAL(triggered(bool)), this,
	// SLOT(removeKeywordTag()));
	// connect(_refreshKeywordTags, SIGNAL(triggered(bool)), this,
	// SLOT(refreshKeywordTags()));
	// _control_panel->lwKeywordTags->addAction(removeKeywordTag_forList);
	// _control_panel->lwAvailKeywordTags->addAction(addKeywordTag_forList);
	// _control_panel->lwAvailKeywordTags->addAction(_refreshKeywordTags);

	connect(_control_tab->lwAvailKeywordTags, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addKeywordTagFromAvailTags()));
	connect(_control_tab->leAddKeywordTag, SIGNAL(returnPressed()), this, SLOT(addKeywordTagFromLineEdit()));
	connect(_control_tab->tbAddKeywordTag, SIGNAL(clicked(bool)), this, SLOT(addKeywordTagFromLineEdit()));
	_control_tab->tbSelectKeywordTag->setIcon(QIcon(":/images/next.png"));
	_control_tab->tbRemoveKeywordTag->setIcon(QIcon(":/images/previous.png"));

	_child_category_here_action = add_action<QListWidget, QAction, QListWidget>(
	    _control_tab->lwOtherCats, _control_tab->lwOtherCats, tr("Add child category"), tr("Add child category"), tr(""), default_icon, [&](bool) { newChildCategory(); });
// connect(childCategoryHereAction, SIGNAL(triggered(bool)), this,
// SLOT(newChildCategory()));
// _control_panel->lwOtherCats->addAction(childCategoryHereAction);

#ifdef Q_OS_MAC
	cw->lwOtherCats->setWhatsThis(
	    tr("Secondary categories, if your blog system supports "
	       "them.  To highlight more than one category, press "
	       "Command and click the mouse, or the left mouse button."));
#endif

	tagValidator = new QRegExpValidator(
	    QRegExp("([a-zA-Z0-9\\.%]+[\\+ ])*[a-zA-Z0-9\\.%]+"), this);
	_control_tab->leAddTag->setValidator(tagValidator);
	_control_tab->cbDoTB->hide();
	////    add a toolbar at the top of left panel
	// QVBoxLayout *left_layout = new QVBoxLayout();
	////    left_layout->addWidget(toolBar);
	// left_layout->addWidget(_control_panel); //
	// Set up main stack widget

	// mainStack     = new QStackedWidget(cWidget);
	_preview_window = new QTextBrowser(_main_stack);

	_preview_id = _main_stack->addWidget(_preview_window);
	_preview_window->setOpenExternalLinks(true);
	_preview_window->installEventFilter(this);

	// Set up console
	// _console = new TEXTEDIT_FOR_READ(_main_stack);

	_console_id = _main_stack->addWidget(_console);

	_console->setReadOnly(true);

	_console->installEventFilter(this);
	_console->setToolTip("Readonly editor");
// Set up editor widget
// #ifdef USE_WYEDIT
// _editor = new TEXTEDIT("", _main_stack, _find_screen, _editentry);
// #else
// _editor = new TEXTEDIT(mainStack);
// #endif

#if defined DONT_USE_PTE
#ifdef USE_WYEDIT
#else
	editor_object->setAcceptRichText(false);
#endif
#endif
	//
	_editor_id = _main_stack->addWidget(_editor);
//
#ifdef USE_WYEDIT
#else
	editor_object->setReadOnly(false);
#endif
	_previous_raised_ls_widget = _editor_id;
	_main_layout->addWidget(_splitter);

	// mainWindowLayout->addWidget(_control_panel, 3);   //
	// mainWindowLayout->addLayout(left_layout, 3);//
	// mainWindowLayout->addWidget(_main_stack, 6);
	// mainWindowLayout->setSpacing(1);
	_main_layout->setMargin(0);
	_main_layout->setContentsMargins(0, 0, 0, 0);
	_main_layout->setSpacing(0);

#ifdef EDITOR_CONTEXT_MENU_EXPRESS
	{ // supermenu does the samething
		// Set up editor widget's actions
		auto editor_object_ = editor_object->_text_area;
		editor_object_->setContextMenuPolicy(Qt::ActionsContextMenu);
		QList<QAction*> editorActions;
		QAction* sep1 = new QAction(editor_object_);
		sep1->setSeparator(true);
		QAction* sep2 = new QAction(editor_object_);
		sep2->setSeparator(true);
		QAction* actionSelectAll = new QAction(tr("Select all"), editor_object_);
		connect(actionSelectAll, SIGNAL(triggered(bool)), editor_object_, SLOT(selectAll()));
		actionSelectAll->setShortcut(QKeySequence::fromString("Ctrl+A"));
		editorActions << _super_menu->getUndoAction()
			      << _super_menu->getRedoAction() << sep1;
		editorActions << _super_menu->getCutAction() << _super_menu->getCopyAction()
			      << _super_menu->getPasteAction() << sep2;
		editorActions << actionSelectAll;
		editor_object_->addActions(editorActions);
		editor_object_->installEventFilter(this);
	}
#endif

	// Set up search widget
	_main_layout_with_search = new QVBoxLayout(_central_widget);
	_search_widget = new QijSearchWidget(_editor, this);
	_search_widget->hide();
	// mainWindowLayoutWithSearch->setSpacing(1);
	// mainWindowLayoutWithSearch->setContentsMargins(5, 5, 5, 5);
	// mainWindowLayoutWithSearch->addLayout(toolsLine1Layout);
	_main_layout_with_search->setMargin(0);
	_main_layout_with_search->setContentsMargins(0, 0, 0, 0);
	_main_layout_with_search->setSpacing(0);

	_main_layout_with_search->addLayout(_main_layout);
	_main_layout_with_search->addWidget(_search_widget);
	_central_widget->setLayout(_main_layout_with_search);
	setCentralWidget(_central_widget);

	// Connect side widget signals
	connect(_control_tab->pbRefresh, SIGNAL(clicked(bool)), this, SLOT(refreshCategories()));
	connect(_control_tab->pbRefresh, SIGNAL(clicked(bool)), _control_tab->cbMainCat, SLOT(clear()));
	connect(_control_tab->pbAddCategory, SIGNAL(clicked(bool)), this, SLOT(newCategory()));
	connect(_control_tab->pbRefresh, SIGNAL(clicked(bool)), _control_tab->lwOtherCats, SLOT(clear()));
	connect(_control_tab->leTitle, SIGNAL(editingFinished()), this, SLOT(changeCaptionAfterTitleChanged()));
	connect(_control_tab->leAddTag, SIGNAL(returnPressed()), this, SLOT(addTechTagFromAddButton()));
	connect(_control_tab->tbAddTag, SIGNAL(clicked()), this, SLOT(addTechTagFromLineEdit()));
	connect(_control_tab->leTBPingURL, SIGNAL(returnPressed()), this, SLOT(addTBPingFromLineEdit()));
	connect(_control_tab->tbTBAdd, SIGNAL(clicked()), this, SLOT(addTBPingFromLineEdit()));
	connect(_control_tab, SIGNAL(currentChanged(int)), this, SLOT(handleSideWidgetPageSwitch(int)));
	connect(_control_tab->tbSelectKeywordTag, SIGNAL(clicked(bool)), this, SLOT(addKeywordTagFromAvailTags()));
	connect(_control_tab->tbRemoveKeywordTag, SIGNAL(clicked(bool)), this, SLOT(removeKeywordTag()));
	connect(_control_tab->pbRefreshKeywordTags, SIGNAL(clicked(bool)), this, SLOT(refreshKeywordTags()));
	connect(_control_tab->chShowPassword, SIGNAL(toggled(bool)), this, SLOT(showPassword(bool)));

	// cw->cbPageSelector->setCurrentIndex(0);
	_control_tab->setCurrentIndex(0);

	_status_widget = statusBar();

	connect(_control_tab->pbCopyURL, SIGNAL(clicked()), this, SLOT(copyURL()));
	_control_tab->copyURLWidget->hide();

	//	_dirty_indicator = new QLabel(this);
	_dirty_indicator->setPixmap(QPixmap(filesave));
	_dirty_indicator->setToolTip("dirtyIndicator");
	statusBar()->addPermanentWidget(_dirty_indicator);

	_control_tab->progressWidget->setVisible(false);
	connect(_control_tab->progressBar, SIGNAL(valueChanged(int)), this, SLOT(hideProgressBarIfMaximum(int)));
#ifdef WIN_QTV
	taskbarButton = new QWinTaskbarButton(this);
	taskbarButton->setWindow(QWindow::fromWinId(effectiveWinId()));
	taskbarProgress = taskbarButton->progress();
	taskbarProgress->setVisible(false);
#endif

	_highlighter = new Highlighter((QTextDocument*)nullptr, this);

	// Set up hash of entry attributes
	accountAttributes["useHTTPS"] = &useHTTPS;
	accountAttributes["categoriesEnabled"] = &categoriesEnabled;
	accountAttributes["postDateTime"] = &postDateTime;
	accountAttributes["allowComments"] = &allowComments;
	accountAttributes["allowTB"] = &allowTB;
	accountAttributes["useWordpressAPI"] = &useWordpressAPI;

	accountStrings["server"] = &server;
	accountStrings["location"] = &location;
	accountStrings["port"] = &port;
	accountStrings["login"] = &login;
	accountStrings["password"] = &password;

	setWindowModified(false);
	_entry_ever_saved = false;
	_clean_save = false;
	loadAutoLinkDictionary();
	connect(this->_editor->_close_button, &FlatToolButton::clicked, [&](bool checked) {
		(void)checked;
		_editor_docker->hide();
		appconfig->editor_show(false);
		//		close_trigger_from_others()(nullptr);
		//		this->close();
	});

	////	moved to HidableTabWidget
	// connect(_splitter, &QSplitter::splitterMoved, [&](int pos, int index){
	//// save sizes
	// (void) pos;
	// (void) index;
	// auto result =
	// _control_panel->inner_rebuild_on_splitter_moved(pos, index);

	// auto sizes = std::get<1>(result);
	////            auto sizes_ = _splitter->sizes();// 50, 50
	////            assert(sizes == sizes_);
	////
	// if(sizes[0] <= 0) sizes[0] =
	// _control_panel->tabBar()->sizeHint().width();
	// if((sizes[1] > width() - sizes[0]) || (sizes[1] <= 0)) sizes[1]
	// = width() - sizes[0];
	// assert(sizes[1] >= 0);
	// auto msw = _main_stack->width();// 50
	// if(msw < sizes[1]) _main_stack->resize(sizes[1], height());
	////            assert(msw == sizes[1]);
	// auto ew = _editor->width(); // 0
	////            assert(ew == sizes[1]);
	// if(ew < sizes[1]) _editor->resize(sizes[1], height());
	////

	// QSettings qtm_settings(_config_file_name
	// , QSettings::IniFormat);
	// QStringList line_list_;
	// QList<int>  list_;
	// qtm_settings.beginGroup("geometry");
	// line_list_ = (qtm_settings.value("splitter_sizelist"
	// , "100,100")).toString().split(",");
	// for(int i = 0; i < line_list_.size(); ++i)
	// list_.append(line_list_.at(i).toInt());
	// if(sizes != list_){
	// QStringList line_list;
	// for(int i = 0; i < sizes.size(); ++i)
	// line_list.append(QString::number(sizes.at(i)));
	// qtm_settings.setValue("splitter_sizelist"
	// , line_list.join(","));
	// }
	// qtm_settings.endGroup();
	// });
}

//// QAction *
// FlatToolButton *EditingWindow::new_action(QWidget *parent, QString title,
// QString
// toolTip, QString whatsThis, QObject *receiver, const char *slot){
////    QAction *
// FlatToolButton *return_value = new FlatToolButton(title, parent); // new
// QAction(title, parent);   // parent = this by default
// return_value->setToolTip(toolTip);
// return_value->setWhatsThis(whatsThis);
// if(receiver) connect(return_value, SIGNAL(triggered(bool)), receiver, slot);
// return return_value;
// }

bool Blogger::handleArguments()
{
	bool rv = true;
	int i;
	Blogger* create_ = sapp_t::instance()->blogger(); //nullptr;
	Blogger* d = nullptr;
	QStringList failedFiles;
	QStringList args = QApplication::arguments();
	if (args.size() > 1) {
		for (i = 1; i < args.size(); i++) {
			if (create_) // if there is a current new window
				d = create_;

#ifdef Q_OS_MAC
// setNoStatusBar( c );
#endif
			QString argument = args.at(i);
			if (!(argument.at(0) == '-' && argument.at(1) == '-')) {
				create_ = new Blogger();
				if (create_->load(argument)) {
#ifdef USE_SYSTRAYICON
					create_->setSTI(sti);
#endif
					if (d) // if there's an old window
						positionWidget(create_, d);
					create_->show();
					rv = false;
				} else {
					failedFiles.append(args.at(i));
				}
			}
			if (failedFiles.size()) {
				if (failedFiles.size() < args.size() - 1) {
					QMessageBox::information(this, tr("Error"), tr("Could not load the following:\n\n%1").arg(failedFiles.join("\n")), QMessageBox::Ok);
					rv = false;
				} else {
					if (QMessageBox::question(0, tr("Error"), tr("Could not load the "
										     "following:\n\n%1")
										      .arg(failedFiles.join("\n")),
						tr("&Open blank window"), tr("E&xit"), 0))
						QApplication::exit();
					else
						rv = false;
				}
			}
		}
	}
	return rv;
}

void Blogger::positionWidget(QWidget* w, QWidget* refWidget)
{
	QDesktopWidget* dw = QApplication::desktop();
	QRect r = dw->availableGeometry();
	if (qobject_cast<QWidget*>(refWidget)) {
		QRect g = refWidget->geometry(); // of the reference widget
		if ((g.right() + 30) >= (r.right() + 30) &&
		    (g.bottom() + 30) >= (r.bottom() + 30))
			w->move(r.left(), r.top());
		else
			w->move(refWidget->x() + 30, refWidget->y() + 30);
	}
}

TEXTEDIT* Blogger::editor()
{
	return _editor;
}

void Blogger::changeCaptionAfterTitleChanged()
{
	if (_control_tab->leTitle->text().isEmpty())
		setWindowTitle(tr((program_title_string + " - new entry [*]").c_str()));
	else
		setWindowTitle(QString(("%1 - " + program_title_string + " [*]").c_str()).arg(_control_tab->leTitle->text().trimmed()));
}

void Blogger::closeEvent(QCloseEvent* event)
{
// QSettings _topic_editor_config(_current_topic_config_name,
// QSettings::IniFormat);

#ifndef NO_DEBUG_OUTPUT
// qDebug( "close event" );
#endif
	if (isWindowModified()) {
		if (!saveCheck()) {
			event->ignore();
			sapp_t::instance()->setQuitOnLastWindowClosed(false);
		} else {
			_topic_editor_config->setValue("account/lastAccountId", currentAccountId);
			event->accept();
		}
	} else {
		writeSettings();
		_topic_editor_config->setValue("account/lastAccountId", currentAccountId);
		event->accept();
	}
}

void Blogger::showEvent(QShowEvent* event)
{
	// If the document is empty, the window unedited and the entry never saved,
	// chances are it's new
	if (_editor->document()->isEmpty() && !_dirty_indicator->isVisible() &&
	    !event->spontaneous() && !_entry_ever_saved) _control_tab->leTitle->setFocus(Qt::ActiveWindowFocusReason);
	_destroy_request_sent = false;
	super::showEvent(event);
}

void Blogger::resizeEvent(QResizeEvent* event)
{
	writeSettings();

	super::resizeEvent(event);
}

void Blogger::doQuit()
{
#ifndef NO_DEBUG_OUTPUT
	int i = QApplication::topLevelWidgets().size();
	qDebug() << i << " top level widgets";
#endif

	sapp_t::instance()->setQuitOnLastWindowClosed(true);
	sapp_t::instance()->closeAllWindows();
}

void Blogger::checkForEmptySettings()
{
	// Check if this is a brand new user
	if (gl_paras->editors_shared_full_path_name().isEmpty() || server.isEmpty()) {
		if (QMessageBox::question(
			0, tr("Welcome to QTM"), tr("You do not have any preferences set, and QTM "
						    "needs to know where to find your blog, and where "
						    "to store your data.\n\n"
						    "Set these preferences now?"),
			QMessageBox::Yes | QMessageBox::Default, QMessageBox::No) == QMessageBox::Yes) {
			getPreferences(tr("Stage 1 of 2: Preferences"));
			getAccounts(tr("Stage 2 of 2: Accounts"));
		}
	}
}

void Blogger::setEditorColors()
{
	QPalette widgetPalette;
	QTextCharFormat tagFormat, entityFormat, commentFormat, linkFormat;

	// Set colours
	widgetPalette = _editor->palette();
	widgetPalette.setColor(QPalette::Base, editorBgColor);
	widgetPalette.setColor(QPalette::Text, editorFgColor);
	_editor->setPalette(widgetPalette);

	widgetPalette = _preview_window->palette();
	widgetPalette.setColor(QPalette::Base, previewBgColor);
	widgetPalette.setColor(QPalette::Text, previewFgColor);
	_preview_window->setPalette(widgetPalette);

	widgetPalette = _console->palette();
	widgetPalette.setColor(QPalette::Base, consoleBgColor);
	widgetPalette.setColor(QPalette::Text, consoleFgColor);
	_console->setPalette(widgetPalette);

	// setDirtySignals( false );
	// Set HTML tag colours
	tagFormat.setForeground(QColor(tagFgColor));
	tagFormat.setBackground(QColor(tagBgColor));
	tagFormat.setFontWeight(tagBold ? QFont::Bold : QFont::Normal);
	tagFormat.setFontItalic(tagItalic);
	_highlighter->setFormatFor(Highlighter::Tag, tagFormat);

	// Set entity colours
	entityFormat.setForeground(QColor(entityFgColor));
	entityFormat.setBackground(QColor(entityBgColor));
	entityFormat.setFontWeight(entityBold ? QFont::Bold : QFont::Normal);
	entityFormat.setFontItalic(entityItalic);
	_highlighter->setFormatFor(Highlighter::Entity, entityFormat);

	// Set comment colours
	commentFormat.setForeground(QColor(commentFgColor));
	commentFormat.setBackground(QColor(commentBgColor));
	commentFormat.setFontWeight(commentBold ? QFont::Bold : QFont::Normal);
	commentFormat.setFontItalic(commentItalic);
	_highlighter->setFormatFor(Highlighter::Comment, commentFormat);

	// setDirtySignals( true );
}

void Blogger::setTextFonts()
{
	QFont f, g, h;
	if (editorFontString != "") {
		f.fromString(editorFontString);
		_editor->setFont(f);
	} else {
		f = _editor->font();
		editorFontString = f.toString();
	}
	if (previewFontString != "") {
		g.fromString(previewFontString);
		_preview_window->setFont(g);
	} else {
		g = _preview_window->font();
		previewFontString = g.toString();
	}
	if (consoleFontString != "") {
		h.fromString(consoleFontString);
		_console->setFont(h);
	} else {
		h = _console->font();
		consoleFontString = h.toString();
	}
	// Enable syntax highlighting, if applicable
	if (enableHighlighting) {
		// setDirtySignals( false );
		if (_super_menu) _super_menu->setHighlightingChecked(true);
		_highlighter->setDocument(_editor->document());
		// setDirtySignals( true );
	} else if (_super_menu)
		_super_menu->setHighlightingChecked(false);
}

void Blogger::setInitialAccount()
{
	int i;

	// qDebug() << "server is empty";
	// QSettings _topic_editor_config(_current_topic_config_name,
	// QSettings::IniFormat);
	lastAccountID =
	    _topic_editor_config->value("account/lastAccountID", "").toString();
	QDomNodeList accountsList =
	    _accounts_dom.documentElement().elementsByTagName("account");
	QDomElement thisTitleElem;
	_control_tab->cbAccountSelector->clear();
	if (accountsList.count() == 1 &&
	    accountsList.at(0).firstChildElement("details").isNull()) {
		// This is if there is just one legacy account
		readServerSettings();
		QDomElement detailElem, titleElem, serverElem, portElem, locElem, loginElem, pwdElem;
		detailElem = _accounts_dom.createElement("details");
		titleElem = _accounts_dom.createElement("title");
		titleElem.appendChild(_accounts_dom.createTextNode(tr("Default account")));
		serverElem = _accounts_dom.createElement("server");
		serverElem.appendChild(_accounts_dom.createTextNode(server));
		locElem = _accounts_dom.createElement("location");
		locElem.appendChild(_accounts_dom.createTextNode(location));
		if (!port.isEmpty()) {
			portElem = _accounts_dom.createElement("port");
			portElem.appendChild(_accounts_dom.createTextNode(port));
		}
		loginElem = _accounts_dom.createElement("login");
		loginElem.appendChild(_accounts_dom.createTextNode(login));
		pwdElem = _accounts_dom.createElement("password");
		pwdElem.appendChild(_accounts_dom.createTextNode(password));
		detailElem.appendChild(titleElem);
		detailElem.appendChild(serverElem);
		if (!portElem.isNull()) detailElem.appendChild(portElem);
		detailElem.appendChild(locElem);
		detailElem.appendChild(loginElem);
		detailElem.appendChild(pwdElem);
		accountsList.at(0).insertBefore(detailElem, QDomNode()); // i.e. insert it at the start
		accountsList.at(0).toElement().setAttribute("id", "default");
	}
	for (i = 0; i < accountsList.count(); i++) {
		thisTitleElem = accountsList.at(i)
				    .toElement()
				    .firstChildElement("details")
				    .firstChildElement("title");
		if (!thisTitleElem.isNull())
			_control_tab->cbAccountSelector->addItem(
			    decodeXmlEntities(thisTitleElem.text()), accountsList.at(i).toElement().attribute("id"));
		else
			_control_tab->cbAccountSelector->addItem(
			    tr("Unnamed account"), accountsList.at(i).toElement().attribute("id").isEmpty() ? QString("noid_%1").arg(i) : accountsList.at(i).toElement().attribute("id"));
	}
	// Check if the last account is in the accounts list, and if so, make it
	// current
	for (i = 0; i < accountsList.count(); i++) {
		if (accountsList.at(i).toElement().attribute("id") == lastAccountID) {
			// qDebug() << "found it";
			currentAccountElement = accountsList.at(i).toElement();
			currentAccountId = currentAccountElement.attribute("id");
			_control_tab->cbAccountSelector->setCurrentIndex(i);
			extractAccountDetails();
			populateBlogList();
			setNetworkActionsEnabled(true);

			disconnect(_control_tab->cbAccountSelector, SIGNAL(activated(int)), this, SLOT(changeAccount(int)));
			connect(_control_tab->cbAccountSelector, SIGNAL(activated(int)), this, SLOT(changeAccount(int)));
			break;
		}
		// If it reaches the end of the loop with no joy
		if (i == accountsList.count() - 1) {
			// qDebug() << "using first account";
			currentAccountElement =
			    _accounts_dom.documentElement().firstChildElement("account");
			extractAccountDetails();
			populateBlogList();
			setNetworkActionsEnabled(true);
			disconnect(_control_tab->cbAccountSelector, SIGNAL(activated(int)), this, SLOT(changeAccount(int)));
			connect(_control_tab->cbAccountSelector, SIGNAL(activated(int)), this, SLOT(changeAccount(int)));
		}
	}
}

void Blogger::readSettings()
{
	QString crf;
	app::RecentFile current_recent_file;
	QString defaultMarkdownPath, defaultPerlLocation;
	bool defaultUseMarkdown;

#ifdef Q_OS_MAC
	QString mdPathString =
	    QString("%1/../Resources").arg(sapp_t::instance()->applicationDirPath());
	QDir mdPath(mdPathString);
	defaultMarkdownPath = QString("%1/Markdown.pl").arg(mdPath.canonicalPath());

	defaultUseMarkdown = true;
	defaultPerlLocation = "/usr/bin/perl";
#else
#ifdef Q_OS_WIN32
	defaultMarkdownPath =
	    QString("%1/Markdown.pl")
		.arg(QString(MARKDOWN_LOCATION).replace("%APPDIRPATH%", sapp_t::instance()->applicationDirPath()));
	defaultUseMarkdown = false;
	defaultMarkdownPath = PROPERSEPS(defaultMarkdownPath);
	defaultPerlLocation = "C:\\Perl\\bin\\perl.exe";
#else
	// Presumably we're in X11
	defaultMarkdownPath = MARKDOWN_LOCATION;
	defaultUseMarkdown = true;
	defaultPerlLocation = "/usr/bin/perl";
#endif
#endif

	// #ifdef Q_OS_WIN32
	// QString _local_storage_directory = QString(qtm->isSandbox() ?
	// "%1\\QTMSandbox"
	// : "%1\\QTM blog").arg(globalparameters.root_path() + "/" +
	// globalparameters.target_os());
	// #else
	// QString _local_storage_directory = QString(_app->isSandbox() ?
	// "%1/qtm-sandbox" : "%1/qtm-blog").arg(globalparameters.root_path() + "/" +
	// globalparameters.target_os());
	// #endif

	// QSettings _topic_editor_config(_current_topic_config_name,
	// QSettings::IniFormat);
	applicationVersion =
	    _topic_editor_config->value("application/version", "").toString();

	_topic_editor_config->beginGroup("geometry");
	resize(_topic_editor_config->value("size", QSize(640, 450)).toSize());
	// #endif
	move(_topic_editor_config->value("position", QPoint(20, 20)).toPoint());
	{
		QStringList line_list;
		QList<int> list;

		line_list = (_topic_editor_config->value("splitter_sizelist", "100,100"))
				.toString()
				.split(",");
		for (int i = 0; i < line_list.size(); ++i) list.append(line_list.at(i).toInt());
		_splitter->setSizes(list);
	}
	qDebug() << "Done resize";

	_topic_editor_config->endGroup();

	_topic_editor_config->beginGroup("account");
	// _local_storage_directory = _app->isSandbox() ?
	// QString("%1/sandbox").arg(qtm_settings.value("editors_shared_directory",
	// _local_storage_directory).toString()) :
	// qtm_settings.value("editors_shared_directory",
	// _local_storage_directory).toString();
	if (gl_paras->editors_shared_full_path_name().contains("~/")) {
		gl_paras->editors_shared_full_path_name().replace("~", QDir::homePath());
		_topic_editor_config->setValue("editors_shared_directory", gl_paras->editors_shared_full_path_name());
	}
	_local_storage_file_extension =
	    _topic_editor_config->value("local_storage_file_ext", "cqt").toString();
	useMarkdown =
	    _topic_editor_config->value("useMarkdown", defaultUseMarkdown).toBool();
	perlPath =
	    _topic_editor_config->value("perlPath", defaultPerlLocation).toString();
	markdownPath =
	    _topic_editor_config->value("markdownPath", defaultMarkdownPath)
		.toString();
	doMarkdownWhenPosting =
	    _topic_editor_config->value("doMarkdownWhenPosting", useMarkdown)
		.toBool();
	stripParaTags = _topic_editor_config->value("stripParaTags", false).toBool();
	useHTTPS = _topic_editor_config->value("useHTTPS", true).toBool();
	categoriesEnabled =
	    _topic_editor_config->value("categoriesEnabled", true).toBool();
	useNewWindows = _topic_editor_config->value("useNewWindows", true).toBool();
	savePassword = _topic_editor_config->value("savePassword", false).toBool();
	postAsSave = _topic_editor_config->value("postAsSave", true).toBool();
	allowComments = _topic_editor_config->value("allowComments", true).toBool();
	allowTB = _topic_editor_config->value("allowTB", true).toBool();
	enableHighlighting =
	    _topic_editor_config->value("enableHighlighting", true).toBool();
	suppressReadMore =
	    _topic_editor_config->value("suppressReadMore", true).toBool();
	useUtf8 = _topic_editor_config->value("useUtf8", false).toBool();
#ifdef USE_SYSTRAYICON
	copyTitle = _topic_editor_config->value("copyTitle", true).toBool();
#endif
	allowRegexSearch =
	    _topic_editor_config->value("allowRegexSearch", false).toBool();
	_topic_editor_config->endGroup();
	_topic_editor_config->beginGroup("fonts");
	editorFontString =
	    _topic_editor_config->value("editorFontString", "").toString();
	previewFontString =
	    _topic_editor_config->value("previewFontString", "").toString();
	consoleFontString =
	    _topic_editor_config->value("consoleFontString", "").toString();
	editorBgColor = QColor(
	    _topic_editor_config->value("editorBgColor", "#FFFFFF").toString());
	editorFgColor = QColor(
	    _topic_editor_config->value("editorFgColor", "#000000").toString());
	previewBgColor = QColor(
	    _topic_editor_config->value("previewBgColor", "#FFFFFF").toString());
	previewFgColor = QColor(
	    _topic_editor_config->value("previewFgColor", "#000000").toString());
	consoleBgColor = QColor(
	    _topic_editor_config->value("consoleBgColor", "#FFFFFF").toString());
	consoleFgColor = QColor(
	    _topic_editor_config->value("consoleFgColor", "#000000").toString());
	_topic_editor_config->endGroup();

	_topic_editor_config->beginGroup("highlighting");
	tagBgColor =
	    QColor(_topic_editor_config->value("tagBgColor", "#FFFFFF").toString());
	tagFgColor =
	    QColor(_topic_editor_config->value("tagFgColor", "#008000").toString());
	entityBgColor = QColor(
	    _topic_editor_config->value("entityBgColor", "#FFFFFF").toString());
	entityFgColor = QColor(
	    _topic_editor_config->value("entityFgColor", "#000080").toString());
	commentBgColor = QColor(
	    _topic_editor_config->value("commentBgColor", "#FFFFFF").toString());
	commentFgColor = QColor(
	    _topic_editor_config->value("commentFgColor", "#008080").toString());
	linkBgColor =
	    QColor(_topic_editor_config->value("linkBgColor", "#ffffff").toString());
	linkFgColor =
	    QColor(_topic_editor_config->value("linkFgColor", "#0000ff").toString());
	tagBold = _topic_editor_config->value("tagBold", false).toBool();
	tagItalic = _topic_editor_config->value("tagItalic", false).toBool();
	entityBold = _topic_editor_config->value("entityBold", false).toBool();
	entityItalic = _topic_editor_config->value("entityItalic", false).toBool();
	commentBold = _topic_editor_config->value("commentBold", false).toBool();
	commentItalic = _topic_editor_config->value("commentItalic", false).toBool();
	linkBold = _topic_editor_config->value("linkBold", false).toBool();
	linkItalic = _topic_editor_config->value("linkItalic", false).toBool();
	linkUnderline = _topic_editor_config->value("linkUnderline", true).toBool();
	_topic_editor_config->endGroup();

#ifdef USE_SYSTRAYICON
	_topic_editor_config->beginGroup("sysTrayIcon");
	STI2ClickFunction =
	    _topic_editor_config->value("doubleClickFunction", 0).toInt();
	_topic_editor_config->endGroup();
#endif
	if (server.isEmpty() || location.isEmpty() || login.isEmpty()) {
		// Adequate network details absent, therefore disable all widgets and
		// actions
		// leading to network use.
		setNetworkActionsEnabled(false);
	}
	_control_tab->_categories->setEnabled(true);
	_control_tab->chAllowComments->setEnabled(true);
	_control_tab->chAllowTB->setEnabled(true);
	_search_widget->setExpertEnabled(allowRegexSearch);
}

void Blogger::readServerSettings()
{
	// qDebug() << "getting server settings";
	// QSettings _topic_editor_config(_current_topic_config_name,
	// QSettings::IniFormat);
	_topic_editor_config->beginGroup("account");
	server = _topic_editor_config->value("server", "").toString();
	location = _topic_editor_config->value("location", "").toString();
	login = _topic_editor_config->value("login", "").toString();
	password = _topic_editor_config->value("password", "").toString();
}

void Blogger::handleEnableCategories()
{
	_control_tab->gbCategory->setEnabled(categoriesEnabled);
	if (_super_menu) _super_menu->setCategoriesEnabled(categoriesEnabled);
}

void Blogger::openRecentFile()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action) choose(action->data().toString());
}

void Blogger::writeSettings()
{
	// QSettings _topic_editor_config(_current_topic_config_name,
	// QSettings::IniFormat);

	_topic_editor_config->beginGroup("geometry");
	_topic_editor_config->setValue("size", size());
	_topic_editor_config->setValue("position", pos());

	_topic_editor_config->endGroup();
}

void Blogger::callRefreshCategories()
{
	qDebug() << "Starting refresh";
	if (!_current_http_business) {
		_control_tab->cbMainCat->clear();
		_control_tab->lwOtherCats->clear();
		refreshCategories();
	} else {
#ifdef QTM_DEBUG
		if (currentHttpBusiness != _mt_getCategoryList) {
			statusWidget->showMessage(tr("changeBlog: All HTTP requests are blocked"), 2000);
			addToConsole(QString("changeBlog %1 failed - HTTP job of type %2 "
					     "ongoing")
					 .arg(b)
					 .arg((int)currentHttpBusiness));
		}
#else
		if (_current_http_business != _mt_getCategoryList) _status_widget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
	}
#ifndef NO_DEBUG_OUTPUT
// qDebug() << "Finishing changeblog";
#endif
}

void Blogger::refreshCategories()
{
	QDomElement param, value, integer, string;

	disconnect(SIGNAL(httpBusinessFinished()));
	if (!_current_http_business) {
		_control_tab->cbMainCat->clear();
		_control_tab->lwOtherCats->clear();
		qDebug() << "Cleared category lists";
		qDebug() << "Cleared category lists";

		QDomDocument doc;
		QDomElement methodCall = doc.createElement("methodCall");
		methodCall.appendChild(XmlMethodName(doc, "mt.getCategoryList"));

		QDomElement params = doc.createElement("params");
		params.appendChild(XmlValue(doc, "string", currentBlogid));
		params.appendChild(XmlValue(doc, "string", currentAccountElement.firstChildElement("details").firstChildElement("login").text()));
		params.appendChild(XmlValue(doc, "string", currentAccountElement.firstChildElement("details").firstChildElement("password").text()));

		methodCall.appendChild(params);
		doc.appendChild(methodCall);
		doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());
		QByteArray requestArray(doc.toByteArray(2));
		responseData = "";

		placeNetworkRequest(_mt_getCategoryList, requestArray);
	} else {
#ifdef QTM_DEBUG
		statusWidget->showMessage(
		    tr("refreshCategories: All HTTP requests are blocked"), 2000);
#else
		_status_widget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
	}
}

QString Blogger::processWithMarkdown(const QString& text)
{
	QString conversionString = text;
	QString nullString, conversionStringB, tempFileName, mdPath;
	QTemporaryFile tf;
	if (tf.open()) {
		tempFileName = tf.fileName();
		QTextStream tstream(&tf);
		tstream.setCodec("UTF-8");
		tstream << conversionString;
		tf.close();
	} else
		return QString();
	QStringList markdownPaths;
	markdownPaths << markdownPath
		      << QString("%1/Markdown/Markdown.pl")
			     .arg(QCoreApplication::applicationDirPath())
		      << QString("%1/../Markdown/Markdown.pl")
			     .arg(QCoreApplication::applicationDirPath())
		      << "nofile";
	Q_FOREACH (QString mdp, markdownPaths) {
		if (mdp == "nofile")
			return QString();
		else if (QFile::exists(mdp)) {
			mdPath = mdp;
			break;
		}
	}
	QProcess proc;
	proc.start(perlPath, QStringList() << mdPath << tempFileName);
	if (!proc.waitForStarted()) {
		_status_widget->showMessage(tr("Failed to start conversion"), 2000);
		return QString();
	}
	// Now wait until the process finishes; use a loop and process events every
	// 10th of
	// a second
	for (int i = 0; i <= 300; ++i) {
		if (i == 300) { // if 30 seconds has elapsed without a finish signal
			_status_widget->showMessage(tr("Failed to convert"), 2000);
			return QString();
		}
		if (proc.waitForFinished(100))
			break;
		else
			sapp_t::instance()->processEvents();
	}
	conversionStringB = QString::fromUtf8(proc.readAllStandardOutput());
	if (conversionStringB.length() < conversionString.length() ||
	    proc.exitStatus() != QProcess::NormalExit) {
		_status_widget->showMessage(tr("Conversion failed"), 2000);
		if (proc.exitStatus() != QProcess::NormalExit) qDebug() << "Bad exit";
		// qDebug() << "Length:" << conversionStringB.length();
		return QString();
	}
	return conversionStringB;
}
void Blogger::getAccounts()
{
	getAccounts(QString());
}

void Blogger::getAccounts(const QString& title)
{
	QList<AccountsDialog::Account> acctsList, returnedAccountsList;
	AccountsDialog::Account acct;
	QDomNodeList accounts_list, accounts_attributes;
	QDomDocument newAccountsDom;
	QDomElement newQTMAccounts, newAccount, detailElement, nameElement, serverElement, locationElement, portElement, loginElement, pwdElement, blogsElement, boolElement, attribsElement, hbtElement;
	QString oldCurrentAccountId, oldBlogid, currentTitle, currentPort;
	QStringList thisAccountsAttribStrings;
	QUrl thisUrl;
	int i, j;
	bool ok;

	purgeBlankAccounts();

	// Extract accounts list from account tree
	accounts_list = _accounts_dom.elementsByTagName("account");
	for (i = 0; i < accounts_list.count(); i++) {
		accounts_attributes = accounts_list.at(i)
					  .toElement()
					  .firstChildElement("details")
					  .firstChildElement("attributes")
					  .elementsByTagName("attribute");
		thisAccountsAttribStrings = QStringList();
		for (j = 0; j < accounts_attributes.count(); j++) thisAccountsAttribStrings
		    << accounts_attributes.at(j).toElement().attribute("name");
		acct = AccountsDialog::Account();
		acct.id = accounts_list.at(i).toElement().attribute("id");
		detailElement = accounts_list.at(i).firstChildElement("details");
		currentPort = detailElement.firstChildElement("port").text();

		acct.name = detailElement.firstChildElement("title").text();
		acct.endpoint =
		    QString("%1://%2%3/%4")
			.arg(thisAccountsAttribStrings.contains("useHTTPS") ? "https" : "http")
			.arg(detailElement.firstChildElement("server").text())
			.arg(currentPort.isEmpty() ? "" : QString(":%1").arg(currentPort))
			.arg(detailElement.firstChildElement("location").text().remove(QRegExp("^/")));
		acct.login = detailElement.firstChildElement("login").text();
		acct.password = detailElement.firstChildElement("password").text();

		acct.hostedBlogType = 0;
		int hbt =
		    detailElement.firstChildElement("hostedBlogType").text().toInt(&ok);
		if (ok) acct.hostedBlogType = hbt;
		acct.categoriesEnabled = false;
		acct.postDateTime = false;
		acct.allowComments = false;
		acct.allowTB = false;
		acct.useWordpressAPI = false;
		if (thisAccountsAttribStrings.contains("categoriesEnabled")) acct.categoriesEnabled = true;
		if (thisAccountsAttribStrings.contains("postDateTime")) acct.postDateTime = true;
		if (thisAccountsAttribStrings.contains("allowComments") ||
		    thisAccountsAttribStrings.contains("comments")) acct.allowComments = true;
		if (thisAccountsAttribStrings.contains("allowTB") ||
		    thisAccountsAttribStrings.contains("trackback")) acct.allowTB = true;
		if (thisAccountsAttribStrings.contains("useWordpressAPI")) acct.useWordpressAPI = true;
		acctsList.append(acct);
	}
	oldCurrentAccountId =
	    _control_tab->cbAccountSelector
		->itemData(_control_tab->cbAccountSelector->currentIndex())
		.toString();
	oldBlogid = _control_tab->cbBlogSelector
			->itemData(_control_tab->cbBlogSelector->currentIndex())
			.toString();
	int oldCurrentBlog = _control_tab->cbAccountSelector->currentIndex();
	// accountsList = accountsDom.documentElement().elementsByTagName( "account"
	// );

	AccountsDialog accounts_dialog(acctsList, oldCurrentBlog, this);
	if (!title.isEmpty()) accounts_dialog.setWindowTitle(title);
	if (accounts_dialog.exec()) {
		returnedAccountsList = accounts_dialog.accounts();
		newQTMAccounts =
		    newAccountsDom.createElement(program_title_qstring + "Accounts");
		for (i = 0; i < returnedAccountsList.count(); i++) {
			newAccount = newAccountsDom.createElement("account");
			newAccount.setAttribute("id", returnedAccountsList.at(i).id);
			detailElement = newAccountsDom.createElement("details");
			nameElement = newAccountsDom.createElement("title");
			nameElement.appendChild(
			    newAccountsDom.createTextNode(returnedAccountsList.at(i).name));
			hbtElement = newAccountsDom.createElement("hostedBlogType");
			hbtElement.appendChild(newAccountsDom.createTextNode(QString::number(returnedAccountsList.at(i).hostedBlogType)));

			thisUrl.setUrl(returnedAccountsList.at(i).endpoint);
			serverElement = newAccountsDom.createElement("server");
			serverElement.appendChild(newAccountsDom.createTextNode(thisUrl.host()));
			locationElement = newAccountsDom.createElement("location");
			locationElement.appendChild(
			    newAccountsDom.createTextNode(thisUrl.path()));
			portElement = newAccountsDom.createElement("port");
			portElement.appendChild(newAccountsDom.createTextNode(thisUrl.port() == -1 ? "" : QString::number(thisUrl.port())));
			loginElement = newAccountsDom.createElement("login");
			loginElement.appendChild(
			    newAccountsDom.createTextNode(returnedAccountsList.at(i).login));
			pwdElement = newAccountsDom.createElement("password");
			pwdElement.appendChild(
			    newAccountsDom.createTextNode(returnedAccountsList.at(i).password));

			detailElement.appendChild(nameElement);
			detailElement.appendChild(hbtElement);
			detailElement.appendChild(serverElement);
			detailElement.appendChild(locationElement);
			detailElement.appendChild(portElement);
			detailElement.appendChild(loginElement);
			detailElement.appendChild(pwdElement);
			if (returnedAccountsList.at(i).categoriesEnabled ||
			    returnedAccountsList.at(i).postDateTime ||
			    returnedAccountsList.at(i).allowComments ||
			    returnedAccountsList.at(i).allowTB) attribsElement = newAccountsDom.createElement("attributes");
			if (thisUrl.scheme() == "https") {
				boolElement = newAccountsDom.createElement("attribute");
				boolElement.setAttribute("name", "useHTTPS");
				attribsElement.appendChild(boolElement);
			}
			if (returnedAccountsList.at(i).categoriesEnabled) {
				boolElement = newAccountsDom.createElement("attribute");
				boolElement.setAttribute("name", "categoriesEnabled");
				attribsElement.appendChild(boolElement);
			}
			if (returnedAccountsList.at(i).postDateTime) {
				boolElement = newAccountsDom.createElement("attribute");
				boolElement.setAttribute("name", "postDateTime");
				attribsElement.appendChild(boolElement);
			}
			if (returnedAccountsList.at(i).allowComments) {
				// qDebug() << "comments attribute set";
				boolElement = newAccountsDom.createElement("attribute");
				boolElement.setAttribute("name", "allowComments");
				attribsElement.appendChild(boolElement);
			}
			if (returnedAccountsList.at(i).allowTB) {
				// qDebug() << "TB attribute set";
				boolElement = newAccountsDom.createElement("attribute");
				boolElement.setAttribute("name", "allowTB");
				attribsElement.appendChild(boolElement);
			}
			if (returnedAccountsList.at(i).useWordpressAPI) {
				boolElement = newAccountsDom.createElement("attribute");
				boolElement.setAttribute("name", "useWordpressAPI");
				attribsElement.appendChild(boolElement);
			}
			if (!attribsElement.isNull()) detailElement.appendChild(attribsElement);
			newAccount.appendChild(detailElement);
			// Check if each account is matched from the old list; if it is,
			// copy the blogs list
			for (j = 0; j < accounts_list.count(); j++) {
				if (accounts_list.at(j).toElement().attribute("id") ==
				    returnedAccountsList.at(i).id) {
					blogsElement =
					    accounts_list.at(j).toElement().firstChildElement("blogs");
					if (!blogsElement.isNull()) newAccount.appendChild(
					    newAccountsDom.importNode(blogsElement, true));
					break;
				}
			}
			newQTMAccounts.appendChild(newAccount);
		}
		// Now add processing instruction and the whole QTMAccounts element, and
		// replace the old main
		// accounts tree with this one
		newAccountsDom.appendChild(
		    newAccountsDom.createProcessingInstruction("xml", "version='1.0'"));
		newAccountsDom.appendChild(newQTMAccounts);
		_accounts_dom = newAccountsDom.cloneNode(true).toDocument();

		_control_tab->cbAccountSelector->clear();
		accounts_list =
		    _accounts_dom.documentElement().elementsByTagName("account");
		for (i = 0; i < accounts_list.count(); ++i) {
			currentTitle = decodeXmlEntities(accounts_list.at(i).firstChildElement("details").firstChildElement("title").text());
			if (currentTitle.isEmpty()) currentTitle = tr("(Unnamed account)");
			_control_tab->cbAccountSelector->addItem(
			    currentTitle, accounts_list.at(i).toElement().attribute("id"));
		}
		// Check if the old current account is in this list; if so, make it current
		// again
		for (i = 0; i < accounts_list.count(); ++i) {
			if (accounts_list.at(i).toElement().attribute("id") ==
			    oldCurrentAccountId) {
				_control_tab->cbAccountSelector->setCurrentIndex(i);
				currentAccountElement = accounts_list.at(i).toElement();
				extractAccountDetails();

				QStringList accountStringNames(accountStrings.keys());
				Q_FOREACH (QString s, accountStringNames) {
					*(accountStrings[s]) =
					    currentAccountElement.firstChildElement("details")
						.firstChildElement(s)
						.text();
				}
				// Now check if the current account has any blogs
				if (!currentAccountElement.firstChildElement("blogs").isNull()) {
					// qDebug() << "blogs found";
					populateBlogList();
				} else {
					// qDebug() << "no blogs found";
					refreshBlogList();
				}
				break;
			}
			if (i == accounts_list.count() - 1) {
				// qDebug() << "reached end of account list";
				_control_tab->cbAccountSelector->setCurrentIndex(0);
				currentAccountElement = accounts_list.at(0).toElement();
				currentAccountId = currentAccountElement.attribute("id");
				extractAccountDetails();
				refreshBlogList();
				setNetworkActionsEnabled(true);
			}
		}
		disconnect(_control_tab->cbAccountSelector, SIGNAL(activated(int)), this, SLOT(changeAccount(int)));
		connect(_control_tab->cbAccountSelector, SIGNAL(activated(int)), this, SLOT(changeAccount(int)));
		saveAccountsDom();
	}
}

void Blogger::getPreferences()
{
	getPreferences(QString());
}

void Blogger::getPreferences(const QString& title)
{
	// QSettings _topic_editor_config(_current_topic_config_name,
	// QSettings::IniFormat);
	QPalette palette, widget_palette;

	PrefsDialog prefs_dialog(this);
	assert(!gl_paras->editors_shared_full_path_name().isEmpty());
	//	if(gl_paras->editors_shared_full_path_name().isEmpty()){
	//#ifdef Q_OS_WIN32
	//		QString lsd = QString(qtm->isSandbox() ? "%1\\QTMsandbox" : "%1\\QTM
	//blog")
	//		              .arg(globalparameters.root_path() + "/" +
	//globalparameters.target_os()) // QDir::homePath()
	//		              .replace("/", "\\");
	//#else
	//		QString lsd = QString(sapp_t::instance()->isSandbox() ?
	//"%1/qtm-sandbox" : "%1/qtm-blog").arg(gl_paras->root_path() + "/" +
	//gl_paras->target_os()); // QDir::homePath()

	//#endif
	//		prefs_dialog.leLocalDir->setText(lsd);
	//	}else

	prefs_dialog.leLocalDir->setText(gl_paras->editors_shared_full_path_name());
	prefs_dialog.leFileExtn->setText(_local_storage_file_extension);
	prefs_dialog.chUseNewWindows->setCheckState(useNewWindows ? Qt::Checked : Qt::Unchecked);
	prefs_dialog.chPostAsSave->setCheckState(postAsSave ? Qt::Checked : Qt::Unchecked);
	prefs_dialog.chAllowComments->setCheckState(allowComments ? Qt::Checked : Qt::Unchecked);
	prefs_dialog.chAllowTB->setCheckState(allowTB ? Qt::Checked : Qt::Unchecked);
#if defined USE_SYSTRAYICON
	prefs_dialog.chCopyTitle->setCheckState(copyTitle ? Qt::Checked : Qt::Unchecked);
#else
	prefsDialog.chCopyTitle->setVisible(false);
#endif
	prefs_dialog.chUseMarkdown->setCheckState(useMarkdown ? Qt::Checked : Qt::Unchecked);
	prefs_dialog.lePerlPath->setText(PROPERSEPS(perlPath));
	prefs_dialog.leMarkdownPath->setText(PROPERSEPS(markdownPath));
	prefs_dialog.chDoMarkdownWhenPosting->setCheckState(
	    doMarkdownWhenPosting ? Qt::Checked : Qt::Unchecked);
	prefs_dialog.chStripParaTags->setCheckState(stripParaTags ? Qt::Checked : Qt::Unchecked);
	prefs_dialog.chStripParaTags->setEnabled(doMarkdownWhenPosting);
	if (!useMarkdown) {
		prefs_dialog.lineBelowFonts->setVisible(false);
		prefs_dialog.gbMarkdown->setVisible(false);
	}
	prefs_dialog.chSuppressReadMore->setCheckState(
	    suppressReadMore ? Qt::Checked : Qt::Unchecked);
	prefs_dialog.chUseUtf8->setCheckState(useUtf8 ? Qt::Checked : Qt::Unchecked);

	prefs_dialog.chAllowRegexSearch->setCheckState(
	    allowRegexSearch ? Qt::Checked : Qt::Unchecked);

#ifdef Q_OS_MAC
	prefsDialog.setWindowFlags(Qt::Sheet);
#endif

	prefs_dialog.tabWidget->setTabText(0, tr("General"));
	prefs_dialog.tabWidget->setTabText(1, tr("Fonts && Markdown"));
	prefs_dialog.tabWidget->setTabText(2, tr("Syntax highlighting"));
	prefs_dialog.tabWidget->setCurrentIndex(0);

	QFont editorFont = _editor->font();
	QFont previewFont = _preview_window->font();
	QFont consoleFont = _console->font();
	prefs_dialog.fcbComposer->setCurrentFont(editorFont);
	prefs_dialog.sbComposer->setValue(editorFont.pointSize());
	prefs_dialog.fcbPreview->setCurrentFont(previewFont);
	prefs_dialog.sbPreview->setValue(previewFont.pointSize());
	prefs_dialog.fcbConsole->setCurrentFont(consoleFont);
	prefs_dialog.sbConsole->setValue(consoleFont.pointSize());

	// Set editor colours
	palette = prefs_dialog.lEditorExampleText->palette();
	palette.setColor(QPalette::Window, editorBgColor);
	palette.setColor(QPalette::WindowText, editorFgColor);
	prefs_dialog.lEditorExampleText->setPalette(palette);

	// Set preview colours
	palette = prefs_dialog.lPreviewExampleText->palette();
	palette.setColor(QPalette::Window, previewBgColor);
	palette.setColor(QPalette::WindowText, previewFgColor);
	prefs_dialog.lPreviewExampleText->setPalette(palette);

	// Set console colours
	palette = prefs_dialog.lConsoleExampleText->palette();
	palette.setColor(QPalette::Window, consoleBgColor);
	palette.setColor(QPalette::WindowText, consoleFgColor);
	prefs_dialog.lConsoleExampleText->setPalette(palette);

	// Set colours for HTML tags
	palette = prefs_dialog.lTagColorDemo->palette();
	palette.setColor(QPalette::Window, tagBgColor);
	palette.setColor(QPalette::WindowText, tagFgColor);
	prefs_dialog.lTagColorDemo->setPalette(palette);
	prefs_dialog.setLabelText(prefs_dialog.lTagColorDemo, tagBold, tagItalic);
	prefs_dialog.chTagBold->setChecked(tagBold);
	prefs_dialog.chTagItalic->setChecked(tagItalic);

	// Set colours for HTML entities
	palette = prefs_dialog.lEntityColorDemo->palette();
	palette.setColor(QPalette::Window, entityBgColor);
	palette.setColor(QPalette::WindowText, entityFgColor);
	prefs_dialog.lEntityColorDemo->setPalette(palette);
	prefs_dialog.setLabelText(prefs_dialog.lEntityColorDemo, entityBold, entityItalic);
	prefs_dialog.chEntityBold->setChecked(entityBold);
	prefs_dialog.chEntityItalic->setChecked(entityItalic);

	// Set colours for comments
	palette = prefs_dialog.lCommentColorDemo->palette();
	palette.setColor(QPalette::Window, commentBgColor);
	palette.setColor(QPalette::WindowText, commentFgColor);
	prefs_dialog.lCommentColorDemo->setPalette(palette);
	prefs_dialog.setLabelText(prefs_dialog.lCommentColorDemo, commentBold, commentItalic);
	prefs_dialog.chCommentBold->setChecked(commentBold);
	prefs_dialog.chCommentItalic->setChecked(commentItalic);

#ifdef USE_SYSTRAYICON
	prefs_dialog.cbSTI2ClickFunction->setCurrentIndex(STI2ClickFunction);
#else
	prefsDialog.cbSTI2ClickFunction->hide();
	prefsDialog.label_10->hide();
#endif

	prefs_dialog.resize(
	    QSize(prefs_dialog.width(), prefs_dialog.minimumHeight()));
	if (!title.isEmpty()) prefs_dialog.setWindowTitle(title);
	// Disable cbStripParaTags as this feature is not properly implemented
	prefs_dialog.chStripParaTags->setVisible(false);
	if (prefs_dialog.exec()) {
#ifndef NO_DEBUG_OUTPUT
// qDebug( "Setting account variables" );
#endif
		gl_paras->editors_shared_full_path_name() =
		    PROPERSEPS(prefs_dialog.leLocalDir->text());
		_local_storage_file_extension = prefs_dialog.leFileExtn->text();
		useNewWindows = prefs_dialog.chUseNewWindows->isChecked();
		postAsSave = prefs_dialog.chPostAsSave->isChecked();
		allowComments = prefs_dialog.chAllowComments->isChecked();
		allowTB = prefs_dialog.chAllowTB->isChecked();
		copyTitle = prefs_dialog.chCopyTitle->isChecked();
		useMarkdown = prefs_dialog.chUseMarkdown->isChecked();
		perlPath = PROPERSEPS(prefs_dialog.lePerlPath->text());
		markdownPath = PROPERSEPS(prefs_dialog.leMarkdownPath->text());
		doMarkdownWhenPosting = prefs_dialog.chDoMarkdownWhenPosting->isChecked();
		stripParaTags = prefs_dialog.chStripParaTags->isChecked();
		allowRegexSearch = prefs_dialog.chAllowRegexSearch->isChecked();
		suppressReadMore = prefs_dialog.chSuppressReadMore->isChecked();
		useUtf8 = prefs_dialog.chUseUtf8->isChecked();

#ifndef NO_DEBUG_OUTPUT
// qDebug( "setting fonts" );
#endif
		QFont ef = prefs_dialog.fcbComposer->currentFont();
		ef.setPointSize(prefs_dialog.sbComposer->value());
		editorFontString = ef.toString();
		_editor->setFont(ef);
		QFont pf = prefs_dialog.fcbPreview->currentFont();
		pf.setPointSize(prefs_dialog.sbPreview->value());
		previewFontString = pf.toString();
		_preview_window->setFont(pf);
		QFont cf = prefs_dialog.fcbConsole->currentFont();
		cf.setPointSize(prefs_dialog.sbConsole->value());
		consoleFontString = cf.toString();
		_console->setFont(cf);

		// Set colours
		palette = prefs_dialog.lEditorExampleText->palette();
		editorBgColor = palette.color(QPalette::Window);
		editorFgColor = palette.color(QPalette::WindowText);

		palette = prefs_dialog.lPreviewExampleText->palette();
		previewBgColor = palette.color(QPalette::Window);
		previewFgColor = palette.color(QPalette::WindowText);

		palette = prefs_dialog.lConsoleExampleText->palette();
		consoleBgColor = palette.color(QPalette::Window);
		consoleFgColor = palette.color(QPalette::WindowText);

		setDirtySignals(false);
		palette = prefs_dialog.lTagColorDemo->palette();
		tagBgColor = palette.color(QPalette::Window);
		tagFgColor = palette.color(QPalette::WindowText);

		palette = prefs_dialog.lEntityColorDemo->palette();
		entityBgColor = palette.color(QPalette::Window);
		entityFgColor = palette.color(QPalette::WindowText);

		palette = prefs_dialog.lCommentColorDemo->palette();
		commentBgColor = palette.color(QPalette::Window);
		commentFgColor = palette.color(QPalette::WindowText);

		tagBold = prefs_dialog.chTagBold->isChecked();
		tagItalic = prefs_dialog.chTagItalic->isChecked();
		entityBold = prefs_dialog.chEntityBold->isChecked();
		entityItalic = prefs_dialog.chEntityItalic->isChecked();
		commentBold = prefs_dialog.chCommentBold->isChecked();
		commentItalic = prefs_dialog.chCommentItalic->isChecked();

		setEditorColors();
		setDirtySignals(true);

#if defined USE_SYSTRAYICON
		STI2ClickFunction = prefs_dialog.cbSTI2ClickFunction->currentIndex();
		if (sti) {
#ifndef NO_DEBUG_OUTPUT
// qDebug( "setting double click function" );
#endif
			sti->setDoubleClickFunction(STI2ClickFunction);
			sti->setCopyTitle(copyTitle);
		}
#endif
		assert(!gl_paras->editors_shared_full_path_name().isEmpty());
		// Handle local directory settings; a default is used if none is specified
		//		if(gl_paras->editors_shared_full_path_name().isEmpty()){
		//#ifdef Q_OS_WIN32
		//			_editors_shared_full_path_name = QString(qtm->isSandbox() ?
		//"%1\\QTMsandbox" : "%1\\QTM blog")
		//			                        .arg(globalparameters.root_path() + "/" +
		//globalparameters.target_os()); // QDir::homePath()
		//#else
		//			_editors_shared_full_path_name =
		//QString(sapp_t::instance()->isSandbox() ? "%1/qtm-sandbox" :
		//"%1/qtm-blog")
		//			                                 .arg(gl_paras->root_path() + "/" +
		//gl_paras->target_os()); // QDir::homePath()
		//#endif
		//		}
		QDir qd(gl_paras->editors_shared_full_path_name());
		if (!qd.exists()) {
			addToConsole(tr("Making directory %1").arg(gl_paras->editors_shared_full_path_name()));
			if (!qd.mkpath(gl_paras->editors_shared_full_path_name())) _status_widget->showMessage(tr("Could not create QTM directory."), 2000);
			if (!_no_auto_save) saveAccountsDom();
		}
		/*setDirtySignals( false );
						setDirtySignals( true ); */
		// Now check to see if this is an old-style Blogger account, which does not
		// support categories.
		if (server.contains("blogger.com")) {
			// Blogger no longer supports this API, so all these features are
			// redundant.
			_status_widget->showMessage(
			    tr("Blogger no longer supports QTM or similar clients."), 2000);
			server = "";
			location = "";
		} else
			_control_tab->_categories->setEnabled(true);
		handleEnableCategories();

		_search_widget->setExpertEnabled(allowRegexSearch);

		_topic_editor_config->setValue("application/version", app_version); // QTM_VERSION
		_topic_editor_config->beginGroup("account");
		_topic_editor_config->setValue(
		    "editors_shared_directory", gl_paras->editors_shared_full_path_name().replace("~", QDir::homePath()));
		_topic_editor_config->setValue("local_storage_file_ext", _local_storage_file_extension);
		_topic_editor_config->setValue("useNewWindows", useNewWindows);
		_topic_editor_config->setValue("postAsSave", postAsSave);
		_topic_editor_config->setValue("allowComments", allowComments);
		_topic_editor_config->setValue("allowTB", allowTB);
#if defined USE_SYSTRAYICON
		_topic_editor_config->setValue("copyTitle", copyTitle);
#endif
		_topic_editor_config->setValue("useMarkdown", useMarkdown);
		_topic_editor_config->setValue("perlPath", perlPath);
		_topic_editor_config->setValue("markdownPath", markdownPath);
		_topic_editor_config->setValue("doMarkdownWhenPosting", doMarkdownWhenPosting);
		_topic_editor_config->setValue("stripParaTags", stripParaTags);
		_topic_editor_config->setValue("allowRegexSearch", allowRegexSearch);
		_topic_editor_config->setValue("enableHighlighting", enableHighlighting);
		_topic_editor_config->setValue("suppressReadMore", suppressReadMore);
		_topic_editor_config->setValue("useUtf8", useUtf8);
		_topic_editor_config->endGroup();

		_topic_editor_config->beginGroup("fonts");
		_topic_editor_config->setValue("editorFontString", editorFontString);
		_topic_editor_config->setValue("previewFontString", previewFontString);
		_topic_editor_config->setValue("consoleFontString", consoleFontString);
		_topic_editor_config->setValue("editorBgColor", editorBgColor.name());
		_topic_editor_config->setValue("editorFgColor", editorFgColor.name());
		_topic_editor_config->setValue("previewBgColor", previewBgColor.name());
		_topic_editor_config->setValue("previewFgColor", previewFgColor.name());
		_topic_editor_config->setValue("consoleBgColor", consoleBgColor.name());
		_topic_editor_config->setValue("consoleFgColor", consoleFgColor.name());
		_topic_editor_config->endGroup();

		_topic_editor_config->beginGroup("highlighting");
		_topic_editor_config->setValue("tagBgColor", tagBgColor.name());
		_topic_editor_config->setValue("tagFgColor", tagFgColor.name());
		_topic_editor_config->setValue("entityBgColor", entityBgColor.name());
		_topic_editor_config->setValue("entityFgColor", entityFgColor.name());
		_topic_editor_config->setValue("commentBgColor", commentBgColor.name());
		_topic_editor_config->setValue("commentFgColor", commentFgColor.name());
		_topic_editor_config->setValue("linkBgColor", linkBgColor.name());
		_topic_editor_config->setValue("linkFgColor", linkFgColor.name());
		_topic_editor_config->setValue("tagBold", tagBold);
		_topic_editor_config->setValue("tagItalic", tagItalic);
		_topic_editor_config->setValue("entityBold", entityBold);
		_topic_editor_config->setValue("entityItalic", entityItalic);
		_topic_editor_config->setValue("commentBold", commentBold);
		_topic_editor_config->setValue("commentItalic", commentItalic);
		_topic_editor_config->setValue("linkBold", linkBold);
		_topic_editor_config->setValue("linkItalic", linkItalic);
		_topic_editor_config->setValue("linkUnderline", linkUnderline);
		_topic_editor_config->endGroup();

#ifdef USE_SYSTRAYICON
		_topic_editor_config->beginGroup("sysTrayIcon");
		_topic_editor_config->setValue("doubleClickFunction", STI2ClickFunction);
		_topic_editor_config->endGroup();
#endif
	}
}
void Blogger::setHighlighting(bool hl)
{
	// QSettings _topic_editor_config(_current_topic_config_name,
	// QSettings::IniFormat);

	enableHighlighting = hl;
	disconnect(_editor->document(), 0, this, SLOT(dirtify()));
	if (enableHighlighting) {
		if (_highlighter->document() == 0) _highlighter->setDocument(_editor->document());
	} else
		_highlighter->setDocument(0);
	_topic_editor_config->setValue("account/enableHighlighting", enableHighlighting);
	QTimer::singleShot(250, this, SLOT(reenableDirty()));
	// connect( editor_object->document(), SIGNAL( contentsChanged() ), this,
	// SLOT(
	// dirtify() ) );
}

void Blogger::reenableDirty()
{
	connect(_editor->document(), SIGNAL(contentsChanged()), this, SLOT(dirtify()));
}

void Blogger::purgeBlankAccounts()
{
	int i, j;

	// First, remove blank account elements
	QDomNode thisNode, parentNode;
	QDomNodeList accountNodes = _accounts_dom.elementsByTagName("account");
	i = accountNodes.count();
	for (j = 0; j < i; j++) {
		thisNode = accountNodes.at(j);
		if (!thisNode.hasChildNodes()) {
			parentNode = thisNode.parentNode();
			parentNode.removeChild(thisNode);
		}
	}
}

void Blogger::populateAccountList()
{ // slot
	int i;
	QDomElement ct, detail;
	QString cid, cname;

	QDomNodeList accountNodeList =
	    _accounts_dom.documentElement().elementsByTagName("account");
	int a = accountNodeList.count();
	if (a) {
		// qDebug () << "populating:" << a << "accounts";
		_control_tab->cbAccountSelector->clear();
		for (i = 0; i < a; i++) {
			cid = accountNodeList.at(i).toElement().attribute("id");
			if (cid.isEmpty()) cid = QString("nameless_%1").arg(i + 1);
			detail = accountNodeList.at(i).firstChildElement("details");
			if (!detail.isNull()) {
				cname = detail.firstChildElement("title").text();
				if (cname.isEmpty()) cname = tr("Unnamed account %1").arg(i + 1);
				_control_tab->cbAccountSelector->addItem(decodeXmlEntities(cname), cid);
			}
		}
	}
}

void Blogger::populateBlogList()
{ // slot
	addToConsole(_accounts_dom.toString(2));

	QDomNodeList blogNodeList =
	    currentAccountElement.firstChildElement("blogs").elementsByTagName(
		"blog");
	QDomNodeList catNodeList, tagNodeList;
	QDomElement catsElement, tagsElement;
	QDomElement ct;
	QString thisTagString;
	int a = blogNodeList.count();
	int uncategorized = 0;
#ifndef NO_DEBUG_OUTPUT
// qDebug() << "Blogs: " << a;
#endif
	int b, i, j;
	QString catName, catId;
	if (a) {
		_control_tab->cbBlogSelector->clear();
		for (i = 0; i < a; i++) {
			ct = blogNodeList.at(i).firstChildElement("blogName");
			_control_tab->cbBlogSelector->addItem(
			    decodeXmlEntities(
				blogNodeList.at(i).firstChildElement("blogName").text()),
			    QVariant(blogNodeList.at(i).firstChildElement("blogid").text()));
			currentBlog = i;
			currentBlogElement = currentAccountElement.firstChildElement("blogs")
						 .elementsByTagName("blog")
						 .at(currentBlog)
						 .toElement();
			currentBlogid = currentBlogElement.firstChildElement("blogid").text();
		}
		currentBlog = _control_tab->cbBlogSelector->currentIndex();

		catsElement = blogNodeList.at(currentBlog).firstChildElement("categories");
		if (!catsElement.isNull()) {
			catNodeList = catsElement.elementsByTagName("category");
			b = catNodeList.count();
			if (b) {
				_control_tab->cbMainCat->clear();
				_control_tab->lwOtherCats->clear();
				for (j = 0; j < b; j++) {
					catName = decodeXmlEntities(catNodeList.at(j).firstChildElement("categoryName").text());
					catId = catNodeList.at(j).firstChildElement("categoryId").text();
					_control_tab->cbMainCat->addItem(catName, QVariant(catNodeList.at(j).firstChildElement("categoryId").text()));
					_control_tab->lwOtherCats->addItem(catName);
					if (catId == "1") uncategorized = j;
				}
				if (uncategorized) _control_tab->cbMainCat->setCurrentIndex(uncategorized);
			} else {
				_control_tab->chNoCats->setEnabled(false);
				_control_tab->cbMainCat->setEnabled(false);
				_control_tab->lwOtherCats->setEnabled(false);
			}
		}
		_control_tab->lwAvailKeywordTags->clear();
		_control_tab->lwKeywordTags->clear();
		populateTagList();
		disconnect(_control_tab->cbBlogSelector, SIGNAL(activated(int)), this, SLOT(changeBlog(int)));
		connect(_control_tab->cbBlogSelector, SIGNAL(activated(int)), this, SLOT(changeBlog(int)));
	} else {
		_control_tab->cbBlogSelector->setEnabled(false);
		_control_tab->chNoCats->setEnabled(false);
		_control_tab->cbMainCat->setEnabled(false);
		_control_tab->lwOtherCats->setEnabled(false);
	}
}

void Blogger::populateTagList()
{ // slot
	if (useWordpressAPI) {
		_control_tab->lwAvailKeywordTags->setVisible(true);
		_control_tab->tbSelectKeywordTag->setVisible(true);
		_control_tab->tbRemoveKeywordTag->setVisible(true);
		_control_tab->pbRefreshKeywordTags->setVisible(true);
		QDomElement tagsElement = currentBlogElement.firstChildElement("wpTags");
		if (!tagsElement.isNull()) {
			QStringList tagList;
			QString thisTagString;
			QDomNodeList tagNodeList = tagsElement.elementsByTagName("tag");
			// qDebug() << "Tag nodes:" << tagNodeList.count();
			for (int j = 0; j < tagNodeList.count(); j++) {
				thisTagString = tagNodeList.at(j).toElement().text();
				if (!tagList.contains(thisTagString) && !thisTagString.isEmpty()) tagList += thisTagString;
			}
			// qDebug() << "Tags:" << tagList.count();
			_control_tab->lwAvailKeywordTags->addItems(tagList);
		}
	} else {
		_control_tab->lwAvailKeywordTags->setVisible(false);
		_control_tab->tbSelectKeywordTag->setVisible(false);
		_control_tab->tbRemoveKeywordTag->setVisible(false);
		_control_tab->pbRefreshKeywordTags->setVisible(false);
	}
}

void Blogger::refreshBlogList()
{ // slot
	QDomDocument doc;
	QDomElement methodCall = doc.createElement("methodCall");
	methodCall.appendChild(XmlMethodName(doc, "blogger.getUsersBlogs"));

	QDomElement params = doc.createElement("params");
	params.appendChild(XmlValue(doc, "string", "xxx"));
	params.appendChild(XmlValue(doc, "string", login));
	params.appendChild(XmlValue(doc, "string", password));
	methodCall.appendChild(params);

	doc.appendChild(methodCall);
	doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

	responseData = "";
	QByteArray requestArray(doc.toByteArray(2));
	placeNetworkRequest(_blogger_getUsersBlogs, requestArray);
}

void Blogger::placeNetworkRequest(HttpBusinessType hbtype, QByteArray& array)
{
	bool ok;

	int p = port.toInt(&ok);
	(void)p;
#ifdef DONT_USE_SSL
	if (!ok) p = ":80";
#else
	if (!ok) p = 0;
#endif
	QUrl url(QString("%1://%2%3%4").arg(useHTTPS ? "https" : "http").arg(server).arg(ok ? QString(":%1").arg(port) : "").arg(location));

	QNetworkRequest request(url);
	request.setRawHeader("User-Agent", userAgentArray);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
	_current_reply = _net_acess_manager->post(request, array);

	addToConsole(request.url().toString());
	addToConsole(QString(array));
	if (QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	_current_http_business = hbtype;
	connect(_net_acess_manager.get(), SIGNAL(finished(QNetworkReply*)), this, SLOT(handleDone(QNetworkReply*)));
#ifndef DONT_USE_SSL
	connect(_current_reply, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(handleSslErrors(const QList<QSslError>&)));
#endif
}

// void EditingWindow::setHost(QHttp *h, QString &serv, QString &pt, bool cm){
// int p;
// bool ok;

// p = pt.toInt(&ok);
// #ifdef DONT_USE_SSL
// if(!ok) p = 80;
// h->setHost(serv, QHttp::ConnectionModeHttp, p);
// #else
// if(!ok) p = 0;
// h->setHost(serv, useHTTPS ? QHttp::ConnectionModeHttps :
// QHttp::ConnectionModeHttp, p);
// #endif
// }

// void EditingWindow::handleResponseHeader(const QHttpResponseHeader
// &header){// slot
// if(header.statusCode() == 404){
// QMessageBox::warning(this, tr("Error 404")
// , tr("There is no web service at the location you\n"
// "specified.  Please change it in the Preferences\n"
// "window."));
// http->disconnect();
// http->abort();
// setNetworkActionsEnabled(false);
// }else responseData.append(http->readAll());
// }

void Blogger::stopThisJob()
{
#ifndef NO_DEBUG_OUTPUT
// qDebug() << "Aborting.";
#endif

	_net_acess_manager->disconnect();
	if (_current_reply) {
		_current_reply->abort();

		_current_http_business = None;
		disconnect(this, SIGNAL(httpBusinessFinished()), 0, 0);
		if (QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
		_current_reply->deleteLater();
		_current_reply = nullptr;
	}
	_status_widget->showMessage(tr("Operation cancelled"), 2000);
}

void Blogger::handleDone(QNetworkReply* reply)
{
	if (reply->error() != QNetworkReply::NoError) {
		if (reply->error() == QNetworkReply::OperationCanceledError)
			_status_widget->showMessage(tr("Operation cancelled."), 2000);
		else
			_status_widget->showMessage(tr("The request failed"), 2000);
		if (QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
	} else {
		responseData = reply->readAll();
		_status_widget->showMessage(
		    QString(tr("The request succeeded; %1 bytes received"))
			.arg(responseData.size()),
		    2000);
		switch (_current_http_business) {
		case _blogger_getUsersBlogs:
			blogger_getUsersBlogs(responseData);
			break;

		case _metaWeblog_newMediaObject:
			metaWeblog_newMediaObject(responseData);
			break;

		case _metaWeblog_newPost:
			metaWeblog_newPost(responseData);
			break;

		case _metaWeblog_editPost:
			metaWeblog_editPost(responseData);
			break;

		case _mt_getCategoryList:
			mt_getCategoryList(responseData);
			break;

		case _mt_publishPost:
			mt_publishPost(responseData);
			break;

		case _mt_setPostCategories:
			mt_setPostCategories(responseData);
			break;

		case _wp_getTags:
			wp_getTags(responseData);
			break;

		case _wp_newCategory:
			wp_newCategory(responseData);
			break;

		case _wp_newPost:
			wp_newPost(responseData);
			break;

		case _wp_editPost:
			wp_editPost(responseData);
			break;

		case None:
			break;
		}
	}
	_net_acess_manager->disconnect();
	reply->disconnect();
	reply->deleteLater();
	_current_reply = nullptr;
	_control_tab->progressBar->setValue(_control_tab->progressBar->value() + 1);
#ifdef WIN_QTV
	taskbarProgress->setValue(taskbarProgress->value() + 1);
#endif
	_current_http_business = None;
	_doing_new_post = false;
	emit httpBusinessFinished();
}

#ifndef DONT_USE_SSL
void Blogger::handleSslErrors(const QList<QSslError>& errorList)
{
	int i = errorList.count();
	if (i) _status_widget->showMessage(tr("%1 SSL errors").arg(i), 2000);
}
#endif

void Blogger::changeCurrentBlog(int b)
{ // slot
	currentBlog = b;
}

void Blogger::changeAccount(int a)
{ // slot
	QString currentBlogText;
	if (currentAccount != a) {
		qDebug() << "clearing categories etc";
		currentAccount = a;
		_control_tab->cbBlogSelector->clear();
		_control_tab->cbMainCat->clear();
		_control_tab->lwOtherCats->clear();
		_control_tab->lwAvailKeywordTags->clear();
		_control_tab->lwKeywordTags->clear();

		currentAccountElement = _accounts_dom.documentElement()
					    .elementsByTagName("account")
					    .at(a)
					    .toElement();
		currentAccountId = currentAccountElement.toElement().attribute("id");
		extractAccountDetails();
		/*qDebug() << "Current account: " <<
						   currentAccountElement.firstChildElement(
		   "details" )
						  .firstChildElement( "title"
		   ).text();
		 */
		QStringList accountStringNames(accountStrings.keys());
		Q_FOREACH (QString s, accountStringNames)
			*(accountStrings[s]) = currentAccountElement.firstChildElement("details")
						   .firstChildElement(s)
						   .text();
		extractAccountAttributes();

		QDomElement blogsElement = currentAccountElement.firstChildElement("blogs");
		if (!blogsElement.isNull()) {
			QDomNodeList blogsList = blogsElement.elementsByTagName("blog");
			int b = blogsList.count();
			if (b) {
				// qDebug() << "Blogs: " << b;
				_control_tab->cbBlogSelector->clear();
				for (int i = 0; i < b; i++)
					_control_tab->cbBlogSelector->addItem(
					    decodeXmlEntities(
						blogsList.at(i).firstChildElement("blogName").text()),
					    blogsList.at(i).firstChildElement("blogid").text());
				_control_tab->cbBlogSelector->setEnabled(true);
				changeBlog(0, true);
				currentBlogid = _control_tab->cbBlogSelector->itemData(0).toString();
				_control_tab->cbBlogSelector->disconnect(
				    this, SLOT(changeBlog(int))); // eliminate duplicate connections
				connect(_control_tab->cbBlogSelector, SIGNAL(activated(int)), this, SLOT(changeBlog(int)));
				emit blogRefreshFinished();
				if (QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
			} else
				refreshBlogList();
		} else
			refreshBlogList();
	}
}

void Blogger::extractAccountDetails()
{ // slot
	QDomElement caDetails =
	    currentAccountElement.firstChildElement("details").toElement();
	server = caDetails.firstChildElement("server").text();
	location = caDetails.firstChildElement("location").text();
	port = caDetails.firstChildElement("port").text();
	login = caDetails.firstChildElement("login").text();
	password = caDetails.firstChildElement("password").text();
	currentAccountId = currentAccountElement.attribute("id");

	extractAccountAttributes();
}

void Blogger::extractAccountAttributes()
{
	QStringList accountAttribNames(accountAttributes.keys());
	QString attribName;
	QDomNodeList attribNodes =
	    currentAccountElement.firstChildElement("details").elementsByTagName(
		"attribute");

	QList<QWidget*> wordpressOnlyWidgets;
	wordpressOnlyWidgets << _control_tab->lPassword << _control_tab->lePassword
			     << _control_tab->wBlankWidget
			     << _control_tab->chShowPassword;
	Q_FOREACH (QString t, accountAttribNames) {
		*(accountAttributes[t]) = false;
		for (int i = 0; i < attribNodes.count(); i++) {
			attribName = attribNodes.at(i).toElement().attribute("name");
			if (attribName == "comments") attribName = "allowComments";
			if (attribName == "trackback") attribName = "allowTB";
			if (attribName == t) *(accountAttributes[t]) = true;
		}
	}
	_control_tab->chAllowComments->setCheckState(allowComments ? Qt::Checked : Qt::Unchecked);
	_control_tab->chAllowTB->setCheckState(allowTB ? Qt::Checked : Qt::Unchecked);
	_control_tab->pbAddCategory->setEnabled(useWordpressAPI);
	// cw->chSticky->setEnabled( useWordpressAPI );
	_control_tab->chSticky->setVisible(false /*useWordpressAPI */);
	Q_FOREACH (QWidget* w, wordpressOnlyWidgets)
		w->setVisible(useWordpressAPI);
	if (_super_menu) _super_menu->addCatAction->setEnabled(useWordpressAPI);
}

void Blogger::changeBlog(int b, bool fromChangeAccount)
{ // slot
	QString currentCategoryText, currentCatId;
	QDomElement tagsElement;
	QDomNodeList tagsList;
	int i;
	int uncategorized = 0;
	// qDebug() << "Starting changeblog" << b;
	if (currentBlog != b || fromChangeAccount) {
		// qDebug() << "changing the blog anyway";
		currentBlog = b;
		_control_tab->cbMainCat->clear();
		_control_tab->lwOtherCats->clear();
		_control_tab->lwAvailKeywordTags->clear();
		_control_tab->lwKeywordTags->clear();

		currentBlogElement = currentAccountElement.elementsByTagName("blog")
					 .at(currentBlog)
					 .toElement();
		currentBlogid = currentBlogElement.firstChildElement("blogid").text();
		// qDebug() << currentBlogid;

		_control_tab->lwAvailKeywordTags->clear();
		_control_tab->lwKeywordTags->clear();
		populateTagList();

		QDomElement catsElement =
		    currentBlogElement.firstChildElement("categories");
		if (!catsElement.isNull()) {
			QDomNodeList catsList = catsElement.elementsByTagName("category");
			int c = catsList.count();
			qDebug() << c << "categories";
			if (c) {
				// #ifndef NO_DEBUG_OUTPUT
				// qDebug() << "Categories: " << c;
				// #endif
				_control_tab->cbMainCat->clear();
				_control_tab->lwOtherCats->clear();
				for (i = 0; i < c; i++) {
					currentCategoryText = decodeXmlEntities(
					    catsList.at(i).firstChildElement("categoryName").text());
					currentCatId = catsList.at(i).firstChildElement("categoryId").text();
					_control_tab->cbMainCat->addItem(currentCategoryText, QVariant(currentCatId));
					_control_tab->lwOtherCats->addItem(currentCategoryText);
					if (currentCatId == "1") uncategorized = i;
					_control_tab->chNoCats->setEnabled(true);
					_control_tab->cbMainCat->setEnabled(true);
					_control_tab->lwOtherCats->setEnabled(true);
					_status_widget->clearMessage(); // as otherwise,
									// "there are no
									// categories"
									// would still show
				}
				if (uncategorized) _control_tab->cbMainCat->setCurrentIndex(uncategorized);
				emit categoryRefreshFinished();
				if (QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
			} else
				callRefreshCategories();
		} else
			callRefreshCategories();
	}
}

bool Blogger::caseInsensitiveLessThan(const QString& s1, const QString& s2)
{
	return s1.toLower() < s2.toLower();
}

void Blogger::handleConsole(bool isChecked)
{
	QAction *menuConsoleAction = nullptr, *menuPreviewAction = nullptr;
	if (_super_menu) {
		menuConsoleAction =
		    _super_menu->getConsoleAction(); // qobject_cast<QAction
						     // *>(_super_menu->getConsoleAction());
		menuPreviewAction =
		    _super_menu->getPreviewAction(); // qobject_cast<QAction
						     // *>(_super_menu->getPreviewAction());
		if (menuConsoleAction) menuConsoleAction->setChecked(isChecked);
	}
	if (!isChecked) {
		_main_stack->setCurrentIndex(_editor_id);
		_search_widget->setTextEdit(_editor);
		if (menuConsoleAction) menuConsoleAction->setText(tr("&Console"));
		if (menuPreviewAction) menuPreviewAction->setEnabled(true);
	} else {
		_previous_raised_ls_widget = _main_stack->currentIndex();
		_main_stack->setCurrentIndex(_console_id);
		_search_widget->setTextEdit(_console);
		if (menuConsoleAction) menuConsoleAction->setText(tr("Exit &console"));
		if (menuPreviewAction) menuPreviewAction->setEnabled(false);
		_console->setFocus(Qt::OtherFocusReason);
	}
}

void Blogger::clearConsole()
{
	_console->clear();
}

//#ifndef Q_OS_MAC

//void Blogger::setToolBarVisible(bool vis)
//{
////	_editor->tool_bar()->setVisible(vis);
//}

//#endif

void Blogger::makeBold()
{
	_editor->insertPlainText(
	    QString("<strong>%1</strong>").arg(_editor->textCursor().selectedText()));
}

void Blogger::makeItalic()
{
	_editor->insertPlainText(
	    QString("<em>%1</em>").arg(_editor->textCursor().selectedText()));
}

void Blogger::makeUnderline()
{
	_editor->insertPlainText(
	    QString("<u>%1</u>").arg(_editor->textCursor().selectedText()));
}

void Blogger::insertMore()
{
	if (!_editor->toPlainText().contains("<!--more-->"))
		_editor->insertPlainText("<!--more-->");
	else
		_status_widget->showMessage(tr("A 'more' tag already exists."), 2000);
}

void Blogger::makeBlockquote()
{
	_editor->insertPlainText(QString("<blockquote>%1</blockquote>").arg(_editor->textCursor().selectedText()));
}

void Blogger::makePara()
{
	_editor->insertPlainText(
	    QString("<p>%1</p>").arg(_editor->textCursor().selectedText()));
}

void Blogger::insertLink(bool isAutoLink)
{
	QString linkString, titleString;
	QString insertionString = "";
	QString selectedString = _editor->textCursor().selectedText();
	QString selectedStringLC;
	QDialog linkEntry(this);
	Ui::LinkEntry leui;
#ifdef Q_OS_MAC
	linkEntry.setWindowFlags(Qt::Sheet);
#endif
	leui.setupUi(&linkEntry);
	if (isAutoLink) leui.cbMakeAutoLink->setChecked(Qt::Checked);
	if (!selectedString.isEmpty()) leui.leLinkText->setText(selectedString);
	if (linkEntry.exec()) {
		linkString = leui.leLinkURL->text();
		insertionString += QString("<a href=\"%1\"").arg(linkString);
		if (leui.leLinkTitle->text().size()) {
			titleString = leui.leLinkTitle->text();
			insertionString +=
			    QString(" title=\"%1\"").arg(titleString.replace("\"", "&quot;"));
		}
		if (leui.chLinkTarget->isChecked()) {
			switch (leui.cbLinkTarget->currentIndex()) {
			case 0:
				insertionString += QString(" target=\"_top\"");
				break;

			case 1:
				insertionString += QString(" target=\"_blank\"");
				break;

			case 2:
				insertionString += QString(" target=\"view_window\"");
				break;
			}
		}
		insertionString += ">";
		_editor->insertPlainText(
		    QString("%1%2</a>").arg(insertionString).arg(leui.leLinkText->text()));
	}
	if (leui.cbMakeAutoLink->isChecked()) {
		selectedStringLC = selectedString.toLower().trimmed();
		autoLinkDictionary.insert(selectedStringLC, linkString);
		autoLinkTitleDictionary.insert(selectedStringLC, titleString);
		if (leui.chLinkTarget->isChecked())
			autoLinkTargetDictionary.insert(selectedStringLC, leui.cbLinkTarget->currentIndex());
		else
			autoLinkTargetDictionary.remove(selectedStringLC);
		saveAutoLinkDictionary();
	}
}

void Blogger::insertLinkFromClipboard()
{
	QString linkString(QApplication::clipboard()->text());
	_editor->insertPlainText(QString("<a href=\"%1\">%2</a>").arg(linkString).arg(_editor->textCursor().selectedText()));
}

void Blogger::insertSelfLink()
{
	QString linkString(_editor->textCursor().selectedText());
	if (QUrl(linkString, QUrl::StrictMode).isValid())
		_editor->insertPlainText(QString("<a href=\"%1\">%1</a>").arg(linkString));
	else
		_status_widget->showMessage(tr("The selection is not a valid URL."), 2000);
}

void Blogger::insertAutoLink()
{
	QString selectedText = _editor->textCursor().selectedText();
	QString selectedTextLC = selectedText.toLower().trimmed();
	QString titleString, targetString;
	QList<QString> targets;
	targets << "_top"
		<< "_blank"
		<< "view_window";
	if (autoLinkDictionary.contains(selectedTextLC)) {
		titleString = QString(" title=\"%1\"")
				  .arg(autoLinkTitleDictionary.value(selectedTextLC));
		if (autoLinkTargetDictionary.contains(selectedTextLC)) targetString = QString(" target=\"%1\"")
											  .arg(targets.value(autoLinkTargetDictionary.value(selectedTextLC)));
		_editor->insertPlainText(QString("<a href=\"%1\"%2%3>%4</a>").arg(autoLinkDictionary.value(selectedTextLC)).arg(titleString).arg(targetString).arg(selectedText));
	} else
		insertLink(true);
}

void Blogger::insertImage()
{
	QString insertionString, styleString;
	QStringList borderStyles;
	borderStyles << "dotted"
		     << "dashed"
		     << "solid"
		     << "double";

	QDialog image_entry(this);
	Ui::ImageEntry ieui;
#ifdef Q_OS_MAC
	image_entry.setWindowFlags(Qt::Sheet);
#endif
	ieui.setupUi(&image_entry);
	ieui.gbMargins->setVisible(false);
	if (image_entry.exec()) {
		insertionString = QString("<img src=\"%1\"").arg(ieui.leImageURL->text());
		if (ieui.leTitle->text().size()) insertionString += QString(" title=\"%1\"")
									.arg(ieui.leTitle->text().replace("\"", "&quot;"));
		if (ieui.leAltText->text().size())
			insertionString +=
			    QString(" alt=\"%1\"")
				.arg(ieui.leAltText->text().replace("\"", "&quot;"));
		// Alignment
		if (ieui.cbAlign->currentIndex()) {
			if (ieui.cbAlign->currentIndex() == 1)
				styleString += "float: left; ";
			else // index is 2
				styleString += "float: right; ";
		}
		// Border
		if (ieui.cbBorderStyle->currentIndex() != 0 &&
		    ieui.sbBorderWidth->value() != 0)
			styleString +=
			    QString("border: %1px %2; ")
				.arg(ieui.sbBorderWidth->value())
				.arg(borderStyles[ieui.cbBorderStyle->currentIndex() - 1]);
		// Margins
		if (ieui.sbMarginLeft->value() != 0) styleString +=
		    QString("margin-left: %1px; ").arg(ieui.sbMarginLeft->value());
		if (ieui.sbMarginRight->value() != 0) styleString +=
		    QString("margin-right: %1px; ").arg(ieui.sbMarginRight->value());
		if (ieui.sbMarginTop->value() != 0) styleString +=
		    QString("margin-top: %1px; ").arg(ieui.sbMarginTop->value());
		if (ieui.sbMarginBottom->value() != 0) styleString +=
		    QString("margin-bottom: %1px").arg(ieui.sbMarginBottom->value());
		// Insert style string into main insertion string
		if (!styleString.isEmpty()) insertionString += QString(" style=\"%1\"").arg(styleString.trimmed());
		insertionString += " />";
		_editor->insertPlainText(insertionString);
	}
}

void Blogger::insertImageFromClipboard()
{
	QString linkString(QApplication::clipboard()->text());
	_editor->insertPlainText(QString("<img src=\"%1\">%2</img>").arg(linkString).arg(_editor->textCursor().selectedText()));
}

// void EditingWindow::cut(){
// editor_object->cut();
// }

void Blogger::copy()
{
	if (_main_stack->currentIndex() == _preview_id)
		_preview_window->copy();
	else
		_editor->copy();
}

QString Blogger::removeReadMore(QString& source)
{
	// List of regexes to match "read more" strings. Will keep the list
	// in case other variants are found. (Previously had list of specific
	// websites.)
	QList<QRegExp> regExpList;
	regExpList << QRegExp(
	    "\\n\\nRead more:? "
	    "http://"
	    "(([a-zA-Z]|[a-zA-Z][a-zA-Z0-9\\-]*[a-zA-Z0-9])\\.)*([A-Za-z]|[A-Za-z][A-"
	    "Za-z0-9\\-]*[A-Za-z0-9]).*$");
	if (suppressReadMore) {
		// qDebug() << "suppressing read more";
		QString text = source;
		Q_FOREACH (QRegExp rx, regExpList) {
			if (text.contains(rx)) {
				text.remove(rx);
				return text;
			}
		}
	}
	return source;
}

void Blogger::paste()
{
	QString pastedText = QApplication::clipboard()->text();
	_editor->insertPlainText(removeReadMore(pastedText));
	_editor->ensureCursorVisible();
}

void Blogger::tidyPaste()
{
	QString pastedText = QApplication::clipboard()->text();
	pastedText = removeReadMore(pastedText);
	pastedText.replace(QRegExp(" {2,}"), " ");
	pastedText.replace(QRegExp(" *\n{1} *"), " ");
	_editor->insertPlainText(pastedText);
	_editor->ensureCursorVisible();
}

// void EditingWindow::undo(){
// editor_object->document()->undo();
// }

// void EditingWindow::redo(){
// editor_object->document()->redo();
// }

void Blogger::makeUnorderedList()
{
	QString listString = _editor->textCursor().selection().toPlainText();
	if (!listString.isEmpty()) _editor->insertPlainText(getHTMLList(QString("ul"), listString));
}

void Blogger::makeOrderedList()
{
	QString listString = _editor->textCursor().selection().toPlainText();
	if (!listString.isEmpty()) _editor->insertPlainText(getHTMLList(QString("ol"), listString));
}

QString& Blogger::getHTMLList(QString tag, QString& text)
{
	QString return_value, workstring;
	QStringList worklist;

	worklist = text.split("\n");
	for (int a = 0; a < worklist.count(); a++) {
		worklist[a].prepend("<li>");
		worklist[a].append("</li>");
	}
	text = worklist.join("\n");
	text.prepend(QString("<%1>").arg(tag));
	text.append(QString("</%1>").arg(tag));

	return text;
}

void Blogger::pasteAsMarkedParagraphs()
{
	QString insertion = QApplication::clipboard()->text().trimmed();
	insertion = removeReadMore(insertion);
	if (!insertion.isEmpty()) {
		insertion.replace(QRegExp("\n+"), "</p>\n<p>");
		insertion.prepend("<p>");
		insertion.append("</p>");
		_editor->insertPlainText(insertion);
		_editor->ensureCursorVisible();
	}
}

void Blogger::pasteAsBlockquote()
{
	QString insertion = QApplication::clipboard()->text().trimmed();
	insertion = removeReadMore(insertion);
	if (!insertion.isEmpty()) {
		insertion.replace(QRegExp("\n{1,2}"), "\n\n");
		_editor->insertPlainText(
		    QString("<blockquote>%1</blockquote>").arg(insertion));
		_editor->ensureCursorVisible();
	}
}

void Blogger::pasteAsMarkdownBlockquote()
{
	QString insertion = QApplication::clipboard()->text().trimmed();
	insertion = removeReadMore(insertion);
	QString separator = "\n>\n";
	if (!insertion.isEmpty()) {
		insertion.prepend(">");
		insertion.replace(QRegExp("\n{1,2}"), "\n>\n>");
		_editor->insertPlainText(insertion);
		_editor->ensureCursorVisible();
	}
}

void Blogger::pasteAsUnorderedList()
{
	QString insertion = QApplication::clipboard()->text().trimmed();
	insertion = removeReadMore(insertion);
	insertion.replace(QRegExp("\n{2,}"), "\n");
	if (!insertion.isEmpty()) {
		_editor->insertPlainText(getHTMLList(QString("ul"), insertion));
		_editor->ensureCursorVisible();
	}
}

void Blogger::pasteAsOrderedList()
{
	QString insertion = QApplication::clipboard()->text().trimmed();
	insertion = removeReadMore(insertion);
	insertion.replace(QRegExp("\n{2,}"), "\n");
	if (!insertion.isEmpty()) {
		_editor->insertPlainText(getHTMLList(QString("ol"), insertion));
		_editor->ensureCursorVisible();
	}
}

void Blogger::doPreview(bool isChecked, bool markdownFailed)
{
	qDebug() << "preview starting";
	QString line, techTagString;
	QString conversionString = "", conversionStringB = "";
	QTextDocument cDoc;
	// bool finished = false;
	bool isPre = false;

	QAction* menuPreviewAction;
	if (_super_menu) {
		menuPreviewAction =
		    qobject_cast<QAction*>(_super_menu->getPreviewAction());
		if (menuPreviewAction) menuPreviewAction->setChecked(isChecked);
		_editor->_action_preview->setChecked(isChecked);
	}
	if (isChecked) {
		if (useMarkdown && !markdownFailed) {
			conversionStringB = processWithMarkdown(_editor->toPlainText());
			if (conversionStringB.isNull()) {
				qDebug() << "Markdown conversion failed";
				doPreview(isChecked, true);
				return;
			}
			// Now that the process has done its job, we can add the title and
			// display
			// the preview

			conversionStringB.prepend(QString("<strong>%1</strong>\n\n").arg(_control_tab->leTitle->text().size() ? _control_tab->leTitle->text() : "<em>Untitled</em>"));
			_preview_window->setHtml(conversionStringB);
		} else { // i.e. if not Markdown or Markdown failed
			conversionString += QString("<b>%1</b>\n\n")
						.arg(_control_tab->leTitle->text().size() ? _control_tab->leTitle->text() : "<i>Untitled</i>");
			conversionString += _editor->toPlainText();
			QTextStream a(&conversionString);
			QRegExp re(
			    "^(<table|thead|tfoot|caption|tbody|tr|td|th|div|dl|dd|dt|ul|"
			    "ol|li|select|form|blockquote|address|math|p|h[1-6])>");
			do {
				line = a.readLine();
				if (!line.isEmpty()) {
					if (re.exactMatch(line))
						conversionStringB += line;
					else {
						if (line.startsWith("<pre>")) {
							isPre = true;
							conversionStringB += line;
						} else {
							conversionStringB +=
							    QString(isPre ? "%1\n" : "<p>%1</p>").arg(line);
							if (line.contains("</pre>")) isPre = false;
						}
					}
				}
			} while (!a.atEnd());
			_preview_window->setHtml(conversionStringB);
		}
		_previous_raised_ls_widget = _main_stack->currentIndex();
		connect(_preview_window, SIGNAL(highlighted(const QString&)), this, SLOT(showHighlightedURL(const QString&)));
		_main_stack->setCurrentIndex(_preview_id);
		_search_widget->setTextEdit(_preview_window);
		_editor->_action_preview->setText(tr("Exit p&review"));
		// action_Preview->setIconText(tr("Exit preview"));
		_editor->_action_preview->setToolTip(tr("Exit preview"));
#ifdef Q_OS_MAC
		if (menuPreviewAction) menuPreviewAction->setText(tr("Exit p&review"));
		if (superMenu) superMenu->setConsoleEnabled(false);
#endif
		_preview_window->setFocus(Qt::OtherFocusReason);
		_control_tab->setEnabled(false); // leftWidget->setEnabled(false);
		setEditActionsEnabled(false);
		_search_widget->close();
	} else { // i.e. if the preview button is being released
		 // mainStack->setCurrentIndex( previousRaisedLSWidget );
		_main_stack->setCurrentIndex(_editor_id);
		_preview_window->disconnect(SIGNAL(highlighted(const QString&)));
		_search_widget->setTextEdit(_editor);
		_editor->_action_preview->setText(tr("Entry in p&review"));
		// action_Preview->setIconText(tr("Entry in preview"));
		_editor->_action_preview->setToolTip(tr("Entry in preview"));
#ifdef Q_OS_MAC
		if (menuPreviewAction) menuPreviewAction->setText(tr("Entry in p&review"));
		if (superMenu) superMenu->setConsoleEnabled(true);
#endif
		_control_tab->setEnabled(true); // leftWidget->setEnabled(true);
		setEditActionsEnabled(true);
		_search_widget->close();
	}
}

void Blogger::setEditActionsEnabled(bool state)
{
	if (_super_menu) _super_menu->setEditActionsEnabled(state);
	QList<FlatToolButton*> edit_actions;

	edit_actions << _editor->_bold << _editor->_italic << _editor->_underline
		     << _editor->_code << _editor->_action_link
		     << _editor->_action_image << _editor->_action_more;
	Q_FOREACH (FlatToolButton* a, edit_actions)
		a->setEnabled(state);
#ifndef Q_OS_MAC
	if (_super_menu) {
		_super_menu->insMenu->setEnabled(state);
		_super_menu->pasteAsMenu->setEnabled(state);
		_super_menu->categoryMenu->setEnabled(state);
	}
#endif
}

void Blogger::showHighlightedURL(const QString& highlightedURL)
{
	_status_widget->showMessage(highlightedURL, 2000);
}

void Blogger::blogThis()
{
	if (useWordpressAPI) {
		if (entryBlogged)
			submitWPEdit();
		else
			newWPPost();
	} else
		newMTPost();
}

void Blogger::newMTPost()
{
	QDomDocument doc;
	QDomElement methodCall, params, param, member, value, integer, string, rpcstruct, rpcarray, actualValue;
	QString description, extEntry, techTagString, convertedString, keywordTagList;
	bool takeComms = _control_tab->chAllowComments->isChecked();
	bool takeTB = _control_tab->chAllowTB->isChecked();
	// bool blogidIsInt;
	int count, tags;
	QList<QString> tblist;
	if (!_current_http_business) {
		if (!entryBlogged) {
			if (_editor->toPlainText().contains("<!--more-->")) {
				description =
				    QString(_editor->toPlainText()).section("<!--more-->", 0, 0);
				extEntry =
				    QString(_editor->toPlainText()).section("<!--more-->", -1, -1);
			} else {
				description = QString(_editor->toPlainText());
				extEntry = "";
			}
			if (doMarkdownWhenPosting) {
				convertedString = processWithMarkdown(description);
				if (!convertedString.isNull())
					/* description = stripParaTags ?
														     convertedString.remove( "<p>"
														     ).remove( "</p>" ) : convertedString; */
					description = convertedString;
				else
					_status_widget->showMessage(
					    tr("Markdown conversion failed; posting main entry as is."), 2000);
				if (!extEntry.isEmpty()) {
					convertedString = processWithMarkdown(extEntry);
					if (!convertedString.isNull())
						/*extEntry = stripParaTags ?
																	  convertedString.remove(
																	   "<p>" ).remove( "</p>" ) :
																	   convertedString; */
						extEntry = convertedString;
					else
						_status_widget->showMessage(
						    tr("Markdown conversion failed; posting extension as is."), 2000);
				}
			}
			dateTime = QDateTime::currentDateTime().toString(Qt::ISODate);
			dateTime.remove(QChar('-'));
			if (_control_tab->lwTags->count()) {
				tags = _control_tab->lwTags->count();
				techTagString =
				    "<p style=\"text-align:right;font-size:10px;\">Technorati Tags: ";
				for (count = 0; count < tags; count++)
					techTagString.append(
					    QString("<a href=\"http://technorati.com/tag/%1\" "
						    "rel=\"tag\">%2</a>%3")
						.arg(_control_tab->lwTags->item(count)->text().replace(' ', '+'))
						.arg(_control_tab->lwTags->item(count)->text().replace("+", " "))
						.arg((count == tags - 1) ? "</p>\n\n" : ", "));
				if (_control_tab->rbStartOfMainEntry->isChecked())
					description.insert(0, techTagString);
				else
					description.append(techTagString);
			}
			QString blogid =
			    _control_tab->cbBlogSelector
				->itemData(_control_tab->cbBlogSelector->currentIndex())
				.toString();
			/*      QRegExp blogidRegExp( "^[0-9]+$" );
								      blogidIsInt
			   = blogidRegExp.exactMatch( blogid ); */

			methodCall = doc.createElement("methodCall");
			methodCall.appendChild(XmlMethodName(doc, "metaWeblog.newPost"));

			params = doc.createElement("params");
			params.appendChild(XmlValue(doc, "string", blogid));
			params.appendChild(XmlValue(doc, "string", login));
			params.appendChild(XmlValue(doc, "string", password));

			param = doc.createElement("param");
			value = doc.createElement("value");
			rpcstruct = doc.createElement("struct");
			rpcstruct.appendChild(
			    XmlMember(doc, "title", "string", _control_tab->leTitle->text()));
			rpcstruct.appendChild(
			    XmlMember(doc, "description", "string", description));
			if (postDateTime) rpcstruct.appendChild(
			    XmlMember(doc, "dateCreated", "dateTime.iso8601", dateTime));
			rpcstruct.appendChild(XmlMember(doc, "mt_allow_comments", "boolean", takeComms ? "1" : "0"));
			rpcstruct.appendChild(
			    XmlMember(doc, "mt_allow_pings", "boolean", takeTB ? "1" : "0"));
			rpcstruct.appendChild(XmlMember(doc, "mt_text_more", "string", extEntry));
			if (_control_tab->teExcerpt->toPlainText().length())
				rpcstruct.appendChild(
				    XmlMember(doc, "mt_excerpt", "string", _control_tab->teExcerpt->toPlainText().replace(QChar('&'), "&amp;")));
			else
				rpcstruct.appendChild(XmlMember(doc, "mt_excerpt", "string", ""));
			if (!_control_tab->lwKeywordTags->count())
				rpcstruct.appendChild(XmlMember(doc, "mt_keywords", "string", ""));
			else {
				for (count = 0; count < _control_tab->lwKeywordTags->count(); count++) {
					keywordTagList.append(
					    _control_tab->lwKeywordTags->item(count)->text());
					if (count != _control_tab->lwKeywordTags->count() - 1) keywordTagList.append(", ");
				}
				rpcstruct.appendChild(
				    XmlMember(doc, "mt_keywords", "string", keywordTagList));
			}
			if (_control_tab->lwTBPings->count()) {
				for (count = 0; count < _control_tab->lwTBPings->count(); count++) tblist.append(_control_tab->lwTBPings->item(count)->text());
				rpcstruct.appendChild(XmlRpcArray(doc, "mt_tb_ping_urls", tblist));
			}
			value.appendChild(rpcstruct);
			param.appendChild(value);
			params.appendChild(param);
			if (!useWordpressAPI) params.appendChild(
			    XmlValue(doc, "boolean", _control_tab->cbStatus->currentIndex() ? "1" : "0"));
			methodCall.appendChild(params);
			doc.appendChild(methodCall);
			doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

			QByteArray requestArray(doc.toByteArray());
			responseData = "";
			_doing_new_post = true;
			placeNetworkRequest(_metaWeblog_newPost, requestArray);
			if (QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
			if (postAsSave && !_entry_ever_saved) {
				_clean_save = true;
				setDirtySignals(true);
			}
			if (location.contains("mt-xmlrpc.cgi") &&
			    _control_tab->cbStatus->currentIndex() == 1)
				_control_tab->progressBar->setMaximum(
				    (categoriesEnabled && !_control_tab->chNoCats->isChecked()) ? 6 : 4);
			else
				_control_tab->progressBar->setMaximum(
				    (categoriesEnabled && !_control_tab->chNoCats->isChecked()) ? 4 : 2);
			_control_tab->progressBar->setValue(1);
			_control_tab->progressWidget->setVisible(true);

#ifdef WIN_QTV
			addToConsole("Showing taskbar button");
			qDebug() << "Showing taskbar button";
			if ((useWordpressAPI || location.contains("mt-xmlrpc.cgi")) &&
			    cw->cbStatus->currentIndex() == 1)
				taskbarProgress->setMaximum(6);
			else
				taskbarProgress->setMaximum(4);
			taskbarProgress->setValue(1);
			taskbarProgress->show();
#endif

			// cw->cbPageSelector->setCurrentIndex(0);
			_control_tab->setCurrentIndex(0);
			/*#ifndef Q_OS_MAC
								      progressBarAction->setVisible(
			   true );
			 #endif */
		} else
			submitMTEdit();
	} else {
#ifdef QTM_DEBUG
		statusWidget->showMessage(tr("newPost: All HTTP requests are blocked"), 2000);
#else
		_status_widget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
	}
}

void Blogger::newWPPost()
{
	QDomDocument doc;
	QDomElement methodCall, params, param, member, value, integer, string, rpcstruct, rpcarray, actualValue, primCatElem, taxStruct, taxValue, taxMember, taxMemberName;
	QString primCat, blogid, entryText, techTagString, convertedString, keywordTagList, tblist;
	bool takeComms = _control_tab->chAllowComments->isChecked();
	bool takeTB = _control_tab->chAllowTB->isChecked();
	// bool blogidIsInt;
	int count, tags;
	QList<QString> catList, tagList;
	if (!_current_http_business) {
		entryText = _editor->toPlainText();
		// Do Markdown, if enabled
		if (doMarkdownWhenPosting) {
			convertedString = processWithMarkdown(entryText);
			if (!convertedString.isNull())
				/* description = stripParaTags ?
											       convertedString.remove( "<p>" ).remove(
											       "</p>" ) : convertedString; */
				entryText = convertedString;
			else
				_status_widget->showMessage(
				    tr("Markdown conversion failed; posting main entry as is."), 2000);
		}
		dateTime = QDateTime::currentDateTime().toString(Qt::ISODate);
		dateTime.remove(QChar('-'));
		// Make a string of the Technorati tags
		if (_control_tab->lwTags->count()) {
			tags = _control_tab->lwTags->count();
			techTagString =
			    "<p style=\"text-align:right;font-size:10px;\">Technorati "
			    "Tags: ";
			for (count = 0; count < tags; count++)
				techTagString.append(
				    QString(
					"<a href=\"http://technorati.com/tag/%1\" rel=\"tag\">%2</a>%3")
					.arg(
					    _control_tab->lwTags->item(count)->text().replace(' ', '+'))
					.arg(
					    _control_tab->lwTags->item(count)->text().replace("+", " "))
					.arg((count == tags - 1) ? "</p>\n\n" : ", "));
			// Whether to put the Technorati tags at beginning or end
			if (_control_tab->rbStartOfMainEntry->isChecked())
				entryText.insert(0, techTagString);
			else
				entryText.append(techTagString);
		}
		// Make a list of the selected categories (WP API has no "primary category")
		if (_control_tab->chAllowComments->isChecked()) {
			/*primCatElem = currentBlogElement.firstChildElement( "categories" )
									.childNodes().at(
			   cw->cbMainCat->currentIndex()
									 ).toElement();
			 */
			catList << _control_tab->cbMainCat
				       ->itemData(_control_tab->cbMainCat->currentIndex())
				       .toString();
			for (int a = 0; a < _control_tab->lwOtherCats->count(); a++) {
				if (_control_tab->lwOtherCats->isItemSelected(
					_control_tab->lwOtherCats->item(a))) {
					// cat = currentBlogElement.firstChildElement(
					// "categories" ).childNodes().at( a ).toElement();
					catList << _control_tab->cbMainCat->itemData(a).toString();
				}
			}
		}
		// Make a list of selected WP tags
		if (_control_tab->lwKeywordTags->count())
			for (count = 0; count < _control_tab->lwKeywordTags->count(); count++) tagList << _control_tab->lwKeywordTags->item(count)->text();
		// Make a list of trackback pings to be sent
		if (_control_tab->lwTBPings->count()) {
			for (count = 0; count < _control_tab->lwTBPings->count(); count++) {
				if (count) tblist.append(' ');
				tblist.append(_control_tab->lwTBPings->item(count)->text());
			}
		}
		blogid = _control_tab->cbBlogSelector
			     ->itemData(_control_tab->cbBlogSelector->currentIndex())
			     .toString();

		methodCall = doc.createElement("methodCall");
		methodCall.appendChild(XmlMethodName(doc, "wp.newPost"));

		params = doc.createElement("params");
		params.appendChild(XmlValue(doc, "string", blogid));
		params.appendChild(XmlValue(doc, "string", login));
		params.appendChild(XmlValue(doc, "string", password));

		param = doc.createElement("param");
		value = doc.createElement("value");
		rpcstruct = doc.createElement("struct");
		rpcstruct.appendChild(XmlMember(doc, "post_type", "string", "post"));
		rpcstruct.appendChild(XmlMember(doc, "post_status", "string", _control_tab->cbStatus->currentIndex() ? "publish" : "draft"));
		rpcstruct.appendChild(
		    XmlMember(doc, "post_title", "string", _control_tab->leTitle->text()));
		rpcstruct.appendChild(XmlMember(doc, "post_content", "string", entryText));
		if (postDateTime) rpcstruct.appendChild(
		    XmlMember(doc, "post_date", "dateTime.iso8601", dateTime));
		rpcstruct.appendChild(XmlMember(doc, "comment_status", "string", takeComms ? "open" : "closed"));
		rpcstruct.appendChild(
		    XmlMember(doc, "ping_status", "string", takeTB ? "open" : "closed"));
		if (!_control_tab->lePassword->text().isEmpty()) rpcstruct.appendChild(XmlMember(doc, "post_password", "string", _control_tab->lePassword->text()));
		/* if( cw->chSticky->isChecked() )
						  rpcstruct.appendChild(
		   XmlMember( doc, "sticky", "bool", "true" )
						   ); */
		if (catList.count()) {
			taxMember = doc.createElement("member");
			taxMemberName = doc.createElement("name");
			taxMemberName.appendChild(QDomText(doc.createTextNode("terms")));
			taxMember.appendChild(taxMemberName);
			taxValue = doc.createElement("value");
			taxStruct = doc.createElement("struct");
			taxStruct.appendChild(XmlRpcArray(doc, "category", "string", catList));
			taxValue.appendChild(taxStruct);
			taxMember.appendChild(taxValue);
			rpcstruct.appendChild(taxMember);
		}
		if (tagList.count()) rpcstruct.appendChild(XmlRpcArray(doc, "tags_input", "string", tagList));
		if (tblist.length()) rpcstruct.appendChild(XmlMember(doc, "to_ping", "string", tblist));
		value.appendChild(rpcstruct);
		param.appendChild(value);
		params.appendChild(param);
		methodCall.appendChild(params);
		doc.appendChild(methodCall);
		doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

		QByteArray requestArray(doc.toByteArray());
		responseData = "";
		placeNetworkRequest(_wp_newPost, requestArray);
		if (QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		if (postAsSave && !_entry_ever_saved) {
			_clean_save = true;
			setDirtySignals(true);
		}
		_control_tab->progressBar->setMaximum(2);
		_control_tab->progressBar->setValue(1);
		_control_tab->progressWidget->setVisible(true);
		// cw->cbPageSelector->setCurrentIndex(0);
		_control_tab->setCurrentIndex(0);
	} else {
#ifdef QTM_DEBUG
		statusWidget->showMessage(
		    tr("refreshCategories: All HTTP requests are blocked"), 2000);
#else
		_status_widget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
	}
}

void Blogger::submitMTEdit()
{
	QDomDocument doc;
	QDomElement methodCall, params, param, value, rpcstruct, rpcarray;
	QString description, extEntry, techTagString, convertedString, keywordTagList;
	int count, tags;
	bool takeComms = _control_tab->chAllowComments->isChecked();
	bool takeTB = _control_tab->chAllowTB->isChecked();
	QList<QString> tblist;
	if ((!useWordpressAPI) && _editor->toPlainText().contains("<!--more-->")) {
		description = QString(_editor->toPlainText()).section("<!--more-->", 0, 0);
		extEntry = QString(_editor->toPlainText()).section("<!--more-->", -1, -1);
	} else {
		description = QString(_editor->toPlainText());
		extEntry = "";
	}
	if (doMarkdownWhenPosting) {
		convertedString = processWithMarkdown(description);
		if (!convertedString.isNull())
			/* description = stripParaTags ?
									convertedString.remove(
			   "<p>" ).remove( "</p>" ) :
									 convertedString;
			 */
			description = convertedString;
		else
			_status_widget->showMessage(
			    tr("Markdown conversion failed; posting main entry as is."), 2000);
		if (!extEntry.isEmpty()) {
			convertedString = processWithMarkdown(extEntry);
			if (!convertedString.isNull())
				/* extEntry = stripParaTags ?
											      convertedString.remove( "<p>" ).remove( "</p>"
											       ) : convertedString; */
				extEntry = convertedString;
			else
				_status_widget->showMessage(
				    tr("Markdown conversion failed; posting extension as is."), 2000);
		}
	}
	if (_control_tab->lwTags->count()) {
		tags = _control_tab->lwTags->count();
		techTagString =
		    "<p style=\"text-align:right;font-size:10px;\">Technorati Tags: ";
		for (count = 0; count < tags; count++) {
			techTagString.append(
			    QString(
				"<a href=\"http://technorati.com/tag/%1\" rel=\"tag\">%2</a>%3")
				.arg(_control_tab->lwTags->item(count)->text())
				.arg(_control_tab->lwTags->item(count)->text().replace("+", " "))
				.arg((count == tags - 1) ? "</p>" : ", "));
		}
		if (_control_tab->rbStartOfMainEntry->isChecked())
			description.insert(0, techTagString);
		else
			description.append(techTagString);
	}
	methodCall = doc.createElement("methodCall");
	methodCall.appendChild(XmlMethodName(doc, "metaWeblog.editPost"));

	params = doc.createElement("params");
	params.appendChild(XmlValue(doc, "string", entryNumber));
	params.appendChild(XmlValue(doc, "string", login));
	params.appendChild(XmlValue(doc, "string", password));

	param = doc.createElement("param");
	value = doc.createElement("value");
	rpcstruct = doc.createElement("struct");
	rpcstruct.appendChild(
	    XmlMember(doc, "title", "string", _control_tab->leTitle->text()));
	rpcstruct.appendChild(XmlMember(doc, "description", "string", description));
	if (postDateTime) rpcstruct.appendChild(
	    XmlMember(doc, "dateCreated", "dateTime.iso8601", dateTime));
	rpcstruct.appendChild(
	    XmlMember(doc, "mt_allow_comments", "boolean", takeComms ? "1" : "0"));
	rpcstruct.appendChild(
	    XmlMember(doc, "mt_allow_pings", "boolean", takeTB ? "1" : "0"));
	rpcstruct.appendChild(XmlMember(doc, "mt_text_more", "string", extEntry));
	if (_control_tab->teExcerpt->toPlainText().length())
		rpcstruct.appendChild(XmlMember(doc, "mt_excerpt", "string", _control_tab->teExcerpt->toPlainText().replace(QChar('&'), "&amp;")));
	else
		rpcstruct.appendChild(XmlMember(doc, "mt_excerpt", "string", ""));
	if (!_control_tab->lwKeywordTags->count())
		rpcstruct.appendChild(XmlMember(doc, "mt_keywords", "string", ""));
	else {
		for (count = 0; count < _control_tab->lwKeywordTags->count(); count++) {
			keywordTagList.append(_control_tab->lwKeywordTags->item(count)->text());
			if (count != _control_tab->lwKeywordTags->count() - 1) keywordTagList.append(", ");
		}
		rpcstruct.appendChild(
		    XmlMember(doc, "mt_keywords", "string", keywordTagList));
	}
	if (_control_tab->lwTBPings->count()) {
		for (count = 0; count < _control_tab->lwTBPings->count(); count++) tblist.append(_control_tab->lwTBPings->item(count)->text());
		rpcstruct.appendChild(XmlRpcArray(doc, "mt_tb_ping_urls", tblist));
	}
	value.appendChild(rpcstruct);
	param.appendChild(value);
	params.appendChild(param);
	params.appendChild(XmlValue(doc, "boolean", _control_tab->cbStatus->currentIndex() ? "1" : "0"));
	methodCall.appendChild(params);
	doc.appendChild(methodCall);
	doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

	QByteArray requestArray(doc.toByteArray());
	responseData = "";
	placeNetworkRequest(_metaWeblog_editPost, requestArray);
	if (QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	if (postAsSave && !_entry_ever_saved) {
		_clean_save = true;
		setDirtySignals(true);
	}
	if (location.contains("mt-xmlrpc.cgi") &&
	    _control_tab->cbStatus->currentIndex() == 1)
		_control_tab->progressBar->setMaximum(
		    (categoriesEnabled && _control_tab->chNoCats->isChecked()) ? 6 : 4);
	else
		_control_tab->progressBar->setMaximum(
		    (categoriesEnabled && _control_tab->chNoCats->isChecked()) ? 4 : 2);
	_control_tab->progressBar->setValue(1);
	_control_tab->progressWidget->setVisible(true);

#ifdef WIN_QTV
	if (location.contains("mt-xmlrpc.cgi") && cw->cbStatus->currentIndex() == 1)
		taskbarProgress->setMaximum(6);
	else
		taskbarProgress->setMaximum(4);
	taskbarProgress->setValue(1);
	taskbarProgress->setVisible(true);
#endif

	// cw->cbPageSelector->setCurrentIndex(0);
	_control_tab->setCurrentIndex(0);
}

void Blogger::submitWPEdit()
{
	QDomDocument doc;
	QDomElement methodCall, params, param, member, value, integer, string, rpcstruct, rpcarray, actualValue, primCatElem, taxStruct, taxValue, taxMember, taxMemberName;
	QString primCat, blogid, entryText, techTagString, convertedString, keywordTagList, tblist;
	bool takeComms = _control_tab->chAllowComments->isChecked();
	bool takeTB = _control_tab->chAllowTB->isChecked();
	// bool blogidIsInt;
	int count, tags;
	QList<QString> catList, tagList;
	if (!_current_http_business) {
		entryText = _editor->toPlainText();
		// Do Markdown, if enabled
		if (doMarkdownWhenPosting) {
			convertedString = processWithMarkdown(entryText);
			if (!convertedString.isNull())
				/* description = stripParaTags ?
											       convertedString.remove( "<p>" ).remove(
											       "</p>" ) : convertedString; */
				entryText = convertedString;
			else
				_status_widget->showMessage(
				    tr("Markdown conversion failed; posting main entry as is."), 2000);
		}
		dateTime = QDateTime::currentDateTime().toString(Qt::ISODate);
		dateTime.remove(QChar('-'));
		// Make a string of the Technorati tags
		if (_control_tab->lwTags->count()) {
			tags = _control_tab->lwTags->count();
			techTagString =
			    "<p style=\"text-align:right;font-size:10px;\">Technorati Tags: ";
			for (count = 0; count < tags; count++) {
				techTagString.append(
				    QString(
					"<a href=\"http://technorati.com/tag/%1\" rel=\"tag\">%2</a>%3")
					.arg(
					    _control_tab->lwTags->item(count)->text().replace(' ', '+'))
					.arg(
					    _control_tab->lwTags->item(count)->text().replace("+", " "))
					.arg((count == tags - 1) ? "</p>\n\n" : ", "));
			}
			// Whether to put the Technorati tags at beginning or end
			if (_control_tab->rbStartOfMainEntry->isChecked())
				entryText.insert(0, techTagString);
			else
				entryText.append(techTagString);
		}
		// Make a list of the selected categories (WP API has no "primary category")
		if (_control_tab->chAllowComments->isChecked()) {
			/*primCatElem = currentBlogElement.firstChildElement( "categories" )
									.childNodes().at(
			   cw->cbMainCat->currentIndex()
									 ).toElement();
			 */
			catList << _control_tab->cbMainCat
				       ->itemData(_control_tab->cbMainCat->currentIndex())
				       .toString();
			for (int a = 0; a < _control_tab->lwOtherCats->count(); a++) {
				if (_control_tab->lwOtherCats->isItemSelected(
					_control_tab->lwOtherCats->item(a))) {
					// cat = currentBlogElement.firstChildElement(
					// "categories" ).childNodes().at( a ).toElement();
					catList << _control_tab->cbMainCat->itemData(a).toString();
				}
			}
		}
		// Make a list of selected WP tags
		if (_control_tab->lwKeywordTags->count())
			for (count = 0; count < _control_tab->lwKeywordTags->count(); count++) tagList << _control_tab->lwKeywordTags->item(count)->text();
		// Make a list of trackback pings to be sent
		if (_control_tab->lwTBPings->count()) {
			for (count = 0; count < _control_tab->lwTBPings->count(); count++) {
				if (count) tblist.append(' ');
				tblist.append(_control_tab->lwTBPings->item(count)->text());
			}
		}
		blogid = _control_tab->cbBlogSelector
			     ->itemData(_control_tab->cbBlogSelector->currentIndex())
			     .toString();

		methodCall = doc.createElement("methodCall");
		methodCall.appendChild(XmlMethodName(doc, "wp.editPost"));

		params = doc.createElement("params");
		params.appendChild(XmlValue(doc, "int", blogid));
		params.appendChild(XmlValue(doc, "string", login));
		params.appendChild(XmlValue(doc, "string", password));
		params.appendChild(XmlValue(doc, "int", entryNumber));

		param = doc.createElement("param");
		value = doc.createElement("value");
		rpcstruct = doc.createElement("struct");
		rpcstruct.appendChild(XmlMember(doc, "post_status", "string", _control_tab->cbStatus->currentIndex() ? "publish" : "draft"));
		rpcstruct.appendChild(
		    XmlMember(doc, "post_title", "string", _control_tab->leTitle->text()));
		rpcstruct.appendChild(XmlMember(doc, "post_content", "string", entryText));
		if (postDateTime) rpcstruct.appendChild(
		    XmlMember(doc, "post_date", "dateTime.iso8601", dateTime));
		rpcstruct.appendChild(XmlMember(doc, "comment_status", "string", takeComms ? "open" : "closed"));
		rpcstruct.appendChild(
		    XmlMember(doc, "ping_status", "string", takeTB ? "open" : "closed"));
		rpcstruct.appendChild(XmlMember(doc, "post_password", "string", _control_tab->lePassword->text()));
		/*rpcstruct.appendChild( XmlMember( doc, "sticky", "bool",
						  cw->chSticky->isChecked() ?
		   "true" : "false" ) ); */
		if (catList.count()) {
			taxMember = doc.createElement("member");
			taxMemberName = doc.createElement("name");
			taxMemberName.appendChild(QDomText(doc.createTextNode("terms")));
			taxMember.appendChild(taxMemberName);
			taxValue = doc.createElement("value");
			taxStruct = doc.createElement("struct");
			taxStruct.appendChild(XmlRpcArray(doc, "category", "string", catList));
			taxValue.appendChild(taxStruct);
			taxMember.appendChild(taxValue);
			rpcstruct.appendChild(taxMember);
		}
		if (tagList.count()) rpcstruct.appendChild(XmlRpcArray(doc, "tags_input", "string", tagList));
		if (tblist.length()) rpcstruct.appendChild(XmlMember(doc, "to_ping", "string", tblist));
		value.appendChild(rpcstruct);
		param.appendChild(value);
		params.appendChild(param);
		methodCall.appendChild(params);
		doc.appendChild(methodCall);
		doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

		QByteArray requestArray(doc.toByteArray());
		responseData = "";
		placeNetworkRequest(_wp_editPost, requestArray);
		if (QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		if (postAsSave && !_entry_ever_saved) {
			_clean_save = true;
			setDirtySignals(true);
		}
		_control_tab->progressBar->setMaximum(2);
		_control_tab->progressBar->setValue(1);
		_control_tab->progressWidget->setVisible(true);
		// cw->cbPageSelector->setCurrentIndex(0);
		_control_tab->setCurrentIndex(0);
	} else {
#ifdef QTM_DEBUG
		statusWidget->showMessage(
		    tr("refreshCategories: All HTTP requests are blocked"), 2000);
#else
		_status_widget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
	}
}

void Blogger::updatePostCategories()
{
	if (!entryNumber.isEmpty())
		setPostCategories();
	else
		_status_widget->showMessage(tr("This entry has not been posted yet."), 2000);
}

void Blogger::setPostCategories()
{
	QDomDocument doc;
	QString secCatId, secCatName;
	QDomElement cat, methodCall, params, param, value, array, _data, arrayValue, arrayStruct;
	// #ifndef NO_DEBUG_OUTPUT
	// qDebug() << "starting to post categories";
	// #endif
	if (categoriesEnabled) {
		// qDebug() << "categories enabled";
		if (!_current_http_business) {
			// qDebug() << "no biz";
			methodCall = doc.createElement("methodCall");
			methodCall.appendChild(XmlMethodName(doc, "mt.setPostCategories"));
			params = doc.createElement("params");
			params.appendChild(XmlValue(doc, "string", entryNumber));
			params.appendChild(XmlValue(doc, "string", login));
			params.appendChild(XmlValue(doc, "string", password));

			param = doc.createElement("param");
			value = doc.createElement("value");
			array = doc.createElement("array");
			_data = doc.createElement("data");
			arrayValue = doc.createElement("value");
			arrayStruct = doc.createElement("struct");
			if (!_control_tab->chNoCats->isChecked()) {
#ifndef NO_DEBUG_OUTPUT
// qDebug() << "posting prim cat";
#endif
				cat = currentBlogElement.firstChildElement("categories")
					  .childNodes()
					  .at(_control_tab->cbMainCat->currentIndex())
					  .toElement();
				QString primCat =
				    _control_tab->cbMainCat
					->itemData(_control_tab->cbMainCat->currentIndex())
					.toString();
#ifndef NO_DEBUG_OUTPUT
// qDebug() << "posted prim cat";
#endif
				arrayStruct.appendChild(
				    XmlMember(doc, "categoryId", "string", primCat.trimmed()));
				arrayStruct.appendChild(XmlMember(doc, "isPrimary", "boolean", "1"));
				arrayValue.appendChild(arrayStruct);
				_data.appendChild(arrayValue);
				for (int a = 0; a < _control_tab->lwOtherCats->count(); a++) {
					if (_control_tab->lwOtherCats->isItemSelected(
						_control_tab->lwOtherCats->item(a))) {
						cat = currentBlogElement.firstChildElement("categories")
							  .childNodes()
							  .at(a)
							  .toElement();
						secCatId = _control_tab->cbMainCat->itemData(a).toString();
						secCatName = _control_tab->cbMainCat->itemText(a);
						arrayValue = doc.createElement("value");
						arrayStruct = doc.createElement("struct");
						arrayStruct.appendChild(
						    XmlMember(doc, "categoryId", "int", secCatId));
						arrayStruct.appendChild(
						    XmlMember(doc, "categoryName", "string", secCatName));
						arrayStruct.appendChild(
						    XmlMember(doc, "isPrimary", "boolean", "0"));
						arrayValue.appendChild(arrayStruct);
						_data.appendChild(arrayValue);
					}
				}
			}
			array.appendChild(_data);
			value.appendChild(array);
			param.appendChild(value);
			params.appendChild(param);
			methodCall.appendChild(params);
			doc.appendChild(methodCall);
			doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());
			QByteArray requestArray(doc.toByteArray(2));

			responseData = "";
			placeNetworkRequest(_mt_setPostCategories, requestArray);
			// if( progressBarAction->isVisible() )
			if (_control_tab->progressBar->isVisible()) _control_tab->progressBar->setValue(_control_tab->progressBar->value() + 1);
#ifdef WIN_QTV
			if (taskbarProgress->isVisible()) taskbarProgress->setValue(taskbarProgress->value() + 1);
#endif
			if (QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		} else {
#ifdef QTM_DEBUG
			statusWidget->showMessage(
			    tr("setPostCategories: All HTTP requests are blocked"), 2000);
#else
			_status_widget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
			QApplication::restoreOverrideCursor();
		}
	} else {
		// An override cursor might have been set when posting an entry in a
		// non-category-enabled blog
		qDebug() << "categories not enabled";
		if (QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
	}
}

void Blogger::publishPost()
{ // slot
	QDomDocument doc;
	if (!_current_http_business) {
		QDomElement methodCall = doc.createElement("methodCall");
		methodCall.appendChild(XmlMethodName(doc, "mt.publishPost"));
		QDomElement params = doc.createElement("params");

		params.appendChild(XmlValue(doc, "string", entryNumber));
		params.appendChild(XmlValue(doc, "string", login));
		params.appendChild(XmlValue(doc, "string", password));
		methodCall.appendChild(params);
		doc.appendChild(methodCall);
		doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

		QByteArray requestArray(doc.toByteArray());

		responseData = "";
		placeNetworkRequest(_mt_publishPost, requestArray);
		// if( progressBarAction->isVisible() )
		if (_control_tab->progressBar->isVisible()) _control_tab->progressBar->setValue(_control_tab->progressBar->value() + 1);
#ifdef WIN_QTV
		if (taskbarProgress->isVisible()) taskbarProgress->setValue(taskbarProgress->value() + 1);
#endif
	} else
		_status_widget->showMessage(tr("Cannot publish; HTTP is blocked"), 2000);
}

void Blogger::exportEntry()
{
	saveAs(true);
}

void Blogger::topic(const QString& topic_)
{
	if (topic_ != _control_tab->topic()) _control_tab->topic(topic_);
}

QString Blogger::topic() const
{
	return _control_tab->topic();
}

void Blogger::title(const QString& title_)
{
	if (title_ != _control_tab->title()) _control_tab->title(title_);
}

QString Blogger::title() const
{
	return _control_tab->title();
}

void Blogger::on_topic_changed(const QString& tp)
{
	if (_control_tab && tp != _current_topic_name) {
		//		QLineEdit* lineedit_topic_ = _control_tab->lineedit_topic();
		auto topic = tp;
		// deal with folder name change
		const auto original_topic_ = _current_topic_name;
		auto original_plain_text = _editor->toPlainText();
		QString original_topic_folder = _current_topic_full_folder_name;
		QString new_topic = purify(topic); // tr(topic.remove(QRegExp("[\"/\\\\<>\\?:\\*\\|]+")).trimmed().toStdString().c_str());
		QString dest_topic_folder = gl_paras->editors_shared_full_path_name() + "/" + new_topic;
		auto point_to_folder = [&](const QString& dest_topic_folder_, bool append_mode = false) -> void {
			_current_topic_name = new_topic;
			_current_topic_full_folder_name = dest_topic_folder_;
			_current_topic_full_config_name = _current_topic_full_folder_name + "/" + gl_para::_editor_conf_filename;
			_topic_editor_config = [&]() -> std::unique_ptr<QSettings> {
				if (!QDir(_current_topic_full_folder_name).exists())
					if (!QDir::root().mkpath(_current_topic_full_folder_name)) critical_error("Can not create directory: \"" + _current_topic_full_folder_name + "\"");
				if (!QFile(_current_topic_full_config_name).exists())
					if (!QFile::copy(QString(":/resource/standardconfig/") + gl_paras->target_os() + "/" + ::gl_para::_editor_conf_filename, _current_topic_full_config_name)) critical_error(QString("Can not copy \"") + ::gl_para::_editor_conf_filename + "\""); // throw std::runtime_error("Can not copy document.ini");
				if ((QFile::ReadUser | QFile::WriteUser) !=
				    (QFile::permissions(_current_topic_full_config_name) &
					(QFile::ReadUser | QFile::WriteUser))) QFile::setPermissions(_current_topic_full_config_name, QFile::ReadUser | QFile::WriteUser);
				return std::make_unique<QSettings>(_current_topic_full_config_name, QSettings::IniFormat);
			}();
			_local_storage_file_extension = _topic_editor_config->value("local_storage_file_ext", "cqt").toString();
			if (new_topic != _control_tab->topic()) _control_tab->topic(new_topic);
			_editor->work_directory(_current_topic_full_folder_name, QString(_default_filename) + ".html");

			_filename = _current_topic_full_folder_name + "/" + _default_filename + "." + _local_storage_file_extension;

			_entry_ever_saved = false;
			emit topic_changed(original_topic_, new_topic, append_mode);
		};
		//
		//		auto get_text = [&](const QString& filename_) {
		//			QFile f(filename_);
		//			QFileInfo fileInfo(f);
		//			if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::ReadOnly))
		//				critical_error(
		//				    "Editor::file_name(QString _file_name) : Can\'t open text file " +
		//				    filename_ + " for read / write.");
		//			return QString::fromUtf8(f.readAll());
		//		};
		//
		if (original_topic_folder != dest_topic_folder) {
			if (original_topic_folder == gl_paras->editors_shared_full_path_name() + "/undefined") {
				point_to_folder(dest_topic_folder);
				//				if (QDir(original_topic_folder).exists())
				//					if (!QDir(original_topic_folder).removeRecursively()) critical_error("Can\'t remove folder \"" + original_topic_folder + "\"");
			} else {
				QDir dir;
				if (!QDir(dest_topic_folder).exists()) {
					if (!dir.rename(original_topic_folder, dest_topic_folder))
						critical_error("Move folder \"" + original_topic_folder + "\" to folder \"" + dest_topic_folder + "\" failed");
					else {
						point_to_folder(dest_topic_folder);
						// load(_filename);
						//						assert(!QDir(original_topic_folder).exists());//might opened by another browser
					}
				} else {
					//					auto original_text = get_text(original_topic_folder + "/" + _default_filename + ".html");
					//					auto current_plain_text = get_text(dest_topic_folder + "/" + _default_filename + ".html");
					auto current_plain_text = _editor->toPlainText();
					QString new_content = current_plain_text.contains(original_plain_text) ? current_plain_text : (original_plain_text.contains(current_plain_text) ? original_plain_text : "");
					if ("" == new_content) {
						QString long_one;
						int length = (original_plain_text.length() < current_plain_text.length()) ? [&] {
							long_one = current_plain_text;
							return original_plain_text.length();
						}() :
															    [&] {
																    long_one = original_plain_text;
																    return current_plain_text.length();
															    }();
						int same_count = 0;
						for (int i = 0; i < length; i++) {
							if (original_plain_text.at(i) == current_plain_text.at(i)) {
								new_content += original_plain_text.at(i);
								same_count++;
							}
						}
						new_content += long_one.right(long_one.length() - same_count);
					}
					_editor->setPlainText(new_content);
					point_to_folder(dest_topic_folder, true);
					if (QDir(original_topic_folder).exists())
						if (!QDir(original_topic_folder).removeRecursively()) critical_error("Can\'t remove folder \"" + original_topic_folder + "\"");
				}
			}
			//			if (_record_screen) _record_screen->tabbar_topic(new_topic);
		}
	}
}

void Blogger::save_impl(const QString& file_name_with_full_path, bool exp)
{
	// save_impl = [&](const QString &fname, bool exp = false){
	if (!_entry_ever_saved) {
		int count, tags;
#ifdef LOAD_TEXT_BY_WYEDITOR
#else
		QString text = editor_object->document()->toPlainText();
#endif // LOAD_TEXT_BY_WYEDITOR
		//		auto file_exists =
		//find_folder_recursively(file_name_with_full_path);
		//		if(file_exists != "")
		//			if(!QFile::remove(file_exists))	critical_error("Can\'t remove file
		//\"" + file_exists + "\"");
		if ("" != file_name_with_full_path) {
			// Rename the old file to a back-up file
			if (QFile::exists(file_name_with_full_path)) {
				auto backup_file_name = QString("%1.backup").arg(file_name_with_full_path);
				QFile backup(backup_file_name);
				if (backup.exists())
					if (!QFile::remove(backup_file_name)) critical_error("Can\'t remove file \"" + backup_file_name + "\"");
				if (!QFile::rename(file_name_with_full_path, backup_file_name)) critical_error("Rename file \"" + file_name_with_full_path + "\" to \"" + backup_file_name + "\" failed");
				//				if(QFile::remove(file_name_with_full_path))
				//critical_error("Can\'t remove file \"" + file_name_with_full_path +
				//"\"");
			}
		}
		QFile f(file_name_with_full_path);
		if (!f.open(QIODevice::WriteOnly)) {
			_status_widget->showMessage(
			    tr("Could not write to %1").arg(file_name_with_full_path), 2000);
			return;
		}
		QTextStream out(&f);
		if (useUtf8) out.setCodec(QTextCodec::codecForName("UTF-8"));
		out << (exp ? (program_title_string + " saved blog entry v2.0\n").c_str() : (program_title_string + " saved blog entry v3.0\n").c_str());
		out << QString("Title:%1\n").arg(_control_tab->leTitle->text());
		out << QString("Publish:%1\n")
			   .arg(QString::number(_control_tab->cbStatus->currentIndex()));
		if (entryBlogged) out << QString("EntryNumber:%1\n").arg(entryNumber);
		out << QString("Comments:%1\n")
			   .arg(_control_tab->chAllowComments->isChecked() ? "1" : "0");
		out << QString("TB:%1\n").arg(_control_tab->chAllowTB->isChecked() ? "1" : "0");
		// out << QString( "Sticky:%1\n" ).arg( cw->chSticky->isChecked() ? "1" :
		// "0" );
		if (_control_tab->lePassword->text().length()) out << QString("PWD:%1\n").arg(_control_tab->lePassword->text());
		if (exp) {
			out << QString("Server:%1\n").arg(server);
			out << QString("Location:%1\n").arg(location);
			out << QString("Login:%1\n").arg(login);
			out << QString("Password:%1\n").arg(password);
			out << QString("Blog:%1\n")
				   .arg(_control_tab->cbBlogSelector->currentIndex());
		} else {
			out << QString(
				   "AcctBlog:%1@%2 (%3)\n") // Include the blog name so it can be relayed to the user later
				   .arg(currentBlogid)
				   .arg(currentAccountId)
				   .arg(_control_tab->cbBlogSelector->itemText(_control_tab->cbBlogSelector->currentIndex()));
		}
		tags = _control_tab->lwTags->count();
		if (tags) {
			out << "Tags:";
			for (count = 0; count < tags; count++) out << QString(count ? ";%1" : "%1")
									  .arg(_control_tab->lwTags->item(count)->text().replace(' ', '+'));
			out << "\n";
		}
		tags = _control_tab->lwKeywordTags->count();
		if (tags) {
			out << "Keywords:";
			for (count = 0; count < tags; count++) out << QString(count ? ",%1" : "%1")
									  .arg(_control_tab->lwKeywordTags->item(count)->text());
			out << "\n";
		}
		if (!_control_tab->chNoCats->isChecked()) {
			QDomNodeList catNodeList =
			    currentBlogElement.firstChildElement("categories")
				.elementsByTagName("category");
			out << QString("PrimaryID:%1\n")
				   .arg(_control_tab->cbMainCat->itemData(_control_tab->cbMainCat->currentIndex()).toString());
			QString catsList;
			int cats = 0;
			for (int a = 0; a < _control_tab->lwOtherCats->count(); a++) {
				if (_control_tab->lwOtherCats->isItemSelected(
					_control_tab->lwOtherCats->item(a))) {
					if (cats)
						catsList.append(QString(";%1").arg(_control_tab->cbMainCat->itemData(a).toString()));
					else
						catsList.append(_control_tab->cbMainCat->itemData(a).toString());
					cats++;
				}
			}
			out << QString("CatIDs:%1\n").arg(catsList);
		} else
			out << "PrimaryID:none\n";
		if (_control_tab->teExcerpt->toPlainText().length() > 0) out << QString("Excerpt:%1\n").arg(_control_tab->teExcerpt->toPlainText().replace(QChar('\n'), "\\n"));
#ifdef LOAD_TEXT_BY_WYEDITOR
		_editor->save_textarea();
#else
		out << QString("Text:\n%1").arg(text);
#endif // LOAD_TEXT_BY_WYEDITOR
		f.close();

		_dirty_indicator->hide();
		setWindowModified(false);
		setDirtySignals(true);

		_entry_ever_saved = true;
		// }else critical_error("file \"" + full_path_file_name + "\" already
		// exists");
		if (_browser) _browser->save();
	}
}

void Blogger::saveAccountsDom()
{
	purgeBlankAccounts();

	QFile domOut(PROPERSEPS(QString("%1/qtmaccounts2.xml").arg(gl_paras->editors_shared_full_path_name())));
	if (domOut.open(QIODevice::WriteOnly)) {
		QTextStream domFileStream(&domOut);
		_accounts_dom.save(domFileStream, 2);
		domOut.close();
	} else {
		QDir dir(gl_paras->editors_shared_full_path_name());
		if (!dir.exists()) {
			if (QMessageBox::question(
				0, tr("Cannot find storage directory"), tr((program_title_string +
									       " cannot find the directory you specified to " +
									       "store your account data and files.\n\n" + "Create it?")
										.c_str()),
				QMessageBox::Yes | QMessageBox::Default, QMessageBox::No) == QMessageBox::Yes) {
				if (dir.mkpath(gl_paras->editors_shared_full_path_name())) {
					domOut.setFileName(
					    PROPERSEPS(QString("%1/qtmaccounts2.xml").arg(gl_paras->editors_shared_full_path_name())));
					if (domOut.open(QIODevice::WriteOnly)) {
						QTextStream dfs(&domOut);
						_accounts_dom.save(dfs, 2);
						domOut.close();
					} else
						_status_widget->showMessage(tr("Could not write to accounts file."), 2000);
				} else
					_status_widget->showMessage(tr("Could not create the directory."), 2000);
			}
		}
	}
}

//void Blogger::saveAll()
//{
//	sapp_t::instance()->saveSession();
//}

void Blogger::saveAs(bool exp)
{
	QString suggestedFilename;
	if (_control_tab->leTitle->text().isEmpty())
		suggestedFilename = gl_paras->editors_shared_full_path_name();
	else {
		suggestedFilename = QString("%1/%2")
					.arg(gl_paras->editors_shared_full_path_name())
					.arg(_control_tab->leTitle->text().remove(QRegExp("[\"/\\\\<>\\?:\\*\\|]+")));
	}
	QString extn = QString("%1 (*.%2)")
			   .arg(tr("Blog entries"))
			   .arg(_local_storage_file_extension);
	QString fn = QFileDialog::getSaveFileName(this, tr("Choose a filename"), suggestedFilename, extn);
	if (!fn.isEmpty()) {
		if (!_local_storage_file_extension.isEmpty())
			if (!fn.endsWith(QString(".%1").arg(_local_storage_file_extension))) fn.append(QString(".%1").arg(_local_storage_file_extension));
		_filename = fn;
		// This section has been commented out as unnecessary; the
		// getSaveFileName function
		// already checks for the existence of a file.  Keeping in code
		// until tested on other platforms.
		// if(QFile::exists(fn)){
		// if(!QMessageBox::warning(0, tr("File exists")
		// , tr("A file by this name already exists. Continue?")
		// , QMessageBox::Yes, QMessageBox::No | QMessageBox::Default)){
		// QFile::remove(fn);
		// save(fn);
		// }
		// }else
		save_impl(fn, exp);
		sapp_t::instance()->add_recent_file(_control_tab->leTitle->text(), fn);
	} else
		_status_widget->showMessage(tr("Saving aborted"), 2000);
}

void Blogger::save()
{
	auto file_name_with_full_path = this->_current_topic_full_folder_name + "/" + _default_filename + "." + _local_storage_file_extension;
	if (!_no_auto_save)
		save_impl(file_name_with_full_path);
	else {
		if (_filename.isEmpty()) {
			saveAs();
			return;
		}
		save_impl(_filename);
	}
}

// QString EditingWindow::find_file_name_in_current_folder(const QString
// &file_name_with_full_path){
//	// bool found	= false;
//	// QString id = global_root_id;
//	QString result = "";
//	QDirIterator it(_editors_shared_directory, QStringList() << "*." +
//_topic_editor_config->value("local_storage_file_ext").toString(), QDir::Files,
//QDirIterator::Subdirectories);
//	while(it.hasNext()){
//		auto item = it.next();
//		if(item.contains(file_name_with_full_path)){
//			// found = true;
//			result = item;
//			break;
//		}
//	}
//	return result;
//}

void Blogger::choose(QString fname)
{
	QString fn;
	QString extn(QString("%1 (*.%2)").arg(tr("Blog entries")).arg(_local_storage_file_extension));
	if (fname.isEmpty())
		fn = QFileDialog::getOpenFileName(this, tr("Choose a file to open"), gl_paras->editors_shared_full_path_name(), extn);
	else
		fn = fname;
	if (!fn.isEmpty()) {
		if (!useNewWindows) {
			if (saveCheck(true)) {
				if (!load(fn))
					_status_widget->showMessage(tr("File could not be loaded."), 2000);
				else {
					sapp_t::instance()->add_recent_file(_control_tab->leTitle->text(), fn);

					_dirty_indicator->hide();
					setWindowModified(false);
					setDirtySignals(true);
				}
			}
		} else {
			blogger_ref e(new Blogger());
			if (e->load(fn, true)) {
#ifdef USE_SYSTRAYICON
				e->setSTI(sti);
#endif

				positionWidget(e, this);
				sapp_t::instance()->add_recent_file(e->postTitle(), fn);

				e->show();
			} else {
				_status_widget->showMessage("Loading of new window failed", 2000);
				e->deleteLater();
			}
		}
	} else
		_status_widget->showMessage(tr("Loading aborted"), 2000);
}

// bool EditingWindow::load(const QString &fname, QDomDocument &dd){
// _accounts_dom = dd;

// return load(fname);
// }

bool Blogger::load(const QString& fname, bool fromSTI)
{
	Ui::dPassword pui;
	addToConsole("Starting load");
	QMap<QString, QString> emap;
	QString currentLine, key, value, fetchedText, tags, keywords;
	QString catName;
	QStringList otherCatStringList;
	QDomNodeList accts;
	// bool getDetailsAgain = false;
	bool isOK;
	int b, c, d, g, h, hh;
	_no_auto_save = true;
	QFile f(fname);
	QDomElement details;
	QDomProcessingInstruction testPI;
	QDomElement testAccountsElement;
	if (_super_menu) {
		_super_menu->setEnabled(true);
#ifndef Q_OS_MAC
		_super_menu->setVisible(true);
#endif
	}
	if (fromSTI) {
		testPI = _accounts_dom.firstChild().toProcessingInstruction();
		testAccountsElement =
		    _accounts_dom.firstChildElement(program_title_qstring + "Accounts");
		if (testAccountsElement.isNull())
			accountsElement =
			    _accounts_dom.createElement(program_title_qstring + "Accounts");
		else
			accountsElement =
			    _accounts_dom.firstChildElement(program_title_qstring + "Accounts");
		currentAccountElement = _accounts_dom.createElement("account");
		accountsElement.appendChild(currentAccountElement);
		if (testAccountsElement.isNull()) _accounts_dom.appendChild(accountsElement);
		if (testPI.isNull()) _accounts_dom.insertBefore(
		    _accounts_dom.createProcessingInstruction("xml", "version=\"1.0\""), _accounts_dom.firstChild());
	}
	if (!f.open(QIODevice::ReadOnly)) {
		addToConsole("Cannot open file");
		_status_widget->showMessage(tr("Cannot open file."), 2000);
		return false;
	}
	QTextStream t(&f);
	if (useUtf8) t.setCodec(QTextCodec::codecForName("UTF-8"));
	if (!t.readLine().startsWith(program_title_qstring + " saved blog entry")) {
		addToConsole("Not a " + program_title_qstring + " blog entry");
		_status_widget->showMessage(
		    tr(("This is not a " + program_title_string + " blog entry.").c_str()), 2000);
		return false;
	}
	do {
		currentLine = t.readLine();
		if (currentLine.startsWith("Text:")) break;
		key = currentLine.section(QChar(':'), 0, 0);
		value = currentLine.section(QChar(':'), 1);
		emap[key] = value;
		addToConsole(currentLine);
	} while (!t.atEnd());
	if (emap.contains("Title")) {
		_control_tab->leTitle->setText(emap.value("Title"));
		setWindowTitle(QString("%1 - QTM [*]").arg(emap.value("Title")));
	}
	if (emap.contains("Publish")) _control_tab->cbStatus->setCurrentIndex(emap.value("Publish") == "1" ? 1 : 0);
	if (emap.contains("EntryNumber")) {
		entryNumber = emap.value("EntryNumber");
		entryBlogged = true;
	}
	if (emap.contains("Comments")) _control_tab->chAllowComments->setChecked(emap.value("Comments") == "1" ? true : false);
	if (emap.contains("TB")) _control_tab->chAllowTB->setChecked(emap.value("TB") == "1" ? true : false);
	/* if( emap.contains( "Sticky" ) )
			 cw->chSticky->setChecked( emap.value( "Sticky" ) == "1"
	   ? true : false ); */
	if (emap.contains("PWD"))
		_control_tab->lePassword->setText(emap.value("PWD"));
	else
		_control_tab->lePassword->clear();
	if (emap.contains("Server")) {
		if (emap.value("Server") != server) {
			server = emap.value("Server");
			// getDetailsAgain = true;
		}
	}
	if (emap.contains("Location")) {
		if (emap.value("Location") != location) {
			location = emap.value("Location");
			// getDetailsAgain = true;
		}
	}
	if (emap.contains("Login")) {
		if (emap.value("Login") != login) {
			login = emap.value("Login");
			// getDetailsAgain = true;
		}
	}
	if (emap.contains("Password")) {
		if (emap.value("Password") != password) {
			password = emap.value("Password");
			// getDetailsAgain = true;
		}
		noPassword = false;
	} else {
		password = "";
		noPassword = true;
	}
	if (emap.contains("Tags")) {
		tags = emap.value("Tags");
		d = tags.count(QChar(';'));
		if (d)
			for (c = 0; c <= d; c++) _control_tab->lwTags->addItem(tags.section(QChar(';'), c, c));
		else
			_control_tab->lwTags->addItem(tags);
	}
	_control_tab->lwKeywordTags->clear();
	if (emap.contains("Keywords")) {
		keywords = emap.value("Keywords");
		d = keywords.count(QChar(','));
		if (d)
			for (c = 0; c <= d; c++) _control_tab->lwKeywordTags->addItem(
			    keywords.section(QChar(','), c, c));
		else
			_control_tab->lwKeywordTags->addItem(keywords);
	}
	if (emap.contains("Blog")) {
		b = emap.value("Blog").toInt(&isOK);
		if (isOK) {
			currentBlog = b;
			loadedEntryBlog = b;
		}
	}
	if (emap.contains("AcctBlog")) {
		currentBlogid = emap.value("AcctBlog").section("@", 0, 0);
		loadedAccountId =
		    emap.value("AcctBlog").section("@", 1, 1).section(" (", 0, 0);
		b = emap.value("AcctBlog").section("@", 0, 0).toInt(&isOK);
		if (isOK) {
			currentBlog = b;
			loadedEntryBlog = b;
		}
	}
	// Get old-style index categories
	if (emap.contains("Primary")) {
		b = emap.value("Primary").toInt(&isOK);
		if (isOK) {
			primaryCat = b;
			_no_alpha_cats = true;
			// getDetailsAgain = true;
		} else {
			primaryCat = 0;
			addToConsole(tr("Assignation of primary category failed"));
		}
	}
	if (emap.contains("Cats")) {
		otherCatsList = emap.value("Cats");
		otherCatStringList = otherCatsList.split(';');
		_no_alpha_cats = true;
		// getDetailsAgain = true;
	}
	// Get new-style ID categories
	if (emap.contains("PrimaryID")) {
		b = emap.value("PrimaryID").toInt(&isOK);
		if (isOK)
			primaryCat = b;
		else {
			primaryCat = 0;
			addToConsole(tr("Assignation of primary category failed"));
		}
	}
	if (emap.contains("CatIDs")) {
		otherCatsList = emap.value("CatIDs");
		otherCatStringList = otherCatsList.split(';');
	}
	if (emap.contains("Excerpt")) _control_tab->teExcerpt->setPlainText(
	    QString(emap.value("Excerpt")).replace("\\n", "\n"));
#ifdef LOAD_TEXT_BY_WYEDITOR
	_editor->load_textarea();
#else
	while (!t.atEnd())
		fetchedText += QString("%1\n").arg(t.readLine());
	editor_object->setPlainText(fetchedText);

#endif
	f.close();
	setDirtySignals(true);
	_dirty_indicator->hide();
	setWindowModified(false);
	// First of all, deal with entries saved to accounts
	if (!loadedAccountId.isNull()) {
		accts = _accounts_dom.elementsByTagName("account");
		qDebug() << accts.count() << "accounts found";
		for (g = 0; g <= accts.count(); g++) {
			if (g == accts.count()) {
				// qDebug() << accts.count() << "accounts";
				// i.e. if it gets to the end of the accounts tree without
				// finding the account
				QMessageBox::information(
				    0, tr((program_title_string + " - No such account").c_str()), tr((program_title_string + " could not find this account (perhaps it was deleted).\n\nWill set up a blank default account; you will need to fill in the access details by choosing Accounts from the File menu.").c_str()), QMessageBox::Ok);
				QDomElement newDefaultAccount = _accounts_dom.createElement("account");
				newDefaultAccount.setAttribute("id", QString("newAccount_%1").arg(QDateTime::currentDateTime().toString(Qt::ISODate)));
				QDomElement newDetailElement = _accounts_dom.createElement("details");
				QDomElement newNameElement = _accounts_dom.createElement("title");
				newNameElement.appendChild(QDomText(_accounts_dom.createTextNode(tr("New blank element"))));
				newDetailElement.appendChild(newNameElement);
				newDefaultAccount.appendChild(newDetailElement);
				_accounts_dom.firstChildElement("accounts").appendChild(newDefaultAccount);
				currentAccountElement = newDefaultAccount;
				_control_tab->cbAccountSelector->addItem(
				    tr("New blank element"), newDefaultAccount.attribute("id"));
				_control_tab->cbAccountSelector->setCurrentIndex(
				    _control_tab->cbAccountSelector->count() - 1);
				_control_tab->cbBlogSelector->clear();
				_control_tab->cbBlogSelector->setEnabled(false);
				_control_tab->chNoCats->setEnabled(false);
				_control_tab->cbMainCat->clear();
				_control_tab->cbMainCat->setEnabled(false);
				_control_tab->lwOtherCats->clear();
				_control_tab->lwOtherCats->setEnabled(false);
				setPostClean();
				return true;
			}
			if (accts.at(g).toElement().attribute("id") == loadedAccountId) {
				// qDebug() << "found the account:" << loadedAccountId;
				populateAccountList();
				currentAccountElement = accts.at(g).toElement();
				extractAccountDetails();

				QString st;
				for (h = 0; h < _control_tab->cbAccountSelector->count(); h++) {
					st = _control_tab->cbAccountSelector->itemData(h).toString();
					if (st == loadedAccountId) _control_tab->cbAccountSelector->setCurrentIndex(h);
				}
				// Now populate the blog list
				QDomNodeList blogNodeList =
				    currentAccountElement.elementsByTagName("blog");
				_control_tab->cbBlogSelector->clear();
				for (hh = 0; hh < blogNodeList.count(); hh++) {
					_control_tab->cbBlogSelector->addItem(
					    decodeXmlEntities(blogNodeList.at(hh).toElement().firstChildElement("blogName").text()), blogNodeList.at(hh)
																	 .toElement()
																	 .firstChildElement("blogid")
																	 .text());
					if (blogNodeList.at(hh).firstChildElement("blogid").text() ==
					    currentBlogid) currentBlogElement = blogNodeList.at(hh).toElement();
				}
				_control_tab->cbBlogSelector->disconnect(SIGNAL(activated(int)), this, 0);
				// qDebug() << "connecting changeBlog";
				connect(_control_tab->cbBlogSelector, SIGNAL(activated(int)), this, SLOT(changeBlog(int)));
				if (!isOK) {
					QMessageBox::information(
					    0, tr((program_title_string + " - Invalid blog").c_str()), tr("Could not get a valid blog number. Please set it again."), QMessageBox::Ok);
					return true;
				}
				for (hh = 0; hh < _control_tab->cbBlogSelector->count(); hh++) {
					if (_control_tab->cbBlogSelector->itemData(hh) == currentBlogid) {
						_control_tab->cbBlogSelector->setCurrentIndex(hh);
						break;
					}
				}
				_control_tab->lwAvailKeywordTags->clear();
				populateTagList();

				// Now populate and set the categories
				QDomElement catsElement =
				    currentBlogElement.firstChildElement("categories");
				if (!catsElement.isNull()) {
					_control_tab->cbMainCat->clear();
					_control_tab->lwOtherCats->clear();

					QDomNodeList catNodeList = catsElement.elementsByTagName("category");
					int b = catNodeList.count();
					if (b) {
						// qDebug() << "categories:" << b;
						_control_tab->cbMainCat->clear();
						_control_tab->lwOtherCats->clear();
						for (int j = 0; j < b; j++) {
							catName = decodeXmlEntities(
							    catNodeList.at(j).firstChildElement("categoryName").text());
							_control_tab->cbMainCat->addItem(
							    catName, QVariant(catNodeList.at(j).firstChildElement("categoryId").text()));
							_control_tab->lwOtherCats->addItem(catName);
						}
						// qDebug() << "primaryCat is" <<
						// QString::number( primaryCat );
						for (int i = 0; i < catNodeList.size(); i++) {
							QString cc =
							    catNodeList.at(i).firstChildElement("categoryId").text();
							// qDebug() << "cc is" << cc;
							if (cc == QString::number(primaryCat)) {
								// qDebug() << "found the
								// primary category";
								_control_tab->cbMainCat->setCurrentIndex(i);
							} else if (otherCatStringList.contains(cc))
								_control_tab->lwOtherCats->setItemSelected(
								    _control_tab->lwOtherCats->item(i), true);
						}
					} else {
						_control_tab->chNoCats->setEnabled(false);
						_control_tab->cbMainCat->setEnabled(false);
						_control_tab->lwOtherCats->setEnabled(false);
					}
				}
				_filename = fname;
				setPostClean();
				return true;
			}
		}
		_filename = fname;
		setPostClean();
		return true;
	}
	// Now we know this isn't an account entry, check whether the saved details
	// actually
	// belong to an account; if it does, there is no need to check for the
	// password

	// qDebug() << "this is an old-style account";
	QDomNodeList blogs;

	accts = _accounts_dom.documentElement().elementsByTagName("account");
	for (int e = 0; e <= accts.count(); e++) {
		if (e == accts.count()) break;
		details = accts.at(e).toElement().firstChildElement("details");
		// qDebug() << "matching against" << details.firstChildElement( "title"
		// ).text();
		if (details.firstChildElement("server").text() == server &&
		    details.firstChildElement("location").text() == location &&
		    details.firstChildElement("login").text() == login) {
			// qDebug() << "match found";
			currentAccountElement = accts.at(e).toElement();
			_control_tab->cbAccountSelector->setCurrentIndex(e);
			extractAccountDetails();
			// First check whether the blog still exists
			blogs = currentAccountElement.elementsByTagName("blog");
			if (currentBlog > blogs.count()) {
				connect(this, SIGNAL(categoryRefreshFinished()), this, SLOT(setLoadedPostCategories()));
				refreshBlogList();
				setPostClean();
				return true;
			} else {
				// qDebug() << "now setting categories";
				if (blogs.at(currentBlog)
					.toElement()
					.elementsByTagName("category")
					.count()) {
					_control_tab->cbBlogSelector->clear();
					for (hh = 0; hh < blogs.count(); hh++)
						_control_tab->cbBlogSelector->addItem(
						    decodeXmlEntities(
							blogs.at(hh).firstChildElement("blogName").text()),
						    QVariant(blogs.at(hh).firstChildElement("blogid").text()));
					setLoadedPostCategories();
					setPostClean();
					return true;
				} else {
					connect(this, SIGNAL(categoryRefreshFinished()), this, SLOT(setLoadedPostCategories()));
					changeBlog(currentBlog);
					setPostClean();
					return true;
				}
			}
		}
	}
	if (noPassword) {
		QDialog pwd;
		pui.setupUi(&pwd);
		if (pwd.exec())
			password = pui.lePassword->text();
		else
			QMessageBox::warning(0, tr("No password"), tr("This entry was saved without a password.\n"
								      "You will need to set one, using the\n"
								      "Preferences window."),
			    QMessageBox::Ok, QMessageBox::NoButton);
	}
	// This is an old-style account which isn't in the database
	// qDebug() << "old-style, not found";

	QDomElement newAcct, newDetails, newTitle, newServer, newLocation, newLogin, newPwd;
	newAcct = _accounts_dom.createElement("account");
	currentAccountId =
	    tr("newAccount_%1")
		.arg(QDateTime::currentDateTime().toString(Qt::ISODate));
	newAcct.setAttribute("id", currentAccountId);
	newDetails = _accounts_dom.createElement("details");
	newTitle = _accounts_dom.createElement("title");
	newTitle.appendChild(
	    QDomText(_accounts_dom.createTextNode(tr("New un-named account"))));
	newServer = _accounts_dom.createElement("server");
	newServer.appendChild(QDomText(_accounts_dom.createTextNode(server)));
	newLocation = _accounts_dom.createElement("location");
	newLocation.appendChild(QDomText(_accounts_dom.createTextNode(location)));
	newLogin = _accounts_dom.createElement("login");
	newLogin.appendChild(QDomText(_accounts_dom.createTextNode(login)));
	newPwd = _accounts_dom.createElement("password");
	newPwd.appendChild(QDomText(_accounts_dom.createTextNode(password)));
	newDetails.appendChild(newTitle);
	newDetails.appendChild(newServer);
	newDetails.appendChild(newLocation);
	newDetails.appendChild(newLogin);
	newDetails.appendChild(newPwd);
	newAcct.appendChild(newDetails);
	_accounts_dom.documentElement().appendChild(newAcct);
	currentAccountElement = newAcct;
	_control_tab->cbAccountSelector->addItem(newTitle.text(), newAcct.attribute("id"));
	_control_tab->cbAccountSelector->setCurrentIndex(
	    _control_tab->cbAccountSelector->count() - 1);

	connect(this, SIGNAL(categoryRefreshFinished()), this, SLOT(setLoadedPostCategories()));
	refreshBlogList();
	saveAccountsDom();

	_filename = fname;
	_entry_ever_saved = true;
	setPostClean();
	return true;
}

void Blogger::setLoadedPostCategories()
{ // slot
	int a, b, j, z;
	int i = 0;
	QString c, cc, d;
	bool isOK;

	disconnect(SIGNAL(categoryRefreshFinished()));
	disconnect(SIGNAL(httpBusinessFinished()));

	QDomNodeList blogNodes =
	    currentAccountElement.firstChildElement("blogs").elementsByTagName(
		"blog");
	if (!_control_tab->cbBlogSelector->count()) {
		j = blogNodes.size();
		for (z = 0; z < j; z++)
			_control_tab->cbBlogSelector->addItem(
			    decodeXmlEntities(
				blogNodes.at(z).firstChildElement("blogName").text()),
			    QVariant(blogNodes.at(z).firstChildElement("blogName").text()));
		_control_tab->cbBlogSelector->setCurrentIndex(currentBlog);
	}
	currentBlogElement = blogNodes.at(currentBlog).toElement();
	_control_tab->cbBlogSelector->setCurrentIndex(currentBlog);
	_control_tab->cbMainCat->clear();
	_control_tab->lwOtherCats->clear();
	QDomNodeList catNodeList = currentBlogElement.elementsByTagName("category");
	b = catNodeList.count();
	if (b) {
		// qDebug() << "populating cat list";
		_control_tab->cbMainCat->clear();
		_control_tab->lwOtherCats->clear();
		for (j = 0; j < b; j++) {
			_control_tab->cbMainCat->addItem(
			    catNodeList.at(j).firstChildElement("categoryName").text(), QVariant(catNodeList.at(j).firstChildElement("categoryId").text()));
			_control_tab->lwOtherCats->addItem(
			    catNodeList.at(j).firstChildElement("categoryName").text());
		}
	} else {
		// qDebug() << "no categories found";
		connect(this, SIGNAL(categoryRefreshFinished()), this, SLOT(setLoadedPostCategories()));
		return;
	}
	if (_no_alpha_cats) {
		_control_tab->cbMainCat->setCurrentIndex(primaryCat);

		QString f(otherCatsList.section(QChar(';'), -1, -1));
		do {
			c = otherCatsList.section(QChar(';'), i, i);
			a = c.toInt(&isOK);
			if (isOK)
				_control_tab->lwOtherCats->setItemSelected(
				    _control_tab->lwOtherCats->item(a), true);
			else
				break;
			i++;
		} while (c != f);
		initialChangeBlog = false;
	} else {
		QDomNodeList catNodes = currentBlogElement.firstChildElement("categories")
					    .elementsByTagName("category");
		QStringList catStringList = otherCatsList.split(";");
		// qDebug() << "current Blog is" << currentBlogElement.firstChildElement(
		// "blogName" ).text();
		// qDebug() << "primaryCat is" << primaryCat;
		for (i = 0; i < catNodes.size(); i++) {
			cc = catNodes.at(i).firstChildElement("categoryId").text();
			if (cc == QString::number(primaryCat)) {
				// qDebug() << "found primary category";
				_control_tab->cbMainCat->setCurrentIndex(i);
			} else if (catStringList.contains(cc))
				_control_tab->lwOtherCats->setItemSelected(
				    _control_tab->lwOtherCats->item(i), true);
		}
		initialChangeBlog = false;
	}
}

void Blogger::uploadFile()
{
	QString fileInBase64;
	QByteArray conversionBuffer;
	QFile inFile;
	QDomDocument doc;
	if (!_current_http_business) {
		QString uploadFilename = QFileDialog::getOpenFileName(this, tr("Select file to upload"), QDir::homePath());
		if (!uploadFilename.isEmpty()) {
			if (QFile::exists(uploadFilename)) {
				inFile.setFileName(uploadFilename);
				if (inFile.open(QIODevice::ReadOnly)) {
					conversionBuffer = inFile.readAll();
					if (conversionBuffer.isEmpty())
						_status_widget->showMessage(tr("This file was empty, or an error occurred."), 2000);
					else {
						QApplication::processEvents();
						fileInBase64 = toBase64(conversionBuffer);
						if (!fileInBase64.isEmpty()) {
							QDomElement methodCall = doc.createElement("methodCall");
							methodCall.appendChild(XmlMethodName(doc, "metaWeblog.newMediaObject"));
							QDomElement params = doc.createElement("params");

							QString blogid = currentBlogElement.firstChildElement("blogid").text();
							//							QRegExp blogidRegExp("^[0-9]+$");
							//							bool blogidIsInt = blogidRegExp.exactMatch(blogid);

							params.appendChild(XmlValue(doc, "string", blogid));
							params.appendChild(XmlValue(doc, "string", login));
							params.appendChild(XmlValue(doc, "string", password));

							QDomElement param = doc.createElement("param");
							QDomElement value = doc.createElement("value");
							QDomElement rpcStruct = doc.createElement("struct");
							rpcStruct.appendChild(XmlMember(doc, "bits", "base64", fileInBase64));
							rpcStruct.appendChild(XmlMember(doc, "name", "string", QFileInfo(uploadFilename).fileName()));
							value.appendChild(rpcStruct);
							param.appendChild(value);
							params.appendChild(param);

							methodCall.appendChild(params);
							doc.appendChild(methodCall);
							doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

							_current_http_business = _metaWeblog_newMediaObject;

							QByteArray requestArray(doc.toByteArray());
							responseData = "";
							placeNetworkRequest(_metaWeblog_newMediaObject, requestArray);
							if (QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
						} else
							_status_widget->showMessage(tr("Upload was cancelled."), 2000);
					}
					inFile.close();
				} else
					_status_widget->showMessage(tr("Could not open the file."), 2000);
			} else
				_status_widget->showMessage(tr("That file could not be found."), 2000);
		}
	} else
		_status_widget->showMessage(tr("HTTP requests are blocked."), 2000);
}

void Blogger::doWhatsThis()
{
	QWhatsThis::enterWhatsThisMode();
}

void Blogger::doViewBasicSettings()
{
	_control_tab->setCurrentIndex(0); // cw->cbPageSelector->setCurrentIndex(0);
}

void Blogger::doViewCategories()
{
	_control_tab->setCurrentIndex(1); // cw->cbPageSelector->setCurrentIndex(1);
}

void Blogger::changeOtherCatsHeading()
{
	int c = _control_tab->lwOtherCats->selectedItems().count();
	if (c)
		_control_tab->lOtherCats->setText(tr("Others (%1)").arg(c));
	else
		_control_tab->lOtherCats->setText(tr("Others"));
}

void Blogger::doViewExcerpt()
{
	_control_tab->setCurrentIndex(2); // cw->cbPageSelector->setCurrentIndex(2);
	_control_tab->teExcerpt->setFocus();
}

void Blogger::doViewKeywordTags()
{
	_control_tab->setCurrentIndex(3); // cw->cbPageSelector->setCurrentIndex(3);
}

void Blogger::doViewTechTags()
{
	_control_tab->setCurrentIndex(4); // cw->cbPageSelector->setCurrentIndex(4);
}

void Blogger::doViewTBPings()
{
	_control_tab->setCurrentIndex(5); // cw->cbPageSelector->setCurrentIndex(5);
}

// This slot is to refresh categories automatically if there are none when the
// user
// switches to the category page
void Blogger::handleSideWidgetPageSwitch(int index)
{
	QDomNodeList cl;
	QString currentCatName, currentCatId;
	int uncategorized = 0;
	if (index == 1) {
		if (_control_tab->cbMainCat->count() == 0 && _network_actions_enabled &&
		    categoriesEnabled) {
			/*if( currentBlogElement.isNull() )
									qDebug()
			   << "currentBlogElement is null"; */
			cl = currentBlogElement.firstChildElement("categories")
				 .elementsByTagName("category");
			if (cl.count() == 0) {
				_status_widget->showMessage(tr("Getting categories, please wait"), 2000);
				refreshCategories();
			} else {
				_control_tab->cbMainCat->clear();
				_control_tab->lwOtherCats->clear();
				for (int i = 0; i < cl.count(); i++) {
					currentCatName = cl.at(i).firstChildElement("categoryName").text();
					currentCatId = cl.at(i).firstChildElement("categoryId").text();
					_control_tab->cbMainCat->addItem(currentCatName, currentCatId);
					if (currentCatId == "1") uncategorized = i;
					_control_tab->lwOtherCats->addItem(currentCatName);
				}
				if (uncategorized) _control_tab->cbMainCat->setCurrentIndex(uncategorized);
			}
		}
	}
}

void Blogger::newChildCategory()
{
	newCategory(_control_tab->lwOtherCats->currentRow() + 1);
}

void Blogger::newCategory(int parentCategory)
{
	QDomElement catsElement;
	QDomDocument doc;
	QDomElement methodCall, params, param, member, value, string, rpcstruct, actualValue;
	QDomNodeList catNodeList;
	int b, j;
	// This feature uses a Wordpress API call
	if (useWordpressAPI) {
		QDialog newCategoryDialog(this);
		Ui::NewCategoryForm ncui;
		ncui.setupUi(&newCategoryDialog);

		catsElement = currentBlogElement.firstChildElement("categories");
		if (!catsElement.isNull()) {
			catNodeList = catsElement.elementsByTagName("category");
			b = catNodeList.count();
			if (b) {
				ncui.cbParent->clear();
				ncui.cbParent->addItem("(Top level)", QVariant("0"));
				for (j = 0; j < b; j++)
					ncui.cbParent->addItem(
					    catNodeList.at(j).firstChildElement("categoryName").text(), QVariant(
													    catNodeList.at(j).firstChildElement("categoryId").text()));
				ncui.cbParent->setCurrentIndex(parentCategory);
			}
			if (newCategoryDialog.exec()) {
				if (!ncui.leName->text().isEmpty()) {
					if (!_current_http_business) {
						QString blogid =
						    _control_tab->cbBlogSelector
							->itemData(_control_tab->cbBlogSelector->currentIndex())
							.toString();
						// QRegExp
						// blogidRegExp("^[0-9]+$");
						// bool
						// blogidIsInt =
						// blogidRegExp.exactMatch(blogid);

						methodCall = doc.createElement("methodCall");
						methodCall.appendChild(XmlMethodName(doc, "wp.newCategory"));

						params = doc.createElement("params");
						params.appendChild(XmlValue(doc, "string", blogid));
						params.appendChild(XmlValue(doc, "string", login));
						params.appendChild(XmlValue(doc, "string", password));

						param = doc.createElement("param");
						value = doc.createElement("value");
						rpcstruct = doc.createElement("struct");

						rpcstruct.appendChild(
						    XmlMember(doc, "name", "string", ncui.leName->text()));
						rpcstruct.appendChild(XmlMember(doc, "slug", "string", ""));
						rpcstruct.appendChild(
						    XmlMember(doc, "parent_id", "int", ncui.cbParent->itemData(ncui.cbParent->currentIndex()).toString()));
						rpcstruct.appendChild(
						    XmlMember(doc, "description", "string", ncui.teDescription->toPlainText().simplified()));

						value.appendChild(rpcstruct);
						param.appendChild(value);
						params.appendChild(param);

						methodCall.appendChild(params);
						doc.appendChild(methodCall);
						doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

						QByteArray requestArray(doc.toByteArray());
						responseData = "";
						placeNetworkRequest(_wp_newCategory, requestArray);
						if (QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
					} else
						_status_widget->showMessage(tr("All HTTP requests are blocked."), 2000);
				} else
					_status_widget->showMessage(tr("Your category must have a name."), 2000);
			}
		}
	}
}

void Blogger::addKeywordTag()
{
	_control_tab->setCurrentIndex(3); // cw->cbPageSelector->setCurrentIndex(3);
	_control_tab->leAddKeywordTag->setFocus();
}

void Blogger::addKeywordTagFromLineEdit()
{
	QString text(_control_tab->leAddKeywordTag->text());
	if (!text.isEmpty()) {
		text.remove(QRegExp("^\""));
		text.remove(QRegExp("\"$"));

		_control_tab->lwKeywordTags->addItem(text);
		_control_tab->leAddKeywordTag->clear();
		if (!isWindowModified()) dirtify();
	}
}

void Blogger::addKeywordTagFromAvailTags()
{
	if (_control_tab->lwAvailKeywordTags->count() &&
	    _control_tab->lwAvailKeywordTags->currentRow() != -1) {
		// Check whether the tag selected is already in use
		QListWidgetItem* currentItem =
		    _control_tab->lwAvailKeywordTags->currentItem();
		QList<QListWidgetItem*> matches = _control_tab->lwKeywordTags->findItems(
		    currentItem->text(), Qt::MatchFixedString | Qt::MatchCaseSensitive);
		if (!matches.count()) _control_tab->lwKeywordTags->addItem(currentItem->text());
	}
}

void Blogger::refreshKeywordTags()
{
	if (!_current_http_business) {
		if (useWordpressAPI) {
			QDomDocument doc;
			QDomElement methodCall = doc.createElement("methodCall");
			methodCall.appendChild(XmlMethodName(doc, "wp.getTags"));

			QDomElement params = doc.createElement("params");
			params.appendChild(XmlValue(doc, "int", currentBlogid));
			params.appendChild(XmlValue(doc, "string", currentAccountElement.firstChildElement("details").firstChildElement("login").text()));
			params.appendChild(XmlValue(doc, "string", currentAccountElement.firstChildElement("details").firstChildElement("password").text()));

			methodCall.appendChild(params);
			doc.appendChild(methodCall);
			doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());
			addToConsole(doc.toString());
			QByteArray requestArray(doc.toByteArray(2));
			responseData = "";
			placeNetworkRequest(_wp_getTags, requestArray);
			if (QApplication::overrideCursor() == 0) QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		} else
			_status_widget->showMessage(tr("This feature only works on Wordpress."), 2000);
	} else {
#ifdef QTM_DEBUG
		statusWidget->showMessage(
		    tr("refreshCategories: All HTTP requests are blocked"), 2000);
#else
		_status_widget->showMessage(tr("All HTTP requests are blocked."), 2000);
#endif
	}
}

void Blogger::removeKeywordTag()
{
	int c = _control_tab->lwKeywordTags->currentRow();
	if (c != -1) {
		_control_tab->lwKeywordTags->takeItem(c);
		if (!isWindowModified()) dirtify();
	}
}

void Blogger::addTechTag()
{
	_control_tab->setCurrentIndex(4); // cw->cbPageSelector->setCurrentIndex(4);
	_control_tab->leAddTag->setFocus();
}

void Blogger::addClipTag()
{
	int l;
	QRegExpValidator tagFormat(QRegExp("^http:\\/\\/(www\\.)?technorati\\.com\\/tag\\/([a-zA-Z0-9\\.%]+[\\+ ])*[a-zA-Z0-9\\.%]+$"), this);
	QString tagText = QApplication::clipboard()->text();

	_control_tab->setCurrentIndex(4); // cw->cbPageSelector->setCurrentIndex(4);
	addToConsole(QString("Validating %1").arg(tagText));
	if (tagFormat.validate(tagText, l) != QValidator::Acceptable)
		_status_widget->showMessage(tr("This is not a valid tag."), 2000);
	else {
		_status_widget->showMessage(tr("This tag validates OK."), 2000);
		tagText.remove(QRegExp("(http:\\/\\/)?(www\\.)?technorati\\.com\\/tag\\/"));
		_control_tab->lwTags->addItem(tagText);
		if (!isWindowModified()) dirtify();
	}
}

void Blogger::addTechTagFromLineEdit()
{
	if (!_control_tab->leAddTag->text().isEmpty()) {
		_control_tab->lwTags->addItem(_control_tab->leAddTag->text());
		_control_tab->leAddTag->clear();
		if (!isWindowModified()) dirtify();
	}
}

void Blogger::addTechTagFromAddButton()
{
	int i;
	QString lineEditText = _control_tab->leAddTag->text();
	if (!lineEditText.isEmpty()) {
		if (tagValidator->validate(lineEditText, i) == QValidator::Acceptable) {
			_status_widget->showMessage(tr("This tag validates."), 2000);
			_control_tab->lwTags->addItem(_control_tab->leAddTag->text());
			_control_tab->leAddTag->clear();
			if (!isWindowModified()) dirtify();
		} else
			_status_widget->showMessage(tr("This is not a valid tag."), 2000);
	}
}

void Blogger::addTBPing()
{
	_control_tab->setCurrentIndex(5); // cw->cbPageSelector->setCurrentIndex(5);
	_control_tab->leTBPingURL->setFocus();
}

void Blogger::addClipTBPing()
{
	QString clipboardText = QApplication::clipboard()->text();
	if (!clipboardText.isEmpty()) {
		if (QUrl(clipboardText).isValid()) {
			_status_widget->showMessage(tr("This URL validates."));
			_control_tab->lwTBPings->addItem(clipboardText);
			if (!isWindowModified()) dirtify();
		} else
			_status_widget->showMessage(tr("This is not a valid URL."), 2000);
	}
}

void Blogger::addTBPingFromLineEdit()
{
	QString lineEditText = _control_tab->leTBPingURL->text();
	if (!lineEditText.isEmpty()) {
		if (QUrl(lineEditText).isValid()) {
			_status_widget->showMessage(tr("This URL validates."), 2000);
			_control_tab->lwTBPings->addItem(lineEditText);
			_control_tab->leTBPingURL->clear();
			if (!isWindowModified()) dirtify();
		} else
			_status_widget->showMessage(tr("This is not a valid URL."), 2000);
	}
}

void Blogger::removeTechTag()
{
	int c = _control_tab->lwTags->currentRow();
	_control_tab->lwTags->takeItem(c);
	if (!isWindowModified()) dirtify();
}

void Blogger::addTBPingFromAddButton()
{
	QString lineEditText = _control_tab->leTBPingURL->text();
	if (!lineEditText.isEmpty()) {
		if (QUrl(lineEditText).isValid()) {
			_status_widget->showMessage(tr("This URL validates."), 2000);
			_control_tab->lwTBPings->addItem(_control_tab->leTBPingURL->text());
			_control_tab->leTBPingURL->clear();
			if (!isWindowModified()) dirtify();
		} else
			_status_widget->showMessage(tr("This is not a valid URL."), 2000);
	}
}

void Blogger::removeTBPing()
{
	int c = _control_tab->lwTBPings->currentRow();
	_control_tab->lwTBPings->takeItem(c);
	if (!isWindowModified()) dirtify();
}

void Blogger::showPassword(bool showIt)
{
	_control_tab->lePassword->setEchoMode(showIt ? QLineEdit::Normal : QLineEdit::Password);
}

void Blogger::dirtify()
{
	_dirty_indicator->show();
	setWindowModified(true);
	setDirtySignals(false);
	_clean_save = false;
}

void Blogger::dirtifyIfText()
{
	if (!_editor->document()->isEmpty()) {
		_dirty_indicator->show();
		setWindowModified(true);
		setDirtySignals(false);
		_clean_save = false;
	}
}

void Blogger::setDirtySignals(bool d)
{
	QList<QWidget*> widgetList;
	widgetList << _editor << _control_tab->cbAccountSelector
		   << _control_tab->cbBlogSelector << _control_tab->cbStatus
		   << _control_tab->chAllowComments << _control_tab->chAllowTB
		   << _control_tab->cbMainCat << _control_tab->lwOtherCats
		   << _control_tab->teExcerpt;
	if (d) {
		connect(_editor->document(), SIGNAL(contentsChanged()), this, SLOT(dirtify()));
		connect(_control_tab->leTitle, SIGNAL(textEdited(const QString&)), this, SLOT(dirtifyIfText()));
		connect(_control_tab->cbAccountSelector, SIGNAL(activated(int)), this, SLOT(dirtifyIfText()));
		connect(_control_tab->cbBlogSelector, SIGNAL(activated(int)), this, SLOT(dirtifyIfText()));
		connect(_control_tab->cbStatus, SIGNAL(activated(int)), this, SLOT(dirtifyIfText()));
		connect(_control_tab->chAllowComments, SIGNAL(clicked(bool)), this, SLOT(dirtifyIfText()));
		connect(_control_tab->chAllowTB, SIGNAL(clicked(bool)), this, SLOT(dirtifyIfText()));
		connect(_control_tab->cbMainCat, SIGNAL(activated(int)), this, SLOT(dirtifyIfText()));
		connect(_control_tab->lwOtherCats, SIGNAL(activated(const QModelIndex&)), this, SLOT(dirtifyIfText()));
		connect(_control_tab->teExcerpt, SIGNAL(textChanged()), this, SLOT(dirtify()));
	} else {
		foreach (QWidget* w, widgetList) {
			disconnect(w, 0, this, SLOT(dirtify()));
			disconnect(w, 0, this, SLOT(dirtifyIfText()));
		}

		disconnect(_editor->document(), 0, this, SLOT(dirtify()));
	}
}

void Blogger::setPostClean()
{
	_dirty_indicator->hide();
	setWindowModified(false);
	setDirtySignals(true);
	_clean_save = false;
}

void Blogger::showStatusBarMessage(const QString& msg)
{
	_status_widget->showMessage(msg, 2000);
}

bool Blogger::saveCheck(bool)
{
	bool return_value;
	if (!isWindowModified())
		return_value = true;
	else
		return_value = saveCheck();
	return return_value;
}

bool Blogger::saveCheck()
{
	bool return_value;

	activateWindow();
	int a = QMessageBox::warning(
	    this, tr("Not saved"), tr("You have not saved this file.\n Save first?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No, QMessageBox::Cancel);
	switch (a) {
	case QMessageBox::Cancel:
		return_value = false;
		break;

	case QMessageBox::Yes:
		save();

		BOOST_FALLTHROUGH;
	default:
		return_value = true;
	}
	return return_value;
}

void Blogger::doInitialChangeBlog()
{
#ifdef QTM_DEBUG
	addToConsole(QString("loadedEntryBlog: %1\n").arg(loadedEntryBlog));
#endif
	disconnect(this, SIGNAL(httpBusinessFinished()), 0, 0);
	if (loadedEntryBlog != 999) {
		currentBlog = loadedEntryBlog;
		loadedEntryBlog = 999;
		_control_tab->cbBlogSelector->setCurrentIndex(currentBlog);
		currentBlogElement = currentAccountElement.firstChildElement("blogs")
					 .elementsByTagName("blog")
					 .at(currentBlog)
					 .toElement();
		currentBlogid = currentBlogElement.firstChildElement("blogid").text();
#ifndef NO_DEBUG_OUTPUT
// qDebug() << currentBlogid;
#endif
	}
	changeBlog(currentBlog);

	disconnect(_control_tab->cbBlogSelector, SIGNAL(activated(int)), this, SLOT(changeBlog(int)));
	connect(_control_tab->cbBlogSelector, SIGNAL(activated(int)), this, SLOT(changeBlog(int)));
}

void Blogger::copyURL()
{
	QApplication::clipboard()->setText(remoteFileLocation);
	if (_super_menu) {
		_super_menu->copyURLAction->setVisible(false);
		_super_menu->copyURLAction->setEnabled(false);
	}
	_control_tab->copyURLWidget->hide();
}

void Blogger::handleFind()
{
	if (_search_widget->isVisible()) {
		if (_super_menu) _super_menu->findAction->setText(tr("&Find ..."));
		_search_widget->close();
	} else {
		if (_super_menu) _super_menu->findAction->setText(tr("Exit &find"));
		_search_widget->find();
	}
}

void Blogger::findAgain()
{
#ifdef Q_OS_MAC
	searchWidget->findAgain();
#endif
}

QDomElement Blogger::XmlValue(QDomDocument& doc, QString valueType, QString actualValue, QString memberType)
{
	QDomElement param = doc.createElement(memberType);
	QDomElement value = doc.createElement("value");
	QDomElement realValue = doc.createElement(valueType);
	realValue.appendChild(QDomText(doc.createTextNode(actualValue)));
	value.appendChild(realValue);
	param.appendChild(value);

	return param;
}

QDomElement Blogger::XmlMember(QDomDocument& doc, QString valueName, QString valueType, QString actualValue)
{
	QDomElement mem = doc.createElement("member");
	QDomElement name = doc.createElement("name");
	name.appendChild(QDomText(doc.createTextNode(valueName)));
	mem.appendChild(name);
	QDomElement value = doc.createElement("value");
	QDomElement realValue = doc.createElement(valueType);
	realValue.appendChild(QDomText(doc.createTextNode(actualValue)));
	value.appendChild(realValue);
	mem.appendChild(value);

	return mem;
}

QDomElement Blogger::XmlMethodName(QDomDocument& doc, QString methodName)
{
	QDomElement methName = doc.createElement("methodName");
	methName.appendChild(QDomText(doc.createTextNode(methodName)));

	return methName;
}

QDomElement Blogger::XmlRpcArray(QDomDocument& doc, QString valueName, QString valueType, QList<QString> text)
{
	QDomElement arrayValue, arrayValueString;
	int i;

	QDomElement mem = doc.createElement("member");
	QDomElement name = doc.createElement("name");
	name.appendChild(QDomText(doc.createTextNode(valueName)));
	mem.appendChild(name);
	QDomElement value = doc.createElement("value");
	QDomElement array = doc.createElement("array");
	QDomElement arrayData = doc.createElement("data");
	for (i = 0; i < text.count(); i++) {
		arrayValue = doc.createElement("value");
		arrayValueString = doc.createElement(valueType);
		arrayValueString.appendChild(QDomText(doc.createTextNode(text.value(i))));
		arrayValue.appendChild(arrayValueString);
		arrayData.appendChild(arrayValue);
	}
	array.appendChild(arrayData);
	value.appendChild(array);
	mem.appendChild(value);

	return mem;
}

QDomElement Blogger::XmlRpcArray(QDomDocument& doc, QString valueName, QList<QString> text)
{
	return XmlRpcArray(doc, valueName, "string", text);
}

void Blogger::setNetworkActionsEnabled(bool a)
{
	_network_actions_enabled = a;
	if (_super_menu) {
		_super_menu->refreshBlogListAction->setEnabled(a);
		_super_menu->refreshCatsAction->setEnabled(categoriesEnabled ? a : false);
		_super_menu->updateCatsAction->setEnabled(categoriesEnabled ? a : false);
		_super_menu->blogThisAction->setEnabled(a);
	}
	_control_tab->lBlog->setEnabled(a);
	_control_tab->cbBlogSelector->setEnabled(a);
	_control_tab->pbRefresh->setEnabled(a);
}

void Blogger::handleInitialLookup(const QHostInfo& hostInfo)
{
	if (hostInfo.error() == QHostInfo::NoError) {
		refreshBlogList();
		setNetworkActionsEnabled(true);
	} else
		setNetworkActionsEnabled(false);
}
/*
   void EditingWindow::handleHostLookupFailed()
   {
   statusBar()->showMessage( tr( "The host specified could not be found." ),
   2000 );

   http->disconnect();
   http->abort();

   disconnect( SIGNAL( httpBusinessFinished() ) );
   if( QApplication::overrideCursor() != 0 )
    QApplication::restoreOverrideCursor();
   } */

void Blogger::configureQuickpostTemplates()
{
#if defined USE_SYSTRAYICON
	if (sti) sti->configureQuickpostTemplates(this);
#endif
}

void Blogger::setPublishStatus(int publishStatus)
{
	_control_tab->cbStatus->setCurrentIndex(publishStatus);
}

QString Blogger::postTitle()
{
	return _control_tab->leTitle->text();
}

void Blogger::setPostTitle(const QString& t)
{
	_control_tab->leTitle->setText(t);
}

void Blogger::saveAutoLinkDictionary()
{
	QDomDocument doc;
	QDomElement currentElement, currentKey, currentValue, currentTitle, currentTarget;
	QDomText titleText, targetText;

	QDomElement autoLinkDictionaryElement =
	    doc.createElement("AutoLinkDictionary");
	QHashIterator<QString, QString> aldIterator(autoLinkDictionary);
	while (aldIterator.hasNext()) {
		aldIterator.next();
		currentElement = doc.createElement("entry");
		currentKey = doc.createElement("key");
		currentKey.appendChild(QDomText(doc.createTextNode(aldIterator.key())));
		currentValue = doc.createElement("URL");
		currentValue.appendChild(QDomText(doc.createTextNode(aldIterator.value())));
		currentElement.appendChild(currentKey);
		currentElement.appendChild(currentValue);
		if (autoLinkTitleDictionary.contains(aldIterator.key())) {
			currentTitle = doc.createElement("title");
			titleText =
			    doc.createTextNode(autoLinkTitleDictionary.value(aldIterator.key()));
			currentTitle.appendChild(titleText);
			currentElement.appendChild(currentTitle);
		}
		if (autoLinkTargetDictionary.contains(aldIterator.key())) {
			currentTarget = doc.createElement("target");
			targetText = doc.createTextNode(
			    QString::number(autoLinkTargetDictionary.value(aldIterator.key())));
			currentTarget.appendChild(targetText);
			currentElement.appendChild(currentTarget);
		}
		autoLinkDictionaryElement.appendChild(currentElement);
	}
	doc.appendChild(autoLinkDictionaryElement);
	doc.insertBefore(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""), doc.firstChild());

	// QSettings _topic_editor_config(_current_topic_config_name,
	// QSettings::IniFormat);
	_topic_editor_config->beginGroup("account");
	QString dictionaryFileName =
	    QString("%1/qtmautolinkdict.xml")
		.arg(_topic_editor_config->value("editors_shared_directory", "").toString());
	_topic_editor_config->endGroup();
	QFile dictionaryFile(dictionaryFileName);
	if (dictionaryFile.open(QIODevice::WriteOnly)) {
		QTextStream dictionaryFileStream(&dictionaryFile);
		doc.save(dictionaryFileStream, 4);
		dictionaryFile.close();
	} else
		_status_widget->showMessage(tr("Could not write to dictionary file"), 2000);
}

void Blogger::loadAutoLinkDictionary()
{
	QString dictionaryFileName, errorString, currentKey, currentURL;
	int currentTarget;
	QDomElement currentKeyElement, currentURLElement, currentTitleElement, currentTargetElement;
	// QSettings _topic_editor_config(_current_topic_config_name,
	// QSettings::IniFormat);
	int errorLine, errorCol;
	bool ok;

	_topic_editor_config->beginGroup("account");
	dictionaryFileName =
	    QString("%1/qtmautolinkdict.xml")
		.arg(_topic_editor_config->value("editors_shared_directory", "").toString());
	_topic_editor_config->endGroup();
	if (QFile::exists(dictionaryFileName)) {
		QDomDocument dictDoc("autoLinkDictionary");
		QFile file(dictionaryFileName);
		if (!dictDoc.setContent(&file, true, &errorString, &errorLine, &errorCol))
			QMessageBox::warning(0, tr("Failed to read templates"), QString(tr("Error: %1\nLine %2, col %3")).arg(errorString).arg(errorLine).arg(errorCol));
		else {
			QDomNodeList entryList = dictDoc.elementsByTagName("entry");
			for (int i = 0; i < entryList.count(); i++) {
				currentKeyElement = entryList.at(i).firstChildElement("key");
				currentURLElement = entryList.at(i).firstChildElement("URL");
				currentTitleElement = entryList.at(i).firstChildElement("title");
				currentTargetElement = entryList.at(i).firstChildElement("target");
				if (currentKeyElement.isNull() || currentURLElement.isNull())
					continue;
				else {
					currentKey = currentKeyElement.text();
					currentURL = currentURLElement.text();
					autoLinkDictionary.insert(currentKey, currentURL);
					if (!currentTitleElement.isNull()) autoLinkTitleDictionary.insert(currentKey, currentTitleElement.text());
					if (!currentTargetElement.isNull()) {
						currentTarget = currentTargetElement.text().toInt(&ok);
						if (ok) autoLinkTargetDictionary.insert(currentKey, currentTarget);
					}
				}
			}
		}
	}
}

// This method was adapted from the QByteArray source code in Qt v4.3.1

QByteArray Blogger::toBase64(QByteArray& qbarray)
{
	QProgressDialog pdialog(tr("Converting file to Base64"), tr("Cancel"), 0, qbarray.size(), 0);
	pdialog.setWindowModality(Qt::WindowModal);
	pdialog.setMinimumDuration(2000);
	pdialog.setWindowTitle(program_title);

	const char alphabet[] =
	    "ABCDEFGH"
	    "IJKLMNOP"
	    "QRSTUVWX"
	    "YZabcdef"
	    "ghijklmn"
	    "opqrstuv"
	    "wxyz0123"
	    "456789+/";
	const char padchar = '=';
	int padlen = 0;

	char* data = qbarray.data();
	int size = qbarray.size();

	QByteArray tmp;
	tmp.resize(((size * 4) / 3) + 3);

	int i = 0;
	char* out = tmp.data();
	while (i < size) {
		int chunk = 0;
		chunk |= int(uchar(data[i++])) << 16;
		if (i == size)
			padlen = 2;
		else {
			chunk |= int(uchar(data[i++])) << 8;
			if (i == size)
				padlen = 1;
			else
				chunk |= int(uchar(data[i++]));
		}
		int j = (chunk & 0x00fc0000) >> 18;
		int k = (chunk & 0x0003f000) >> 12;
		int l = (chunk & 0x00000fc0) >> 6;
		int m = (chunk & 0x0000003f);
		*out++ = alphabet[j];
		*out++ = alphabet[k];
		if (padlen > 1)
			*out++ = padchar;
		else
			*out++ = alphabet[l];
		if (padlen > 0)
			*out++ = padchar;
		else
			*out++ = alphabet[m];
		if (pdialog.wasCanceled()) break;
		if (i % 1024 == 0) {
			pdialog.setValue(i);
			sapp_t::instance()->processEvents();
		}
	}
	tmp.truncate(out - tmp.data());
	if (pdialog.wasCanceled())
		return QByteArray();
	else
		return tmp;
}

void Blogger::addToConsole(const QString& t)
{
	QString text = t;
#ifndef NO_DEBUG_OUTPUT
	if (_current_http_business == _metaWeblog_newMediaObject)
// qDebug() << "Adding image to console";
#endif
		text.replace(QString("<string>%1</string>").arg(password), tr("<string>(password omitted)</string>"));

	QTextCursor cursor(_console->textCursor());

	cursor.movePosition(QTextCursor::End);
	_console->setTextCursor(cursor);
	if (text.contains("<base64>"))
		_console->insertPlainText(
		    text.section("<base64>", 0, 0, QString::SectionIncludeTrailingSep)
			.append(tr(" ... base64 encoded file omitted ... "))
			.append(text.section("</base64>", 1, 1, QString::SectionIncludeLeadingSep)));
	else
		_console->insertPlainText(text);
}

void Blogger::hideProgressBarIfMaximum(int val)
{
	if (val == _control_tab->progressBar->maximum()) QTimer::singleShot(1000, this, SLOT(hideProgressBar()));
}

void Blogger::hideProgressBar()
{
	_control_tab->progressWidget->setVisible(false);
	_control_tab->progressBar->reset();

#ifdef WIN_QTV
	taskbarProgress->setVisible(false);
	taskbarProgress->reset();
#endif
	if (QApplication::overrideCursor() != 0) QApplication::restoreOverrideCursor();
}

QString Blogger::checkBoxName(QString source)
{
	QChar firstLetter(source.at(1));
	return QString("%1%2%3")
	    .arg("ch")
	    .arg(firstLetter.toUpper())
	    .arg(source.section(1, -1));
}

QString Blogger::decodeXmlEntities(QString source)
{
	QString rv;
	int i, tc;
	int pos = 0;
	QChar thisChar;
	QString key, thisCap;
	bool ok;
	QStringList lst, caps;

	QRegExp rx1("&(\\S+);");      // Detects named entities
	QRegExp rx2("&#(\\d{1-3});"); // Detects only numerically-defined characters
	int index = rx1.indexIn(source);
	if (index == -1)
		return source;
	else {
		lst = source.split(rx1);
		while ((pos = rx1.indexIn(source, pos)) != -1) {
			caps += rx1.cap(0);
			// qDebug() << "Matched" << rx1.cap( 0 );
			pos += rx1.matchedLength();
		}
		// qDebug() << "Occurrences:" << caps.length();
		for (i = 0; i < lst.count(); ++i) {
			rv += lst.value(i);
			thisCap = caps.value(i);
			// qDebug() << "i =" << i;
			if (!thisCap.isNull()) {
				if (rx2.exactMatch(caps.value(i))) {
					tc = rx2.cap(0).toInt(&ok);
					if (ok && tc < 256)
						rv += QChar(tc);
					else
						rv += caps.value(i);
				} else {
					rx1 = QRegExp(rx1);
					rx1.indexIn(thisCap);
					key = rx1.cap(1);
					// qDebug() << "Using caps value" << i;
					// qDebug() << thisCap << "is not a numerical:
					// caps:" << rx1.numCaptures();
					if (xmlEntities.contains(key))
						rv += xmlEntities.value(key);
					else
						rv += rx1.cap(0);
				}
			}
		}
		return rv;
	}
}

bool Blogger::event(QEvent* event)
{
	if (event->type() == QEvent::WindowActivate) {
#ifdef Q_OS_MAC
//		_super_menu->blogger_changed(this);
#endif
		sapp_t::instance()->blogger(this);
	}
	return true;
	//	return super::event(event);
}

bool Blogger::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == _editor) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent* kpevent = static_cast<QKeyEvent*>(event);
			if (kpevent->key() == Qt::Key_V &&
			    kpevent->modifiers() == Qt::ControlModifier) {
				paste();
				return true;
			} else
				return false;
		} else
			return false;
	}
	if (obj == _preview_window && event->type() == QEvent::KeyPress) {
		QKeyEvent* kpevent = static_cast<QKeyEvent*>(event);
		if (kpevent->key() == Qt::Key_Escape && kpevent->modifiers() == 0) {
			doPreview(false);
			return true;
		} else
			return false;
	}
	if (obj == _console && event->type() == QEvent::KeyPress) {
		QKeyEvent* kpevent = static_cast<QKeyEvent*>(event);
		if (kpevent->key() == Qt::Key_Escape && kpevent->modifiers() == 0) {
			handleConsole(false);
			return true;
		} else
			return false;
	}
	return super::eventFilter(obj, event);
}

// void EditingWindow::assembly_closebutton(void){
//// Вертикальная область с кнопкой закрытия и распоркой
//// чтобы кнопка была вверху
// _toolsarea_of_close_button = new QVBoxLayout();
// _toolsarea_of_close_button->setContentsMargins(0, 0, 0, 0);
// _toolsarea_of_close_button->addWidget(_close_button);
// _toolsarea_of_close_button->addStretch();
// }

// void EditingWindow::setup_closebutton(void){
//// Кнопка закрытия виджета
// _close_button = new FlatToolButton(this);
// _close_button->setVisible(true);
// _close_button->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton));//
// SP_TitleBarCloseButton SP_DialogCloseButton
// _close_button->setAutoRaise(true);
// if(appconfig->interface_mode() == "desktop"){
// int w	= _close_button->geometry().width();
// int h	= _close_button->geometry().height();
// int x	= std::min(w, h) / 2;   // imin(w, h) / 2;
// _close_button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed,
// QSizePolicy::Fixed
// , QSizePolicy::ToolButton));
// _close_button->setMinimumSize(x, x);
// _close_button->setMaximumSize(x, x);
// _close_button->resize(x, x);
// }
// }

QStackedWidget* Blogger::main_stack()
{
	return _main_stack;
}

// rs_t *Blogger::record_screen(){return _record_screen;}

web::Docker<Blogger>* Blogger::editor_docker()
{
	return _editor_docker;
}

// QString Blogger::purify_topic(const QString &topic){
//	auto new_topic = topic;
//	new_topic	=
//tr(new_topic.remove(QRegExp("[\"/\\\\<>\\?:\\*\\|]+")).toStdString().c_str());
//	new_topic	= purify(new_topic);
//	return
//tr(new_topic.remove(QRegExp("[\"/\\\\<>\\?:\\*\\|]+")).toStdString().c_str());
//}

#ifdef USE_EDITOR_WRAP
void EditingWindow::name(const QString& nm)
{
	_editor->name(nm);
}

// void Editentry::tree_path(const QString &path){
// _editor->tree_path(path);
// }
void EditingWindow::pin(const QString& pin_)
{
	_editor->pin(pin_);
}

// void Editentry::switch_pin(bool checked){_editor->switch_pin(checked);}
void EditingWindow::author(const QString& author_)
{
	_editor->author(author_);
}

void EditingWindow::home(const QString& home_)
{
	_editor->home(home_);
}

void EditingWindow::url(const QString& url_)
{
	_editor->url(url_);
}

void EditingWindow::tags(const QString& tags_)
{
	_editor->tags(tags_);
}

boost::intrusive_ptr<TreeItem> EditingWindow::item()
{
	return _editor->item();
}

void EditingWindow::bind(boost::intrusive_ptr<TreeItem> item_to_be_bound)
{
	_editor->bind(item_to_be_bound);
}
#endif // USE_EDITOR_WRAP

const std::function<void(QObject* editor, QString saveString)> Blogger::save_callback() const
{
	return _editor->save_callback();
}

void Blogger::save_callback(const std::function<void(QObject* editor, QString saveString)>& func)
{
	_editor->save_callback(func);
}

std::function<void(QObject* editor, QString& String)> Blogger::load_callback() const
{
	return _editor->load_callback();
}

void Blogger::load_callback(const std::function<void(QObject* editor, QString& String)>& func)
{
	_editor->load_callback(func);
}

// void Editentry::editor_load_callback(QObject *editor, QString &load_text){
// Editor::editor_load_callback(editor, load_text);
// }
// void Editentry::editor_save_callback(QObject *editor, const QString
// &save_text){
// Editor::editor_save_callback(editor, save_text);
// }
#ifdef USE_FILE_PER_TREEITEM
bool EditingWindow::work_directory(QString dir_name)
{
	return _editor->work_directory(dir_name);
}
#endif // USE_FILE_PER_TREEITEM
QString Blogger::work_directory()
{
	return _editor->work_directory();
}
#ifdef USE_FILE_PER_TREEITEM
void EditingWindow::file_name(QString file_name_)
{
	_editor->file_name(file_name_);
}
#endif // USE_FILE_PER_TREEITEM
QString Blogger::file_name()
{
	return _editor->file_name();
}

void Blogger::save_textarea()
{
	_editor->save_textarea();
}

void Blogger::dir_file_empty_reaction(int mode)
{
	_editor->dir_file_empty_reaction(mode);
}

int Blogger::dir_file_empty_reaction()
{
	return _editor->dir_file_empty_reaction();
}

bool Blogger::load_textarea()
{
	return _editor->load_textarea();
}

int Blogger::cursor_position()
{
	return _editor->cursor_position();
}

void Blogger::cursor_position(int n)
{
	_editor->cursor_position(n);
}

int Blogger::scrollbar_position()
{
	return _editor->scrollbar_position();
}

void Blogger::scrollbar_position(int n)
{
	_editor->scrollbar_position(n);
}

QTextDocument* Blogger::textarea_document()
{
	return _editor->document();
}

void Blogger::textarea_editable(bool editable)
{
	_editor->textarea_editable(editable);
}

#ifdef USE_FILE_PER_TREEITEM
void Blogger::clear_all()
{
	_editor->initialize_data();
}
#endif // USE_FILE_PER_TREEITEM
FlatToolButton* Blogger::to_attach() const
{
	return _editor->_to_attach;
}

QIcon Blogger::icon_attach_exists() const
{
	return _editor->_icon_attach_exists;
}

QIcon Blogger::icon_attach_not_exists() const
{
	return _editor->_icon_attach_not_exists;
}

QVBoxLayout* Blogger::textformat_buttons_layout() const
{
	return _editor->_textformat_buttons_layout;
}

EditorTextArea* Blogger::text_area() const
{
	return _editor->_text_area;
}

void Blogger::to_editor_layout()
{
#ifdef USE_EDITOR_WRAP
	_editor->to_editor_layout();
#else
	_control_tab->setCurrentWidget(_control_tab->_basics);
#endif // USE_EDITOR_WRAP
}

void Blogger::to_attach_layout()
{
#ifdef USE_EDITOR_WRAP
	_editor->to_attach_layout();
#else
	_control_tab->setCurrentWidget(_control_tab->_attachtable_screen);
#endif // USE_EDITOR_WRAP
}

// Предпросмотр печати текущей статьи
void Blogger::file_print_preview(void)
{
	PrintPreview* preview = new PrintPreview(
	    textarea_document(), this); // _editor_screen->textarea_document()

	preview->setModal(true);
	preview->setAttribute(Qt::WA_DeleteOnClose);
	preview->show();
}

// Напечатать текущую статью
void Blogger::file_print(void)
{
#ifndef QT_NO_PRINTER
	QPrinter printer(QPrinter::HighResolution);
	printer.setFullPage(true);

	QPrintDialog* dlg = new QPrintDialog(&printer, this);
	dlg->setWindowTitle(tr("Print Document"));
	if (dlg->exec() == QDialog::Accepted) {
		// _editor_screen->textarea_document()->print(&printer);
		textarea_document()->print(&printer);
	}
	delete dlg;
#endif
}

// Вывод текущей статьи в PDF файл
void Blogger::file_print_pdf(void)
{
#ifndef QT_NO_PRINTER
	QString fileName =
	    QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
	if (!fileName.isEmpty()) {
		if (QFileInfo(fileName).suffix().isEmpty()) fileName.append(".pdf");
		QPrinter printer(QPrinter::HighResolution);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(fileName);
		// _editor_screen->textarea_document()->print(&printer);
		textarea_document()->print(&printer);
	}
#endif
}

// Метод, вызываемый из всех точек интерфейса, в которых происходит
// переход к другой записи. Метод вызывается до перехода, чтобы сохранить
// текст редактируемой записи
void Blogger::save_text_context()
{
	//	id_value id = this->misc_field<id_key>(); // static_cast<id_value>(_control_tab->topic());//
	//	if (detail::to_string(id) == "") {
	//		id = //_browser->currentTab()->page()->host()
	//		    current_item->id();
	//		this->misc_field<id_key>(id);
	//		qDebug() << "Blogger::save_text_context() : id : " << id << " _control_tab->topic() : " << _control_tab->topic();
	//	}
	//	// _editor_screen->misc_field("id")

	//	qDebug() << "Blogger::save_text_context() : id : " << id << " _control_tab->topic() : " << _control_tab->topic();
	save(); // save_textarea();
		// _editor_screen->save_textarea();
		// move to global wn_t::save_all_state
		//	walkhistory.add<WALK_HISTORY_GO_NONE>(
		//	    id,                    //static_cast<id_value>(_control_tab->topic()), // id
		//	    cursor_position(),     // _editor_screen->cursor_position()
		//	    scrollbar_position()); // _editor_screen->scrollbar_position());
}

void Blogger::save_editor_cursor_position(void)
{
	// int n = _editor_screen->cursor_position();
	int n = cursor_position();
	_topic_editor_config->setValue("editor_cursor_position", n);
}

void Blogger::save_editor_scrollbar_position(void)
{
	// int n = _editor_screen->scrollbar_position();
	int n = scrollbar_position();
	_topic_editor_config->setValue("editor_scroll_bar_position", n);
}

void Blogger::walk_history_previous(void)
{
	// _editor_screen->save_textarea();
	save_textarea();
	id_value id = misc_field<id_key>(); // static_cast<id_value>(_control_tab->topic()); //
	// _editor_screen->misc_field("id")

	walkhistory.add<WALK_HISTORY_GO_PREVIOUS>(
	    id,
	    cursor_position(),   // _editor_screen->cursor_position()
	    scrollbar_position() // _editor_screen->scrollbar_position()
	    );
	//	walkhistory.set_drop(true);

	walk_history_global();
}

void Blogger::walk_history_next(void)
{
	// _editor_screen->save_textarea();
	save_textarea();
	id_value id = misc_field<id_key>(); // static_cast<id_value>(_control_tab->topic());//
	// _editor_screen->misc_field("id")

	walkhistory.add<WALK_HISTORY_GO_NEXT>(
	    id,
	    cursor_position(),   // _editor_screen->cursor_position()
	    scrollbar_position() // _editor_screen->scrollbar_position()
	    );
	//	walkhistory.set_drop(true);

	walk_history_global();
}

void Blogger::walk_history_global()
{
	bool drop_flag_status = walkhistory.drop_flag();
	walkhistory.drop_flag(true);
	// QString tree_root_id = walkhistory.tree_root_id();

	// if(_tree_screen->know_root()->root_item()->id() != tree_root_id) {
	// _tree_screen->intercept(tree_root_id);
	// }

	// Выясняется идентификатор записи, на которую надо переключиться
	auto record_at_iterator = walkhistory.record_at_iterator();

	// if(record_id.length() == 0) {
	// walkhistory.set_drop(false);
	// return;
	// }
	auto _tree_view = gl_paras->tree_screen()->view();
	auto know_model_board = std::bind(
	    &tv_t::know_model_board, _tree_view); // [&](){return _tree_screen->view()->know_model_board();};
	if (static_cast<QString>(record_at_iterator).length() > 0) {
		// Выясняется путь к ветке, где находится данная запись
		QStringList absolute_path = static_cast<tkm_t*>(know_model_board())->path_list(record_at_iterator); // on know_root semantic

		//// Проверяем, есть ли такая ветка
		// if(_tree_screen->know_model_board()->is_item_valid(absolute_path) ==
		// false) {    // on know_root semantic
		// walkhistory.set_drop(false);
		// return;
		// }

		// Выясняется позицию записи в таблице конечных записей
		auto target = know_model_board()->item(absolute_path); // on know_root semantic
		auto vtab = gl_paras->vtab_record();
		web::Browser* current_browser = nullptr;
		if (vtab) {
			auto wid = vtab->currentWidget();
			if (wid) {
				if (wid->objectName() == record_screen_multi_instance_name) {
					auto rs = dynamic_cast<rs_t*>(wid);
					if (rs) {
						current_browser = rs->browser();
					}
				}
			}
		}
		auto current = current_browser ? current_browser->currentTab()->page()->host() : boost::intrusive_ptr<i_t>(nullptr);
		if (target && target != know_model_board()->root_item() && target != current) {
			//// Проверяем, есть ли такая позиция
			// if(!item->item_direct(record_id)) {  // == false
			// walkhistory.set_drop(false);
			// return;
			// }
			// if(item->child_direct(record_id)){//?
			gl_paras->main_window()->set_tree_position(global_root_id, absolute_path);
			// select_id(id);
			if (_topic_editor_config->value("remember_cursor_at_history_navigation").toBool()) {
				// _editor_screen->cursor_position(walkhistory.cursor_position(record_id));
				cursor_position(walkhistory.cursor_position(record_at_iterator));
				// _editor_screen->scrollbar_position(walkhistory.scrollbar_position(record_id));
				scrollbar_position(walkhistory.scrollbar_position(record_at_iterator));
			}
			// }
			auto view = gl_paras->main_window()->find([&](boost::intrusive_ptr<const ::Binder> b) {
				return url_equal(url_value(detail::to_qstring(b->host()->field<home_key>())), target->field<url_key>()) && b == target->binder() && b->host()->id() == target->id();
			});
			if (view) {
				auto browser_ = view->browser();
				auto page = view->page();
				if (page && browser_) {
					auto host = page->host();
					if (host) {
						view = host->activate(std::bind(&web::Browser::find, browser_, std::placeholders::_1));
					}
				}
			}
			if (!view) gl_paras->main_window()->browser(target);
			//			auto url_target = target->field<url_key>();
			//			// walkhistory.set_drop(false);
			//			real_url_t<url_value>::instance<boost::intrusive_ptr<i_t>>(target->field<url_key>(),
			//			    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) -> boost::intrusive_ptr<i_t> {
			//				    return TreeIndex::url_activate(real_target_url_,
			//					[&] { return _tree_view->source_model(); },
			//					_tree_view->current_item(),
			//					std::bind(&tv_t::move, _tree_view, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4),
			//					[&](boost::intrusive_ptr<const i_t> it_) -> bool { return url_equal(url_value(detail::to_qstring(it_->field<home_key>())), url_target) || url_equal(url_value(detail::to_qstring(it_->field<url_key>())), url_target); },
			//					browser_ref());
			//			    });
		}
		// else {
		// walkhistory.set_drop(false);
		// }
	}
	walkhistory.drop_flag(drop_flag_status);
}

void Blogger::restore_editor_cursor_position(void)
{
	int n = _topic_editor_config->value("editor_cursor_position").toInt();

	// _editor_screen->cursor_position(n);
	cursor_position(n);
}

void Blogger::restore_editor_scrollbar_position(void)
{
	int n = _topic_editor_config->value("editor_scroll_bar_position").toInt();
	// _editor_screen->scrollbar_position(n);
	scrollbar_position(n);
}

QString Blogger::current_topic_folder_name() const
{
	return _current_topic_full_folder_name;
}

SuperMenu* Blogger::super_menu()
{
	return _super_menu;
}


web::Browser* Blogger::browser()
{
	return _browser;
}

//void Blogger::on_record_screen_close_requested()
//{
//	_record_screen = nullptr;
//	close();
//}

//void Blogger::on_browser_close_requested()
//{
//	_browser = nullptr;
//	close();
//}

SideTabWidget* Blogger::control_tab()
{
	return _control_tab;
}

void Blogger::history_sychronize(boost::intrusive_ptr<i_t> current_item)
{
	//	if (_browser) {
	//		//	if (!_browser) {
	//		//		//		_browser.detach();
	//		//		_browser = browser_;// _browser might undefined
	//		//	}
	//		boost::intrusive_ptr<i_t> current_item = _browser->tab_widget()->current_item(); //_binder->host();
	//		// boost::intrusive_ptr<Record> record = this->table_model()->table_data()->record(pos);
	assert(current_item);
//        // assert(record ==
//        // view()->tabmanager()->currentWebView()->page()->current_record()); // may
//        // be in backend?

//        // Внимание! Наверно, всю эту логику следует перенести в MetaEditor. А здесь
//        // только получить данные из таблицы

//        // Выясняется указатель на объект редактирования текста записи
//        // MetaEditor *_editor_screen = globalparameters.meta_editor();    //
//        // find_object<MetaEditor>(meta_editor_singleton_name);
//        assert(_blogger);
//// boost::intrusive_ptr<RecordModel::ModelIndex> record_index;
//// try {
//// record_index = new RecordModel::ModelIndex([&] {return
//// _record_controller->source_model();},
//// _record_controller->source_model()->sibling(current_item), current_item);
//// } catch(std::exception &) {}
#ifdef USE_FILE_PER_TREEITEM // if(record_index) {
	if (_blogger->item() != current_item)
		_blogger->bind(current_item);
// int pos = indexOf(currentWebView());
// Turns the reference to the table of final data   // Выясняется ссылка на
// таблицу конечных данных
// std::shared_ptr<TableData> table = std::make_shared<TableData>();   //
// _source_model->table_data_internal();

// for(int i = 0; i < count(); i++) {
// table->insert_new_record(table->work_pos(), static_cast<WebView
// *>(widget(i))->page()->current_record());
// }

// В таблице конечных данных запоминается какая запись была выбрана
// чтобы затем при выборе этой же подветки засветка автоматически
// установилась на последнюю рабочую запись
// table->work_pos(pos);
#endif // USE_FILE_PER_TREEITEM
	// Устанавливается функция обратного вызова для записи данных
	this->save_callback(Editor::editor_save_callback); // _editentry->editor_save_callback

	// Сохраняется текст и картинки в окне редактирования
	// find_object<MainWindow>("mainwindow")
	this->save_text_context();
#ifdef USE_FILE_PER_TREEITEM
	// Для новой выбраной записи выясняется директория и основной файл
	if (current_item->field<id_type>().length() == 0)
		current_item
		    ->field<id_type>(current_item->field<dir_type>().length() > 0 ? current_item->field<dir_type>() : get_unical_id()); // "id",	// ||
																	// current_item->field("url")
																	// ==
																	// web::Browser::_defaulthome
	if (current_item->field<url_type>() == "")
		current_item->field<dir_type>(current_item->id()); // "dir",
	if (current_item->field<file_type>() == "")
		current_item->field<file_type>("text.html"); // "file",
	QString current_dir =
	    current_item->field<dir_type>(); // table->field(pos, "dir");
	QString current_file =
	    current_item->field<file_type>(); // table->field(pos, "file");
	QString full_dir = gl_paras->root_path() + "/" +
	    QDir(appconfig->data_dir()).dirName() + "/base/" +
	    current_dir;
	QString full_file_name = full_dir + "/" + current_file;
	qDebug() << " File " << full_file_name << "\n";
	// If the window contents of the record is already selected record  // Если в
	// окне содержимого записи уже находится выбираемая запись
	if (this->work_directory() == full_dir &&
	    this->file_name() == current_file) {
		gl_paras->window_switcher()->recordtable_ro_record_editor();

		return;
	}
	// Перед открытием редактора происходит попытка получения текста записи
	// Этот вызов создаст файл с текстом записи, если он еще не создан (подумать,
	// переделать)
	// Before the opening of the editor it attempts to get the text records
	// This call will create a text file with the record if it is not already
	// created (think remake)
	this->save_textarea(); // table->text(pos);    // ?

	// едактору задаются имя файла и директории
	// И дается команда загрузки файла
	this->work_directory(full_dir);
	this->file_name(current_file);

	// Если идет работа с зашифрованной записью
	// И если имя директории или имя файла пусты, то это означает что
	// запись не была расшифрована, и редактор должен просто показывать пустой
	// текст
	// ничего не сохранять и не считывать
	qDebug() << "void WebPage::metaeditor_sychronize() : id "
		 << current_item->field<id_type>(); // table->field(pos, "id");
	qDebug() << "void WebPage::metaeditor_sychronize() : name "
		 << current_item->field<name_type>(); // table->field(pos, "name");
	qDebug() << "void WebPage::metaeditor_sychronize() : crypt "
		 << current_item->field<crypt_type>(); // table->field(pos,
						       // boost::mpl::c_str <
						       // crypt_type > ::value);
	if (current_item->field<crypt_type>() ==
	    "1") // table->field(pos, boost::mpl::c_str < crypt_type > ::value)
		if (full_dir.length() == 0 || current_file.length() == 0)
			this->dir_file_empty_reaction(
			    EditorWrap::DIRFILEEMPTY_REACTION_SUPPRESS_ERROR);
	// _editentry->blog_editor()->editor()->clear_all_misc_field();
	// В редактор заносится информация, идет ли работа с зашифрованным текстом
	this->misc_field(boost::mpl::c_str<crypt_type>::value,
	    current_item->field<crypt_type>()); // table->field(pos,
						// boost::mpl::c_str
						// < crypt_type >
						// ::value)

	// В редакторе устанавливается функция обратного вызова для чтения данных
	this->load_callback(&Editor::editor_load_callback);

	this->load_textarea();
	// edView->set_textarea(table->get_text(index.row()));

	// Заполняются прочие инфо-поля
	this->pin(current_item->field<pin_type>()); // table->field(pos, "pin")

	this->name(current_item->field<name_type>()); // table->field(pos, "name")

	this->author(
	    current_item->field<author_type>()); // table->field(pos, "author")

	this->home(current_item->field<home_type>()); // table->field(pos, "home")

	this->url(current_item->field<url_type>()); // table->field(pos, "url")

	this->tags(current_item->field<tags_type>()); // table->field(pos, "tags")

	auto id =
	    id_value(current_item->field<id_type>()); // table->field(pos, "id");
	this->misc_field("id", id);

	this->misc_field(
	    "title", current_item->field<name_type>()); // table->field(pos, "name")
	// should each record carry it's tree path?
	//// Set the path to the branch in which lies the record (in the form of the
	///names of the branches)   // Устанавливается путь до ветки в которой лежит
	///запись (в виде названий веток)
	// QString path = qobject_cast<TableScreen *>(parent())->getTreePath();
	//// В мобильном интерфейсе редактор должен показывать путь до записи
	// if(appconfig->getInterfaceMode() == "mobile")
	// meta_editor->setTreePath(path);
	// В редакторе восстанавливается позиция курсора и прокрутки если это
	// необходимо
	if (appconfig->remember_cursor_at_ordinary_selection()) {
		this->cursor_position(walkhistory.cursor_position(id));
		this->scrollbar_position(walkhistory.scrollbar_position(id));
	}
#else
	this->misc_field<crypt_key>(current_item->field<crypt_key>()); // table->field(pos,
								       // boost::mpl::c_str
								       // < crypt_type >
								       // ::value)
	auto id = id_value(current_item->field<id_key>());             // table->field(pos, "id");
	assert(detail::to_string(id) != "");

	//		if (detail::to_string(id) == "") {
	//			id = currrent_blogger->browser()->currentTab()->page()->host()->id();
	//			currrent_blogger->misc_field<id_key>(id);
	//			qDebug() << "Blogger::current id : " << id << " _control_tab->topic() : " << currrent_blogger->control_tab()->topic();
	//		}
	this->misc_field<id_key>(id);
	this->misc_field<name_key>(current_item->field<name_key>()); // table->field(pos, "name")
	//
	//	        auto id = this->topic();
	// В редакторе восстанавливается позиция курсора и прокрутки если это
	// необходимо
	if (appconfig->remember_cursor_at_ordinary_selection()) {
		this->cursor_position(walkhistory.cursor_position(static_cast<id_value>(id)));
		this->scrollbar_position(walkhistory.scrollbar_position(static_cast<id_value>(id)));
	}
#endif // USE_FILE_PER_TREEITEM
	// Обновление иконки аттачей
	if (current_item->attach_table()->size() == 0) // table->record(pos)->getAttachTablePointer()->size()
		this->to_attach()->setIcon(this->icon_attach_not_exists());
	// Если нет приаттаченных файлов
	else
		this->to_attach()->setIcon(this->icon_attach_exists()); // Есть приаттаченные файлы

	//		auto drop_flag_status = walkhistory.drop_flag();
	//		walkhistory.drop_flag(false);// not sure, might triggered by walk_history or select_as_current
	walkhistory.add<WALK_HISTORY_GO_NONE>(
	    id,                          //static_cast<id_value>(_control_tab->topic()), // id
	    this->cursor_position(),     // _editor_screen->cursor_position()
	    this->scrollbar_position()); // _editor_screen->scrollbar_position());
					 //		walkhistory.drop_flag(drop_flag_status);
					 //	} // _browser != nullptr
}


#include "libraries/qtm/blogger.inl"
