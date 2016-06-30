#ifndef __RECORDINFOFIELDSEDITOR_H__
#define __RECORDINFOFIELDSEDITOR_H__

#include <wobjectdefs.h>
#include <QObject>

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

class InfoFieldEnter;

// Окно редактирования инфополей записи

class InfoFieldsEditor : public QDialog {
    W_OBJECT(InfoFieldsEditor)

public:
#if QT_VERSION < 0x050000
    RecordInfoFieldsEditor(QWidget *parent = 0, Qt::WFlags f = 0);
#else
    InfoFieldsEditor(QWidget *parent = 0, Qt::WindowFlags f = 0);
#endif

    ~InfoFieldsEditor();

    QString getField(QString name);
    void setField(QString name, QString value);

private slots:

    void okClick(void);

private:

    // Widget input information field recording // Виджет ввода инфополей записи
    InfoFieldEnter *infoField;

    QDialogButtonBox *buttonBox;

    void setupUI(void);
    void setupSignals(void);
    void assembly(void);
};

#endif // __RECORDINFOFIELDSEDITOR_H__
