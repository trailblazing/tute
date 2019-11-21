#ifndef _EDITORFINDDIALOG_H_
#define _EDITORFINDDIALOG_H_

#include <QDialog>
#include <QTextDocument>
#include <QWidget>
#include <memory>

#include <QtGlobal>
#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class QCheckBox;
class QLineEdit;
class QPushButton;
class FlatToolButton;
class FlatCheckBox;
class EditorConfig;

class EditorFindDialog : public QDialog {
#if QT_VERSION == 0x050600
	W_OBJECT(EditorFindDialog)
#else
	Q_OBJECT
#endif

	public:
	EditorFindDialog(std::shared_ptr<EditorConfig> editor_config, QWidget *parent = 0);

	signals:
	void find_text(const QString &text, QTextDocument::FindFlags flags)
#if QT_VERSION == 0x050600
	    W_SIGNAL(find_text, (const QString &, QTextDocument::FindFlags), text,
	             flags)  //
#else
	    ;
#endif

	    private slots : void find_clicked(void);
	void enable_find_button(const QString &text);

	private:
	QLineEdit *_line_edit;
	FlatCheckBox *_math_case;
	FlatCheckBox *_whole_words;
	FlatCheckBox *_search_backward;
	FlatToolButton *_find_button;

	void setup_ui(void);
	void setup_signals(void);
	void assembly(void);

	void hideEvent(QHideEvent *event);
	void showEvent(QShowEvent *event);

	std::shared_ptr<EditorConfig> _editor_config;
};

#endif /* _EDITORFINDDIALOG_H_ */
