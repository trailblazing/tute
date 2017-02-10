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
#include <memory>
// #include "ui_EditingWindowBase.h"
#include "ui_SideWidget.h"
#include "ui_aboutbox.h"
// #include "ui_account_form.h"
#include "ui_ImageEntry.h"
#include "ui_LinkEntry.h"
#include "ui_password-form.h"
#include "ui_NewCategoryForm.h"
#include "accounts_dialog.h"
#include "highlighter.h"
#include "prefs_dialog.h"
#include "qij_search_widget.h"
#include "libraries/flat_control.h"
#include "utility/add_action.h"

// #include "ui_ListDialog.h"
#ifdef USE_SYSTRAYICON
#include "sys_tray_icon.h"
#endif
// #include "Application.h"

#include <QMainWindow>
#include <QHash>
#include <QColor>
#include <QtXml>
#include <QString>

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
using namespace Ui;



#ifdef Q_OS_WIN
#if QT_VERSION >= 0x050000
#define WIN_QTV
class QWinTaskbarButton;
class QWinTaskbarProgress;
#endif  // QT_VERSION >= 0x050000
#define PROPERSEPS(x) x.replace("/", "\\")
#else
#define PROPERSEPS(x) x
#endif


class QSplitter;

class SuperMenu;

class EditingWindow : public QMainWindow {
Q_OBJECT

public:
	// EditingWindow( QWidget *widget = 0 );
	/*  Catkin( QList<QString>, QList<QString>, int,
	    QWidget *parent = 0 );*/
	EditingWindow(ts_t *_tree_screen, browser::Entrance *_entrance, browser::Profile *_profile, FindScreen *_find_screen, Editentry *editentry, Qt::WindowFlags flags, QString style_source_, QString newPost = QString());
	~EditingWindow();
#ifdef USE_SYSTRAYICON
	void setSTI(SysTrayIcon *);
#endif
	static bool caseInsensitiveLessThan(const QString &, const QString &);
	bool handleArguments();

	QString postTitle();
	static void positionWidget(QWidget *, QWidget *);

	typedef enum _hbiz {
		None
		, _blogger_getUsersBlogs
		, _metaWeblog_newPost, _metaWeblog_editPost, _metaWeblog_newMediaObject
		, _mt_publishPost, _mt_getCategoryList, _mt_setPostCategories
		, _wp_getTags, _wp_newCategory, _wp_newPost, _wp_editPost
	}HttpBusinessType;
	TEXTEDIT *editor();
	QStackedWidget *main_stack();
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
	QString processWithMarkdown(const QString &);
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
	QDomElement XmlValue(QDomDocument &, QString, QString
			    , QString memberType = "param");
	QDomElement XmlMember(QDomDocument &, QString, QString, QString);
	QDomElement XmlMethodName(QDomDocument &, QString);
	QDomElement XmlRpcArray(QDomDocument &, QString, QList<QString> );
	QDomElement XmlRpcArray(QDomDocument &, QString, QString, QList<QString> );
	void setNetworkActionsEnabled(bool);
	QString &getHTMLList(QString, QString &);
	void saveAutoLinkDictionary();
	void loadAutoLinkDictionary();
	QByteArray toBase64(QByteArray &);
	void addToConsole(const QString &);
	void handleEnableCategories();
	void extractAccountAttributes();
	QString decodeXmlEntities(QString);
	void placeNetworkRequest(HttpBusinessType, QByteArray &);
	// void setHost( QHttp *, QString &, QString &, bool );
	void setEditActionsEnabled(bool);
	void purgeBlankAccounts();


	void assembly_closebutton(void);
	void widget_hide(bool checked);
	void setup_closebutton(void);
private:
	QString _config_file_name = "";
	int currentAccount
	, currentBlog
	, loadedEntryBlog;
	QString loadedAccountId;
	QString applicationVersion;
	QString server;
	QString location;
	QString login;
	QString password;
	QString port;
	QString _local_storage_directory;
	QString _local_storage_fFile_extn;
	QString remoteFileLocation;
	QString lastAccountID;
	bool useHTTPS
	, categoriesEnabled
	, entryBlogged
	, useNewWindows
	, savePassword
	, postAsSave
	, noPassword
	, initialChangeBlog
	, allowComments
	, allowTB
	, postDateTime
	, copyTitle
	, useWordpressAPI
	, allowRegexSearch
	, useTwoNewlines
	, useMarkdown
	, doMarkdownWhenPosting
	, stripParaTags
	, enableHighlighting
	, suppressReadMore
	, useUtf8;
	QHash<QString, bool *> accountAttributes;
	QHash<QString, QString *> accountStrings;
	bool entryEverSaved
	, cleanSave
	, noAutoSave
	, noAlphaCats
	, networkActionsEnabled;
	QList<QString> usersBlogs;
	QList<QString> categoryList;
	QStringList usersBlogURIs
	, usersBlogNames
	, usersBlogIDs;
	QStringList categoryNames
	, categoryIDs;
	QStringList qtmaccounts_xml;
	QHash<QString, QString> autoLinkDictionary;
	QHash<QString, QString> autoLinkTitleDictionary;
	QHash<QString, int> autoLinkTargetDictionary;
	QByteArray responseData;
	QString entryNumber
	, dateTime;
	QString filename;
	int primaryCat;
	QString otherCatsList;
	QRegExpValidator *tagValidator;
	QString editorFontString
	, previewFontString
	, consoleFontString;
	QString perlPath
	, markdownPath;
	QColor editorBgColor
	, editorFgColor
	, previewBgColor
	, previewFgColor
	, consoleBgColor
	, consoleFgColor;
	QColor tagBgColor
	, tagFgColor
	, entityBgColor
	, entityFgColor
	, commentBgColor
	, commentFgColor
	, linkFgColor
	, linkBgColor;
	bool tagBold
	, tagItalic
	, entityBold
	, entityItalic
	, commentBold
	, commentItalic
	, linkBold
	, linkItalic
	, linkUnderline;

	bool useBloggerTitleFormatting;
	int bloggerTitleFormat;
	QStringList bloggerTitleFormatStrings;
	QLabel *dirtyIndicator;
	int STI2ClickFunction;
	QDomDocument accountsDom;
	QDomElement accountsElement
	, currentAccountElement
	, currentBlogElement
	, currentCategoryElement;
	QString currentAccountId
	, currentBlogid;
	QString userAgentString;
	QByteArray userAgentArray;
	sapp_t *_app;
	QAction *recentFileActions[10];
	QAction *removeKeywordTag_forList;
	QAction *addKeywordTag_forList;
	QAction *addKeywordTag_forAvailTags;
	QAction *_refreshKeywordTags;
	QList<sapp_t::RecentFile> recentFiles;
	QString checkBoxName(QString);
#ifdef USE_SYSTRAYICON
	SysTrayIcon *sti;
#endif
	QHash <QString, QChar> xmlEntities;
	SuperMenu *_super_menu;
//	QToolBar *toolBar;
//	FlatToolButton
//	FlatToolButton *action_Open;
//	FlatToolButton *action_Save;
//	FlatToolButton *_action_bold;
//	FlatToolButton *_action_italic;
//	FlatToolButton *_action_underline;
//	FlatToolButton *_action_blockquote;
//	FlatToolButton *_action_link;
//	FlatToolButton *_action_image;
//	FlatToolButton *_action_more;
//	FlatToolButton *_action_preview;
//	FlatToolButton *action_Blog_this;
	QAction *addWPTagAction
	, *removeWPTagAction
	, *refreshWPTagsAction
	, *addTechTagAction
	, *addClipTechTagAction
	, *removeTechTagAction
	, *addTBPingAction
	, *addClipPingAction
	, *removeTBPingAction;

private slots:
	void choose(const QString fname = QString());
	void openRecentFile();
	void save(const QString &, bool exp = false);
	void exportEntry();
	void saveAll();
	void saveAs(bool exp = false);
	void stopThisJob();
	// void handleDone( bool );
	void handleDone(QNetworkReply *);
	// void handleResponseHeader( const QHttpResponseHeader & );
#ifndef DONT_USE_SSL
	void handleSslErrors(const QList<QSslError> &);
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
	void cut();
	void copy();
	QString removeReadMore(QString &);
	void paste();
	void tidyPaste();
	void pasteAsMarkedParagraphs();
	void pasteAsBlockquote();
	void pasteAsMarkdownBlockquote();
	void pasteAsUnorderedList();
	void pasteAsOrderedList();
	void undo();
	void redo();
	void makeUnorderedList();
	void makeOrderedList();
	void showHighlightedURL(const QString &);
	void blogThis();
	void newMTPost();       // formerly blogThis()
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
	void handleInitialLookup(const QHostInfo &);
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
	void refreshCategories();
	// void refreshWPTags();
	void getAccounts(const QString &);
	void getAccounts();
	void getPreferences(const QString &);
	void getPreferences();
	void setHighlighting(bool);
	void refreshBlogList();
	void changeCaptionAfterTitleChanged();
	void save();
	bool load(const QString &, QDomDocument &);
	bool load(const QString &, bool fromSTI = false);
	void dirtify();
	void dirtifyIfText();
	void setDirtySignals(bool);
	void doInitialChangeBlog();
	void doQuit();
	void setPublishStatus(int);
	void setPostTitle(const QString &);
	void setPostClean();
	void showStatusBarMessage(const QString &);
	void findAgain();
	void doPreview(bool, bool markdownFailed = false);
	void handleConsole(bool);
	void clearConsole();
	void setToolBarVisible(bool);

protected:

	QWidget	*_central_widget;       // *leftWidget,
	QSplitter *_splitter;
	//	Ui::CategoryWidget
	SideTabWidget *_control_tab;

	QStackedWidget *_main_stack;
	QTextBrowser *_preview_window;
	Highlighter *_highlighter;

	QijSearchWidget *_search_widget;
	QPushButton *_preview_button;
	QPushButton *_blog_this_button;
	// QAction *blogThisIntlAction;
	std::unique_ptr<QNetworkAccessManager> _net_acess_manager;
	QNetworkReply *_current_reply;


	ts_t *_tree_screen;
	browser::Entrance *_entrance;
//		wn_t *_main_window;
	browser::Profile *_profile;
	Qt::WindowFlags _flags;
	QString _style_source;


	Editentry *_editentry;
	FindScreen *_find_screen;
	TEXTEDIT_FOR_READ *_console;
	TEXTEDIT *_editor;
	QVBoxLayout *_main_layout_with_search;
	QHBoxLayout *_main_layout;
	// QPushButton *pbCopyURL;
	QWidget *_progress_bar_container;
#ifdef WIN_QTV
	QWinTaskbarButton *taskbarButton;
	QWinTaskbarProgress *taskbarProgress;
#endif
	QHBoxLayout *_progress_bar_layout;
	QProgressBar *_progress_bar;
	QAction *_progress_bar_action;
	QMenu *_recent_files_menu;
	QAction *_no_recent_files_action;
	QAction *_child_category_here_action;
	QStatusBar *_status_widget;

	int _preview_id;
	int _console_id, _editor_id;
	int _previous_raised_ls_widget;
	HttpBusinessType _current_http_business;
	bool _doing_new_post;

//	FlatToolButton	*_close_button;
//	QVBoxLayout	*_toolsarea_of_close_button;
//		QSplitter *_splitter;
	std::shared_ptr<QSettings>  _splitter_config;
	QString _splitter_groupname;
	QString _splitter_sizelist;

	// virtual void resizeEvent( QResizeEvent *e );
	virtual void closeEvent(QCloseEvent *);
	virtual void showEvent(QShowEvent *);
	virtual void resizeEvent(QResizeEvent *);
	bool eventFilter(QObject *, QEvent *);
	virtual bool event(QEvent *);

signals:
	void httpBusinessFinished();
	void categoryRefreshFinished();
	void blogRefreshFinished();
	friend class Editentry;
	friend class EditorWrap;
	friend class Editor;
	friend class SideTabWidget;
};

#endif
