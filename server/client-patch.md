# 客户端连接配置

为了让人脸识别排座系统考勤端连接到我们的模拟服务器，需要进行以下修改：

## 1. 服务器地址配置

在登录界面中，需要设置服务器地址为：
```
http://localhost:8080
```

## 2. 登录凭据

使用以下账号登录系统：
- 用户名：`operator`
- 密码：`operator123`

## 3. NetworkManager修改

如果需要修改NetworkManager文件以便对接我们的模拟服务器API，可以参考以下修改：

```cpp
// 修改登录接口实现
bool NetworkManager::login(const QString &username, const QString &password)
{
    QJsonObject requestData;
    requestData["username"] = username;
    requestData["password"] = password;
    
    QJsonDocument response = sendRequest("auth/login", requestData);
    
    if (response.isNull()) {
        return false;
    }
    
    QJsonObject responseObj = response.object();
    if (responseObj.contains("token")) {
        authToken = responseObj["token"].toString();
        return true;
    }
    
    return false;
}

// 修改获取考场信息接口实现
QVector<ExamRoom> NetworkManager::getExamRooms()
{
    QVector<ExamRoom> examRooms;
    
    QJsonDocument response = sendRequest("teacher/examrooms", QJsonObject());
    
    if (response.isNull()) {
        return examRooms; // 返回空列表
    }
    
    QJsonObject responseObj = response.object();
    if (responseObj["status"].toString() != "success" || !responseObj.contains("data")) {
        return examRooms;
    }
    
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
    
    return examRooms;
}

// 修改获取学生名单接口实现
QVector<StudentInfo> NetworkManager::getStudentsForExam(int examRoomId)
{
    QVector<StudentInfo> students;
    
    QJsonObject requestData;
    requestData["examRoomId"] = examRoomId;
    
    QJsonDocument response = sendRequest("attendance/students", requestData);
    
    if (response.isNull()) {
        return students; // 返回空列表
    }
    
    QJsonObject responseObj = response.object();
    if (responseObj["status"].toString() != "success" || !responseObj.contains("data")) {
        return students;
    }
    
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
    
    return students;
}

// 修改上传考勤记录接口实现
bool NetworkManager::uploadAttendanceRecord(int studentId, int examRoomId, const QDateTime &time)
{
    QJsonObject requestData;
    requestData["studentId"] = studentId;
    requestData["examRoomId"] = examRoomId;
    requestData["time"] = time.toString(Qt::ISODate);
    
    QJsonDocument response = sendRequest("attendance/record", requestData);
    
    if (response.isNull()) {
        return false;
    }
    
    QJsonObject responseObj = response.object();
    return responseObj["status"].toString() == "success";
}
```

## 4. 调试提示

如果在连接过程中遇到问题，可以检查以下几点：

1. 确保服务器正在运行并监听8080端口
2. 检查网络连接和防火墙设置
3. 在服务器控制台输出中查看请求日志
4. 确保请求和响应格式正确

## 5. 模拟学生数据

服务器为每个考场生成了一些模拟学生数据。每个学生的名称格式为：`学生{考场ID}-{序号}`。

例如，考场1的第一个学生名称为"学生1-1"。这与人脸识别系统中的姓名需要匹配才能正确识别。

## 6. 现有限制

由于是模拟服务器，目前有一些限制：

1. 不支持文件上传和下载
2. 不保存人脸数据到服务器
3. 数据存储在内存中，服务器重启后数据会丢失

这些限制不影响基本功能测试。 