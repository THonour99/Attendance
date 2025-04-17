#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "studentwidget.h"
#include "teacherwidget.h"
#include "adminwidget.h"
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stackedWidget(new QStackedWidget(this))
    , loginDialog(nullptr)
    , studentWidget(nullptr)
    , teacherWidget(nullptr)
    , adminWidget(nullptr)
{
    ui->setupUi(this);
    setupUI();
    
    // 自动显示登录对话框
    login();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 设置窗口标题和大小
    setWindowTitle("人脸识别考勤系统 - 客户端");
    resize(1000, 700);
    
    // 设置中央部件为堆叠部件
    setCentralWidget(stackedWidget);
    
    // 创建菜单栏和操作
    QMenu *fileMenu = menuBar()->addMenu("文件");
    QAction *logoutAction = fileMenu->addAction("注销");
    QAction *exitAction = fileMenu->addAction("退出");
    
    // 连接操作信号
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);
    connect(exitAction, &QAction::triggered, this, &QApplication::quit);
}

void MainWindow::login()
{
    // 创建登录对话框
    loginDialog = new LoginDialog(this);
    
    // 显示登录对话框
    if (loginDialog->exec() == QDialog::Accepted) {
        // 获取登录信息
        userRole = loginDialog->getUserRole();
        userInfo = loginDialog->getUserInfo();
        token = loginDialog->getToken();
        
        // 显示对应界面
        showInterface();
    } else {
        // 登录取消或失败，退出应用
        close();
    }
    
    // 清理登录对话框
    delete loginDialog;
    loginDialog = nullptr;
}

void MainWindow::logout()
{
    // 确认注销
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认注销", 
                                                             "确定要注销当前用户吗?",
                                                             QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // 清理当前界面
    if (studentWidget) {
        stackedWidget->removeWidget(studentWidget);
        delete studentWidget;
        studentWidget = nullptr;
    }
    
    if (teacherWidget) {
        stackedWidget->removeWidget(teacherWidget);
        delete teacherWidget;
        teacherWidget = nullptr;
    }
    
    if (adminWidget) {
        stackedWidget->removeWidget(adminWidget);
        delete adminWidget;
        adminWidget = nullptr;
    }
    
    // 清空用户信息
    userRole.clear();
    userInfo = QJsonObject();
    token.clear();
    
    // 重新登录
    login();
}

void MainWindow::showInterface()
{
    // 根据用户角色加载对应界面
    if (userRole == "student") {
        // 学生界面
        studentWidget = new StudentWidget(this);
        studentWidget->setUserInfo(
            userInfo["username"].toString(),
            userInfo["studentId"].toString(),
            token
        );
        stackedWidget->addWidget(studentWidget);
        stackedWidget->setCurrentWidget(studentWidget);
    } else if (userRole == "teacher") {
        // 教师界面
        teacherWidget = new TeacherWidget(userInfo, token, this);
        stackedWidget->addWidget(teacherWidget);
        stackedWidget->setCurrentWidget(teacherWidget);
    } else if (userRole == "admin") {
        // 管理员界面
        adminWidget = new AdminWidget(userInfo, token, this);
        stackedWidget->addWidget(adminWidget);
        stackedWidget->setCurrentWidget(adminWidget);
    } else if (userRole == "operator") {
        // 操作员界面 - 考勤端操作员
        // 考勤端在单独的应用程序中实现，这里也为操作员提供基本界面
        // 这里假设操作员可以访问学生和考勤信息
        studentWidget = new StudentWidget(this);
        studentWidget->setUserInfo(
            userInfo["username"].toString(),
            userInfo["id"].toString(),
            token
        );
        stackedWidget->addWidget(studentWidget);
        stackedWidget->setCurrentWidget(studentWidget);
    } else {
        QMessageBox::critical(this, "错误", "未知的用户角色: " + userRole);
        logout();
    }
}
