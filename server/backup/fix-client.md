# 客户端网络连接问题修复指南

从错误信息看，客户端在尝试获取考场信息时遇到了两个主要问题：
1. "Error transferring http://localhost:8080/teacher/examrooms - server replied: Not Found"
2. "Host requires authentication"

## 问题原因

1. **认证问题**：客户端没有先调用登录接口获取令牌，或者没有正确地将令牌附加到请求头中
2. **请求方式问题**：服务器API可能与客户端实现不匹配

## 修复步骤

### 1. 确认服务器正常运行

运行测试脚本确认服务器API工作正常：
```bash
node test-api.js
```

### 2. 修改NetworkManager实现

修改NetworkManager.cpp中的相关代码，确保正确的认证流程：

```cpp
// 1. 修改登录方法，确保正确获取令牌
bool NetworkManager::login(const QString &username, const QString &password) {
    // 准备请求数据
    QJsonObject requestData;
    requestData["username"] = username;
    requestData["password"] = password;
    
    // 创建网络请求
    QNetworkRequest request(QUrl(serverAddress + "/auth/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 发送POST请求
    QNetworkReply *reply = manager->post(request, QJsonDocument(requestData).toJson());
    
    // 等待请求完成
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    
    // 检查是否有错误
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "登录请求失败:" << reply->errorString();
        reply->deleteLater();
        return false;
    }
    
    // 解析响应
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject responseObj = doc.object();
    
    // 保存令牌
    if (responseObj.contains("token")) {
        authToken = responseObj["token"].toString();
        qDebug() << "成功获取认证令牌";
        reply->deleteLater();
        return true;
    }
    
    reply->deleteLater();
    return false;
}

// 2. 确保所有其他请求都附加了认证令牌
QVector<ExamRoom> NetworkManager::getExamRooms() {
    QVector<ExamRoom> examRooms;
    
    // 创建网络请求
    QNetworkRequest request(QUrl(serverAddress + "/teacher/examrooms"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 添加认证头
    if (!authToken.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(authToken).toUtf8());
    } else {
        qDebug() << "错误: 尝试获取考场信息时没有认证令牌";
        return examRooms;
    }
    
    // 发送GET请求
    QNetworkReply *reply = manager->get(request);
    
    // 等待请求完成
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    
    // 检查是否有错误
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "获取考场信息失败:" << reply->errorString();
        reply->deleteLater();
        return examRooms;
    }
    
    // 解析响应
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject responseObj = doc.object();
    
    // 确保响应成功
    if (responseObj["status"].toString() != "success") {
        qDebug() << "获取考场信息响应错误:" << responseObj["message"].toString();
        reply->deleteLater();
        return examRooms;
    }
    
    // 解析考场信息
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
    
    reply->deleteLater();
    return examRooms;
}
```

### 3. 修改登录流程

确保在登录界面中，用户登录成功后再请求其他数据：

```cpp
void LoginDialog::onLoginButtonClicked() {
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString serverUrl = ui->serverLineEdit->text();
    
    // 设置服务器地址
    NetworkManager::getInstance().setServerAddress(serverUrl);
    
    // 执行登录请求
    if (NetworkManager::getInstance().login(username, password)) {
        // 登录成功
        QDialog::accept();
    } else {
        // 登录失败
        QMessageBox::warning(this, "登录失败", "用户名或密码错误，或者服务器连接问题");
    }
}
```

### 4. 调试提示

如果仍然遇到问题，可以添加更多的调试输出：

```cpp
// 在NetworkManager中添加调试输出
QNetworkReply *reply = manager->get(request);
qDebug() << "发送GET请求:" << request.url().toString();
qDebug() << "认证头:" << request.rawHeader("Authorization");
```

## 测试验证

修改完成后：
1. 重新编译客户端
2. 启动服务器(`node server.js`)
3. 运行客户端，使用正确的登录凭据
4. 点击刷新考场按钮
5. 检查是否成功获取考场信息

如有进一步问题，请提供具体的错误信息或日志。 