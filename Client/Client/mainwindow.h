#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QJsonObject>

class LoginDialog;
class StudentWidget;
class TeacherWidget;
class AdminWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void login();
    void logout();

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
    
    LoginDialog *loginDialog;
    StudentWidget *studentWidget;
    TeacherWidget *teacherWidget;
    AdminWidget *adminWidget;
    
    QString userRole;
    QJsonObject userInfo;
    QString token;
    
    void setupUI();
    void showInterface();
};
#endif // MAINWINDOW_H
