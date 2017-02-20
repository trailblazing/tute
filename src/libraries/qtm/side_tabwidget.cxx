#include "side_tabwidget.h"
// #include "views/main_window/hidable_tabwidget.h"
#include "editing_window.h"
#include "views/record/editor_dock.h"
#include "views/attach_table/attach_table_screen.h"



SideTabWidget::SideTabWidget(ts_t *tree_screen, FindScreen *find_screen, browser::BrowserDock *browser_dock, EditingWindow *editing_window, browser::Profile *profile, QString style_source_, QSplitter *splitter, QWidget *parent)
	: HidableTab(tree_screen, find_screen, browser_dock, editing_window, profile, style_source_, splitter, std::make_shared<QSettings>(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_editor_conf_filename, QSettings::IniFormat), "geometry", "splitter_sizelist", "collapsed", parent)
	  , _editing_window(editing_window)
	  , _attachtable_screen(new AttachTableScreen(editing_window, this)){
	// , tabWidget(new HidableTabWidget(_tree_screen, _find_screen, _editentry, _entrance, _main_window, _profile, style_source_, this))
	setupUi();
	connect(_lineedit_topic, &QLineEdit::textChanged, [&](const QString &tp){
			_editing_window->on_topic_changed(_lineedit_topic, tp);
		});
}

void SideTabWidget::setupUi(){
	// QWidget *this = this;
	// if(this->objectName().isEmpty())
	this->setObjectName(QStringLiteral("CategoryWidget"));
	// vboxLayout = new QVBoxLayout(this);
	// vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
	// vboxLayout->setContentsMargins(0, 0, 0, 0);

	// vboxLayout->setMargin(0);

	// vboxLayout->setSpacing(0);

	// hboxLayout = new QHBoxLayout();
	// hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
	// spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	// hboxLayout->addItem(spacerItem);

	// cbPageSelector = new QComboBox(this);
	// cbPageSelector->setObjectName(QStringLiteral("cbPageSelector"));

	// hboxLayout->addWidget(cbPageSelector);

	// spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	// hboxLayout->addItem(spacerItem1);

	// vboxLayout->addLayout(hboxLayout);

	// tabWidget = new HidableTabWidget(CategoryWidget);
	// this->setObjectName(QStringLiteral("tabWidget"));
	this->setTabPosition(QTabWidget::West);
	this->setTabsClosable(false);
	[&] {
		_basics = new QWidget();
		_basics->setObjectName(QStringLiteral("Basics"));
		verticalLayout_3 = new QVBoxLayout(_basics);
		verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
		gridLayout = new QGridLayout();
		gridLayout->setObjectName(QStringLiteral("gridLayout"));
		cbStatus = new QComboBox(_basics);
		cbStatus->setObjectName(QStringLiteral("cbStatus"));

		gridLayout->addWidget(cbStatus, 3, 1, 1, 1);

		leTitle = new QLineEdit(_basics);
		leTitle->setObjectName(QStringLiteral("leTitle"));

		gridLayout->addWidget(leTitle, 0, 1, 1, 1);

		cbAccountSelector = new QComboBox(_basics);
		cbAccountSelector->setObjectName(QStringLiteral("cbAccountSelector"));

		gridLayout->addWidget(cbAccountSelector, 1, 1, 1, 1);

		lStatus = new QLabel(_basics);
		lStatus->setObjectName(QStringLiteral("lStatus"));

		gridLayout->addWidget(lStatus, 3, 0, 1, 1);

		cbBlogSelector = new QComboBox(_basics);
		cbBlogSelector->setObjectName(QStringLiteral("cbBlogSelector"));

		gridLayout->addWidget(cbBlogSelector, 2, 1, 1, 1);

		lePassword = new QLineEdit(_basics);
		lePassword->setObjectName(QStringLiteral("lePassword"));
		lePassword->setEchoMode(QLineEdit::Password);

		gridLayout->addWidget(lePassword, 4, 1, 1, 1);

		lBlog = new QLabel(_basics);
		lBlog->setObjectName(QStringLiteral("lBlog"));

		gridLayout->addWidget(lBlog, 2, 0, 1, 1);

		lTitle = new QLabel(_basics);
		lTitle->setObjectName(QStringLiteral("lTitle"));
		QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(lTitle->sizePolicy().hasHeightForWidth());
		lTitle->setSizePolicy(sizePolicy);

		gridLayout->addWidget(lTitle, 0, 0, 1, 1);

		lAccount = new QLabel(_basics);
		lAccount->setObjectName(QStringLiteral("lAccount"));

		gridLayout->addWidget(lAccount, 1, 0, 1, 1);

		lPassword = new QLabel(_basics);
		lPassword->setObjectName(QStringLiteral("lPassword"));

		gridLayout->addWidget(lPassword, 4, 0, 1, 1);

		chShowPassword = new QCheckBox(_basics);
		chShowPassword->setObjectName(QStringLiteral("chShowPassword"));

		gridLayout->addWidget(chShowPassword, 5, 1, 1, 1);

		wBlankWidget = new QWidget(_basics);
		wBlankWidget->setObjectName(QStringLiteral("wBlankWidget"));

		gridLayout->addWidget(wBlankWidget, 5, 0, 1, 1);

		verticalLayout_3->addLayout(gridLayout);

		gridLayout1 = new QGridLayout();
		gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
		label = new QLabel(_basics);
		label->setObjectName(QStringLiteral("label"));

		gridLayout1->addWidget(label, 0, 0, 2, 1);

		chAllowComments = new QCheckBox(_basics);
		chAllowComments->setObjectName(QStringLiteral("chAllowComments"));

		gridLayout1->addWidget(chAllowComments, 0, 1, 1, 1);

		chAllowTB = new QCheckBox(_basics);
		chAllowTB->setObjectName(QStringLiteral("chAllowTB"));

		gridLayout1->addWidget(chAllowTB, 1, 1, 1, 1);

		verticalLayout_3->addLayout(gridLayout1);

		chSticky = new QCheckBox(_basics);
		chSticky->setObjectName(QStringLiteral("chSticky"));
		chSticky->setEnabled(false);

		verticalLayout_3->addWidget(chSticky);

		cbDoTB = new QCheckBox(_basics);
		cbDoTB->setObjectName(QStringLiteral("cbDoTB"));

		verticalLayout_3->addWidget(cbDoTB);

		copyURLWidget = new QWidget(_basics);
		copyURLWidget->setObjectName(QStringLiteral("copyURLWidget"));
		horizontalLayout = new QHBoxLayout(copyURLWidget);
		horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
		horizontalLayout->setContentsMargins(-1, 0, -1, 0);
		horizontalSpacer = new QSpacerItem(56, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout->addItem(horizontalSpacer);

		pbCopyURL = new QPushButton(copyURLWidget);
		pbCopyURL->setObjectName(QStringLiteral("pbCopyURL"));

		horizontalLayout->addWidget(pbCopyURL);

		horizontalSpacer_2 = new QSpacerItem(55, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

		horizontalLayout->addItem(horizontalSpacer_2);

		verticalLayout_3->addWidget(copyURLWidget);

		progressWidget = new QWidget(_basics);
		progressWidget->setObjectName(QStringLiteral("progressWidget"));
		horizontalLayout_2 = new QHBoxLayout(progressWidget);
		horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
		horizontalLayout_2->setContentsMargins(-1, 0, -1, 0);
		lProgressBarLabel = new QLabel(progressWidget);
		lProgressBarLabel->setObjectName(QStringLiteral("lProgressBarLabel"));

		horizontalLayout_2->addWidget(lProgressBarLabel);

		progressBar = new QProgressBar(progressWidget);
		progressBar->setObjectName(QStringLiteral("progressBar"));
		progressBar->setValue(24);

		horizontalLayout_2->addWidget(progressBar);

		verticalLayout_3->addWidget(progressWidget);

		spacerItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

		verticalLayout_3->addItem(spacerItem2);
		_label_topic = new QLabel(_basics);
		_label_topic->setObjectName(QStringLiteral("label_topic"));
		QSizePolicy sizeP_policy_topic_label(QSizePolicy::Minimum, QSizePolicy::Preferred);
		sizeP_policy_topic_label.setHorizontalStretch(0);
		sizeP_policy_topic_label.setVerticalStretch(0);
		sizeP_policy_topic_label.setHeightForWidth(_label_topic->sizePolicy().hasHeightForWidth());
		_label_topic->setSizePolicy(sizeP_policy_topic_label);
		gridLayout->addWidget(_label_topic, 6, 0, 1, 1);
		_lineedit_topic = new QLineEdit(_basics);
		_lineedit_topic->setObjectName(QStringLiteral("lineedit_topic"));
		gridLayout->addWidget(_lineedit_topic, 6, 1, 1, 1);

		this->addTab(_basics, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Basics"));
	} ();
	[&] {
		_categories = new QWidget();
		_categories->setObjectName(QStringLiteral("Categories"));
		vboxLayout1 = new QVBoxLayout(_categories);
		vboxLayout1->setObjectName(QStringLiteral("vboxLayout1"));
		gbCategory = new QGroupBox(_categories);
		gbCategory->setObjectName(QStringLiteral("gbCategory"));
		gridLayout_2 = new QGridLayout(gbCategory);
		gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
		lMainCat = new QLabel(gbCategory);
		lMainCat->setObjectName(QStringLiteral("lMainCat"));

		gridLayout_2->addWidget(lMainCat, 0, 0, 1, 1);

		cbMainCat = new QComboBox(gbCategory);
		cbMainCat->setObjectName(QStringLiteral("cbMainCat"));

		gridLayout_2->addWidget(cbMainCat, 1, 0, 1, 2);

		lOtherCats = new QLabel(gbCategory);
		lOtherCats->setObjectName(QStringLiteral("lOtherCats"));

		gridLayout_2->addWidget(lOtherCats, 2, 0, 1, 1);

		lwOtherCats = new QListWidget(gbCategory);
		lwOtherCats->setObjectName(QStringLiteral("lwOtherCats"));
		lwOtherCats->setContextMenuPolicy(Qt::ActionsContextMenu);
		lwOtherCats->setSelectionMode(QAbstractItemView::MultiSelection);

		gridLayout_2->addWidget(lwOtherCats, 3, 0, 1, 2);

		chNoCats = new QCheckBox(gbCategory);
		chNoCats->setObjectName(QStringLiteral("chNoCats"));

		gridLayout_2->addWidget(chNoCats, 4, 0, 1, 2);

		pbRefresh = new QPushButton(gbCategory);
		pbRefresh->setObjectName(QStringLiteral("pbRefresh"));

		gridLayout_2->addWidget(pbRefresh, 5, 0, 1, 1);

		pbAddCategory = new QPushButton(gbCategory);
		pbAddCategory->setObjectName(QStringLiteral("pbAddCategory"));

		gridLayout_2->addWidget(pbAddCategory, 5, 1, 1, 1);

		vboxLayout1->addWidget(gbCategory);

		this->addTab(_categories, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Categories"));
	} ();

	[&] {
		_excerpt = new QWidget();
		_excerpt->setObjectName(QStringLiteral("Excerpt"));
		vboxLayout2 = new QVBoxLayout(_excerpt);
		vboxLayout2->setObjectName(QStringLiteral("vboxLayout2"));
		gbExcerpt = new QGroupBox(_excerpt);
		gbExcerpt->setObjectName(QStringLiteral("gbExcerpt"));
		vboxLayout3 = new QVBoxLayout(gbExcerpt);
		vboxLayout3->setObjectName(QStringLiteral("vboxLayout3"));
		teExcerpt = new QTextEdit(gbExcerpt);
		teExcerpt->setObjectName(QStringLiteral("teExcerpt"));

		vboxLayout3->addWidget(teExcerpt);

		lExcerptExplanation = new QLabel(gbExcerpt);
		lExcerptExplanation->setObjectName(QStringLiteral("lExcerptExplanation"));
		lExcerptExplanation->setAlignment(Qt::AlignJustify | Qt::AlignVCenter);
		lExcerptExplanation->setWordWrap(true);

		vboxLayout3->addWidget(lExcerptExplanation);

		vboxLayout2->addWidget(gbExcerpt);

		this->addTab(_excerpt, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Excerpt"));
	} ();
	[&] {
		_wp_tags = new QWidget();
		_wp_tags->setObjectName(QStringLiteral("WP tags"));
		verticalLayout_2 = new QVBoxLayout(_wp_tags);
		verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
		gbKeywordTags = new QGroupBox(_wp_tags);
		gbKeywordTags->setObjectName(QStringLiteral("gbKeywordTags"));
		verticalLayout = new QVBoxLayout(gbKeywordTags);
		verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
		gridLayout_1 = new QGridLayout();
		gridLayout_1->setObjectName(QStringLiteral("gridLayout_1"));
		leAddKeywordTag = new QLineEdit(gbKeywordTags);
		leAddKeywordTag->setObjectName(QStringLiteral("leAddKeywordTag"));

		gridLayout_1->addWidget(leAddKeywordTag, 7, 0, 1, 3);

		lwAvailKeywordTags = new QListWidget(gbKeywordTags);
		lwAvailKeywordTags->setObjectName(QStringLiteral("lwAvailKeywordTags"));
		lwAvailKeywordTags->setContextMenuPolicy(Qt::ActionsContextMenu);
		lwAvailKeywordTags->setSortingEnabled(true);

		gridLayout_1->addWidget(lwAvailKeywordTags, 0, 0, 1, 4);

		lwKeywordTags = new QListWidget(gbKeywordTags);
		lwKeywordTags->setObjectName(QStringLiteral("lwKeywordTags"));
		lwKeywordTags->setContextMenuPolicy(Qt::ActionsContextMenu);

		gridLayout_1->addWidget(lwKeywordTags, 4, 0, 1, 4);

		tbAddKeywordTag = new QToolButton(gbKeywordTags);
		tbAddKeywordTag->setObjectName(QStringLiteral("tbAddKeywordTag"));

		gridLayout_1->addWidget(tbAddKeywordTag, 7, 3, 1, 1);

		tbSelectKeywordTag = new QToolButton(gbKeywordTags);
		tbSelectKeywordTag->setObjectName(QStringLiteral("tbSelectKeywordTag"));

		gridLayout_1->addWidget(tbSelectKeywordTag, 1, 0, 1, 1);

		tbRemoveKeywordTag = new QToolButton(gbKeywordTags);
		tbRemoveKeywordTag->setObjectName(QStringLiteral("tbRemoveKeywordTag"));

		gridLayout_1->addWidget(tbRemoveKeywordTag, 1, 1, 1, 1);

		pbRefreshKeywordTags = new QPushButton(gbKeywordTags);
		pbRefreshKeywordTags->setObjectName(QStringLiteral("pbRefreshKeywordTags"));

		gridLayout_1->addWidget(pbRefreshKeywordTags, 1, 2, 1, 2);

		verticalLayout->addLayout(gridLayout_1);

		verticalLayout_2->addWidget(gbKeywordTags);

		this->addTab(_wp_tags, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("WP tags"));
	} ();
	[&] {
		_technorati_tags = new QWidget();
		_technorati_tags->setObjectName(QStringLiteral("Technorati tags"));
		hboxLayout1 = new QHBoxLayout(_technorati_tags);
		hboxLayout1->setObjectName(QStringLiteral("hboxLayout1"));
		gbTags = new QGroupBox(_technorati_tags);
		gbTags->setObjectName(QStringLiteral("gbTags"));
		gridLayout2 = new QGridLayout(gbTags);
		gridLayout2->setObjectName(QStringLiteral("gridLayout2"));
		hboxLayout2 = new QHBoxLayout();
		hboxLayout2->setObjectName(QStringLiteral("hboxLayout2"));
		leAddTag = new QLineEdit(gbTags);
		leAddTag->setObjectName(QStringLiteral("leAddTag"));

		hboxLayout2->addWidget(leAddTag);

		tbAddTag = new QToolButton(gbTags);
		tbAddTag->setObjectName(QStringLiteral("tbAddTag"));
		tbAddTag->setToolButtonStyle(Qt::ToolButtonTextOnly);

		hboxLayout2->addWidget(tbAddTag);

		gridLayout2->addLayout(hboxLayout2, 2, 0, 1, 1);

		lwTags = new QListWidget(gbTags);
		lwTags->setObjectName(QStringLiteral("lwTags"));
		lwTags->setContextMenuPolicy(Qt::ActionsContextMenu);

		gridLayout2->addWidget(lwTags, 1, 0, 1, 1);

		gbTagPlace = new QGroupBox(gbTags);
		gbTagPlace->setObjectName(QStringLiteral("gbTagPlace"));
		vboxLayout4 = new QVBoxLayout(gbTagPlace);
		vboxLayout4->setObjectName(QStringLiteral("vboxLayout4"));
		rbStartOfMainEntry = new QRadioButton(gbTagPlace);
		rbStartOfMainEntry->setObjectName(QStringLiteral("rbStartOfMainEntry"));
		rbStartOfMainEntry->setChecked(true);

		vboxLayout4->addWidget(rbStartOfMainEntry);

		rbEndOfMainEntry = new QRadioButton(gbTagPlace);
		rbEndOfMainEntry->setObjectName(QStringLiteral("rbEndOfMainEntry"));

		vboxLayout4->addWidget(rbEndOfMainEntry);

		gridLayout2->addWidget(gbTagPlace, 0, 0, 1, 1);

		hboxLayout1->addWidget(gbTags);

		this->addTab(_technorati_tags, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Technorati tags"));
	} ();
	[&] {
		_trackbacks = new QWidget();
		_trackbacks->setObjectName(QStringLiteral("Trackbacks"));
		gridLayout3 = new QGridLayout(_trackbacks);
		gridLayout3->setObjectName(QStringLiteral("gridLayout3"));
		gbTrackbacks = new QGroupBox(_trackbacks);
		gbTrackbacks->setObjectName(QStringLiteral("gbTrackbacks"));
		vboxLayout5 = new QVBoxLayout(gbTrackbacks);
		vboxLayout5->setObjectName(QStringLiteral("vboxLayout5"));
		lwTBPings = new QListWidget(gbTrackbacks);
		lwTBPings->setObjectName(QStringLiteral("lwTBPings"));
		lwTBPings->setContextMenuPolicy(Qt::ActionsContextMenu);

		vboxLayout5->addWidget(lwTBPings);

		hboxLayout3 = new QHBoxLayout();
		hboxLayout3->setObjectName(QStringLiteral("hboxLayout3"));
		leTBPingURL = new QLineEdit(gbTrackbacks);
		leTBPingURL->setObjectName(QStringLiteral("leTBPingURL"));

		hboxLayout3->addWidget(leTBPingURL);

		tbTBAdd = new QToolButton(gbTrackbacks);
		tbTBAdd->setObjectName(QStringLiteral("tbTBAdd"));

		hboxLayout3->addWidget(tbTBAdd);

		vboxLayout5->addLayout(hboxLayout3);

		gridLayout3->addWidget(gbTrackbacks, 0, 0, 1, 1);

		this->addTab(_trackbacks, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Trackbacks"));
	} ();
	{
		this->addTab(_attachtable_screen, QIcon(":/resource/pic/three_leaves_clover.svg"), QString("Attachment"));
	}
	// vboxLayout->addWidget(this);

	// QWidget::setTabOrder(cbPageSelector, leTitle);
	QWidget::setTabOrder(leTitle, cbAccountSelector);
	QWidget::setTabOrder(cbAccountSelector, cbBlogSelector);
	QWidget::setTabOrder(cbBlogSelector, cbStatus);
	QWidget::setTabOrder(cbStatus, chAllowComments);
	QWidget::setTabOrder(chAllowComments, chAllowTB);
	QWidget::setTabOrder(chAllowTB, cbDoTB);
	QWidget::setTabOrder(cbDoTB, _lineedit_topic);
	QWidget::setTabOrder(_lineedit_topic, cbMainCat);
	QWidget::setTabOrder(cbMainCat, lwOtherCats);
	QWidget::setTabOrder(lwOtherCats, chNoCats);
	QWidget::setTabOrder(chNoCats, pbRefresh);
	QWidget::setTabOrder(pbRefresh, pbAddCategory);
	QWidget::setTabOrder(pbAddCategory, teExcerpt);
	QWidget::setTabOrder(teExcerpt, leAddKeywordTag);
	QWidget::setTabOrder(leAddKeywordTag, rbStartOfMainEntry);
	QWidget::setTabOrder(rbStartOfMainEntry, rbEndOfMainEntry);
	QWidget::setTabOrder(rbEndOfMainEntry, lwTags);
	QWidget::setTabOrder(lwTags, leAddTag);
	QWidget::setTabOrder(leAddTag, tbAddTag);
	QWidget::setTabOrder(tbAddTag, lwTBPings);
	QWidget::setTabOrder(lwTBPings, leTBPingURL);
	QWidget::setTabOrder(leTBPingURL, tbTBAdd);

	retranslateUi();

	this->setCurrentIndex(1);

	QMetaObject::connectSlotsByName(this);
}

void SideTabWidget::retranslateUi(){
	// QWidget *this = this;
	this->setWindowTitle(QApplication::translate("CategoryWidget", "Form", 0));
	// cbPageSelector->clear();
	// cbPageSelector->insertItems(0, QStringList()
	// << QApplication::translate("CategoryWidget", "Basics", 0)
	// << QApplication::translate("CategoryWidget", "Categories", 0)
	// << QApplication::translate("CategoryWidget", "Excerpt", 0)
	// << QApplication::translate("CategoryWidget", "WP tags", 0)
	// << QApplication::translate("CategoryWidget", "Technorati tags", 0)
	// << QApplication::translate("CategoryWidget", "Trackbacks", 0)
	// );
	cbStatus->clear();
	cbStatus->insertItems(0, QStringList()
	                        << QApplication::translate("CategoryWidget", "Draft", 0)
	                        << QApplication::translate("CategoryWidget", "Publish", 0));
#ifndef QT_NO_WHATSTHIS
	cbStatus->setWhatsThis(QApplication::translate("CategoryWidget", "Whether the entry is to be published immediately, or just stored in the blog database for publication later", 0));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
	lStatus->setWhatsThis(QApplication::translate("CategoryWidget", "Whether the entry is to be published immediately, or stored in the blog database for publication later", 0));
#endif // QT_NO_WHATSTHIS
	lStatus->setText(QApplication::translate("CategoryWidget", "&Status", 0));
	lStatus->setBuddy(cbStatus);
#ifndef QT_NO_WHATSTHIS
	cbBlogSelector->setWhatsThis(QApplication::translate("CategoryWidget", "If your blog software allows you to publish more than one blog, as with Movable Type, you must choose which blog here.  (If you use Wordpress, ignore this.)", 0));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
	lePassword->setWhatsThis(QApplication::translate("CategoryWidget", "A password to read this post. Leave this blank if you do not want to apply a password. Note that the title will be visible.", 0));
#endif // QT_NO_WHATSTHIS
	lBlog->setText(QApplication::translate("CategoryWidget", "&Blog", 0));
	lBlog->setBuddy(cbBlogSelector);
	// lTitle->setTextFormat(Qt::TextFormat::RichText);
	// lTitle->setText(QApplication::translate("CategoryWidget", "Entry &amp;title", 0).toLatin1());
	lTitle->setText(QApplication::translate("CategoryWidget", "Entry &title", 0));
	lTitle->setBuddy(leTitle);
	// lTitle->setWindowTitle(QApplication::translate("CategoryWidget", "Entry &title", 0));
	lAccount->setText(QApplication::translate("CategoryWidget", "&Account", 0));
	lAccount->setBuddy(cbAccountSelector);
	lPassword->setText(QApplication::translate("CategoryWidget", "Pass&word", 0));
	lPassword->setBuddy(lePassword);
	chShowPassword->setText(QApplication::translate("CategoryWidget", "Show passwor&d", 0));
	_label_topic->setText(QApplication::translate("CategoryWidget", "Entry &Topic", 0));
	_label_topic->setBuddy(_lineedit_topic);
	label->setText(QApplication::translate("CategoryWidget", "<b>Feedback</b>", 0));
#ifndef QT_NO_WHATSTHIS
	chAllowComments->setWhatsThis(QApplication::translate("CategoryWidget", "Click here if comments are to be allowed", 0));
#endif // QT_NO_WHATSTHIS
	chAllowComments->setText(QApplication::translate("CategoryWidget", "Comments", 0));
#ifndef QT_NO_WHATSTHIS
	chAllowTB->setWhatsThis(QApplication::translate("CategoryWidget", "Trackbacks are links to other sites, or other people's blog entries, placed by other people.  If you want to allow this, tick this box. Note that if you configure your blog software to refuse trackbacks, or not publish trackback locations, ticking this box will have no effect.", 0));
#endif // QT_NO_WHATSTHIS
	chAllowTB->setText(QApplication::translate("CategoryWidget", "Trackback", 0));
#ifndef QT_NO_WHATSTHIS
	chSticky->setWhatsThis(QApplication::translate("CategoryWidget", "Sticky posts will always appear at the top of your Wordpress blog, usually in a distinctive style.", 0));
#endif // QT_NO_WHATSTHIS
	chSticky->setText(QApplication::translate("CategoryWidget", "Make this post stick&y", 0));
	cbDoTB->setText(QApplication::translate("CategoryWidget", "Trackback this entry", 0));
#ifndef QT_NO_TOOLTIP
	pbCopyURL->setToolTip(QApplication::translate("CategoryWidget", "Copy the uploaded file's URL to the clipboard", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
	pbCopyURL->setWhatsThis(QApplication::translate("CategoryWidget", "Copy the location of the file you just uploaded to the clipboard, so you can paste it into your entry", 0));
#endif // QT_NO_WHATSTHIS
	pbCopyURL->setText(QApplication::translate("CategoryWidget", "Copy URL", 0));
	lProgressBarLabel->setText(QApplication::translate("CategoryWidget", "Uploading:", 0));
	this->setTabText(this->indexOf(_basics), _basics->objectName());
	gbCategory->setTitle(QApplication::translate("CategoryWidget", "Categories", 0));
	lMainCat->setText(QApplication::translate("CategoryWidget", "Primary", 0));
#ifndef QT_NO_WHATSTHIS
	cbMainCat->setWhatsThis(QApplication::translate("CategoryWidget", "The main category under which your entry is to be published.  If you are choosing only one category, choose it here.", 0));
#endif // QT_NO_WHATSTHIS
	lOtherCats->setText(QApplication::translate("CategoryWidget", "Others", 0));
#ifndef QT_NO_WHATSTHIS
	lwOtherCats->setWhatsThis(QApplication::translate("CategoryWidget", "Secondary categories, if your blog system supports them.  To select more than one category, hold down Ctrl and click the main mouse button.", 0));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
	chNoCats->setToolTip(QApplication::translate("CategoryWidget", "Don't use any categories", 0));
#endif // QT_NO_TOOLTIP
	chNoCats->setText(QApplication::translate("CategoryWidget", "No categories", 0));
#ifndef QT_NO_WHATSTHIS
	pbRefresh->setWhatsThis(QApplication::translate("CategoryWidget", "Fetches an updated category list from the blog server", 0));
#endif // QT_NO_WHATSTHIS
	pbRefresh->setText(QApplication::translate("CategoryWidget", "&Refresh", 0));
#ifndef QT_NO_TOOLTIP
	pbAddCategory->setToolTip(QApplication::translate("CategoryWidget", "Add a category", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
	pbAddCategory->setWhatsThis(QApplication::translate("CategoryWidget", "Add a new category to your blog.  Your blog needs to support the Wordpress API for this to work.", 0));
#endif // QT_NO_WHATSTHIS
	pbAddCategory->setText(QApplication::translate("CategoryWidget", "Add category", 0));
	this->setTabText(this->indexOf(_categories), _categories->objectName());
	gbExcerpt->setTitle(QApplication::translate("CategoryWidget", "RSS Excerpt", 0));
	lExcerptExplanation->setText(QApplication::translate("CategoryWidget", "RSS E&xcerpt: This will appear in your summarised RSS feeds if you choose to enter anything here.  If you do not, your blog engine will use the first few lines of the post (or the entire post, if it is short enough).", 0));
	this->setTabText(this->indexOf(_excerpt), _excerpt->objectName());
	gbKeywordTags->setTitle(QApplication::translate("CategoryWidget", "Wordpress-style tags", 0));
#ifndef QT_NO_TOOLTIP
	leAddKeywordTag->setToolTip(QApplication::translate("CategoryWidget", "Enter tag; no need for quotes", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
	leAddKeywordTag->setWhatsThis(QApplication::translate("CategoryWidget", "Enter the tag name here; no need to use quotes if there are two or more words", 0));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
	lwAvailKeywordTags->setToolTip(QApplication::translate("CategoryWidget", "Wordpress-style tags already in use on your blog", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
	lwAvailKeywordTags->setWhatsThis(QApplication::translate("CategoryWidget", "These are the tags that your blog already knows about. Double-click any of these to add it to this entry. You can add another one using the line box below.", 0));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
	lwKeywordTags->setToolTip(QApplication::translate("CategoryWidget", "Wordpress-style tags for this post", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
	lwKeywordTags->setWhatsThis(QApplication::translate("CategoryWidget", "These are the tags used in Wordpress (known as keywords in Movable Type and TypePad)", 0));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
	tbAddKeywordTag->setToolTip(QApplication::translate("CategoryWidget", "Add tag", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
	tbAddKeywordTag->setWhatsThis(QApplication::translate("CategoryWidget", "Click here to add the tag to the list", 0));
#endif // QT_NO_WHATSTHIS
	tbAddKeywordTag->setText(QApplication::translate("CategoryWidget", "&Add", 0));
	tbSelectKeywordTag->setText(QString());
	tbRemoveKeywordTag->setText(QString());
	pbRefreshKeywordTags->setText(QApplication::translate("CategoryWidget", "Refresh", 0));
	this->setTabText(this->indexOf(_wp_tags), _wp_tags->objectName());
	gbTags->setTitle(QApplication::translate("CategoryWidget", "Technorati tags", 0));
#ifndef QT_NO_WHATSTHIS
	leAddTag->setWhatsThis(QApplication::translate("CategoryWidget", "Enter a new Technorati tag here; use the Technorati format which uses plus signs, not spaces, to separate", 0));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
	tbAddTag->setToolTip(QApplication::translate("CategoryWidget", "Add this tag", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
	tbAddTag->setWhatsThis(QApplication::translate("CategoryWidget", "Add this tag to the entry", 0));
#endif // QT_NO_WHATSTHIS
	tbAddTag->setText(QApplication::translate("CategoryWidget", "Add", 0));
#ifndef QT_NO_WHATSTHIS
	lwTags->setWhatsThis(QApplication::translate("CategoryWidget", "Technorati tags (see www.technorati.com) for this entry.", 0));
#endif // QT_NO_WHATSTHIS
	gbTagPlace->setTitle(QApplication::translate("CategoryWidget", "Position of tags", 0));
#ifndef QT_NO_WHATSTHIS
	rbStartOfMainEntry->setWhatsThis(QApplication::translate("CategoryWidget", "Place the tags at the start of the entry", 0));
#endif // QT_NO_WHATSTHIS
	rbStartOfMainEntry->setText(QApplication::translate("CategoryWidget", "Start of main entry", 0));
#ifndef QT_NO_WHATSTHIS
	rbEndOfMainEntry->setWhatsThis(QApplication::translate("CategoryWidget", "Place the tags at the end of the main entry, which means before the More tag if there is one", 0));
#endif // QT_NO_WHATSTHIS
	rbEndOfMainEntry->setText(QApplication::translate("CategoryWidget", "End of main entry", 0));
	this->setTabText(this->indexOf(_technorati_tags), _technorati_tags->objectName());
	gbTrackbacks->setTitle(QApplication::translate("CategoryWidget", "Trackback pings", 0));
#ifndef QT_NO_WHATSTHIS
	lwTBPings->setWhatsThis(QApplication::translate("CategoryWidget", "Locations of web articles (usually blog entries) you want to trackback when posting this entry.", 0));
#endif // QT_NO_WHATSTHIS
	tbTBAdd->setText(QApplication::translate("CategoryWidget", "Add", 0));
	this->setTabText(this->indexOf(_trackbacks), _trackbacks->objectName());
	//

	////
	// QSettings qtm_settings(globalparameters.root_path() + "/" + globalparameters.target_os() + "/" + globalparameters._editor_conf_filename, QSettings::IniFormat);

	// QStringList line_list;
	// QList<int>	list;
	// qtm_settings.beginGroup("geometry");
	// line_list = (qtm_settings.value("splitter_sizelist", "100,100")).toString().split(",");
	// for(int i = 0; i < line_list.size(); ++ i) list.append(line_list.at(i).toInt());
	////	HidableTabWidget::_editentry->_blog_editor->_splitter->setSizes(list);
	// qtm_settings.endGroup();
	// if(list[0] <= this->tabBar()->sizeHint().width()) HidableTabWidget::collapse_when_true(true);
	// else{
	// this->resize(list[0], parentWidget()->geometry().height());	// parentWidget()->geometry().width() * 38 / 100	// 284
	//// 428
	// }
}

void SideTabWidget::title(const QString &title_){leTitle->setText(title_);}
QString SideTabWidget::title() const {return leTitle->text();}
void SideTabWidget::topic(const QString &topic_){_lineedit_topic->setText(topic_);}
QString SideTabWidget::topic() const {return _lineedit_topic->text();}
