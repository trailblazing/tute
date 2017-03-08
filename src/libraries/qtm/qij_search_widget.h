/*******************************************************************************
*
* Foxhound Search Widget - A Qt document search widget based on the
* Qt Designer search widget
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License (version 2), as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Written by Matthew J Smith , 12th Feb 2008
*
*******************************************************************************/

#ifndef QIJSEARCHWIDGET_H
#define QIJSEARCHWIDGET_H

#include "ui_QijSearchWidget.h"

class QTextEdit;
class QObject;
class QEvent;

#if QT_VERSION >= 0x040400
class QPlainTextEdit;
#else
#define QPlainTextEdit void
#endif

#if !defined DONT_USE_PTE

class QPlainTextEdit;
#define TEXTEDIT_FOR_READ QPlainTextEdit
#define TEXTEDIT MetaEditor

#else  //DONT_USE_PTE

#ifdef USE_WYEDIT

#ifdef USE_EDITOR_WRAP
//#define TEXTEDIT_FOR_READ QTextEdit
class EditorWrap;
//#define TEXTEDIT EditorWrap
typedef EditorWrap TEXTEDIT;
#else  //USE_EDITOR_WRAP

class Editor;
typedef Editor TEXTEDIT;

#endif  // USE_EDITOR_WRAP
typedef QTextEdit TEXTEDIT_FOR_READ;
#else   //USE_WYEDIT

//#define TEXTEDIT_FOR_READ	QTextEdit
//#define TEXTEDIT		QTextEdit
typedef QTextEdit TEXTEDIT_FOR_READ;
typedef QTextEdit TEXTEDIT;

#endif  //USE_WYEDIT

#endif  //DONT_USE_PTE

class QijSearchWidget : public QWidget, private Ui::SearchWidgetBase {
	Q_OBJECT

	public:
	QijSearchWidget(TEXTEDIT *, QWidget *parent = 0);
	QijSearchWidget(QPlainTextEdit *, QWidget *parent = 0);
	QPlainTextEdit *plainTextEdit() {
		return _plainTextEdit;
	}
	// QTextEdit
	TEXTEDIT *textEdit() {
		return _editor;
	}
	bool expertEnabled() {
		return _expertEnabled;
	}

	public slots:
	void setTextEdit(QWidget *);
	/*  void setTextEdit( QTextEdit * );
             void setTextEdit( QPlainTextEdit * ); */
	void find();
	void findAgain();
	void setExpertEnabled(bool);
	void clearSearchText();

	protected:
	bool eventFilter(QObject *, QEvent *);
	virtual void closeEvent(QCloseEvent *);

	private:
	enum direction { Forward,
		             Backward,
		             Stay };
	void findInTextEdit(const QString &, direction d = Stay);

	bool matchCase, wholeWords, isRegexSearch;
	bool _expertEnabled;
	direction dir;
	// QTextEdit
	TEXTEDIT *_editor;
	QPlainTextEdit *_plainTextEdit;
	QString currentCursorText;

	private slots:
	void on_tbClose_clicked();
	void on_leFindText_textChanged(const QString &);
	void on_leFindText_returnPressed();
	void on_tbSearchDown_clicked(bool);
	void on_tbSearchUp_clicked(bool);
	void on_tbSelectAll_clicked(bool);
	void on_chMatchCase_toggled(bool);
	void on_chWholeWords_toggled(bool);
	void on_chRegExp_toggled(bool);

	signals:
	void searchMessage(const QString &);
};

#endif
