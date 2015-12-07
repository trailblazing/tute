/****************************************************************************
** Meta object code from reading C++ file 'webview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/webview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'webview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_browser__WebPage_t {
    QByteArrayData data[13];
    char stringdata0[160];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__WebPage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__WebPage_t qt_meta_stringdata_browser__WebPage = {
    {
QT_MOC_LITERAL(0, 0, 16), // "browser::WebPage"
QT_MOC_LITERAL(1, 17, 10), // "loadingUrl"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 3), // "url"
QT_MOC_LITERAL(4, 33, 22), // "authenticationRequired"
QT_MOC_LITERAL(5, 56, 10), // "requestUrl"
QT_MOC_LITERAL(6, 67, 15), // "QAuthenticator*"
QT_MOC_LITERAL(7, 83, 4), // "auth"
QT_MOC_LITERAL(8, 88, 27), // "proxyAuthenticationRequired"
QT_MOC_LITERAL(9, 116, 9), // "proxyHost"
QT_MOC_LITERAL(10, 126, 12), // "onUrlChanged"
QT_MOC_LITERAL(11, 139, 14), // "onTitleChanged"
QT_MOC_LITERAL(12, 154, 5) // "title"

    },
    "browser::WebPage\0loadingUrl\0\0url\0"
    "authenticationRequired\0requestUrl\0"
    "QAuthenticator*\0auth\0proxyAuthenticationRequired\0"
    "proxyHost\0onUrlChanged\0onTitleChanged\0"
    "title"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__WebPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,   42,    2, 0x08 /* Private */,
       8,    3,   47,    2, 0x08 /* Private */,
      10,    1,   54,    2, 0x08 /* Private */,
      11,    1,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUrl,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 6,    5,    7,
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 6, QMetaType::QString,    5,    7,    9,
    QMetaType::Void, QMetaType::QUrl,    3,
    QMetaType::Void, QMetaType::QString,   12,

       0        // eod
};

void browser::WebPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WebPage *_t = static_cast<WebPage *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loadingUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->authenticationRequired((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 2: _t->proxyAuthenticationRequired((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 3: _t->onUrlChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 4: _t->onTitleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WebPage::*_t)(const QUrl & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WebPage::loadingUrl)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::WebPage::staticMetaObject = {
    { &QWebEnginePage::staticMetaObject, qt_meta_stringdata_browser__WebPage.data,
      qt_meta_data_browser__WebPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::WebPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::WebPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__WebPage.stringdata0))
        return static_cast<void*>(const_cast< WebPage*>(this));
    return QWebEnginePage::qt_metacast(_clname);
}

int browser::WebPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebEnginePage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void browser::WebPage::loadingUrl(const QUrl & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_browser__WebView_t {
    QByteArrayData data[16];
    char stringdata0[215];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__WebView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__WebView_t qt_meta_stringdata_browser__WebView = {
    {
QT_MOC_LITERAL(0, 0, 16), // "browser::WebView"
QT_MOC_LITERAL(1, 17, 11), // "iconChanged"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 11), // "setProgress"
QT_MOC_LITERAL(4, 42, 8), // "progress"
QT_MOC_LITERAL(5, 51, 16), // "setStatusBarText"
QT_MOC_LITERAL(6, 68, 6), // "string"
QT_MOC_LITERAL(7, 75, 16), // "openLinkInNewTab"
QT_MOC_LITERAL(8, 92, 3), // "url"
QT_MOC_LITERAL(9, 96, 28), // "onFeaturePermissionRequested"
QT_MOC_LITERAL(10, 125, 14), // "securityOrigin"
QT_MOC_LITERAL(11, 140, 23), // "QWebEnginePage::Feature"
QT_MOC_LITERAL(12, 164, 16), // "onIconUrlChanged"
QT_MOC_LITERAL(13, 181, 10), // "iconLoaded"
QT_MOC_LITERAL(14, 192, 14), // "onLoadFinished"
QT_MOC_LITERAL(15, 207, 7) // "success"

    },
    "browser::WebView\0iconChanged\0\0setProgress\0"
    "progress\0setStatusBarText\0string\0"
    "openLinkInNewTab\0url\0onFeaturePermissionRequested\0"
    "securityOrigin\0QWebEnginePage::Feature\0"
    "onIconUrlChanged\0iconLoaded\0onLoadFinished\0"
    "success"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__WebView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   60,    2, 0x08 /* Private */,
       5,    1,   63,    2, 0x08 /* Private */,
       7,    0,   66,    2, 0x08 /* Private */,
       7,    1,   67,    2, 0x08 /* Private */,
       9,    2,   70,    2, 0x08 /* Private */,
      12,    1,   75,    2, 0x08 /* Private */,
      13,    0,   78,    2, 0x08 /* Private */,
      14,    1,   79,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl,    8,
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 11,   10,    2,
    QMetaType::Void, QMetaType::QUrl,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   15,

       0        // eod
};

void browser::WebView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WebView *_t = static_cast<WebView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->iconChanged(); break;
        case 1: _t->setProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setStatusBarText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->openLinkInNewTab(); break;
        case 4: _t->openLinkInNewTab((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 5: _t->onFeaturePermissionRequested((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< QWebEnginePage::Feature(*)>(_a[2]))); break;
        case 6: _t->onIconUrlChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 7: _t->iconLoaded(); break;
        case 8: _t->onLoadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WebView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WebView::iconChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::WebView::staticMetaObject = {
    { &QWebEngineView::staticMetaObject, qt_meta_stringdata_browser__WebView.data,
      qt_meta_data_browser__WebView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::WebView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::WebView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__WebView.stringdata0))
        return static_cast<void*>(const_cast< WebView*>(this));
    return QWebEngineView::qt_metacast(_clname);
}

int browser::WebView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebEngineView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void browser::WebView::iconChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
