/****************************************************************************
** Meta object code from reading C++ file 'bookmarks.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/bookmarks.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bookmarks.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_browser__BookmarksManager_t {
    QByteArrayData data[12];
    char stringdata0[131];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__BookmarksManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__BookmarksManager_t qt_meta_stringdata_browser__BookmarksManager = {
    {
QT_MOC_LITERAL(0, 0, 25), // "browser::BookmarksManager"
QT_MOC_LITERAL(1, 26, 10), // "entryAdded"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 13), // "BookmarkNode*"
QT_MOC_LITERAL(4, 52, 4), // "item"
QT_MOC_LITERAL(5, 57, 12), // "entryRemoved"
QT_MOC_LITERAL(6, 70, 6), // "parent"
QT_MOC_LITERAL(7, 77, 3), // "row"
QT_MOC_LITERAL(8, 81, 12), // "entryChanged"
QT_MOC_LITERAL(9, 94, 15), // "importBookmarks"
QT_MOC_LITERAL(10, 110, 15), // "exportBookmarks"
QT_MOC_LITERAL(11, 126, 4) // "save"

    },
    "browser::BookmarksManager\0entryAdded\0"
    "\0BookmarkNode*\0item\0entryRemoved\0"
    "parent\0row\0entryChanged\0importBookmarks\0"
    "exportBookmarks\0save"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__BookmarksManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       5,    3,   47,    2, 0x06 /* Public */,
       8,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,   57,    2, 0x0a /* Public */,
      10,    0,   58,    2, 0x0a /* Public */,
      11,    0,   59,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, 0x80000000 | 3,    6,    7,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void browser::BookmarksManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BookmarksManager *_t = static_cast<BookmarksManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->entryAdded((*reinterpret_cast< BookmarkNode*(*)>(_a[1]))); break;
        case 1: _t->entryRemoved((*reinterpret_cast< BookmarkNode*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< BookmarkNode*(*)>(_a[3]))); break;
        case 2: _t->entryChanged((*reinterpret_cast< BookmarkNode*(*)>(_a[1]))); break;
        case 3: _t->importBookmarks(); break;
        case 4: _t->exportBookmarks(); break;
        case 5: _t->save(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BookmarksManager::*_t)(BookmarkNode * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BookmarksManager::entryAdded)) {
                *result = 0;
            }
        }
        {
            typedef void (BookmarksManager::*_t)(BookmarkNode * , int , BookmarkNode * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BookmarksManager::entryRemoved)) {
                *result = 1;
            }
        }
        {
            typedef void (BookmarksManager::*_t)(BookmarkNode * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BookmarksManager::entryChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject browser::BookmarksManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_browser__BookmarksManager.data,
      qt_meta_data_browser__BookmarksManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::BookmarksManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::BookmarksManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__BookmarksManager.stringdata0))
        return static_cast<void*>(const_cast< BookmarksManager*>(this));
    return QObject::qt_metacast(_clname);
}

int browser::BookmarksManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void browser::BookmarksManager::entryAdded(BookmarkNode * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void browser::BookmarksManager::entryRemoved(BookmarkNode * _t1, int _t2, BookmarkNode * _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void browser::BookmarksManager::entryChanged(BookmarkNode * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_browser__BookmarksModel_t {
    QByteArrayData data[9];
    char stringdata0[92];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__BookmarksModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__BookmarksModel_t qt_meta_stringdata_browser__BookmarksModel = {
    {
QT_MOC_LITERAL(0, 0, 23), // "browser::BookmarksModel"
QT_MOC_LITERAL(1, 24, 10), // "entryAdded"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 13), // "BookmarkNode*"
QT_MOC_LITERAL(4, 50, 4), // "item"
QT_MOC_LITERAL(5, 55, 12), // "entryRemoved"
QT_MOC_LITERAL(6, 68, 6), // "parent"
QT_MOC_LITERAL(7, 75, 3), // "row"
QT_MOC_LITERAL(8, 79, 12) // "entryChanged"

    },
    "browser::BookmarksModel\0entryAdded\0\0"
    "BookmarkNode*\0item\0entryRemoved\0parent\0"
    "row\0entryChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__BookmarksModel[] = {

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
       1,    1,   29,    2, 0x0a /* Public */,
       5,    3,   32,    2, 0x0a /* Public */,
       8,    1,   39,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, 0x80000000 | 3,    6,    7,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

       0        // eod
};

void browser::BookmarksModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BookmarksModel *_t = static_cast<BookmarksModel *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->entryAdded((*reinterpret_cast< BookmarkNode*(*)>(_a[1]))); break;
        case 1: _t->entryRemoved((*reinterpret_cast< BookmarkNode*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< BookmarkNode*(*)>(_a[3]))); break;
        case 2: _t->entryChanged((*reinterpret_cast< BookmarkNode*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject browser::BookmarksModel::staticMetaObject = {
    { &QAbstractItemModel::staticMetaObject, qt_meta_stringdata_browser__BookmarksModel.data,
      qt_meta_data_browser__BookmarksModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::BookmarksModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::BookmarksModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__BookmarksModel.stringdata0))
        return static_cast<void*>(const_cast< BookmarksModel*>(this));
    return QAbstractItemModel::qt_metacast(_clname);
}

int browser::BookmarksModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemModel::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_browser__BookmarksMenu_t {
    QByteArrayData data[6];
    char stringdata0[52];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__BookmarksMenu_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__BookmarksMenu_t qt_meta_stringdata_browser__BookmarksMenu = {
    {
QT_MOC_LITERAL(0, 0, 22), // "browser::BookmarksMenu"
QT_MOC_LITERAL(1, 23, 7), // "openUrl"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 3), // "url"
QT_MOC_LITERAL(4, 36, 9), // "activated"
QT_MOC_LITERAL(5, 46, 5) // "index"

    },
    "browser::BookmarksMenu\0openUrl\0\0url\0"
    "activated\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__BookmarksMenu[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   27,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUrl,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QModelIndex,    5,

       0        // eod
};

void browser::BookmarksMenu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BookmarksMenu *_t = static_cast<BookmarksMenu *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BookmarksMenu::*_t)(const QUrl & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BookmarksMenu::openUrl)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::BookmarksMenu::staticMetaObject = {
    { &ModelMenu::staticMetaObject, qt_meta_stringdata_browser__BookmarksMenu.data,
      qt_meta_data_browser__BookmarksMenu,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::BookmarksMenu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::BookmarksMenu::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__BookmarksMenu.stringdata0))
        return static_cast<void*>(const_cast< BookmarksMenu*>(this));
    return ModelMenu::qt_metacast(_clname);
}

int browser::BookmarksMenu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ModelMenu::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void browser::BookmarksMenu::openUrl(const QUrl & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_browser__AddBookmarkProxyModel_t {
    QByteArrayData data[1];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__AddBookmarkProxyModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__AddBookmarkProxyModel_t qt_meta_stringdata_browser__AddBookmarkProxyModel = {
    {
QT_MOC_LITERAL(0, 0, 30) // "browser::AddBookmarkProxyModel"

    },
    "browser::AddBookmarkProxyModel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__AddBookmarkProxyModel[] = {

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

void browser::AddBookmarkProxyModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject browser::AddBookmarkProxyModel::staticMetaObject = {
    { &QSortFilterProxyModel::staticMetaObject, qt_meta_stringdata_browser__AddBookmarkProxyModel.data,
      qt_meta_data_browser__AddBookmarkProxyModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::AddBookmarkProxyModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::AddBookmarkProxyModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__AddBookmarkProxyModel.stringdata0))
        return static_cast<void*>(const_cast< AddBookmarkProxyModel*>(this));
    return QSortFilterProxyModel::qt_metacast(_clname);
}

int browser::AddBookmarkProxyModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSortFilterProxyModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_browser__AddBookmarkDialog_t {
    QByteArrayData data[3];
    char stringdata0[35];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__AddBookmarkDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__AddBookmarkDialog_t qt_meta_stringdata_browser__AddBookmarkDialog = {
    {
QT_MOC_LITERAL(0, 0, 26), // "browser::AddBookmarkDialog"
QT_MOC_LITERAL(1, 27, 6), // "accept"
QT_MOC_LITERAL(2, 34, 0) // ""

    },
    "browser::AddBookmarkDialog\0accept\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__AddBookmarkDialog[] = {

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

void browser::AddBookmarkDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AddBookmarkDialog *_t = static_cast<AddBookmarkDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->accept(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject browser::AddBookmarkDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_browser__AddBookmarkDialog.data,
      qt_meta_data_browser__AddBookmarkDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::AddBookmarkDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::AddBookmarkDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__AddBookmarkDialog.stringdata0))
        return static_cast<void*>(const_cast< AddBookmarkDialog*>(this));
    if (!strcmp(_clname, "Ui_AddBookmarkDialog"))
        return static_cast< Ui_AddBookmarkDialog*>(const_cast< AddBookmarkDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int browser::AddBookmarkDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_browser__BookmarksDialog_t {
    QByteArrayData data[8];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__BookmarksDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__BookmarksDialog_t qt_meta_stringdata_browser__BookmarksDialog = {
    {
QT_MOC_LITERAL(0, 0, 24), // "browser::BookmarksDialog"
QT_MOC_LITERAL(1, 25, 7), // "openUrl"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 3), // "url"
QT_MOC_LITERAL(4, 38, 26), // "customContextMenuRequested"
QT_MOC_LITERAL(5, 65, 3), // "pos"
QT_MOC_LITERAL(6, 69, 4), // "open"
QT_MOC_LITERAL(7, 74, 9) // "newFolder"

    },
    "browser::BookmarksDialog\0openUrl\0\0url\0"
    "customContextMenuRequested\0pos\0open\0"
    "newFolder"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__BookmarksDialog[] = {

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

void browser::BookmarksDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BookmarksDialog *_t = static_cast<BookmarksDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->customContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 2: _t->open(); break;
        case 3: _t->newFolder(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BookmarksDialog::*_t)(const QUrl & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BookmarksDialog::openUrl)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::BookmarksDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_browser__BookmarksDialog.data,
      qt_meta_data_browser__BookmarksDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::BookmarksDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::BookmarksDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__BookmarksDialog.stringdata0))
        return static_cast<void*>(const_cast< BookmarksDialog*>(this));
    if (!strcmp(_clname, "Ui_BookmarksDialog"))
        return static_cast< Ui_BookmarksDialog*>(const_cast< BookmarksDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int browser::BookmarksDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void browser::BookmarksDialog::openUrl(const QUrl & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_browser__BookmarksToolBar_t {
    QByteArrayData data[10];
    char stringdata0[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__BookmarksToolBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__BookmarksToolBar_t qt_meta_stringdata_browser__BookmarksToolBar = {
    {
QT_MOC_LITERAL(0, 0, 25), // "browser::BookmarksToolBar"
QT_MOC_LITERAL(1, 26, 7), // "openUrl"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 3), // "url"
QT_MOC_LITERAL(4, 39, 9), // "triggered"
QT_MOC_LITERAL(5, 49, 8), // "QAction*"
QT_MOC_LITERAL(6, 58, 6), // "action"
QT_MOC_LITERAL(7, 65, 9), // "activated"
QT_MOC_LITERAL(8, 75, 5), // "index"
QT_MOC_LITERAL(9, 81, 5) // "build"

    },
    "browser::BookmarksToolBar\0openUrl\0\0"
    "url\0triggered\0QAction*\0action\0activated\0"
    "index\0build"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__BookmarksToolBar[] = {

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
       7,    1,   40,    2, 0x08 /* Private */,
       9,    0,   43,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUrl,    3,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::QModelIndex,    8,
    QMetaType::Void,

       0        // eod
};

void browser::BookmarksToolBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BookmarksToolBar *_t = static_cast<BookmarksToolBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->triggered((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 2: _t->activated((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: _t->build(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BookmarksToolBar::*_t)(const QUrl & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BookmarksToolBar::openUrl)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::BookmarksToolBar::staticMetaObject = {
    { &QToolBar::staticMetaObject, qt_meta_stringdata_browser__BookmarksToolBar.data,
      qt_meta_data_browser__BookmarksToolBar,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::BookmarksToolBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::BookmarksToolBar::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__BookmarksToolBar.stringdata0))
        return static_cast<void*>(const_cast< BookmarksToolBar*>(this));
    return QToolBar::qt_metacast(_clname);
}

int browser::BookmarksToolBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void browser::BookmarksToolBar::openUrl(const QUrl & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
