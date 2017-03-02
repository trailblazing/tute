/*******************************************************************************

   QTM - Qt-based blog manager
   Copyright (C) 2006-2011 Matthew J Smith

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License (version 2), as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*******************************************************************************/

// StatusWidget.cc - Replacement status bar for QTM on the Mac

#include <QtCore>
#include <QtGui>

#include "status_widget.h"

StatusWidget::StatusWidget(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

StatusWidget::~StatusWidget()
{
    deleteLater();
}

void StatusWidget::showMessage(QString message, int time)
{
    QTimer* timer = new QTimer(this);
    if (time != 0)
        connect(timer, SIGNAL(timeout()), this, SLOT(clearMessage()));
    ui.lMessageSpace->setText(message);
    if (time != 0)
        timer->start();
}

void StatusWidget::clearMessage()
{
    ui.lMessageSpace->setText(QString());
}

QPushButton* StatusWidget::copyURLButton()
{
    return ui.pbCopyURLBase;
}

QProgressBar* StatusWidget::progressBar()
{
    return ui.progressBarBase;
}
