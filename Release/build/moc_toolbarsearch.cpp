/****************************************************************************
** Meta object code from reading C++ file 'toolbarsearch.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/toolbarsearch.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'toolbarsearch.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_browser__ToolbarSearch_t {
    QByteArrayData data[11];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__ToolbarSearch_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__ToolbarSearch_t qt_meta_stringdata_browser__ToolbarSearch = {
    {
QT_MOC_LITERAL(0, 0, 22), // "browser::ToolbarSearch"
QT_MOC_LITERAL(1, 23, 6), // "search"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 3), // "url"
QT_MOC_LITERAL(4, 35, 5), // "clear"
QT_MOC_LITERAL(5, 41, 9), // "searchNow"
QT_MOC_LITERAL(6, 51, 4), // "save"
QT_MOC_LITERAL(7, 56, 15), // "aboutToShowMenu"
QT_MOC_LITERAL(8, 72, 19), // "triggeredMenuAction"
QT_MOC_LITERAL(9, 92, 8), // "QAction*"
QT_MOC_LITERAL(10, 101, 6) // "action"

    },
    "browser::ToolbarSearch\0search\0\0url\0"
    "clear\0searchNow\0save\0aboutToShowMenu\0"
    "triggeredMenuAction\0QAction*\0action"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__ToolbarSearch[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   47,    2, 0x0a /* Public */,
       5,    0,   48,    2, 0x0a /* Public */,
       6,    0,   49,    2, 0x08 /* Private */,
       7,    0,   50,    2, 0x08 /* Private */,
       8,    1,   51,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUrl,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

void browser::ToolbarSearch::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ToolbarSearch *_t = static_cast<ToolbarSearch *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->search((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->clear(); break;
        case 2: _t->searchNow(); break;
        case 3: _t->save(); break;
        case 4: _t->aboutToShowMenu(); break;
        case 5: _t->triggeredMenuAction((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ToolbarSearch::*_t)(const QUrl & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ToolbarSearch::search)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject browser::ToolbarSearch::staticMetaObject = {
    { &SearchLineEdit::staticMetaObject, qt_meta_stringdata_browser__ToolbarSearch.data,
      qt_meta_data_browser__ToolbarSearch,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::ToolbarSearch::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::ToolbarSearch::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__ToolbarSearch.stringdata0))
        return static_cast<void*>(const_cast< ToolbarSearch*>(this));
    return SearchLineEdit::qt_metacast(_clname);
}

int browser::ToolbarSearch::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SearchLineEdit::qt_metacall(_c, _id, _a);
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
void browser::ToolbarSearch::search(const QUrl & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
