#ifndef _EDITORMULTILINEINPUTDIALOG_H_
#define _EDITORMULTILINEINPUTDIALOG_H_

#include <QDialog>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QWidget>
#include <QtGlobal>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class EditorMultiLineInputDialog : public QDialog {
#if QT_VERSION == 0x050600
    W_OBJECT(EditorMultiLineInputDialog)
#else
    Q_OBJECT
#endif

public:
    EditorMultiLineInputDialog(QWidget* parent = 0);
    virtual ~EditorMultiLineInputDialog();

    void set_text(QString text);
    QString get_text();
    bool isModified(); // Выяснение, был ли изменен текст, показанный в диалоге

    void set_window_title(QString title);

private:
    QTextEdit* textArea;
    QDialogButtonBox* buttonBox;

    void setup_ui(void);
    void setup_signals(void);
    void assembly(void);
};

#endif /* _EDITORMULTILINEINPUTDIALOG_H_ */
