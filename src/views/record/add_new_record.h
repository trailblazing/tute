#ifndef __ADDNEWRECORD_H__
#define __ADDNEWRECORD_H__

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <memory>


#include "libraries/global_parameters.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class Editor;
class InfoFieldEnter;
class Blogger;

// Окно добавления новой записи

class AddNewRecord : public QDialog {
#if QT_VERSION == 0x050600
	W_OBJECT(AddNewRecord)
#else
	Q_OBJECT
#endif

	public:
#if QT_VERSION < 0x050000
	AddNewRecord(QWidget *parent = 0, Qt::WFlags f = 0);
#else
	AddNewRecord(QWidget *parent = 0, Qt::WindowFlags f = 0);
#endif

	~AddNewRecord();

	QString getField(QString name);
	QString getImagesDirectory(void);

	private slots:

	void okClick(void);

	private:
	// Ввод инфополей записи
	InfoFieldEnter *infoField;

	blogger_ref _blogger;
	//// Ввод текста записи
	// Editor *recordTextEditor;

	QDialogButtonBox *buttonBox;

	QString imagesDirName;

	void setupUI(void);
	void setupSignals(void);
	void assembly(void);

	void setupEventFilter(void);

	virtual bool eventFilter(QObject *object, QEvent *event);
};

#endif  // __ADDNEWRECORD_H__
