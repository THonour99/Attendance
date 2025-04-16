#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    // 获取当前登录的用户信息和角色
    QString getUserRole() const;
    QJsonObject getUserInfo() const;
    QString getToken() const;

private slots:
    void onLoginButtonClicked();
    void onNetworkReply(QNetworkReply *reply);

private:
    Ui::LoginDialog *ui;
    QNetworkAccessManager *networkManager;
    QString userRole;        // 用户角色：teacher, student, admin
    QJsonObject userInfo;    // 用户信息
    QString token;           // 认证令牌
};

#endif // LOGINDIALOG_H 