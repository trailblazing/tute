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

#if QT_VERSION == 0x050600
#include <wobjectimpl.h>
#endif

#include "downloadmanager.h"

#include "autosaver.h"
#include "libraries/global_parameters.h"
#include "libraries/qt_single_application5/qtsingleapplication.h"
#include "networkaccessmanager.h"
#include "views/browser/tabwidget.h"
#include "views/main_window/hidable_tab.h"
#include <math.h>

#include <QtCore/QMetaEnum>
#include <QtCore/QSettings>

#include <QtGui/QDesktopServices>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileIconProvider>
#include <QtWidgets/QHeaderView>

#include <QtCore/QDebug>

#include <QWebEngineDownloadItem>
#include <QWebEngineSettings>

// extern std::string	url_difference(const std::string &url_compare_stored,
// const std::string &url_compare_get);
extern bool url_equal(const std::string& url_compare_stored, const std::string& url_compare_get);

namespace web {
/*!
            DownloadWidget is a widget that is displayed in the download manager
   list.
            It moves the data from the QWebEngineDownloadItem into the QFile as
   well
            as update the information/progressbar and report errors.
 */

#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(DownloadWidget)
#endif

	DownloadWidget::DownloadWidget(QWebEngineDownloadItem* download_, TabWidget* tab_manager_, DownloadManager* parent_)
	    : QWidget(parent_)
	    , std::enable_shared_from_this<DownloadWidget>()
	    , _bytesreceived(0)
	    , _download(download_)
	    , _tab_manager(tab_manager_)
	{
		setupUi(this);
		QPalette p = downloadInfoLabel->palette();
		p.setColor(QPalette::Text, Qt::darkGray);
		downloadInfoLabel->setPalette(p);
		progressBar->setMaximum(0);
		connect(stopButton, &FlatToolButton::clicked, this, &DownloadWidget::stop);
		connect(openButton, &FlatToolButton::clicked, this, &DownloadWidget::open);
		if (_download) {
			assert(_download->state() == QWebEngineDownloadItem::DownloadRequested || _download->state() == QWebEngineDownloadItem::DownloadInProgress);
			if (_download->state() == QWebEngineDownloadItem::DownloadRequested ||
			    _download->state() == QWebEngineDownloadItem::DownloadInProgress) {
				QSettings settings(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_browser_conf_filename, QSettings::IniFormat);
				settings.beginGroup(QLatin1String("downloadmanager"));
				QString download_directory =
				    settings.value(QLatin1String("downloadDirectory")).toString();
				settings.endGroup();

				auto default_full_path = _download->path();
				auto file_dir = default_full_path.mid(0, default_full_path.lastIndexOf('/')); //  + 1
				// auto	difference	= url_difference(file_path.toStdString(),
				// download_directory.toStdString());
				if (!url_equal(file_dir.toStdString(), download_directory.toStdString())) { // if(! (difference ==
													    // "" || difference ==
													    // "/")){
					auto file_name = default_full_path.mid(default_full_path.lastIndexOf('/') + 1);
					//					*file_name.begin() != '/' ? file_name.prepend('/') : "";
					*download_directory.rbegin() != '/' ? "" : download_directory.remove(download_directory.size() - 1, 1); // *download_directory.rbegin() !=
																		// '/' ? download_directory += '/' :
																		// "";
					auto new_path = download_directory + "/" + file_name;
					_download->setPath(new_path);
					assert(_download->path() == new_path);
				}
				_file.setFile(_download->path());
				_url = _download->url();
			}
		}
		init();
	}

	void DownloadWidget::init()
	{
		if (_download) {
			if (_download->state() == QWebEngineDownloadItem::DownloadRequested) {
				connect(_download, &QWebEngineDownloadItem::downloadProgress, this,
				    &DownloadWidget::downloadProgress); // _download.get() //
									// _download.data()
				connect(_download, &QWebEngineDownloadItem::finished, this,
				    &DownloadWidget::finished); // _download.get() // _download.data()
				// }
				// if(_download->state() == QWebEngineDownloadItem::DownloadRequested ){
				// reset info
				downloadInfoLabel->clear();
				progressBar->setValue(0);

				getFileName(static_cast<DownloadManager* const>(this->parent())->_prompt_store_position);

				_download->accept();
				// start timer for the download estimation
				_downloadtime.start();
				// }
			}
			// else if(_download->state() == QWebEngineDownloadItem::DownloadInProgress)
			// {
			// getFileName(static_cast<DownloadManager
			// *const>(this->parent())->_prompt_store_position);
			// _download->accept();
			// _downloadtime.start();
			// }
		}
	}

	bool DownloadWidget::getFileName(bool prompt_for_filename)
	{
		(void)prompt_for_filename;
		QSettings settings(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_browser_conf_filename, QSettings::IniFormat);
		settings.beginGroup(QLatin1String("downloadmanager"));
		QString defaultLocation =
		    QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
		//
		if (_file.absoluteDir().exists())
			defaultLocation = _file.absolutePath();
		//
		QString download_directory =
		    settings.value(QLatin1String("downloadDirectory"), defaultLocation)
			.toString();
		QString _default_filename =
		    QFileInfo(download_directory, _file.fileName()).exists() ? QFileInfo(download_directory, _file.fileName()).absoluteFilePath() : "";
		QString _new_filename = _default_filename;
		// if(! download_directory.isEmpty())download_directory += QLatin1Char('/');
		// else{
		// {
		// _default_filename	= QFileInfo(defaultLocation,
		// _file.fileName()).absoluteFilePath();
		// _new_filename	= QFileDialog::getSaveFileName(this, tr("Save File"),
		// _default_filename);
		// }
		// if(prompt_for_filename)
		if (_default_filename.isEmpty()) {
			_default_filename =
			    QFileInfo(defaultLocation, _file.fileName()).absoluteFilePath();
#if QT_VERSION == 0x050800
			if (_new_filename.isEmpty()) _new_filename = QFileInfo(download_directory, _file.fileName()).absoluteFilePath();
#else
			if (_new_filename.isEmpty()) _new_filename = QFileDialog::getSaveFileName(this, tr("Save File"), _default_filename); // sucks on Qt5.8?
#endif // QT_VERSION == 0x050800
		}

		if (_new_filename.isEmpty()) {
			if (_download)
				_download->cancel();
			fileNameLabel->setText(tr("Download canceled: %1").arg(QFileInfo(_default_filename).fileName()));

			return false;
		} else {
			// }
			//
			_file.setFile(_new_filename);

			auto download_state = _download->state();
			assert(download_state == QWebEngineDownloadItem::DownloadRequested || download_state == QWebEngineDownloadItem::DownloadInProgress);
			if (_download &&
			    (download_state == QWebEngineDownloadItem::DownloadRequested ||
				download_state == QWebEngineDownloadItem::DownloadInProgress))
				_download->setPath(_file.absoluteFilePath());
			fileNameLabel->setText(_file.fileName());
			auto _new_path =
			    QFileInfo(_new_filename)
				.absolutePath(); // _new_filename.truncate(_new_filename.lastIndexOf('/'));

			settings.setValue(QLatin1String("downloadDirectory"), _new_path);
		}
		settings.endGroup();

		return true;
	}

	TabWidget* DownloadWidget::tab_manager() const
	{
		return _tab_manager;
	}

	void DownloadWidget::stop()
	{
		setUpdatesEnabled(false);
		stopButton->setEnabled(false);
		stopButton->hide();
		setUpdatesEnabled(true);
		if (_download) {
			_download->cancel();
			fileNameLabel->setText(QString("Download canceled: ") + _download->path());
		}
		emit statusChanged();
	}

	void DownloadWidget::open()
	{
		QUrl url = QUrl::fromLocalFile(_file.absoluteFilePath());
		QDesktopServices::openUrl(url);
	}

	void DownloadWidget::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
	{
		_bytesreceived = bytesReceived;
		if (bytesTotal == -1) {
			progressBar->setValue(0);
			progressBar->setMaximum(0);
		} else {
			progressBar->setValue(bytesReceived);
			progressBar->setMaximum(bytesTotal);
		}
		updateInfoLabel();
	}

	void DownloadWidget::updateInfoLabel()
	{
		qint64 bytesTotal = progressBar->maximum();

		// update info label
		double speed = _bytesreceived * 1000.0 / _downloadtime.elapsed();
		double timeRemaining = ((double)(bytesTotal - _bytesreceived)) / speed;
		QString timeRemainingString = tr("seconds");
		if (timeRemaining > 60) {
			timeRemaining = timeRemaining / 60;
			timeRemainingString = tr("minutes");
		}
		timeRemaining = floor(timeRemaining);
		// When downloading the eta should never be 0
		if (timeRemaining == 0)
			timeRemaining = 1;
		QString info;
		if (!downloadedSuccessfully()) {
			QString remaining;
			if (bytesTotal != 0)
				remaining =
				    tr("- %4 %5 remaining").arg(timeRemaining).arg(timeRemainingString);
			info = tr("%1 of %2 (%3/sec) %4")
				   .arg(dataString(_bytesreceived))
				   .arg(bytesTotal == 0 ? tr("?") : dataString(bytesTotal))
				   .arg(dataString((int)speed))
				   .arg(remaining);
		} else {
			if (_bytesreceived != bytesTotal) {
				info = tr("%1 of %2 - Stopped")
					   .arg(dataString(_bytesreceived))
					   .arg(dataString(bytesTotal));
			} else
				info = dataString(_bytesreceived);
		}
		downloadInfoLabel->setText(info);
	}

	QString DownloadWidget::dataString(int size) const
	{
		QString unit;
		if (size < 1024)
			unit = tr("bytes");
		else if (size < 1024 * 1024) {
			size /= 1024;
			unit = tr("kB");
		} else {
			size /= 1024 * 1024;
			unit = tr("MB");
		}
		return QString(QLatin1String("%1 %2")).arg(size).arg(unit);
	}

	bool DownloadWidget::downloading() const
	{
		return progressBar->isVisible();
	}

	bool DownloadWidget::downloadedSuccessfully() const
	{
		bool completed =
		    _download && _download->isFinished() &&
		    _download->state() == QWebEngineDownloadItem::DownloadCompleted;

		return completed || !stopButton->isVisible();
	}

	void DownloadWidget::finished()
	{
		if (_download) {
			QWebEngineDownloadItem::DownloadState state = _download->state();
			QString message;
			bool interrupted = false;
			switch (state) {
			case QWebEngineDownloadItem::DownloadRequested: // Fall-through.
			case QWebEngineDownloadItem::DownloadInProgress:
				Q_UNREACHABLE();
				break;

			case QWebEngineDownloadItem::DownloadCompleted:
				// _tab_manager->current_download_acceptance({_download->url(), false});
				break;

			case QWebEngineDownloadItem::DownloadCancelled:
				message = QStringLiteral("Download cancelled");
				interrupted = true;
				// _tab_manager->current_download_acceptance({_download->url(), false});
				break;

			case QWebEngineDownloadItem::DownloadInterrupted:
				message = QStringLiteral("Download interrupted");
				interrupted = true;
				// _tab_manager->current_download_acceptance({_download->url(), false});
				break;
			}
			if (interrupted) {
				downloadInfoLabel->setText(message);

				return;
			}
		}
		progressBar->hide();
		stopButton->setEnabled(false);
		stopButton->hide();
		updateInfoLabel();
		emit statusChanged();
	}

	QWebEngineDownloadItem* DownloadWidget::download() const { return _download; }
#if QT_VERSION == 0x050600

	W_OBJECT_IMPL(DownloadManager)
#endif

	/*!
            DownloadManager is a Dialog that contains a list of DownloadWidgets

            It is a basic download manager.  It only downloads the file, doesn't
   do BitTorrent,
            extract zipped files or anything fancy.
 */
	DownloadManager::DownloadManager(QString object_name, HidableTab* vtab_record_)
	    : QDialog(vtab_record_)
	    , _vtab_record(vtab_record_)
	    , _autosaver(new AutoSaver(this))
	    , _model(new DownloadModel(this))
	    , _iconprovider(0)
	    , _removepolicy(Never)
	{
		setObjectName(object_name);
		setStyleSheet("border : 0px;");
		setupUi(this);
		downloadsView->setShowGrid(false);
		downloadsView->verticalHeader()->hide();
		downloadsView->horizontalHeader()->hide();
		downloadsView->setAlternatingRowColors(true);
		downloadsView->horizontalHeader()->setStretchLastSection(true);
		// _model = new DownloadModel(this);
		downloadsView->setModel(_model);
		connect(cleanupButton, &FlatToolButton::clicked, this, &DownloadManager::cleanup);
		if (_vtab_record->indexOf(this) == -1)
			_vtab_record->addTab(static_cast<QWidget*>(this), QIcon(":/resource/pic/apple.svg"), "Download");
		if (gl_paras->download_manager() != this)
			gl_paras->download_manager(this);
		setAttribute(Qt::WA_DeleteOnClose, true);
		load();
	}

	DownloadManager::~DownloadManager()
	{

		// delete
		//		_model->deleteLater();
		_autosaver->changeOccurred();
		_autosaver->saveIfNeccessary();
		if (_iconprovider)
			delete _iconprovider;
		auto index_ = _vtab_record->indexOf(this);
		if (index_ != -1) {
			_vtab_record->removeTab(index_);
			// if(_vtab_record->tabText(index_) == download_manager_singleton_name)
		}
		if (gl_paras->download_manager() == this)
			gl_paras->download_manager(nullptr);
	}

	int DownloadManager::activeDownloads() const
	{
		int count = 0;
		for (int i = 0; i < _downloads.count(); ++i)
			if (_downloads.at(i)->stopButton->isEnabled())
				++count;
		return count;
	}

	void DownloadManager::download(TabWidget* _tab_manager, QWebEngineDownloadItem* download)
	{
		bool found = false;
		for (auto dl : _downloads) {
			if (dl->download() == download) found = true;
		}
		if (!found) {
			assert(download->state() == QWebEngineDownloadItem::DownloadRequested || download->state() == QWebEngineDownloadItem::DownloadInProgress);
			if (download->state() == QWebEngineDownloadItem::DownloadRequested) {
				QSettings settings(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_browser_conf_filename, QSettings::IniFormat);
				settings.beginGroup(QLatin1String("downloadmanager"));
				QString download_directory =
				    settings.value(QLatin1String("downloadDirectory")).toString();
				settings.endGroup();

				auto default_full_path = download->path();
				auto file_dir = default_full_path.mid(0, default_full_path.lastIndexOf('/')); //+ 1
				// auto	difference	= url_difference(file_path.toStdString(),
				// download_directory.toStdString());
				if (!url_equal(file_dir.toStdString(), download_directory.toStdString())) { // if(! (difference ==
													    // "" || difference ==
													    // "/")){
					auto file_name = default_full_path.mid(default_full_path.lastIndexOf('/') + 1);
					//					*file_name.begin() != '/' ? file_name.prepend('/') : "";
					*download_directory.rbegin() != '/' ? "" : download_directory.remove(download_directory.size() - 1, 1); // *download_directory.rbegin() != '/'
																		// ? download_directory += '/' : "";
					auto new_path = download_directory + "/" + file_name;
					download->setPath(new_path);
					assert(download->path() == new_path);
				}
				auto widget =
				    std::make_shared<DownloadWidget>(download, _tab_manager, this);
				addItem(widget);
			}
		}
	}

	void DownloadManager::addItem(std::shared_ptr<DownloadWidget> downloadwidget)
	{
		connect(downloadwidget.get(), &DownloadWidget::statusChanged, this, &DownloadManager::updateRow);
		int row = _downloads.count();
		_model->beginInsertRows(QModelIndex(), row, row);
		_downloads.append(downloadwidget);
		_model->endInsertRows();
		updateItemCount();
		if (row == 0)
			show();
		downloadsView->setIndexWidget(_model->index(row, 0), downloadwidget.get());
		QIcon icon = style()->standardIcon(QStyle::SP_FileIcon);
		downloadwidget->fileIcon->setPixmap(icon.pixmap(48, 48));
		downloadsView->setRowHeight(row, downloadwidget->sizeHint().height());
	}

	void DownloadManager::updateRow()
	{
		DownloadWidget* widget = qobject_cast<DownloadWidget*>(sender());
		int row = _downloads.indexOf(widget->shared_from_this());
		if (-1 == row)
			return;
		if (!_iconprovider)
			_iconprovider = new QFileIconProvider();
		QIcon icon = _iconprovider->icon(widget->_file);
		if (icon.isNull())
			icon = style()->standardIcon(QStyle::SP_FileIcon);
		widget->fileIcon->setPixmap(icon.pixmap(48, 48));
		downloadsView->setRowHeight(row, widget->minimumSizeHint().height());

		bool remove = false;
		if (!widget->downloading() && sapp_t::instance()->privateBrowsing())
			remove = true;
		if (widget->downloadedSuccessfully() &&
		    removePolicy() == DownloadManager::SuccessFullDownload)
			remove = true;
		if (remove)
			_model->removeRow(row);
		cleanupButton->setEnabled(_downloads.count() - activeDownloads() > 0);
	}

	DownloadManager::RemovePolicy DownloadManager::removePolicy() const
	{
		return _removepolicy;
	}

	void DownloadManager::setRemovePolicy(RemovePolicy policy)
	{
		if (policy == _removepolicy)
			return;
		_removepolicy = policy;
		_autosaver->changeOccurred();
	}

	void DownloadManager::save() const
	{
		QSettings settings(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_browser_conf_filename, QSettings::IniFormat);
		settings.beginGroup(QLatin1String("downloadmanager"));
		QMetaEnum removePolicyEnum = staticMetaObject.enumerator(
		    staticMetaObject.indexOfEnumerator("RemovePolicy"));
		settings.setValue(QLatin1String("removeDownloadsPolicy"), QLatin1String(removePolicyEnum.valueToKey(_removepolicy)));
		settings.setValue(QLatin1String("size"), size());
		if (_removepolicy == Exit)
			return;
		for (int i = 0; i < _downloads.count(); ++i) {
			QString key = QString(QLatin1String("download_%1_")).arg(i);
			settings.setValue(key + QLatin1String("url"), _downloads[i]->_url);
			settings.setValue(key + QLatin1String("location"), _downloads[i]->_file.filePath());
			settings.setValue(key + QLatin1String("done"), _downloads[i]->downloadedSuccessfully());
		}
		int i = _downloads.count();
		QString key = QString(QLatin1String("download_%1_")).arg(i);
		while (settings.contains(key + QLatin1String("url"))) {
			settings.remove(key + QLatin1String("url"));
			settings.remove(key + QLatin1String("location"));
			settings.remove(key + QLatin1String("done"));
			key = QString(QLatin1String("download_%1_")).arg(++i);
		}
		settings.setValue(
		    QLatin1String("prompt_store_position"),
		    QLatin1String((QString::number(_prompt_store_position)).toLatin1()));
		settings.endGroup();
	}

	void DownloadManager::load()
	{
		QSettings settings(gl_paras->root_path() + "/" + gl_paras->target_os() + "/" + gl_para::_browser_conf_filename, QSettings::IniFormat);
		settings.beginGroup(QLatin1String("downloadmanager"));
		QSize size = settings.value(QLatin1String("size")).toSize();
		if (size.isValid())
			resize(size);
		QByteArray value =
		    settings
			.value(QLatin1String("removeDownloadsPolicy"), QLatin1String("Never"))
			.toByteArray();
		QMetaEnum removePolicyEnum = staticMetaObject.enumerator(
		    staticMetaObject.indexOfEnumerator("RemovePolicy"));
		_removepolicy =
		    removePolicyEnum.keyToValue(value) == -1 ? Never : static_cast<RemovePolicy>(removePolicyEnum.keyToValue(value));

		int i = 0;
		QString key = QString(QLatin1String("download_%1_")).arg(i);
		while (settings.contains(key + QLatin1String("url"))) {
			QUrl url = settings.value(key + QLatin1String("url")).toUrl();
			QString fileName =
			    settings.value(key + QLatin1String("location")).toString();
			bool done = settings.value(key + QLatin1String("done"), true).toBool();
			if (done && !url.isEmpty() && !fileName.isEmpty()) {
				std::shared_ptr<DownloadWidget> widget =
				    std::make_shared<DownloadWidget>(nullptr, nullptr, this);
				widget->_file.setFile(fileName);
				widget->fileNameLabel->setText(widget->_file.fileName());
				widget->_url = url;
				widget->stopButton->setVisible(false);
				widget->stopButton->setEnabled(false);
				widget->progressBar->hide();
				addItem(widget);
			}
			key = QString(QLatin1String("download_%1_")).arg(++i);
		}
		cleanupButton->setEnabled(_downloads.count() - activeDownloads() > 0);
		_prompt_store_position =
		    settings.value(QLatin1String("prompt_store_position")).toBool();
		settings.endGroup();
	}

	void DownloadManager::cleanup()
	{
		if (_downloads.isEmpty())
			return;
		// for(auto &download : _downloads){
		// download->tab_manager()->current_download_acceptance({QUrl(), false});
		// }
		_model->removeRows(0, _downloads.count());
		updateItemCount();
		if (_downloads.isEmpty() && _iconprovider) {
			delete _iconprovider;
			_iconprovider = 0;
		}
		_autosaver->changeOccurred();
	}

	void DownloadManager::updateItemCount()
	{
		int count = _downloads.count();
		itemCount->setText(count == 1 ? tr("1 Download") : tr("%1 Downloads").arg(count));
	}

#if QT_VERSION == 0x050600
	W_OBJECT_IMPL(DownloadModel)
#endif

	DownloadModel::DownloadModel(DownloadManager* downloadManager, QObject* parent)
	    : QAbstractListModel(parent)
	    , _downloadmanager(downloadManager)
	{
	}

	QVariant DownloadModel::data(const QModelIndex& index, int role) const
	{
		if (index.row() < 0 || index.row() >= rowCount(index.parent()))
			return QVariant();
		if (role == Qt::ToolTipRole)
			if (!_downloadmanager->_downloads.at(index.row())->downloadedSuccessfully())
				return _downloadmanager->_downloads.at(index.row())
				    ->downloadInfoLabel->text();
		return QVariant();
	}

	int DownloadModel::rowCount(const QModelIndex& parent) const
	{
		return (parent.isValid()) ? 0 : _downloadmanager->_downloads.count();
	}

	bool DownloadModel::removeRows(int row, int count, const QModelIndex& parent)
	{
		if (parent.isValid())
			return false;
		int lastRow = row + count - 1;
		for (int i = lastRow; i >= row; --i) {
			if (_downloadmanager->_downloads.at(i)->downloadedSuccessfully()) {
				beginRemoveRows(parent, i, i);
				_downloadmanager->_downloads.takeAt(i)->deleteLater();
				endRemoveRows();
			}
		}
		_downloadmanager->_autosaver->changeOccurred();

		return true;
	}
}
