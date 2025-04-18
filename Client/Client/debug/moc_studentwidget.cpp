/****************************************************************************
** Meta object code from reading C++ file 'studentwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../studentwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'studentwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_StudentWidget_t {
    QByteArrayData data[23];
    char stringdata0[440];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_StudentWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_StudentWidget_t qt_meta_stringdata_StudentWidget = {
    {
QT_MOC_LITERAL(0, 0, 13), // "StudentWidget"
QT_MOC_LITERAL(1, 14, 6), // "logout"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 22), // "onRefreshButtonClicked"
QT_MOC_LITERAL(4, 45, 26), // "onUploadPhotoButtonClicked"
QT_MOC_LITERAL(5, 72, 24), // "onAttendanceDataReceived"
QT_MOC_LITERAL(6, 97, 14), // "QNetworkReply*"
QT_MOC_LITERAL(7, 112, 5), // "reply"
QT_MOC_LITERAL(8, 118, 19), // "onClassDataReceived"
QT_MOC_LITERAL(9, 138, 21), // "onPhotoUploadResponse"
QT_MOC_LITERAL(10, 160, 18), // "onSeatInfoReceived"
QT_MOC_LITERAL(11, 179, 18), // "onExamInfoReceived"
QT_MOC_LITERAL(12, 198, 21), // "onLogoutButtonClicked"
QT_MOC_LITERAL(13, 220, 27), // "onNotificationButtonClicked"
QT_MOC_LITERAL(14, 248, 18), // "checkNotifications"
QT_MOC_LITERAL(15, 267, 20), // "processNotifications"
QT_MOC_LITERAL(16, 288, 13), // "notifications"
QT_MOC_LITERAL(17, 302, 34), // "onPhotoRequestNotificationRec..."
QT_MOC_LITERAL(18, 337, 12), // "notification"
QT_MOC_LITERAL(19, 350, 22), // "markNotificationAsRead"
QT_MOC_LITERAL(20, 373, 14), // "notificationId"
QT_MOC_LITERAL(21, 388, 20), // "updateNotificationUI"
QT_MOC_LITERAL(22, 409, 30) // "updateUnreadNotificationsCount"

    },
    "StudentWidget\0logout\0\0onRefreshButtonClicked\0"
    "onUploadPhotoButtonClicked\0"
    "onAttendanceDataReceived\0QNetworkReply*\0"
    "reply\0onClassDataReceived\0"
    "onPhotoUploadResponse\0onSeatInfoReceived\0"
    "onExamInfoReceived\0onLogoutButtonClicked\0"
    "onNotificationButtonClicked\0"
    "checkNotifications\0processNotifications\0"
    "notifications\0onPhotoRequestNotificationReceived\0"
    "notification\0markNotificationAsRead\0"
    "notificationId\0updateNotificationUI\0"
    "updateUnreadNotificationsCount"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_StudentWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   95,    2, 0x08 /* Private */,
       4,    0,   96,    2, 0x08 /* Private */,
       5,    1,   97,    2, 0x08 /* Private */,
       8,    1,  100,    2, 0x08 /* Private */,
       9,    1,  103,    2, 0x08 /* Private */,
      10,    1,  106,    2, 0x08 /* Private */,
      11,    1,  109,    2, 0x08 /* Private */,
      12,    0,  112,    2, 0x08 /* Private */,
      13,    0,  113,    2, 0x08 /* Private */,
      14,    0,  114,    2, 0x08 /* Private */,
      15,    1,  115,    2, 0x08 /* Private */,
      17,    1,  118,    2, 0x08 /* Private */,
      19,    1,  121,    2, 0x08 /* Private */,
      21,    1,  124,    2, 0x08 /* Private */,
      22,    0,  127,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonArray,   16,
    QMetaType::Void, QMetaType::QJsonObject,   18,
    QMetaType::Void, QMetaType::LongLong,   20,
    QMetaType::Void, QMetaType::QJsonArray,   16,
    QMetaType::Void,

       0        // eod
};

void StudentWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<StudentWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->logout(); break;
        case 1: _t->onRefreshButtonClicked(); break;
        case 2: _t->onUploadPhotoButtonClicked(); break;
        case 3: _t->onAttendanceDataReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 4: _t->onClassDataReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 5: _t->onPhotoUploadResponse((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 6: _t->onSeatInfoReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 7: _t->onExamInfoReceived((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        case 8: _t->onLogoutButtonClicked(); break;
        case 9: _t->onNotificationButtonClicked(); break;
        case 10: _t->checkNotifications(); break;
        case 11: _t->processNotifications((*reinterpret_cast< const QJsonArray(*)>(_a[1]))); break;
        case 12: _t->onPhotoRequestNotificationReceived((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 13: _t->markNotificationAsRead((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 14: _t->updateNotificationUI((*reinterpret_cast< const QJsonArray(*)>(_a[1]))); break;
        case 15: _t->updateUnreadNotificationsCount(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        case 5:
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
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QNetworkReply* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (StudentWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&StudentWidget::logout)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject StudentWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_StudentWidget.data,
    qt_meta_data_StudentWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *StudentWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *StudentWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_StudentWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int StudentWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void StudentWidget::logout()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
