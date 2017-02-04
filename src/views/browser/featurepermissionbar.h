/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWebEngine module of the Qt Toolkit.
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

#ifndef FEATUREPERMISSIONBAR_H
#define FEATUREPERMISSIONBAR_H



#include <QWidget>
#include <QWebEnginePage>


#if QT_VERSION == 0x050600
#include <wobjectdefs.h>
#include <QObject>
#endif

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
QT_END_NAMESPACE

//    QT_BEGIN_NAMESPACE


namespace browser {
    class FeaturePermissionBar : public QWidget {
#if QT_VERSION == 0x050600
	W_OBJECT(FeaturePermissionBar)
#else
	Q_OBJECT
#endif
	public:
	    FeaturePermissionBar(QWidget *);
	    void requestPermission(const QUrl &, QWebEnginePage::Feature feature);

	signals:
	    void featurePermissionProvided(const QUrl &securityOrigin, QWebEnginePage::Feature f, QWebEnginePage::PermissionPolicy p)
#if QT_VERSION == 0x050600
	    W_SIGNAL(featurePermissionProvided, (const QUrl &, QWebEnginePage::Feature, QWebEnginePage::PermissionPolicy), securityOrigin, f, p)	// ;
#else
	    ;
#endif
	private slots:
	    void	permissionDenied();
	    void	permissionGranted();
	    void	permissionUnknown();

	private:
	    QWebEnginePage::Feature	_feature;
	    QLabel			*_messagelabel;
	    QUrl			_securityorigin;
    };
}


// QT_END_NAMESPACE

#endif	// FEATUREPERMISSIONBAR_H



