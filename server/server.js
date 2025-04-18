const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const jwt = require('jsonwebtoken');
const fs = require('fs');
const path = require('path');
const multer = require('multer');

const app = express();
const PORT = 8080;
const JWT_SECRET = 'face-recognition-system-secret';
const DATA_FILE = path.join(__dirname, 'data', 'database.json');

// 配置文件上传存储
const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    const uploadDir = path.join(__dirname, 'uploads');
    // 确保上传目录存在
    if (!fs.existsSync(uploadDir)) {
      fs.mkdirSync(uploadDir, { recursive: true });
    }
    cb(null, uploadDir);
  },
  filename: function (req, file, cb) {
    // 使用学生ID作为文件名前缀
    const userId = req.user ? req.user.id : 'unknown';
    cb(null, `student_${userId}_${Date.now()}${path.extname(file.originalname)}`);
  }
});

// 创建multer实例
const upload = multer({ 
  storage: storage,
  limits: {
    fileSize: 5 * 1024 * 1024, // 限制5MB
  },
  fileFilter: function (req, file, cb) {
    // 只接受图片文件
    if (!file.originalname.match(/\.(jpg|jpeg|png|gif)$/)) {
      return cb(new Error('只允许上传图片文件!'), false);
    }
    cb(null, true);
  }
});

// 确保data目录存在
if (!fs.existsSync(path.join(__dirname, 'data'))) {
    fs.mkdirSync(path.join(__dirname, 'data'));
}

// 确保uploads目录存在
if (!fs.existsSync(path.join(__dirname, 'uploads'))) {
    fs.mkdirSync(path.join(__dirname, 'uploads'));
}

// 中间件
app.use(cors());
app.use(bodyParser.json());

// 提供静态文件访问，用于照片预览
app.use('/uploads', express.static(path.join(__dirname, 'uploads')));

// 日志中间件
app.use((req, res, next) => {
    console.log(`${new Date().toISOString()} - ${req.method} ${req.url}`);
    next();
});

// 全局数据变量
let users = [];
let classes = [];
let teacherClasses = [];
let studentClasses = [];
let classStudents = {}; 
let examRooms = [];
let examRoomClasses = {};
let examRoomSeats = {};
let studentPhotos = {};
let attendanceRecords = [];
let studentsMap = new Map();
let data = {}; // 存储完整数据
// 通知系统数据
let notifications = [];

// 初始化数据
// 原有的用户数据
users = [
    { id: 1, username: 'admin', password: 'admin123', role: 'admin' },
    { id: 2, username: 'teacher', password: 'teacher123', role: 'teacher' },
    { id: 3, username: 'operator', password: 'operator123', role: 'operator' },
    // 添加学生账号
    { id: 4, username: '2023100001', password: 'student123', role: 'student' },
    { id: 5, username: '2023100002', password: 'student123', role: 'student' },
    { id: 6, username: '2023200001', password: 'student123', role: 'student' },
    { id: 7, username: '2023200002', password: 'student123', role: 'student' },
    { id: 8, username: '2023300001', password: 'student123', role: 'student' },
    { id: 9, username: '2023300002', password: 'student123', role: 'student' }
];

// 添加模拟班级数据
classes = [
    { id: 1, className: '计算机科学与技术1班', teacher: '教师1', location: '教学楼A栋101' },
    { id: 2, className: '软件工程2班', teacher: '教师2', location: '教学楼B栋203' },
    { id: 3, className: '人工智能3班', teacher: '教师3', location: '教学楼C栋305' }
];

// 为教师分配班级
teacherClasses = [
    { teacherId: 2, classIds: [1, 2] } // teacher用户管理班级1和2
];

// 为学生分配班级
studentClasses = [
    { studentId: 3, classIds: [1, 3] }, // operator用户(作为学生)属于班级1和3
    { studentId: 4, classIds: [1] },    // 学生1属于班级1
    { studentId: 5, classIds: [1] },    // 学生2属于班级1
    { studentId: 6, classIds: [2] },    // 学生3属于班级2
    { studentId: 7, classIds: [2] },    // 学生4属于班级2
    { studentId: 8, classIds: [3] },    // 学生5属于班级3
    { studentId: 9, classIds: [3] }     // 学生6属于班级3
];

// 班级学生名单
classStudents = {
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

examRooms = [
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

// 存储考场与班级的关联关系
examRoomClasses = {};

// 存储学生照片信息
studentPhotos = {};

// 存储考场的座位安排
examRoomSeats = {};

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
studentsMap = new Map();
examRooms.forEach(room => {
    studentsMap.set(room.id, generateStudents(room.id, room.capacity / 2)); // 生成一半容量的学生
});

// 考勤记录
attendanceRecords = [];

// 数据持久化函数
function saveData() {
    try {
        // 准备要保存的数据
        data = {
            users,
            classes,
            teacherClasses,
            studentClasses,
            classStudents,
            examRooms,
            attendanceRecords,
            examRoomClasses,
            studentPhotos,
            examRoomSeats,
            notifications, // 添加通知数据
            lastUpdate: new Date().toISOString()
        };

        // 序列化studentsMap
        const serializedStudentsMap = {};
        studentsMap.forEach((value, key) => {
            serializedStudentsMap[key] = value;
        });
        data.studentsMap = serializedStudentsMap;
        
        try {
            fs.writeFileSync(DATA_FILE, JSON.stringify(data, null, 2));
            console.log(`数据已保存到 ${DATA_FILE}`);
        } catch (error) {
            console.error('保存数据出错:', error);
        }
    } catch (error) {
        console.error(`保存数据出错: ${error.message}`);
    }
}

// 尝试加载持久化数据
function loadData() {
    try {
        if (fs.existsSync(DATA_FILE)) {
            const fileData = fs.readFileSync(DATA_FILE, 'utf8');
            data = JSON.parse(fileData);
            
            // 恢复数据
            users = data.users || users;
            classes = data.classes || classes;
            teacherClasses = data.teacherClasses || teacherClasses;
            studentClasses = data.studentClasses || studentClasses;
            classStudents = data.classStudents || classStudents;
            examRooms = data.examRooms || examRooms;
            attendanceRecords = data.attendanceRecords || attendanceRecords;
            examRoomClasses = data.examRoomClasses || {};
            studentPhotos = data.studentPhotos || {};
            examRoomSeats = data.examRoomSeats || {};
            
            // 加载通知数据
            if (data.notifications) {
                notifications = data.notifications;
                console.log(`已加载 ${notifications.length} 条通知数据`);
            }
            
            // 恢复studentsMap
            studentsMap = new Map();
            
            if (data.studentsMap) {
                Object.keys(data.studentsMap).forEach(key => {
                    // 确保数组不为空
                    if (data.studentsMap[key] && Array.isArray(data.studentsMap[key])) {
                        studentsMap.set(parseInt(key), data.studentsMap[key]);
                        console.log(`加载考场ID=${key}的学生数据: ${data.studentsMap[key].length}条记录`);
                    }
                });
            } else {
                console.log('数据文件中缺少studentsMap字段，将使用默认空Map');
            }
            
            console.log(`从 ${DATA_FILE} 加载了数据, 最后更新: ${data.lastUpdate || '未知'}`);
            console.log(`加载了 ${Object.keys(data.studentsMap || {}).length} 个考场的学生数据`);
            return true;
        } else {
            console.log('数据文件不存在，初始化默认数据');
            initializeDefaultData();
            return false;
        }
    } catch (error) {
        console.error('加载数据出错:', error);
        initializeDefaultData();
        return false;
    }
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
    
    console.log(`尝试登录: 用户名=${username}, 密码长度=${password ? password.length : 0}`);
    
    // 查找匹配用户
    const user = users.find(u => u.username === username && u.password === password);
    if (!user) {
        console.log(`登录失败: 用户名或密码错误 - ${username}`);
        return res.status(401).json({ status: 'error', message: '用户名或密码错误' });
    }
    
    console.log(`用户找到: ID=${user.id}, 角色=${user.role}`);
    
    // 生成令牌
    const token = jwt.sign({ id: user.id, username: user.username, role: user.role }, JWT_SECRET, { expiresIn: '24h' });
    
    // 为不同角色的用户构建不同的响应信息
    const userInfo = {
        id: user.id,
        username: user.username,
        role: user.role
    };
    
    // 如果是学生，添加学号信息
    if (user.role === 'student') {
        // 以用户名作为学号
        userInfo.studentId = user.username;
        
        // 查找此用户对应的学生信息
        for (const classId in classStudents) {
            const student = classStudents[classId].find(s => s.studentId && s.studentId.toString() === user.username);
            if (student) {
                console.log(`找到学生信息: 班级=${classId}, 学号=${student.studentId}, 姓名=${student.name}`);
                userInfo.studentId = student.studentId;
                break;
            }
        }
        
        console.log(`学生登录成功: 学号=${userInfo.studentId}`);
    }
    
    // 发送成功响应
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
    
    console.log(`尝试从班级 ${classId} 移除学生 ${studentId}, 请求来自用户ID: ${userId}`);
    
    // 验证教师是否管理该班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (!teacher || !teacher.classIds.includes(parseInt(classId))) {
        console.log(`拒绝：用户(${userId})没有权限操作班级(${classId})`);
        return res.status(403).json({ 
            status: 'error', 
            message: '您没有权限操作该班级' 
        });
    }
    
    // 验证班级是否存在
    if (!classStudents[classId]) {
        console.log(`拒绝：班级ID(${classId})不存在`);
        return res.status(404).json({
            status: 'error',
            message: '班级不存在'
        });
    }
    
    // 获取班级学生
    const students = classStudents[classId] || [];
    console.log(`班级(${classId})的学生数量: ${students.length}`);
    
    // 查找要删除的学生 - 同时支持数字ID和字符串ID
    const studentIdNum = parseInt(studentId);
    let studentIndex = students.findIndex(s => s.id === studentIdNum);
    
    // 记录查找过程
    console.log(`按数字ID(${studentIdNum})查找学生, 索引: ${studentIndex}`);
    
    // 如果按照数字ID没找到，尝试按照其他方式查找
    if (studentIndex === -1) {
        // 尝试按照studentId字段查找
        studentIndex = students.findIndex(s => s.studentId && s.studentId.toString() === studentId);
        console.log(`按学号(${studentId})查找学生, 索引: ${studentIndex}`);
        
        // 如果还是没找到，尝试按照name查找
        if (studentIndex === -1) {
            studentIndex = students.findIndex(s => s.name && s.name.toString() === studentId);
            console.log(`按姓名(${studentId})查找学生, 索引: ${studentIndex}`);
        }
    }
    
    // 如果仍然找不到
    if (studentIndex === -1) {
        // 记录所有学生信息以便调试
        console.log(`未找到学生ID: ${studentId}`);
        console.log('当前班级学生列表:');
        students.forEach((s, i) => {
            console.log(`  学生 #${i}: id=${s.id}, studentId=${s.studentId}, name=${s.name}`);
        });
        
        return res.status(404).json({ 
            status: 'error', 
            message: '学生不存在' 
        });
    }
    
    // 获取要删除的学生信息用于日志
    const studentToRemove = students[studentIndex];
    console.log(`找到学生: id=${studentToRemove.id}, studentId=${studentToRemove.studentId}, name=${studentToRemove.name}`);
    
    // 删除学生
    students.splice(studentIndex, 1);
    
    // 保存数据到文件
    saveData();
    
    console.log(`成功从班级(${classId})移除学生(${studentToRemove.name})`);
    
    res.json({
        status: 'success',
        message: '学生已从班级移除',
        data: {
            removedStudent: {
                name: studentToRemove.name,
                studentId: studentToRemove.studentId
            }
        }
    });
});

// 获取考场考勤
app.get('/teacher/examrooms/:examRoomId/attendance', authenticateToken, (req, res) => {
    const { examRoomId } = req.params;
    
    console.log(`获取考场ID=${examRoomId}的考勤信息`);
    
    // 获取考场信息
    const examRoom = examRooms.find(e => e.id === parseInt(examRoomId));
    if (!examRoom) {
        console.log(`考场ID=${examRoomId}不存在`);
        return res.status(404).json({ 
            status: 'error', 
            message: '考场不存在' 
        });
    }
    
    console.log(`找到考场: ${examRoom.name}`);
    
    // 确保studentsMap正确初始化
    if (!studentsMap) {
        console.log('studentsMap未初始化，正在创建新的Map');
        studentsMap = new Map();
    }
    
    // 获取考勤记录
    const examAttendance = attendanceRecords.filter(record => record.examRoomId === parseInt(examRoomId));
    console.log(`找到${examAttendance.length}条考勤记录`);
    
    // 获取考场学生 - 先尝试数字key
    let students = studentsMap.get(parseInt(examRoomId));
    
    // 如果没有找到，尝试字符串key
    if (!students) {
        students = studentsMap.get(examRoomId);
    }
    
    // 如果仍然没有，尝试从序列化的studentsMap中获取
    if (!students && data && data.studentsMap && data.studentsMap[examRoomId]) {
        students = data.studentsMap[examRoomId];
        console.log(`从序列化数据中获取学生数据: ${students.length}条记录`);
    }
    
    // 如果仍然找不到，使用空数组
    if (!students) {
        console.log(`考场ID=${examRoomId}没有找到学生数据，检查data.studentsMap:`, Object.keys(data.studentsMap || {}));
        students = [];
    } else {
        console.log(`找到${students.length}名学生`);
    }
    
    // 组合学生和考勤信息
    const attendanceData = students.map(student => {
        const record = examAttendance.find(r => r.studentId === student.id);
        
        return {
            id: student.id,
            studentId: student.studentId,
            studentName: student.name,
            status: student.attendanceStatus || '未到',
            checkInTime: record ? record.time : null
        };
    });
    
    console.log(`返回${attendanceData.length}条考勤数据`);
    
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
            attendanceRecords,
            examRoomClasses,
            studentPhotos,
            examRoomSeats
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
        
        if (data.examRoomClasses) {
            examRoomClasses = data.examRoomClasses;
        }
        
        if (data.studentPhotos) {
            studentPhotos = data.studentPhotos;
        }
        
        if (data.examRoomSeats) {
            examRoomSeats = data.examRoomSeats;
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

// 关联班级到考场
app.post('/teacher/examrooms/:examRoomId/classes', authenticateToken, (req, res) => {
    const { examRoomId } = req.params;
    const { classIds } = req.body;
    const userId = req.user.id;
    
    console.log(`尝试关联班级到考场: examRoomId=${examRoomId}, classIds=${JSON.stringify(classIds)}, userId=${userId}`);
    
    // 验证用户是否为教师
    if (req.user.role !== 'teacher' && req.user.role !== 'admin') {
        console.log(`拒绝: 用户(${userId})不是教师或管理员`);
        return res.status(403).json({ 
            status: 'error', 
            message: '只有教师和管理员可以管理考场' 
        });
    }
    
    // 验证考场是否存在
    const examRoom = examRooms.find(e => e.id === parseInt(examRoomId));
    if (!examRoom) {
        console.log(`拒绝: 考场(${examRoomId})不存在`);
        return res.status(404).json({ 
            status: 'error', 
            message: '考场不存在' 
        });
    }
    
    // 验证班级ID是否有效
    if (!classIds || !Array.isArray(classIds)) {
        console.log(`拒绝: 无效的班级ID列表格式`);
        return res.status(400).json({
            status: 'error',
            message: '请提供有效的班级ID列表'
        });
    }
    
    // 过滤掉空字符串和无效ID
    const validClassIds = classIds
        .filter(id => id !== undefined && id !== null && id !== '')
        .map(id => parseInt(id))
        .filter(id => !isNaN(id));
    
    console.log(`过滤后的有效班级ID: ${JSON.stringify(validClassIds)}`);
    
    if (validClassIds.length === 0) {
        console.log(`拒绝: 没有有效的班级ID`);
        return res.status(400).json({
            status: 'error',
            message: '请至少提供一个有效的班级ID'
        });
    }
    
    // 确保班级存在
    const nonExistentClasses = validClassIds.filter(id => !classes.some(c => c.id === id));
    
    if (nonExistentClasses.length > 0) {
        console.log(`拒绝: 班级不存在 - ${nonExistentClasses.join(', ')}`);
        return res.status(404).json({
            status: 'error',
            message: `以下班级不存在: ${nonExistentClasses.join(', ')}`
        });
    }
    
    // 关联班级到考场
    examRoomClasses[examRoomId] = validClassIds;
    console.log(`成功关联班级: ${validClassIds.join(', ')} 到考场 ${examRoomId}`);
    
    // 保存数据
    saveData();
    
    res.json({
        status: 'success',
        message: '班级已成功关联到考场',
        data: {
            examRoomId: parseInt(examRoomId),
            classIds: examRoomClasses[examRoomId]
        }
    });
});

// 获取考场关联的班级
app.get('/teacher/examrooms/:examRoomId/classes', authenticateToken, (req, res) => {
    const { examRoomId } = req.params;
    
    // 验证考场是否存在
    const examRoom = examRooms.find(e => e.id === parseInt(examRoomId));
    if (!examRoom) {
        return res.status(404).json({ 
            status: 'error', 
            message: '考场不存在' 
        });
    }
    
    // 获取关联的班级ID
    const classIds = examRoomClasses[examRoomId] || [];
    
    // 获取班级详细信息
    const classesInfo = classes.filter(c => classIds.includes(c.id));
    
    res.json({
        status: 'success',
        message: '获取考场关联班级成功',
        data: classesInfo
    });
});

// 上传学生照片
app.post('/student/photo', authenticateToken, (req, res) => {
    const { photoData } = req.body;
    const userId = req.user.id;
    
    if (!photoData) {
        return res.status(400).json({ 
            status: 'error', 
            message: '未提供照片数据' 
        });
    }
    
    // 存储照片
    studentPhotos[userId] = photoData;
    
    // 保存数据
    saveData();
    
    res.json({
        status: 'success',
        message: '照片上传成功'
    });
});

// 获取学生照片
app.get('/teacher/student/:studentId/photo', authenticateToken, (req, res) => {
    const { studentId } = req.params;
    const teacherId = req.user.id;
    
    // 验证教师权限
    if (req.user.role !== 'teacher' && req.user.role !== 'admin') {
        return res.json({ status: 403, message: '没有权限获取学生照片' });
    }
    
    // 查找学生所在班级和照片信息
    let studentRecord = null;
    let studentClassId = null;
    
    // 遍历所有班级，查找学生记录
    Object.keys(classStudents).forEach(classId => {
        const students = classStudents[classId];
        const foundStudent = students.find(s => s.id === parseInt(studentId));
        if (foundStudent) {
            studentRecord = foundStudent;
            studentClassId = classId;
        }
    });
    
    if (!studentRecord) {
        return res.json({ status: 404, message: '学生信息不存在' });
    }
    
    // 验证教师是否有权限查看该班级的学生照片
    if (req.user.role === 'teacher') {
        const teacherClass = teacherClasses.find(tc => tc.teacherId === teacherId);
        if (!teacherClass || !teacherClass.classIds.includes(parseInt(studentClassId))) {
            return res.json({ status: 403, message: '您没有权限查看该学生照片' });
        }
    }
    
    // 检查照片是否存在
    if (!studentRecord.photoUrl) {
        return res.json({ status: 404, message: '学生照片不存在' });
    }
    
    return res.json({ 
        status: 200, 
        message: '获取学生照片成功', 
        data: studentRecord.photoUrl
    });
});

// 生成考场座位
app.post('/teacher/examrooms/:examRoomId/seats', authenticateToken, (req, res) => {
    const { examRoomId } = req.params;
    const { arrangementType, customArrangement } = req.body;
    
    // 验证教师权限
    if (req.user.role !== 'teacher') {
        return res.status(403).json({ 
            status: 'error', 
            message: '只有教师可以管理座位安排' 
        });
    }
    
    // 验证考场是否存在
    const examRoom = examRooms.find(e => e.id === parseInt(examRoomId));
    if (!examRoom) {
        return res.status(404).json({ 
            status: 'error', 
            message: '考场不存在' 
        });
    }
    
    // 获取考场关联的班级
    const classIds = examRoomClasses[examRoomId] || [];
    if (classIds.length === 0) {
        return res.status(400).json({ 
            status: 'error', 
            message: '请先为考场关联班级' 
        });
    }
    
    // 收集所有关联班级的学生
    let allStudents = [];
    classIds.forEach(classId => {
        const students = classStudents[classId] || [];
        allStudents = allStudents.concat(students.map(student => ({
            ...student,
            classId
        })));
    });
    
    // 根据排座方式安排座位
    let generatedSeats = [];
    switch (arrangementType) {
        case 'random':
            // 随机排座
            generatedSeats = arrangeRandomSeats(allStudents, examRoom.capacity);
            break;
        case 'byClass':
            // 按班级排座
            generatedSeats = arrangeByClassSeats(allStudents, classIds, examRoom.capacity);
            break;
        case 'byGrade':
            // 成绩加权排序
            generatedSeats = arrangeByGradeSeats(allStudents, examRoom.capacity);
            break;
        case 'custom':
            // 自定义排座
            if (!customArrangement || !Array.isArray(customArrangement)) {
                return res.status(400).json({
                    status: 'error',
                    message: '自定义排座需要提供有效的座位安排'
                });
            }
            generatedSeats = customArrangement;
            break;
        default:
            return res.status(400).json({
                status: 'error',
                message: '无效的排座方式'
            });
    }
    
    // 使用嵌套结构存储座位安排，使其与学生API期望的格式一致
    examRoomSeats[examRoomId] = {
        arrangementType: arrangementType,
        createdAt: new Date().toISOString(),
        seats: generatedSeats,
        examRoomId: parseInt(examRoomId)
    };
    
    // 更新考场学生数据
    updateExamRoomStudents(examRoomId, generatedSeats);
    
    // 保存数据
    saveData();
    console.log(`为考场${examRoomId}生成座位安排完成，共${generatedSeats.length}个座位`);
    
    res.json({
        status: 'success',
        message: '座位安排成功',
        data: {
            examRoomId: parseInt(examRoomId),
            seats: generatedSeats
        }
    });
});

// 获取考场座位安排
app.get('/teacher/examrooms/:examRoomId/seats', authenticateToken, (req, res) => {
    const { examRoomId } = req.params;
    
    // 验证考场是否存在
    const examRoom = examRooms.find(e => e.id === parseInt(examRoomId));
    if (!examRoom) {
        return res.status(404).json({ 
            status: 'error', 
            message: '考场不存在' 
        });
    }
    
    // 获取座位安排
    const seatsData = examRoomSeats[examRoomId] || { seats: [] };
    
    console.log(`获取考场${examRoomId}座位安排，数据:`, 
        seatsData.seats ? `${seatsData.seats.length}个座位` : '无座位数据');
    
    res.json({
        status: 'success',
        message: '获取座位安排成功',
        data: seatsData.seats || []
    });
});

// 学生获取自己的座位信息
app.get('/student/examroom/seat', authenticateToken, (req, res) => {
    const userId = req.user.id;
    const username = req.user.username;
    console.log(`获取学生座位信息请求 - 用户: ${userId}, 用户名: ${username}`);
    
    // 在所有考场中查找学生座位
    let studentSeat = null;
    let examRoomInfo = null;
    
    // 首先，找到学生在哪些考场有座位
    for (const [examRoomId, seatsData] of Object.entries(examRoomSeats)) {
        if (!seatsData || !seatsData.seats || !Array.isArray(seatsData.seats)) {
            console.log(`考场 ${examRoomId} 的座位数据无效`);
            continue;
        }
        
        // 尝试多种方式匹配学生ID
        const seat = seatsData.seats.find(s => 
            s.studentId == userId || 
            (username && s.studentId == username) ||
            (s.studentNumber && s.studentNumber == username)
        );
        
        if (seat) {
            studentSeat = seat;
            examRoomInfo = examRooms.find(e => e.id === parseInt(examRoomId));
            console.log(`找到学生座位 - 学生: ${username}, 考场: ${examRoomId}, 座位: ${JSON.stringify(seat)}`);
            break;
        }
    }
    
    if (!studentSeat || !examRoomInfo) {
        console.log(`未找到学生座位信息 - 学生: ${username}`);
        return res.json({
            status: 'success',
            message: '未找到座位信息',
            data: null
        });
    }
    
    res.json({
        status: 'success',
        message: '获取座位信息成功',
        data: {
            examRoom: examRoomInfo,
            seat: studentSeat
        }
    });
});

// 辅助函数: 随机排座
function arrangeRandomSeats(students, capacity) {
    // 打乱学生顺序
    const shuffledStudents = [...students].sort(() => 0.5 - Math.random());
    
    // 分配座位
    return shuffledStudents.slice(0, capacity).map((student, index) => ({
        seatId: index + 1,
        studentId: student.id,
        studentName: student.name,
        studentNumber: student.studentId,
        className: student.className
    }));
}

// 辅助函数: 按班级排座
function arrangeByClassSeats(students, classIds, capacity) {
    let seats = [];
    let seatId = 1;
    
    // 按班级顺序排座
    for (const classId of classIds) {
        const classStudents = students.filter(s => s.classId === classId);
        
        for (const student of classStudents) {
            if (seats.length >= capacity) break;
            
            seats.push({
                seatId: seatId++,
                studentId: student.id,
                studentName: student.name,
                studentNumber: student.studentId,
                className: student.className
            });
        }
        
        if (seats.length >= capacity) break;
    }
    
    return seats;
}

// 辅助函数: 成绩加权排座
function arrangeByGradeSeats(students, capacity) {
    // 模拟的学生成绩 (实际应用中需要从数据库获取)
    const studentGrades = {};
    students.forEach(student => {
        // 生成随机成绩 (60-100)
        studentGrades[student.id] = Math.floor(Math.random() * 41) + 60;
    });
    
    // 按成绩排序学生 (成绩高的坐后排)
    const sortedStudents = [...students].sort((a, b) => {
        return studentGrades[b.id] - studentGrades[a.id];
    });
    
    // 分配座位 (成绩高的坐后排)
    return sortedStudents.slice(0, capacity).map((student, index) => ({
        seatId: index + 1,
        studentId: student.id,
        studentName: student.name,
        studentNumber: student.studentId,
        className: student.className,
        grade: studentGrades[student.id] // 添加成绩信息
    }));
}

// 辅助函数: 更新考场学生数据
function updateExamRoomStudents(examRoomId, seats) {
    if (!seats || !Array.isArray(seats)) {
        console.error(`更新考场学生数据失败: 座位数据无效 - 考场ID: ${examRoomId}`);
        return;
    }
    
    const students = seats.map(seat => ({
        id: seat.studentId,
        studentId: seat.studentNumber,
        name: seat.studentName,
        seatNumber: seat.seatId,
        attendanceStatus: '未到'
    }));
    
    studentsMap.set(parseInt(examRoomId), students);
    console.log(`更新考场ID=${examRoomId}的学生数据，共${students.length}名学生`);
}

// 教师请求照片API
app.post('/teacher/requestPhotos', authenticateToken, (req, res) => {
    const { classId } = req.body;
    const teacherId = req.user.id;
    
    console.log(`教师请求照片 - 教师ID: ${teacherId}, 班级ID: ${classId}`);
    
    // 验证参数
    if (!classId) {
        return res.json({ status: 400, message: '请提供班级ID' });
    }
    
    // 验证教师身份
    if (req.user.role !== 'teacher') {
        return res.json({ status: 403, message: '权限不足，只有教师可以请求学生照片' });
    }
    
    // 验证该班级是否存在
    if (!classes[classId]) {
        return res.json({ status: 404, message: '班级不存在' });
    }
    
    // 验证该教师是否有权限操作该班级
    const teacherClass = teacherClasses.find(tc => tc.teacherId === teacherId);
    if (!teacherClass || !teacherClass.classIds.includes(parseInt(classId))) {
        return res.json({ status: 403, message: '您没有权限操作该班级' });
    }
    
    // 获取班级学生
    const students = classStudents[classId] || [];
    if (students.length === 0) {
        return res.json({ status: 404, message: '该班级没有学生' });
    }
    
    console.log(`班级 ${classId} 有 ${students.length} 名学生`);
    
    // 更新所有学生的照片状态为"已请求"
    let updatedCount = 0;
    
    students.forEach(student => {
        if (student) {
            // 如果学生还没有上传照片，将状态设置为requested
            if (!student.photoUrl) {
                student.photoStatus = 'requested';
                updatedCount++;
                
                // 查找对应的学生用户账号
                const studentUser = users.find(u => 
                    (u.username === student.studentId) || 
                    (u.username === student.studentNumber) ||
                    (u.id === student.id)
                );
                
                // 为学生创建通知
                if (studentUser) {
                    console.log(`为学生创建照片请求通知: ${studentUser.username} (ID: ${studentUser.id})`);
                    
                    const teacherName = req.user.username || '教师';
                    const className = classes[classId]?.className || '班级';
                    
                    try {
                        const notification = createNotification(
                            studentUser.id,
                            'photo_request',
                            `${teacherName}请求您上传照片`,
                            {
                                teacherId: teacherId,
                                teacherName: teacherName,
                                classId: classId,
                                className: className
                            }
                        );
                        console.log(`成功创建通知: `, notification);
                    } catch (error) {
                        console.error(`创建通知失败: ${error.message}`);
                    }
                } else {
                    console.log(`未找到学生用户账号: ${student.studentId} 或 ${student.studentNumber} 或 ID ${student.id}`);
                }
            }
        }
    });
    
    // 保存数据
    saveData();
    
    return res.json({ 
        status: 200, 
        message: '照片请求已发送', 
        data: { totalStudents: students.length, updatedStudents: updatedCount } 
    });
});

// 教师查看学生照片API
app.get('/teacher/students/photos', authenticateToken, (req, res) => {
    const { classId } = req.query;
    const teacherId = req.user.id;
    
    // 验证参数
    if (!classId) {
        return res.json({ status: 400, message: '请提供班级ID' });
    }
    
    // 验证教师身份
    if (req.user.role !== 'teacher') {
        return res.json({ status: 403, message: '权限不足，只有教师可以查看学生照片' });
    }
    
    // 验证该班级是否存在
    if (!classes[classId]) {
        return res.json({ status: 404, message: '班级不存在' });
    }
    
    // 验证该教师是否有权限操作该班级
    const teacherClass = teacherClasses.find(tc => tc.teacherId === teacherId);
    if (!teacherClass || !teacherClass.classIds.includes(parseInt(classId))) {
        return res.json({ status: 403, message: '您没有权限查看该班级信息' });
    }
    
    // 获取班级学生
    const students = classStudents[classId] || [];
    if (students.length === 0) {
        return res.json({ status: 200, message: '该班级没有学生', data: [] });
    }
    
    // 收集学生照片信息
    const studentsWithPhotos = students.map(student => {
        if (!student) return null;
        
        // 如果没有照片状态，默认为none
        const photoStatus = student.photoStatus || 'none';
        
        // 处理照片URL，确保不返回null或undefined
        let photoUrl = '';
        if (student.photoUrl) {
            // 确保URL是完整的URL格式或以/开头
            photoUrl = student.photoUrl;
            // 如果没有照片但状态是requested，将photoUrl设置为空字符串
        } else if (photoStatus === 'requested') {
            photoUrl = '';
        }
        
        console.log(`获取学生照片信息 - 学生: ${student.name}, 状态: ${photoStatus}, URL: ${photoUrl}`);
        
        return {
            studentId: student.id,
            name: student.name,
            photoStatus: photoStatus,
            photoUrl: photoUrl
        };
    }).filter(student => student !== null);
    
    return res.json({ 
        status: 200, 
        message: '获取成功', 
        data: studentsWithPhotos 
    });
});

// 学生上传照片API
app.post('/student/uploadPhoto', authenticateToken, upload.single('photo'), (req, res) => {
    const studentId = req.user.id;
    
    // 验证学生身份
    if (req.user.role !== 'student') {
        return res.json({ status: 403, message: '权限不足，只有学生可以上传照片' });
    }
    
    // 检查是否有文件上传
    if (!req.file) {
        return res.json({ status: 400, message: '请上传照片文件' });
    }
    
    // 保存文件路径 - 使用完整URL路径
    const fileName = req.file.filename;
    const photoUrl = `/uploads/${fileName}`;
    
    console.log(`学生照片上传成功 - 学生ID: ${studentId}, 文件名: ${fileName}`);
    
    // 寻找学生在哪个班级
    let studentRecord = null;
    let studentClassId = null;
    
    // 遍历所有班级，查找学生记录
    Object.keys(classStudents).forEach(classId => {
        const students = classStudents[classId];
        const foundStudent = students.find(s => s.id === studentId);
        if (foundStudent) {
            studentRecord = foundStudent;
            studentClassId = classId;
        }
    });
    
    if (!studentRecord) {
        // 如果没找到学生记录，尝试用studentId查找
        Object.keys(classStudents).forEach(classId => {
            const students = classStudents[classId];
            const foundStudent = students.find(s => s.studentId === req.user.username);
            if (foundStudent) {
                studentRecord = foundStudent;
                studentClassId = classId;
            }
        });
        
        if (!studentRecord) {
            return res.json({ status: 404, message: '学生信息不存在' });
        }
    }
    
    // 更新学生照片信息
    studentRecord.photoUrl = photoUrl;
    studentRecord.photoStatus = 'uploaded';
    
    console.log(`已更新学生照片信息 - 学生ID: ${studentId}, 照片URL: ${photoUrl}`);
    
    // 保存数据
    saveData();
    
    return res.json({ 
        status: 200, 
        message: '照片上传成功', 
        data: { photoUrl } 
    });
});

// 学生获取座位信息的API
app.get('/student/seat', authenticateToken, (req, res) => {
  console.log(`获取学生座位信息请求 - 用户: ${req.user.id}`);
  
  // 验证是否为学生用户
  if (req.user.role !== 'student') {
    console.log(`座位信息请求被拒绝 - 非学生用户: ${req.user.username}, 角色: ${req.user.role}`);
    return res.status(403).json({
      status: 'error',
      message: '只有学生用户可以查看自己的座位信息'
    });
  }
  
  const studentId = req.user.id;
  const studentUsername = req.user.username;
  
  // 查找学生的座位信息
  let studentSeat = null;
  let examRoomInfo = null;
  
  // 遍历所有考场座位安排
  for (const examRoomId in examRoomSeats) {
    const examRoomData = examRoomSeats[examRoomId];
    if (!examRoomData || !examRoomData.seats || !Array.isArray(examRoomData.seats)) {
      console.log(`考场 ${examRoomId} 的座位数据无效`);
      continue;
    }
    
    // 在座位安排中查找该学生
    const seat = examRoomData.seats.find(s => 
      s.studentId == studentId || 
      (studentUsername && s.studentId == studentUsername) ||
      (s.studentNumber && s.studentNumber == studentUsername)
    );
    
    if (seat) {
      const examRoom = examRooms.find(e => e.id === parseInt(examRoomId));
      if (examRoom) {
        studentSeat = seat;
        examRoomInfo = examRoom;
        console.log(`找到学生座位 - 学生: ${req.user.username}, 考场: ${examRoom.name}, 座位: ${seat.seatId}`);
        break;
      }
    }
  }
  
  // 返回座位信息
  if (studentSeat && examRoomInfo) {
    return res.json({
      status: 'success',
      message: '获取座位信息成功',
      data: {
        examRoom: examRoomInfo,
        seat: studentSeat
      }
    });
  } else {
    console.log(`未找到学生座位信息 - 学生: ${req.user.username}`);
    return res.json({
      status: 'success',
      message: '未分配座位',
      data: null
    });
  }
});

// 学生获取考试信息的API
app.get('/student/exam-info', authenticateToken, (req, res) => {
    const userId = req.user.id;
    console.log(`获取学生考试信息请求 - 用户: ${userId}`);
    
    // 检查用户是学生
    if (req.user.role !== 'student') {
        return res.status(403).json({
            status: 'error',
            message: '只有学生可以访问此API'
        });
    }
    
    // 查找学生所在班级
    console.log(`查找学生(${userId})所在班级...`);
    const studentClassRecord = studentClasses.find(sc => sc.studentId === userId);
    
    if (!studentClassRecord || !studentClassRecord.classIds || studentClassRecord.classIds.length === 0) {
        console.log(`在studentClasses中未找到学生(${userId})的记录`);
        
        // 尝试使用学号查找
        const username = req.user.username;
        for (const classId in classStudents) {
            const students = classStudents[classId];
            const foundStudent = students.find(s => s.studentId === username);
            if (foundStudent) {
                console.log(`通过学号找到学生在班级 ${classId} 中`);
                // 检查该班级是否有关联的考场
                for (const [examRoomId, classes] of Object.entries(examRoomClasses)) {
                    console.log(`检查考场 ${examRoomId} 是否包含班级 ${classId}`);
                    if (classes && classes.includes(parseInt(classId))) {
                        const examRoom = examRooms.find(er => er.id === parseInt(examRoomId));
                        if (examRoom) {
                            console.log(`找到学生考试信息 - 学生: ${username}, 考场: ${examRoomId}`);
                            return res.json({
                                status: 'success',
                                message: '获取考试信息成功',
                                data: examRoom
                            });
                        }
                    } else {
                        console.log(`考场 ${examRoomId} 没有关联班级 ${classId}`);
                    }
                }
            }
        }
        
        console.log(`未找到学生考试信息 - 学生: ${req.user.username}`);
        return res.json({
            status: 'success',
            message: '未找到考试信息',
            data: null
        });
    }
    
    // 检查学生班级是否有关联的考场
    for (const classId of studentClassRecord.classIds) {
        for (const [examRoomId, classes] of Object.entries(examRoomClasses)) {
            if (classes && classes.includes(classId)) {
                const examRoom = examRooms.find(er => er.id === parseInt(examRoomId));
                if (examRoom) {
                    console.log(`找到学生考试信息 - 学生ID: ${userId}, 班级: ${classId}, 考场: ${examRoomId}`);
                    return res.json({
                        status: 'success',
                        message: '获取考试信息成功',
                        data: examRoom
                    });
                }
            }
        }
    }
    
    console.log(`未找到学生考试信息 - 用户ID: ${userId}`);
    return res.json({
        status: 'success',
        message: '未找到考试信息',
        data: null
    });
});

// 初始化默认数据
function initializeDefaultData() {
    console.log('初始化默认数据...');
    
    // 重新初始化用户数据
    users = [
        { id: 1, username: 'admin', password: 'admin123', role: 'admin' },
        { id: 2, username: 'teacher', password: 'teacher123', role: 'teacher' },
        { id: 3, username: 'operator', password: 'operator123', role: 'operator' },
        // 添加学生账号
        { id: 4, username: '2023100001', password: 'student123', role: 'student' },
        { id: 5, username: '2023100002', password: 'student123', role: 'student' },
        { id: 6, username: '2023200001', password: 'student123', role: 'student' },
        { id: 7, username: '2023200002', password: 'student123', role: 'student' },
        { id: 8, username: '2023300001', password: 'student123', role: 'student' },
        { id: 9, username: '2023300002', password: 'student123', role: 'student' }
    ];

    // 重新初始化班级数据
    classes = [
        { id: 1, className: '计算机科学与技术1班', teacher: '教师1', location: '教学楼A栋101' },
        { id: 2, className: '软件工程2班', teacher: '教师2', location: '教学楼B栋203' },
        { id: 3, className: '人工智能3班', teacher: '教师3', location: '教学楼C栋305' }
    ];

    // 重新初始化教师-班级关系
    teacherClasses = [
        { teacherId: 2, classIds: [1, 2] }
    ];

    // 重新初始化学生-班级关系
    studentClasses = [
        { studentId: 3, classIds: [1, 3] }, // operator用户(作为学生)属于班级1和3
        { studentId: 4, classIds: [1] },    // 学生1属于班级1
        { studentId: 5, classIds: [1] },    // 学生2属于班级1
        { studentId: 6, classIds: [2] },    // 学生3属于班级2
        { studentId: 7, classIds: [2] },    // 学生4属于班级2
        { studentId: 8, classIds: [3] },    // 学生5属于班级3
        { studentId: 9, classIds: [3] }     // 学生6属于班级3
    ];

    // 重新初始化班级学生名单
    classStudents = {
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

    // 重新初始化考场数据
    examRooms = [
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

    // 重新初始化关联关系和照片数据
    examRoomClasses = {};
    studentPhotos = {};
    examRoomSeats = {};
    attendanceRecords = [];

    // 重新生成学生数据
    studentsMap = new Map();
    examRooms.forEach(room => {
        studentsMap.set(room.id, generateStudents(room.id, room.capacity / 2));
    });
    
    console.log('初始化默认数据完成');
    saveData();
}

// 启动服务器
app.listen(PORT, () => {
    console.log(`服务器已启动，监听端口 ${PORT}`);
    console.log(`访问地址: http://localhost:${PORT}`);
    console.log('可用账号:');
    console.log('- 管理员: admin / admin123');
    console.log('- 教师: teacher / teacher123');
    console.log('- 操作员: operator / operator123');
}); 

// 创建通知函数
function createNotification(userId, type, message, data = {}) {
  if (!userId) {
    console.error("创建通知失败: 用户ID为空");
    return null;
  }
  
  console.log(`正在为用户 ${userId} 创建${type}类型通知`);
  
  const notification = {
    id: Date.now() + Math.floor(Math.random() * 1000), // 确保唯一性
    userId: userId,
    type: type,
    message: message,
    data: data,
    createdAt: new Date().toISOString(),
    read: false
  };
  
  // 确保notifications数组已初始化
  if (!Array.isArray(notifications)) {
    console.log("初始化notifications数组");
    notifications = [];
  }
  
  notifications.push(notification);
  
  console.log(`创建通知 - 用户ID: ${userId}, 类型: ${type}, 消息: ${message}`);
  console.log(`当前共有 ${notifications.length} 条通知`);
  
  // 保存到持久化存储
  saveData();
  
  return notification;
}

// 通知相关API
// 获取用户通知
app.get('/notifications', authenticateToken, (req, res) => {
  const userId = req.user.id;
  
  console.log(`获取通知 - 用户ID: ${userId}`);
  
  // 确保notifications数组已初始化
  if (!Array.isArray(notifications)) {
    notifications = [];
    console.log("初始化notifications数组");
  }
  
  const userNotifications = notifications.filter(n => n.userId == userId);
  console.log(`用户 ${userId} 有 ${userNotifications.length} 条通知`);
  
  // 按时间降序排序，最新的通知在前面
  userNotifications.sort((a, b) => {
    return new Date(b.createdAt) - new Date(a.createdAt);
  });
  
  return res.json({
    status: 200,
    message: '获取通知成功',
    data: userNotifications
  });
});

// 标记通知为已读
app.post('/notifications/read', authenticateToken, (req, res) => {
  const userId = req.user.id;
  const { notificationId } = req.body;
  
  console.log(`标记通知已读 - 用户ID: ${userId}, 通知ID: ${notificationId}`);
  
  if (!notificationId) {
    return res.json({
      status: 'error',
      message: '缺少通知ID'
    });
  }
  
  const notification = notifications.find(n => n.id == notificationId && n.userId == userId);
  
  if (notification) {
    notification.read = true;
    saveData();
    
    return res.json({
      status: 200,
      message: '通知已标记为已读'
    });
  }
  
  return res.json({
    status: 'error',
    message: '通知不存在'
  });
}); 

// 教师获取单个班级信息
app.get('/teacher/classes/:id', authenticateToken, (req, res) => {
    const classId = parseInt(req.params.id);
    const userId = req.user.id;
    
    // 验证用户角色为教师
    if (req.user.role !== 'teacher') {
        return res.status(403).json({ 
            status: 'error', 
            message: '只有教师可以访问班级信息' 
        });
    }
    
    // 验证教师是否管理该班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (!teacher || !teacher.classIds.includes(classId)) {
        return res.status(403).json({ 
            status: 'error', 
            message: '您没有权限查看该班级' 
        });
    }
    
    // 获取班级信息
    const classInfo = classes.find(c => c.id === classId);
    if (!classInfo) {
        return res.status(404).json({ 
            status: 'error', 
            message: '班级不存在' 
        });
    }
    
    res.json({
        status: 'success',
        message: '获取班级信息成功',
        data: classInfo
    });
});

// 教师修改单个班级信息
app.put('/teacher/classes/:id', authenticateToken, (req, res) => {
    const classId = parseInt(req.params.id);
    const userId = req.user.id;
    const { className, location } = req.body;
    
    // 验证用户角色为教师
    if (req.user.role !== 'teacher') {
        return res.status(403).json({ 
            status: 'error', 
            message: '只有教师可以修改班级信息' 
        });
    }
    
    // 验证教师是否管理该班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (!teacher || !teacher.classIds.includes(classId)) {
        return res.status(403).json({ 
            status: 'error', 
            message: '您没有权限修改该班级' 
        });
    }
    
    // 获取班级信息
    const classInfo = classes.find(c => c.id === classId);
    if (!classInfo) {
        return res.status(404).json({ 
            status: 'error', 
            message: '班级不存在' 
        });
    }
    
    // 更新班级信息
    if (className) classInfo.className = className;
    if (location) classInfo.location = location;
    
    // 保存数据
    saveData();
    
    res.json({
        status: 'success',
        message: '更新班级信息成功',
        data: classInfo
    });
});

// 教师删除班级
app.delete('/teacher/classes/:id', authenticateToken, (req, res) => {
    const classId = parseInt(req.params.id);
    const userId = req.user.id;
    
    // 验证用户角色为教师
    if (req.user.role !== 'teacher') {
        return res.status(403).json({ 
            status: 'error', 
            message: '只有教师可以删除班级' 
        });
    }
    
    // 验证教师是否管理该班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (!teacher || !teacher.classIds.includes(classId)) {
        return res.status(403).json({ 
            status: 'error', 
            message: '您没有权限删除该班级' 
        });
    }
    
    // 获取班级信息
    const classIndex = classes.findIndex(c => c.id === classId);
    if (classIndex === -1) {
        return res.status(404).json({ 
            status: 'error', 
            message: '班级不存在' 
        });
    }
    
    // 删除班级
    classes.splice(classIndex, 1);
    
    // 从教师管理的班级中移除
    const teacherIndex = teacherClasses.findIndex(tc => tc.teacherId === userId);
    if (teacherIndex !== -1) {
        const classIdIndex = teacherClasses[teacherIndex].classIds.indexOf(classId);
        if (classIdIndex !== -1) {
            teacherClasses[teacherIndex].classIds.splice(classIdIndex, 1);
        }
    }
    
    // 删除班级学生
    delete classStudents[classId];
    
    // 保存数据
    saveData();
    
    res.json({
        status: 'success',
        message: '删除班级成功'
    });
});

// 教师获取单个学生信息
app.get('/teacher/students/:id', authenticateToken, (req, res) => {
    const studentId = req.params.id;
    const userId = req.user.id;
    
    // 验证用户角色为教师
    if (req.user.role !== 'teacher') {
        return res.status(403).json({ 
            status: 'error', 
            message: '只有教师可以查看学生信息' 
        });
    }
    
    // 查找学生
    let foundStudent = null;
    let studentClassId = null;
    
    // 遍历班级学生列表
    for (const classId in classStudents) {
        const classIdNum = parseInt(classId);
        const students = classStudents[classId];
        
        const student = students.find(s => s.studentId === studentId || s.id === parseInt(studentId));
        if (student) {
            foundStudent = student;
            studentClassId = classIdNum;
            break;
        }
    }
    
    if (!foundStudent) {
        return res.status(404).json({ 
            status: 'error', 
            message: '学生不存在' 
        });
    }
    
    // 验证教师是否管理该学生所在班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (!teacher || !teacher.classIds.includes(studentClassId)) {
        return res.status(403).json({ 
            status: 'error', 
            message: '您没有权限查看该学生信息' 
        });
    }
    
    // 获取学生照片信息
    let photoStatus = 'none';
    let photoUrl = '';
    
    if (studentPhotos[studentId]) {
        photoStatus = studentPhotos[studentId].status || 'uploaded';
        photoUrl = studentPhotos[studentId].photoPath || '';
    }
    
    // 附加学生照片信息
    const studentInfo = {
        ...foundStudent,
        photoStatus,
        photoUrl,
        classId: studentClassId
    };
    
    res.json({
        status: 'success',
        message: '获取学生信息成功',
        data: studentInfo
    });
});

// 教师修改单个学生信息
app.put('/teacher/students/:id', authenticateToken, (req, res) => {
    const studentId = req.params.id;
    const userId = req.user.id;
    const { name, studentNumber } = req.body;
    
    // 验证用户角色为教师
    if (req.user.role !== 'teacher') {
        return res.status(403).json({ 
            status: 'error', 
            message: '只有教师可以修改学生信息' 
        });
    }
    
    // 查找学生
    let foundStudent = null;
    let studentClassId = null;
    
    // 遍历班级学生列表
    for (const classId in classStudents) {
        const classIdNum = parseInt(classId);
        const students = classStudents[classId];
        
        const studentIndex = students.findIndex(s => s.studentId === studentId || s.id === parseInt(studentId));
        if (studentIndex !== -1) {
            foundStudent = students[studentIndex];
            studentClassId = classIdNum;
            
            // 更新学生信息
            if (name) students[studentIndex].name = name;
            if (studentNumber) students[studentIndex].studentId = studentNumber;
            
            break;
        }
    }
    
    if (!foundStudent) {
        return res.status(404).json({ 
            status: 'error', 
            message: '学生不存在' 
        });
    }
    
    // 验证教师是否管理该学生所在班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (!teacher || !teacher.classIds.includes(studentClassId)) {
        return res.status(403).json({ 
            status: 'error', 
            message: '您没有权限修改该学生信息' 
        });
    }
    
    // 保存数据
    saveData();
    
    res.json({
        status: 'success',
        message: '更新学生信息成功',
        data: foundStudent
    });
});

// 教师删除单个学生
app.delete('/teacher/students/:id', authenticateToken, (req, res) => {
    const studentId = req.params.id;
    const userId = req.user.id;
    
    // 验证用户角色为教师
    if (req.user.role !== 'teacher') {
        return res.status(403).json({ 
            status: 'error', 
            message: '只有教师可以删除学生' 
        });
    }
    
    // 查找学生
    let foundClassId = null;
    let studentIndex = -1;
    
    // 遍历班级学生列表
    for (const classId in classStudents) {
        const classIdNum = parseInt(classId);
        const students = classStudents[classId];
        
        studentIndex = students.findIndex(s => s.studentId === studentId || s.id === parseInt(studentId));
        if (studentIndex !== -1) {
            foundClassId = classIdNum;
            break;
        }
    }
    
    if (foundClassId === null || studentIndex === -1) {
        return res.status(404).json({ 
            status: 'error', 
            message: '学生不存在' 
        });
    }
    
    // 验证教师是否管理该学生所在班级
    const teacher = teacherClasses.find(tc => tc.teacherId === userId);
    if (!teacher || !teacher.classIds.includes(foundClassId)) {
        return res.status(403).json({ 
            status: 'error', 
            message: '您没有权限删除该学生' 
        });
    }
    
    // 删除学生
    const removedStudent = classStudents[foundClassId].splice(studentIndex, 1)[0];
    
    // 保存数据
    saveData();
    
    res.json({
        status: 'success',
        message: '删除学生成功',
        data: removedStudent
    });
});