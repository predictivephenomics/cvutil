/****************************************************************************
** Meta object code from reading C++ file 'RoiManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "include/RoiManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RoiManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RoiManager_t {
    QByteArrayData data[21];
    char stringdata0[258];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RoiManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RoiManager_t qt_meta_stringdata_RoiManager = {
    {
QT_MOC_LITERAL(0, 0, 10), // "RoiManager"
QT_MOC_LITERAL(1, 11, 27), // "selectedROIDimensionChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 9), // "deleteAll"
QT_MOC_LITERAL(4, 50, 14), // "deleteSelected"
QT_MOC_LITERAL(5, 65, 9), // "selectROI"
QT_MOC_LITERAL(6, 75, 3), // "idx"
QT_MOC_LITERAL(7, 79, 14), // "clearSelection"
QT_MOC_LITERAL(8, 94, 9), // "selectAll"
QT_MOC_LITERAL(9, 104, 17), // "setROIBorderWidth"
QT_MOC_LITERAL(10, 122, 1), // "s"
QT_MOC_LITERAL(11, 124, 16), // "updateBoundaries"
QT_MOC_LITERAL(12, 141, 1), // "w"
QT_MOC_LITERAL(13, 143, 1), // "h"
QT_MOC_LITERAL(14, 145, 14), // "loadAnnotation"
QT_MOC_LITERAL(15, 160, 31), // "std::vector<QGraphicsRectItem*>"
QT_MOC_LITERAL(16, 192, 8), // "filepath"
QT_MOC_LITERAL(17, 201, 14), // "saveAnnotation"
QT_MOC_LITERAL(18, 216, 17), // "setMessageHandler"
QT_MOC_LITERAL(19, 234, 16), // "QtMessageHandler"
QT_MOC_LITERAL(20, 251, 6) // "logger"

    },
    "RoiManager\0selectedROIDimensionChanged\0"
    "\0deleteAll\0deleteSelected\0selectROI\0"
    "idx\0clearSelection\0selectAll\0"
    "setROIBorderWidth\0s\0updateBoundaries\0"
    "w\0h\0loadAnnotation\0std::vector<QGraphicsRectItem*>\0"
    "filepath\0saveAnnotation\0setMessageHandler\0"
    "QtMessageHandler\0logger"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RoiManager[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   75,    2, 0x0a /* Public */,
       4,    0,   76,    2, 0x0a /* Public */,
       5,    1,   77,    2, 0x0a /* Public */,
       7,    0,   80,    2, 0x0a /* Public */,
       7,    1,   81,    2, 0x0a /* Public */,
       8,    0,   84,    2, 0x0a /* Public */,
       9,    1,   85,    2, 0x0a /* Public */,
      11,    2,   88,    2, 0x0a /* Public */,
      14,    1,   93,    2, 0x0a /* Public */,
      17,    1,   96,    2, 0x0a /* Public */,
      18,    1,   99,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void, QMetaType::QReal, QMetaType::QReal,   12,   13,
    0x80000000 | 15, QMetaType::QString,   16,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, 0x80000000 | 19,   20,

       0        // eod
};

void RoiManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RoiManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->selectedROIDimensionChanged(); break;
        case 1: _t->deleteAll(); break;
        case 2: _t->deleteSelected(); break;
        case 3: _t->selectROI((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->clearSelection(); break;
        case 5: _t->clearSelection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->selectAll(); break;
        case 7: _t->setROIBorderWidth((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: _t->updateBoundaries((*reinterpret_cast< qreal(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 9: { std::vector<QGraphicsRectItem*> _r = _t->loadAnnotation((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< std::vector<QGraphicsRectItem*>*>(_a[0]) = std::move(_r); }  break;
        case 10: _t->saveAnnotation((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 11: _t->setMessageHandler((*reinterpret_cast< QtMessageHandler(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (RoiManager::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RoiManager::selectedROIDimensionChanged)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject RoiManager::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_RoiManager.data,
    qt_meta_data_RoiManager,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RoiManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RoiManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RoiManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RoiManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void RoiManager::selectedROIDimensionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
