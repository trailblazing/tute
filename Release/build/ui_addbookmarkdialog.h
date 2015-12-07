/********************************************************************************
** Form generated from reading UI file 'addbookmarkdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDBOOKMARKDIALOG_H
#define UI_ADDBOOKMARKDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

namespace browser {

class Ui_AddBookmarkDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *name;
    QComboBox *location;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *browser__AddBookmarkDialog)
    {
        if (browser__AddBookmarkDialog->objectName().isEmpty())
            browser__AddBookmarkDialog->setObjectName(QStringLiteral("browser__AddBookmarkDialog"));
        browser__AddBookmarkDialog->resize(450, 168);
        verticalLayout = new QVBoxLayout(browser__AddBookmarkDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(browser__AddBookmarkDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setTextFormat(Qt::PlainText);
        label->setWordWrap(true);

        verticalLayout->addWidget(label);

        name = new QLineEdit(browser__AddBookmarkDialog);
        name->setObjectName(QStringLiteral("name"));

        verticalLayout->addWidget(name);

        location = new QComboBox(browser__AddBookmarkDialog);
        location->setObjectName(QStringLiteral("location"));

        verticalLayout->addWidget(location);

        verticalSpacer = new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(browser__AddBookmarkDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(false);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(browser__AddBookmarkDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), browser__AddBookmarkDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), browser__AddBookmarkDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(browser__AddBookmarkDialog);
    } // setupUi

    void retranslateUi(QDialog *browser__AddBookmarkDialog)
    {
        browser__AddBookmarkDialog->setWindowTitle(QApplication::translate("browser::AddBookmarkDialog", "Add Bookmark", 0));
        label->setText(QApplication::translate("browser::AddBookmarkDialog", "Type a name for the bookmark, and choose where to keep it.", 0));
    } // retranslateUi

};

} // namespace browser

namespace browser {
namespace Ui {
    class AddBookmarkDialog: public Ui_AddBookmarkDialog {};
} // namespace Ui
} // namespace browser

#endif // UI_ADDBOOKMARKDIALOG_H
