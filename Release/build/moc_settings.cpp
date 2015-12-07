/****************************************************************************
** Meta object code from reading C++ file 'settings.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/settings.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_browser__SettingsDialog_t {
    QByteArrayData data[10];
    char stringdata0[145];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__SettingsDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__SettingsDialog_t qt_meta_stringdata_browser__SettingsDialog = {
    {
QT_MOC_LITERAL(0, 0, 23), // "browser::SettingsDialog"
QT_MOC_LITERAL(1, 24, 12), // "loadDefaults"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 16), // "loadFromSettings"
QT_MOC_LITERAL(4, 55, 14), // "saveToSettings"
QT_MOC_LITERAL(5, 70, 20), // "setHomeToCurrentPage"
QT_MOC_LITERAL(6, 91, 11), // "showCookies"
QT_MOC_LITERAL(7, 103, 14), // "showExceptions"
QT_MOC_LITERAL(8, 118, 10), // "chooseFont"
QT_MOC_LITERAL(9, 129, 15) // "chooseFixedFont"

    },
    "browser::SettingsDialog\0loadDefaults\0"
    "\0loadFromSettings\0saveToSettings\0"
    "setHomeToCurrentPage\0showCookies\0"
    "showExceptions\0chooseFont\0chooseFixedFont"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__SettingsDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    0,   55,    2, 0x08 /* Private */,
       4,    0,   56,    2, 0x08 /* Private */,
       5,    0,   57,    2, 0x08 /* Private */,
       6,    0,   58,    2, 0x08 /* Private */,
       7,    0,   59,    2, 0x08 /* Private */,
       8,    0,   60,    2, 0x08 /* Private */,
       9,    0,   61,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void browser::SettingsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SettingsDialog *_t = static_cast<SettingsDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loadDefaults(); break;
        case 1: _t->loadFromSettings(); break;
        case 2: _t->saveToSettings(); break;
        case 3: _t->setHomeToCurrentPage(); break;
        case 4: _t->showCookies(); break;
        case 5: _t->showExceptions(); break;
        case 6: _t->chooseFont(); break;
        case 7: _t->chooseFixedFont(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject browser::SettingsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_browser__SettingsDialog.data,
      qt_meta_data_browser__SettingsDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::SettingsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::SettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__SettingsDialog.stringdata0))
        return static_cast<void*>(const_cast< SettingsDialog*>(this));
    if (!strcmp(_clname, "Ui_Settings"))
        return static_cast< Ui_Settings*>(const_cast< SettingsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int browser::SettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
    return _id;
}
QT_END_MOC_NAMESPACE
