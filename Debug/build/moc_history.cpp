/****************************************************************************
** Meta object code from reading C++ file 'history.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/history.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'history.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_browser__HistoryManager_t {
    QByteArrayData data[14];
    char stringdata0[152];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__HistoryManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__HistoryManager_t qt_meta_stringdata_browser__HistoryManager = {
    {
QT_MOC_LITERAL(0, 0, 23), // "browser::HistoryManager"
QT_MOC_LITERAL(1, 24, 12), // "historyReset"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 10), // "entryAdded"
QT_MOC_LITERAL(4, 49, 11), // "HistoryItem"
QT_MOC_LITERAL(5, 61, 4), // "item"
QT_MOC_LITERAL(6, 66, 12), // "entryRemoved"
QT_MOC_LITERAL(7, 79, 12), // "entryUpdated"
QT_MOC_LITERAL(8, 92, 6), // "offset"
QT_MOC_LITERAL(9, 99, 5), // "clear"
QT_MOC_LITERAL(10, 105, 12), // "loadSettings"
QT_MOC_LITERAL(11, 118, 4), // "save"
QT_MOC_LITERAL(12, 123, 15), // "checkForExpired"
QT_MOC_LITERAL(13, 139, 12) // "historyLimit"

    },
    "browser::HistoryManager\0historyReset\0"
    "\0entryAdded\0HistoryItem\0item\0entryRemoved\0"
    "entryUpdated\0offset\0clear\0loadSettings\0"
    "save\0checkForExpired\0historyLimit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__HistoryManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       1,   68, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    1,   55,    2, 0x06 /* Public */,
       6,    1,   58,    2, 0x06 /* Public */,
       7,    1,   61,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   64,    2, 0x0a /* Public */,
      10,    0,   65,    2, 0x0a /* Public */,
      11,    0,   66,    2, 0x08 /* Private */,
      12,    0,   67,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, QMetaType::Int,    8,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      13, QMetaType::Int, 0x00095103,

       0        // eod
};

void browser::HistoryManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistoryManager *_t = static_cast<HistoryManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->historyReset(); break;
        case 1: _t->entryAdded((*reinterpret_cast< const HistoryItem(*)>(_a[1]))); break;
        case 2: _t->entryRemoved((*reinterpret_cast< const HistoryItem(*)>(_a[1]))); break;
        case 3: _t->entryUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->clear(); break;
        case 5: _t->loadSettings(); break;
        case 6: _t->save(); break;
        case 7: _t->checkForExpired(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (HistoryManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HistoryManager::historyReset)) {
                *result = 0;
            }
        }
        {
            typedef void (HistoryManager::*_t)(const HistoryItem & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HistoryManager::entryAdded)) {
                *result = 1;
            }
        }
        {
            typedef void (HistoryManager::*_t)(const HistoryItem & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HistoryManager::entryRemoved)) {
                *result = 2;
            }
        }
        {
            typedef void (HistoryManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HistoryManager::entryUpdated)) {
                *result = 3;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        HistoryManager *_t = static_cast<HistoryManager *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->historyLimit(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        HistoryManager *_t = static_cast<HistoryManager *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setHistoryLimit(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject browser::HistoryManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_browser__HistoryManager.data,
      qt_meta_data_browser__HistoryManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::HistoryManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::HistoryManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__HistoryManager.stringdata0))
        return static_cast<void*>(const_cast< HistoryManager*>(this));
    return QObject::qt_metacast(_clname);
}

int browser::HistoryManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void browser::HistoryManager::historyReset()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void browser::HistoryManager::entryAdded(const HistoryItem & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void browser::HistoryManager::entryRemoved(const HistoryItem & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void browser::HistoryManager::entryUpdated(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
struct qt_meta_stringdata_browser__HistoryModel_t {
    QByteArrayData data[6];
    char stringdata0[67];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__HistoryModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__HistoryModel_t qt_meta_stringdata_browser__HistoryModel = {
    {
QT_MOC_LITERAL(0, 0, 21), // "browser::HistoryModel"
QT_MOC_LITERAL(1, 22, 12), // "historyReset"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 10), // "entryAdded"
QT_MOC_LITERAL(4, 47, 12), // "entryUpdated"
QT_MOC_LITERAL(5, 60, 6) // "offset"

    },
    "browser::HistoryModel\0historyReset\0\0"
    "entryAdded\0entryUpdated\0offset"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__HistoryModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    0,   30,    2, 0x0a /* Public */,
       4,    1,   31,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void browser::HistoryModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistoryModel *_t = static_cast<HistoryModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->historyReset(); break;
        case 1: _t->entryAdded(); break;
        case 2: _t->entryUpdated((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject browser::HistoryModel::staticMetaObject = {
    { &QAbstractTableModel::staticMetaObject, qt_meta_stringdata_browser__HistoryModel.data,
      qt_meta_data_browser__HistoryModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::HistoryModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::HistoryModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__HistoryModel.stringdata0))
        return static_cast<void*>(const_cast< HistoryModel*>(this));
    return QAbstractTableModel::qt_metacast(_clname);
}

int browser::HistoryModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractTableModel::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_browser__HistoryFilterModel_t {
    QByteArrayData data[11];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__HistoryFilterModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__HistoryFilterModel_t qt_meta_stringdata_browser__HistoryFilterModel = {
    {
QT_MOC_LITERAL(0, 0, 27), // "browser::HistoryFilterModel"
QT_MOC_LITERAL(1, 28, 11), // "sourceReset"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 17), // "sourceDataChanged"
QT_MOC_LITERAL(4, 59, 7), // "topLeft"
QT_MOC_LITERAL(5, 67, 11), // "bottomRight"
QT_MOC_LITERAL(6, 79, 18), // "sourceRowsInserted"
QT_MOC_LITERAL(7, 98, 6), // "parent"
QT_MOC_LITERAL(8, 105, 5), // "start"
QT_MOC_LITERAL(9, 111, 3), // "end"
QT_MOC_LITERAL(10, 115, 17) // "sourceRowsRemoved"

    },
    "browser::HistoryFilterModel\0sourceReset\0"
    "\0sourceDataChanged\0topLeft\0bottomRight\0"
    "sourceRowsInserted\0parent\0start\0end\0"
    "sourceRowsRemoved"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__HistoryFilterModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08 /* Private */,
       3,    2,   35,    2, 0x08 /* Private */,
       6,    3,   40,    2, 0x08 /* Private */,
      10,    3,   47,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::QModelIndex,    4,    5,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,    7,    8,    9,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,    2,    2,    2,

       0        // eod
};

void browser::HistoryFilterModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistoryFilterModel *_t = static_cast<HistoryFilterModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sourceReset(); break;
        case 1: _t->sourceDataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 2: _t->sourceRowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->sourceRowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject browser::HistoryFilterModel::staticMetaObject = {
    { &QAbstractProxyModel::staticMetaObject, qt_meta_stringdata_browser__HistoryFilterModel.data,
      qt_meta_data_browser__HistoryFilterModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::HistoryFilterModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::HistoryFilterModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__HistoryFilterModel.stringdata0))
        return static_cast<void*>(const_cast< HistoryFilterModel*>(this));
    return QAbstractProxyModel::qt_metacast(_clname);
}

int browser::HistoryFilterModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_browser__HistoryMenuModel_t {
    QByteArrayData data[1];
    char stringdata0[26];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__HistoryMenuModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__HistoryMenuModel_t qt_meta_stringdata_browser__HistoryMenuModel = {
    {
QT_MOC_LITERAL(0, 0, 25) // "browser::HistoryMenuModel"

    },
    "browser::HistoryMenuModel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__HistoryMenuModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void browser::HistoryMenuModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject browser::HistoryMenuModel::staticMetaObject = {
    { &QAbstractProxyModel::staticMetaObject, qt_meta_stringdata_browser__HistoryMenuModel.data,
      qt_meta_data_browser__HistoryMenuModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::HistoryMenuModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::HistoryMenuModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__HistoryMenuModel.stringdata0))
        return static_cast<void*>(const_cast< HistoryMenuModel*>(this));
    return QAbstractProxyModel::qt_metacast(_clname);
}

int browser::HistoryMenuModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_browser__HistoryMenu_t {
    QByteArrayData data[7];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__HistoryMenu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__HistoryMenu_t qt_meta_stringdata_browser__HistoryMenu = {
    {
QT_MOC_LITERAL(0, 0, 20), // "browser::HistoryMenu"
QT_MOC_LITERAL(1, 21, 7), // "openUrl"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 3), // "url"
QT_MOC_LITERAL(4, 34, 9), // "activated"
QT_MOC_LITERAL(5, 44, 5), // "index"
QT_MOC_LITERAL(6, 50, 17) // "showHistoryDialog"

    },
    "browser::HistoryMenu\0openUrl\0\0url\0"
    "activated\0index\0showHistoryDialog"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__HistoryMenu[] = {

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
       4,    1,   32,    2, 0x08 /* Private */,
       6,    0,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUrl,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex,    5,
    QMetaType::Void,

       0        // eod
};

void browser::HistoryMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistoryMenu *_t = static_cast<HistoryMenu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 2: _t->showHistoryDialog(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (HistoryMenu::*_t)(const QUrl & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HistoryMenu::openUrl)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::HistoryMenu::staticMetaObject = {
    { &ModelMenu::staticMetaObject, qt_meta_stringdata_browser__HistoryMenu.data,
      qt_meta_data_browser__HistoryMenu,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::HistoryMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::HistoryMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__HistoryMenu.stringdata0))
        return static_cast<void*>(const_cast< HistoryMenu*>(this));
    return ModelMenu::qt_metacast(_clname);
}

int browser::HistoryMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ModelMenu::qt_metacall(_c, _id, _a);
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
void browser::HistoryMenu::openUrl(const QUrl & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_browser__HistoryCompletionModel_t {
    QByteArrayData data[3];
    char stringdata0[45];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__HistoryCompletionModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__HistoryCompletionModel_t qt_meta_stringdata_browser__HistoryCompletionModel = {
    {
QT_MOC_LITERAL(0, 0, 31), // "browser::HistoryCompletionModel"
QT_MOC_LITERAL(1, 32, 11), // "sourceReset"
QT_MOC_LITERAL(2, 44, 0) // ""

    },
    "browser::HistoryCompletionModel\0"
    "sourceReset\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__HistoryCompletionModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void browser::HistoryCompletionModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistoryCompletionModel *_t = static_cast<HistoryCompletionModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sourceReset(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject browser::HistoryCompletionModel::staticMetaObject = {
    { &QAbstractProxyModel::staticMetaObject, qt_meta_stringdata_browser__HistoryCompletionModel.data,
      qt_meta_data_browser__HistoryCompletionModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::HistoryCompletionModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::HistoryCompletionModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__HistoryCompletionModel.stringdata0))
        return static_cast<void*>(const_cast< HistoryCompletionModel*>(this));
    return QAbstractProxyModel::qt_metacast(_clname);
}

int browser::HistoryCompletionModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_browser__HistoryTreeModel_t {
    QByteArrayData data[8];
    char stringdata0[93];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__HistoryTreeModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__HistoryTreeModel_t qt_meta_stringdata_browser__HistoryTreeModel = {
    {
QT_MOC_LITERAL(0, 0, 25), // "browser::HistoryTreeModel"
QT_MOC_LITERAL(1, 26, 11), // "sourceReset"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 18), // "sourceRowsInserted"
QT_MOC_LITERAL(4, 58, 6), // "parent"
QT_MOC_LITERAL(5, 65, 5), // "start"
QT_MOC_LITERAL(6, 71, 3), // "end"
QT_MOC_LITERAL(7, 75, 17) // "sourceRowsRemoved"

    },
    "browser::HistoryTreeModel\0sourceReset\0"
    "\0sourceRowsInserted\0parent\0start\0end\0"
    "sourceRowsRemoved"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__HistoryTreeModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x08 /* Private */,
       3,    3,   30,    2, 0x08 /* Private */,
       7,    3,   37,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,    4,    5,    6,
    QMetaType::Void, QMetaType::QModelIndex, QMetaType::Int, QMetaType::Int,    4,    5,    6,

       0        // eod
};

void browser::HistoryTreeModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistoryTreeModel *_t = static_cast<HistoryTreeModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sourceReset(); break;
        case 1: _t->sourceRowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->sourceRowsRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject browser::HistoryTreeModel::staticMetaObject = {
    { &QAbstractProxyModel::staticMetaObject, qt_meta_stringdata_browser__HistoryTreeModel.data,
      qt_meta_data_browser__HistoryTreeModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::HistoryTreeModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::HistoryTreeModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__HistoryTreeModel.stringdata0))
        return static_cast<void*>(const_cast< HistoryTreeModel*>(this));
    return QAbstractProxyModel::qt_metacast(_clname);
}

int browser::HistoryTreeModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractProxyModel::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_browser__TreeProxyModel_t {
    QByteArrayData data[1];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__TreeProxyModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__TreeProxyModel_t qt_meta_stringdata_browser__TreeProxyModel = {
    {
QT_MOC_LITERAL(0, 0, 23) // "browser::TreeProxyModel"

    },
    "browser::TreeProxyModel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__TreeProxyModel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void browser::TreeProxyModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject browser::TreeProxyModel::staticMetaObject = {
    { &QSortFilterProxyModel::staticMetaObject, qt_meta_stringdata_browser__TreeProxyModel.data,
      qt_meta_data_browser__TreeProxyModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::TreeProxyModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::TreeProxyModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__TreeProxyModel.stringdata0))
        return static_cast<void*>(const_cast< TreeProxyModel*>(this));
    return QSortFilterProxyModel::qt_metacast(_clname);
}

int browser::TreeProxyModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSortFilterProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_browser__HistoryDialog_t {
    QByteArrayData data[8];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__HistoryDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__HistoryDialog_t qt_meta_stringdata_browser__HistoryDialog = {
    {
QT_MOC_LITERAL(0, 0, 22), // "browser::HistoryDialog"
QT_MOC_LITERAL(1, 23, 7), // "openUrl"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 3), // "url"
QT_MOC_LITERAL(4, 36, 26), // "customContextMenuRequested"
QT_MOC_LITERAL(5, 63, 3), // "pos"
QT_MOC_LITERAL(6, 67, 4), // "open"
QT_MOC_LITERAL(7, 72, 4) // "copy"

    },
    "browser::HistoryDialog\0openUrl\0\0url\0"
    "customContextMenuRequested\0pos\0open\0"
    "copy"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__HistoryDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   37,    2, 0x08 /* Private */,
       6,    0,   40,    2, 0x08 /* Private */,
       7,    0,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUrl,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QPoint,    5,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void browser::HistoryDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HistoryDialog *_t = static_cast<HistoryDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: _t->open(); break;
        case 3: _t->copy(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (HistoryDialog::*_t)(const QUrl & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HistoryDialog::openUrl)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::HistoryDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_browser__HistoryDialog.data,
      qt_meta_data_browser__HistoryDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::HistoryDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::HistoryDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__HistoryDialog.stringdata0))
        return static_cast<void*>(const_cast< HistoryDialog*>(this));
    if (!strcmp(_clname, "Ui_HistoryDialog"))
        return static_cast< Ui_HistoryDialog*>(const_cast< HistoryDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int browser::HistoryDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void browser::HistoryDialog::openUrl(const QUrl & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
