/****************************************************************************
** Meta object code from reading C++ file 'browsertabmanager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/views/browser/browsertabmanager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'browsertabmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_browser__TabBar_t {
    QByteArrayData data[15];
    char stringdata0[168];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__TabBar_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__TabBar_t qt_meta_stringdata_browser__TabBar = {
    {
QT_MOC_LITERAL(0, 0, 15), // "browser::TabBar"
QT_MOC_LITERAL(1, 16, 6), // "newTab"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 8), // "cloneTab"
QT_MOC_LITERAL(4, 33, 5), // "index"
QT_MOC_LITERAL(5, 39, 8), // "closeTab"
QT_MOC_LITERAL(6, 48, 14), // "closeOtherTabs"
QT_MOC_LITERAL(7, 63, 9), // "reloadTab"
QT_MOC_LITERAL(8, 73, 13), // "reloadAllTabs"
QT_MOC_LITERAL(9, 87, 16), // "tabMoveRequested"
QT_MOC_LITERAL(10, 104, 9), // "fromIndex"
QT_MOC_LITERAL(11, 114, 7), // "toIndex"
QT_MOC_LITERAL(12, 122, 15), // "selectTabAction"
QT_MOC_LITERAL(13, 138, 20), // "contextMenuRequested"
QT_MOC_LITERAL(14, 159, 8) // "position"

    },
    "browser::TabBar\0newTab\0\0cloneTab\0index\0"
    "closeTab\0closeOtherTabs\0reloadTab\0"
    "reloadAllTabs\0tabMoveRequested\0fromIndex\0"
    "toIndex\0selectTabAction\0contextMenuRequested\0"
    "position"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__TabBar[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x06 /* Public */,
       3,    1,   80,    2, 0x06 /* Public */,
       5,    1,   83,    2, 0x06 /* Public */,
       6,    1,   86,    2, 0x06 /* Public */,
       7,    1,   89,    2, 0x06 /* Public */,
       8,    0,   92,    2, 0x06 /* Public */,
       9,    2,   93,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    0,   98,    2, 0x08 /* Private */,
       3,    0,   99,    2, 0x08 /* Private */,
       5,    0,  100,    2, 0x08 /* Private */,
       6,    0,  101,    2, 0x08 /* Private */,
       7,    0,  102,    2, 0x08 /* Private */,
      13,    1,  103,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void, QMetaType::Int,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   10,   11,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   14,

       0        // eod
};

void browser::TabBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TabBar *_t = static_cast<TabBar *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newTab(); break;
        case 1: _t->cloneTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->closeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->closeOtherTabs((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->reloadTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->reloadAllTabs(); break;
        case 6: _t->tabMoveRequested((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->selectTabAction(); break;
        case 8: _t->cloneTab(); break;
        case 9: _t->closeTab(); break;
        case 10: _t->closeOtherTabs(); break;
        case 11: _t->reloadTab(); break;
        case 12: _t->contextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TabBar::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabBar::newTab)) {
                *result = 0;
            }
        }
        {
            typedef void (TabBar::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabBar::cloneTab)) {
                *result = 1;
            }
        }
        {
            typedef void (TabBar::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabBar::closeTab)) {
                *result = 2;
            }
        }
        {
            typedef void (TabBar::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabBar::closeOtherTabs)) {
                *result = 3;
            }
        }
        {
            typedef void (TabBar::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabBar::reloadTab)) {
                *result = 4;
            }
        }
        {
            typedef void (TabBar::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabBar::reloadAllTabs)) {
                *result = 5;
            }
        }
        {
            typedef void (TabBar::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabBar::tabMoveRequested)) {
                *result = 6;
            }
        }
    }
}

const QMetaObject browser::TabBar::staticMetaObject = {
    { &QTabBar::staticMetaObject, qt_meta_stringdata_browser__TabBar.data,
      qt_meta_data_browser__TabBar,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::TabBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::TabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__TabBar.stringdata0))
        return static_cast<void*>(const_cast< TabBar*>(this));
    return QTabBar::qt_metacast(_clname);
}

int browser::TabBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void browser::TabBar::newTab()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void browser::TabBar::cloneTab(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void browser::TabBar::closeTab(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void browser::TabBar::closeOtherTabs(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void browser::TabBar::reloadTab(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void browser::TabBar::reloadAllTabs()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void browser::TabBar::tabMoveRequested(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
struct qt_meta_stringdata_browser__WebActionMapper_t {
    QByteArrayData data[6];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__WebActionMapper_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__WebActionMapper_t qt_meta_stringdata_browser__WebActionMapper = {
    {
QT_MOC_LITERAL(0, 0, 24), // "browser::WebActionMapper"
QT_MOC_LITERAL(1, 25, 13), // "rootTriggered"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 12), // "childChanged"
QT_MOC_LITERAL(4, 53, 13), // "rootDestroyed"
QT_MOC_LITERAL(5, 67, 16) // "currentDestroyed"

    },
    "browser::WebActionMapper\0rootTriggered\0"
    "\0childChanged\0rootDestroyed\0"
    "currentDestroyed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__WebActionMapper[] = {

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
       3,    0,   35,    2, 0x08 /* Private */,
       4,    0,   36,    2, 0x08 /* Private */,
       5,    0,   37,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void browser::WebActionMapper::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WebActionMapper *_t = static_cast<WebActionMapper *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->rootTriggered(); break;
        case 1: _t->childChanged(); break;
        case 2: _t->rootDestroyed(); break;
        case 3: _t->currentDestroyed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject browser::WebActionMapper::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_browser__WebActionMapper.data,
      qt_meta_data_browser__WebActionMapper,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::WebActionMapper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::WebActionMapper::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__WebActionMapper.stringdata0))
        return static_cast<void*>(const_cast< WebActionMapper*>(this));
    return QObject::qt_metacast(_clname);
}

int browser::WebActionMapper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_browser__TabManager_t {
    QByteArrayData data[56];
    char stringdata0[806];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__TabManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__TabManager_t qt_meta_stringdata_browser__TabManager = {
    {
QT_MOC_LITERAL(0, 0, 19), // "browser::TabManager"
QT_MOC_LITERAL(1, 20, 8), // "loadPage"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 3), // "url"
QT_MOC_LITERAL(4, 34, 11), // "tabsChanged"
QT_MOC_LITERAL(5, 46, 13), // "lastTabClosed"
QT_MOC_LITERAL(6, 60, 15), // "setCurrentTitle"
QT_MOC_LITERAL(7, 76, 20), // "showStatusBarMessage"
QT_MOC_LITERAL(8, 97, 7), // "message"
QT_MOC_LITERAL(9, 105, 11), // "linkHovered"
QT_MOC_LITERAL(10, 117, 4), // "link"
QT_MOC_LITERAL(11, 122, 12), // "loadProgress"
QT_MOC_LITERAL(12, 135, 8), // "progress"
QT_MOC_LITERAL(13, 144, 23), // "geometryChangeRequested"
QT_MOC_LITERAL(14, 168, 8), // "geometry"
QT_MOC_LITERAL(15, 177, 32), // "menuBarVisibilityChangeRequested"
QT_MOC_LITERAL(16, 210, 7), // "visible"
QT_MOC_LITERAL(17, 218, 34), // "statusBarVisibilityChangeRequ..."
QT_MOC_LITERAL(18, 253, 32), // "toolBarVisibilityChangeRequested"
QT_MOC_LITERAL(19, 286, 19), // "loadUrlInCurrentTab"
QT_MOC_LITERAL(20, 306, 10), // "newTabVoid"
QT_MOC_LITERAL(21, 317, 10), // "newTabFull"
QT_MOC_LITERAL(22, 328, 9), // "PageView*"
QT_MOC_LITERAL(23, 338, 7), // "Record*"
QT_MOC_LITERAL(24, 346, 6), // "record"
QT_MOC_LITERAL(25, 353, 22), // "RecordTableController*"
QT_MOC_LITERAL(26, 376, 21), // "recordtablecontroller"
QT_MOC_LITERAL(27, 398, 11), // "makeCurrent"
QT_MOC_LITERAL(28, 410, 6), // "newTab"
QT_MOC_LITERAL(29, 417, 8), // "cloneTab"
QT_MOC_LITERAL(30, 426, 5), // "index"
QT_MOC_LITERAL(31, 432, 8), // "closeTab"
QT_MOC_LITERAL(32, 441, 14), // "closeOtherTabs"
QT_MOC_LITERAL(33, 456, 9), // "reloadTab"
QT_MOC_LITERAL(34, 466, 13), // "reloadAllTabs"
QT_MOC_LITERAL(35, 480, 7), // "nextTab"
QT_MOC_LITERAL(36, 488, 11), // "previousTab"
QT_MOC_LITERAL(37, 500, 14), // "currentChanged"
QT_MOC_LITERAL(38, 515, 25), // "aboutToShowRecentTabsMenu"
QT_MOC_LITERAL(39, 541, 32), // "aboutToShowRecentTriggeredAction"
QT_MOC_LITERAL(40, 574, 8), // "QAction*"
QT_MOC_LITERAL(41, 583, 6), // "action"
QT_MOC_LITERAL(42, 590, 17), // "downloadRequested"
QT_MOC_LITERAL(43, 608, 23), // "QWebEngineDownloadItem*"
QT_MOC_LITERAL(44, 632, 8), // "download"
QT_MOC_LITERAL(45, 641, 18), // "webViewLoadStarted"
QT_MOC_LITERAL(46, 660, 18), // "webViewIconChanged"
QT_MOC_LITERAL(47, 679, 19), // "webViewTitleChanged"
QT_MOC_LITERAL(48, 699, 5), // "title"
QT_MOC_LITERAL(49, 705, 17), // "webViewUrlChanged"
QT_MOC_LITERAL(50, 723, 21), // "lineEditReturnPressed"
QT_MOC_LITERAL(51, 745, 20), // "windowCloseRequested"
QT_MOC_LITERAL(52, 766, 7), // "moveTab"
QT_MOC_LITERAL(53, 774, 9), // "fromIndex"
QT_MOC_LITERAL(54, 784, 7), // "toIndex"
QT_MOC_LITERAL(55, 792, 13) // "onTabsChanged"

    },
    "browser::TabManager\0loadPage\0\0url\0"
    "tabsChanged\0lastTabClosed\0setCurrentTitle\0"
    "showStatusBarMessage\0message\0linkHovered\0"
    "link\0loadProgress\0progress\0"
    "geometryChangeRequested\0geometry\0"
    "menuBarVisibilityChangeRequested\0"
    "visible\0statusBarVisibilityChangeRequested\0"
    "toolBarVisibilityChangeRequested\0"
    "loadUrlInCurrentTab\0newTabVoid\0"
    "newTabFull\0PageView*\0Record*\0record\0"
    "RecordTableController*\0recordtablecontroller\0"
    "makeCurrent\0newTab\0cloneTab\0index\0"
    "closeTab\0closeOtherTabs\0reloadTab\0"
    "reloadAllTabs\0nextTab\0previousTab\0"
    "currentChanged\0aboutToShowRecentTabsMenu\0"
    "aboutToShowRecentTriggeredAction\0"
    "QAction*\0action\0downloadRequested\0"
    "QWebEngineDownloadItem*\0download\0"
    "webViewLoadStarted\0webViewIconChanged\0"
    "webViewTitleChanged\0title\0webViewUrlChanged\0"
    "lineEditReturnPressed\0windowCloseRequested\0"
    "moveTab\0fromIndex\0toIndex\0onTabsChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__TabManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      40,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  214,    2, 0x06 /* Public */,
       4,    0,  217,    2, 0x06 /* Public */,
       5,    0,  218,    2, 0x06 /* Public */,
       6,    1,  219,    2, 0x06 /* Public */,
       7,    1,  222,    2, 0x06 /* Public */,
       9,    1,  225,    2, 0x06 /* Public */,
      11,    1,  228,    2, 0x06 /* Public */,
      13,    1,  231,    2, 0x06 /* Public */,
      15,    1,  234,    2, 0x06 /* Public */,
      17,    1,  237,    2, 0x06 /* Public */,
      18,    1,  240,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      19,    1,  243,    2, 0x0a /* Public */,
      20,    0,  246,    2, 0x0a /* Public */,
      21,    3,  247,    2, 0x0a /* Public */,
      21,    2,  254,    2, 0x2a /* Public | MethodCloned */,
      21,    1,  259,    2, 0x2a /* Public | MethodCloned */,
      28,    1,  262,    2, 0x0a /* Public */,
      28,    0,  265,    2, 0x2a /* Public | MethodCloned */,
      29,    1,  266,    2, 0x0a /* Public */,
      29,    0,  269,    2, 0x2a /* Public | MethodCloned */,
      31,    1,  270,    2, 0x0a /* Public */,
      31,    0,  273,    2, 0x2a /* Public | MethodCloned */,
      32,    1,  274,    2, 0x0a /* Public */,
      33,    1,  277,    2, 0x0a /* Public */,
      33,    0,  280,    2, 0x2a /* Public | MethodCloned */,
      34,    0,  281,    2, 0x0a /* Public */,
      35,    0,  282,    2, 0x0a /* Public */,
      36,    0,  283,    2, 0x0a /* Public */,
      37,    1,  284,    2, 0x08 /* Private */,
      38,    0,  287,    2, 0x08 /* Private */,
      39,    1,  288,    2, 0x08 /* Private */,
      42,    1,  291,    2, 0x08 /* Private */,
      45,    0,  294,    2, 0x08 /* Private */,
      46,    0,  295,    2, 0x08 /* Private */,
      47,    1,  296,    2, 0x08 /* Private */,
      49,    1,  299,    2, 0x08 /* Private */,
      50,    0,  302,    2, 0x08 /* Private */,
      51,    0,  303,    2, 0x08 /* Private */,
      52,    2,  304,    2, 0x08 /* Private */,
      55,    0,  309,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::QRect,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   16,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl,    3,
    QMetaType::Void,
    0x80000000 | 22, 0x80000000 | 23, 0x80000000 | 25, QMetaType::Bool,   24,   26,   27,
    0x80000000 | 22, 0x80000000 | 23, 0x80000000 | 25,   24,   26,
    0x80000000 | 22, 0x80000000 | 23,   24,
    QMetaType::Void, QMetaType::Bool,   27,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   30,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 40,   41,
    QMetaType::Void, 0x80000000 | 43,   44,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   48,
    QMetaType::Void, QMetaType::QUrl,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   53,   54,
    QMetaType::Void,

       0        // eod
};

void browser::TabManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TabManager *_t = static_cast<TabManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->loadPage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->tabsChanged(); break;
        case 2: _t->lastTabClosed(); break;
        case 3: _t->setCurrentTitle((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->showStatusBarMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->linkHovered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->loadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->geometryChangeRequested((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        case 8: _t->menuBarVisibilityChangeRequested((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->statusBarVisibilityChangeRequested((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->toolBarVisibilityChangeRequested((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->loadUrlInCurrentTab((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 12: _t->newTabVoid(); break;
        case 13: { PageView* _r = _t->newTabFull((*reinterpret_cast< Record*(*)>(_a[1])),(*reinterpret_cast< RecordTableController*(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< PageView**>(_a[0]) = _r; }  break;
        case 14: { PageView* _r = _t->newTabFull((*reinterpret_cast< Record*(*)>(_a[1])),(*reinterpret_cast< RecordTableController*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< PageView**>(_a[0]) = _r; }  break;
        case 15: { PageView* _r = _t->newTabFull((*reinterpret_cast< Record*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< PageView**>(_a[0]) = _r; }  break;
        case 16: _t->newTab((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->newTab(); break;
        case 18: _t->cloneTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->cloneTab(); break;
        case 20: _t->closeTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->closeTab(); break;
        case 22: _t->closeOtherTabs((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 23: _t->reloadTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->reloadTab(); break;
        case 25: _t->reloadAllTabs(); break;
        case 26: _t->nextTab(); break;
        case 27: _t->previousTab(); break;
        case 28: _t->currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 29: _t->aboutToShowRecentTabsMenu(); break;
        case 30: _t->aboutToShowRecentTriggeredAction((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 31: _t->downloadRequested((*reinterpret_cast< QWebEngineDownloadItem*(*)>(_a[1]))); break;
        case 32: _t->webViewLoadStarted(); break;
        case 33: _t->webViewIconChanged(); break;
        case 34: _t->webViewTitleChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 35: _t->webViewUrlChanged((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 36: _t->lineEditReturnPressed(); break;
        case 37: _t->windowCloseRequested(); break;
        case 38: _t->moveTab((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 39: _t->onTabsChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< RecordTableController* >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< RecordTableController* >(); break;
            }
            break;
        case 30:
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
            typedef void (TabManager::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::loadPage)) {
                *result = 0;
            }
        }
        {
            typedef void (TabManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::tabsChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (TabManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::lastTabClosed)) {
                *result = 2;
            }
        }
        {
            typedef void (TabManager::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::setCurrentTitle)) {
                *result = 3;
            }
        }
        {
            typedef void (TabManager::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::showStatusBarMessage)) {
                *result = 4;
            }
        }
        {
            typedef void (TabManager::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::linkHovered)) {
                *result = 5;
            }
        }
        {
            typedef void (TabManager::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::loadProgress)) {
                *result = 6;
            }
        }
        {
            typedef void (TabManager::*_t)(const QRect & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::geometryChangeRequested)) {
                *result = 7;
            }
        }
        {
            typedef void (TabManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::menuBarVisibilityChangeRequested)) {
                *result = 8;
            }
        }
        {
            typedef void (TabManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::statusBarVisibilityChangeRequested)) {
                *result = 9;
            }
        }
        {
            typedef void (TabManager::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TabManager::toolBarVisibilityChangeRequested)) {
                *result = 10;
            }
        }
    }
}

const QMetaObject browser::TabManager::staticMetaObject = {
    { &QTabWidget::staticMetaObject, qt_meta_stringdata_browser__TabManager.data,
      qt_meta_data_browser__TabManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::TabManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::TabManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__TabManager.stringdata0))
        return static_cast<void*>(const_cast< TabManager*>(this));
    return QTabWidget::qt_metacast(_clname);
}

int browser::TabManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 40)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 40;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 40)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 40;
    }
    return _id;
}

// SIGNAL 0
void browser::TabManager::loadPage(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void browser::TabManager::tabsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void browser::TabManager::lastTabClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void browser::TabManager::setCurrentTitle(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void browser::TabManager::showStatusBarMessage(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void browser::TabManager::linkHovered(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void browser::TabManager::loadProgress(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void browser::TabManager::geometryChangeRequested(const QRect & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void browser::TabManager::menuBarVisibilityChangeRequested(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void browser::TabManager::statusBarVisibilityChangeRequested(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}

// SIGNAL 10
void browser::TabManager::toolBarVisibilityChangeRequested(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
struct qt_meta_stringdata_browser__PopupWindow_t {
    QByteArrayData data[6];
    char stringdata0[60];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_browser__PopupWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_browser__PopupWindow_t qt_meta_stringdata_browser__PopupWindow = {
    {
QT_MOC_LITERAL(0, 0, 20), // "browser::PopupWindow"
QT_MOC_LITERAL(1, 21, 6), // "setUrl"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 3), // "url"
QT_MOC_LITERAL(4, 33, 14), // "adjustGeometry"
QT_MOC_LITERAL(5, 48, 11) // "newGeometry"

    },
    "browser::PopupWindow\0setUrl\0\0url\0"
    "adjustGeometry\0newGeometry"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_browser__PopupWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x08 /* Private */,
       4,    1,   27,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QUrl,    3,
    QMetaType::Void, QMetaType::QRect,    5,

       0        // eod
};

void browser::PopupWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PopupWindow *_t = static_cast<PopupWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->setUrl((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->adjustGeometry((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject browser::PopupWindow::staticMetaObject = {
    { &TabManager::staticMetaObject, qt_meta_stringdata_browser__PopupWindow.data,
      qt_meta_data_browser__PopupWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *browser::PopupWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *browser::PopupWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_browser__PopupWindow.stringdata0))
        return static_cast<void*>(const_cast< PopupWindow*>(this));
    return TabManager::qt_metacast(_clname);
}

int browser::PopupWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TabManager::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
