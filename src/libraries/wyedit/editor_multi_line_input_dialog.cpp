
#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QDebug>
#include <QGridLayout>
#include <QGroupBox>
#include <QVBoxLayout>

#include "editor_multi_line_input_dialog.h"

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(EditorMultiLineInputDialog)
#endif

EditorMultiLineInputDialog::EditorMultiLineInputDialog(QWidget* parent)
    : QDialog(parent)
{
    setup_ui();
    setup_signals();
    assembly();
}

EditorMultiLineInputDialog::~EditorMultiLineInputDialog()
{
    if (textArea != nullptr)
        delete textArea;
    if (buttonBox != nullptr)
        delete buttonBox;
}

void EditorMultiLineInputDialog::setup_ui()
{
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);

    textArea = new QTextEdit(this);
    textArea->setAcceptRichText(false);
    textArea->setSizePolicy(sizePolicy);

    buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(tr("OK"), QDialogButtonBox::AcceptRole);
    buttonBox->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
    // Устанавливается размер окна, равный виджету, из которого
    // этот виджет был вызван
    if (this->parent()->isWidgetType()) {
        QWidget* parentWidget = qobject_cast<QWidget*>(this->parent());
        QRect geom(parentWidget->pos(), parentWidget->size());

        qDebug() << "Parent window geometry " << geom.x() << geom.y() << geom.width() << geom.height();

        setMinimumSize(parentWidget->size());
    }
}

void EditorMultiLineInputDialog::setup_signals()
{
    connect(buttonBox, &QDialogButtonBox::accepted, this, &EditorMultiLineInputDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &EditorMultiLineInputDialog::reject);
}

void EditorMultiLineInputDialog::assembly()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Добавляется область текста
    mainLayout->addWidget(textArea);

    // Добавляется линейка кнопок OK и Cancel
    mainLayout->addWidget(buttonBox);
}

void EditorMultiLineInputDialog::set_window_title(QString title)
{
    this->setWindowTitle(title);
}

void EditorMultiLineInputDialog::set_text(QString text)
{
    textArea->setPlainText(text);
}

QString EditorMultiLineInputDialog::get_text()
{
    return textArea->toPlainText();
}

// Выясняется, был ли изменен текст, показанный в диалоге
bool EditorMultiLineInputDialog::isModified()
{
    return textArea->document()->isModified();
}
