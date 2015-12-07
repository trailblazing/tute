/********************************************************************************
** Form generated from reading UI file 'downloads.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOADS_H
#define UI_DOWNLOADS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include "edittableview.h"

namespace browser {

class Ui_DownloadDialog
{
public:
    QGridLayout *gridLayout;
    EditTableView *downloadsView;
    QHBoxLayout *horizontalLayout;
    QPushButton *cleanupButton;
    QSpacerItem *spacerItem;
    QLabel *itemCount;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDialog *browser__DownloadDialog)
    {
        if (browser__DownloadDialog->objectName().isEmpty())
            browser__DownloadDialog->setObjectName(QStringLiteral("browser__DownloadDialog"));
        browser__DownloadDialog->resize(586, 252);
        gridLayout = new QGridLayout(browser__DownloadDialog);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        downloadsView = new EditTableView(browser__DownloadDialog);
        downloadsView->setObjectName(QStringLiteral("downloadsView"));

        gridLayout->addWidget(downloadsView, 0, 0, 1, 3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        cleanupButton = new QPushButton(browser__DownloadDialog);
        cleanupButton->setObjectName(QStringLiteral("cleanupButton"));
        cleanupButton->setEnabled(false);

        horizontalLayout->addWidget(cleanupButton);

        spacerItem = new QSpacerItem(58, 24, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);

        itemCount = new QLabel(browser__DownloadDialog);
        itemCount->setObjectName(QStringLiteral("itemCount"));

        gridLayout->addWidget(itemCount, 1, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(148, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 2, 1, 1);


        retranslateUi(browser__DownloadDialog);

        QMetaObject::connectSlotsByName(browser__DownloadDialog);
    } // setupUi

    void retranslateUi(QDialog *browser__DownloadDialog)
    {
        browser__DownloadDialog->setWindowTitle(QApplication::translate("browser::DownloadDialog", "Downloads", 0));
        cleanupButton->setText(QApplication::translate("browser::DownloadDialog", "Clean up", 0));
        itemCount->setText(QApplication::translate("browser::DownloadDialog", "0 Items", 0));
    } // retranslateUi

};

} // namespace browser

namespace browser {
namespace Ui {
    class DownloadDialog: public Ui_DownloadDialog {};
} // namespace Ui
} // namespace browser

#endif // UI_DOWNLOADS_H
