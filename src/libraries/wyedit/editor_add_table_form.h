#ifndef _EDITORADDTABLEFORM_H_
#define _EDITORADDTABLEFORM_H_

#include <QDialog>

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

class QLabel;
class QSpinBox;
class QDialogButtonBox;

class EditorAddTableForm : public QDialog {
#if QT_VERSION == 0x050600
	W_OBJECT(EditorAddTableForm)
#else
	Q_OBJECT
#endif

	public:
	EditorAddTableForm();

	int get_columns(void);
	int get_rows(void);
	int get_width(void);

	private:
	QLabel *labelColumns;
	QLabel *labelRows;
	QLabel *labelWidth;
	QLabel *labelPercent;

	QSpinBox *spinColumns;
	QSpinBox *spinRows;
	QSpinBox *spinWidth;

	QDialogButtonBox *buttonBox;
};

#endif  // _EDITORADDTABLEFORM_H_
