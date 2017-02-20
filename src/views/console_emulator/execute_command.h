#ifndef _EXECUTECOMMAND_H_
#define _EXECUTECOMMAND_H_

#include <QDialog>
#include <QProcess>
#include <QWidget>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class ConsoleEmulator;

class ExecuteCommand : public QDialog {
#if QT_VERSION == 0x050600
W_OBJECT(ExecuteCommand)
#else
Q_OBJECT
#endif

public:
	ExecuteCommand(QWidget *parent = 0);
	virtual ~ExecuteCommand();

	void setCommand(QString cmd);
	void run(void);

	void setWindowTitle(QString title);
	void setMessageText(QString text);

private slots:

	void manualCloseProcess(void);
	void errorHanler(QProcess::ProcessError error);

private:
	QString command;
	QString shell;
	QString windowTitle;
	QString messageText;

	QProcess *process;
	QTextCodec *outputCodec;

	ConsoleEmulator *console;

	bool isError;
	bool isManualClose;

	void printOutput(QProcess *process, ConsoleEmulator *console);
	void closeProcess(void);
};

#endif /* _EXECUTECOMMAND_H_ */
