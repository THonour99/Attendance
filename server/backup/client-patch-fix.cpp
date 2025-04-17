// 客户端补丁代码 - 修复服务器连接问题

// ======== 1. 修改 networkmanager.h ========

/*
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QVector>
#include <QUrl>

// 考场信息结构体
struct ExamRoom {
    int id;
    QString name;
    QString location;
    int capacity;
    QString examDate;
    QString examTime;
};

// 学生信息结构体
struct StudentInfo {
    int id;
    QString studentId; // 学号
    QString name;
    int seatNumber;
    QString attendanceStatus;
};

class NetworkManager : public QObject
{
    Q_OBJECT
    
public:
    static NetworkManager& getInstance();
    void setServerAddress(const QString &url) { serverUrl = url; }
    
    bool login(const QString &username, const QString &password);
    QVector<ExamRoom> getExamRooms();
    QVector<StudentInfo> getStudentsForExam(int examRoomId);
    bool uploadAttendanceRecord(int studentId, int examRoomId, const QDateTime &time);
    
signals:
    void networkError(const QString &errorMessage);
    
private:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();
    
    static NetworkManager *m_instance;
    
    QNetworkAccessManager *manager;
    QString serverUrl;
    QString authToken;
    
    QJsonDocument sendRequest(const QString &endpoint, const QJsonObject &data, bool isPost = true);
};

#endif // NETWORKMANAGER_H
*/

// ======== 2. 修改 networkmanager.cpp ========

/*
#include "networkmanager.h"
#include <QEventLoop>
#include <QDebug>

// 初始化静态成员变量
NetworkManager* NetworkManager::m_instance = nullptr;

NetworkManager& NetworkManager::getInstance()
{
    if (!m_instance) {
        m_instance = new NetworkManager();
    }
    return *m_instance;
}

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

NetworkManager::~NetworkManager()
{
    delete manager;
}

bool NetworkManager::login(const QString &username, const QString &password)
{
    qDebug() << "尝试登录服务器: " << serverUrl;
    
    QJsonObject requestData;
    requestData["username"] = username;
    requestData["password"] = password;
    
    // 创建网络请求
    QUrl url(serverUrl + "/auth/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 转换数据为JSON格式
    QJsonDocument requestDoc(requestData);
    QByteArray requestDataBytes = requestDoc.toJson();
    
    // 创建事件循环以同步处理网络请求
    QEventLoop eventLoop;
    
    // 发送POST请求
    QNetworkReply *reply = manager->post(request, requestDataBytes);
    
    // 连接信号到事件循环
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    
    // 等待请求完成
    eventLoop.exec();
    
    // 处理响应
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        QJsonObject responseObj = responseDoc.object();
        
        qDebug() << "登录响应: " << responseObj;
        
        if (responseObj.contains("token")) {
            authToken = responseObj["token"].toString();
            qDebug() << "成功获取认证令牌";
            reply->deleteLater();
            return true;
        }
    } else {
        qDebug() << "登录请求失败: " << reply->errorString();
        emit networkError(reply->errorString());
    }
    
    reply->deleteLater();
    return false;
}

QVector<ExamRoom> NetworkManager::getExamRooms()
{
    QVector<ExamRoom> examRooms;
    
    if (authToken.isEmpty()) {
        qDebug() << "错误: 尝试获取考场信息时没有认证令牌";
        emit networkError("未登录，无法获取考场信息");
        return examRooms;
    }
    
    qDebug() << "尝试获取考场信息";
    
    // 创建网络请求
    QUrl url(serverUrl + "/teacher/examrooms");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(authToken).toUtf8());
    
    qDebug() << "请求URL: " << url.toString();
    qDebug() << "认证头: " << request.rawHeader("Authorization");
    
    // 创建事件循环以同步处理网络请求
    QEventLoop eventLoop;
    
    // 发送GET请求
    QNetworkReply *reply = manager->get(request);
    
    // 连接信号到事件循环
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    
    // 等待请求完成
    eventLoop.exec();
    
    // 处理响应
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        QJsonObject responseObj = responseDoc.object();
        
        qDebug() << "考场信息响应: " << responseObj;
        
        if (responseObj["status"].toString() == "success") {
            QJsonArray examRoomsArray = responseObj["data"].toArray();
            
            for (const QJsonValue &value : examRoomsArray) {
                QJsonObject obj = value.toObject();
                ExamRoom room;
                room.id = obj["id"].toInt();
                room.name = obj["name"].toString();
                room.location = obj["location"].toString();
                room.capacity = obj["capacity"].toInt();
                room.examDate = obj["examDate"].toString();
                room.examTime = obj["examTime"].toString();
                examRooms.append(room);
            }
        } else {
            qDebug() << "获取考场信息响应错误: " << responseObj["message"].toString();
            emit networkError(responseObj["message"].toString());
        }
    } else {
        qDebug() << "获取考场信息失败: " << reply->errorString();
        emit networkError(reply->errorString());
    }
    
    reply->deleteLater();
    return examRooms;
}

QVector<StudentInfo> NetworkManager::getStudentsForExam(int examRoomId)
{
    QVector<StudentInfo> students;
    
    if (authToken.isEmpty()) {
        qDebug() << "错误: 尝试获取学生名单时没有认证令牌";
        emit networkError("未登录，无法获取学生名单");
        return students;
    }
    
    QJsonObject requestData;
    requestData["examRoomId"] = examRoomId;
    
    // 创建网络请求
    QUrl url(serverUrl + "/attendance/students");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(authToken).toUtf8());
    
    // 转换数据为JSON格式
    QJsonDocument requestDoc(requestData);
    QByteArray requestDataBytes = requestDoc.toJson();
    
    // 创建事件循环以同步处理网络请求
    QEventLoop eventLoop;
    
    // 发送POST请求
    QNetworkReply *reply = manager->post(request, requestDataBytes);
    
    // 连接信号到事件循环
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    
    // 等待请求完成
    eventLoop.exec();
    
    // 处理响应
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        QJsonObject responseObj = responseDoc.object();
        
        if (responseObj["status"].toString() == "success") {
            QJsonArray studentsArray = responseObj["data"].toArray();
            
            for (const QJsonValue &value : studentsArray) {
                QJsonObject obj = value.toObject();
                StudentInfo student;
                student.id = obj["id"].toInt();
                student.studentId = obj["studentId"].toString();
                student.name = obj["name"].toString();
                student.seatNumber = obj["seatNumber"].toInt();
                student.attendanceStatus = obj["attendanceStatus"].toString();
                students.append(student);
            }
        } else {
            qDebug() << "获取学生名单响应错误: " << responseObj["message"].toString();
            emit networkError(responseObj["message"].toString());
        }
    } else {
        qDebug() << "获取学生名单失败: " << reply->errorString();
        emit networkError(reply->errorString());
    }
    
    reply->deleteLater();
    return students;
}

bool NetworkManager::uploadAttendanceRecord(int studentId, int examRoomId, const QDateTime &time)
{
    if (authToken.isEmpty()) {
        qDebug() << "错误: 尝试上传考勤记录时没有认证令牌";
        emit networkError("未登录，无法上传考勤记录");
        return false;
    }
    
    QJsonObject requestData;
    requestData["studentId"] = studentId;
    requestData["examRoomId"] = examRoomId;
    requestData["time"] = time.toString(Qt::ISODate);
    
    // 创建网络请求
    QUrl url(serverUrl + "/attendance/record");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(authToken).toUtf8());
    
    // 转换数据为JSON格式
    QJsonDocument requestDoc(requestData);
    QByteArray requestDataBytes = requestDoc.toJson();
    
    // 创建事件循环以同步处理网络请求
    QEventLoop eventLoop;
    
    // 发送POST请求
    QNetworkReply *reply = manager->post(request, requestDataBytes);
    
    // 连接信号到事件循环
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    
    // 等待请求完成
    eventLoop.exec();
    
    // 处理响应
    bool success = false;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        QJsonObject responseObj = responseDoc.object();
        
        success = responseObj["status"].toString() == "success";
        if (!success) {
            qDebug() << "上传考勤记录响应错误: " << responseObj["message"].toString();
            emit networkError(responseObj["message"].toString());
        }
    } else {
        qDebug() << "上传考勤记录失败: " << reply->errorString();
        emit networkError(reply->errorString());
    }
    
    reply->deleteLater();
    return success;
}
*/

// ======== 3. 修改 logindialog.cpp ========

/*
void LoginDialog::onLoginButtonClicked()
{
    username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    serverUrl = serverUrlEdit->text().trimmed();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "登录失败", "用户名和密码不能为空");
        return;
    }
    
    if (serverUrl.isEmpty()) {
        QMessageBox::warning(this, "登录失败", "请输入服务器地址");
        return;
    }
    
    NetworkManager::getInstance().setServerAddress(serverUrl);
    
    // 显示正在登录提示
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    // 尝试登录
    bool loginSuccess = NetworkManager::getInstance().login(username, password);
    
    // 恢复鼠标
    QApplication::restoreOverrideCursor();
    
    if (loginSuccess) {
        accept(); // 关闭登录对话框，返回接受结果
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误，或服务器连接失败");
    }
}
*/

// ======== 4. 使用指南 ========
/*
修复步骤：

1. 将networkmanager.h和networkmanager.cpp替换为上述代码
2. 修改logindialog.cpp中的onLoginButtonClicked方法
3. 确保更新了所有网络请求的实现，特别是认证相关的部分
4. 重新编译客户端应用程序
5. 确保服务器已启动(node server.js)
6. 使用正确的登录凭据测试:
   - 服务器: http://localhost:8080
   - 用户名: operator
   - 密码: operator123

调试提示:
- 检查控制台输出的调试信息
- 验证服务器是否正在运行
- 确认请求URL和认证令牌格式是否正确
*/ 