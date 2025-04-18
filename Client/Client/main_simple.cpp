#include "mainwindow.h"
#include <QApplication>
#include <QIcon>
#include <QDebug>
#include <Windows.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序图标
    QIcon appIcon(":/icon/app_icon.ico");
    a.setWindowIcon(appIcon);
    
    // 设置应用程序名称和组织
    a.setApplicationName("人脸识别考勤系统");
    a.setOrganizationName("考勤系统开发团队");
    a.setApplicationVersion("1.0.0");
    
    MainWindow w;
    
    // 确保窗口在任务栏中显示
    w.setAttribute(Qt::WA_NativeWindow);
    w.show();
    
    return a.exec();
} 