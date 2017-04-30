/***************************************************************************
 * QuickpostTemplateDialog.cc
 *
 * Copyright (C) 2007, 2008, Matthew J Smith
 *
 * This file is part of QTM.
 * QTM is free software; you can redistribute it and/or modify
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
 *****************************************************************************/

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QModelIndex>
#include <QString>
#include <QStringList>
#include <QTextEdit>
#include <QWhatsThis>
#include <QWidget>

#include "quickpost_template_dialog.h"
#include "ui_QuickpostTemplateForm.h"

QuickpostTemplateDialog::QuickpostTemplateDialog(QStringList &titles, QStringList &templates, QList<int> &pubStates, QList<bool> &ctStates, QList<QStringList> &ahLists, bool status, QWidget *parent)
    : QDialog(parent) {
	setupUi(this);

	teTemplateText->setWhatsThis(
	    tr("<p>Enter the text of your template here, in HTML "
	       "format.</p>"
	       "<p>Four place-markers are supported:</p>"
	       "<p><strong>%title%</strong> - The page's title<br />"
	       "<strong>%url%</strong> - the full web location (URL) of "
	       "the page being blogged;<br />"
	       "<strong>%host%</strong> - the page's host name (after http://), "
	       "and<br />"
	       "<strong>%domain%</strong> - the page's hostname without the "
	       "\"www\".</p>"));
	lTemplate->setWhatsThis(
	    tr("<p>Enter the text of your template here, in HTML "
	       "format.</p>"
	       "<p>Four place-markers are supported:</p>"
	       "<p><strong>%title%</strong> - The page's title<br />"
	       "<strong>%url%</strong> - the full web location (URL) of "
	       "the page being blogged;<br />"
	       "<strong>%host%</strong> - the page's host name (after http://), "
	       "and<br />"
	       "<strong>%domain%</strong> - the page's hostname without the "
	       "\"www\".</p>"));
	cbDefaultPublishStatus->setWhatsThis(
	    tr("<p>Set this to Draft or Publish if you always want "
	       "posts based on this template to have that status.</p>"
	       "<p>Otherwise, the post will have the status you specify in "
	       "the Preferences window.</p>"));
	lDefaultPublishStatus->setWhatsThis(
	    tr("<p>Set this to Draft or Publish if you always want "
	       "posts based on this template to have that status.</p>"
	       "<p>Otherwise, the post will have the status you specify in "
	       "the Preferences window.</p>"));

	_templateTitles = titles;
	_templateStrings = templates;
	_defaultPublishStates = pubStates;
	_copyTitleStates = ctStates;
	_assocHostLists = ahLists;
	_assocHostStrings = compileAssocHostStrings(_assocHostLists);

	originalTitles = titles;
	originalStrings = templates;
	originalPublishStates = pubStates;
	originalCopyTitleStates = ctStates;
	originalAssocHostLists = ahLists;

	hboxLayout->setStretchFactor(hboxLayout->itemAt(0)->layout(), 2);
	hboxLayout->setStretchFactor(hboxLayout->itemAt(1)->layout(), 3);
	Q_FOREACH (QString a, _templateTitles) {
		if (a.isEmpty())
			lwTemplates->addItem(tr("(No name)"));
		else
			lwTemplates->addItem(a);
	}
	// qDebug( "added the templates" );

	pbReset->setVisible(false);

	leName->setEnabled(false);
	teTemplateText->setEnabled(false);
	teAssocHosts->setEnabled(false);
	cbDefaultPublishStatus->setEnabled(false);
	chCopyTitle->setEnabled(false);

	doingNewTemplate = false;
	// setClean();
	currentRow = -1;
	defaultCPStatus = status;

	addTemplate = new QAction(tr("&Add template"), lwTemplates);
	removeTemplate = new QAction(tr("&Remove this template"), lwTemplates);
	lwTemplates->addAction(addTemplate);
	lwTemplates->addAction(removeTemplate);
	lwTemplates->setContextMenuPolicy(Qt::ActionsContextMenu);

	insertTabIntoTemplate = new QAction(teTemplateText);
//	insertTabIntoTemplate->setShortcut(tr("Ctrl+T"));
	insertTabIntoTemplate->setShortcutContext(Qt::WidgetShortcut);
	teTemplateText->addAction(insertTabIntoTemplate);

	connect(addTemplate, SIGNAL(triggered(bool)), this, SLOT(doNewTemplate()));
	connect(removeTemplate, SIGNAL(triggered(bool)), this, SLOT(removeThisTemplate()));
	connect(lwTemplates, SIGNAL(currentRowChanged(int)), this, SLOT(changeListIndex(int)));
	connect(insertTabIntoTemplate, SIGNAL(triggered(bool)), this, SLOT(doInsertTabIntoTemplate()));
}

//// mark
// void QuickpostTemplateDialog::on_lwTemplates_clicked(const QModelIndex &ind){
// qDebug("lwTemplates clicked");

// int row = ind.row();
// if(row != currentRow){
// if(dirty){
// qDebug("status is dirty");
// int ret = QMessageBox::warning(0, tr("QTM")
// , tr("The template has been modified.\n"
// "Do you want to save your changes?")
// , tr("&Save"), tr("&Discard")
// , tr("&Cancel"), 0, 2);
// switch(ret){
// case 2:
// return; // Click will have no effect
// case 0:
// acceptTemplate();
// default:
// lwTemplates->setCurrentRow(row);
// changeListIndex(row);
// }
// }else{
// lwTemplates->setCurrentRow(row);
// changeListIndex(row);
// }
// }else qDebug("row is current row");
// }

void QuickpostTemplateDialog::changeListIndex(int index) {
	if (doingNewTemplate) {
		if (dirty)
			lwTemplates->item((lwTemplates->count()) - 1)
			    ->setText(leName->text().isEmpty() ? tr("(No name)") : leName->text());
		else {
			_templateTitles.removeAt(currentRow);
			_templateStrings.removeAt(currentRow);
			_defaultPublishStates.removeAt(currentRow);
			_assocHostStrings.removeAt(currentRow);
			_copyTitleStates.removeAt(currentRow);
			lwTemplates->takeItem(lwTemplates->count() - 1);
		}
	}
	doingNewTemplate = false;
	currentRow = index;

	currentTemplateTitle = _templateTitles[currentRow];
	currentTemplateString = _templateStrings[currentRow];

	leName->setText(_templateTitles[currentRow]);
	teTemplateText->setPlainText(_templateStrings[currentRow]);
	cbDefaultPublishStatus->setCurrentIndex(_defaultPublishStates[currentRow]);
	chCopyTitle->setCheckState(_copyTitleStates[currentRow] ? Qt::Checked : Qt::Unchecked);
	teAssocHosts->setPlainText(_assocHostStrings[currentRow]);
	leName->setEnabled(true);
	teTemplateText->setEnabled(true);
	cbDefaultPublishStatus->setEnabled(true);
	chCopyTitle->setEnabled(true);
	teAssocHosts->setEnabled(true);
	// setClean();
}

void QuickpostTemplateDialog::doNewTemplate() {
	leName->setEnabled(true);
	teTemplateText->setEnabled(true);
	cbDefaultPublishStatus->setCurrentIndex(2);
	if (cbDefaultPublishStatus->count() == 4)   // if it's been disabled and blank
		cbDefaultPublishStatus->removeItem(3);  // remove the blank item
	cbDefaultPublishStatus->setEnabled(true);
	chCopyTitle->setEnabled(true);
	teAssocHosts->setEnabled(true);
	// pbAccept->setEnabled( true );

	_templateTitles.append(QString());
	_templateStrings.append(QString());
	_defaultPublishStates.append(int(2));
	_assocHostStrings.append(QString());
	_copyTitleStates.append(bool(defaultCPStatus));

	lwTemplates->disconnect(SIGNAL(currentRowChanged(int)), this, SLOT(changeListIndex(int)));
	currentRow = -1;
	leName->clear();
	teTemplateText->clear();
	teAssocHosts->clear();
	chCopyTitle->setChecked(defaultCPStatus);

	lwTemplates->addItem(tr("New template"));
	currentRow = lwTemplates->count() - 1;
	lwTemplates->setCurrentRow(lwTemplates->count() - 1);
	connect(lwTemplates, SIGNAL(currentRowChanged(int)), this, SLOT(changeListIndex(int)));

	doingNewTemplate = true;
	leName->setFocus(Qt::OtherFocusReason);

	setClean();
}

void QuickpostTemplateDialog::removeThisTemplate() {
	int c = lwTemplates->currentRow();
	// lwTemplates->takeItem( c );
	if (lwTemplates->count() == 1) {
		lwTemplates->disconnect(SIGNAL(currentRowChanged(int)));
		lwTemplates->clear();
		_templateTitles.clear();
		_templateStrings.clear();
		_defaultPublishStates.clear();
		_copyTitleStates.clear();
		_assocHostStrings.clear();
		leName->clear();
		leName->setEnabled(false);
		teTemplateText->clear();
		teTemplateText->setEnabled(false);
		cbDefaultPublishStatus->addItem("");
		cbDefaultPublishStatus->setCurrentIndex(3);
		cbDefaultPublishStatus->setEnabled(false);
		teAssocHosts->clear();
		teAssocHosts->setEnabled(false);
		chCopyTitle->setEnabled(false);
		// pbAccept->setEnabled( false );
		currentRow = -1;
	} else {
		lwTemplates->takeItem(c);
		_templateTitles.removeAt(c);
		_templateStrings.removeAt(c);
		_defaultPublishStates.removeAt(c);
		_copyTitleStates.removeAt(c);
		_assocHostStrings.removeAt(c);
	}
	// setClean();
	// qDebug( "exiting" );
	/*if( originalTitles.size() < lowestNumber )
            lowestNumber = titles.size(); */
}

void QuickpostTemplateDialog::doInsertTabIntoTemplate() {
	teTemplateText->insertPlainText("\t");
}

void QuickpostTemplateDialog::setDirty() {
	dirty = true;
	disconnect(leName, 0, this, SLOT(setDirty()));
	disconnect(teTemplateText, 0, this, SLOT(setDirty()));
	disconnect(teAssocHosts, 0, this, SLOT(setDirty()));
	disconnect(cbDefaultPublishStatus, 0, this, SLOT(setDirty()));
	disconnect(chCopyTitle, 0, this, SLOT(setDirty()));
	// disconnect( this, SLOT( setDirty() ) );
}

void QuickpostTemplateDialog::setClean() {
	// qDebug( "clean" );
	dirty = false;

	// disconnect( this, SLOT( setDirty() ) ); // to prevent duplicates
	disconnect(leName, 0, this, SLOT(setDirty()));
	disconnect(teTemplateText, 0, this, SLOT(setDirty()));
	disconnect(teAssocHosts, 0, this, SLOT(setDirty()));
	disconnect(cbDefaultPublishStatus, 0, this, SLOT(setDirty()));
	disconnect(chCopyTitle, 0, this, SLOT(setDirty()));

	connect(leName, SIGNAL(textChanged(const QString &)), this, SLOT(setDirty()));
	connect(teTemplateText, SIGNAL(textChanged()), this, SLOT(setDirty()));
	connect(teAssocHosts, SIGNAL(textChanged()), this, SLOT(setDirty()));
	connect(cbDefaultPublishStatus, SIGNAL(currentIndexChanged(int)), this, SLOT(setDirty()));
	connect(chCopyTitle, SIGNAL(clicked(bool)), this, SLOT(setDirty()));
}

void QuickpostTemplateDialog::acceptTemplate() {
	if (doingNewTemplate) {
		_templateTitles.append(leName->text());
		_templateStrings.append(teTemplateText->toPlainText());
		_defaultPublishStates.append(cbDefaultPublishStatus->currentIndex());
		_copyTitleStates.append(chCopyTitle->isChecked());
		_assocHostStrings.append(teAssocHosts->toPlainText().trimmed());
		lwTemplates->item(lwTemplates->count() - 1)
		    ->setText(leName->text().isEmpty() ? tr("(No name)") : leName->text());
		if (lwTemplates->count() == 1)
			connect(lwTemplates, SIGNAL(currentRowChanged(int)), this, SLOT(changeListIndex(int)));
		/*   QListWidgetItem *newItem = new QListWidgetItem( leName->text() );
                    lwTemplates->addItem( newItem );
                    lwTemplates->setCurrentItem( newItem );*/
	} else {
		if (lwTemplates->count()) {
			lwTemplates->item(currentRow)
			    ->setText(leName->text().isEmpty() ? tr("(No name)") : leName->text());
			/*      _templateTitles[currentRow] = leName->text();
                              _templateStrings[currentRow] =
         teTemplateText->toPlainText();
                              _defaultPublishStates[currentRow] =
         cbDefaultPublishStatus->currentIndex();
                              _copyTitleStates[currentRow] =
         chCopyTitle->isChecked();
                              _assocHostLists[currentRow] =
         teAssocHosts->toPlainText().trimmed().split( QRegExp( "\\n+" ) ); */
		}
	}
	doingNewTemplate = false;
	setClean();
	// qDebug( "Finished accept routine" );
}

void QuickpostTemplateDialog::on_pbWhatsThis_clicked() {
	QWhatsThis::enterWhatsThisMode();
}

//// mark
// void QuickpostTemplateDialog::on_pbReset_clicked(){
// if(QMessageBox::question(0, tr("QTM")
// , tr("Are you sure you wish to lose all your changes?")
// , QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
// == QMessageBox::Yes){
// lwTemplates->disconnect(SIGNAL(currentRowChanged(int)));
// lwTemplates->clear();
// qDebug("cleared templates list widget");
// leName->setText(QString());
// qDebug("cleared name");
// teTemplateText->clear();

// qDebug("starting to rewrite lists");
// _templateTitles = originalTitles;
// qDebug("rewritten titles");
// _templateStrings		= originalStrings;
// _defaultPublishStates	= originalPublishStates;
// _copyTitleStates		= originalCopyTitleStates;
// _assocHostLists			= originalAssocHostLists;
// _assocHostStrings		= compileAssocHostStrings(_assocHostLists);
// qDebug("rewritten all lists");

// lwTemplates->addItems(_templateTitles);
//// qDebug( "added titles to list " );
// leName->clear();
// teTemplateText->clear();
// teAssocHosts->clear();
// chCopyTitle->setChecked(false);
// chCopyTitle->setEnabled(false);
// leName->setEnabled(false);
// teTemplateText->setEnabled(false);
// teAssocHosts->setEnabled(false);
// doingNewTemplate = false;
// connect(lwTemplates, SIGNAL(currentRowChanged(int))
// , this, SLOT(changeListIndex(int)));
// setClean();
// if(lwTemplates->count() == 0) currentRow = - 1;
// }
// }

void QuickpostTemplateDialog::on_pbOK_clicked() {
	// qDebug( "Accept clicked" );
	// on_pbAccept_clicked();
	if (doingNewTemplate && !dirty) {
		_templateTitles.removeAt(currentRow);
		_templateStrings.removeAt(currentRow);
		_defaultPublishStates.removeAt(currentRow);
		_assocHostStrings.removeAt(currentRow);
		_copyTitleStates.removeAt(currentRow);
	}
	// We need to get rid of any empty or whitespace-only associated hosts entries
	QRegExp emptyString("^\\s+$");
	int i, j;
	_assocHostLists = compileAssocHostLists(_assocHostStrings);
	if (_assocHostLists.count()) {
		for (i = 0; i < _assocHostLists.count(); i++) {
			for (j = 0; j < _assocHostLists[i].count(); j++)
				if (_assocHostLists[i].at(j).isEmpty() ||
				    emptyString.exactMatch(_assocHostLists[i].at(j)))
					_assocHostLists[i].removeAt(j);
		}
	}
	accept();
}

void QuickpostTemplateDialog::on_leName_textEdited(const QString &newName) {
	if (currentRow != -1) {
		// qDebug( "title edited" );
		lwTemplates->item(currentRow)
		    ->setText(newName.isEmpty() ? tr("(No name)") : newName);
		_templateTitles[currentRow] = newName;
	}
}

void QuickpostTemplateDialog::on_teTemplateText_textChanged() {
	if (currentRow != -1)
		_templateStrings[currentRow] = teTemplateText->toPlainText();
}

void QuickpostTemplateDialog::on_teAssocHosts_textChanged() {
	if (currentRow != -1)
		_assocHostStrings[currentRow] = teAssocHosts->toPlainText();
}

void QuickpostTemplateDialog::on_cbDefaultPublishStatus_currentIndexChanged(
    int newState) {
	if (currentRow != -1)
		_defaultPublishStates[currentRow] = newState;
}

void QuickpostTemplateDialog::on_chCopyTitle_stateChanged(int) {
	if (currentRow != -1)
		_copyTitleStates[currentRow] = chCopyTitle->isChecked();
}

QList<QStringList> QuickpostTemplateDialog::assocHostLists() {
	int i, j;
	QRegExp emptyString("^\\s+$");

	_assocHostLists = compileAssocHostLists(_assocHostStrings);
	if (_assocHostLists.count()) {
		for (i = 0; i < _assocHostLists.count(); i++) {
			for (j = 0; j < _assocHostLists[i].count(); j++)
				if (_assocHostLists[i].at(j).isEmpty() ||
				    emptyString.exactMatch(_assocHostLists[i].at(j)))
					_assocHostLists[i].removeAt(j);
		}
	}
	return _assocHostLists;
}

QList<QStringList>
QuickpostTemplateDialog::compileAssocHostLists(QStringList &input) {
	QList<QStringList> rv;
	Q_FOREACH (QString a, input)
		rv.append(a.split("\n"));
	return rv;
}

QStringList
QuickpostTemplateDialog::compileAssocHostStrings(QList<QStringList> &input) {
	QStringList rv;
	Q_FOREACH (QStringList a, input)
		rv.append(a.join("\n"));
	return rv;
}
