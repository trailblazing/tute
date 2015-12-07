/********************************************************************************
** Form generated from reading UI file 'downloaditem.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DOWNLOADITEM_H
#define UI_DOWNLOADITEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "squeezelabel.h"

namespace browser {

class Ui_DownloadItem
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *fileIcon;
    QVBoxLayout *verticalLayout_2;
    SqueezeLabel *fileNameLabel;
    QProgressBar *progressBar;
    SqueezeLabel *downloadInfoLabel;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *stopButton;
    QPushButton *openButton;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *browser__DownloadItem)
    {
        if (browser__DownloadItem->objectName().isEmpty())
            browser__DownloadItem->setObjectName(QStringLiteral("browser__DownloadItem"));
        browser__DownloadItem->resize(423, 110);
        horizontalLayout = new QHBoxLayout(browser__DownloadItem);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        fileIcon = new QLabel(browser__DownloadItem);
        fileIcon->setObjectName(QStringLiteral("fileIcon"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fileIcon->sizePolicy().hasHeightForWidth());
        fileIcon->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(fileIcon);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        fileNameLabel = new SqueezeLabel(browser__DownloadItem);
        fileNameLabel->setObjectName(QStringLiteral("fileNameLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(fileNameLabel->sizePolicy().hasHeightForWidth());
        fileNameLabel->setSizePolicy(sizePolicy1);

        verticalLayout_2->addWidget(fileNameLabel);

        progressBar = new QProgressBar(browser__DownloadItem);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(0);

        verticalLayout_2->addWidget(progressBar);

        downloadInfoLabel = new SqueezeLabel(browser__DownloadItem);
        downloadInfoLabel->setObjectName(QStringLiteral("downloadInfoLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(downloadInfoLabel->sizePolicy().hasHeightForWidth());
        downloadInfoLabel->setSizePolicy(sizePolicy2);

        verticalLayout_2->addWidget(downloadInfoLabel);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalSpacer = new QSpacerItem(17, 1, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        stopButton = new QPushButton(browser__DownloadItem);
        stopButton->setObjectName(QStringLiteral("stopButton"));

        verticalLayout->addWidget(stopButton);

        openButton = new QPushButton(browser__DownloadItem);
        openButton->setObjectName(QStringLiteral("openButton"));

        verticalLayout->addWidget(openButton);

        verticalSpacer_2 = new QSpacerItem(17, 5, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(browser__DownloadItem);

        QMetaObject::connectSlotsByName(browser__DownloadItem);
    } // setupUi

    void retranslateUi(QWidget *browser__DownloadItem)
    {
        browser__DownloadItem->setWindowTitle(QApplication::translate("browser::DownloadItem", "Form", 0));
        fileIcon->setText(QApplication::translate("browser::DownloadItem", "Ico", 0));
        fileNameLabel->setProperty("text", QVariant(QApplication::translate("browser::DownloadItem", "Filename", 0)));
        downloadInfoLabel->setProperty("text", QVariant(QString()));
        stopButton->setText(QApplication::translate("browser::DownloadItem", "Stop", 0));
        openButton->setText(QApplication::translate("browser::DownloadItem", "Open", 0));
    } // retranslateUi

};

} // namespace browser

namespace browser {
namespace Ui {
    class DownloadItem: public Ui_DownloadItem {};
} // namespace Ui
} // namespace browser

#endif // UI_DOWNLOADITEM_H
