/***************************************************************************
 * Application.h - Header for application subclass for QTM
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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QList>
#include <QStringList>

class SuperMenu;
class QMenuBar;
class Blogger;

class Application : public QApplication {
Q_OBJECT

public:
	Application(int &, char * *);
	void setupRecentFiles();
	QStringList recentFileTitles();
	QStringList recentFilenames();
	typedef struct _RF {
		QString title;
		QString filename;
	}recentFile;
	QList<recentFile> recentFiles();
	recentFile getRecentFile(int);
	QStringList titles();
	QStringList filenames();
	Blogger *currentEditingWindow(){return _currentEditingWindow;}
	void deleteSandbox();
	bool isSandbox(){return _isSandbox;}

public slots:
	void set_recent_files(const QStringList &, const QStringList &);
	void add_recent_file(const QString &, const QString &);
	void saveAll();
	void setMainWindow(Blogger *sm);

signals:
	void recent_files_updated(QStringList, QStringList);
	void recent_files_updated(const QList<Application::recentFile> &);
	void mainWindowChanged(Blogger *);

private:
	QList<recentFile> _recentFiles;
#ifdef Q_OS_MAC
	SuperMenu *superMenu;
#endif
	Blogger *_currentEditingWindow;
	bool _isSandbox;

private slots:
	void saveRecentFiles();
	void handleWindowChange(QWidget *, QWidget *);
	void handleLastWindowClosed();
};

#endif
