/********************************************************************************
** Form generated from reading UI file 'cookies.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COOKIES_H
#define UI_COOKIES_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include "edittableview.h"
#include "searchlineedit.h"

namespace browser {

class Ui_CookiesDialog
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    SearchLineEdit *search;
    EditTableView *cookiesTable;
    QHBoxLayout *hboxLayout;
    QPushButton *removeButton;
    QPushButton *removeAllButton;
    QSpacerItem *spacerItem1;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *browser__CookiesDialog)
    {
        if (browser__CookiesDialog->objectName().isEmpty())
            browser__CookiesDialog->setObjectName(QStringLiteral("browser__CookiesDialog"));
        browser__CookiesDialog->resize(550, 370);
        gridLayout = new QGridLayout(browser__CookiesDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        spacerItem = new QSpacerItem(252, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 0, 0, 1, 1);

        search = new SearchLineEdit(browser__CookiesDialog);
        search->setObjectName(QStringLiteral("search"));

        gridLayout->addWidget(search, 0, 1, 1, 1);

        cookiesTable = new EditTableView(browser__CookiesDialog);
        cookiesTable->setObjectName(QStringLiteral("cookiesTable"));

        gridLayout->addWidget(cookiesTable, 1, 0, 1, 2);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        removeButton = new QPushButton(browser__CookiesDialog);
        removeButton->setObjectName(QStringLiteral("removeButton"));

        hboxLayout->addWidget(removeButton);

        removeAllButton = new QPushButton(browser__CookiesDialog);
        removeAllButton->setObjectName(QStringLiteral("removeAllButton"));

        hboxLayout->addWidget(removeAllButton);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem1);

        buttonBox = new QDialogButtonBox(browser__CookiesDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        hboxLayout->addWidget(buttonBox);


        gridLayout->addLayout(hboxLayout, 2, 0, 1, 2);


        retranslateUi(browser__CookiesDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), browser__CookiesDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(browser__CookiesDialog);
    } // setupUi

    void retranslateUi(QDialog *browser__CookiesDialog)
    {
        browser__CookiesDialog->setWindowTitle(QApplication::translate("browser::CookiesDialog", "Cookies", 0));
        removeButton->setText(QApplication::translate("browser::CookiesDialog", "&Remove", 0));
        removeAllButton->setText(QApplication::translate("browser::CookiesDialog", "Remove &All Cookies", 0));
    } // retranslateUi

};

} // namespace browser

namespace browser {
namespace Ui {
    class CookiesDialog: public Ui_CookiesDialog {};
} // namespace Ui
} // namespace browser

#endif // UI_COOKIES_H
