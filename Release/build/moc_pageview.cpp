/****************************************************************************
** Meta object code from reading C++ file 'pageview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/pageview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pageview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_browser__Page_t {
    QByteArrayData data[10];
    char stringdata0[123];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__Page_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__Page_t qt_meta_stringdata_browser__Page = {
    {
QT_MOC_LITERAL(0, 0, 13), // "browser::Page"
QT_MOC_LITERAL(1, 14, 10), // "loadingUrl"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 3), // "url"
QT_MOC_LITERAL(4, 30, 22), // "authenticationRequired"
QT_MOC_LITERAL(5, 53, 10), // "requestUrl"
QT_MOC_LITERAL(6, 64, 15), // "QAuthenticator*"
QT_MOC_LITERAL(7, 80, 4), // "auth"
QT_MOC_LITERAL(8, 85, 27), // "proxyAuthenticationRequired"
QT_MOC_LITERAL(9, 113, 9) // "proxyHost"

    },
    "browser::Page\0loadingUrl\0\0url\0"
    "authenticationRequired\0requestUrl\0"
    "QAuthenticator*\0auth\0proxyAuthenticationRequired\0"
    "proxyHost"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__Page[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,   32,    2, 0x08 /* Private */,
       8,    3,   37,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUrl,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 6,    5,    7,
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 6, QMetaType::QString,    5,    7,    9,

       0        // eod
};

void browser::Page::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Page *_t = static_cast<Page *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loadingUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->authenticationRequired((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2]))); break;
        case 2: _t->proxyAuthenticationRequired((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< QAuthenticator*(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Page::*_t)(const QUrl & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Page::loadingUrl)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::Page::staticMetaObject = {
    { &QWebEnginePage::staticMetaObject, qt_meta_stringdata_browser__Page.data,
      qt_meta_data_browser__Page,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::Page::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::Page::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__Page.stringdata0))
        return static_cast<void*>(const_cast< Page*>(this));
    return QWebEnginePage::qt_metacast(_clname);
}

int browser::Page::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebEnginePage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void browser::Page::loadingUrl(const QUrl & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_browser__PageView_t {
    QByteArrayData data[19];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__PageView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__PageView_t qt_meta_stringdata_browser__PageView = {
    {
QT_MOC_LITERAL(0, 0, 17), // "browser::PageView"
QT_MOC_LITERAL(1, 18, 11), // "iconChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 11), // "setProgress"
QT_MOC_LITERAL(4, 43, 8), // "progress"
QT_MOC_LITERAL(5, 52, 14), // "onLoadFinished"
QT_MOC_LITERAL(6, 67, 7), // "success"
QT_MOC_LITERAL(7, 75, 16), // "setStatusBarText"
QT_MOC_LITERAL(8, 92, 6), // "string"
QT_MOC_LITERAL(9, 99, 16), // "openLinkInNewTab"
QT_MOC_LITERAL(10, 116, 3), // "url"
QT_MOC_LITERAL(11, 120, 28), // "onFeaturePermissionRequested"
QT_MOC_LITERAL(12, 149, 14), // "securityOrigin"
QT_MOC_LITERAL(13, 164, 23), // "QWebEnginePage::Feature"
QT_MOC_LITERAL(14, 188, 16), // "onIconUrlChanged"
QT_MOC_LITERAL(15, 205, 10), // "iconLoaded"
QT_MOC_LITERAL(16, 216, 12), // "onUrlChanged"
QT_MOC_LITERAL(17, 229, 14), // "onTitleChanged"
QT_MOC_LITERAL(18, 244, 5) // "title"

    },
    "browser::PageView\0iconChanged\0\0"
    "setProgress\0progress\0onLoadFinished\0"
    "success\0setStatusBarText\0string\0"
    "openLinkInNewTab\0url\0onFeaturePermissionRequested\0"
    "securityOrigin\0QWebEnginePage::Feature\0"
    "onIconUrlChanged\0iconLoaded\0onUrlChanged\0"
    "onTitleChanged\0title"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__PageView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   65,    2, 0x08 /* Private */,
       5,    1,   68,    2, 0x08 /* Private */,
       7,    1,   71,    2, 0x08 /* Private */,
       9,    1,   74,    2, 0x08 /* Private */,
      11,    2,   77,    2, 0x08 /* Private */,
      14,    1,   82,    2, 0x08 /* Private */,
      15,    0,   85,    2, 0x08 /* Private */,
      16,    1,   86,    2, 0x08 /* Private */,
      17,    1,   89,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QUrl,   10,
    QMetaType::Void, QMetaType::QUrl, 0x80000000 | 13,   12,    2,
    QMetaType::Void, QMetaType::QUrl,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl,   10,
    QMetaType::Void, QMetaType::QString,   18,

       0        // eod
};

void browser::PageView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PageView *_t = static_cast<PageView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->iconChanged(); break;
        case 1: _t->setProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onLoadFinished((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->setStatusBarText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->openLinkInNewTab((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 5: _t->onFeaturePermissionRequested((*reinterpret_cast< const QUrl(*)>(_a[1])),(*reinterpret_cast< QWebEnginePage::Feature(*)>(_a[2]))); break;
        case 6: _t->onIconUrlChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 7: _t->iconLoaded(); break;
        case 8: _t->onUrlChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 9: _t->onTitleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PageView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PageView::iconChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::PageView::staticMetaObject = {
    { &QWebEngineView::staticMetaObject, qt_meta_stringdata_browser__PageView.data,
      qt_meta_data_browser__PageView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::PageView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::PageView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__PageView.stringdata0))
        return static_cast<void*>(const_cast< PageView*>(this));
    return QWebEngineView::qt_metacast(_clname);
}

int browser::PageView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebEngineView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void browser::PageView::iconChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
