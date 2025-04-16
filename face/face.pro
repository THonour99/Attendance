QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp

HEADERS += \
    logindialog.h \
    mainwindow.h \
    networkmanager.h

FORMS += \
    mainwindow.ui

# OpenCV配置
INCLUDEPATH += D:/opencv/mingw-build-with-contrib/install/include
LIBS += -LD:/opencv/mingw-build-with-contrib/lib \
    -lopencv_core460 \
    -lopencv_highgui460 \
    -lopencv_imgproc460 \
    -lopencv_imgcodecs460 \
    -lopencv_videoio460 \
    -lopencv_face460 \
    -lopencv_objdetect460

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
