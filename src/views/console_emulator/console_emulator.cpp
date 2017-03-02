

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include <QScrollBar>
#include <QWidget>

#include "console_emulator.h"
#include "libraries/flat_control.h"
#include "main.h"
#include "models/app_config/app_config.h"
#include "views/find_in_base_screen/find_screen.h"

extern std::shared_ptr<AppConfig> appconfig;

#if QT_VERSION == 0x050600
W_OBJECT_IMPL(ConsoleEmulator)
#endif

ConsoleEmulator::ConsoleEmulator(QWidget* parent)
    : QDialog(parent)
{
    isError = false;

    setupUI();
    setupSignals();
    assembly();
}

ConsoleEmulator::~ConsoleEmulator()
{
    delete waitClock;
    delete messageLabel;
    delete buttonCancel;
    delete buttonDetails;
    delete consoleOutput;
    delete escShortcut;
}

void ConsoleEmulator::setupUI(void)
{
    messageLabel = new QLabel(this);

    buttonDetails = new FlatToolButton("", this);
    buttonDetails->setIcon(QIcon(":/resource/pic/expand_console.svg"));

    buttonCancel = new QPushButton(this);
    buttonCancel->setText(tr("Cancel"));
    buttonCancel->setDefault(true);

    consoleOutput = new QTextEdit(this);
    consoleOutput->setReadOnly(true);
    consoleOutput->setFontFamily("monospace");
    if (appconfig->synchro_console_details() == false)
        consoleOutput->hide();
    waitClock = new WaitClock(this);

    escShortcut = new QShortcut(QKeySequence("Esc"), this);
}

void ConsoleEmulator::setupSignals(void)
{
    connect(buttonCancel, &QPushButton::clicked, this, &ConsoleEmulator::onCancelClick);
    connect(buttonDetails, &FlatToolButton::clicked, this, &ConsoleEmulator::onDetailsClick);

    connect(escShortcut, &QShortcut::activated, this, &ConsoleEmulator::onCancelClick);
}

void ConsoleEmulator::assembly(void)
{
    upToolbar = new QHBoxLayout;
    upToolbar->addWidget(waitClock);
    upToolbar->addWidget(messageLabel);
    upToolbar->addStretch();
    upToolbar->addWidget(buttonDetails);
    upToolbar->addWidget(buttonCancel);

    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addLayout(upToolbar);
    vLayout->addWidget(consoleOutput);

    buttonCancel->setFocus();

    setLayout(vLayout);
}

void ConsoleEmulator::setMessageText(QString text)
{
    messageLabel->setText(text);
}

void ConsoleEmulator::setConsoleOutput(QString text)
{
    consoleOutput->setPlainText(text);
    consoleOutput->moveCursor(QTextCursor::End);

    QScrollBar* v = consoleOutput->verticalScrollBar();
    v->setValue(v->maximum());
}

void ConsoleEmulator::clearConsoleOutput(void)
{
    consoleOutput->setPlainText("");
}

void ConsoleEmulator::addConsoleOutput(QString text)
{
    consoleOutput->setPlainText(consoleOutput->toPlainText() + text);
    consoleOutput->moveCursor(QTextCursor::End);

    QScrollBar* v = consoleOutput->verticalScrollBar();
    v->setValue(v->maximum());
}

void ConsoleEmulator::onCancelClick(void)
{
    qDebug() << "ConsoleEmulator::onCancelClick() : Click cancel";

    this->close(); // Будет сгенерировано событие closeEvent
}

void ConsoleEmulator::closeEvent(QCloseEvent* event)
{
    qDebug() << "ConsoleEmulator::closeEvent() : Detect close event";

    emit cancelConsole();

    event->accept();
}

void ConsoleEmulator::onDetailsClick(void)
{
    if (consoleOutput->isHidden()) {
        consoleOutput->show();
        appconfig->synchro_console_details(true);
    } else {
        consoleOutput->hide();
        appconfig->synchro_console_details(false);
    }
    this->adjustSize();
}

void ConsoleEmulator::switchToErrorView(void)
{
    qDebug() << "ConsoleEmulator::switchToErrorView() : Detect error!";

    isError = true;

    // Верхняя строка скрывается
    // QLayoutItem *child;
    // while ((child = upToolbar->takeAt(0)) != 0)
    // child->widget()->hide();

    // Сообщение об обнаруженной ошибке
    messageLabel->setText("<b>" + tr("Commands running error") + "</b>");

    // Консольный вывод показывается, так как он возможно не был открыт, если не был выбран развернутый вид
    consoleOutput->show();
}
