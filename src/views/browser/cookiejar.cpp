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

#include "cookiejar.h"

#include "autosaver.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaEnum>
#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include <QtCore/QSortFilterProxyModel>
#include <QtGui/QDesktopServices>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtNetwork/QNetworkCookie>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QHeaderView>

#include <QWebEngineSettings>

#include <QtCore/QDebug>

static const unsigned int JAR_VERSION = 23;

QT_BEGIN_NAMESPACE

QDataStream& operator<<(QDataStream& stream, const QList<QNetworkCookie>& list)
{
    stream << JAR_VERSION;
    stream << quint32(list.size());
    for (int i = 0; i < list.size(); ++i)
        stream << list.at(i).toRawForm();
    return stream;
}

QDataStream& operator>>(QDataStream& stream, QList<QNetworkCookie>& list)
{
    list.clear();

    quint32 version;
    stream >> version;
    if (version != JAR_VERSION)
        return stream;
    quint32 count;
    stream >> count;
    for (quint32 i = 0; i < count; ++i) {
        QByteArray value;
        stream >> value;
        QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
        if (newCookies.count() == 0 && value.length() != 0)
            qWarning() << "CookieJar: Unable to parse saved cookie:" << value;
        for (int j = 0; j < newCookies.count(); ++j)
            list.append(newCookies.at(j));
        if (stream.atEnd())
            break;
    }
    return stream;
}

QT_END_NAMESPACE

namespace web {
CookieJar::CookieJar(QObject* parent)
    : QNetworkCookieJar(parent)
    , _loaded(false)
    , _save_timer(new AutoSaver(this))
    , _accept_cookies(AcceptOnlyFromSitesNavigatedTo)
{
}

CookieJar::~CookieJar()
{
    if (_keep_cookies == KeepUntilExit)
        clear();
    _save_timer->saveIfNeccessary();
}

void CookieJar::clear()
{
    setAllCookies(QList<QNetworkCookie>());
    _save_timer->changeOccurred();
    emit cookiesChanged();
}

void CookieJar::load()
{
    if (_loaded)
        return;
    // load cookies and exceptions
    qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >("QList<QNetworkCookie>");
    QSettings cookieSettings(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1String("/cookies.ini"), QSettings::IniFormat);
    setAllCookies(qvariant_cast<QList<QNetworkCookie> >(cookieSettings.value(QLatin1String("cookies"))));
    cookieSettings.beginGroup(QLatin1String("Exceptions"));
    _exceptions_block = cookieSettings.value(QLatin1String("block")).toStringList();
    _exceptions_allow = cookieSettings.value(QLatin1String("allow")).toStringList();
    _exceptions_allow_for_session = cookieSettings.value(QLatin1String("allowForSession")).toStringList();
    qSort(_exceptions_block.begin(), _exceptions_block.end());
    qSort(_exceptions_allow.begin(), _exceptions_allow.end());
    qSort(_exceptions_allow_for_session.begin(), _exceptions_allow_for_session.end());

    loadSettings();
}

void CookieJar::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("cookies"));
    QByteArray value = settings.value(QLatin1String("acceptCookies"), QLatin1String("AcceptOnlyFromSitesNavigatedTo")).toByteArray();
    QMetaEnum acceptPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("AcceptPolicy"));
    _accept_cookies = acceptPolicyEnum.keyToValue(value) == -1
        ? AcceptOnlyFromSitesNavigatedTo
        : static_cast<AcceptPolicy>(acceptPolicyEnum.keyToValue(value));

    value = settings.value(QLatin1String("keepCookiesUntil"), QLatin1String("KeepUntilExpire")).toByteArray();
    QMetaEnum keepPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("KeepPolicy"));
    _keep_cookies = keepPolicyEnum.keyToValue(value) == -1
        ? KeepUntilExpire
        : static_cast<KeepPolicy>(keepPolicyEnum.keyToValue(value));
    if (_keep_cookies == KeepUntilExit)
        setAllCookies(QList<QNetworkCookie>());
    _loaded = true;
    emit cookiesChanged();
}

void CookieJar::save()
{
    if (!_loaded)
        return;
    purgeOldCookies();
    QString directory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (directory.isEmpty())
        directory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    if (!QFile::exists(directory)) {
        QDir dir;
        dir.mkpath(directory);
    }
    QSettings cookieSettings(directory + QLatin1String("/cookies.ini"), QSettings::IniFormat);
    QList<QNetworkCookie> cookies = allCookies();
    for (int i = cookies.count() - 1; i >= 0; --i)
        if (cookies.at(i).isSessionCookie())
            cookies.removeAt(i);
    cookieSettings.setValue(QLatin1String("cookies"), QVariant::fromValue<QList<QNetworkCookie> >(cookies));
    cookieSettings.beginGroup(QLatin1String("Exceptions"));
    cookieSettings.setValue(QLatin1String("block"), _exceptions_block);
    cookieSettings.setValue(QLatin1String("allow"), _exceptions_allow);
    cookieSettings.setValue(QLatin1String("allowForSession"), _exceptions_allow_for_session);

    // save cookie settings
    QSettings settings;
    settings.beginGroup(QLatin1String("cookies"));
    QMetaEnum acceptPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("AcceptPolicy"));
    settings.setValue(QLatin1String("acceptCookies"), QLatin1String(acceptPolicyEnum.valueToKey(_accept_cookies)));

    QMetaEnum keepPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("KeepPolicy"));
    settings.setValue(QLatin1String("keepCookiesUntil"), QLatin1String(keepPolicyEnum.valueToKey(_keep_cookies)));
}

void CookieJar::purgeOldCookies()
{
    QList<QNetworkCookie> cookies = allCookies();
    if (cookies.isEmpty())
        return;
    int oldCount = cookies.count();
    QDateTime now = QDateTime::currentDateTime();
    for (int i = cookies.count() - 1; i >= 0; --i)
        if (!cookies.at(i).isSessionCookie() && cookies.at(i).expirationDate() < now)
            cookies.removeAt(i);
    if (oldCount == cookies.count())
        return;
    setAllCookies(cookies);
    emit cookiesChanged();
}

QList<QNetworkCookie> CookieJar::cookiesForUrl(const QUrl& url) const
{
    CookieJar* that = const_cast<CookieJar*>(this);
    if (!_loaded)
        that->load();
    // QWebEngineSettings *globalSettings = QWebEngineSettings::globalSettings();

    // if(globalSettings->testAttribute(QWebEngineSettings::PrivateBrowsingEnabled)) {
    // QList<QNetworkCookie> noCookies;
    // return noCookies;
    // }

    return QNetworkCookieJar::cookiesForUrl(url);
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie>& cookieList, const QUrl& url)
{
    if (!_loaded)
        load();
    // QWebEngineSettings *globalSettings = QWebEngineSettings::globalSettings();

    // if(globalSettings->testAttribute(QWebEngineSettings::PrivateBrowsingEnabled))
    // return false;

    QString host = url.host();
    bool eBlock = qBinaryFind(_exceptions_block.begin(), _exceptions_block.end(), host) != _exceptions_block.end();
    bool eAllow = qBinaryFind(_exceptions_allow.begin(), _exceptions_allow.end(), host) != _exceptions_allow.end();
    bool eAllowSession = qBinaryFind(_exceptions_allow_for_session.begin(), _exceptions_allow_for_session.end(), host) != _exceptions_allow_for_session.end();

    bool addedCookies = false;
    // pass exceptions
    bool acceptInitially = (_accept_cookies != AcceptNever);
    if ((acceptInitially && !eBlock)
        || (!acceptInitially && (eAllow || eAllowSession))) {
        // pass url domain == cookie domain
        QDateTime soon = QDateTime::currentDateTime();
        soon = soon.addDays(90);

        foreach (QNetworkCookie cookie, cookieList) {
            QList<QNetworkCookie> lst;
            if (_keep_cookies == KeepUntilTimeLimit
                && !cookie.isSessionCookie()
                && cookie.expirationDate() > soon)
                cookie.setExpirationDate(soon);
            lst += cookie;
            if (QNetworkCookieJar::setCookiesFromUrl(lst, url))
                addedCookies = true;
            else {
                // finally force it in if wanted
                if (_accept_cookies == AcceptAlways) {
                    QList<QNetworkCookie> cookies = allCookies();
                    cookies += cookie;
                    setAllCookies(cookies);
                    addedCookies = true;
                }
#if 0
					else qWarning() << "setCookiesFromUrl failed" << url << cookieList.value(0).toRawForm();

#endif
            }
        }
    }
    if (addedCookies) {
        _save_timer->changeOccurred();
        emit cookiesChanged();
    }
    return addedCookies;
}

CookieJar::AcceptPolicy CookieJar::acceptPolicy() const
{
    if (!_loaded)
        (const_cast<CookieJar*>(this))->load();
    return _accept_cookies;
}

void CookieJar::setAcceptPolicy(AcceptPolicy policy)
{
    if (!_loaded)
        load();
    if (policy == _accept_cookies)
        return;
    _accept_cookies = policy;
    _save_timer->changeOccurred();
}

CookieJar::KeepPolicy CookieJar::keepPolicy() const
{
    if (!_loaded)
        (const_cast<CookieJar*>(this))->load();
    return _keep_cookies;
}

void CookieJar::setKeepPolicy(KeepPolicy policy)
{
    if (!_loaded)
        load();
    if (policy == _keep_cookies)
        return;
    _keep_cookies = policy;
    _save_timer->changeOccurred();
}

QStringList CookieJar::blockedCookies() const
{
    if (!_loaded)
        (const_cast<CookieJar*>(this))->load();
    return _exceptions_block;
}

QStringList CookieJar::allowedCookies() const
{
    if (!_loaded)
        (const_cast<CookieJar*>(this))->load();
    return _exceptions_allow;
}

QStringList CookieJar::allowForSessionCookies() const
{
    if (!_loaded)
        (const_cast<CookieJar*>(this))->load();
    return _exceptions_allow_for_session;
}

void CookieJar::setBlockedCookies(const QStringList& list)
{
    if (!_loaded)
        load();
    _exceptions_block = list;
    qSort(_exceptions_block.begin(), _exceptions_block.end());
    _save_timer->changeOccurred();
}

void CookieJar::setAllowedCookies(const QStringList& list)
{
    if (!_loaded)
        load();
    _exceptions_allow = list;
    qSort(_exceptions_allow.begin(), _exceptions_allow.end());
    _save_timer->changeOccurred();
}

void CookieJar::setAllowForSessionCookies(const QStringList& list)
{
    if (!_loaded)
        load();
    _exceptions_allow_for_session = list;
    qSort(_exceptions_allow_for_session.begin(), _exceptions_allow_for_session.end());
    _save_timer->changeOccurred();
}

CookieModel::CookieModel(CookieJar* cookieJar, QObject* parent)
    : QAbstractTableModel(parent)
    , m_cookieJar(cookieJar)
{
    connect(m_cookieJar, SIGNAL(cookiesChanged()), this, SLOT(cookiesChanged()));
    m_cookieJar->load();
}

QVariant CookieModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::SizeHintRole) {
        QFont font;
        font.setPointSize(10);
        QFontMetrics fm(font);
        int height = fm.height() + fm.height() / 3;
        int width = fm.width(headerData(section, orientation, Qt::DisplayRole).toString());
        return QSize(width, height);
    }
    if (orientation == Qt::Horizontal) {
        if (role != Qt::DisplayRole)
            return QVariant();
        switch (section) {
        case 0:
            return tr("Website");

        case 1:
            return tr("Name");

        case 2:
            return tr("Path");

        case 3:
            return tr("Secure");

        case 4:
            return tr("Expires");

        case 5:
            return tr("Contents");

        default:
            return QVariant();
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant CookieModel::data(const QModelIndex& index, int role) const
{
    QList<QNetworkCookie> lst;
    if (m_cookieJar)
        lst = m_cookieJar->allCookies();
    if (index.row() < 0 || index.row() >= lst.size())
        return QVariant();
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        QNetworkCookie cookie = lst.at(index.row());
        switch (index.column()) {
        case 0:
            return cookie.domain();

        case 1:
            return cookie.name();

        case 2:
            return cookie.path();

        case 3:
            return cookie.isSecure();

        case 4:
            return cookie.expirationDate();

        case 5:
            return cookie.value();
        }
    }

    case Qt::FontRole: {
        QFont font;
        font.setPointSize(10);
        return font;
    }
    }
    return QVariant();
}

int CookieModel::columnCount(const QModelIndex& parent) const
{
    return (parent.isValid()) ? 0 : 6;
}

int CookieModel::rowCount(const QModelIndex& parent) const
{
    return (parent.isValid() || !m_cookieJar) ? 0 : m_cookieJar->allCookies().count();
}

bool CookieModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid() || !m_cookieJar)
        return false;
    int lastRow = row + count - 1;
    beginRemoveRows(parent, row, lastRow);
    QList<QNetworkCookie> lst = m_cookieJar->allCookies();
    for (int i = lastRow; i >= row; --i)
        lst.removeAt(i);
    m_cookieJar->setAllCookies(lst);
    endRemoveRows();
    return true;
}

void CookieModel::cookiesChanged()
{
    beginResetModel();
    endResetModel();
}

CookiesDialog::CookiesDialog(CookieJar* cookieJar, QWidget* parent)
    : QDialog(parent)
{
    setupUi(this);
    setWindowFlags(Qt::Sheet);
    CookieModel* model = new CookieModel(cookieJar, this);
    m_proxyModel = new QSortFilterProxyModel(this);
    connect(search, SIGNAL(textChanged(QString)), m_proxyModel, SLOT(setFilterFixedString(QString)));
    connect(removeButton, SIGNAL(clicked()), cookiesTable, SLOT(removeOne()));
    connect(removeAllButton, SIGNAL(clicked()), cookiesTable, SLOT(removeAll()));
    m_proxyModel->setSourceModel(model);
    cookiesTable->verticalHeader()->hide();
    cookiesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    cookiesTable->setModel(m_proxyModel);
    cookiesTable->setAlternatingRowColors(true);
    cookiesTable->setTextElideMode(Qt::ElideMiddle);
    cookiesTable->setShowGrid(false);
    cookiesTable->setSortingEnabled(true);
    QFont f = font();
    f.setPointSize(10);
    QFontMetrics fm(f);
    int height = fm.height() + fm.height() / 3;
    cookiesTable->verticalHeader()->setDefaultSectionSize(height);
    cookiesTable->verticalHeader()->setMinimumSectionSize(-1);
    for (int i = 0; i < model->columnCount(); ++i) {
        int header = cookiesTable->horizontalHeader()->sectionSizeHint(i);
        switch (i) {
        case 0:
            header = fm.width(QLatin1String("averagehost.domain.com"));
            break;

        case 1:
            header = fm.width(QLatin1String("_session_id"));
            break;

        case 4:
            header = fm.width(QDateTime::currentDateTime().toString(Qt::LocalDate));
            break;
        }
        int buffer = fm.width(QLatin1String("xx"));
        header += buffer;
        cookiesTable->horizontalHeader()->resizeSection(i, header);
    }
    cookiesTable->horizontalHeader()->setStretchLastSection(true);
}

CookieExceptionsModel::CookieExceptionsModel(CookieJar* cookiejar, QObject* parent)
    : QAbstractTableModel(parent)
    , m_cookieJar(cookiejar)
{
    m_allowedCookies = m_cookieJar->allowedCookies();
    m_blockedCookies = m_cookieJar->blockedCookies();
    m_sessionCookies = m_cookieJar->allowForSessionCookies();
}

QVariant CookieExceptionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::SizeHintRole) {
        QFont font;
        font.setPointSize(10);
        QFontMetrics fm(font);
        int height = fm.height() + fm.height() / 3;
        int width = fm.width(headerData(section, orientation, Qt::DisplayRole).toString());
        return QSize(width, height);
    }
    if (orientation == Qt::Horizontal
        && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Website");

        case 1:
            return tr("Status");
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant CookieExceptionsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        int row = index.row();
        if (row < m_allowedCookies.count()) {
            switch (index.column()) {
            case 0:
                return m_allowedCookies.at(row);

            case 1:
                return tr("Allow");
            }
        }
        row = row - m_allowedCookies.count();
        if (row < m_blockedCookies.count()) {
            switch (index.column()) {
            case 0:
                return m_blockedCookies.at(row);

            case 1:
                return tr("Block");
            }
        }
        row = row - m_blockedCookies.count();
        if (row < m_sessionCookies.count()) {
            switch (index.column()) {
            case 0:
                return m_sessionCookies.at(row);

            case 1:
                return tr("Allow For Session");
            }
        }
    }

    case Qt::FontRole: {
        QFont font;
        font.setPointSize(10);
        return font;
    }
    }
    return QVariant();
}

int CookieExceptionsModel::columnCount(const QModelIndex& parent) const
{
    return (parent.isValid()) ? 0 : 2;
}

int CookieExceptionsModel::rowCount(const QModelIndex& parent) const
{
    return (parent.isValid() || !m_cookieJar) ? 0 : m_allowedCookies.count() + m_blockedCookies.count() + m_sessionCookies.count();
}

bool CookieExceptionsModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (parent.isValid() || !m_cookieJar)
        return false;
    int lastRow = row + count - 1;
    beginRemoveRows(parent, row, lastRow);
    for (int i = lastRow; i >= row; --i) {
        if (i < m_allowedCookies.count()) {
            m_allowedCookies.removeAt(row);
            continue;
        }
        i = i - m_allowedCookies.count();
        if (i < m_blockedCookies.count()) {
            m_blockedCookies.removeAt(row);
            continue;
        }
        i = i - m_blockedCookies.count();
        if (i < m_sessionCookies.count()) {
            m_sessionCookies.removeAt(row);
            continue;
        }
    }
    m_cookieJar->setAllowedCookies(m_allowedCookies);
    m_cookieJar->setBlockedCookies(m_blockedCookies);
    m_cookieJar->setAllowForSessionCookies(m_sessionCookies);
    endRemoveRows();
    return true;
}

CookiesExceptionsDialog::CookiesExceptionsDialog(CookieJar* cookieJar, QWidget* parent)
    : QDialog(parent)
    , m_cookieJar(cookieJar)
{
    setupUi(this);
    setWindowFlags(Qt::Sheet);
    connect(removeButton, SIGNAL(clicked()), exceptionTable, SLOT(removeOne()));
    connect(removeAllButton, SIGNAL(clicked()), exceptionTable, SLOT(removeAll()));
    exceptionTable->verticalHeader()->hide();
    exceptionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    exceptionTable->setAlternatingRowColors(true);
    exceptionTable->setTextElideMode(Qt::ElideMiddle);
    exceptionTable->setShowGrid(false);
    exceptionTable->setSortingEnabled(true);
    m_exceptionsModel = new CookieExceptionsModel(cookieJar, this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_exceptionsModel);
    connect(search, SIGNAL(textChanged(QString)), m_proxyModel, SLOT(setFilterFixedString(QString)));
    exceptionTable->setModel(m_proxyModel);

    CookieModel* cookieModel = new CookieModel(cookieJar, this);
    domainLineEdit->setCompleter(new QCompleter(cookieModel, domainLineEdit));

    connect(domainLineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(blockButton, SIGNAL(clicked()), this, SLOT(block()));
    connect(allowButton, SIGNAL(clicked()), this, SLOT(allow()));
    connect(allowForSessionButton, SIGNAL(clicked()), this, SLOT(allowForSession()));

    QFont f = font();
    f.setPointSize(10);
    QFontMetrics fm(f);
    int height = fm.height() + fm.height() / 3;
    exceptionTable->verticalHeader()->setDefaultSectionSize(height);
    exceptionTable->verticalHeader()->setMinimumSectionSize(-1);
    for (int i = 0; i < m_exceptionsModel->columnCount(); ++i) {
        int header = exceptionTable->horizontalHeader()->sectionSizeHint(i);
        switch (i) {
        case 0:
            header = fm.width(QLatin1String("averagebiglonghost.domain.com"));
            break;

        case 1:
            header = fm.width(QLatin1String("Allow For Session"));
            break;
        }
        int buffer = fm.width(QLatin1String("xx"));
        header += buffer;
        exceptionTable->horizontalHeader()->resizeSection(i, header);
    }
}

void CookiesExceptionsDialog::textChanged(const QString& text)
{
    bool enabled = !text.isEmpty();
    blockButton->setEnabled(enabled);
    allowButton->setEnabled(enabled);
    allowForSessionButton->setEnabled(enabled);
}

void CookiesExceptionsDialog::block()
{
    if (domainLineEdit->text().isEmpty())
        return;
    m_exceptionsModel->m_blockedCookies.append(domainLineEdit->text());
    m_cookieJar->setBlockedCookies(m_exceptionsModel->m_blockedCookies);
    m_exceptionsModel->beginResetModel();
    m_exceptionsModel->endResetModel();
}

void CookiesExceptionsDialog::allow()
{
    if (domainLineEdit->text().isEmpty())
        return;
    m_exceptionsModel->m_allowedCookies.append(domainLineEdit->text());
    m_cookieJar->setAllowedCookies(m_exceptionsModel->m_allowedCookies);
    m_exceptionsModel->beginResetModel();
    m_exceptionsModel->endResetModel();
}

void CookiesExceptionsDialog::allowForSession()
{
    if (domainLineEdit->text().isEmpty())
        return;
    m_exceptionsModel->m_sessionCookies.append(domainLineEdit->text());
    m_cookieJar->setAllowForSessionCookies(m_exceptionsModel->m_sessionCookies);
    m_exceptionsModel->beginResetModel();
    m_exceptionsModel->endResetModel();
}
}
