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
            
            // 保存用户信息
            loggedInUsername = username;
            
            // 从响应中直接获取用户ID
            if (responseObj.contains("user") && responseObj["user"].isObject()) {
                QJsonObject userObj = responseObj["user"].toObject();
                if (userObj.contains("id")) {
                    // 确保ID转换为字符串
                    if (userObj["id"].isDouble()) {
                        loggedInUserId = QString::number(userObj["id"].toInt());
                    } else {
                        loggedInUserId = userObj["id"].toString();
                    }
                    qDebug() << "从登录响应中获取用户ID:" << loggedInUserId;
                }
            }
            
            qDebug() << "成功获取认证令牌，用户名:" << loggedInUsername << "用户ID:" << loggedInUserId;
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

// 新增：获取用户信息
void NetworkManager::getUserInfo(const QString &username)
{
    if (authToken.isEmpty()) {
        qDebug() << "错误：尝试获取用户信息时没有认证令牌";
        return;
    }
    
    // 创建网络请求
    QUrl url(serverUrl + "/users/info?username=" + username);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(authToken).toUtf8());
    
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
        
        if (responseObj["status"].toString() == "success") {
            QJsonObject userData = responseObj["data"].toObject();
            loggedInUserId = userData["id"].toString();
            qDebug() << "获取用户ID成功:" << loggedInUserId;
        } else {
            qDebug() << "获取用户信息响应错误:" << responseObj["message"].toString();
        }
    } else {
        qDebug() << "获取用户信息失败:" << reply->errorString();
    }
    
    reply->deleteLater();
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
    
    // 创建网络请求 - 获取所有考场
    QUrl url = QUrl(serverUrl + "/teacher/examrooms");
    
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
                room.teacherId = obj["teacherId"].toString();
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
    
    // 只有在用户ID不为空时才添加
    if (!loggedInUserId.isEmpty()) {
        requestData["teacherId"] = loggedInUserId;  // 添加教师ID
    }
    
    requestData["status"] = "present";  // 默认状态：已到
    
    // 检查是否迟到
    // 尝试获取考试信息，判断是否迟到
    for (const auto &room : getExamRooms()) {
        if (room.id == examRoomId) {
            QString dateTimeStr = room.examDate + " " + room.examTime.split("-").first();
            QDateTime examStartTime = QDateTime::fromString(dateTimeStr, "yyyy-MM-dd hh:mm");
            
            if (examStartTime.isValid() && time > examStartTime.addSecs(15 * 60)) { // 15分钟后算迟到
                requestData["status"] = "late"; // 迟到
            }
            break;
        }
    }
    
    // 创建网络请求
    QUrl url(serverUrl + "/attendance/record");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(authToken).toUtf8());
    
    // 转换数据为JSON格式
    QJsonDocument requestDoc(requestData);
    QByteArray requestDataBytes = requestDoc.toJson();
    
    qDebug() << "上传考勤记录:" << QString::fromUtf8(requestDataBytes);
    
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
        if (success) {
            qDebug() << "考勤记录上传成功";
            
            // 发送更新通知到服务器，通知其他客户端更新
            sendUpdateNotification(examRoomId);
        } else {
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

// 发送更新通知，通知服务器数据已更新，需要推送给其他客户端
void NetworkManager::sendUpdateNotification(int examRoomId)
{
    if (authToken.isEmpty()) {
        qDebug() << "错误: 尝试发送更新通知时没有认证令牌";
        return;
    }
    
    QJsonObject requestData;
    requestData["examRoomId"] = examRoomId;
    requestData["type"] = "attendance_update";
    
    // 只在用户ID不为空时添加
    if (!loggedInUserId.isEmpty()) {
        requestData["senderId"] = loggedInUserId;
    }
    
    // 创建网络请求
    QUrl url(serverUrl + "/notification/push");
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
            qDebug() << "更新通知发送成功";
        } else {
            qDebug() << "更新通知发送错误: " << responseObj["message"].toString();
        }
    } else {
        qDebug() << "更新通知发送失败: " << reply->errorString();
    }
    
    reply->deleteLater();
} 