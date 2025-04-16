#include "mainwindow.h"
#include "logindialog.h"
#include "networkmanager.h"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序名称
    QCoreApplication::setApplicationName("人脸识别排座系统");
    QCoreApplication::setOrganizationName("FaceRecognition");
    
    // 创建数据目录
    QDir dir;
    dir.mkpath(QCoreApplication::applicationDirPath() + "/data");
    dir.mkpath(QCoreApplication::applicationDirPath() + "/data/faces");
    
    // 显示登录对话框
    LoginDialog loginDialog;
    if (loginDialog.exec() != QDialog::Accepted) {
        // 用户取消登录
        return 0;
    }
    
    // 设置服务器地址
    NetworkManager::getInstance().setServerAddress(loginDialog.getServerUrl());
    
    // 登录成功，显示主窗口
    MainWindow w;
    w.setWindowTitle("人脸识别排座系统 - 考勤端 [" + loginDialog.getUsername() + "]");
    w.show();
    
    return a.exec();
}
