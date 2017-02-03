
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

// StatusWidget.h - Header file for replacement Mac status bar

#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include "ui_StatusWidgetBase.h"

class StatusWidget : public QWidget
{
Q_OBJECT

  public:
  StatusWidget( QWidget *parent = 0 );
  ~StatusWidget();
  QProgressBar *progressBar();
  QPushButton *copyURLButton();

  public slots:
  void showMessage( QString, int time = 0 );
  void clearMessage();

  private:
  Ui::StatusWidgetBase ui;
};

#endif

