QT       += core gui network axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# 定义是否使用Windows API的标志
# 如果出现编译错误，请将此行改为：#DEFINES += USE_WINDOWS_API
DEFINES += USE_WINDOWS_API

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    adminwidget.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    studentwidget.cpp \
    teacherwidget.cpp

HEADERS += \
    adminwidget.h \
    logindialog.h \
    mainwindow.h \
    studentwidget.h \
    teacherwidget.h

FORMS += \
    adminwidget.ui \
    logindialog.ui \
    mainwindow.ui \
    studentwidget.ui \
    teacherwidget.ui

# 添加资源文件
RESOURCES += \
    resources.qrc

# 添加应用程序图标
RC_ICONS = app_icon.ico
win32:RC_FILE = windows_resource.rc

# 添加Windows库
win32:LIBS += -lshell32 -lole32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
