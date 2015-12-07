/********************************************************************************
** Form generated from reading UI file 'proxy.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROXY_H
#define UI_PROXY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

namespace browser {

class Ui_ProxyDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *iconLabel;
    QLabel *introLabel;
    QLabel *usernameLabel;
    QLineEdit *userNameLineEdit;
    QLabel *passwordLabel;
    QLineEdit *passwordLineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *browser__ProxyDialog)
    {
        if (browser__ProxyDialog->objectName().isEmpty())
            browser__ProxyDialog->setObjectName(QStringLiteral("browser__ProxyDialog"));
        browser__ProxyDialog->resize(510, 146);
        gridLayout = new QGridLayout(browser__ProxyDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        iconLabel = new QLabel(browser__ProxyDialog);
        iconLabel->setObjectName(QStringLiteral("iconLabel"));

        gridLayout->addWidget(iconLabel, 0, 0, 1, 1);

        introLabel = new QLabel(browser__ProxyDialog);
        introLabel->setObjectName(QStringLiteral("introLabel"));
        introLabel->setWordWrap(true);

        gridLayout->addWidget(introLabel, 0, 1, 1, 2);

        usernameLabel = new QLabel(browser__ProxyDialog);
        usernameLabel->setObjectName(QStringLiteral("usernameLabel"));

        gridLayout->addWidget(usernameLabel, 1, 0, 1, 2);

        userNameLineEdit = new QLineEdit(browser__ProxyDialog);
        userNameLineEdit->setObjectName(QStringLiteral("userNameLineEdit"));

        gridLayout->addWidget(userNameLineEdit, 1, 2, 1, 1);

        passwordLabel = new QLabel(browser__ProxyDialog);
        passwordLabel->setObjectName(QStringLiteral("passwordLabel"));

        gridLayout->addWidget(passwordLabel, 2, 0, 1, 2);

        passwordLineEdit = new QLineEdit(browser__ProxyDialog);
        passwordLineEdit->setObjectName(QStringLiteral("passwordLineEdit"));
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(passwordLineEdit, 2, 2, 1, 1);

        buttonBox = new QDialogButtonBox(browser__ProxyDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 3, 0, 1, 3);


        retranslateUi(browser__ProxyDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), browser__ProxyDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), browser__ProxyDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(browser__ProxyDialog);
    } // setupUi

    void retranslateUi(QDialog *browser__ProxyDialog)
    {
        browser__ProxyDialog->setWindowTitle(QApplication::translate("browser::ProxyDialog", "Proxy Authentication", 0));
        iconLabel->setText(QApplication::translate("browser::ProxyDialog", "ICON", 0));
        introLabel->setText(QApplication::translate("browser::ProxyDialog", "Connect to proxy", 0));
        usernameLabel->setText(QApplication::translate("browser::ProxyDialog", "Username:", 0));
        passwordLabel->setText(QApplication::translate("browser::ProxyDialog", "Password:", 0));
    } // retranslateUi

};

} // namespace browser

namespace browser {
namespace Ui {
    class ProxyDialog: public Ui_ProxyDialog {};
} // namespace Ui
} // namespace browser

#endif // UI_PROXY_H
