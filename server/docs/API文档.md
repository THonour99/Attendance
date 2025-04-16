# 人脸识别排座系统 - API文档

## API概述

人脸识别排座系统API采用RESTful设计风格，使用HTTP协议进行通信，数据格式为JSON。所有API（除登录接口外）都需要JWT认证。

## 基础信息

- **基础URL**: `http://localhost:8080`
- **认证方式**: JWT Bearer Token
- **默认数据格式**: JSON

## 认证机制

### 获取访问令牌

大多数API端点需要认证才能访问。客户端必须先通过登录API获取JWT令牌，然后在后续请求中使用此令牌。

**认证请求头格式**:
```
Authorization: Bearer <token>
```

## API端点

### 1. 用户认证

#### 1.1 用户登录

- **URL**: `/auth/login`
- **方法**: `POST`
- **认证要求**: 无
- **请求体**:
  ```json
  {
    "username": "operator",
    "password": "operator123"
  }
  ```
- **成功响应** (200 OK):
  ```json
  {
    "status": "success",
    "message": "登录成功",
    "token": "eyJhbGciOiJIUzI1NiIsInR5...",
    "user": {
      "id": 3,
      "username": "operator",
      "role": "operator"
    }
  }
  ```
- **错误响应** (401 Unauthorized):
  ```json
  {
    "status": "error",
    "message": "用户名或密码错误"
  }
  ```
- **备注**:
  - 成功登录后返回的JWT令牌有效期为24小时
  - 令牌过期后需要重新登录获取新令牌

### 2. 考场管理

#### 2.1 获取考场列表

- **URL**: `/teacher/examrooms`
- **方法**: `GET`
- **认证要求**: 是
- **请求参数**: 无
- **成功响应** (200 OK):
  ```json
  {
    "status": "success",
    "message": "获取考场信息成功",
    "data": [
      {
        "id": 1,
        "name": "计算机学院期末考试 - 教学楼A101",
        "location": "教学楼A101",
        "capacity": 60,
        "examDate": "2023-12-25",
        "examTime": "09:00-11:00"
      },
      {
        "id": 2,
        "name": "软件工程期末考试 - 教学楼B203",
        "location": "教学楼B203",
        "capacity": 45,
        "examDate": "2023-12-26",
        "examTime": "14:00-16:00"
      }
    ]
  }
  ```
- **错误响应** (401 Unauthorized):
  ```json
  {
    "status": "error",
    "message": "未提供认证令牌"
  }
  ```
- **错误响应** (403 Forbidden):
  ```json
  {
    "status": "error",
    "message": "令牌无效或已过期"
  }
  ```

### 3. 学生管理

#### 3.1 获取考场学生名单

- **URL**: `/attendance/students`
- **方法**: `POST`
- **认证要求**: 是
- **请求体**:
  ```json
  {
    "examRoomId": 1
  }
  ```
- **成功响应** (200 OK):
  ```json
  {
    "status": "success",
    "message": "获取学生名单成功",
    "data": [
      {
        "id": 101,
        "studentId": "2023100001",
        "name": "学生1-1",
        "seatNumber": 1,
        "attendanceStatus": "未到"
      },
      {
        "id": 102,
        "studentId": "2023100002",
        "name": "学生1-2",
        "seatNumber": 2,
        "attendanceStatus": "未到"
      }
    ]
  }
  ```
- **错误响应** (400 Bad Request):
  ```json
  {
    "status": "error",
    "message": "缺少考场ID"
  }
  ```
- **错误响应** (401/403):
  与上述认证错误响应相同

### 4. 考勤管理

#### 4.1 记录考勤

- **URL**: `/attendance/record`
- **方法**: `POST`
- **认证要求**: 是
- **请求体**:
  ```json
  {
    "studentId": 101,
    "examRoomId": 1,
    "time": "2023-12-25T09:05:00Z"
  }
  ```
- **成功响应** (200 OK):
  ```json
  {
    "status": "success",
    "message": "考勤记录添加成功",
    "data": {
      "id": 1,
      "studentId": 101,
      "examRoomId": 1,
      "time": "2023-12-25T09:05:00Z",
      "createdAt": "2023-12-25T09:05:10Z"
    }
  }
  ```
- **错误响应** (400 Bad Request):
  ```json
  {
    "status": "error",
    "message": "缺少必要参数"
  }
  ```
- **备注**:
  - `time`参数应采用ISO 8601格式的时间字符串
  - 系统会自动根据考试开始时间判断学生是否迟到

#### 4.2 获取考勤记录

- **URL**: `/attendance/records`
- **方法**: `GET`
- **认证要求**: 是
- **请求参数**: 无
- **成功响应** (200 OK):
  ```json
  {
    "status": "success",
    "message": "获取考勤记录成功",
    "data": [
      {
        "id": 1,
        "studentId": 101,
        "examRoomId": 1,
        "time": "2023-12-25T09:05:00Z",
        "createdAt": "2023-12-25T09:05:10Z"
      },
      {
        "id": 2,
        "studentId": 102,
        "examRoomId": 1,
        "time": "2023-12-25T09:10:00Z",
        "createdAt": "2023-12-25T09:10:05Z"
      }
    ]
  }
  ```

## 状态码说明

- **200 OK**: 请求成功
- **400 Bad Request**: 请求参数错误
- **401 Unauthorized**: 未提供认证信息
- **403 Forbidden**: 无权访问
- **404 Not Found**: 资源不存在
- **500 Internal Server Error**: 服务器内部错误

## 数据模型

### 用户对象

```json
{
  "id": 1,
  "username": "admin",
  "role": "admin"
}
```

### 考场对象

```json
{
  "id": 1,
  "name": "计算机学院期末考试 - 教学楼A101",
  "location": "教学楼A101",
  "capacity": 60,
  "examDate": "2023-12-25",
  "examTime": "09:00-11:00"
}
```

### 学生对象

```json
{
  "id": 101,
  "studentId": "2023100001",
  "name": "学生1-1",
  "seatNumber": 1,
  "attendanceStatus": "未到"
}
```

### 考勤记录对象

```json
{
  "id": 1,
  "studentId": 101,
  "examRoomId": 1,
  "time": "2023-12-25T09:05:00Z",
  "createdAt": "2023-12-25T09:05:10Z"
}
```

## 错误处理

所有API请求如果发生错误，将返回相应的HTTP状态码和包含错误信息的JSON响应：

```json
{
  "status": "error",
  "message": "错误信息描述"
}
```

## 使用示例

### 客户端示例 (JavaScript)

```javascript
// 登录获取令牌
async function login(username, password) {
  const response = await fetch('http://localhost:8080/auth/login', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json'
    },
    body: JSON.stringify({ username, password })
  });
  
  const data = await response.json();
  if (data.token) {
    localStorage.setItem('authToken', data.token);
    return true;
  }
  return false;
}

// 获取考场信息
async function getExamRooms() {
  const token = localStorage.getItem('authToken');
  
  const response = await fetch('http://localhost:8080/teacher/examrooms', {
    method: 'GET',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': `Bearer ${token}`
    }
  });
  
  return await response.json();
}
```

### 客户端示例 (C++/Qt)

```cpp
// 登录获取令牌
bool NetworkManager::login(const QString &username, const QString &password)
{
    QJsonObject requestData;
    requestData["username"] = username;
    requestData["password"] = password;
    
    QUrl url(serverUrl + "/auth/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QByteArray requestDataBytes = QJsonDocument(requestData).toJson();
    
    QNetworkReply *reply = manager->post(request, requestDataBytes);
    
    // 等待请求完成并处理响应
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
        QJsonObject responseObj = responseDoc.object();
        
        if (responseObj.contains("token")) {
            authToken = responseObj["token"].toString();
            reply->deleteLater();
            return true;
        }
    }
    
    reply->deleteLater();
    return false;
}
```

## 限制和注意事项

1. API请求频率限制: 无限制
2. 认证令牌有效期: 24小时
3. 最大请求体大小: 10MB
4. 服务器时区: UTC 