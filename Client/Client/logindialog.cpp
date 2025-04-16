#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QUrl>

// 服务器地址
const QString SERVER_URL = "http://localhost:8080";

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog),
    networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setWindowTitle("登录系统");
    
    // 连接登录按钮点击信号
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginButtonClicked);
    
    // 连接网络响应信号
    connect(networkManager, &QNetworkAccessManager::finished, this, &LoginDialog::onNetworkReply);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::getUserRole() const
{
    return userRole;
}

QJsonObject LoginDialog::getUserInfo() const
{
    return userInfo;
}

QString LoginDialog::getToken() const
{
    return token;
}

void LoginDialog::onLoginButtonClicked()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    
    // 验证输入
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "错误", "用户名和密码不能为空");
        return;
    }
    
    // 准备登录请求
    QUrl url(SERVER_URL + "/auth/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 构建登录数据
    QJsonObject loginData;
    loginData["username"] = username;
    loginData["password"] = password;
    QJsonDocument doc(loginData);
    
    // 发送请求
    networkManager->post(request, doc.toJson());
    
    // 禁用登录按钮，显示等待状态
    ui->loginButton->setEnabled(false);
    ui->loginButton->setText("登录中...");
}

void LoginDialog::onNetworkReply(QNetworkReply *reply)
{
    // 恢复登录按钮状态
    ui->loginButton->setEnabled(true);
    ui->loginButton->setText("登录");
    
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "网络错误", "连接服务器失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    // 检查登录是否成功
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        // 保存令牌
        if (jsonObject.contains("token")) {
            token = jsonObject["token"].toString();
        }
        
        // 保存用户信息
        if (jsonObject.contains("user")) {
            QJsonObject user = jsonObject["user"].toObject();
            
            // 保存用户角色
            if (user.contains("role")) {
                userRole = user["role"].toString();
            }
            
            // 创建用户信息对象
            userInfo = user;
        }
        
        // 接受对话框(成功)
        accept();
    } else {
        // 登录失败，显示错误消息
        QString errorMessage = "登录失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "登录失败", errorMessage);
    }
    
    reply->deleteLater();
} 