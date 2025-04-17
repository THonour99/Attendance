# 人脸识别排座系统 - 模拟服务器

这是一个简单的模拟服务器，用于测试人脸识别排座系统考勤端的网络功能。

## 功能

- 用户登录认证
- 考场信息获取
- 学生名单查询
- 考勤记录上传
- 考勤记录查询

## 安装与运行

### 安装依赖

```bash
cd server
npm install
```

### 启动服务器

```bash
npm start
```

服务器将在 http://localhost:8080 运行。

### 测试API

可以使用提供的测试脚本验证API是否正常工作：

```bash
node test-requests.js
```

## API接口说明

### 登录认证

- 路径: `/auth/login`
- 方法: POST
- 请求体: 
  ```json
  {
    "username": "操作员用户名",
    "password": "密码"
  }
  ```
- 响应: 
  ```json
  {
    "status": "success",
    "message": "登录成功",
    "token": "JWT令牌",
    "user": {
      "id": 3,
      "username": "operator",
      "role": "operator"
    }
  }
  ```

### 获取考场信息

- 路径: `/teacher/examrooms`
- 方法: GET
- 请求头: `Authorization: Bearer JWT令牌`
- 响应:
  ```json
  {
    "status": "success",
    "message": "获取考场信息成功",
    "data": [
      {
        "id": 1,
        "name": "考场名称",
        "location": "教学楼A101",
        "capacity": 60,
        "examDate": "2023-12-25",
        "examTime": "09:00-11:00"
      }
    ]
  }
  ```

### 获取考场学生名单

- 路径: `/attendance/students`
- 方法: POST
- 请求头: `Authorization: Bearer JWT令牌`
- 请求体:
  ```json
  {
    "examRoomId": 1
  }
  ```
- 响应:
  ```json
  {
    "status": "success",
    "message": "获取学生名单成功",
    "data": [
      {
        "id": 101,
        "studentId": "202310001",
        "name": "学生1-1",
        "seatNumber": 1,
        "attendanceStatus": "未到"
      }
    ]
  }
  ```

### 记录考勤

- 路径: `/attendance/record`
- 方法: POST
- 请求头: `Authorization: Bearer JWT令牌`
- 请求体:
  ```json
  {
    "studentId": 101,
    "examRoomId": 1,
    "time": "2023-12-25T09:00:00Z"
  }
  ```
- 响应:
  ```json
  {
    "status": "success",
    "message": "考勤记录添加成功",
    "data": {
      "id": 1,
      "studentId": 101,
      "examRoomId": 1,
      "time": "2023-12-25T09:00:00Z",
      "createdAt": "2023-12-24T12:00:00Z"
    }
  }
  ```

### 获取考勤记录

- 路径: `/attendance/records`
- 方法: GET
- 请求头: `Authorization: Bearer JWT令牌`
- 响应:
  ```json
  {
    "status": "success",
    "message": "获取考勤记录成功",
    "data": [
      {
        "id": 1,
        "studentId": 101,
        "examRoomId": 1,
        "time": "2023-12-25T09:00:00Z",
        "createdAt": "2023-12-24T12:00:00Z"
      }
    ]
  }
  ```

## 测试账号

系统预设了以下账号：

- 管理员: `admin` / `admin123`
- 教师: `teacher` / `teacher123`
- 操作员: `operator` / `operator123` 

学生账号信息
| 学号 | 密码 | 所属班级 |
|------|------|---------|
| 2023100001 | student123 | 计算机科学与技术1班 |
| 2023100002 | student123 | 计算机科学与技术1班 |
| 2023200001 | student123 | 软件工程2班 |
| 2023200002 | student123 | 软件工程2班 |
| 2023300001 | student123 | 人工智能3班 |
| 2023300002 | student123 | 人工智能3班 |
登录步骤