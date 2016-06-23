#ifndef _RECORD_TABLE_PRINT_H_
#define	_RECORD_TABLE_PRINT_H_

#include <QtGlobal>
#include <QWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextEdit>


class RecordProxyModel;
class rs_t;

class RecordPrint : public QDialog {
    Q_OBJECT

    public:
        RecordPrint(rs_t *_record_screen = 0);
        virtual ~RecordPrint();

        void setModel(RecordProxyModel *iModel);
        void generateHtmlTableFromModel(void);
        void setTitleToHtml(QString title);

    protected slots:

        void print(void);
        void save(void);

    private:

        QTextEdit *textArea;	// Отображение текста с таблицей, выводимой на печать
        QDialogButtonBox *buttonBox;	// Линейка с кнопками

        QPushButton *printButton;
        QPushButton *saveButton;
        QPushButton *cancelButton;

        RecordProxyModel *model;

        void setup_ui(void);
        void setup_signals(void);
        void assembly(void);
};

#endif /* _RECORD_TABLE_PRINT_H_ */

