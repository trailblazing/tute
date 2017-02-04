#ifndef __REDUCEMESSAGEBOX_H__
#define __REDUCEMESSAGEBOX_H__

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QFlags>



#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif


// Виджет, похожий на QMessageBox, только позволяющий выделять и копировать текст
// (т. к. в Windows невозможно скопировать текст, отображаемый в QMessageBox)


class ReduceMessageBox : public QDialog {
#if QT_VERSION == 0x050600
    W_OBJECT(ReduceMessageBox)
#else
    Q_OBJECT
#endif

    public:
	ReduceMessageBox(QWidget *parent = 0);

	void	setText(QString iText);
	void	setDetailedText(QString iDetailedText);
	void	setDetailedTextReadOnly(bool iReadOnly);
	void	setStandardButtons(QFlags<QDialogButtonBox::StandardButton> buttons);

    protected:

	void	setupSignals(void);
	void	setupUI(void);
	void	assembly(void);


	QLabel			text;
	QTextEdit		detailedText;
	QDialogButtonBox	buttonBox;
};

#endif	// __REDUCEMESSAGEBOX_H__
