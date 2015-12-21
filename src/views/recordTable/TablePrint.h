#ifndef _RECORD_TABLE_PRINT_H_
#define	_RECORD_TABLE_PRINT_H_

#include <QtGlobal>
#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextEdit>


class TableProxyModel;


class TablePrint : public QDialog {
    Q_OBJECT

public:
    TablePrint(QWidget *parent=0);
    virtual ~TablePrint();

    void setModel(TableProxyModel *iModel);
    void generateHtmlTableFromModel(void);
    void setTitleToHtml(QString title);

protected slots:

    void print(void);
    void save(void);

private:

    QTextEdit *textArea; // Отображение текста с таблицей, выводимой на печать
    QDialogButtonBox *buttonBox; // Линейка с кнопками

    QPushButton *printButton;
    QPushButton *saveButton;
    QPushButton *cancelButton;

    TableProxyModel *model;

    void setup_ui(void);
    void setup_signals(void);
    void assembly(void);
};

#endif	/* _RECORD_TABLE_PRINT_H_ */

