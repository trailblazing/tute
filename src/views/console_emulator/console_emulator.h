#ifndef _CONSOLEEMULATOR_H_
#define _CONSOLEEMULATOR_H_


#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include <QTextEdit>
#include <QCloseEvent>
#include <QShortcut>

#include "views/wait_clock/wait_clock.h"


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif


class FlatToolButton;

// Виджет, показывающий окно с консольным выводом
// Выглядит так:
// +----------+--------------------+-----+----------+
// | Картинка |Тут какая-то надпись| [v] | [Cancel] |
// +----------+--------------------+-----+----------+
// | # ls -1                                        |
// | file1.txt                                      |
// | file2.txt                                      |
// | file3.txt                                      |
// +------------------------------------------------+
// |                   [Close]                      |
// +------------------------------------------------+

class ConsoleEmulator : public QDialog {
#if QT_VERSION == 0x050600
    W_OBJECT(ConsoleEmulator)
#else
    Q_OBJECT
#endif

    public:
	ConsoleEmulator(QWidget *parent = 0);
	~ConsoleEmulator();

	void setMessageText(QString text);

	void	setConsoleOutput(QString text);
	void	clearConsoleOutput(void);
	void	addConsoleOutput(QString text);

	// Преобразование внешнего вида консольного эмулятора к отображению ошибки
	void switchToErrorView(void);

    public slots:

	// Слот срабатывающий при нажатии Cancel или при принудительном закрытии окна
	void onCancelClick(void);

	// Слот срабатывающий при клике на кнопку скрыть/показать консольный вывод
	void onDetailsClick(void);

    signals:

	// Сигнал, вырабатывающийся в слоте onCancelConsole()
	void cancelConsole(void)
#if QT_VERSION == 0x050600
	W_SIGNAL(cancelConsole)//
#else
	;
#endif

    private:

	void	setupUI(void);
	void	setupSignals(void);
	void	assembly(void);

	// Переопределенный метод, срабатывает при принудительном закрытии окна
	void closeEvent(QCloseEvent *event);

	WaitClock	*waitClock;
	QLabel		*messageLabel;
	QPushButton	*buttonCancel;
	FlatToolButton	*buttonDetails;

	// Верхний блок вида "Картинка |Тут какая-то надпись| [v] | [Cancel]"
	QHBoxLayout *upToolbar;

	QTextEdit *consoleOutput;

	QShortcut *escShortcut;

	// Возникала ли ошибка при выполнении команд
	bool isError;
};

#endif /* _CONSOLEEMULATOR_H_ */
