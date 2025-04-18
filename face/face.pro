QT       += core gui sql multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    logindialog.cpp

HEADERS += \
    mainwindow.h \
    networkmanager.h \
    logindialog.h

FORMS += \
    mainwindow.ui \
    logindialog.ui

# OpenCV配置
INCLUDEPATH += D:/opencv/mingw-build-with-contrib/install/include
LIBS += -LD:/opencv/mingw-build-with-contrib/install/x64/mingw/lib \
    -lopencv_core460 \
    -lopencv_imgproc460 \
    -lopencv_highgui460 \
    -lopencv_imgcodecs460 \
    -lopencv_videoio460 \
    -lopencv_video460 \
    -lopencv_calib3d460 \
    -lopencv_photo460 \
    -lopencv_features2d460 \
    -lopencv_objdetect460 \
    -lopencv_face460

# 添加资源文件
RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
