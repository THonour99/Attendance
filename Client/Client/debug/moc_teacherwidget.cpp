/****************************************************************************
** Meta object code from reading C++ file 'teacherwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../teacherwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'teacherwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TeacherWidget_t {
    QByteArrayData data[45];
    char stringdata0[981];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TeacherWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TeacherWidget_t qt_meta_stringdata_TeacherWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "TeacherWidget"
QT_MOC_LITERAL(1, 14, 21), // "onClassesDataReceived"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 14), // "QNetworkReply*"
QT_MOC_LITERAL(4, 52, 5), // "reply"
QT_MOC_LITERAL(5, 58, 22), // "onStudentsDataReceived"
QT_MOC_LITERAL(6, 81, 15), // "onClassSelected"
QT_MOC_LITERAL(7, 97, 11), // "QModelIndex"
QT_MOC_LITERAL(8, 109, 5), // "index"
QT_MOC_LITERAL(9, 115, 17), // "onAddClassClicked"
QT_MOC_LITERAL(10, 133, 26), // "onAddStudentToClassClicked"
QT_MOC_LITERAL(11, 160, 31), // "onRemoveStudentFromClassClicked"
QT_MOC_LITERAL(12, 192, 23), // "onExamRoomsDataReceived"
QT_MOC_LITERAL(13, 216, 18), // "onExamRoomSelected"
QT_MOC_LITERAL(14, 235, 28), // "onExamAttendanceDataReceived"
QT_MOC_LITERAL(15, 264, 20), // "onAddExamRoomClicked"
QT_MOC_LITERAL(16, 285, 25), // "onExportAttendanceClicked"
QT_MOC_LITERAL(17, 311, 20), // "onManageSeatsClicked"
QT_MOC_LITERAL(18, 332, 20), // "onLinkClassesClicked"
QT_MOC_LITERAL(19, 353, 24), // "onLinkClassButtonClicked"
QT_MOC_LITERAL(20, 378, 23), // "onClassesLinkedReceived"
QT_MOC_LITERAL(21, 402, 22), // "onGenerateSeatsClicked"
QT_MOC_LITERAL(22, 425, 28), // "onSeatArrangementTypeChanged"
QT_MOC_LITERAL(23, 454, 20), // "onExportSeatsClicked"
QT_MOC_LITERAL(24, 475, 25), // "onViewStudentPhotoClicked"
QT_MOC_LITERAL(25, 501, 22), // "onStudentPhotoReceived"
QT_MOC_LITERAL(26, 524, 29), // "onRequestStudentPhotosClicked"
QT_MOC_LITERAL(27, 554, 26), // "onViewStudentPhotosClicked"
QT_MOC_LITERAL(28, 581, 27), // "onViewStudentPhotosFinished"
QT_MOC_LITERAL(29, 609, 22), // "onRefreshButtonClicked"
QT_MOC_LITERAL(30, 632, 21), // "onLogoutButtonClicked"
QT_MOC_LITERAL(31, 654, 29), // "onRefreshClassesButtonClicked"
QT_MOC_LITERAL(32, 684, 23), // "onAddClassButtonClicked"
QT_MOC_LITERAL(33, 708, 24), // "onEditClassButtonClicked"
QT_MOC_LITERAL(34, 733, 26), // "onDeleteClassButtonClicked"
QT_MOC_LITERAL(35, 760, 21), // "onStudentDataReceived"
QT_MOC_LITERAL(36, 782, 25), // "onAddStudentButtonClicked"
QT_MOC_LITERAL(37, 808, 26), // "onEditStudentButtonClicked"
QT_MOC_LITERAL(38, 835, 28), // "onDeleteStudentButtonClicked"
QT_MOC_LITERAL(39, 864, 21), // "onExportButtonClicked"
QT_MOC_LITERAL(40, 886, 21), // "onSearchButtonClicked"
QT_MOC_LITERAL(41, 908, 19), // "onSearchTextChanged"
QT_MOC_LITERAL(42, 928, 4), // "text"
QT_MOC_LITERAL(43, 933, 22), // "setupClassPhotoRefresh"
QT_MOC_LITERAL(44, 956, 24) // "refreshClassPhotosStatus"

    },
    "TeacherWidget\0onClassesDataReceived\0"
    "\0QNetworkReply*\0reply\0onStudentsDataReceived\0"
    "onClassSelected\0QModelIndex\0index\0"
    "onAddClassClicked\0onAddStudentToClassClicked\0"
    "onRemoveStudentFromClassClicked\0"
    "onExamRoomsDataReceived\0onExamRoomSelected\0"
    "onExamAttendanceDataReceived\0"
    "onAddExamRoomClicked\0onExportAttendanceClicked\0"
    "onManageSeatsClicked\0onLinkClassesClicked\0"
    "onLinkClassButtonClicked\0"
    "onClassesLinkedReceived\0onGenerateSeatsClicked\0"
    "onSeatArrangementTypeChanged\0"
    "onExportSeatsClicked\0onViewStudentPhotoClicked\0"
    "onStudentPhotoReceived\0"
    "onRequestStudentPhotosClicked\0"
    "onViewStudentPhotosClicked\0"
    "onViewStudentPhotosFinished\0"
    "onRefreshButtonClicked\0onLogoutButtonClicked\0"
    "onRefreshClassesButtonClicked\0"
    "onAddClassButtonClicked\0"
    "onEditClassButtonClicked\0"
    "onDeleteClassButtonClicked\0"
    "onStudentDataReceived\0onAddStudentButtonClicked\0"
    "onEditStudentButtonClicked\0"
    "onDeleteStudentButtonClicked\0"
    "onExportButtonClicked\0onSearchButtonClicked\0"
    "onSearchTextChanged\0text\0"
    "setupClassPhotoRefresh\0refreshClassPhotosStatus"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TeacherWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      38,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  204,    2, 0x08 /* Private */,
       5,    1,  207,    2, 0x08 /* Private */,
       6,    1,  210,    2, 0x08 /* Private */,
       9,    0,  213,    2, 0x08 /* Private */,
      10,    0,  214,    2, 0x08 /* Private */,
      11,    0,  215,    2, 0x08 /* Private */,
      12,    1,  216,    2, 0x08 /* Private */,
      13,    1,  219,    2, 0x08 /* Private */,
      14,    1,  222,    2, 0x08 /* Private */,
      15,    0,  225,    2, 0x08 /* Private */,
      16,    0,  226,    2, 0x08 /* Private */,
      17,    0,  227,    2, 0x08 /* Private */,
      18,    0,  228,    2, 0x08 /* Private */,
      19,    0,  229,    2, 0x08 /* Private */,
      20,    1,  230,    2, 0x08 /* Private */,
      21,    0,  233,    2, 0x08 /* Private */,
      22,    1,  234,    2, 0x08 /* Private */,
      23,    0,  237,    2, 0x08 /* Private */,
      24,    0,  238,    2, 0x08 /* Private */,
      25,    1,  239,    2, 0x08 /* Private */,
      26,    0,  242,    2, 0x08 /* Private */,
      27,    0,  243,    2, 0x08 /* Private */,
      28,    1,  244,    2, 0x08 /* Private */,
      29,    0,  247,    2, 0x08 /* Private */,
      30,    0,  248,    2, 0x08 /* Private */,
      31,    0,  249,    2, 0x08 /* Private */,
      32,    0,  250,    2, 0x08 /* Private */,
      33,    0,  251,    2, 0x08 /* Private */,
      34,    0,  252,    2, 0x08 /* Private */,
      35,    1,  253,    2, 0x08 /* Private */,
      36,    0,  256,    2, 0x08 /* Private */,
      37,    0,  257,    2, 0x08 /* Private */,
      38,    0,  258,    2, 0x08 /* Private */,
      39,    0,  259,    2, 0x08 /* Private */,
      40,    0,  260,    2, 0x08 /* Private */,
      41,    1,  261,    2, 0x08 /* Private */,
      43,    0,  264,    2, 0x08 /* Private */,
      44,    0,  265,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   42,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TeacherWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TeacherWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onClassesDataReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 1: _t->onStudentsDataReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 2: _t->onClassSelected((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: _t->onAddClassClicked(); break;
        case 4: _t->onAddStudentToClassClicked(); break;
        case 5: _t->onRemoveStudentFromClassClicked(); break;
        case 6: _t->onExamRoomsDataReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 7: _t->onExamRoomSelected((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: _t->onExamAttendanceDataReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 9: _t->onAddExamRoomClicked(); break;
        case 10: _t->onExportAttendanceClicked(); break;
        case 11: _t->onManageSeatsClicked(); break;
        case 12: _t->onLinkClassesClicked(); break;
        case 13: _t->onLinkClassButtonClicked(); break;
        case 14: _t->onClassesLinkedReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 15: _t->onGenerateSeatsClicked(); break;
        case 16: _t->onSeatArrangementTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->onExportSeatsClicked(); break;
        case 18: _t->onViewStudentPhotoClicked(); break;
        case 19: _t->onStudentPhotoReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 20: _t->onRequestStudentPhotosClicked(); break;
        case 21: _t->onViewStudentPhotosClicked(); break;
        case 22: _t->onViewStudentPhotosFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 23: _t->onRefreshButtonClicked(); break;
        case 24: _t->onLogoutButtonClicked(); break;
        case 25: _t->onRefreshClassesButtonClicked(); break;
        case 26: _t->onAddClassButtonClicked(); break;
        case 27: _t->onEditClassButtonClicked(); break;
        case 28: _t->onDeleteClassButtonClicked(); break;
        case 29: _t->onStudentDataReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 30: _t->onAddStudentButtonClicked(); break;
        case 31: _t->onEditStudentButtonClicked(); break;
        case 32: _t->onDeleteStudentButtonClicked(); break;
        case 33: _t->onExportButtonClicked(); break;
        case 34: _t->onSearchButtonClicked(); break;
        case 35: _t->onSearchTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 36: _t->setupClassPhotoRefresh(); break;
        case 37: _t->refreshClassPhotosStatus(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 19:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 22:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 29:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TeacherWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_TeacherWidget.data,
    qt_meta_data_TeacherWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TeacherWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TeacherWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TeacherWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TeacherWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 38)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 38;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 38)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 38;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
