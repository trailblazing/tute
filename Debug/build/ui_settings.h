/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace browser {

class Ui_Settings
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout_4;
    QLabel *label_3;
    QLineEdit *homeLineEdit;
    QPushButton *setHomeToCurrentPageButton;
    QSpacerItem *horizontalSpacer;
    QLabel *label_4;
    QComboBox *expireHistory;
    QLabel *label_7;
    QLineEdit *downloadsLocation;
    QLabel *label_8;
    QComboBox *openLinksIn;
    QCheckBox *enableScrollAnimator;
    QSpacerItem *spacerItem;
    QWidget *tab_3;
    QGridLayout *gridLayout_3;
    QLabel *label_5;
    QLabel *standardLabel;
    QPushButton *standardFontButton;
    QLabel *label_6;
    QLabel *fixedLabel;
    QPushButton *fixedFontButton;
    QSpacerItem *verticalSpacer;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *enablePlugins;
    QCheckBox *enableJavascript;
    QGroupBox *cookiesGroupBox;
    QGridLayout *gridLayout1;
    QLabel *label_2;
    QComboBox *sessionCookiesCombo;
    QLabel *label;
    QLineEdit *persistentDataPath;
    QSpacerItem *verticalSpacer_4;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout;
    QGroupBox *proxySupport;
    QGridLayout *gridLayout_6;
    QLabel *label_9;
    QComboBox *proxyType;
    QLabel *label_10;
    QLineEdit *proxyHostName;
    QLabel *label_11;
    QSpinBox *proxyPort;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_12;
    QLineEdit *proxyUserName;
    QLabel *label_13;
    QLineEdit *proxyPassword;
    QSpacerItem *verticalSpacer_2;
    QWidget *tab_5;
    QGridLayout *gridLayout_2;
    QTextEdit *userStyleSheet;
    QLineEdit *httpUserAgent;
    QLabel *label_15;
    QLabel *label_14;
    QSpacerItem *verticalSpacer_3;

    void setupUi(QDialog *browser__Settings)
    {
        if (browser__Settings->objectName().isEmpty())
            browser__Settings->setObjectName(QStringLiteral("browser__Settings"));
        browser__Settings->resize(657, 325);
        gridLayout = new QGridLayout(browser__Settings);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        buttonBox = new QDialogButtonBox(browser__Settings);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 0, 1, 1);

        tabWidget = new QTabWidget(browser__Settings);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_4 = new QGridLayout(tab);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_4->addWidget(label_3, 0, 0, 1, 1);

        homeLineEdit = new QLineEdit(tab);
        homeLineEdit->setObjectName(QStringLiteral("homeLineEdit"));

        gridLayout_4->addWidget(homeLineEdit, 0, 1, 1, 2);

        setHomeToCurrentPageButton = new QPushButton(tab);
        setHomeToCurrentPageButton->setObjectName(QStringLiteral("setHomeToCurrentPageButton"));

        gridLayout_4->addWidget(setHomeToCurrentPageButton, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(280, 18, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer, 1, 2, 1, 1);

        label_4 = new QLabel(tab);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_4->addWidget(label_4, 2, 0, 1, 1);

        expireHistory = new QComboBox(tab);
        expireHistory->setObjectName(QStringLiteral("expireHistory"));

        gridLayout_4->addWidget(expireHistory, 2, 1, 1, 2);

        label_7 = new QLabel(tab);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_4->addWidget(label_7, 3, 0, 1, 1);

        downloadsLocation = new QLineEdit(tab);
        downloadsLocation->setObjectName(QStringLiteral("downloadsLocation"));

        gridLayout_4->addWidget(downloadsLocation, 3, 1, 1, 2);

        label_8 = new QLabel(tab);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_4->addWidget(label_8, 4, 0, 1, 1);

        openLinksIn = new QComboBox(tab);
        openLinksIn->setObjectName(QStringLiteral("openLinksIn"));

        gridLayout_4->addWidget(openLinksIn, 4, 1, 1, 2);

        enableScrollAnimator = new QCheckBox(tab);
        enableScrollAnimator->setObjectName(QStringLiteral("enableScrollAnimator"));
        enableScrollAnimator->setChecked(true);

        gridLayout_4->addWidget(enableScrollAnimator, 5, 1, 1, 2);

        spacerItem = new QSpacerItem(391, 262, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(spacerItem, 6, 1, 1, 2);

        tabWidget->addTab(tab, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout_3 = new QGridLayout(tab_3);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_5 = new QLabel(tab_3);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_5, 0, 0, 1, 1);

        standardLabel = new QLabel(tab_3);
        standardLabel->setObjectName(QStringLiteral("standardLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(standardLabel->sizePolicy().hasHeightForWidth());
        standardLabel->setSizePolicy(sizePolicy);
        standardLabel->setFrameShape(QFrame::StyledPanel);
        standardLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(standardLabel, 0, 1, 1, 1);

        standardFontButton = new QPushButton(tab_3);
        standardFontButton->setObjectName(QStringLiteral("standardFontButton"));

        gridLayout_3->addWidget(standardFontButton, 0, 2, 1, 1);

        label_6 = new QLabel(tab_3);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(label_6, 1, 0, 1, 1);

        fixedLabel = new QLabel(tab_3);
        fixedLabel->setObjectName(QStringLiteral("fixedLabel"));
        fixedLabel->setFrameShape(QFrame::StyledPanel);
        fixedLabel->setAlignment(Qt::AlignCenter);

        gridLayout_3->addWidget(fixedLabel, 1, 1, 1, 1);

        fixedFontButton = new QPushButton(tab_3);
        fixedFontButton->setObjectName(QStringLiteral("fixedFontButton"));

        gridLayout_3->addWidget(fixedFontButton, 1, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 93, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 2, 1, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_3 = new QVBoxLayout(tab_2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        groupBox = new QGroupBox(tab_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        enablePlugins = new QCheckBox(groupBox);
        enablePlugins->setObjectName(QStringLiteral("enablePlugins"));
        enablePlugins->setChecked(true);

        verticalLayout_2->addWidget(enablePlugins);

        enableJavascript = new QCheckBox(groupBox);
        enableJavascript->setObjectName(QStringLiteral("enableJavascript"));
        enableJavascript->setChecked(true);

        verticalLayout_2->addWidget(enableJavascript);


        verticalLayout_3->addWidget(groupBox);

        cookiesGroupBox = new QGroupBox(tab_2);
        cookiesGroupBox->setObjectName(QStringLiteral("cookiesGroupBox"));
        gridLayout1 = new QGridLayout(cookiesGroupBox);
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        gridLayout1->setContentsMargins(9, -1, -1, -1);
        label_2 = new QLabel(cookiesGroupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout1->addWidget(label_2, 0, 0, 1, 1);

        sessionCookiesCombo = new QComboBox(cookiesGroupBox);
        sessionCookiesCombo->setObjectName(QStringLiteral("sessionCookiesCombo"));

        gridLayout1->addWidget(sessionCookiesCombo, 0, 1, 1, 1);

        label = new QLabel(cookiesGroupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout1->addWidget(label, 1, 0, 1, 1);

        persistentDataPath = new QLineEdit(cookiesGroupBox);
        persistentDataPath->setObjectName(QStringLiteral("persistentDataPath"));

        gridLayout1->addWidget(persistentDataPath, 1, 1, 1, 1);


        verticalLayout_3->addWidget(cookiesGroupBox);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_4);

        tabWidget->addTab(tab_2, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QStringLiteral("tab_4"));
        verticalLayout = new QVBoxLayout(tab_4);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        proxySupport = new QGroupBox(tab_4);
        proxySupport->setObjectName(QStringLiteral("proxySupport"));
        proxySupport->setCheckable(true);
        gridLayout_6 = new QGridLayout(proxySupport);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        label_9 = new QLabel(proxySupport);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_6->addWidget(label_9, 0, 0, 1, 1);

        proxyType = new QComboBox(proxySupport);
        proxyType->setObjectName(QStringLiteral("proxyType"));

        gridLayout_6->addWidget(proxyType, 0, 1, 1, 2);

        label_10 = new QLabel(proxySupport);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_6->addWidget(label_10, 1, 0, 1, 1);

        proxyHostName = new QLineEdit(proxySupport);
        proxyHostName->setObjectName(QStringLiteral("proxyHostName"));

        gridLayout_6->addWidget(proxyHostName, 1, 1, 1, 2);

        label_11 = new QLabel(proxySupport);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_6->addWidget(label_11, 2, 0, 1, 1);

        proxyPort = new QSpinBox(proxySupport);
        proxyPort->setObjectName(QStringLiteral("proxyPort"));
        proxyPort->setMaximum(10000);
        proxyPort->setValue(1080);

        gridLayout_6->addWidget(proxyPort, 2, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(293, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(horizontalSpacer_2, 2, 2, 1, 1);

        label_12 = new QLabel(proxySupport);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_6->addWidget(label_12, 3, 0, 1, 1);

        proxyUserName = new QLineEdit(proxySupport);
        proxyUserName->setObjectName(QStringLiteral("proxyUserName"));

        gridLayout_6->addWidget(proxyUserName, 3, 1, 1, 2);

        label_13 = new QLabel(proxySupport);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_6->addWidget(label_13, 4, 0, 1, 1);

        proxyPassword = new QLineEdit(proxySupport);
        proxyPassword->setObjectName(QStringLiteral("proxyPassword"));
        proxyPassword->setEchoMode(QLineEdit::Password);

        gridLayout_6->addWidget(proxyPassword, 4, 1, 1, 2);

        verticalSpacer_2 = new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_6->addItem(verticalSpacer_2, 5, 0, 1, 1);


        verticalLayout->addWidget(proxySupport);

        tabWidget->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QStringLiteral("tab_5"));
        gridLayout_2 = new QGridLayout(tab_5);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        userStyleSheet = new QTextEdit(tab_5);
        userStyleSheet->setObjectName(QStringLiteral("userStyleSheet"));

        gridLayout_2->addWidget(userStyleSheet, 2, 1, 1, 1);

        httpUserAgent = new QLineEdit(tab_5);
        httpUserAgent->setObjectName(QStringLiteral("httpUserAgent"));

        gridLayout_2->addWidget(httpUserAgent, 0, 1, 1, 1);

        label_15 = new QLabel(tab_5);
        label_15->setObjectName(QStringLiteral("label_15"));

        gridLayout_2->addWidget(label_15, 0, 0, 1, 1);

        label_14 = new QLabel(tab_5);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout_2->addWidget(label_14, 2, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 176, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_3, 4, 1, 1, 1);

        tabWidget->addTab(tab_5, QString());

        gridLayout->addWidget(tabWidget, 1, 0, 1, 1);


        retranslateUi(browser__Settings);
        QObject::connect(buttonBox, SIGNAL(accepted()), browser__Settings, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), browser__Settings, SLOT(reject()));

        tabWidget->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(browser__Settings);
    } // setupUi

    void retranslateUi(QDialog *browser__Settings)
    {
        browser__Settings->setWindowTitle(QApplication::translate("browser::Settings", "Settings", 0));
        label_3->setText(QApplication::translate("browser::Settings", "Home:", 0));
        setHomeToCurrentPageButton->setText(QApplication::translate("browser::Settings", "Set to current page", 0));
        label_4->setText(QApplication::translate("browser::Settings", "Remove history items:", 0));
        expireHistory->clear();
        expireHistory->insertItems(0, QStringList()
         << QApplication::translate("browser::Settings", "After one day", 0)
         << QApplication::translate("browser::Settings", "After one week", 0)
         << QApplication::translate("browser::Settings", "After two weeks", 0)
         << QApplication::translate("browser::Settings", "After one month", 0)
         << QApplication::translate("browser::Settings", "After one year", 0)
         << QApplication::translate("browser::Settings", "Manually", 0)
        );
        label_7->setText(QApplication::translate("browser::Settings", "Save downloads to:", 0));
        label_8->setText(QApplication::translate("browser::Settings", "Open links from applications:", 0));
        openLinksIn->clear();
        openLinksIn->insertItems(0, QStringList()
         << QApplication::translate("browser::Settings", "In a tab in the current window", 0)
         << QApplication::translate("browser::Settings", "In a new window", 0)
        );
        enableScrollAnimator->setText(QApplication::translate("browser::Settings", "Enable Scroll Animator", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("browser::Settings", "General", 0));
        label_5->setText(QApplication::translate("browser::Settings", "Standard font:", 0));
        standardLabel->setText(QApplication::translate("browser::Settings", "Times 16", 0));
        standardFontButton->setText(QApplication::translate("browser::Settings", "Select...", 0));
        label_6->setText(QApplication::translate("browser::Settings", "Fixed-width font:", 0));
        fixedLabel->setText(QApplication::translate("browser::Settings", "Courier 13", 0));
        fixedFontButton->setText(QApplication::translate("browser::Settings", "Select...", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("browser::Settings", "Appearance", 0));
        groupBox->setTitle(QApplication::translate("browser::Settings", "Web Content", 0));
        enablePlugins->setText(QApplication::translate("browser::Settings", "Enable Plugins", 0));
        enableJavascript->setText(QApplication::translate("browser::Settings", "Enable Javascript", 0));
        cookiesGroupBox->setTitle(QApplication::translate("browser::Settings", "Cookies", 0));
        label_2->setText(QApplication::translate("browser::Settings", "Persistent Cookie Policy", 0));
        sessionCookiesCombo->clear();
        sessionCookiesCombo->insertItems(0, QStringList()
         << QApplication::translate("browser::Settings", "Treat all cookies as session cookies", 0)
         << QApplication::translate("browser::Settings", "Allow persistent cookies", 0)
         << QApplication::translate("browser::Settings", "Treat all cookies as persistent cookies", 0)
        );
        label->setText(QApplication::translate("browser::Settings", "Persistent Data Path:", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("browser::Settings", "Privacy", 0));
        proxySupport->setTitle(QApplication::translate("browser::Settings", "Enable proxy", 0));
        label_9->setText(QApplication::translate("browser::Settings", "Type:", 0));
        proxyType->clear();
        proxyType->insertItems(0, QStringList()
         << QApplication::translate("browser::Settings", "Socks5", 0)
         << QApplication::translate("browser::Settings", "Http", 0)
        );
        label_10->setText(QApplication::translate("browser::Settings", "Host:", 0));
        label_11->setText(QApplication::translate("browser::Settings", "Port:", 0));
        label_12->setText(QApplication::translate("browser::Settings", "User Name:", 0));
        label_13->setText(QApplication::translate("browser::Settings", "Password:", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("browser::Settings", "Proxy", 0));
        label_15->setText(QApplication::translate("browser::Settings", "HTTP User-Agent:", 0));
        label_14->setText(QApplication::translate("browser::Settings", "Style Sheet:", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("browser::Settings", "Advanced", 0));
    } // retranslateUi

};

} // namespace browser

namespace browser {
namespace Ui {
    class Settings: public Ui_Settings {};
} // namespace Ui
} // namespace browser

#endif // UI_SETTINGS_H
