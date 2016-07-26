#ifndef EDITORSHOWTEXT_H
#define EDITORSHOWTEXT_H





#include <QDialog>
#include <QTextEdit>

#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif

// Класс отдельного (открепляемого) окна для просмотра текста записи без возможности редактирования
class EditorShowText : public QDialog {
#if QT_VERSION == 0x050600
    W_OBJECT(EditorShowText)
#else
    Q_OBJECT
#endif



    public:
	explicit EditorShowText(QWidget *parent = 0);
	virtual ~EditorShowText();

	void	setHtml(QString text);
	void	setDocument(QTextDocument *document);

    signals:

    public slots:

    private:

	QTextEdit *textArea;

	void	setupUi(void);
	void	setupSignals(void);
	void	assembly(void);
};

#endif	// EDITORSHOWTEXT_H
