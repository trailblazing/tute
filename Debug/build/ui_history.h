/********************************************************************************
** Form generated from reading UI file 'history.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTORY_H
#define UI_HISTORY_H

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
#include "edittreeview.h"
#include "searchlineedit.h"

namespace browser {

class Ui_HistoryDialog
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    SearchLineEdit *search;
    EditTreeView *tree;
    QHBoxLayout *hboxLayout;
    QPushButton *removeButton;
    QPushButton *removeAllButton;
    QSpacerItem *spacerItem1;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *browser__HistoryDialog)
    {
        if (browser__HistoryDialog->objectName().isEmpty())
            browser__HistoryDialog->setObjectName(QStringLiteral("browser__HistoryDialog"));
        browser__HistoryDialog->resize(758, 450);
        gridLayout = new QGridLayout(browser__HistoryDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        spacerItem = new QSpacerItem(252, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 0, 0, 1, 1);

        search = new SearchLineEdit(browser__HistoryDialog);
        search->setObjectName(QStringLiteral("search"));

        gridLayout->addWidget(search, 0, 1, 1, 1);

        tree = new EditTreeView(browser__HistoryDialog);
        tree->setObjectName(QStringLiteral("tree"));

        gridLayout->addWidget(tree, 1, 0, 1, 2);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        removeButton = new QPushButton(browser__HistoryDialog);
        removeButton->setObjectName(QStringLiteral("removeButton"));

        hboxLayout->addWidget(removeButton);

        removeAllButton = new QPushButton(browser__HistoryDialog);
        removeAllButton->setObjectName(QStringLiteral("removeAllButton"));

        hboxLayout->addWidget(removeAllButton);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem1);

        buttonBox = new QDialogButtonBox(browser__HistoryDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        hboxLayout->addWidget(buttonBox);


        gridLayout->addLayout(hboxLayout, 2, 0, 1, 2);


        retranslateUi(browser__HistoryDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), browser__HistoryDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(browser__HistoryDialog);
    } // setupUi

    void retranslateUi(QDialog *browser__HistoryDialog)
    {
        browser__HistoryDialog->setWindowTitle(QApplication::translate("browser::HistoryDialog", "History", 0));
        removeButton->setText(QApplication::translate("browser::HistoryDialog", "&Remove", 0));
        removeAllButton->setText(QApplication::translate("browser::HistoryDialog", "Remove &All", 0));
    } // retranslateUi

};

} // namespace browser

namespace browser {
namespace Ui {
    class HistoryDialog: public Ui_HistoryDialog {};
} // namespace Ui
} // namespace browser

#endif // UI_HISTORY_H
