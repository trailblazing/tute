/*******************************************************************************

    QTM - Qt-based blog manager
    Copyright (C) 2006-2010 Matthew J Smith

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

// EditingWindow.h - Headers for QTM main window class.

#ifndef CATKIN_H
#define CATKIN_H
#include <functional>
#include <memory>
// #include "ui_EditingWindowBase.h"
//#include "ui_SideWidget.h"
//#include "ui_aboutbox.h"
// #include "ui_account_form.h"
#include "accounts_dialog.h"
#include "highlighter.h"
#include "libraries/fixed_parameters.h"
#include "libraries/flat_control.h"
#include "libraries/global_parameters.h"

#include "models/app_config/app_config.h"
#include "prefs_dialog.h"
#include "qij_search_widget.h"
#include "ui_ImageEntry.h"
#include "ui_LinkEntry.h"
#include "ui_NewCategoryForm.h"
#include "ui_password-form.h"
#include "utility/add_action.h"
#include "utility/expose.h"
#include "utility/lease.h"
// #include "ui_ListDialog.h"
//#ifdef USE_SYSTRAYICON
//#include "sys_tray_icon.h"
//#endif
// #include "Application.h"

#include <QColor>
#include <QHash>
#include <QMainWindow>
#include <QString>
#include <QWidget>
#include <QtXml>

#ifndef DONT_USE_SSL
#include <QSslError>
#endif

class QWidget;
class QStackedWidget;
// class QSplitter;
class QTextBrowser;
class QPushButton;
class QProgressBar;
class QAction;
class QResizeEvent;
class QComboBox;
// class QHttp;
// class SafeHttp;
class QHttpResponseHeader;
class QTextEdit;
class QHBoxLayout;
class QRegExpValidator;
class QHostInfo;
class QSystemTrayIcon;
class QMenu;
class QToolBar;
class FlatToolButton;
class QNetworkAccessManager;
class QNetworkReply;
class FlatToolButton;
class sapp_t;
class SideTabWidget;
class EditorTextArea;
class rs_t;
class SysTrayIcon;
class HidableTab;

namespace app {
	struct RecentFile;
}
// class ExposedSappRecentFile;

using namespace Ui;

#ifdef Q_OS_WIN
#if QT_VERSION >= 0x050000
#define WIN_QTV
class QWinTaskbarButton;
class QWinTaskbarProgress;
#endif // QT_VERSION >= 0x050000
#define PROPERSEPS(x) x.replace("/", "\\")
#else
#define PROPERSEPS(x) x
#endif

class QSplitter;

class SuperMenu;

extern std::shared_ptr<AppConfig> appconfig;
extern const char* global_root_id;

namespace web {
	template <typename>
	class Docker;
	class Browser;
}

class Blogger : public QMainWindow
#ifdef USE_SIGNAL_CLOSE
		,
		public boost::intrusive_ref_counter<Blogger, boost::thread_safe_counter>,
		public sd::renter //<Blogger>
#endif                            //USE_SIGNAL_CLOSE
{                                 // QWidget
	Q_OBJECT
	typedef QMainWindow super;

    public:
	// EditingWindow( QWidget *widget = 0 );
	/*  Catkin( QList<QString>, QList<QString>, int,
             QWidget *parent = 0 );*/
	Blogger(const QString& new_post_topic = gl_para::_default_topic, // global_root_id// get_unical_id()
	    const QString& new_post_content = gl_para::_default_post,    // (std::string("Welcome to \"") + gl_para::_default_topic + "\" topic").c_str() // = QString()
	    QStringList hide_editor_tools_ = appconfig->hide_editor_tools(),
	    const QByteArray& state_ = QByteArray(),
	    Qt::WindowFlags flags = Qt::Widget);
	~Blogger();
	template <typename T>
	struct initialize_prior_to {
		static constexpr bool value = true;
	};
#ifdef USE_SYSTRAYICON
	void setSTI(SysTrayIcon*);
#endif
	static bool caseInsensitiveLessThan(const QString&, const QString&);
	bool handleArguments();

	QString postTitle();
	static void positionWidget(QWidget*, QWidget*);

	typedef enum _hbiz {
		None,
		_blogger_getUsersBlogs,
		_metaWeblog_newPost,
		_metaWeblog_editPost,
		_metaWeblog_newMediaObject,
		_mt_publishPost,
		_mt_getCategoryList,
		_mt_setPostCategories,
		_wp_getTags,
		_wp_newCategory,
		_wp_newPost,
		_wp_editPost
	} HttpBusinessType;
	TEXTEDIT* editor();
	QStackedWidget* main_stack();

	//	rs_t *record_screen();
	web::Docker<Blogger>* editor_docker();
	//	static QString purify_topic(const QString &topic);
	void metaeditor_sychronize();

    private:
	void doUiSetup();
	void checkForEmptySettings();
	void setEditorColors();
	void setTextFonts();
	void setInitialAccount();
	void readSettings();
	void readServerSettings();
	// void copyOldSettingsToNewDefaultElement();
	void writeSettings();
	void callRefreshCategories();
	QString processWithMarkdown(const QString&);
	void blogger_getUsersBlogs(QByteArray);
	void metaWeblog_newPost(QByteArray);
	void metaWeblog_editPost(QByteArray);
	void metaWeblog_newMediaObject(QByteArray);
	void mt_publishPost(QByteArray);
	void mt_getCategoryList(QByteArray);
	void mt_setPostCategories(QByteArray);
	void wp_getTags(QByteArray);
	void wp_newCategory(QByteArray);
	void wp_newPost(QByteArray);
	void wp_editPost(QByteArray);
	void submitMTEdit();
	void submitWPEdit();
	bool saveCheck();
	bool saveCheck(bool);
	QDomElement XmlValue(QDomDocument&, QString, QString, QString memberType = "param");
	QDomElement XmlMember(QDomDocument&, QString, QString, QString);
	QDomElement XmlMethodName(QDomDocument&, QString);
	QDomElement XmlRpcArray(QDomDocument&, QString, QList<QString>);
	QDomElement XmlRpcArray(QDomDocument&, QString, QString, QList<QString>);
	void setNetworkActionsEnabled(bool);
	QString& getHTMLList(QString, QString&);
	void saveAutoLinkDictionary();
	void loadAutoLinkDictionary();
	QByteArray toBase64(QByteArray&);
	void addToConsole(const QString&);
	void handleEnableCategories();
	void extractAccountAttributes();
	QString decodeXmlEntities(QString);
	void placeNetworkRequest(HttpBusinessType, QByteArray&);
	// void setHost( QHttp *, QString &, QString &, bool );
	void setEditActionsEnabled(bool);
	void purgeBlankAccounts();

	void assembly_closebutton(void);

	void setup_closebutton(void);

    public:
	static constexpr char _default_filename[] = "text";

    private:
	//	sapp_t *_app;

	//	QString _editors_shared_full_path_name = "";
	QString _current_topic_name = "";
	QString _current_topic_full_folder_name = "";
	QString _current_topic_full_config_name = "";

	std::shared_ptr<QSettings> _topic_editor_config;

	int currentAccount, currentBlog, loadedEntryBlog;
	QString loadedAccountId;
	QString applicationVersion;
	QString server;
	QString location;
	QString login;
	QString password;
	QString port;

	QString _local_storage_file_extension;
	QString remoteFileLocation;
	QString lastAccountID;
	bool useHTTPS, categoriesEnabled, entryBlogged, useNewWindows, savePassword,
	    postAsSave, noPassword, initialChangeBlog, allowComments, allowTB,
	    postDateTime, copyTitle, useWordpressAPI, allowRegexSearch,
	    useTwoNewlines, useMarkdown, doMarkdownWhenPosting, stripParaTags,
	    enableHighlighting, suppressReadMore, useUtf8;
	QHash<QString, bool*> accountAttributes;
	QHash<QString, QString*> accountStrings;
	bool _entry_ever_saved, _clean_save, _no_auto_save, _no_alpha_cats,
	    _network_actions_enabled;
	QList<QString> usersBlogs;
	QList<QString> categoryList;
	QStringList usersBlogURIs, usersBlogNames, usersBlogIDs;
	QStringList categoryNames, categoryIDs;
	QStringList qtmaccounts_xml;
	QHash<QString, QString> autoLinkDictionary;
	QHash<QString, QString> autoLinkTitleDictionary;
	QHash<QString, int> autoLinkTargetDictionary;
	QByteArray responseData;
	QString entryNumber, dateTime;

	QString _filename = "";
	int primaryCat;
	QString otherCatsList;
	QRegExpValidator* tagValidator;
	QString editorFontString, previewFontString, consoleFontString;
	QString perlPath, markdownPath;
	QColor editorBgColor, editorFgColor, previewBgColor, previewFgColor,
	    consoleBgColor, consoleFgColor;
	QColor tagBgColor, tagFgColor, entityBgColor, entityFgColor, commentBgColor,
	    commentFgColor, linkFgColor, linkBgColor;
	bool tagBold, tagItalic, entityBold, entityItalic, commentBold, commentItalic,
	    linkBold, linkItalic, linkUnderline;

	bool useBloggerTitleFormatting;
	int bloggerTitleFormat;
	QStringList bloggerTitleFormatStrings;
	QLabel* _dirty_indicator;
	int STI2ClickFunction;
	QDomDocument _accounts_dom;
	QDomElement accountsElement, currentAccountElement, currentBlogElement,
	    currentCategoryElement;
	QString currentAccountId, currentBlogid;
	QString userAgentString;
	QByteArray userAgentArray;

	QAction* recentFileActions[10];
	QAction* removeKeywordTag_forList;
	QAction* addKeywordTag_forList;
	QAction* addKeywordTag_forAvailTags;
	QAction* _refreshKeywordTags;
	QList<std::shared_ptr<app::RecentFile>> recentFiles;
	QString checkBoxName(QString);
#ifdef USE_SYSTRAYICON
	SysTrayIcon* sti;
#endif
	QHash<QString, QChar> xmlEntities;

	// QToolBar *toolBar;
	// FlatToolButton
	// FlatToolButton *action_Open;
	// FlatToolButton *action_Save;
	// FlatToolButton *_action_bold;
	// FlatToolButton *_action_italic;
	// FlatToolButton *_action_underline;
	// FlatToolButton *_action_blockquote;
	// FlatToolButton *_action_link;
	// FlatToolButton *_action_image;
	// FlatToolButton *_action_more;
	// FlatToolButton *_action_preview;
	// FlatToolButton *action_Blog_this;
	QAction *addWPTagAction, *removeWPTagAction, *refreshWPTagsAction,
	    *addTechTagAction, *addClipTechTagAction, *removeTechTagAction,
	    *addTBPingAction, *addClipPingAction, *removeTBPingAction;

    private slots:
	void choose(const QString fname = QString());
	void openRecentFile();
	void save_impl(const QString& file_name_with_full_path, bool exp = false);
	void exportEntry();
//	void saveAll();
	void saveAs(bool exp = false);
	void stopThisJob();
	// void handleDone( bool );
	void handleDone(QNetworkReply*);
// void handleResponseHeader( const QHttpResponseHeader & );
#ifndef DONT_USE_SSL
	void handleSslErrors(const QList<QSslError>&);
#endif
	void changeCurrentBlog(int);
	void changeAccount(int);
	void extractAccountDetails();
	void changeBlog(int, bool fromChangeAccount = false);
	void makeBold();
	void makeItalic();
	void makeUnderline();
	void makeBlockquote();
	void makePara();
	void insertLink(bool isAutoLink = false);
	void insertLinkFromClipboard();
	void insertSelfLink();
	void insertAutoLink();
	void insertImage();
	void insertImageFromClipboard();
	void insertMore();
	// void cut();

	QString removeReadMore(QString&);

	void pasteAsMarkedParagraphs();
	void pasteAsBlockquote();
	void pasteAsMarkdownBlockquote();
	void pasteAsUnorderedList();
	void pasteAsOrderedList();
	// void undo();
	// void redo();
	void makeUnorderedList();
	void makeOrderedList();
	void showHighlightedURL(const QString&);
	void blogThis();
	void newMTPost(); // formerly blogThis()
	void newWPPost();
	void updatePostCategories();
	void setPostCategories();
	void publishPost();
	void setLoadedPostCategories();
	void doWhatsThis();
	void doViewBasicSettings();
	void doViewCategories();
	void changeOtherCatsHeading();
	void doViewExcerpt();
	void doViewKeywordTags();
	void doViewTechTags();
	void doViewTBPings();
	void handleSideWidgetPageSwitch(int);
	void newChildCategory();
	void newCategory(int parentCategory = 0);
	void addKeywordTag();
	void addKeywordTagFromLineEdit();
	void addKeywordTagFromAvailTags();
	void refreshKeywordTags();
	void removeKeywordTag();
	void addTechTag();
	void addClipTag();
	void addTechTagFromLineEdit();
	void addTechTagFromAddButton();
	void removeTechTag();
	void addTBPing();
	void addClipTBPing();
	void addTBPingFromLineEdit();
	void addTBPingFromAddButton();
	void removeTBPing();
	void handleInitialLookup(const QHostInfo&);
	// void handleHostLookupFailed();
	void configureQuickpostTemplates();
	void uploadFile();
	void copyURL();
	void handleFind();
	void populateAccountList();
	void populateBlogList();
	void populateTagList();
	void hideProgressBarIfMaximum(int);
	void hideProgressBar();
	void reenableDirty();
	void saveAccountsDom();
	void showPassword(bool);

    public slots:
	void copy();
	void paste();
	void tidyPaste();
	void refreshCategories();
	// void refreshWPTags();
	void getAccounts(const QString&);
	void getAccounts();
	void getPreferences(const QString&);
	void getPreferences();
	void setHighlighting(bool);
	void refreshBlogList();
	void changeCaptionAfterTitleChanged();
	void save();
	// bool load(const QString &, QDomDocument &);
	bool load(const QString&, bool fromSTI = false);
	void dirtify();
	void dirtifyIfText();
	void setDirtySignals(bool);
	void doInitialChangeBlog();
	void doQuit();
	void setPublishStatus(int);
	void setPostTitle(const QString&);
	void setPostClean();
	void showStatusBarMessage(const QString&);
	void findAgain();
	void doPreview(bool, bool markdownFailed = false);
	void handleConsole(bool);
	void clearConsole();
	// void setToolBarVisible(bool);

	void file_print_preview();
	void file_print();
	void file_print_pdf();
	void on_topic_changed(const QString& tp);

	//	void on_record_screen_close_requested();
	//	void on_browser_close_requested();

    protected:
	QTextBrowser* _preview_window;
	Highlighter* _highlighter;

	QijSearchWidget* _search_widget;
	QPushButton* _preview_button;
	QPushButton* _blog_this_button;
	// QAction *blogThisIntlAction;
	std::unique_ptr<QNetworkAccessManager> _net_acess_manager;
	QNetworkReply* _current_reply;

	//	ts_t *_tree_screen;
	//	web::Docker *_browser_docker;
	HidableTab* _vtab_record;
	// wn_t *_main_window;
	//	web::Profile *_profile;
	//	Qt::WindowFlags _flags;
	//	QString _style_source;

	//	FindScreen *_find_screen;

	QWidget* _central_widget; // *leftWidget,
	QSplitter* _splitter;
	QStackedWidget* _main_stack;

	// Ui::CategoryWidget

	TEXTEDIT_FOR_READ* _console;
	TEXTEDIT* _editor;
	SideTabWidget* _control_tab;

	QVBoxLayout* _main_layout_with_search;
	QHBoxLayout* _main_layout;
	// QPushButton *pbCopyURL;
	QWidget* _progress_bar_container;
#ifdef WIN_QTV
	QWinTaskbarButton* taskbarButton;
	QWinTaskbarProgress* taskbarProgress;
#endif
	QHBoxLayout* _progress_bar_layout;
	QProgressBar* _progress_bar;
	QAction* _progress_bar_action;
	QMenu* _recent_files_menu;
	QAction* _no_recent_files_action;
	QAction* _child_category_here_action;
	QStatusBar* _status_widget;

	int _preview_id;
	int _console_id, _editor_id;
	int _previous_raised_ls_widget;
	HttpBusinessType _current_http_business;
	bool _doing_new_post;

	// FlatToolButton	*_close_button;
	// QVBoxLayout	*_toolsarea_of_close_button;
	// QSplitter *_splitter;

	QString _splitter_groupname;
	QString _splitter_sizelist;

	browser_ref _browser;

	//	sd::intrusive_ptr<rs_t> _record_screen;
	web::Docker<Blogger>* _editor_docker;
	SuperMenu* _super_menu;
	// web::Browser *_browser = nullptr;
	//	bool _closed = false;
	// virtual void resizeEvent( QResizeEvent *e );
	virtual void closeEvent(QCloseEvent*);
	virtual void showEvent(QShowEvent*);
	virtual void resizeEvent(QResizeEvent*);
	bool eventFilter(QObject*, QEvent*);
	virtual bool event(QEvent*);
	// std::function<void (const QString &, bool)> save_impl;

	QStatusBar* statusBar();
    signals:
	void httpBusinessFinished();
	void categoryRefreshFinished();
	void blogRefreshFinished();
	//	void self_close_request(QWidget*);
	void topic_changed(const QString& original_topic_, const QString& tp, bool append_mode = false);
	friend class web::Docker<Blogger>;
	friend class EditorWrap;
	friend class Editor;
	friend class SideTabWidget;

    protected:
#ifndef USE_SIGNAL_CLOSE
	boost::signals2::signal<void(Blogger*)> _prepended;
#endif // NO USE_SIGNAL_CLOSE
    public:
#ifdef USE_EDITOR_WRAP
	void name(const QString& nm);
	// void tree_path(const QString &path);
	void pin(const QString& pin_);
	// void switch_pin(bool checked);
	void author(const QString& author_);
	void home(const QString& home_);
	void url(const QString& url_);
	void tags(const QString& tags_);
	boost::intrusive_ptr<TreeItem> item();
	void bind(boost::intrusive_ptr<TreeItem> item_to_be_bound);
#endif // USE_EDITOR_WRAP
	// #ifdef USE_QTM
	// public:
	////	FlatToolButton		*_to_attach = nullptr;
	////	QIcon _icon_attach_exists;		// Иконка, когда аттачи есть
	////	QIcon _icon_attach_not_exists;		// Иконка, когда аттачей нет

	// Методы установки нестандартных процедур чтения и сохранения текста
	const std::function<void(QObject* editor, QString saveString)>
	save_callback() const;
	void save_callback(
	    const std::function<void(QObject* editor, QString saveString)>& func);

	std::function<void(QObject* editor, QString& String)> load_callback() const;
	void load_callback(
	    const std::function<void(QObject* editor, QString& String)>& func);

// static void editor_load_callback(QObject *editor, QString &load_text);
// static void editor_save_callback(QObject *editor, const QString &save_text);
#ifdef USE_FILE_PER_TREEITEM
	// Абсолютный или относительный путь (т.е. директория),
	// куда будет сохраняться текст. Без завершающего слеша
	bool work_directory(QString dir_name);
#endif // USE_FILE_PER_TREEITEM
	QString work_directory(void);
#ifdef USE_FILE_PER_TREEITEM
	// Имя файла, куда должен сохраняться текст
	// Без пути, только имя
	void file_name(QString file_name_);
#endif // USE_FILE_PER_TREEITEM
	QString file_name(void);
	void save_textarea();

	void dir_file_empty_reaction(int mode);
	int dir_file_empty_reaction(void);

	// enum {
	// DIRFILEEMPTY_REACTION_SHOW_ERROR,
	// DIRFILEEMPTY_REACTION_SUPPRESS_ERROR
	// };
	// Методы установки и чтения произвольных нестандартных данных
	// которые может хранить объект редактора
	template <typename field_key_type>
	void misc_field(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, field_key_type>::type>::type& value); // , typename field_type_switch = typename


	template <typename field_key_type>
	typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, field_key_type>::type>::type misc_field() const;

	bool load_textarea();
	int cursor_position(void);
	void cursor_position(int n);

	int scrollbar_position(void);
	void scrollbar_position(int n);

	// void send_expand_edit_area(bool
	// flag){_editor->send_expand_edit_area(flag);}
	// #if QT_VERSION == 0x050600
	// W_SIGNAL(send_expand_edit_area, (bool), flag)	//
	// #else
	////	;
	// #endif

	// void
	// wyedit_find_in_base_clicked(){_editor->wyedit_find_in_base_clicked();}
	// #if QT_VERSION == 0x050600
	// W_SIGNAL(wyedit_find_in_base_clicked)	//
	// #else
	////	;
	// #endif

	QTextDocument* textarea_document(void);
	void textarea_editable(bool editable);
//// Метод позволяющий управлять доступностью инструментов редактирования
// void disable_tool_list(QStringList
// toolNames){_editor->disable_tool_list(toolNames);}

// void init_enable_assembly(bool
// flag){_editor->init_enable_assembly(flag);}
// void init_config_file_name(QString
// name){_editor->init_config_file_name(name);}
// void init_enable_random_seed(bool
// flag){_editor->init_enable_random_seed(flag);}
// void init(int mode){_editor->init(mode);}

//// Метод установки функции нажатия на кнопку Attach
// void attach_callback(void
// (*func)(void)){_editor->attach_callback(func);}
// void
// update_indentline_geometry(){_editor->update_indentline_geometry();}
// void textarea(QString text){_editor->textarea(text);}
// void clear_all_misc_field(void){_editor->clear_all_misc_field();}
////	// Горизонтальная линейка, содержащая кнопки форматирования
////	QVBoxLayout		*_textformat_buttons_layout = nullptr;
////	// Область редактирования текста
////	EditorTextArea *_text_area;
#ifdef USE_FILE_PER_TREEITEM
	void clear_all(void);
#endif // USE_FILE_PER_TREEITEM
	FlatToolButton* to_attach() const;

	QIcon icon_attach_exists() const;
	QIcon icon_attach_not_exists() const;

	QVBoxLayout* textformat_buttons_layout() const;
	EditorTextArea* text_area() const;

	void to_editor_layout(void);
	void to_attach_layout(void);

	void save_text_context();
	void save_editor_cursor_position();
	void save_editor_scrollbar_position();
	void go_walk_history();
	void go_walk_history_next();
	void go_walk_history_previous();
	void restore_editor_cursor_position();
	void restore_editor_scrollbar_position();
	//	QString find_file_name_in_current_folder(const QString
	//&file_name_with_full_path);
	void topic(const QString& topic_);
	QString topic() const;
	QString current_topic_folder_name() const;
	void title(const QString& title_);
	QString title() const;
	SuperMenu* super_menu();
	web::Browser* browser();
	SideTabWidget* control_tab();
};


#ifndef EXTERN_MISC_FIELD_GET_DECLARATION
#define EXTERN_MISC_FIELD_GET_DECLARATION(key_type) \
	extern template typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map, key_type>::type>::type Blogger::misc_field<key_type>() const;
#endif // EXTERN_MISC_FIELD_GET_DECLARATION

EXTERN_MISC_FIELD_GET_DECLARATION(id_key)
EXTERN_MISC_FIELD_GET_DECLARATION(pin_key)
EXTERN_MISC_FIELD_GET_DECLARATION(rating_key)
EXTERN_MISC_FIELD_GET_DECLARATION(name_key)
EXTERN_MISC_FIELD_GET_DECLARATION(author_key)
EXTERN_MISC_FIELD_GET_DECLARATION(home_key)
EXTERN_MISC_FIELD_GET_DECLARATION(url_key)
EXTERN_MISC_FIELD_GET_DECLARATION(tags_key)
EXTERN_MISC_FIELD_GET_DECLARATION(ctime_key)
EXTERN_MISC_FIELD_GET_DECLARATION(dir_key)
EXTERN_MISC_FIELD_GET_DECLARATION(file_key)
EXTERN_MISC_FIELD_GET_DECLARATION(crypt_key)
EXTERN_MISC_FIELD_GET_DECLARATION(has_attach_key)
EXTERN_MISC_FIELD_GET_DECLARATION(attach_size_key)


#ifndef EXTERN_MISC_FIELD_SET_DECLARATION
#define EXTERN_MISC_FIELD_SET_DECLARATION(key_type) \
	extern template void Blogger::misc_field<key_type>(const typename boost::remove_reference<typename boost::fusion::result_of::at_key<full_fields_map const, key_type>::type>::type& value);
#endif // EXTERN_MISC_FIELD_SET_DECLARATION

EXTERN_MISC_FIELD_SET_DECLARATION(id_key)
EXTERN_MISC_FIELD_SET_DECLARATION(pin_key)
EXTERN_MISC_FIELD_SET_DECLARATION(rating_key)
EXTERN_MISC_FIELD_SET_DECLARATION(name_key)
EXTERN_MISC_FIELD_SET_DECLARATION(author_key)
EXTERN_MISC_FIELD_SET_DECLARATION(home_key)
EXTERN_MISC_FIELD_SET_DECLARATION(url_key)
EXTERN_MISC_FIELD_SET_DECLARATION(tags_key)
EXTERN_MISC_FIELD_SET_DECLARATION(ctime_key)
EXTERN_MISC_FIELD_SET_DECLARATION(dir_key)
EXTERN_MISC_FIELD_SET_DECLARATION(file_key)
EXTERN_MISC_FIELD_SET_DECLARATION(crypt_key)
EXTERN_MISC_FIELD_SET_DECLARATION(has_attach_key)
EXTERN_MISC_FIELD_SET_DECLARATION(attach_size_key)


#endif
