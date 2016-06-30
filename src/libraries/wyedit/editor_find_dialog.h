#ifndef _EDITORFINDDIALOG_H_
#define	_EDITORFINDDIALOG_H_


#include <wobjectdefs.h>
#include <QObject>


#include <QWidget>
#include <QDialog>
#include <QTextDocument>


class QCheckBox;
class QLineEdit;
class QPushButton;


class EditorFindDialog : public QDialog {
    W_OBJECT(EditorFindDialog)

    public:
	EditorFindDialog(QWidget *parent = 0);

    signals:
	void find_text(const QString &text, QTextDocument::FindFlags flags) W_SIGNAL(find_text, (const QString &, QTextDocument::FindFlags), text, flags)// ;

    private slots:
	void	find_clicked(void);
	void	enable_find_button(const QString &text);

    private:
	QLineEdit	*lineEdit;
	QCheckBox	*mathCase;
	QCheckBox	*wholeWords;
	QCheckBox	*searchBackward;
	QPushButton	*findButton;

	void	setup_ui(void);
	void	setup_signals(void);
	void	assembly(void);

	void	hideEvent(QHideEvent *event);
	void	showEvent(QShowEvent *event);
};

#endif /* _EDITORFINDDIALOG_H_ */

