/****************************************************************************
** Meta object code from reading C++ file 'FlatControl.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/libraries/FlatControl.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FlatControl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MtComboBox_t {
    QByteArrayData data[1];
    char stringdata0[11];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MtComboBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MtComboBox_t qt_meta_stringdata_MtComboBox = {
    {
QT_MOC_LITERAL(0, 0, 10) // "MtComboBox"

    },
    "MtComboBox"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MtComboBox[] = {

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

void MtComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject MtComboBox::staticMetaObject = {
    { &QComboBox::staticMetaObject, qt_meta_stringdata_MtComboBox.data,
      qt_meta_data_MtComboBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MtComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MtComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MtComboBox.stringdata0))
        return static_cast<void*>(const_cast< MtComboBox*>(this));
    return QComboBox::qt_metacast(_clname);
}

int MtComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_FlatComboBox_t {
    QByteArrayData data[3];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FlatComboBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FlatComboBox_t qt_meta_stringdata_FlatComboBox = {
    {
QT_MOC_LITERAL(0, 0, 12), // "FlatComboBox"
QT_MOC_LITERAL(1, 13, 15), // "aboutToPullDown"
QT_MOC_LITERAL(2, 29, 0) // ""

    },
    "FlatComboBox\0aboutToPullDown\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FlatComboBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void FlatComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FlatComboBox *_t = static_cast<FlatComboBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->aboutToPullDown(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FlatComboBox::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FlatComboBox::aboutToPullDown)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject FlatComboBox::staticMetaObject = {
    { &QComboBox::staticMetaObject, qt_meta_stringdata_FlatComboBox.data,
      qt_meta_data_FlatComboBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FlatComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FlatComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FlatComboBox.stringdata0))
        return static_cast<void*>(const_cast< FlatComboBox*>(this));
    return QComboBox::qt_metacast(_clname);
}

int FlatComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QComboBox::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void FlatComboBox::aboutToPullDown()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_FlatFontComboBox_t {
    QByteArrayData data[3];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FlatFontComboBox_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FlatFontComboBox_t qt_meta_stringdata_FlatFontComboBox = {
    {
QT_MOC_LITERAL(0, 0, 16), // "FlatFontComboBox"
QT_MOC_LITERAL(1, 17, 15), // "aboutToPullDown"
QT_MOC_LITERAL(2, 33, 0) // ""

    },
    "FlatFontComboBox\0aboutToPullDown\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FlatFontComboBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void FlatFontComboBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FlatFontComboBox *_t = static_cast<FlatFontComboBox *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->aboutToPullDown(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FlatFontComboBox::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FlatFontComboBox::aboutToPullDown)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject FlatFontComboBox::staticMetaObject = {
    { &QFontComboBox::staticMetaObject, qt_meta_stringdata_FlatFontComboBox.data,
      qt_meta_data_FlatFontComboBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FlatFontComboBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FlatFontComboBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FlatFontComboBox.stringdata0))
        return static_cast<void*>(const_cast< FlatFontComboBox*>(this));
    return QFontComboBox::qt_metacast(_clname);
}

int FlatFontComboBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFontComboBox::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void FlatFontComboBox::aboutToPullDown()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_FlatToolButton_t {
    QByteArrayData data[2];
    char stringdata0[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FlatToolButton_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FlatToolButton_t qt_meta_stringdata_FlatToolButton = {
    {
QT_MOC_LITERAL(0, 0, 14), // "FlatToolButton"
QT_MOC_LITERAL(1, 15, 9) // "autoRaise"

    },
    "FlatToolButton\0autoRaise"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FlatToolButton[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Bool, 0x00095103,

       0        // eod
};

void FlatToolButton::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        FlatToolButton *_t = static_cast<FlatToolButton *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->autoRaise(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        FlatToolButton *_t = static_cast<FlatToolButton *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setAutoRaise(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject FlatToolButton::staticMetaObject = {
    { &QToolButton::staticMetaObject, qt_meta_stringdata_FlatToolButton.data,
      qt_meta_data_FlatToolButton,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FlatToolButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FlatToolButton::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FlatToolButton.stringdata0))
        return static_cast<void*>(const_cast< FlatToolButton*>(this));
    return QToolButton::qt_metacast(_clname);
}

int FlatToolButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QToolButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
   if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
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
QT_END_MOC_NAMESPACE
