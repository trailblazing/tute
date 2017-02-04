/***************************************************************************
 * PrefsDialog.cc - Preferences dialog for QTM
 *
 * Copyright (C) 2008, Matthew J Smith
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


#include "PrefsDialog.h"

#include <QWidget>
#include <QDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QString>
#include <QHostInfo>
#include <QMessageBox>
#include <QWhatsThis>
#include <QLabel>

PrefsDialog::PrefsDialog(QWidget *parent)
    : QDialog(parent){
    setupUi(this);
	// nvh = false;

	// cbBlogType->setCurrentIndex( cbBlogType->count()-1 );

#ifdef Q_OS_MAC
    label_10->setVisible(false);
    cbSTI2ClickFunction->setVisible(false);
#endif
}

void PrefsDialog::on_pbBrowse_clicked(){
    QString dir =
	QFileDialog::getExistingDirectory(0
					 , tr("Choose a directory")
					 , "/home");
    if(! dir.isEmpty()) leLocalDir->setText(dir);
}

void PrefsDialog::on_pbWhatsThis_clicked(){	// slot
    QWhatsThis::enterWhatsThisMode();
}

// Slots for controlling editing & console text colours

void PrefsDialog::on_tbEditorBgColor_clicked(){	// slot
    QPalette	palette = lEditorExampleText->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::Window), this);
    if(color.isValid()){
	palette.setColor(QPalette::Window, color);
	lEditorExampleText->setPalette(palette);
    }
}

void PrefsDialog::on_tbEditorFgColor_clicked(){	// slot
    QPalette	palette = lEditorExampleText->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::WindowText), this);
    if(color.isValid()){
	palette.setColor(QPalette::WindowText, color);
	lEditorExampleText->setPalette(palette);
    }
}

void PrefsDialog::on_tbPreviewBgColor_clicked(){// slot
    QPalette	palette = lPreviewExampleText->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::Window), this);
    if(color.isValid()){
	palette.setColor(QPalette::Window, color);
	lPreviewExampleText->setPalette(palette);
    }
}

void PrefsDialog::on_tbPreviewFgColor_clicked(){// slot
    QPalette	palette = lPreviewExampleText->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::WindowText), this);
    if(color.isValid()){
	palette.setColor(QPalette::WindowText, color);
	lPreviewExampleText->setPalette(palette);
    }
}

void PrefsDialog::on_tbConsoleBgColor_clicked(){// slot
    QPalette	palette = lConsoleExampleText->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::Window), this);
    if(color.isValid()){
	palette.setColor(QPalette::Window, color);
	lConsoleExampleText->setPalette(palette);
    }
}

void PrefsDialog::on_tbConsoleFgColor_clicked(){// slot
    QPalette	palette = lConsoleExampleText->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::WindowText), this);
    if(color.isValid()){
	palette.setColor(QPalette::WindowText, color);
	lConsoleExampleText->setPalette(palette);
    }
}

// Slots for controlling syntax highlighting colours

void PrefsDialog::on_pbTagColorFore_clicked(){
    QPalette	palette = lTagColorDemo->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::WindowText), this);
    if(color.isValid()){
	palette.setColor(QPalette::WindowText, color);
	lTagColorDemo->setPalette(palette);
    }
}

void PrefsDialog::on_pbTagColorBack_clicked(){
    QPalette	palette = lTagColorDemo->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::Base), this);
    if(color.isValid()){
	palette.setColor(QPalette::Window, color);
	lTagColorDemo->setPalette(palette);
    }
}

void PrefsDialog::on_pbEntityColorFore_clicked(){
    QPalette	palette = lEntityColorDemo->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::WindowText), this);
    if(color.isValid()){
	palette.setColor(QPalette::WindowText, color);
	lEntityColorDemo->setPalette(palette);
    }
}

void PrefsDialog::on_pbEntityColorBack_clicked(){
    QPalette	palette = lEntityColorDemo->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::Window), this);
    if(color.isValid()){
	palette.setColor(QPalette::Window, color);
	lEntityColorDemo->setPalette(palette);
    }
}

void PrefsDialog::on_pbCommentColorFore_clicked(){
    QPalette	palette = lCommentColorDemo->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::WindowText), this);
    if(color.isValid()){
	palette.setColor(QPalette::WindowText, color);
	lCommentColorDemo->setPalette(palette);
    }
}

void PrefsDialog::on_pbCommentColorBack_clicked(){
    QPalette	palette = lCommentColorDemo->palette();
    QColor	color	= QColorDialog::getColor(palette.color(QPalette::Window), this);
    if(color.isValid()){
	palette.setColor(QPalette::Window, color);
	lCommentColorDemo->setPalette(palette);
    }
}

void PrefsDialog::on_chTagBold_toggled(bool isBold){
    setLabelText(lTagColorDemo, isBold, chTagItalic->isChecked());
}

void PrefsDialog::on_chTagItalic_toggled(bool isItalic){
    setLabelText(lTagColorDemo, chTagBold->isChecked(), isItalic);
}

void PrefsDialog::on_chEntityBold_toggled(bool isBold){
    setLabelText(lEntityColorDemo, isBold, chTagItalic->isChecked());
}

void PrefsDialog::on_chEntityItalic_toggled(bool isItalic){
    setLabelText(lEntityColorDemo, chEntityBold->isChecked(), isItalic);
}

void PrefsDialog::on_chCommentBold_toggled(bool isBold){
    setLabelText(lCommentColorDemo, isBold, chCommentItalic->isChecked());
}

void PrefsDialog::on_chCommentItalic_toggled(bool isItalic){
    setLabelText(lCommentColorDemo, chCommentBold->isChecked(), isItalic);
}

void PrefsDialog::setLabelText(QLabel *label, bool isBold, bool isItalic){
    label->setText(QString("%1%2Sample text%3%4")
	.arg(isBold ? "<strong>" : "")
	.arg(isItalic ? "<em>" : "")
	.arg(isItalic ? "</em>" : "")
	.arg(isBold ? "</strong>" : ""));
}
