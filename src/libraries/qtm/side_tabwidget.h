/********************************************************************************
** Form generated from reading UI file 'SideWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef _SIDEWIDGET_H
#define _SIDEWIDGET_H

#include "views/main_window/hidable_tab.h"
#include <QString>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class HidableTab;
class ts_t;
class FindScreen;
class EditorDock;
class wn_t;
class Blogger;
class QSplitter;
class AttachTableScreen;

namespace web {
class Docker;
class Profile;
}

QT_BEGIN_NAMESPACE

class SideTabWidget : public HidableTab {
	public:
	SideTabWidget(web::Docker *editor_docker_, Blogger *blogger_, std::shared_ptr<QSettings> topic_editor_config_, QSplitter *splitter, QWidget *parent);

	void title(const QString &title_);
	QString title() const;
	void topic(const QString &topic_);
	QString topic() const;

	QLineEdit *lineedit_topic();

	private:
	//	web::Docker *_editor_docker;
	Blogger *_blogger;
	// QVBoxLayout *vboxLayout;
	// QHBoxLayout *hboxLayout;
	QSpacerItem *spacerItem;
	// QComboBox *cbPageSelector;
	QSpacerItem *spacerItem1;
	// HidableTabWidget *this;
	QWidget *_basics;
	QVBoxLayout *verticalLayout_3;
	QGridLayout *gridLayout;
	QComboBox *cbStatus;
	QLineEdit *leTitle;
	QComboBox *cbAccountSelector;
	QLabel *lStatus;
	QComboBox *cbBlogSelector;
	QLineEdit *lePassword;
	QLabel *lBlog;
	QLabel *lTitle;
	QLabel *lAccount;
	QLabel *lPassword;
	QCheckBox *chShowPassword;
	QWidget *wBlankWidget;
	QGridLayout *gridLayout1;
	QLabel *label;
	QCheckBox *chAllowComments;
	QCheckBox *chAllowTB;
	QCheckBox *chSticky;
	QCheckBox *cbDoTB;
	QWidget *copyURLWidget;
	QHBoxLayout *horizontalLayout;
	QSpacerItem *horizontalSpacer;
	QPushButton *pbCopyURL;
	QSpacerItem *horizontalSpacer_2;
	QWidget *progressWidget;
	QHBoxLayout *horizontalLayout_2;
	QLabel *lProgressBarLabel;
	QProgressBar *progressBar;
	QSpacerItem *spacerItem2;

	QLabel *_label_topic;
	QLineEdit *_lineedit_topic;

	QWidget *_categories;
	QVBoxLayout *vboxLayout1;
	QGroupBox *gbCategory;
	QGridLayout *gridLayout_2;
	QLabel *lMainCat;
	QComboBox *cbMainCat;
	QLabel *lOtherCats;
	QListWidget *lwOtherCats;
	QCheckBox *chNoCats;
	QPushButton *pbRefresh;
	QPushButton *pbAddCategory;
	QWidget *_excerpt;
	QVBoxLayout *vboxLayout2;
	QGroupBox *gbExcerpt;
	QVBoxLayout *vboxLayout3;
	QTextEdit *teExcerpt;
	QLabel *lExcerptExplanation;
	QWidget *_wp_tags;
	QVBoxLayout *verticalLayout_2;
	QGroupBox *gbKeywordTags;
	QVBoxLayout *verticalLayout;
	QGridLayout *gridLayout_1;
	QLineEdit *leAddKeywordTag;
	QListWidget *lwAvailKeywordTags;
	QListWidget *lwKeywordTags;
	QToolButton *tbAddKeywordTag;
	QToolButton *tbSelectKeywordTag;
	QToolButton *tbRemoveKeywordTag;
	QPushButton *pbRefreshKeywordTags;
	QWidget *_technorati_tags;
	QHBoxLayout *hboxLayout1;
	QGroupBox *gbTags;
	QGridLayout *gridLayout2;
	QHBoxLayout *hboxLayout2;
	QLineEdit *leAddTag;
	QToolButton *tbAddTag;
	QListWidget *lwTags;
	QGroupBox *gbTagPlace;
	QVBoxLayout *vboxLayout4;
	QRadioButton *rbStartOfMainEntry;
	QRadioButton *rbEndOfMainEntry;
	QWidget *_trackbacks;
	QGridLayout *gridLayout3;
	QGroupBox *gbTrackbacks;
	QVBoxLayout *vboxLayout5;
	QListWidget *lwTBPings;
	QHBoxLayout *hboxLayout3;
	QLineEdit *leTBPingURL;
	QToolButton *tbTBAdd;

	// Виджет слоя прикрепляемых файлов
	AttachTableScreen *_attachtable_screen;

	protected:
	void setupUi();  // setupUi

	void retranslateUi();  // retranslateUi
	friend class Blogger;
};

QT_END_NAMESPACE

#endif  // _SIDEWIDGET_H
