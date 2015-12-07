/********************************************************************************
** Form generated from reading UI file 'passworddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PASSWORDDIALOG_H
#define UI_PASSWORDDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

namespace browser {

class Ui_PasswordDialog
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *hboxLayout;
    QLabel *iconLabel;
    QLabel *introLabel;
    QLabel *label;
    QLineEdit *userNameLineEdit;
    QLabel *lblPassword;
    QLineEdit *passwordLineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *browser__PasswordDialog)
    {
        if (browser__PasswordDialog->objectName().isEmpty())
            browser__PasswordDialog->setObjectName(QStringLiteral("browser__PasswordDialog"));
        browser__PasswordDialog->resize(510, 148);
        gridLayout = new QGridLayout(browser__PasswordDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        iconLabel = new QLabel(browser__PasswordDialog);
        iconLabel->setObjectName(QStringLiteral("iconLabel"));

        hboxLayout->addWidget(iconLabel);

        introLabel = new QLabel(browser__PasswordDialog);
        introLabel->setObjectName(QStringLiteral("introLabel"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(introLabel->sizePolicy().hasHeightForWidth());
        introLabel->setSizePolicy(sizePolicy);

        hboxLayout->addWidget(introLabel);


        gridLayout->addLayout(hboxLayout, 0, 0, 1, 2);

        label = new QLabel(browser__PasswordDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        userNameLineEdit = new QLineEdit(browser__PasswordDialog);
        userNameLineEdit->setObjectName(QStringLiteral("userNameLineEdit"));

        gridLayout->addWidget(userNameLineEdit, 1, 1, 1, 1);

        lblPassword = new QLabel(browser__PasswordDialog);
        lblPassword->setObjectName(QStringLiteral("lblPassword"));

        gridLayout->addWidget(lblPassword, 2, 0, 1, 1);

        passwordLineEdit = new QLineEdit(browser__PasswordDialog);
        passwordLineEdit->setObjectName(QStringLiteral("passwordLineEdit"));
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(passwordLineEdit, 2, 1, 1, 1);

        buttonBox = new QDialogButtonBox(browser__PasswordDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 3, 0, 1, 2);


        retranslateUi(browser__PasswordDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), browser__PasswordDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), browser__PasswordDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(browser__PasswordDialog);
    } // setupUi

    void retranslateUi(QDialog *browser__PasswordDialog)
    {
        browser__PasswordDialog->setWindowTitle(QApplication::translate("browser::PasswordDialog", "Authentication Required", 0));
        iconLabel->setText(QApplication::translate("browser::PasswordDialog", "DUMMY ICON", 0));
        introLabel->setText(QApplication::translate("browser::PasswordDialog", "INTRO TEXT DUMMY", 0));
        label->setText(QApplication::translate("browser::PasswordDialog", "Username:", 0));
        lblPassword->setText(QApplication::translate("browser::PasswordDialog", "Password:", 0));
    } // retranslateUi

};

} // namespace browser

namespace browser {
namespace Ui {
    class PasswordDialog: public Ui_PasswordDialog {};
} // namespace Ui
} // namespace browser

#endif // UI_PASSWORDDIALOG_H
