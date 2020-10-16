/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MainWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GraphicsView_t {
    QByteArrayData data[8];
    char stringdata0[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GraphicsView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GraphicsView_t qt_meta_stringdata_GraphicsView = {
    {
QT_MOC_LITERAL(0, 0, 12), // "GraphicsView"
QT_MOC_LITERAL(1, 13, 13), // "showPixelInfo"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 5), // "rowid"
QT_MOC_LITERAL(4, 34, 5), // "colid"
QT_MOC_LITERAL(5, 40, 8), // "openFile"
QT_MOC_LITERAL(6, 49, 8), // "filename"
QT_MOC_LITERAL(7, 58, 18) // "propagatePixelInfo"

    },
    "GraphicsView\0showPixelInfo\0\0rowid\0"
    "colid\0openFile\0filename\0propagatePixelInfo"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GraphicsView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   29,    2, 0x06 /* Public */,
       5,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    2,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::QString,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,

       0        // eod
};

void GraphicsView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GraphicsView *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showPixelInfo((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->openFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->propagatePixelInfo((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (GraphicsView::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphicsView::showPixelInfo)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (GraphicsView::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&GraphicsView::openFile)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GraphicsView::staticMetaObject = { {
    &QGraphicsView::staticMetaObject,
    qt_meta_stringdata_GraphicsView.data,
    qt_meta_data_GraphicsView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GraphicsView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GraphicsView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GraphicsView.stringdata0))
        return static_cast<void*>(this);
    return QGraphicsView::qt_metacast(_clname);
}

int GraphicsView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
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
void GraphicsView::showPixelInfo(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GraphicsView::openFile(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_ProgressReporter_t {
    QByteArrayData data[6];
    char stringdata0[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ProgressReporter_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ProgressReporter_t qt_meta_stringdata_ProgressReporter = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ProgressReporter"
QT_MOC_LITERAL(1, 17, 14), // "UpdateProgress"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 6), // "status"
QT_MOC_LITERAL(4, 40, 8), // "finished"
QT_MOC_LITERAL(5, 49, 14) // "cancelProgress"

    },
    "ProgressReporter\0UpdateProgress\0\0"
    "status\0finished\0cancelProgress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ProgressReporter[] = {

 // content:
       8,       // revision
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
       4,    0,   32,    2, 0x0a /* Public */,
       5,    0,   33,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ProgressReporter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ProgressReporter *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->UpdateProgress((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->finished(); break;
        case 2: _t->cancelProgress(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ProgressReporter::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_ProgressReporter.data,
    qt_meta_data_ProgressReporter,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ProgressReporter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ProgressReporter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ProgressReporter.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ProgressReporter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_HeaderViewWithWordWrap_t {
    QByteArrayData data[1];
    char stringdata0[23];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HeaderViewWithWordWrap_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HeaderViewWithWordWrap_t qt_meta_stringdata_HeaderViewWithWordWrap = {
    {
QT_MOC_LITERAL(0, 0, 22) // "HeaderViewWithWordWrap"

    },
    "HeaderViewWithWordWrap"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HeaderViewWithWordWrap[] = {

 // content:
       8,       // revision
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

void HeaderViewWithWordWrap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject HeaderViewWithWordWrap::staticMetaObject = { {
    &QHeaderView::staticMetaObject,
    qt_meta_stringdata_HeaderViewWithWordWrap.data,
    qt_meta_data_HeaderViewWithWordWrap,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *HeaderViewWithWordWrap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HeaderViewWithWordWrap::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_HeaderViewWithWordWrap.stringdata0))
        return static_cast<void*>(this);
    return QHeaderView::qt_metacast(_clname);
}

int HeaderViewWithWordWrap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QHeaderView::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[33];
    char stringdata0[345];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 12), // "imageChanged"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 4), // "open"
QT_MOC_LITERAL(4, 30, 8), // "filename"
QT_MOC_LITERAL(5, 39, 11), // "refreshlist"
QT_MOC_LITERAL(6, 51, 4), // "save"
QT_MOC_LITERAL(7, 56, 5), // "about"
QT_MOC_LITERAL(8, 62, 7), // "zoomout"
QT_MOC_LITERAL(9, 70, 6), // "zoomin"
QT_MOC_LITERAL(10, 77, 7), // "zoomfit"
QT_MOC_LITERAL(11, 85, 16), // "displayPixelInfo"
QT_MOC_LITERAL(12, 102, 1), // "x"
QT_MOC_LITERAL(13, 104, 1), // "y"
QT_MOC_LITERAL(14, 106, 17), // "displayImageStats"
QT_MOC_LITERAL(15, 124, 19), // "std::vector<QRectF>"
QT_MOC_LITERAL(16, 144, 4), // "rois"
QT_MOC_LITERAL(17, 149, 13), // "batchAnalysis"
QT_MOC_LITERAL(18, 163, 18), // "pixelformatchanged"
QT_MOC_LITERAL(19, 182, 12), // "setROILimits"
QT_MOC_LITERAL(20, 195, 19), // "updateROIDimensions"
QT_MOC_LITERAL(21, 215, 14), // "loadAnnotation"
QT_MOC_LITERAL(22, 230, 17), // "reinitfeatureview"
QT_MOC_LITERAL(23, 248, 15), // "viewcontextmenu"
QT_MOC_LITERAL(24, 264, 3), // "pos"
QT_MOC_LITERAL(25, 268, 23), // "watcherDirectoryChanged"
QT_MOC_LITERAL(26, 292, 4), // "path"
QT_MOC_LITERAL(27, 297, 11), // "setRotation"
QT_MOC_LITERAL(28, 309, 5), // "angle"
QT_MOC_LITERAL(29, 315, 10), // "logUpdated"
QT_MOC_LITERAL(30, 326, 8), // "setImage"
QT_MOC_LITERAL(31, 335, 7), // "cv::Mat"
QT_MOC_LITERAL(32, 343, 1) // "m"

    },
    "MainWindow\0imageChanged\0\0open\0filename\0"
    "refreshlist\0save\0about\0zoomout\0zoomin\0"
    "zoomfit\0displayPixelInfo\0x\0y\0"
    "displayImageStats\0std::vector<QRectF>\0"
    "rois\0batchAnalysis\0pixelformatchanged\0"
    "setROILimits\0updateROIDimensions\0"
    "loadAnnotation\0reinitfeatureview\0"
    "viewcontextmenu\0pos\0watcherDirectoryChanged\0"
    "path\0setRotation\0angle\0logUpdated\0"
    "setImage\0cv::Mat\0m"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  124,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,  125,    2, 0x08 /* Private */,
       3,    2,  126,    2, 0x08 /* Private */,
       3,    1,  131,    2, 0x28 /* Private | MethodCloned */,
       6,    0,  134,    2, 0x08 /* Private */,
       7,    0,  135,    2, 0x08 /* Private */,
       8,    0,  136,    2, 0x08 /* Private */,
       9,    0,  137,    2, 0x08 /* Private */,
      10,    0,  138,    2, 0x08 /* Private */,
      11,    2,  139,    2, 0x08 /* Private */,
      14,    1,  144,    2, 0x08 /* Private */,
      17,    0,  147,    2, 0x08 /* Private */,
      18,    0,  148,    2, 0x08 /* Private */,
      19,    0,  149,    2, 0x08 /* Private */,
      20,    0,  150,    2, 0x08 /* Private */,
      21,    0,  151,    2, 0x08 /* Private */,
      22,    0,  152,    2, 0x08 /* Private */,
      23,    1,  153,    2, 0x08 /* Private */,
      25,    1,  156,    2, 0x08 /* Private */,
      27,    1,  159,    2, 0x08 /* Private */,
      29,    0,  162,    2, 0x08 /* Private */,
      30,    1,  163,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    4,    5,
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   12,   13,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   24,
    QMetaType::Void, QMetaType::QString,   26,
    QMetaType::Void, QMetaType::Int,   28,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 31,   32,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->imageChanged(); break;
        case 1: _t->open(); break;
        case 2: _t->open((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->open((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->save(); break;
        case 5: _t->about(); break;
        case 6: _t->zoomout(); break;
        case 7: _t->zoomin(); break;
        case 8: _t->zoomfit(); break;
        case 9: _t->displayPixelInfo((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: _t->displayImageStats((*reinterpret_cast< std::vector<QRectF>(*)>(_a[1]))); break;
        case 11: _t->batchAnalysis(); break;
        case 12: _t->pixelformatchanged(); break;
        case 13: _t->setROILimits(); break;
        case 14: _t->updateROIDimensions(); break;
        case 15: _t->loadAnnotation(); break;
        case 16: _t->reinitfeatureview(); break;
        case 17: _t->viewcontextmenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 18: _t->watcherDirectoryChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->setRotation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->logUpdated(); break;
        case 21: _t->setImage((*reinterpret_cast< cv::Mat(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::imageChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "cvutilWindow"))
        return static_cast< cvutilWindow*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::imageChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
