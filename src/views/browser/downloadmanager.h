/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <memory>

#include "ui_downloaditem.h"
#include "ui_downloads.h"

#include <QtCore/QFileInfo>
#include <QtCore/QTime>
#include <QtCore/QUrl>

#include <QWebEngineDownloadItem>

#include "utility/lease.h"

#if QT_VERSION == 0x050600
#include <QObject>
#include <wobjectdefs.h>
#endif

QT_BEGIN_NAMESPACE
class QFileIconProvider;
QT_END_NAMESPACE

// QT_BEGIN_NAMESPACE

class HidableTab;

namespace web {
	class TabWidget;
	class DownloadManager;
	class DownloadWidget : public QWidget,
			       public Ui_DownloadItem,
			       public std::enable_shared_from_this<DownloadWidget> {
#if QT_VERSION == 0x050600
		W_OBJECT(DownloadWidget)
#else
		Q_OBJECT
#endif
	    signals:

#if QT_VERSION == 0x050600
		void statusChanged() W_SIGNAL(statusChanged); //
#else
		void statusChanged();
#endif

	    public
		:
		DownloadWidget(QWebEngineDownloadItem* download_, TabWidget* tab_manager_,
		    DownloadManager* parent_ = 0 // QWidget *parent = 0
		    );
		bool downloading() const;
		bool downloadedSuccessfully() const;

		void init();
		bool getFileName(bool prompt_for_filename = true);
		TabWidget* tab_manager() const;
		QWebEngineDownloadItem* download() const;
	    private slots:
		void stop();
		void open();

		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void finished();

	    private:
		friend class DownloadManager;
		void updateInfoLabel();
		QString dataString(int size) const;

		QUrl _url;
		QFileInfo _file;
		qint64 _bytesreceived;
		QTime _downloadtime;
		bool _stopped;

		// QScopedPointer<QWebEngineDownloadItem>
		// QSharedPointer<QWebEngineDownloadItem>
		// std::shared_ptr<QWebEngineDownloadItem>
		QWebEngineDownloadItem* _download = nullptr;
		TabWidget* _tab_manager = nullptr;
	};

	class AutoSaver;
	class DownloadModel;
	// QT_BEGIN_NAMESPACE
	// class QFileIconProvider;
	// QT_END_NAMESPACE

	class DownloadManager : public QDialog, public Ui_download_dialog {
	    public:
		enum RemovePolicy { Never,
			Exit,
			SuccessFullDownload };

		RemovePolicy removePolicy() const;
		void setRemovePolicy(RemovePolicy policy);

	    private:
#if QT_VERSION == 0x050600
		W_OBJECT(DownloadManager)
		W_PROPERTY(RemovePolicy, removePolicy, &DownloadManager::removePolicy,
		    &DownloadManager::setRemovePolicy) //
#else
		Q_OBJECT Q_PROPERTY(RemovePolicy removePolicy READ removePolicy WRITE setRemovePolicy)
#endif
		Q_ENUMS(RemovePolicy)

	    public:
		DownloadManager(QString object_name, HidableTab* vtab_record_);
		~DownloadManager();
		int activeDownloads() const;

	    public slots:
		void download(TabWidget* _tab_manager, QWebEngineDownloadItem* download);
		void cleanup();

	    private slots:
		void save() const;
		void updateRow();

	    private:
		void addItem(std::shared_ptr<DownloadWidget> downloadwidget);
		void updateItemCount();
		void load();

		HidableTab* _vtab_record = nullptr;
		AutoSaver* _autosaver = nullptr;
		DownloadModel* _model = nullptr;
		QFileIconProvider* _iconprovider = nullptr;
		QList<std::shared_ptr<DownloadWidget>> _downloads;
		RemovePolicy _removepolicy;
		bool _prompt_store_position = true;

		friend class DownloadModel;
		friend class DownloadWidget;
	};

	class DownloadModel : public QAbstractListModel {
		friend class DownloadManager;
#if QT_VERSION == 0x050600
		W_OBJECT(DownloadModel)
#else
		Q_OBJECT
#endif

	    public:
		DownloadModel(DownloadManager* downloadManager, QObject* parent = 0);
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

	    private:
		DownloadManager* _downloadmanager = nullptr;
	};
}

// QT_END_NAMESPACE

#endif // DOWNLOADMANAGER_H
