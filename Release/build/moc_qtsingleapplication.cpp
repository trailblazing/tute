/****************************************************************************
** Meta object code from reading C++ file 'qtsingleapplication.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libraries/qtSingleApplication5/qtsingleapplication.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtsingleapplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QtSingleApplication_t {
    QByteArrayData data[12];
    char stringdata0[177];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtSingleApplication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtSingleApplication_t qt_meta_stringdata_QtSingleApplication = {
    {
QT_MOC_LITERAL(0, 0, 19), // "QtSingleApplication"
QT_MOC_LITERAL(1, 20, 15), // "messageReceived"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 7), // "message"
QT_MOC_LITERAL(4, 45, 22), // "privateBrowsingChanged"
QT_MOC_LITERAL(5, 68, 11), // "sendMessage"
QT_MOC_LITERAL(6, 80, 7), // "timeout"
QT_MOC_LITERAL(7, 88, 14), // "activateWindow"
QT_MOC_LITERAL(8, 103, 18), // "restoreLastSession"
QT_MOC_LITERAL(9, 122, 18), // "setPrivateBrowsing"
QT_MOC_LITERAL(10, 141, 10), // "postLaunch"
QT_MOC_LITERAL(11, 152, 24) // "newLocalSocketConnection"

    },
    "QtSingleApplication\0messageReceived\0"
    "\0message\0privateBrowsingChanged\0"
    "sendMessage\0timeout\0activateWindow\0"
    "restoreLastSession\0setPrivateBrowsing\0"
    "postLaunch\0newLocalSocketConnection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtSingleApplication[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x06 /* Public */,
       4,    1,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    2,   65,    2, 0x0a /* Public */,
       5,    1,   70,    2, 0x2a /* Public | MethodCloned */,
       7,    0,   73,    2, 0x0a /* Public */,
       8,    0,   74,    2, 0x0a /* Public */,
       9,    1,   75,    2, 0x0a /* Public */,
      10,    0,   78,    2, 0x08 /* Private */,
      11,    0,   79,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Bool, QMetaType::QString, QMetaType::Int,    3,    6,
    QMetaType::Bool, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QtSingleApplication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QtSingleApplication *_t = static_cast<QtSingleApplication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->messageReceived((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->privateBrowsingChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: { bool _r = _t->sendMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 3: { bool _r = _t->sendMessage((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 4: _t->activateWindow(); break;
        case 5: _t->restoreLastSession(); break;
        case 6: _t->setPrivateBrowsing((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->postLaunch(); break;
        case 8: _t->newLocalSocketConnection(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QtSingleApplication::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QtSingleApplication::messageReceived)) {
                *result = 0;
            }
        }
        {
            typedef void (QtSingleApplication::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QtSingleApplication::privateBrowsingChanged)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject QtSingleApplication::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_QtSingleApplication.data,
      qt_meta_data_QtSingleApplication,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QtSingleApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtSingleApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QtSingleApplication.stringdata0))
        return static_cast<void*>(const_cast< QtSingleApplication*>(this));
    return QApplication::qt_metacast(_clname);
}

int QtSingleApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
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
void QtSingleApplication::messageReceived(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtSingleApplication::privateBrowsingChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
