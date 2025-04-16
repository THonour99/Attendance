const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const jwt = require('jsonwebtoken');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = 8080;
const JWT_SECRET = 'face-recognition-system-secret';
const DATA_FILE = path.join(__dirname, 'data', 'database.json');

// 确保data目录存在
if (!fs.existsSync(path.join(__dirname, 'data'))) {
    fs.mkdirSync(path.join(__dirname, 'data'));
}

// 中间件
app.use(cors());
app.use(bodyParser.json());

// 日志中间件
app.use((req, res, next) => {
    console.log(`${new Date().toISOString()} - ${req.method} ${req.url}`);
    next();
});

// 初始化数据
let users = [
    { id: 1, username: 'admin', password: 'admin123', role: 'admin' },
    { id: 2, username: 'teacher', password: 'teacher123', role: 'teacher' },
    { id: 3, username: 'operator', password: 'operator123', role: 'operator' }
];

// 添加模拟班级数据
let classes = [
    { id: 1, className: '计算机科学与技术1班', teacher: '教师1', location: '教学楼A栋101' },
    { id: 2, className: '软件工程2班', teacher: '教师2', location: '教学楼B栋203' },
    { id: 3, className: '人工智能3班', teacher: '教师3', location: '教学楼C栋305' }
];

// 为教师分配班级
let teacherClasses = [
    { teacherId: 2, classIds: [1, 2] } // teacher用户管理班级1和2
];

// 为学生分配班级
let studentClasses = [
    { studentId: 3, classIds: [1, 3] } // operator用户(作为学生)属于班级1和3
];

// 班级学生名单
let classStudents = {
    1: [
        { id: 101, name: '学生1', studentId: '2023100001', className: '计算机科学与技术1班' },
        { id: 102, name: '学生2', studentId: '2023100002', className: '计算机科学与技术1班' }
    ],
    2: [
        { id: 201, name: '学生3', studentId: '2023200001', className: '软件工程2班' },
        { id: 202, name: '学生4', studentId: '2023200002', className: '软件工程2班' }
    ],
    3: [
        { id: 301, name: '学生5', studentId: '2023300001', className: '人工智能3班' },
        { id: 302, name: '学生6', studentId: '2023300002', className: '人工智能3班' }
    ]
};

let examRooms = [
    { 
        id: 1, 
        name: '计算机学院期末考试 - 教学楼A101', 
        location: '教学楼A101', 
        capacity: 60, 
        examDate: '2023-12-25', 
        examTime: '09:00-11:00' 
    },
    { 
        id: 2, 
        name: '软件工程期末考试 - 教学楼B203', 
        location: '教学楼B203', 
        capacity: 45, 
        examDate: '2023-12-26', 
        examTime: '14:00-16:00' 
    },
    { 
        id: 3, 
        name: '人工智能导论 - 综合楼C305', 
        location: '综合楼C305', 
        capacity: 30, 
        examDate: '2023-12-27', 
        examTime: '10:00-12:00' 
    }
];

// 生成模拟学生数据
function generateStudents(examRoomId, count) {
    const students = [];
    for (let i = 0; i < count; i++) {
        students.push({
            id: examRoomId * 100 + i + 1,
            studentId: `2023${examRoomId}${String(i + 1).padStart(3, '0')}`,
            name: `学生${examRoomId}-${i + 1}`,
            seatNumber: i + 1,
            attendanceStatus: '未到'
        });
    }
    return students;
}

// 为每个考场生成学生
const studentsMap = new Map();
examRooms.forEach(room => {
    studentsMap.set(room.id, generateStudents(room.id, room.capacity / 2)); // 生成一半容量的学生
});

// 考勤记录
let attendanceRecords = [];

// 数据持久化函数
function saveData() {
    const data = {
        users,
        classes,
        teacherClasses,
        studentClasses,
        classStudents,
        examRooms,
        attendanceRecords,
        lastUpdate: new Date().toISOString()
    };
    
    // 将学生数据转换为可序列化的格式
    const serializedStudentsMap = {};
    for (const [key, value] of studentsMap.entries()) {
        serializedStudentsMap[key] = value;
    }
    data.studentsMap = serializedStudentsMap;
    
    fs.writeFileSync(DATA_FILE, JSON.stringify(data, null, 2));
    console.log(`数据已保存到 ${DATA_FILE}`);
}

// 尝试加载持久化数据
function loadData() {
    try {
        if (fs.existsSync(DATA_FILE)) {
            const fileData = fs.readFileSync(DATA_FILE, 'utf8');
            const data = JSON.parse(fileData);
            
            // 恢复数据
            users = data.users || users;
            classes = data.classes || classes;
            teacherClasses = data.teacherClasses || teacherClasses;
            studentClasses = data.studentClasses || studentClasses;
            classStudents = data.classStudents || classStudents;
            examRooms = data.examRooms || examRooms;
            attendanceRecords = data.attendanceRecords || attendanceRecords;
            
            // 恢复studentsMap
            if (data.studentsMap) {
                studentsMap.clear();
                for (const [key, value] of Object.entries(data.studentsMap)) {
                    studentsMap.set(parseInt(key), value);
                }
            }
            
            console.log(`从 ${DATA_FILE} 加载了数据, 最后更新: ${data.lastUpdate || '未知'}`);
            return true;
        }
    } catch (error) {
        console.error(`加载数据失败: ${error.message}`);
    }
    return false;
}

// 启动时加载数据
loadData();

// 验证Token中间件
function authenticateToken(req, res, next) {
    const authHeader = req.headers['authorization'];
    const token = authHeader && authHeader.split(' ')[1];
    
    if (!token) {
        return res.status(401).json({ status: 'error', message: '未提供认证令牌' });
    }
    
    jwt.verify(token, JWT_SECRET, (err, user) => {
        if (err) {
            return res.status(403).json({ status: 'error', message: '令牌无效或已过期' });
        }
        req.user = user;
        next();
    });
}

// 令牌刷新接口
app.post('/auth/refresh', authenticateToken, (req, res) => {
    // 从请求中获取当前用户信息
    const user = req.user;
    
    // 生成新令牌（过期时间为24小时）
    const newToken = jwt.sign(
        { id: user.id, username: user.username, role: user.role }, 
        JWT_SECRET, 
        { expiresIn: '24h' }
    );
    
    // 返回新令牌
    res.json({
        status: 'success',
        message: '令牌已刷新',
        token: newToken
    });
});

// 登录接口
app.post('/auth/login', (req, res) => {
    const { username, password } = req.body;
    
    const user = users.find(u => u.username === username && u.password === password);
    if (!user) {
        return res.status(401).json({ status: 'error', message: '用户名或密码错误' });
    }
    
    const token = jwt.sign({ id: user.id, username: user.username, role: user.role }, JWT_SECRET, { expiresIn: '24h' });
    
    // 为不同角色的用户构建不同的响应信息
    const userInfo = {
        id: user.id,
        username: user.username,
        role: user.role
    };
    
    // 如果是学生，添加学号信息
    if (user.role === 'student') {
        // 查找此用户对应的学生信息
        for (const classId in classStudents) {
            const student = classStudents[classId].find(s => s.studentId && s.studentId.toString() === user.username);
            if (student) {
                userInfo.studentId = student.studentId;
                break;
            }
        }
        // 如果没找到对应学生信息，使用用户名作为学号
        if (!userInfo.studentId) {
            userInfo.studentId = user.username;
        }
    }
    
    res.json({
        status: 'success',
        message: '登录成功',
        token: token,
        user: userInfo
    });
});

// 获取考场信息
app.get('/teacher/examrooms', authenticateToken, (req, res) => {
    res.json({
        status: 'success',
        message: '获取考场信息成功',
        data: examRooms
    });
});

// 获取考场学生名单
app.post('/attendance/students', authenticateToken, (req, res) => {
    const { examRoomId } = req.body;
    
    if (!examRoomId) {
        return res.status(400).json({ status: 'error', message: '缺少考场ID' });
    }
    
    const students = studentsMap.get(parseInt(examRoomId)) || [];
    
    res.json({
        status: 'success',
        message: '获取学生名单成功',
        data: students
    });
});

// 记录考勤
app.post('/attendance/record', authenticateToken, (req, res) => {
    const { studentId, examRoomId, time } = req.body;
    
    if (!studentId || !examRoomId || !time) {
        return res.status(400).json({ status: 'error', message: '缺少必要参数' });
    }
    
    // 添加考勤记录
    const attendanceRecord = {
        id: attendanceRecords.length + 1,
        studentId,
        examRoomId,
        time,
        createdAt: new Date().toISOString()
    };
    attendanceRecords.push(attendanceRecord);
    
    // 更新学生考勤状态
    const students = studentsMap.get(parseInt(examRoomId)) || [];
    const student = students.find(s => s.id === parseInt(studentId));
    if (student) {
        const examRoom = examRooms.find(r => r.id === parseInt(examRoomId));
        if (examRoom) {
            const examStartTime = new Date(`${examRoom.examDate}T${examRoom.examTime.split('-')[0]}`);
            const recordTime = new Date(time);
            
            if (recordTime > new Date(examStartTime.getTime() + 15 * 60000)) {
                student.attendanceStatus = '迟到';
            } else {
                student.attendanceStatus = '已到';
            }
        } else {
            student.attendanceStatus = '已到';
        }
    }
    
    res.json({
        status: 'success',
        message: '考勤记录添加成功',
        data: attendanceRecord
    });
});

// 获取考勤记录
app.get('/attendance/records', authenticateToken, (req, res) => {
    res.json({
        status: 'success',
        message: '获取考勤记录成功',
        data: attendanceRecords
    });
});

// 学生获取班级信息
app.get('/student/classes', authenticateToken, (req, res) => {
    // 找到当前用户的班级
    const userClasses = [];
    const userId = req.user.id;
    
    // 查找该学生的班级
    const studentClass = studentClasses.find(sc => sc.studentId === userId);
    if (studentClass) {
        // 获取班级详细信息
        studentClass.classIds.forEach(classId => {
            const classInfo = classes.find(c => c.id === classId);
            if (classInfo) {
                userClasses.push(classInfo);
            }
        });
    }
    
    res.json({
        status: 'success',
        message: '获取班级信息成功',
        data: userClasses
    });
});

// 教师获取班级信息
app.get('/teacher/classes', authenticateToken, (req, res) => {
    // 找到当前教师管理的班级
    const teacherClassList = [];
    const userId = req.user.id;
    
    // 查找该教师的班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (teacher) {
        // 获取班级详细信息
        teacher.classIds.forEach(classId => {
            const classInfo = classes.find(c => c.id === classId);
            if (classInfo) {
                teacherClassList.push(classInfo);
            }
        });
    }
    
    res.json({
        status: 'success',
        message: '获取班级信息成功',
        data: teacherClassList
    });
});

// 教师获取班级学生
app.get('/teacher/classes/:classId/students', authenticateToken, (req, res) => {
    const { classId } = req.params;
    const userId = req.user.id;
    
    console.log(`请求班级(${classId})的学生列表，用户ID: ${userId}`);
    
    // 验证用户角色为教师
    if (req.user.role !== 'teacher') {
        console.log(`用户(${userId})不是教师，无权访问班级学生信息`);
        return res.status(403).json({ 
            status: 'error', 
            message: '只有教师可以访问班级学生信息' 
        });
    }
    
    // 获取班级信息
    const classInfo = classes.find(c => c.id === parseInt(classId));
    if (!classInfo) {
        console.log(`班级(${classId})不存在`);
        return res.status(404).json({ 
            status: 'error', 
            message: '班级不存在' 
        });
    }
    
    console.log(`允许教师(${userId})访问班级(${classId}): ${classInfo.className}`);
    
    // 获取班级学生
    const students = classStudents[classId] || [];
    console.log(`班级(${classId})学生数量: ${students.length}`);
    
    res.json({
        status: 'success',
        message: '获取班级学生成功',
        data: students
    });
});

// 教师添加班级
app.post('/teacher/classes', authenticateToken, (req, res) => {
    const { className } = req.body;
    const userId = req.user.id;
    
    if (!className) {
        return res.status(400).json({ 
            status: 'error', 
            message: '班级名称不能为空' 
        });
    }
    
    // 创建新班级
    const newClassId = classes.length + 1;
    const newClass = {
        id: newClassId,
        className,
        teacher: `教师${newClassId}`,
        location: '待定'
    };
    
    classes.push(newClass);
    
    // 将班级分配给教师
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (teacher) {
        teacher.classIds.push(newClassId);
        console.log(`已将班级 ${newClassId} 分配给教师 ${userId}，当前管理班级: ${teacher.classIds.join(', ')}`);
    } else {
        teacherClasses.push({
            teacherId: userId,
            classIds: [newClassId]
        });
        console.log(`创建新的教师-班级关联，教师 ${userId} 管理班级: ${newClassId}`);
    }
    
    // 保存数据到文件
    saveData();
    
    // 初始化班级学生名单
    classStudents[newClassId] = [];
    
    res.json({
        status: 'success',
        message: '班级添加成功',
        data: newClass
    });
});

// 教师添加学生到班级
app.post('/teacher/classes/:classId/students', authenticateToken, (req, res) => {
    const { classId } = req.params;
    const { studentId, name } = req.body;
    const userId = req.user.id;
    
    // 验证教师是否管理该班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (!teacher || !teacher.classIds.includes(parseInt(classId))) {
        return res.status(403).json({ 
            status: 'error', 
            message: '您没有权限操作该班级' 
        });
    }
    
    if (!studentId) {
        return res.status(400).json({ 
            status: 'error', 
            message: '学生学号不能为空' 
        });
    }
    
    // 获取班级信息
    const classInfo = classes.find(c => c.id === parseInt(classId));
    if (!classInfo) {
        return res.status(404).json({ 
            status: 'error', 
            message: '班级不存在' 
        });
    }
    
    // 检查学生是否已存在
    const students = classStudents[classId] || [];
    if (students.some(s => s.studentId === studentId)) {
        return res.status(400).json({ 
            status: 'error', 
            message: '该学生已在班级中' 
        });
    }
    
    // 添加学生
    const newStudent = {
        id: parseInt(classId) * 100 + students.length + 1,
        name: name || `学生${students.length + 1}`,
        studentId,
        className: classInfo.className
    };
    
    students.push(newStudent);
    classStudents[classId] = students;
    
    // 保存数据到文件
    saveData();
    
    res.json({
        status: 'success',
        message: '学生添加成功',
        data: newStudent
    });
});

// 从班级移除学生
app.delete('/teacher/classes/:classId/students/:studentId', authenticateToken, (req, res) => {
    const { classId, studentId } = req.params;
    const userId = req.user.id;
    
    // 验证教师是否管理该班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (!teacher || !teacher.classIds.includes(parseInt(classId))) {
        return res.status(403).json({ 
            status: 'error', 
            message: '您没有权限操作该班级' 
        });
    }
    
    // 获取班级学生
    const students = classStudents[classId] || [];
    
    // 查找要删除的学生
    const studentIndex = students.findIndex(s => s.id === parseInt(studentId));
    if (studentIndex === -1) {
        return res.status(404).json({ 
            status: 'error', 
            message: '学生不存在' 
        });
    }
    
    // 删除学生
    students.splice(studentIndex, 1);
    
    // 保存数据到文件
    saveData();
    
    res.json({
        status: 'success',
        message: '学生已从班级移除'
    });
});

// 获取考场考勤
app.get('/teacher/examrooms/:examRoomId/attendance', authenticateToken, (req, res) => {
    const { examRoomId } = req.params;
    
    // 获取考场信息
    const examRoom = examRooms.find(e => e.id === parseInt(examRoomId));
    if (!examRoom) {
        return res.status(404).json({ 
            status: 'error', 
            message: '考场不存在' 
        });
    }
    
    // 获取考勤记录
    const examAttendance = attendanceRecords.filter(record => record.examRoomId === parseInt(examRoomId));
    
    // 获取考场学生
    const students = studentsMap.get(parseInt(examRoomId)) || [];
    
    // 组合学生和考勤信息
    const attendanceData = students.map(student => {
        const record = examAttendance.find(r => r.studentId === student.id);
        
        return {
            id: student.id,
            studentId: student.studentId,
            studentName: student.name,
            status: student.attendanceStatus,
            checkInTime: record ? record.time : null
        };
    });
    
    res.json({
        status: 'success',
        message: '获取考场考勤成功',
        data: attendanceData
    });
});

// 添加考场
app.post('/teacher/examrooms', authenticateToken, (req, res) => {
    const { name, location, capacity, examDate, examTime } = req.body;
    
    // 验证必要参数
    if (!name) {
        return res.status(400).json({ 
            status: 'error', 
            message: '考场名称不能为空' 
        });
    }
    
    // 创建新考场
    const newExamRoomId = examRooms.length + 1;
    const newExamRoom = {
        id: newExamRoomId,
        name,
        location: location || '待定',
        capacity: capacity || 30,
        examDate: examDate || new Date().toISOString().split('T')[0],
        examTime: examTime || '09:00-11:00'
    };
    
    examRooms.push(newExamRoom);
    
    // 为新考场生成学生
    studentsMap.set(newExamRoomId, generateStudents(newExamRoomId, newExamRoom.capacity / 2));
    
    res.json({
        status: 'success',
        message: '考场添加成功',
        data: newExamRoom
    });
});

// 管理员获取用户列表
app.get('/admin/users', authenticateToken, (req, res) => {
    // 验证用户是否为管理员
    if (req.user.role !== 'admin') {
        return res.status(403).json({ status: 'error', message: '只有管理员可以访问用户列表' });
    }
    
    // 返回用户列表，但不包含密码
    const safeUsers = users.map(user => ({
        id: user.id,
        username: user.username,
        role: user.role,
        name: user.username, // 假设名称与用户名相同
        createdAt: new Date().toISOString() // 添加创建时间
    }));
    
    res.json({
        status: 'success',
        message: '获取用户列表成功',
        data: safeUsers
    });
});

// 管理员添加用户
app.post('/admin/users', authenticateToken, (req, res) => {
    // 验证用户是否为管理员
    if (req.user.role !== 'admin') {
        return res.status(403).json({ status: 'error', message: '只有管理员可以添加用户' });
    }
    
    const { username, password, name, role } = req.body;
    
    // 验证必要参数
    if (!username || !password || !role) {
        return res.status(400).json({ status: 'error', message: '缺少必要参数' });
    }
    
    // 检查用户名是否已存在
    if (users.some(u => u.username === username)) {
        return res.status(400).json({ status: 'error', message: '用户名已存在' });
    }
    
    // 创建新用户
    const newUser = {
        id: users.length + 1,
        username,
        password,
        role
    };
    
    users.push(newUser);
    
    // 保存数据到文件
    saveData();
    
    // 返回安全的用户信息（不包含密码）
    const safeUser = {
        id: newUser.id,
        username: newUser.username,
        role: newUser.role,
        name: name || newUser.username,
        createdAt: new Date().toISOString()
    };
    
    res.json({
        status: 'success',
        message: '用户添加成功',
        data: safeUser
    });
});

// 管理员编辑用户
app.put('/admin/users/:id', authenticateToken, (req, res) => {
    // 验证用户是否为管理员
    if (req.user.role !== 'admin') {
        return res.status(403).json({ status: 'error', message: '只有管理员可以编辑用户' });
    }
    
    const userId = parseInt(req.params.id);
    const { name, role, password } = req.body;
    
    // 查找用户
    const userIndex = users.findIndex(u => u.id === userId);
    if (userIndex === -1) {
        return res.status(404).json({ status: 'error', message: '用户不存在' });
    }
    
    // 更新用户信息
    const user = users[userIndex];
    
    if (name) user.name = name;
    if (role) user.role = role;
    if (password) user.password = password;
    
    // 保存数据到文件
    saveData();
    
    // 返回安全的用户信息
    const safeUser = {
        id: user.id,
        username: user.username,
        role: user.role,
        name: user.name || user.username,
        createdAt: new Date().toISOString()
    };
    
    res.json({
        status: 'success',
        message: '用户更新成功',
        data: safeUser
    });
});

// 管理员删除用户
app.delete('/admin/users/:id', authenticateToken, (req, res) => {
    // 验证用户是否为管理员
    if (req.user.role !== 'admin') {
        return res.status(403).json({ status: 'error', message: '只有管理员可以删除用户' });
    }
    
    const userId = parseInt(req.params.id);
    
    // 防止删除自己
    if (userId === req.user.id) {
        return res.status(400).json({ status: 'error', message: '不能删除当前登录的用户' });
    }
    
    // 查找用户
    const userIndex = users.findIndex(u => u.id === userId);
    if (userIndex === -1) {
        return res.status(404).json({ status: 'error', message: '用户不存在' });
    }
    
    // 删除用户
    users.splice(userIndex, 1);
    
    // 保存数据到文件
    saveData();
    
    res.json({
        status: 'success',
        message: '用户删除成功'
    });
});

// 数据库备份
app.get('/admin/database/backup', authenticateToken, (req, res) => {
    // 验证用户是否为管理员
    if (req.user.role !== 'admin') {
        return res.status(403).json({ status: 'error', message: '只有管理员可以备份数据库' });
    }
    
    // 创建备份数据
    const backup = {
        timestamp: new Date().toISOString(),
        version: '1.0',
        data: {
            users: users.map(user => ({...user})),
            classes,
            teacherClasses,
            studentClasses,
            classStudents,
            examRooms,
            attendanceRecords
        }
    };
    
    // 将备份数据转换为JSON字符串
    const backupJson = JSON.stringify(backup, null, 2);
    
    // 设置响应头，使浏览器下载文件
    res.setHeader('Content-Type', 'application/json');
    res.setHeader('Content-Disposition', `attachment; filename=backup-${backup.timestamp}.json`);
    
    // 发送备份数据
    res.send(backupJson);
});

// 数据库恢复
app.post('/admin/database/restore', authenticateToken, (req, res) => {
    // 验证用户是否为管理员
    if (req.user.role !== 'admin') {
        return res.status(403).json({ status: 'error', message: '只有管理员可以恢复数据库' });
    }
    
    try {
        // 解析恢复数据
        const backupData = req.body;
        
        // 验证备份数据格式
        if (!backupData || !backupData.data) {
            return res.status(400).json({ status: 'error', message: '无效的备份数据' });
        }
        
        // 恢复数据
        const { data } = backupData;
        
        // 保留原始管理员账户以确保可以登录
        const adminUser = users.find(u => u.role === 'admin');
        
        // 恢复用户数据(保留原始管理员)
        if (data.users && Array.isArray(data.users)) {
            // 过滤掉备份中的管理员账户
            const nonAdminUsers = data.users.filter(u => u.role !== 'admin');
            // 替换非管理员用户
            users.length = 0;
            users.push(adminUser);
            users.push(...nonAdminUsers);
        }
        
        // 恢复其他数据
        if (data.classes && Array.isArray(data.classes)) {
            classes.length = 0;
            classes.push(...data.classes);
        }
        
        if (data.teacherClasses && Array.isArray(data.teacherClasses)) {
            teacherClasses.length = 0;
            teacherClasses.push(...data.teacherClasses);
        }
        
        if (data.studentClasses && Array.isArray(data.studentClasses)) {
            studentClasses.length = 0;
            studentClasses.push(...data.studentClasses);
        }
        
        if (data.classStudents) {
            // 清空现有数据
            Object.keys(classStudents).forEach(key => {
                delete classStudents[key];
            });
            
            // 复制新数据
            Object.keys(data.classStudents).forEach(key => {
                classStudents[key] = data.classStudents[key];
            });
        }
        
        if (data.examRooms && Array.isArray(data.examRooms)) {
            examRooms.length = 0;
            examRooms.push(...data.examRooms);
        }
        
        if (data.attendanceRecords && Array.isArray(data.attendanceRecords)) {
            attendanceRecords.length = 0;
            attendanceRecords.push(...data.attendanceRecords);
        }
        
        res.json({
            status: 'success',
            message: '数据库恢复成功'
        });
    } catch (error) {
        res.status(500).json({
            status: 'error',
            message: '恢复数据库时出错: ' + error.message
        });
    }
});

// 启动服务器
app.listen(PORT, () => {
    console.log(`服务器已启动，监听端口 ${PORT}`);
    console.log(`访问地址: http://localhost:${PORT}`);
    console.log('可用账号:');
    console.log('- 管理员: admin / admin123');
    console.log('- 教师: teacher / teacher123');
    console.log('- 操作员: operator / operator123');
}); 