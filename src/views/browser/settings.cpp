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




#include "settings.h"

#include "libraries/qt_single_application5/qtsingleapplication.h"

#include "browser.h"
#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
#include "cookiejar.h"
#endif
#include "history.h"
#include "networkaccessmanager.h"
#include "webview.h"

#include <QtCore/QSettings>
#include <QtWidgets/QtWidgets>
#include <QtWebEngineWidgets/QtWebEngineWidgets>


namespace browser {
    class WebView;
    class TabWidget;

#if QT_VERSION == 0x050600
    W_OBJECT_IMPL(SettingsDialog)
#endif


    SettingsDialog::SettingsDialog(QWidget *parent)
	: QDialog(parent){
	setupUi(this);
	connect(setHomeToCurrentPageButton, &QPushButton::clicked, this, &SettingsDialog::setHomeToCurrentPage);
	connect(standardFontButton, &QPushButton::clicked, this, &SettingsDialog::chooseFont);
	connect(fixedFontButton, &QPushButton::clicked, this, &SettingsDialog::chooseFixedFont);

	loadDefaults();
	loadFromSettings();
    }

    void SettingsDialog::loadDefaults(){
	QWebEngineSettings	*defaultSettings	= QWebEngineSettings::globalSettings();
	QString			standardFontFamily	= defaultSettings->fontFamily(QWebEngineSettings::StandardFont);
	int			standardFontSize	= defaultSettings->fontSize(QWebEngineSettings::DefaultFontSize);
	standardFont = QFont(standardFontFamily, standardFontSize);
	standardLabel->setText(QString(QLatin1String("%1 %2")).arg(standardFont.family()).arg(standardFont.pointSize()));

	QString fixedFontFamily = defaultSettings->fontFamily(QWebEngineSettings::FixedFont);
	int	fixedFontSize	= defaultSettings->fontSize(QWebEngineSettings::DefaultFixedFontSize);
	fixedFont = QFont(fixedFontFamily, fixedFontSize);
	fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(fixedFont.family()).arg(fixedFont.pointSize()));

	downloadsLocation->setText(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));	// DesktopLocation

	enableJavascript->setChecked(defaultSettings->testAttribute(QWebEngineSettings::JavascriptEnabled));
	// #if defined(QTWEBENGINE_PLUGINS)
	enablePlugins->setChecked(defaultSettings->testAttribute(QWebEngineSettings::PluginsEnabled));
	// #endif

	enableScrollAnimator->setChecked(defaultSettings->testAttribute(QWebEngineSettings::ScrollAnimatorEnabled));
	auto profile = globalparameters.profile();
	persistentDataPath->setText(profile->persistentStoragePath());			// QWebEngineProfile::defaultProfile()
	sessionCookiesCombo->setCurrentIndex(profile->persistentCookiesPolicy());	// QWebEngineProfile::defaultProfile()
	httpUserAgent->setText(profile->httpUserAgent());				// QWebEngineProfile::defaultProfile()
    }

    void SettingsDialog::loadFromSettings(){
	QSettings settings(globalparameters.root_path() + "/" + globalparameters.target_os() + "/" + globalparameters._browser_conf_filename, QSettings::IniFormat);
	settings.beginGroup(QLatin1String("MainWindow"));
	const QString default_home_ = QLatin1String(Browser::_defaulthome);
	homeLineEdit->setText(settings.value(QLatin1String("home"), default_home_).toString());
	settings.endGroup();

	settings.beginGroup(QLatin1String("history"));
	int	historyExpire	= settings.value(QLatin1String("historyExpire")).toInt();
	int	idx		= 0;
	switch(historyExpire){
	    case 1: idx = 0;break;

	    case 7: idx = 1;break;

	    case 14: idx = 2;break;

	    case 30: idx = 3;break;

	    case 365: idx = 4;break;

	    case - 1: idx = 5;break;

	    default:
		idx = 5;
	}
	expireHistory->setCurrentIndex(idx);
	settings.endGroup();

	settings.beginGroup(QLatin1String("downloadmanager"));
	QString downloadDirectory = settings.value(QLatin1String("downloadDirectory"), downloadsLocation->text()).toString();
	downloadsLocation->setText(downloadDirectory);
	settings.endGroup();

	settings.beginGroup(QLatin1String("general"));
	openLinksIn->setCurrentIndex(settings.value(QLatin1String("openLinksIn"), openLinksIn->currentIndex()).toInt());

	settings.endGroup();

	// Appearance
	settings.beginGroup(QLatin1String("websettings"));
	fixedFont	= qvariant_cast<QFont>(settings.value(QLatin1String("fixedFont"), fixedFont));
	standardFont	= qvariant_cast<QFont>(settings.value(QLatin1String("standardFont"), standardFont));

	standardLabel->setText(QString(QLatin1String("%1 %2")).arg(standardFont.family()).arg(standardFont.pointSize()));
	fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(fixedFont.family()).arg(fixedFont.pointSize()));

	enableJavascript->setChecked(settings.value(QLatin1String("enableJavascript"), enableJavascript->isChecked()).toBool());
	enablePlugins->setChecked(settings.value(QLatin1String("enablePlugins"), enablePlugins->isChecked()).toBool());
	userStyleSheet->setPlainText(settings.value(QLatin1String("userStyleSheet")).toString());
	enableScrollAnimator->setChecked(settings.value(QLatin1String("enableScrollAnimator"), enableScrollAnimator->isChecked()).toBool());
	httpUserAgent->setText(settings.value(QLatin1String("httpUserAgent"), httpUserAgent->text()).toString());
	settings.endGroup();

	// Privacy
	settings.beginGroup(QLatin1String("cookies"));

	int persistentCookiesPolicy = settings.value(QLatin1String("persistentCookiesPolicy"), sessionCookiesCombo->currentIndex()).toInt();
	sessionCookiesCombo->setCurrentIndex(persistentCookiesPolicy);

	QString pdataPath = settings.value(QLatin1String("persistentDataPath"), persistentDataPath->text()).toString();
	persistentDataPath->setText(pdataPath);

	settings.endGroup();

	// Proxy
	settings.beginGroup(QLatin1String("proxy"));
	proxySupport->setChecked(settings.value(QLatin1String("enabled"), false).toBool());
	proxyType->setCurrentIndex(settings.value(QLatin1String("type"), 0).toInt());
	proxyHostName->setText(settings.value(QLatin1String("hostName")).toString());
	proxyPort->setValue(settings.value(QLatin1String("port"), 1080).toInt());
	proxyUserName->setText(settings.value(QLatin1String("userName")).toString());
	proxyPassword->setText(settings.value(QLatin1String("password")).toString());
	settings.endGroup();
    }

    void SettingsDialog::saveToSettings(){
	QSettings settings(globalparameters.root_path() + "/" + globalparameters.target_os() + "/" + globalparameters._browser_conf_filename, QSettings::IniFormat);
	settings.beginGroup(QLatin1String("MainWindow"));
	settings.setValue(QLatin1String("home"), homeLineEdit->text());
	settings.endGroup();

	settings.beginGroup(QLatin1String("general"));
	settings.setValue(QLatin1String("openLinksIn"), openLinksIn->currentIndex());
	settings.endGroup();


	settings.beginGroup(QLatin1String("downloadmanager"));
	settings.setValue(QLatin1String("downloadDirectory"), downloadsLocation->text());
	settings.endGroup();

	settings.beginGroup(QLatin1String("history"));
	int	historyExpire	= expireHistory->currentIndex();
	int	idx		= - 1;
	switch(historyExpire){
	    case 0: idx = 1;break;

	    case 1: idx = 7;break;

	    case 2: idx = 14;break;

	    case 3: idx = 30;break;

	    case 4: idx = 365;break;

	    case 5: idx = - 1;break;
	}
	settings.setValue(QLatin1String("historyExpire"), idx);
	settings.endGroup();

	// Appearance
	settings.beginGroup(QLatin1String("websettings"));
	settings.setValue(QLatin1String("fixedFont"), fixedFont);
	settings.setValue(QLatin1String("standardFont"), standardFont);
	settings.setValue(QLatin1String("enableJavascript"), enableJavascript->isChecked());
	settings.setValue(QLatin1String("enablePlugins"), enablePlugins->isChecked());
	settings.setValue(QLatin1String("enableScrollAnimator"), enableScrollAnimator->isChecked());
	settings.setValue(QLatin1String("userStyleSheet"), userStyleSheet->toPlainText());
	settings.setValue(QLatin1String("httpUserAgent"), httpUserAgent->text());
	settings.endGroup();

	// Privacy
	settings.beginGroup(QLatin1String("cookies"));

	int persistentCookiesPolicy = sessionCookiesCombo->currentIndex();
	settings.setValue(QLatin1String("persistentCookiesPolicy"), persistentCookiesPolicy);

	QString pdataPath = persistentDataPath->text();

	settings.setValue(QLatin1String("persistentDataPath"), pdataPath);

	settings.endGroup();


	// proxy
	settings.beginGroup(QLatin1String("proxy"));
	settings.setValue(QLatin1String("enabled"), proxySupport->isChecked());
	settings.setValue(QLatin1String("type"), proxyType->currentIndex());
	settings.setValue(QLatin1String("hostName"), proxyHostName->text());
	settings.setValue(QLatin1String("port"), proxyPort->text());
	settings.setValue(QLatin1String("userName"), proxyUserName->text());
	settings.setValue(QLatin1String("password"), proxyPassword->text());
	settings.endGroup();

	sapp_t::instance()->loadSettings();
#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
	QtSingleApplication::networkAccessManager()->loadSettings();
	QtSingleApplication::cookieJar()->loadSettings();
#endif
	sapp_t::historyManager()->loadSettings();
    }

    void SettingsDialog::accept(){
	QString pdataPath = persistentDataPath->text();
	if(QFile::exists(pdataPath)){
	    saveToSettings();
	    QDialog::accept();
	}else{
	    QMessageBox msgBox;
	    msgBox.setText(tr("The location can\'t be write."));
	    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	    msgBox.setDefaultButton(QMessageBox::Cancel);
//	    int ret =
	    msgBox.exec();
//	    if(ret != QMessageBox::Ok) return;
	    return;
	}
    }

    void SettingsDialog::showCookies(){
#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
	CookiesDialog *dialog = new CookiesDialog(QtSingleApplication::cookieJar(), this);
	dialog->exec();
#endif
    }

    void SettingsDialog::showExceptions(){
#if defined(QWEBENGINEPAGE_SETNETWORKACCESSMANAGER)
	CookiesExceptionsDialog *dialog = new CookiesExceptionsDialog(QtSingleApplication::cookieJar(), this);
	dialog->exec();
#endif
    }

    void SettingsDialog::chooseFont(){
	bool	ok;
	QFont	font = QFontDialog::getFont(&ok, standardFont, this);
	if(ok){
	    standardFont = font;
	    standardLabel->setText(QString(QLatin1String("%1 %2")).arg(font.family()).arg(font.pointSize()));
	}
    }

    void SettingsDialog::chooseFixedFont(){
	bool	ok;
	QFont	font = QFontDialog::getFont(&ok, fixedFont, this);
	if(ok){
	    fixedFont = font;
	    fixedLabel->setText(QString(QLatin1String("%1 %2")).arg(font.family()).arg(font.pointSize()));
	}
    }

    void SettingsDialog::setHomeToCurrentPage(){
	Browser *mw		= static_cast<Browser *>(parent());
	WebView *webView	= mw->currentTab();
	if(webView) homeLineEdit->setText(webView->page()->url().toString());
    }
}


