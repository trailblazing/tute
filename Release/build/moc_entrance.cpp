/****************************************************************************
** Meta object code from reading C++ file 'entrance.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/entrance.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'entrance.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_browser__Entrance_t {
    QByteArrayData data[27];
    char stringdata0[294];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__Entrance_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__Entrance_t qt_meta_stringdata_browser__Entrance = {
    {
QT_MOC_LITERAL(0, 0, 17), // "browser::Entrance"
QT_MOC_LITERAL(1, 18, 11), // "initSetting"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 13), // "setScrollbars"
QT_MOC_LITERAL(4, 45, 4), // "hide"
QT_MOC_LITERAL(5, 50, 8), // "setCache"
QT_MOC_LITERAL(6, 59, 5), // "cache"
QT_MOC_LITERAL(7, 65, 9), // "cacheSize"
QT_MOC_LITERAL(8, 75, 8), // "finished"
QT_MOC_LITERAL(9, 84, 14), // "QNetworkReply*"
QT_MOC_LITERAL(10, 99, 5), // "reply"
QT_MOC_LITERAL(11, 105, 9), // "sslErrors"
QT_MOC_LITERAL(12, 115, 16), // "QList<QSslError>"
QT_MOC_LITERAL(13, 132, 6), // "errors"
QT_MOC_LITERAL(14, 139, 15), // "getactionFreeze"
QT_MOC_LITERAL(15, 155, 8), // "QAction*"
QT_MOC_LITERAL(16, 164, 12), // "setupActions"
QT_MOC_LITERAL(17, 177, 16), // "new_dockedwindow"
QT_MOC_LITERAL(18, 194, 34), // "std::pair<DockedWindow*,PageV..."
QT_MOC_LITERAL(19, 229, 7), // "Record*"
QT_MOC_LITERAL(20, 237, 6), // "record"
QT_MOC_LITERAL(21, 244, 13), // "DockedWindow*"
QT_MOC_LITERAL(22, 258, 5), // "state"
QT_MOC_LITERAL(23, 264, 7), // "prepend"
QT_MOC_LITERAL(24, 272, 9), // "Entrance*"
QT_MOC_LITERAL(25, 282, 7), // "openUrl"
QT_MOC_LITERAL(26, 290, 3) // "url"

    },
    "browser::Entrance\0initSetting\0\0"
    "setScrollbars\0hide\0setCache\0cache\0"
    "cacheSize\0finished\0QNetworkReply*\0"
    "reply\0sslErrors\0QList<QSslError>\0"
    "errors\0getactionFreeze\0QAction*\0"
    "setupActions\0new_dockedwindow\0"
    "std::pair<DockedWindow*,PageView*>\0"
    "Record*\0record\0DockedWindow*\0state\0"
    "prepend\0Entrance*\0openUrl\0url"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__Entrance[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    1,   70,    2, 0x0a /* Public */,
       5,    2,   73,    2, 0x0a /* Public */,
       8,    1,   78,    2, 0x0a /* Public */,
      11,    2,   81,    2, 0x0a /* Public */,
      14,    0,   86,    2, 0x0a /* Public */,
      16,    0,   87,    2, 0x0a /* Public */,
      17,    1,   88,    2, 0x0a /* Public */,
      17,    1,   91,    2, 0x0a /* Public */,
      23,    1,   94,    2, 0x0a /* Public */,
      25,    1,   97,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    4,
    QMetaType::Void, QMetaType::Bool, QMetaType::Int,    6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 12,   10,   13,
    0x80000000 | 15,
    QMetaType::Void,
    0x80000000 | 18, 0x80000000 | 19,   20,
    0x80000000 | 21, QMetaType::QByteArray,   22,
    0x80000000 | 24, 0x80000000 | 21,    2,
    QMetaType::Void, QMetaType::QUrl,   26,

       0        // eod
};

void browser::Entrance::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Entrance *_t = static_cast<Entrance *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->initSetting(); break;
        case 1: _t->setScrollbars((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->setCache((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->finished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 4: _t->sslErrors((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< const QList<QSslError>(*)>(_a[2]))); break;
        case 5: { QAction* _r = _t->getactionFreeze();
            if (_a[0]) *reinterpret_cast< QAction**>(_a[0]) = _r; }  break;
        case 6: _t->setupActions(); break;
        case 7: { std::pair<DockedWindow*,PageView*> _r = _t->new_dockedwindow((*reinterpret_cast< Record*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< std::pair<DockedWindow*,PageView*>*>(_a[0]) = _r; }  break;
        case 8: { DockedWindow* _r = _t->new_dockedwindow((*reinterpret_cast< const QByteArray(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< DockedWindow**>(_a[0]) = _r; }  break;
        case 9: { Entrance* _r = _t->prepend((*reinterpret_cast< DockedWindow*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Entrance**>(_a[0]) = _r; }  break;
        case 10: _t->openUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QSslError> >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< DockedWindow* >(); break;
            }
            break;
        }
    }
}

const QMetaObject browser::Entrance::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_browser__Entrance.data,
      qt_meta_data_browser__Entrance,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::Entrance::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::Entrance::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__Entrance.stringdata0))
        return static_cast<void*>(const_cast< Entrance*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int browser::Entrance::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
