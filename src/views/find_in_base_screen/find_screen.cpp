
#include <cassert>
#include <future>
#include <thread>
#include <utility>

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QBoxLayout>
#include <QByteArray>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressDialog>
#include <QPushButton>
#include <QSplitter>
#include <QTextDocument>
#include <QWidget>
#include <QtWidgets/QStackedWidget>

//#include "models/tree/tree_item.dec"

#include "find_screen.h"
#include "libraries/flat_control.h"
#include "libraries/flat_control.h"
#include "libraries/global_parameters.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "libraries/qtm/blogger.h"
#include "libraries/qtm/side_tabwidget.h"
#include "main.h"
#include "models/app_config/app_config.h"
#include "models/record_table/items_flat.h"
#include "models/record_table/linker.hxx"
#include "models/record_table/record_index.hxx"
#include "models/tree/tree_item.h"
#include "models/tree/tree_know_model.h"
#include "utility/add_action.h"
#include "views/browser/browser.h"
#include "views/browser/chasewidget.h"
#include "views/browser/docker.h"
#include "views/browser/searchlineedit.h"
#include "views/browser/tabwidget.h"
#include "views/browser/toolbar_search.h"
#include "views/browser/webview.h"
#include "views/main_window/main_window.h"
#include "views/record/editor_wrap.h"
#include "views/record_table/record_screen.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_screen.h"
#include "views/tree/tree_view.h"

extern std::shared_ptr<AppConfig> appconfig;
extern std::shared_ptr<gl_para> gl_paras;
extern const char* global_root_id;

constexpr char FindScreen::_find_in_base_expand[];

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(FindScreen)
#endif

FindScreen::FindScreen(QString object_name, //
    ts_t* tree_screen_,
    wn_t* parent // ,boost::intrusive_ptr<TreeItem> _selected_branch_root
    )
    : QWidget(parent)
    , _find_text_and_button_tools_area(new QHBoxLayout())
    , _tree_screen([&, tree_screen_] {gl_paras->find_screen(this); return tree_screen_; }())
    , _browser(nullptr
	  //	  real_url_t<url_value>::instance<web::Browser*>(web::Browser::_defaulthome,
	  //	      [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) {
	  //		      return gl_paras->main_window()->browser(real_target_url_, true); // QtSingleApplication::instance()->newMainWindow();
	  //	      })
	  //	  //	  [&] {
	  //	  //		  //	    _browser = nullptr;
	  //	  //		  gl_paras->find_screen(this);
	  //	  //		  auto view = web::WebView::instance(this, web::Browser::_defaulthome);
	  //	  //		  auto bro = view->page()->browser();
	  //	  //		  return bro;
	  //	  //	  }()
	  )
    , _navigater( //nullptr
	  [&] {
		  _navigater = nullptr;

		  auto nav = new QToolBar(this);
		  nav->setObjectName(gl_para::_browser_navigater_name);
		  nav->setMaximumWidth(130);
		  return nav;
		  //		  return _browser->navigater();
	  }())                                                //new QToolBar(this)
    , _historyback(new QAction(tr("Back"), _navigater))       // FlatToolButton *_history_back;
    , _historyforward(new QAction(tr("Forward"), _navigater)) // FlatToolButton *_history_forward;
    , _historyhome(new QAction(tr("Home"), _navigater))       // FlatToolButton *_history_home;
    , _stopreload(new QAction(_navigater))                    // FlatToolButton *_stop_reload;
    , _stop(new QAction(tr("&Stop"), _navigater))
    , _reload(new QAction(tr("Reload Page"), _navigater)) // , back_ground{      //      // _tree_screen->know_branch()->root_item()      // //_selected_branch_root      // }
    , _editor_switch(new FlatToolButton(tr("Editor Show/Hide"), this))
    , _find_start_button(new FlatToolButton("", this, QIcon()))
    , _tools_expand(new FlatToolButton("", this))
    , _close_button_tools_area(new QVBoxLayout())
    , _close_button(new FlatToolButton("", this))
    , _combo_option_tools_area(new QHBoxLayout())
    , _word_regard(new FlatComboBox())
    , _how_extract(new FlatComboBox())
    , _tree_search_area(new FlatComboBox())
#ifdef USE_CLEAR_BUTTON
    , _chasewidget(new web::ChaseWidget(QSize(16, 16), this))
#endif // USE_CLEAR_BUTTON
    , _where_find_line(new QHBoxLayout())
    , _where_find_label(new QLabel(tr("Find in: ")))
    , _find_in_pin(new QCheckBox(tr("Pin")))
    , _find_in_name(new QCheckBox(tr("Title")))
    , _find_in_author(new QCheckBox(tr("Author(s)")))
    , _find_in_home(new QCheckBox(tr("Home")))
    , _find_in_url(new QCheckBox(tr("Url")))
    , _find_in_tags(new QCheckBox(tr("Tags")))
    , _find_in_text(new QCheckBox(tr("Text")))
    , _tools_line(new QHBoxLayout())
    , _tools_grid(new QGridLayout())
    , _central_desktop_layout(new QVBoxLayout())
#ifdef SHOW_PROCESS_DIALOG
    , _progress(new QProgressDialog(this))
#endif
// , _findtable(new FindTableWidget(this)) // deprecated
// , _is_search_global(_tree_screen->know_branch()->root_item()->id() ==
// global_root_id)
// , _selected_branch_as_pages([ & ]
// {
// std::shared_ptr<ItemsFlat> result = std::make_shared<ItemsFlat>();

// for(int i = 0; i <
// _tree_screen->know_branch()->root_item()->current_count(); i++) {
// auto it = _tree_screen->know_branch()->root_item()->child(i);

// if(it->is_lite())it->to_fat();

// result->insert_new_item(result->current_count() - 1, it);
// }

// return result;
// }())    // static_cast<ItemsFlat *>(_selected_branch_root.get()) //
// never transfer this data directly, it is come from _treeknowmodel!   //
// _resultset_data(std::make_shared<RecordTable>(QDomElement()))
#ifdef USE_STACK_LAYOUT
    , _stack_layout(new QVBoxLayout())
#endif // USE_STACK_LAYOUT
    , _lineedit_stack(
	  [&] {
		  _lineedit_stack = nullptr;
		  auto ls = new QStackedWidget(this);
		  ls->setObjectName(gl_para::_browser_lineedit_stack_name);
		  return ls;
	  }()
	  //	  [&] {
	  //		  _lineedit_stack = nullptr;
	  //		  return _browser->tab_widget()->lineEditStack();
	  //	  }()
	  )
//    , _toolbarsearch_buffer([&]() -> web::ToolbarSearch* {
//	    _toolbarsearch_buffer = nullptr;
//	    auto view = web::WebView::instance(this, web::Browser::_defaulthome);
//	    auto ts = view->toolbarsearch(); //new web::ToolbarSearch(this, view);
//	    return ts;
//    }())

{
	//	_browser = [&] {
	//		//	    _browser = nullptr;
	//		gl_paras->find_screen(this);
	//		auto view = web::WebView::instance(this, web::Browser::_defaulthome);
	//		auto bro = view->page()->browser();
	//		return bro;
	//	}();

	//	_navigater = [&] {
	//		//		_navigater = nullptr;
	//		return _browser->navigater();
	//	}();                                                      //new QToolBar(this)
	//	_historyback = new QAction(tr("Back"), _navigater);       // FlatToolButton *_history_back;
	//	_historyforward = new QAction(tr("Forward"), _navigater); // FlatToolButton *_history_forward;
	//	_historyhome = new QAction(tr("Home"), _navigater);       // FlatToolButton *_history_home;
	//	_stopreload = new QAction(_navigater);                    // FlatToolButton *_stop_reload;
	//	_stop = new QAction(tr("&Stop"), _navigater);
	//	_reload = new QAction(tr("Reload Page"), _navigater);
	//	_lineedit_stack = //new QStackedWidget()
	//	    [&] {
	//		    //		    _lineedit_stack = nullptr;
	//		    return _browser->tab_widget()->lineEditStack();
	//	    }();

	setObjectName(object_name);
//	_navigater->setObjectName(gl_para::_browser_navigater_name);
//	_navigater->setMaximumWidth(130);
#ifdef USE_CLEAR_BUTTON
	_chasewidget->setMaximumWidth(17);
#endif // USE_CLEAR_BUTTON
#ifdef SHOW_PROCESS_DIALOG
	_progress->reset();
#endif

	//	_toolbarsearch_buffer->setMinimumWidth(100);
	//	_toolbarsearch_buffer->sizePolicy().setHorizontalStretch(10);
	//	// _toolbarsearch->setContentsMargins(0, 0, 0, 0);

	// if(appconfig->getInterfaceMode() == "desktop") {
	// _navigater->setMinimumWidth(400);
	// }

	setup_navigate();
	assembly_navigate();

	setup_findtext_and_button();
	assembly_findtext_and_button();

	setup_closebutton();
	assembly_closebutton();

	setup_combooption();
	// assemblyComboOption();

	setup_wherefind_line();
	assembly_wherefind_line();

	setup_ui();
	assembly();

	setup_signals();
}

FindScreen::~FindScreen(void)
{
	// delete _selected_branch_as_pages;
}


FlatToolButton* FindScreen::findstartbutton() { return _find_start_button; }

//void FindScreen::historyback(QAction* hb) { _historyback = hb; }

//void FindScreen::historyforward(QAction* hf) { _historyforward = hf; }

//void FindScreen::historyhome(QAction* hh) { _historyhome = hh; }

//void FindScreen::stopreload(QAction* sl) { _stopreload = sl; }

//void FindScreen::stop(QAction* st) { _stop = st; }

//void FindScreen::reload(QAction* rl) { _reload = rl; }
#ifdef USE_CLEAR_BUTTON
web::ChaseWidget* FindScreen::chasewidget()
{
	return _chasewidget;
}
#endif // USE_CLEAR_BUTTON
void FindScreen::setup_navigate(void)
{
	// assert(globalparameters.entrance());
	// assert(globalparameters.entrance()->activebrowser());
	// this->addAction(globalparameters.entrance()->activebrowser()->historyback());

	// historyback_ = new FlatToolButton(this);
	// historyback_->setStatusTip(tr("Back"));
	// historyback_->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0,
	// this));    //QIcon(":/resource/pic/pentalpha.svg");
	// historyback_->setObjectName("Back");

	// _history_home = new FlatToolButton(this);

	// _historyhome = new QAction(tr("Home"), _navigater);

	_historyhome->setIcon(QIcon(":/resource/pic/mobile_up.svg")); // ":/resource/pic/streamline_home.svg"
	// style()->standardIcon(QStyle::SP_ArrowUp, 0, this)

	_historyhome->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));

	add_action<QToolButton>(_navigater, _historyhome); // _navigater->addAction(_historyhome);

	// _history_back = new FlatToolButton(this);

	// _historyback = new QAction(tr("Back"), _navigater);

	_historyback->setShortcuts(QKeySequence::Back);
	// _historyback->setIconVisibleInMenu(false);
	_historyback->setIcon(QIcon(":/resource/pic/mobile_back.svg")); // QIcon(":/resource/pic/walk_history_next.svg")//
	// style()->standardIcon(QStyle::SP_ArrowBack, 0, this)

	// QMenu *_historybackmenu = new QMenu(this);
	// _historyback->setMenu(_historybackmenu);
	// connect(_historybackmenu, SIGNAL(aboutToShow()), this,
	// SLOT(slotAboutToShowBackMenu()));
	// connect(_historybackmenu, SIGNAL(triggered(QAction *)), this,
	// SLOT(slotOpenActionUrl(QAction *)));
	add_action<QToolButton>(_navigater, _historyback); // _navigater->addAction(_historyback);
	// insertActionAsButton(_container, _historyback);

	// _history_forward = new FlatToolButton(this);

	// _historyforward = new QAction(tr("Forward"), _navigater);

	_historyforward->setShortcuts(QKeySequence::Forward);
	_historyforward->setIcon(QIcon(":/resource/pic/mobile_forward.svg")); // style()->standardIcon(QStyle::SP_ArrowForward,
									      // 0, this)

	// _historyforwardmenu = new QMenu(this);
	// connect(_historyforwardmenu, SIGNAL(aboutToShow()), this,
	// SLOT(slotAboutToShowForwardMenu()));
	// connect(_historyforwardmenu, SIGNAL(triggered(QAction *)), this,
	// SLOT(slotOpenActionUrl(QAction *)));
	// _historyforward->setMenu(_historyforwardmenu);
	add_action<QToolButton>(_navigater, _historyforward); // _navigater->addAction(_historyforward);

	// _stop_reload = new FlatToolButton(this);

	// _stopreload = new QAction(_navigater);

	// _reloadicon = style()->standardIcon(QStyle::SP_BrowserReload);
	_stopreload->setIcon(QIcon(":/resource/pic/mobile_reload.svg")); // style()->standardIcon(QStyle::SP_BrowserReload)

	add_action<QToolButton>(_navigater, _stopreload); // _navigater->addAction(_stopreload);
}

void FindScreen::assembly_navigate(void)
{
	// _navigater = new QHBoxLayout();
	// _navigater->addWidget(_container);
	// _navigater->addStretch();
}

// Текст поиска и кнопка "Поиск"
void FindScreen::setup_findtext_and_button(void)
{

	_editor_switch->setToolTip(tr("Editor Show/Hide"));
	_editor_switch->setStatusTip(tr("Editor open/close"));
	_editor_switch->setIcon(QIcon(":/resource/pic/edit_switch.svg"));
	_editor_switch->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
	connect(_editor_switch, &FlatToolButton::clicked, [&](bool checked) {
		//			if(checked){//
		auto _editor_docker = gl_paras->editor_docker();
		(void)checked;
		if (!(_editor_docker->isVisible())) {
			_editor_docker->show();
			// emit _app->editing_win_changed(_editenty->blog_editor());
			// emit this->editing_activated(blog_editor());
			// _app->sendEvent(_editenty->blog_editor(), new QEvent(QEvent::WindowActivate));

			appconfig->editor_show(true);
		} else {
			_editor_docker->hide();
			// _app->sendEvent(_editenty->blog_editor(), new QEvent(QEvent::WindowActivate));

			appconfig->editor_show(false);
		}
	});
	//	_switch_search_type = new FlatToolButton("", this);
	//	_switch_search_type->setIcon(QIcon(":/resource/pic/find_in_base_expand_tools.svg"));
	//	_switch_search_type->setToolTip(tr("Editor and Topic Switch with url"));
	//	_switch_search_type->setEnabled(true);
	//	_switch_search_type->setAutoRaise(true);
	// Поле текста для поиска
	// _findtext = new QLineEdit();

	// Кнопка "Поиск"
	//	_find_start_button = new FlatToolButton("", this, QIcon()); // QPushButton
	_find_start_button->setText(tr("Find"));
	// _findstartbutton->setDefault(true);
	_find_start_button->setEnabled(false);
	_find_start_button->setFixedWidth(50);
	// _findstartbutton->setFlat(true);
	_find_start_button->setAutoRaise(true);

	// Кнопка разворачивания инструментов
	//	_tools_expand = new FlatToolButton("", this);
	_tools_expand->setIcon(QIcon(":/resource/pic/find_in_base_expand_tools.svg"));
	_tools_expand->setEnabled(true);
	_tools_expand->setAutoRaise(true);
}


// Текст поиска и кнопка "Поиск"
void FindScreen::assembly_findtext_and_button(void)
{
	//	_find_text_and_button_tools_area = new QHBoxLayout();
	// QWidget *con = new QWidget(this);
	// QHBoxLayout *addressbar = new QHBoxLayout();
	// addressbar->addWidget(_toolbarsearch);
	// addressbar->addWidget(_chasewidget);
	_find_text_and_button_tools_area->setContentsMargins(0, 0, 0, 0);
	_find_text_and_button_tools_area->setMargin(0);
	_find_text_and_button_tools_area->setSpacing(0);
	// addressbar->setContentsMargins(0, 0, 0, 0);
	// con->setLayout(addressbar);
	_find_text_and_button_tools_area->addWidget(_navigater);
	// toolsAreaFindTextAndButton->addWidget(_history_home);
	// toolsAreaFindTextAndButton->addWidget(_history_forward);
	// toolsAreaFindTextAndButton->addWidget(_history_back);
	// toolsAreaFindTextAndButton->addWidget(_stop_reload);
	// toolsAreaFindTextAndButton->addLayout(addressbar);
	//	_stack_layout = new QVBoxLayout();
	//	_lineedit_stack->addWidget(_toolbarsearch_buffer);

	_find_text_and_button_tools_area->addWidget(_editor_switch);
//	_find_text_and_button_tools_area->addWidget(_switch_search_type);
#ifdef USE_STACK_LAYOUT
	_stack_layout->setSpacing(0);
	_stack_layout->setMargin(0);
	_stack_layout->setContentsMargins(0, 0, 0, 0);
	_stack_layout->addWidget(_lineedit_stack);
	//	_lineedit_stack->setMargin(0);
	//	_lineedit_stack->setSpacing(0);
	_lineedit_stack->setContentsMargins(0, 0, 0, 0);
	_find_text_and_button_tools_area->addLayout(_stack_layout); //
#else
	_find_text_and_button_tools_area->addWidget(_lineedit_stack); // _findtext
#endif // USE_STACK_LAYOUT
// toolsAreaFindTextAndButton->addWidget(con);
#ifdef USE_CLEAR_BUTTON
	auto chase_layout = new QVBoxLayout();
	chase_layout->addWidget(_chasewidget);
	_find_text_and_button_tools_area->addLayout(chase_layout);
#endif // USE_CLEAR_BUTTON
	_find_text_and_button_tools_area->addWidget(_find_start_button);
	_find_text_and_button_tools_area->addWidget(_tools_expand);
	// toolsAreaFindTextAndButton->addStretch();
	// toolsAreaFindTextAndButton->setContentsMargins(0, 0, 0, 0);
}

QToolBar* FindScreen::navigater() const
{
	QToolBar* nav = nullptr;
	for (int i = 0; i < _find_text_and_button_tools_area->count(); i++) {
		auto item_ = _find_text_and_button_tools_area->itemAt(i);
		//		auto item = qobject_cast<QLayoutItem*>(obj);
		auto navigater = item_->widget();
		if (navigater) {
			if (navigater->objectName() == gl_para::_browser_navigater_name) {
				nav = static_cast<QToolBar*>(navigater);
				break;
			}
		}
	}
	return nav;
}

void FindScreen::switch_navigater(QToolBar* navigater_new)
{
	bool need_update = false;
	//	auto widget_list = _find_text_and_button_tools_area->findChildren<QObject*>();
	for (int i = 0; i < _find_text_and_button_tools_area->count(); i++) {
		auto item_ = _find_text_and_button_tools_area->itemAt(i);
		//		auto item = qobject_cast<QLayoutItem*>(obj);
		auto navigater = item_->widget();
		if (navigater) {
			if (navigater->objectName() == gl_para::_browser_navigater_name) {

				_find_text_and_button_tools_area->removeItem(item_);
				//				break;
				navigater->hide();
				need_update = true;
			}
		}
	}
	if (need_update) {
		if (navigater_new) {
			_find_text_and_button_tools_area->insertWidget(0, navigater_new);
			navigater_new->show();
		} else {
			_find_text_and_button_tools_area->insertWidget(0, _navigater);
			_navigater->show();
		}
		_find_text_and_button_tools_area->update();
	}
}

QStackedWidget* FindScreen::lineedit_stack() const
{
	QStackedWidget* ls = nullptr;
	for (int i = 0; i < _find_text_and_button_tools_area->count(); i++) {
		auto item_ = _find_text_and_button_tools_area->itemAt(i);
		//		auto item = qobject_cast<QLayoutItem*>(obj);
		auto lineedit_stack_ = item_->widget();
		if (lineedit_stack_) {
			if (lineedit_stack_->objectName() == gl_para::_browser_lineedit_stack_name) {
				ls = static_cast<QStackedWidget*>(lineedit_stack_);
				break;
			}
		}
	}
	return ls;
}

void FindScreen::switch_stack(QStackedWidget* lineedit_stack_)
{
#ifdef USE_STACK_LAYOUT
	auto clear_layout = [&](QVBoxLayout* _stack_layout) {
		while (_stack_layout->count() > 0) { // (child_item = _item_tags_layout->takeAt(0)) != 0
			auto child_item = _stack_layout->takeAt(0);
			auto widget = child_item->widget();
			_stack_layout->removeItem(child_item);
			if (widget) { // if(child_item->widget() != nullptr && child_item->widget() != 0)
				//					disconnect(static_cast<QLabel*>(widget), &QLabel::linkActivated, this, &EditorWrap::on_click_to_tag);
				widget->hide(); // delete (widget);
			}
			//                                delete child_item;
		}
	};
#endif // USE_STACK_LAYOUT
//	if (lineedit_stack_) {

#ifdef USE_STACK_LAYOUT
	if (_stack_layout->widget() != lineedit_stack_) {
		if (_stack_layout->count() > 0) clear_layout(_stack_layout);
		_stack_layout->addWidget(lineedit_stack_);
		lineedit_stack_->show();
	}
#else
	bool need_update = false;
	//	auto widget_list = _find_text_and_button_tools_area->findChildren<QObject*>();
	auto index = 0;
	for (int i = 0; i < _find_text_and_button_tools_area->count(); i++) {
		auto item_ = _find_text_and_button_tools_area->itemAt(i);
		//		auto item = qobject_cast<QLayoutItem*>(obj);
		auto stack = item_->widget();
		if (stack) {
			if (stack->objectName() == gl_para::_browser_lineedit_stack_name) {

				_find_text_and_button_tools_area->removeItem(item_);
				//				break;
				stack->hide();
				index = i;
				need_update = true;
				break;
			}
		}
	}
	if (need_update) {
		if (lineedit_stack_) {
			_find_text_and_button_tools_area->insertWidget(index, lineedit_stack_);
			lineedit_stack_->show();
		} else {
			_find_text_and_button_tools_area->insertWidget(index, _lineedit_stack);
			_lineedit_stack->show();
		}
		_find_text_and_button_tools_area->update();
	}
#endif  // USE_LINE_EDIT_LAYOUT
	//	}// end if (lineedit_stack_)


	//	else {
	//		if (_stack_layout->widget() != _lineedit_stack) {
	//			if (_stack_layout->count() > 0) clear_layout(_stack_layout);
	//			if (_lineedit_stack) {
	//				_stack_layout->addWidget(_lineedit_stack);
	//				assert(_stack_layout->count() > 0);
	//				_lineedit_stack->show();
	//			}
	//		}
	//	}
}
// Набор опций поиска в виде выпадающих списков
void FindScreen::setup_combooption(void)
{
	// Выбор "Любое слово" - "Все слова"
	//	_word_regard = new FlatComboBox();
	_word_regard->addItem(QIcon(":/resource/pic/find_in_base_any.svg"), tr("Any word"));
	_word_regard->addItem(QIcon(":/resource/pic/find_in_base_all.svg"), tr("All words"));
	_word_regard->setCurrentIndex(appconfig->findscreen_wordregard());

	// Выбор "Только целые слова" - "Подстрока"
	//	_how_extract = new FlatComboBox();
	_how_extract->addItem(QIcon(":/resource/pic/find_in_base_separate.svg"), tr("Whole words"));
	_how_extract->addItem(QIcon(":/resource/pic/find_in_base_substring.svg"), tr("Substring"));
	_how_extract->setCurrentIndex(appconfig->findscreen_howextract());

	// Выбор "Во всей базе" - "В текущей ветке"
	//	_tree_search_area = new FlatComboBox();
	_tree_search_area->addItem(
	    QIcon(":/resource/pic/find_in_base_search_all.svg"), tr("Entire base")); // Вся база
	_tree_search_area->addItem(
	    QIcon(":/resource/pic/find_in_base_search_branch.svg"), tr("In current branch")); // Текущая ветка
	_tree_search_area->setCurrentIndex(appconfig->find_screen_tree_search_area());
	if (appconfig->interface_mode() == "desktop") _tree_search_area->hide();
	if (appconfig->interface_mode() == "mobile") {
		// wordRegard->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
		// howExtract->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
		// treeSearchArea->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

		// wordRegard->showMinimized();
		// howExtract->showMinimized();
		// treeSearchArea->showMinimized();

		_word_regard->setMinimumContentsLength(1);
		_word_regard->setMaximumWidth(calculate_iconsize_px() * 2);
		_word_regard->setMinimumWidth(calculate_iconsize_px() * 2);
		_word_regard->setSizeAdjustPolicy(
		    QComboBox::AdjustToMinimumContentsLengthWithIcon);

		_how_extract->setMinimumContentsLength(1);
		_how_extract->setMaximumWidth(calculate_iconsize_px() * 2);
		_how_extract->setMinimumWidth(calculate_iconsize_px() * 2);
		_how_extract->setSizeAdjustPolicy(
		    QComboBox::AdjustToMinimumContentsLengthWithIcon);

		_tree_search_area->setMinimumContentsLength(1);
		_tree_search_area->setMaximumWidth(calculate_iconsize_px() * 2);
		_tree_search_area->setMinimumWidth(calculate_iconsize_px() * 2);
		_tree_search_area->setSizeAdjustPolicy(
		    QComboBox::AdjustToMinimumContentsLengthWithIcon);
	}
}

// Набор опций поиска в виде выпадающих списков
void FindScreen::assembly_combooption(void)
{
	//        _combo_option_tools_area = new QHBoxLayout();
	// toolsAreaComboOption->addWidget(wordRegard);
	// toolsAreaComboOption->addWidget(howExtract);
	// toolsAreaComboOption->addWidget(treeSearchArea);
	// toolsAreaComboOption->addStretch();
}

void FindScreen::setup_closebutton(void)
{
	// Кнопка закрытия виджета
	//	_close_button = new FlatToolButton("", this);
	_close_button->setVisible(true);
	_close_button->setIcon(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton)); // SP_TitleBarCloseButton
											     // SP_DialogCloseButton
	_close_button->setAutoRaise(true);
	if (appconfig->interface_mode() == "desktop") {
		int w = _close_button->geometry().width();
		int h = _close_button->geometry().height();
		int x = min(w, h); // / 2; // imin(w, h) / 2;
		_close_button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::ToolButton));
		_close_button->setMinimumSize(x, x);
		_close_button->setMaximumSize(x, x);
		_close_button->resize(x, x);
	}
}

void FindScreen::assembly_closebutton(void)
{
	// Вертикальная область с кнопкой закрытия и распоркой
	// чтобы кнопка была вверху
	//	_close_button_tools_area = new QVBoxLayout();
	_close_button_tools_area->setContentsMargins(0, 0, 0, 0);
	_close_button_tools_area->addWidget(_close_button);
	_close_button_tools_area->addStretch();
}

void FindScreen::setup_wherefind_line(void)
{
	//	_where_find_label = new QLabel(tr("Find in: "));

	//	_find_in_pin = new QCheckBox(tr("Pin"));
	_find_in_pin->setChecked(appconfig->findscreen_find_in_field("pin"));

	//	_find_in_name = new QCheckBox(tr("Title"));
	_find_in_name->setChecked(appconfig->findscreen_find_in_field("name"));

	//	_find_in_author = new QCheckBox(tr("Author(s)"));
	_find_in_author->setChecked(appconfig->findscreen_find_in_field("author"));

	//	_find_in_author = new QCheckBox(tr("Author(s)"));
	//	_find_in_author->setChecked(appconfig->findscreen_find_in_field("author"));

	//	_find_in_home = new QCheckBox(tr("Home"));
	_find_in_home->setChecked(appconfig->findscreen_find_in_field("home"));

	//	_find_in_url = new QCheckBox(tr("Url"));
	_find_in_url->setChecked(appconfig->findscreen_find_in_field("url"));

	//	_find_in_tags = new QCheckBox(tr("Tags"));
	_find_in_tags->setChecked(appconfig->findscreen_find_in_field("tags"));

	//	_find_in_text = new QCheckBox(tr("Text"));
	_find_in_text->setChecked(appconfig->findscreen_find_in_field("text"));
}

void FindScreen::assembly_wherefind_line(void)
{
	//	_where_find_line = new QHBoxLayout();
	_where_find_line->setEnabled(false);
	if (appconfig->interface_mode() == "desktop") _where_find_line->addWidget(_where_find_label);
	if (appconfig->interface_mode() == "mobile") _where_find_label->hide();
	_where_find_line->addWidget(_word_regard);
	_where_find_line->addWidget(_how_extract);
	if (appconfig->interface_mode() == "mobile") _where_find_line->addWidget(_tree_search_area);
	_where_find_line->addWidget(_find_in_pin);
	_where_find_line->addWidget(_find_in_name);
	_where_find_line->addWidget(_find_in_author);
	_where_find_line->addWidget(_find_in_home);
	_where_find_line->addWidget(_find_in_url);
	_where_find_line->addWidget(_find_in_tags);
	_where_find_line->addWidget(_find_in_text);

	_where_find_line->addStretch();

	_where_find_line->setContentsMargins(3, 0, 0, 0); // Устанавливаются границы
}

void FindScreen::setup_signals(void)
{
	//	// При каждом изменении текста в строке запроса
	//	connect(_toolbarsearch_buffer, &web::ToolbarSearch::textChanged, this, &FindScreen::enable_findbutton, Qt::UniqueConnection);

	//	// Done in constructor. При нажатии Enter в строке запроса
	//	connect(_toolbarsearch_buffer, &web::ToolbarSearch::return_pressed, // this,
	//	    [this] {
	//		emit _toolbarsearch_buffer->lineEdit()->returnPressed();
	////		    real_url_t<QString>::instance<decltype(static_cast<web::ToolbarSearch*>(nullptr)->search_now(boost::intrusive_ptr<real_url_t<QString>>()))>(_toolbarsearch_buffer->text(),
	////			[&](boost::intrusive_ptr<real_url_t<QString>> real_target_url_) {
	////				return _toolbarsearch_buffer->search_now(real_target_url_); // FindScreen::find_clicked();
	////			});
	//	    });

	//	// При каждом изменении текста извне может вырабатыватся этот сигнал
	//	// Он вырабатывается в слоте setFindText()
	//	connect(this, &FindScreen::text_changed, this, &FindScreen::enable_findbutton);


	// При нажатии кнопки Find
	connect(_find_start_button, &QPushButton::clicked, //this,
	    [this] {
#ifdef USE_STACK_LAYOUT
		    int count_ = _stack_layout->count();
		    assert(count_ > 0);
		    auto lineedit_stack_ = static_cast<QStackedWidget*>(_stack_layout->itemAt(0)->widget());
		    if (lineedit_stack_) {
			    auto toolbar_search = static_cast<web::ToolbarSearch*>(lineedit_stack_->currentWidget());
#else
		    auto toolbar_search = static_cast<web::ToolbarSearch*>(_lineedit_stack->currentWidget());
#endif // USE_STACK_LAYOUT

			    if (toolbar_search) {
				    emit toolbar_search->return_pressed(); //lineEdit()->returnPressed();
									   //				    auto line_edit = search_edit->lineEdit();
									   //				    real_url_t<QString>::instance<decltype(static_cast<web::ToolbarSearch*>(nullptr)->search_now(boost::intrusive_ptr<real_url_t<QString>>(), true))>(
									   //					line_edit->text(), //_toolbarsearch_buffer->text(),
									   //					[&](boost::intrusive_ptr<real_url_t<QString>> real_target_url_) {
									   //						auto topic_new = real_target_url_->value(); //
									   //						auto topic_old = _browser->blogger()->topic();
									   //						bool is_new_topic = true;
									   //						if (topic_new.contains(topic_old) || topic_old.contains(topic_new)) is_new_topic = false;
									   //						return _toolbarsearch_buffer->search_now(real_target_url_, is_new_topic); // FindScreen::find_clicked();
									   //					});
			    }
#ifdef USE_STACK_LAYOUT
		    }
#endif // USE_STACK_LAYOUT
	    });

	// После установки текста извне, вырабатывается этот сигнал
	//	connect(this, &FindScreen::
	find_clicked_after_another_text_changed.connect(
	    //			, //this,
	    [this](const QString& str) {
#ifdef USE_STACK_LAYOUT
		    int count_ = _stack_layout->count();
		    assert(count_ > 0);
		    auto lineedit_stack_ = static_cast<QStackedWidget*>(_stack_layout->itemAt(0)->widget());
		    if (lineedit_stack_) {
			    auto toolbar_search = static_cast<web::ToolbarSearch*>(lineedit_stack_->currentWidget());
#else
		    auto toolbar_search = static_cast<web::ToolbarSearch*>(_lineedit_stack->currentWidget());
#endif // USE_STACK_LAYOUT
			    if (toolbar_search) {
				    auto line_edit = toolbar_search->lineEdit();
				    line_edit->setText(str);
				    emit toolbar_search->return_pressed(); //lineEdit()->returnPressed();
									   //				    real_url_t<QString>::instance<decltype(static_cast<web::ToolbarSearch*>(nullptr)->search_now(boost::intrusive_ptr<real_url_t<QString>>()))>(
									   //					line_edit->text(), //_toolbarsearch_buffer->text(),
									   //					[&](boost::intrusive_ptr<real_url_t<QString>> real_target_url_) {
									   //						return _toolbarsearch_buffer->search_now(real_target_url_); // FindScreen::find_clicked();
									   //					});
									   //				    search_edit->left_widget(search_edit->searchbutton());
									   //				    search_edit->left_widget()->show();
									   //				    search_edit->iconlabel()->hide();
			    }
#ifdef USE_STACK_LAYOUT
		    }
#endif // USE_STACK_LAYOUT
	    });
	//	connect(_switch_search_type, &FlatToolButton::clicked, this, &FindScreen::switch_search_content);
	// При нажатии кнопки разворачивания/сворачивания инструментов поиска
	connect(_tools_expand, &FlatToolButton::clicked, this, &FindScreen::tools_expand_clicked);

	// При нажатии кнопки закрытия
	connect(_close_button, &FlatToolButton::clicked, this, &FindScreen::widget_hide);
	connect(_close_button, &FlatToolButton::clicked, [&] {
		//		auto browser_ = _browser ? static_cast<web::Browser*>(_browser) : gl_paras->main_window()->browser(QString(gl_para::_current_browser), false);
		if (_browser) _browser->updateToolbarActionText(false);
	});

	// Сигналы для запоминания состояния интерфейса
	connect(_word_regard, _word_regard->currentindexchanged, this, &FindScreen::word_regard);

	connect(_how_extract, _how_extract->currentindexchanged, this, &FindScreen::how_extract);

	connect(_tree_search_area, _tree_search_area->currentindexchanged, this, &FindScreen::tree_search_area);

	// assert(globalparameters.vtab());
	// connect(globalparameters.vtab(), &QTabWidget::currentChanged
	// , this, &FindScreen::changedTreeSearchArea    // , &appconfig,
	// &AppConfig::setFindScreenTreeSearchArea
	// );

	connect(_find_in_pin, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_pin);

	connect(_find_in_name, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_name);

	connect(_find_in_author, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_author);

	connect(_find_in_home, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_home);

	connect(_find_in_url, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_url);

	connect(_find_in_tags, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_tags);

	connect(_find_in_text, &QCheckBox::stateChanged, this, &FindScreen::if_find_in_text);

	// boost::intrusive_ptr<TreeItem> (KnowView::*_item_bind)(
	// boost::intrusive_ptr<TreeItem> tab_brother
	// , const QUrl & _find_url
	// , const KnowView::paste_strategy & _view_paste_strategy
	// , equal_url_t _equal
	// ) = &KnowView::item_bind;
	// connect(_toolbarsearch, &web::ToolbarSearch::search,
	// _tree_screen->tree_view(), _item_bind);
}

void FindScreen::setup_ui(void)
{
// _findtable = new FindTableWidget();
// _findtable->hide();
// _progress = new QProgressDialog(this);
#ifdef SHOW_PROCESS_DIALOG
	_progress->hide();
#endif
}

void FindScreen::assembly(void)
{
	//	_central_desktop_layout = new QVBoxLayout();
	if (appconfig->interface_mode() == "desktop") {
		//		_tools_line = new QHBoxLayout();
		_tools_line->setEnabled(true);
		_tools_line->setContentsMargins(0, 0, 0, 0);
		_tools_line->setMargin(0);
		_tools_line->setSpacing(0);
		QVBoxLayout* vline = new QVBoxLayout();
		vline->setContentsMargins(0, 0, 0, 0);
		vline->setMargin(0);
		vline->setSpacing(0);
		vline->addLayout(_find_text_and_button_tools_area);
		_tools_line->addLayout(vline);
		// toolsLine->addLayout(toolsAreaComboOption);
		_tools_line->addLayout(_close_button_tools_area);

		_central_desktop_layout->addLayout(_tools_line);
	}
	if (appconfig->interface_mode() == "mobile") {
		//		_tools_grid = new QGridLayout();
		_tools_grid->setEnabled(true);
		_tools_grid->addLayout(_find_text_and_button_tools_area, 0, 0);
		_tools_grid->addLayout(_close_button_tools_area, 0, 1);
		// toolsGrid->addLayout(toolsAreaComboOption,       1, 0);

		_central_desktop_layout->addLayout(_tools_grid);
	}
	_central_desktop_layout->addLayout(_where_find_line);
	// centralDesktopLayout->addWidget(_findtable, 10);
	_central_desktop_layout->setContentsMargins(0, 0, 0, 0);              // Boundaries removed // Границы убираются
	_central_desktop_layout->setSizeConstraint(QLayout::SetNoConstraint); // SetFixedSize // this setting will lead TableScreen can not
									      // resize!!!

	_central_desktop_layout->setMargin(0);
	_central_desktop_layout->setSpacing(0);

	// whereFindLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	// _findtable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	this->setLayout(_central_desktop_layout);

	switch_tools_expand(appconfig->find_in_base_expand());
}

void FindScreen::adjust_size()
{
	// int height = toolsLine->isEnabled() ? toolsLine->sizeHint().height() : 0 +
	// whereFindLine->isEnabled() ? whereFindLine->sizeHint().height() : 0;
	// setMinimumHeight(height);
	// setMaximumHeight(height);
	// setMinimumWidth(width());
	// setMaximumWidth(width());
	QWidget::adjustSize();
}

void FindScreen::resizeEvent(QResizeEvent* e)
{
	// adjustSize();
	// auto hint = whereFindLine->sizeHint();
	// int height = whereFindLine->sizeHint().height();
	setMinimumHeight(_tools_line->sizeHint().height() + _where_find_line->sizeHint().height());
	setMaximumHeight(_tools_line->sizeHint().height() + _where_find_line->sizeHint().height());
	QWidget::resizeEvent(e);
}

void FindScreen::enable_findbutton(const QString& text)
{
	_find_start_button->setEnabled(!text.isEmpty());
}

//// Slot, with which other widgets can install text to search// Слот, с помощью которого другие виджеты могут устанавливать текст для поиска
//void FindScreen::find_text(QString text_)
//{
//	// _findtext
//	_toolbarsearch_buffer->text(text_);

//	emit text_changed(text_);
//	//	emit find_clicked_after_another_text_changed();
//}

// Слот, срабатывающий при нажатии на кнопку начала поиска
boost::intrusive_ptr<i_t> FindScreen::find_internal_decomposed(QString const& search_text)
{
	boost::intrusive_ptr<i_t> result(nullptr);
	// Поля, где нужно искать (Заголовок, текст, теги...)
	_search_area["pin"] = _find_in_pin->isChecked();
	_search_area["name"] = _find_in_name->isChecked();
	_search_area["author"] = _find_in_author->isChecked();
	_search_area["home"] = _find_in_home->isChecked();
	_search_area["url"] = _find_in_url->isChecked();
	_search_area["tags"] = _find_in_tags->isChecked();
	_search_area["text"] = _find_in_text->isChecked();

	// Проверяется, установлено ли хоть одно поле для поиска
	int findEnableFlag = 0;

	foreach (bool value, _search_area)
		if (value == true) findEnableFlag = 1;
	// Если не отмечены поля для поиска
	if (findEnableFlag != 0) {
		// Выясняется список слов, которые нужно искать
		_search_word_list = text_delimiter_decompose(search_text); //_toolbarsearch->text()
		if (_search_word_list.size() != 0) {
			result = find_implement(search_text);
		} else {
			QMessageBox messageBox(this);
			messageBox.setWindowTitle(tr("Can not start find process"));
			messageBox.setText(
			    tr("The search request is too short. Enter at least one word."));
			messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
			messageBox.exec();

			//		return nullptr;
		}

	} else {
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Can not start find process"));
		messageBox.setText(tr(
		    "Verify that you selected fields for search for "
		    "starting find process."));
		messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
		messageBox.exec();

		//		return nullptr;
	}

	return result;
}

boost::intrusive_ptr<i_t> FindScreen::find_implement(QString const& topic)
{
	//// deprecated:
	// if(globalparameters.vtab_tree()->currentWidget()->objectName() ==
	// record_screen_multi_instance_name
	// && !_tree_screen->tree_view()->current_index().isValid()
	// ) {
	// appconfig->find_screen_tree_search_area(2);
	// }

	// Сохраняется текущая редактируемая запись, чтобы и в ней
	// были найдены введенные перед нажатием Find данные, если они есть
	// find_object<MainWindow>("mainwindow")
	auto rs = dynamic_cast<rs_t*>(gl_paras->vtab_record()->currentWidget());
	if (rs) rs->blogger()->save_text_context();
	//// Очищается таблица результата поиска
	// _findtable->re_initialize();

	const tkm_t* _global_source_model = _tree_screen->view()->know_model_board();
	// Выясняется ссылка на модель дерева данных
	auto _current_model = [&]() {
		return _tree_screen->view()->source_model();
	}; // static_cast<KnowModel *>(_tree_screen->tree_view()->model());

	// QMap<QString, QString> data;
	// QDateTime ctime_dt = QDateTime::currentDateTime();
	// QString ctime = ctime_dt.toString("yyyyMMddhhmmss");
	// data["id"]      = get_unical_id();
	// data["name"]    = _toolbarsearch->text();
	// data["ctime"]   = ctime;
	// data["dir"]     = data["id"];
	// data["file"]    = "text.html";

	// QList<boost::intrusive_ptr<Linker> > _result_list;

	// boost::intrusive_ptr<TreeItem>	final_result(nullptr);
	full_fields_map data;

	// QDateTime	ctime_dt	= QDateTime::currentDateTime();
	QString ctime = get_qtime(); // ctime_dt.toString("yyyyMMddhhmmss");
	// _item->field("ctime", ctime);

	boost::fusion::at_key<id_key>(data) = get_unical_id();
	boost::fusion::at_key<name_key>(data) = ctime;
	boost::fusion::at_key<ctime_key>(data) = ctime;
	boost::fusion::at_key<dir_key>(data) = boost::fusion::at_key<id_key>(data);
	boost::fusion::at_key<file_key>(data) = "text.html";

	boost::intrusive_ptr<i_t> final_result = i_t::dangle_instance(data); // QMap<QString, QString>()
	// Выясняется стартовый элемент в дереве, с которого будет начат поиск
	// Выясняется сколько всего конечных записей
	boost::intrusive_ptr<i_t> _start_item(nullptr);                                          // = boost::intrusive_ptr<TreeItem>(new TreeItem(QMap<QString,
												 // QString>(), nullptr));
	boost::intrusive_ptr<i_t> _session_root_item(_tree_screen->view()->session_root_auto()); // new TreeItem(nullptr, data)
	// _tree_screen->tree_view()->source_model()->item(_tree_screen->tree_view()->current_index())->parent();
	// //

	// auto original_count = _result_item->count_direct();

	// auto _entrance = globalparameters.entrance();

	// for(auto i : _entrance->browsers()) {
	// auto tabm = i->tabmanager();

	// for(int i = 0; i < tabm->count(); i++) {
	// _result_item->child_transfer(0,
	// tabm->webView(i)->page()->record_binder()->bounded_item());
	// }
	// }

	////    auto _candidate_root =
	///find_object<TreeScreen>(tree_screen_singleton_name)->_shadow_candidate_model->_root_item;
	// _result_item->field("id", get_unical_id());
	// _result_item->field("name", _toolbarsearch->text());

	////    TreeScreen *tree_screen =
	///find_object<TreeScreen>(tree_screen_singleton_name);

	////    web::Browser *_browser = nullptr;

	////    if(!reocrd_controller) {
	////        _browser =
	///globalparameters.entrance()->new_browser(QUrl(web::Browser::_defaulthome));
	////        _browser->record_screen()->setObjectName(_toolbarsearch->text());
	////    } else {
	////
	///(*reocrd_controller)()->record_screen()->setObjectName(_toolbarsearch->text());
	////    }

	//// _tree_screen->insert_branch_process(_tree_screen->last_index(), "buffer",
	///true);
	////    std::shared_ptr<RecordTable> _resultset_data =
	///_candidate_root->tabledata();    //
	///std::make_shared<RecordTable>(_resultset_item);

	int _candidate_records = 0;

	// __________________________________________________________________________________________________

	auto browsers_search_prepare = [&](
	    boost::intrusive_ptr<i_t>& _start_item, int& _candidate_records) { // , boost::intrusive_ptr<TreeItem>
		// &_result_item
		// to be done
		// QMap<QString, QString> data;

		// resultset_item.swap(_candidate_root);   // resultset_item ==
		// _candidate_root
		// std::shared_ptr<QDomDocument> doc = std::make_shared<QDomDocument>();
		// auto dommodel = resultset_item->export_to_dom();    //
		// source->init(startItem, QDomElement());
		// search_start_item->tabledata(dommodel);
		// QMap<QString, QString> field_data;

		_start_item = i_t::dangle_instance(); // boost::intrusive_ptr<TreeItem>(new
		// TreeItem(nullptr, QMap<QString,
		// QString>()));
		// boost::intrusive_ptr<TreeItem> _parent_item
		// resultset_item;
		// web::Entrance *_entrance = globalparameters.entrance();
		for (auto& browser : gl_paras->main_window()->browsers()) {
			auto _tab_widget = browser->tab_widget();
			for (int i = 0; i < _tab_widget->count(); i++) {
				_start_item << _tab_widget->webView(i)->page()->host();
				// _start_item->child_rent(item);
			}
		}
		////        assert(_result_item->direct_count() == 0);

		// _result_item = _result_item->active_subset();

		//// resultset_item
		////
		///resultset_record_source->active_subset(_tree_screen->insert_branch_process(_tree_screen->last_index(),
		///"buffer", true));  //
		////            std::make_shared<TableData>();      //
		///assert(_result->size() == 0); //_result->empty();
		_candidate_records = _start_item->count_children_recursive();
	};

	auto current_model_search_prepare = [&](
	    boost::intrusive_ptr<i_t>& _start_item, int& _candidate_records) { // , boost::intrusive_ptr<TreeItem>
		// &_result_item
		//// Индекс текущей выбранной ветки
		// QModelIndex current_item_index =    //
		// find_object<TreeScreen>(tree_screen_singleton_name)
		// _tree_screen->tree_view()->index_current();

		// if(current_item_index.isValid()) {
		//// Текущая ветка
		_start_item = _current_model()->root_item(); // item(current_item_index);

		// Количество элементов (веток) в текущей ветке и всех подветках
		_candidate_records = _current_model()->size_of(_start_item);
		// _result_item = _result_item->active_subset();
		// }
	};

	// Q_UNUSED(branch_search_prepare)

	auto global_search_prepare = [&](
	    boost::intrusive_ptr<i_t>& _start_item, int& _candidate_records) { // , boost::intrusive_ptr<TreeItem>
		// &_result_item	// ,
		// std::shared_ptr<RecordTable> &resultset_data
		// Корневой элемент дерева
		_start_item = _global_source_model->root_item(); // this change the value of
		// local smart pointer, which can't be return to outer start_item, so function parameter type must be a reference.
		// Количество элементов (веток) во всем дереве
		_candidate_records = _global_source_model->count_records_all();
		// assert(_result_item->count_direct() == 0);  //_result_item =
		// _result_item->active_subset();
	};

#ifdef SHOW_PROCESS_DIALOG

	auto prepare_progressbar = [&]() {
		// Показывается виджет линейки наполняемости
		_progress->reset();
		_progress->setLabelText(tr("Search..."));
		_progress->setRange(0, _candidate_records);
		_progress->setModal(true);
		_progress->setMinimumDuration(0);

		_progress->show();

		// Обнуляется счетчик обработанных конечных записей
		_total_progress_counter = 0;
		_cancel_flag = 0;
	};

	(void)prepare_progressbar;
#endif

	auto assert_start_item = [&]() {
		// Если стартовый элемент не был установлен
		// if(!startItem)
		// {
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("Cannot start find process"));
		messageBox.setText(tr("Can\'t set start search element in tree."));
		messageBox.addButton(tr("OK"), QMessageBox::AcceptRole);
		messageBox.exec();
		// return nullptr;
		// } else
		// return startItem;
	};

#ifdef SHOW_PROCESS_DIALOG

	auto close_progressbar = [&]() {
		// Виджет линейки наполняемости скрывается
		_progress->reset();

		_progress->hide();
		_progress->close();
	};
	(void)close_progressbar;
#endif

	auto final_search = [&](
	    boost::intrusive_ptr<i_t>& final_result,       // QList<boost::intrusive_ptr<Linker> >    &_result_list
	    boost::intrusive_ptr<i_t>& _session_root_item, // std::shared_ptr<RecordTable> &resultset_data
	    boost::intrusive_ptr<i_t>& _start_item) -> boost::intrusive_ptr<i_t>& {
		qDebug() << "Start finding in " << _candidate_records << " records";

		_total_progress_counter = 0;
#ifdef SHOW_PROCESS_DIALOG
		prepare_progressbar();
#endif

		// _result_item->clear();

		////Вызывается рекурсивный поиск в дереве
		////        find_recursive(search_start_item, candidate_root);

		////        if(result->size() == 0 &&
		///appconfig->getFindScreenTreeSearchArea() == 2) {
		////            global_search_prepare(start_item, total_records, result);
		////        }

		// std::future<QList<boost::intrusive_ptr<Linker> > >
		final_result = std::async(std::launch::async, [&] {
				       return find_recursive(topic, final_result, _session_root_item, _start_item);
			       }).get();

		// std::thread(&FindScreen::find_recursive, this, _result_list,
		// _session_root_item, _start_item).detach();	//
		// find_recursive(_result_list, _session_root_item, _start_item); //
		// candidate_root->tabledata();
		return final_result; //
				     // find_recursive(final_result, _session_root_item, _start_item);				//
				     // _result_list;
	};

	// deprecated by KnowModel::model_move_as_child_impl in this->find_recursive
	// auto output = [&](boost::intrusive_ptr<TreeItem> &_result_item) { // ,
	// std::shared_ptr<RecordTable> &resultset_data

	//// После вставки всех данных подгоняется ширина колонок
	////        _findtable->updateColumnsWidth();
	// _tree_screen->view_paste_from_search(TreeIndex(_current_source_model,
	// _tree_screen->tree_view()->current_index())
	// , _result_item, [&](boost::intrusive_ptr<TreeItem> it)->bool {return
	// it->field("name") == _result_item->field("name");}
	// ); // dump to table screen
	// };

	// __________________________________________________________________________________________________

	//// stage 1, should search current tree_item and all browsers
	// if( // appconfig->getFindScreenTreeSearchArea() == 2
	////        globalparameters.vtab()->currentWidget()->objectName() ==
	///table_screen_singleton_name
	////        &&
	///!find_object<TreeScreen>(tree_screen_singleton_name)->getCurrentItemIndex().isValid()
	// _result_item->count_direct() > 0
	// ) { // search in last search result

	browsers_search_prepare(_start_item, _candidate_records); // , _result_item
	if (_start_item) {
		if (0 != _candidate_records) {
			final_result = final_search(final_result, _session_root_item, _start_item);
			_tree_screen->enable_up_action(); // !_is_search_global
							  // _selected_branch_as_pages != _tree_screen->know_root()->root_item()
		}
		// }
		// stage 2
		if (0 == final_result->count_direct()) {
			current_model_search_prepare(_start_item, _candidate_records);
			if (_start_item) {
				if (0 != _candidate_records) {
					final_result = final_search(final_result, _session_root_item, _start_item);
					_tree_screen->enable_up_action(); // !_is_search_global
									  // _selected_branch_as_pages != _tree_screen->know_root()->root_item()
				}
				// stage 3
				if (0 == final_result->count_direct()) { // (_result_item->count_direct() -
					// original_count)
					// auto tree_screen =
					// find_object<TreeScreen>(tree_screen_singleton_name);
					// tree_screen->delete_one_branch(_search_model->index_item(_search_model->findChild<boost::intrusive_ptr<TreeItem>>(QString("buffer"))));

					// if( // appconfig->getFindScreenTreeSearchArea() == 1
					//// globalparameters.vtab()->currentWidget()->objectName() ==
					///tree_screen_singleton_name
					// find_object<TreeScreen>(tree_screen_singleton_name)->getCurrentItemIndex().isValid()
					// ) { // If you want to search the current branch // Если нужен поиск
					// в текущей ветке
					// branch_search_prepare(start_item, total_records, _result);
					// }

					// if( // appconfig->getFindScreenTreeSearchArea() == 0
					// globalparameters.vtab()->currentWidget()->objectName() ==
					// tree_screen_singleton_name
					// &&
					// !find_object<TreeScreen>(tree_screen_singleton_name)->getCurrentItemIndex().isValid()
					// ) { // Если нужен поиск во всем дереве

					global_search_prepare(_start_item, _candidate_records); // , _resultset_data
					if (_start_item) {
						if (0 != _candidate_records) {
							final_result = final_search(final_result, _session_root_item, _start_item);
							_tree_screen->enable_up_action(); // _tree_screen->know_branch()->root_item()->id()
											  // != _search_model->root_item()->id()
											  // // !_is_search_global
											  // _selected_branch_as_pages !=
											  // _tree_screen->know_root()->root_item()
						}
						// }
					} else
						assert_start_item(); // return nullptr;
				}
			} else
				assert_start_item(); // return nullptr;
		}
	} else
		assert_start_item(); // return nullptr;

#ifdef SHOW_PROCESS_DIALOG
	close_progressbar();
#else
	gl_paras->status_bar()->showMessage(
	    "searched node(s) : " + QString::number(_total_progress_counter), 2000); // across thread message
#endif

	//// } else {

	// close_progressbar();
	//// create a new note and open in browser
	//// if it is a web address, open it
	//// else, open from search engine
	//// }
	//// output(_result_item);

	////    if(_result_list.size() > 0)for(auto it : _result_list)final_result <<
	///it->host();	//= _result_list.at(0)->host();

	std::vector<boost::intrusive_ptr<i_t>> pre;
	pre.reserve(final_result->count_direct());
	for (auto il : final_result->child_linkers()) {
		auto it = il->host();
		pre.push_back(it);
	}
	std::sort(std::begin(pre), std::end(pre), [&](boost::intrusive_ptr<i_t> t1, boost::intrusive_ptr<i_t> t2) {
		return t1->rating() > t2->rating();
	});
	full_fields_map data_;

	// QDateTime	ctime_dt	= QDateTime::currentDateTime();
	QString ctime_ = get_qtime();
	boost::fusion::at_key<id_key>(data_) = get_unical_id();
	boost::fusion::at_key<name_key>(data_) = ctime_;
	boost::fusion::at_key<ctime_key>(data_) = ctime_;
	boost::fusion::at_key<dir_key>(data_) = boost::fusion::at_key<id_key>(data_);
	boost::fusion::at_key<file_key>(data_) = "text.html";
	boost::intrusive_ptr<i_t> _final_result = i_t::dangle_instance(data_);
	// if(pre.size() > 0)_final_result << pre[0];
	for (std::vector<boost::intrusive_ptr<i_t>>::size_type i = 0; i < pre.size();
	     i++) {
		if (i < 10)
			_final_result << pre[i];
		else
			break;
	}
	return _final_result; // ->record_table();
}

boost::intrusive_ptr<i_t>& FindScreen::find_recursive(QString const& topic, boost::intrusive_ptr<i_t>& final_result, boost::intrusive_ptr<i_t> _session_root_item, boost::intrusive_ptr<i_t> _start_item //, bool new_topic
    )
{
	//	(void)new_topic;
	//	auto tree_view_ = _tree_screen->view();
	//	auto current_model_ = [&] {return tree_view_->source_model();};
	////    // Если была нажата отмена поиска
	////    if(_cancel_flag == 1)return _result_item;
	if (_cancel_flag != 1) {
		//// Если ветка зашифрована, и пароль не был введен
		// if(_start_item->field(boost::mpl::c_str < crypt_type > ::value) == "1" &&
		// globalparameters.crypt_key().length() == 0) {
		// return _result_item;
		// }
		if (!(_start_item->field<crypt_key>() == crypt_value(true) && gl_paras->crypt_key().length() == 0)) {
			// Если в ветке присутсвует таблица конечных записей
			if (_start_item->count_direct() > 0) {
				// auto _source_model = [&]() {return tree_view->source_model();};
				////        auto _current_item =
				///_tree_screen->tree_view()->current_item();
				////        boost::intrusive_ptr<TreeItem> _current_branch_root;
				////
				///if(_current_item->is_registered_to_browser())_current_branch_root =
				///_current_item->parent();
				////        else _current_branch_root = _current_item;
				//// Обработка таблицы конечных записей
				//// Выясняется ссылка на таблицу конечных записей
				////        auto _start_item = _start_item;
				// Перебираются записи таблицы
				for (int i = 0; i < _start_item->count_direct(); i++) {
					auto candidate = _start_item->child_direct(i);
					++_total_progress_counter;
#ifdef SHOW_PROCESS_DIALOG
					// Обновляется линейка наполняемости
					_progress->setValue(_total_progress_counter);
					sapp_t::instance()->processEvents();
					if (!_progress->wasCanceled()) {
// if(_progress->wasCanceled()) {
// _cancel_flag = 1;
// return _result_item;
// }
#else
					sapp_t::instance()->processEvents();
#endif
						// globalparameters.status_bar()->showMessage("searching node(s) : "
						// + QString::number(_total_progress_counter), 1000);// across
						// thread message

						// езультаты поиска в полях
						QMap<QString, bool> iteration_search_result;

						iteration_search_result["pin"] = false;
						iteration_search_result["name"] = false;
						iteration_search_result["author"] = false;
						iteration_search_result["home"] = false;
						iteration_search_result["url"] = false;
						iteration_search_result["tags"] = false;
						iteration_search_result["text"] = false;

						// Текст в котором будет проводиться поиск
						QString inspect_text;

						// Цикл поиска в отмеченных пользователем полях
						QMapIterator<QString, bool> j(iteration_search_result);
						while (j.hasNext()) {
							j.next();
							QString key = j.key();
							// Если в данном поле нужно проводить поиск
							if (_search_area[key] == true) {
								if (key != "text") {
									// Поиск в обычном поле
									inspect_text = _start_item->child_direct(i)->field_dynamic(key);
									iteration_search_result[key] = find_in_text_process(inspect_text);
								} else {
									// Поиск в тексте записи
									if (_start_item->child_direct(i)->file_exists())
										inspect_text = _start_item->text(i);
									else
										inspect_text = QString();
									QTextDocument textdoc;
									textdoc.setHtml(inspect_text);
									iteration_search_result[key] = find_in_text_process(textdoc.toPlainText());
								}
							}
						} // Закрылся цикл поиска в полях

						// Проверяется, есть ли поле, в котором поиск был успешен
						int found_flag = 0;

						foreach (bool value, iteration_search_result)
							if (value == true) found_flag = 1;
						// Если запись найдена
						if (found_flag == 1) {
							// qDebug() << "Find succesfull in " <<
							// candidate->field<name_type>();
							// QString pin0 = curritem->field("pin");
							// QString pin1 = searchRecordTable->field("pin", i);
							// // work
							//// В таблицу результатов поиска добавляются данные
							//// Имя записи
							//// Имя ветки
							//// Теги
							//// Путь к ветке
							//// ID записи в таблице конечных записей
							// _findtable->addRow(searchRecordTable->field("name", i)
							// , searchRecordTable->field("pin", i)	//
							// curritem->getField("pin")
							// , curritem->getField("name")
							// , searchRecordTable->field("tags", i)
							// , curritem->getPath()
							// , searchRecordTable->field("id", i)
							// );
							if (candidate->is_lite()) candidate->to_fat();
							// {// move to search result
							// auto					browser
							// =
							// globalparameters.main_window()->vtab_record()->activated_browser();
							// auto					record_controller
							// = browser->record_screen()->record_ctrl();
							// auto					tab_brother
							// = record_controller->view()->current_item();
							// boost::intrusive_ptr<RecordIndex>	record_index
							// = RecordIndex::instance([&] {return
							// record_controller->source_model();}, tab_brother, candidate);
							////                            if(record_index){
							////                            if(  (candidate->parent() !=
							///_session_root_item->parent())		//
							///_current_item->parent())
							////                              && !
							///_session_root_item->item_direct([&](boost::intrusive_ptr<const
							///Linker> il){return il == candidate->linker();})
							////                                ){
							////                                auto result =
							///browser->item_bind(record_index);
							////
							///result->activate(std::bind(&web::Entrance::find,
							///globalparameters.entrance(), std::placeholders::_1));
							////                                _result_list <<
							///result->linker();
							/////
							////                            }else{
							//// auto previous_item =
							///_source_model()->item(tree_view->previous_index());
							// auto result = browser->page_instantiate(record_index);
							// result->activate(std::bind(&HidableTabWidget::find,
							// globalparameters.main_window()->vtab_record(),
							// std::placeholders::_1));
							// }

							auto alternative = final_result->contains_direct([&](boost::intrusive_ptr<const Linker> il) {
								return il->host() == candidate || il->host()->field<id_key>() == candidate->field<id_key>() || url_equal(detail::to_string(il->host()->field<home_key>()), detail::to_string(candidate->field<home_key>())) || url_equal(il->host()->field<url_key>(), candidate->field<url_key>());
							});
							if (!alternative) {
								//								auto tags_ =
								//candidate->field<tags_type>();
								//								if(tags_.size()
								//> 0)
								//									while(tags_.at(0)
								//== ',' || tags_.at(0) == ';') tags_.remove(0, 1);
								//								auto tag_list
								//= tags_.split(QRegExp("[,:]+"), QString::SkipEmptyParts);
								//								auto
								//search_topic = _toolbarsearch->text();
								//								if(tag_list.size()
								//> 0){
								//									if(!tag_list.contains(search_topic))
								//tags_ += "," + search_topic;
								//								}else //
								//(tag_list.size() == 0)
								//									tags_ +=
								//search_topic;
								//								candidate->field<tags_type>(tags_);
								candidate->topic_append(topic //_browser->blogger()->topic()
								    );                        //_toolbarsearch_buffer->text()

								final_result << candidate; // result->linker();
							}
							// else if(alternative != candidate)
							// TreeLevel::instance(TreeIndex::require_treeindex(current_model_,
							// alternative), candidate)->merge();

							// }
							// }else{
							// candidate->activate(std::bind(&web::Entrance::find,
							// globalparameters.entrance(), std::placeholders::_1));
							// _result_list << candidate->linker();
							// }
							// else {
							// find_recursive(_start_item->child(i), _result_item);
							// }
						}
						if (candidate)
							if (candidate->count_direct() > 0) find_recursive(topic, final_result, _session_root_item, candidate);
#ifdef SHOW_PROCESS_DIALOG
					} else {
						// if(_progress->wasCanceled()) {
						_cancel_flag = 1;
						_progress->reset();
						_progress->hide();
						_progress->close();
						// return _result_item;
						// }
					}
#else
#endif
				} // Закрылся цикл перебора записей в таблице конечных записей
			}         // Закрылось условие что в ветке есть таблица конечных записей

			//// Рекурсивная обработка каждой подчиненной ветки
			// for(int i = 0; i < _start_item->current_count(); i++)
			// find_recursive(_start_item->child(i), _result_item);
		}
	}
	return final_result; // _result_item;
}

// Поиск в переданном тексте
// Учитываются состояния переключателей wordregard и howextract
bool FindScreen::find_in_text_process(const QString& text)
{
	int findWordCount = 0;
	int findFlag = 0;
	// Перебираются искомые слова
	for (int i = 0; i < _search_word_list.size(); ++i) {
		findFlag = 0;
		// Если надо найти совпадение целого слова
		if (_how_extract->currentIndex() == 0) {
			// Текст разбивается на слова с очисткой от лишних знаков
			// и проверяется, есть ли в полученном списке текущее слово
			if (text_delimiter_decompose(text).contains(_search_word_list.at(i), Qt::CaseInsensitive)) findFlag = 1;
		} else {
			// Если надо найти слово как подстроку
			if (text.contains(_search_word_list.at(i), Qt::CaseInsensitive)) findFlag = 1;
		}
		// Если слово было найдено, количество найденных слов увеличивается
		if (findFlag == 1) findWordCount++;
		// Если ищется хотя бы одно совпадение
		if (findFlag == 1 && _word_regard->currentIndex() == 0) return true; // То при первом же совпадении цикл прекращается
	}
	// Искалось хотябы одно совпадение, но небыло найдено
	if (_word_regard->currentIndex() == 0)
		return false;
	else {
		// Иначе требовалось найти все слова в запросе
		if (findWordCount == _search_word_list.size())
			return true;
		else
			return false;
	}
}

void FindScreen::word_regard(int pos)
{
	appconfig->findscreen_wordregard(pos);
}

void FindScreen::how_extract(int pos)
{
	appconfig->findscreen_howextract(pos);
}

void FindScreen::tree_search_area(int pos)
{
	appconfig->find_screen_tree_search_area(pos);
}

void FindScreen::if_find_in_pin(int state)
{
	if_find_in_field("pin", state);
}

void FindScreen::if_find_in_name(int state)
{
	if_find_in_field("name", state);
}

void FindScreen::if_find_in_author(int state)
{
	if_find_in_field("author", state);
}

void FindScreen::if_find_in_home(int state)
{
	if_find_in_field("home", state);
}

void FindScreen::if_find_in_url(int state)
{
	if_find_in_field("url", state);
}

void FindScreen::if_find_in_tags(int state)
{
	if_find_in_field("tags", state);
}

void FindScreen::if_find_in_text(int state)
{
	if_find_in_field("text", state);
}

void FindScreen::if_find_in_field(QString fieldname, int state)
{
	bool i;
	if (state == Qt::Checked)
		i = true;
	else
		i = false;
	appconfig->findscreen_find_in_field(fieldname, i);
}

void FindScreen::widget_show(void)
{
	appconfig->findscreen_show(true);
	this->show();

	// При появлении виджета курсор должен сразу стоять на поле ввода
	// _findtext
	//	_toolbarsearch_buffer->setFocus();
}

// Полное сокрытие виджета
void FindScreen::widget_hide(void)
{
	// Запоминается размер сплиттера перед скрытием виджета
	QSplitter* findSplitterRel = gl_paras->find_splitter(); // find_object<QSplitter>("find_splitter");
	appconfig->findsplitter_sizelist(findSplitterRel->sizes());

	// Виджет скрывается
	appconfig->findscreen_show(false);
	this->close();
}

//void FindScreen::switch_search_content()
//{
//	if (_browser) {
//		auto blogger_ = _browser->blogger();
//		if (blogger_) {
//			auto topic = blogger_->topic();
//			int count_ = _stack_layout->count();
//			assert(count_ > 0);
//			auto stack = static_cast<QStackedWidget*>(_stack_layout->itemAt(0)->widget());
//			if (stack) {
//				auto search_edit = static_cast<web::ToolbarSearch*>(stack->currentWidget());
//				if (search_edit) {
//					auto line_edit = search_edit->lineEdit();
//					auto url_ = _browser->tab_widget()->current_item()->field<url_key>();

//					if (line_edit->text() == detail::to_qstring(url_)) {
//						if (search_edit->inactiveText() == web::SearchLineEdit::_default_tip) {
//							search_edit->setInactiveText(topic);
//							line_edit->setText(topic);
//						} else
//							line_edit->setText(search_edit->inactiveText());
////						search_edit->left_widget(search_edit->searchbutton());
////						search_edit->left_widget()->show();
////						search_edit->iconlabel()->hide();
//					} else {
//						line_edit->setText(detail::to_qstring(url_));
////						search_edit->left_widget(search_edit->iconlabel());
////						search_edit->left_widget()->show();
////						search_edit->searchbutton()->hide();
//					}
//				}
//			}
//		}
//	}
//}

// Слот, срабатывающий при клике на кнопку expand
void FindScreen::tools_expand_clicked(void)
{
	// Если нужно сомкнуть инструменты
	if (_find_in_name->isVisible()) {
		switch_tools_expand(false);
		appconfig->find_in_base_expand(false);
	} else {
		switch_tools_expand(true);
		appconfig->find_in_base_expand(true);
	}
}

void FindScreen::switch_tools_expand(bool flag)
{
	// toolsAreaComboOption->setVisible(flag);
	// whereFindLine->setVisible(flag);
	// Выпадающие списки скрываются в мобильном интерфейсе, так как они на
	// отдельной строке
	if (appconfig->interface_mode() == "mobile") {
		_find_in_pin->setVisible(flag);
		_word_regard->setVisible(flag);
		_how_extract->setVisible(flag);
		_tree_search_area->setVisible(flag);
	}
	// Надпись Find in видна и управляется только в desktop режиме интерфейса
	if (appconfig->interface_mode() == "desktop") _where_find_label->setVisible(flag);
	// Флаги поиска скрываются для любого интерфейса, так как они всегда находятся
	// на отдельной строке
	_word_regard->setVisible(flag);
	_how_extract->setVisible(flag);
	// treeSearchArea->setVisible(flag);
	_find_in_pin->setVisible(flag);
	_find_in_name->setVisible(flag);
	_find_in_author->setVisible(flag);
	_find_in_home->setVisible(flag);
	_find_in_url->setVisible(flag);
	_find_in_tags->setVisible(flag);
	_find_in_text->setVisible(flag);

	_where_find_line->setEnabled(flag);
	this->adjust_size();
}

web::Browser* FindScreen::browser()
{
	if (!_browser) {

		auto v_tab = gl_paras->vtab_record();
		if (v_tab) {
			auto rs_pre = v_tab->currentWidget();
			auto rs = dynamic_cast<rs_t*>(rs_pre);
			if (rs) {
				_browser = rs->browser();
			}
		}
	}
	if (!_browser) {

		_browser = real_url_t<url_value>::instance<web::Browser*>(web::Browser::_defaulthome,
		    [&](boost::intrusive_ptr<real_url_t<url_value>> real_target_url_) {
			    return gl_paras->main_window()->browser(real_target_url_, true);
		    });
	}
	assert(_browser);
	return _browser;
}

void FindScreen::browser(web::Browser* bro)
{
	//	if (_browser != bro) {//browser_ref might undefined
	_browser = bro;
	//		static_cast<QTabWidget*>(_browser->tab_widget())->currentChanged(_browser->tab_widget()->currentIndex());
	//	}
}

//web::ToolbarSearch* FindScreen::toolbarsearch() const { return _toolbarsearch; }
//// dangerous!
// void FindScreen::remove_child(const QString &id)
// {
// _selected_branch_as_pages->remove_child(id);    // _findtable->remove_id(id);
// }

//// dangerous!
// void FindScreen::remove_child(const int row)
// {
// _selected_branch_as_pages->remove_child(row);    //
// _findtable->remove_row(row);
// }
