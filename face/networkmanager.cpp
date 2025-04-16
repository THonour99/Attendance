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