#include "adminwidget.h"
#include "ui_adminwidget.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QUrl>
#include <QFileDialog>
#include <QInputDialog>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>
#include <QJsonObject>
#include <QDebug>

// 服务器地址
const QString ADMIN_SERVER_URL = "http://localhost:8080";
// 令牌刷新间隔（毫秒）- 20分钟刷新一次
const int TOKEN_REFRESH_INTERVAL = 20 * 60 * 1000;

AdminWidget::AdminWidget(const QJsonObject &userInfo, const QString &token, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminWidget),
    userInfo(userInfo),
    token(token),
    networkManager(new QNetworkAccessManager(this)),
    usersModel(new QStandardItemModel(this)),
    tokenRefreshTimer(new QTimer(this))
{
    ui->setupUi(this);
    setupUI();
    
    // 连接网络响应信号 - 修改处理逻辑，区分HTTP方法
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        QString endpoint = reply->request().url().path();
        QNetworkAccessManager::Operation operation = reply->operation();
        
        // 只对GET请求使用onUsersDataReceived处理
        if (endpoint.contains("/admin/users") && operation == QNetworkAccessManager::GetOperation) {
            onUsersDataReceived(reply);
        } else if (endpoint.contains("/auth/refresh")) {
            onTokenRefreshed(reply);
        }
    });
    
    // 连接用户管理相关信号
    connect(ui->addUserButton, &QPushButton::clicked, this, &AdminWidget::onAddUserClicked);
    connect(ui->editUserButton, &QPushButton::clicked, this, &AdminWidget::onEditUserClicked);
    connect(ui->deleteUserButton, &QPushButton::clicked, this, &AdminWidget::onDeleteUserClicked);
    
    // 连接数据库管理相关信号
    connect(ui->backupButton, &QPushButton::clicked, this, &AdminWidget::onBackupDatabaseClicked);
    connect(ui->restoreButton, &QPushButton::clicked, this, &AdminWidget::onRestoreDatabaseClicked);
    
    // 连接刷新按钮
    connect(ui->refreshButton, &QPushButton::clicked, this, &AdminWidget::onRefreshButtonClicked);
    
    // 初始化令牌刷新定时器
    connect(tokenRefreshTimer, &QTimer::timeout, this, &AdminWidget::refreshToken);
    tokenRefreshTimer->start(TOKEN_REFRESH_INTERVAL);
    
    // 加载初始数据
    loadUsersData();
}

AdminWidget::~AdminWidget()
{
    delete ui;
}

// 刷新令牌
void AdminWidget::refreshToken()
{
    qDebug() << "正在刷新令牌...";
    
    // 准备刷新令牌请求
    QUrl url(ADMIN_SERVER_URL + "/auth/refresh");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 发送POST请求刷新令牌
    networkManager->post(request, QByteArray());
}

// 处理令牌刷新响应
void AdminWidget::onTokenRefreshed(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "令牌刷新失败:" << reply->errorString();
        reply->deleteLater();
        return;
    }
    
    // 解析响应
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("token")) {
            // 更新令牌
            token = jsonObject["token"].toString();
            qDebug() << "令牌刷新成功";
        }
    } else {
        qDebug() << "令牌刷新失败:" << jsonObject["message"].toString();
    }
    
    reply->deleteLater();
}

void AdminWidget::setupUI()
{
    // 设置管理员信息
    if (userInfo.contains("username")) {
        ui->nameLabel->setText("管理员: " + userInfo["username"].toString());
    }
    
    // 设置用户表格模型
    usersModel->setHorizontalHeaderLabels(QStringList() << "用户名" << "姓名" << "角色" << "创建时间");
    ui->usersTableView->setModel(usersModel);
    ui->usersTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void AdminWidget::loadUsersData()
{
    // 准备用户数据请求
    QUrl url(ADMIN_SERVER_URL + "/admin/users");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 发送请求
    networkManager->get(request);
}

void AdminWidget::onUsersDataReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "网络错误", "获取用户数据失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("data") && jsonObject["data"].isArray()) {
            QJsonArray usersData = jsonObject["data"].toArray();
            
            // 更新用户表格
            usersModel->removeRows(0, usersModel->rowCount());
            
            for (int i = 0; i < usersData.size(); ++i) {
                QJsonObject userData = usersData[i].toObject();
                
                QList<QStandardItem*> rowItems;
                rowItems << new QStandardItem(userData["username"].toString());
                rowItems << new QStandardItem(userData["name"].toString());
                
                // 设置角色颜色
                QString role = userData["role"].toString();
                QStandardItem *roleItem = new QStandardItem(role);
                if (role == "admin") {
                    roleItem->setForeground(QBrush(Qt::red));
                } else if (role == "teacher") {
                    roleItem->setForeground(QBrush(Qt::blue));
                } else if (role == "student") {
                    roleItem->setForeground(QBrush(Qt::green));
                }
                rowItems << roleItem;
                
                // 格式化创建时间
                if (userData.contains("createdAt")) {
                    QString createdAtStr = userData["createdAt"].toString();
                    QDateTime createdAt = QDateTime::fromString(createdAtStr, Qt::ISODate);
                    rowItems << new QStandardItem(createdAt.toString("yyyy-MM-dd HH:mm"));
                } else {
                    rowItems << new QStandardItem("-");
                }
                
                // 存储用户ID用于后续操作
                rowItems[0]->setData(userData["id"].toInt(), Qt::UserRole);
                
                usersModel->appendRow(rowItems);
            }
        }
    } else {
        QString errorMessage = "获取用户数据失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
    }
    
    reply->deleteLater();
}

void AdminWidget::onAddUserClicked()
{
    // 创建添加用户对话框
    QDialog dialog(this);
    dialog.setWindowTitle("添加用户");
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    // 用户名
    QHBoxLayout *usernameLayout = new QHBoxLayout();
    QLabel *usernameLabel = new QLabel("用户名:");
    QLineEdit *usernameEdit = new QLineEdit();
    usernameLayout->addWidget(usernameLabel);
    usernameLayout->addWidget(usernameEdit);
    layout->addLayout(usernameLayout);
    
    // 密码
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    QLabel *passwordLabel = new QLabel("密码:");
    QLineEdit *passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordEdit);
    layout->addLayout(passwordLayout);
    
    // 姓名
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel("姓名:");
    QLineEdit *nameEdit = new QLineEdit();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    layout->addLayout(nameLayout);
    
    // 角色
    QHBoxLayout *roleLayout = new QHBoxLayout();
    QLabel *roleLabel = new QLabel("角色:");
    QComboBox *roleCombo = new QComboBox();
    roleCombo->addItem("学生", "student");
    roleCombo->addItem("教师", "teacher");
    roleCombo->addItem("管理员", "admin");
    roleLayout->addWidget(roleLabel);
    roleLayout->addWidget(roleCombo);
    layout->addLayout(roleLayout);
    
    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton("取消");
    QPushButton *confirmButton = new QPushButton("确定");
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(confirmButton);
    layout->addLayout(buttonLayout);
    
    // 连接信号
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(confirmButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    
    // 显示对话框
    if (dialog.exec() == QDialog::Accepted) {
        // 获取输入
        QString username = usernameEdit->text();
        QString password = passwordEdit->text();
        QString name = nameEdit->text();
        QString role = roleCombo->currentData().toString();
        
        // 验证输入
        if (username.isEmpty() || password.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, "错误", "所有字段都必须填写");
            return;
        }
        
        // 准备添加用户请求
        QUrl url(ADMIN_SERVER_URL + "/admin/users");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 构建添加用户数据
        QJsonObject userData;
        userData["username"] = username;
        userData["password"] = password;
        userData["name"] = name;
        userData["role"] = role;
        QJsonDocument doc(userData);
        
        // 发送请求
        QNetworkReply *reply = networkManager->post(request, doc.toJson());
        
        // 处理响应
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "添加用户失败: " + reply->errorString());
                reply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", "用户添加成功");
                loadUsersData(); // 刷新用户列表
            } else {
                QString errorMessage = "添加用户失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            reply->deleteLater();
        });
    }
}

void AdminWidget::onEditUserClicked()
{
    // 获取选中的用户
    QModelIndex index = ui->usersTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择要编辑的用户");
        return;
    }
    
    // 获取用户ID和当前数据
    int userId = usersModel->data(usersModel->index(index.row(), 0), Qt::UserRole).toInt();
    QString username = usersModel->data(usersModel->index(index.row(), 0)).toString();
    QString name = usersModel->data(usersModel->index(index.row(), 1)).toString();
    QString role = usersModel->data(usersModel->index(index.row(), 2)).toString();
    
    // 创建编辑用户对话框
    QDialog dialog(this);
    dialog.setWindowTitle(QString("编辑用户 - %1").arg(username));
    
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    // 姓名
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel("姓名:");
    QLineEdit *nameEdit = new QLineEdit(name);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    layout->addLayout(nameLayout);
    
    // 新密码
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    QLabel *passwordLabel = new QLabel("新密码:");
    QLineEdit *passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("留空则不修改");
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(passwordEdit);
    layout->addLayout(passwordLayout);
    
    // 角色
    QHBoxLayout *roleLayout = new QHBoxLayout();
    QLabel *roleLabel = new QLabel("角色:");
    QComboBox *roleCombo = new QComboBox();
    roleCombo->addItem("学生", "student");
    roleCombo->addItem("教师", "teacher");
    roleCombo->addItem("管理员", "admin");
    
    // 设置当前角色
    int roleIndex = 0;
    if (role == "teacher") roleIndex = 1;
    else if (role == "admin") roleIndex = 2;
    roleCombo->setCurrentIndex(roleIndex);
    
    roleLayout->addWidget(roleLabel);
    roleLayout->addWidget(roleCombo);
    layout->addLayout(roleLayout);
    
    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton("取消");
    QPushButton *confirmButton = new QPushButton("确定");
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(confirmButton);
    layout->addLayout(buttonLayout);
    
    // 连接信号
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(confirmButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    
    // 显示对话框
    if (dialog.exec() == QDialog::Accepted) {
        // 获取输入
        QString newPassword = passwordEdit->text();
        QString newName = nameEdit->text();
        QString newRole = roleCombo->currentData().toString();
        
        // 验证输入
        if (newName.isEmpty()) {
            QMessageBox::warning(this, "错误", "姓名不能为空");
            return;
        }
        
        // 准备编辑用户请求 - 修复这里的URL
        QUrl url(ADMIN_SERVER_URL + "/admin/users/" + QString::number(userId));
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 构建更新用户数据
        QJsonObject userData;
        userData["name"] = newName;
        userData["role"] = newRole;
        if (!newPassword.isEmpty()) {
            userData["password"] = newPassword;
        }
        QJsonDocument doc(userData);
        
        // 发送请求
        QNetworkReply *reply = networkManager->put(request, doc.toJson());
        
        // 处理响应
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "更新用户失败: " + reply->errorString());
                reply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", "用户更新成功");
                loadUsersData(); // 刷新用户列表
            } else {
                QString errorMessage = "更新用户失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            reply->deleteLater();
        });
    }
}

void AdminWidget::onDeleteUserClicked()
{
    // 获取选中的用户
    QModelIndex index = ui->usersTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的用户");
        return;
    }
    
    // 获取用户ID和用户名
    int userId = usersModel->data(usersModel->index(index.row(), 0), Qt::UserRole).toInt();
    QString username = usersModel->data(usersModel->index(index.row(), 0)).toString();
    
    // 确认删除
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认", 
                                                             QString("确定要删除用户\"%1\"吗?").arg(username),
                                                             QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 准备删除用户请求 - 修复URL构建
        QUrl url(ADMIN_SERVER_URL + "/admin/users/" + QString::number(userId));
        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 发送请求
        QNetworkReply *deleteReply = networkManager->deleteResource(request);
        
        // 处理响应
        connect(deleteReply, &QNetworkReply::finished, this, [this, deleteReply]() {
            if (deleteReply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "删除用户失败: " + deleteReply->errorString());
                deleteReply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = deleteReply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", "用户已删除");
                loadUsersData(); // 刷新用户列表
            } else {
                QString errorMessage = "删除用户失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            deleteReply->deleteLater();
        });
    }
}

void AdminWidget::onBackupDatabaseClicked()
{
    // 获取保存文件路径
    QString fileName = QFileDialog::getSaveFileName(this, "备份数据库", "", "SQL文件 (*.sql);;所有文件 (*)");
    if (fileName.isEmpty()) return;
    
    // 准备备份请求
    QUrl url(ADMIN_SERVER_URL + "/admin/database/backup");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 发送请求
    QNetworkReply *reply = networkManager->get(request);
    
    // 处理响应
    connect(reply, &QNetworkReply::finished, this, [this, reply, fileName]() {
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "网络错误", "备份数据库失败: " + reply->errorString());
            reply->deleteLater();
            return;
        }
        
        // 保存响应数据到文件
        QByteArray responseData = reply->readAll();
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, "错误", "无法创建文件: " + fileName);
            reply->deleteLater();
            return;
        }
        
        file.write(responseData);
        file.close();
        
        QMessageBox::information(this, "成功", "数据库已备份到: " + fileName);
        reply->deleteLater();
    });
}

void AdminWidget::onRestoreDatabaseClicked()
{
    // 获取要恢复的文件路径
    QString fileName = QFileDialog::getOpenFileName(this, "恢复数据库", "", "SQL文件 (*.sql);;所有文件 (*)");
    if (fileName.isEmpty()) return;
    
    // 确认恢复
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认", 
                                                             "恢复数据库将覆盖当前所有数据，确定要继续吗?",
                                                             QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 读取文件内容
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, "错误", "无法打开文件: " + fileName);
            return;
        }
        
        QByteArray fileData = file.readAll();
        file.close();
        
        // 准备恢复请求
        QUrl url(ADMIN_SERVER_URL + "/admin/database/restore");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 发送请求
        QNetworkReply *restoreReply = networkManager->post(request, fileData);
        
        // 处理响应
        connect(restoreReply, &QNetworkReply::finished, this, [this, restoreReply]() {
            if (restoreReply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "恢复数据库失败: " + restoreReply->errorString());
                restoreReply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = restoreReply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", "数据库已恢复");
                loadUsersData(); // 刷新用户列表
            } else {
                QString errorMessage = "恢复数据库失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            restoreReply->deleteLater();
        });
    }
}

void AdminWidget::onRefreshButtonClicked()
{
    // 刷新用户数据
    loadUsersData();
} 