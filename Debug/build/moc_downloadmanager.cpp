/****************************************************************************
** Meta object code from reading C++ file 'downloadmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/downloadmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'downloadmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_browser__DownloadWidget_t {
    QByteArrayData data[9];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__DownloadWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__DownloadWidget_t qt_meta_stringdata_browser__DownloadWidget = {
    {
QT_MOC_LITERAL(0, 0, 23), // "browser::DownloadWidget"
QT_MOC_LITERAL(1, 24, 13), // "statusChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 4), // "stop"
QT_MOC_LITERAL(4, 44, 4), // "open"
QT_MOC_LITERAL(5, 49, 16), // "downloadProgress"
QT_MOC_LITERAL(6, 66, 13), // "bytesReceived"
QT_MOC_LITERAL(7, 80, 10), // "bytesTotal"
QT_MOC_LITERAL(8, 91, 8) // "finished"

    },
    "browser::DownloadWidget\0statusChanged\0"
    "\0stop\0open\0downloadProgress\0bytesReceived\0"
    "bytesTotal\0finished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__DownloadWidget[] = {

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
       1,    0,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   40,    2, 0x08 /* Private */,
       4,    0,   41,    2, 0x08 /* Private */,
       5,    2,   42,    2, 0x08 /* Private */,
       8,    0,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,    6,    7,
    QMetaType::Void,

       0        // eod
};

void browser::DownloadWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DownloadWidget *_t = static_cast<DownloadWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->statusChanged(); break;
        case 1: _t->stop(); break;
        case 2: _t->open(); break;
        case 3: _t->downloadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 4: _t->finished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DownloadWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DownloadWidget::statusChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::DownloadWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_browser__DownloadWidget.data,
      qt_meta_data_browser__DownloadWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::DownloadWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::DownloadWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__DownloadWidget.stringdata0))
        return static_cast<void*>(const_cast< DownloadWidget*>(this));
    if (!strcmp(_clname, "Ui_DownloadItem"))
        return static_cast< Ui_DownloadItem*>(const_cast< DownloadWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int browser::DownloadWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void browser::DownloadWidget::statusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_browser__DownloadManager_t {
    QByteArrayData data[12];
    char stringdata0[139];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__DownloadManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__DownloadManager_t qt_meta_stringdata_browser__DownloadManager = {
    {
QT_MOC_LITERAL(0, 0, 24), // "browser::DownloadManager"
QT_MOC_LITERAL(1, 25, 8), // "download"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 23), // "QWebEngineDownloadItem*"
QT_MOC_LITERAL(4, 59, 7), // "cleanup"
QT_MOC_LITERAL(5, 67, 4), // "save"
QT_MOC_LITERAL(6, 72, 9), // "updateRow"
QT_MOC_LITERAL(7, 82, 12), // "removePolicy"
QT_MOC_LITERAL(8, 95, 12), // "RemovePolicy"
QT_MOC_LITERAL(9, 108, 5), // "Never"
QT_MOC_LITERAL(10, 114, 4), // "Exit"
QT_MOC_LITERAL(11, 119, 19) // "SuccessFullDownload"

    },
    "browser::DownloadManager\0download\0\0"
    "QWebEngineDownloadItem*\0cleanup\0save\0"
    "updateRow\0removePolicy\0RemovePolicy\0"
    "Never\0Exit\0SuccessFullDownload"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__DownloadManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       1,   40, // properties
       1,   43, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x0a /* Public */,
       4,    0,   37,    2, 0x0a /* Public */,
       5,    0,   38,    2, 0x08 /* Private */,
       6,    0,   39,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    1,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       7, 0x80000000 | 8, 0x0009510b,

 // enums: name, flags, count, data
       8, 0x0,    3,   47,

 // enum data: key, value
       9, uint(browser::DownloadManager::Never),
      10, uint(browser::DownloadManager::Exit),
      11, uint(browser::DownloadManager::SuccessFullDownload),

       0        // eod
};

void browser::DownloadManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DownloadManager *_t = static_cast<DownloadManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->download((*reinterpret_cast< QWebEngineDownloadItem*(*)>(_a[1]))); break;
        case 1: _t->cleanup(); break;
        case 2: _t->save(); break;
        case 3: _t->updateRow(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QWebEngineDownloadItem* >(); break;
            }
            break;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        DownloadManager *_t = static_cast<DownloadManager *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< RemovePolicy*>(_v) = _t->removePolicy(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        DownloadManager *_t = static_cast<DownloadManager *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setRemovePolicy(*reinterpret_cast< RemovePolicy*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject browser::DownloadManager::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_browser__DownloadManager.data,
      qt_meta_data_browser__DownloadManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::DownloadManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::DownloadManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__DownloadManager.stringdata0))
        return static_cast<void*>(const_cast< DownloadManager*>(this));
    if (!strcmp(_clname, "Ui_DownloadDialog"))
        return static_cast< Ui_DownloadDialog*>(const_cast< DownloadManager*>(this));
    return QDialog::qt_metacast(_clname);
}

int browser::DownloadManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
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
struct qt_meta_stringdata_browser__DownloadModel_t {
    QByteArrayData data[1];
    char stringdata0[23];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__DownloadModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__DownloadModel_t qt_meta_stringdata_browser__DownloadModel = {
    {
QT_MOC_LITERAL(0, 0, 22) // "browser::DownloadModel"

    },
    "browser::DownloadModel"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__DownloadModel[] = {

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

void browser::DownloadModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject browser::DownloadModel::staticMetaObject = {
    { &QAbstractListModel::staticMetaObject, qt_meta_stringdata_browser__DownloadModel.data,
      qt_meta_data_browser__DownloadModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::DownloadModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::DownloadModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__DownloadModel.stringdata0))
        return static_cast<void*>(const_cast< DownloadModel*>(this));
    return QAbstractListModel::qt_metacast(_clname);
}

int browser::DownloadModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
