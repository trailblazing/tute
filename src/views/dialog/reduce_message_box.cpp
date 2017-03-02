

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QVBoxLayout>

#include "libraries/global_parameters.h"
#include "main.h"
#include "reduce_message_box.h"
#include "views/main_window/main_window.h"

extern std::shared_ptr<gl_para> gl_paras;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(ReduceMessageBox)
#endif

ReduceMessageBox::ReduceMessageBox(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    setupSignals();
    assembly();
}

void ReduceMessageBox::setupUI(void)
{
    int w = // find_object<MainWindow>("mainwindow")
        gl_paras->main_window()->width();
    int h = // find_object<MainWindow>("mainwindow")
        gl_paras->main_window()->height();

    this->resize(w * 2 / 3, h / 3);
}

void ReduceMessageBox::setupSignals(void)
{
    connect(&buttonBox, &QDialogButtonBox::accepted, this, &ReduceMessageBox::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, this, &ReduceMessageBox::reject);
}

void ReduceMessageBox::assembly(void)
{
    // азмещалка элементов
    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(&text);
    layout->addWidget(&detailedText);
    layout->addWidget(&buttonBox);
}

void ReduceMessageBox::setText(QString iText)
{
    text.setText(iText);
}

void ReduceMessageBox::setDetailedText(QString iDetailedText)
{
    detailedText.setText(iDetailedText);
}

void ReduceMessageBox::setDetailedTextReadOnly(bool iReadOnly)
{
    detailedText.setReadOnly(iReadOnly);
}

void ReduceMessageBox::setStandardButtons(QFlags<QDialogButtonBox::StandardButton> buttons)
{
    buttonBox.setStandardButtons(buttons);
}
