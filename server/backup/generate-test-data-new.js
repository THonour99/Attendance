/**
 * 测试数据生成脚本 - 生成教师和学生账号
 * 运行方法: node generate-test-data-new.js
 */

const fs = require('fs');
const path = require('path');

// 数据文件路径
const DATA_FILE_PATH = './data.json';

// 初始化基本数据结构
const initialData = {
  users: [],
  classes: [],
  examRooms: [],
  teacherClasses: [],
  studentClasses: [],
  classStudents: {}, 
  examRoomClasses: {},
  examRoomSeats: {},
  studentPhotos: {},
  attendanceRecords: []
};

// 读取现有数据，如果存在的话
let data = initialData;
try {
  if (fs.existsSync(DATA_FILE_PATH)) {
    data = JSON.parse(fs.readFileSync(DATA_FILE_PATH, 'utf8'));
    console.log('已读取现有数据文件');
  }
} catch (error) {
  console.error('读取数据文件出错，将创建新数据:', error.message);
}

// 确保数据结构完整
data.users = data.users || [];
data.classes = data.classes || [];
data.examRooms = data.examRooms || [];
data.teacherClasses = data.teacherClasses || [];
data.studentClasses = data.studentClasses || [];
data.classStudents = data.classStudents || {};
data.examRoomClasses = data.examRoomClasses || {};
data.examRoomSeats = data.examRoomSeats || {};
data.studentPhotos = data.studentPhotos || {};
data.attendanceRecords = data.attendanceRecords || [];

// 生成教师账号
function generateTeachers() {
  const teacherCount = 5; // 生成5个教师账号
  // 修复：当users为空数组时，Math.max可能返回-Infinity
  const maxId = data.users.length > 0 ? Math.max(...data.users.map(u => u.id)) : 0;
  const startId = maxId + 1;

  console.log(`生成${teacherCount}个教师账号，起始ID: ${startId}`);
  
  for (let i = 0; i < teacherCount; i++) {
    const id = startId + i;
    const teacherNumber = i + 1;
    
    data.users.push({
      id,
      username: `teacher${teacherNumber}`,
      password: `teacher123`,
      role: 'teacher',
      name: `教师${teacherNumber}`
    });
    
    console.log(`已创建教师账号: teacher${teacherNumber} (ID: ${id})`);
  }
}

// 生成学生账号
function generateStudents() {
  const studentsPerClass = 20; // 每个班级20个学生
  const classCount = data.classes.length;
  
  if (classCount === 0) {
    console.warn('没有班级数据，无法生成学生');
    return;
  }
  
  // 修复：当users为空数组时，Math.max可能返回-Infinity
  const maxId = data.users.length > 0 ? Math.max(...data.users.map(u => u.id)) : 0;
  const startId = maxId + 1;
  console.log(`生成${classCount}个班级，每班${studentsPerClass}个学生，起始ID: ${startId}`);
  
  let studentId = startId;
  
  for (let classIdx = 0; classIdx < classCount; classIdx++) {
    const classId = data.classes[classIdx].id;
    const classIdStr = classId.toString();
    const classPrefix = `2023${classId.toString().padStart(2, '0')}`;
    
    // 初始化班级学生数组
    if (!data.classStudents[classIdStr]) {
      data.classStudents[classIdStr] = [];
    }
    
    for (let i = 0; i < studentsPerClass; i++) {
      const studentNumber = (i + 1).toString().padStart(2, '0');
      const studentCode = `${classPrefix}${studentNumber}`;
      
      // 创建学生用户
      data.users.push({
        id: studentId,
        username: studentCode,
        password: 'student123',
        role: 'student',
        name: `学生${classPrefix}-${studentNumber}`
      });
      
      // 添加学生到班级
      data.classStudents[classIdStr].push({
        id: studentId,
        studentId: studentCode,
        name: `学生${classPrefix}-${studentNumber}`,
        gender: i % 2 === 0 ? '男' : '女',
        photoStatus: 'none'
      });
      
      // 添加学生班级关联
      data.studentClasses.push({
        studentId: studentId,
        classIds: [classId]
      });
      
      console.log(`已创建学生账号: ${studentCode} (ID: ${studentId})`);
      studentId++;
    }
  }
}

// 生成班级数据
function generateClasses() {
  const classCount = 4; // 生成4个班级
  
  // 找到教师ID
  const teacherIds = data.users
    .filter(user => user.role === 'teacher')
    .map(teacher => teacher.id);
  
  if (teacherIds.length === 0) {
    console.warn('没有教师账号，无法分配班级');
    return;
  }
  
  console.log(`生成${classCount}个班级，可用教师: ${teacherIds.length}名`);
  
  // 清空现有班级
  data.classes = [];
  
  for (let i = 0; i < classCount; i++) {
    const classId = i + 1;
    const teacherIndex = i % teacherIds.length; // 循环分配教师
    const teacherId = teacherIds[teacherIndex];
    
    // 创建班级
    data.classes.push({
      id: classId,
      className: `计算机科学2023级${classId}班`,
      location: `教学楼${String.fromCharCode(65 + i)}${(101 + i).toString()}`,
      teacher: `教师${teacherIndex + 1}`
    });
    
    // 分配教师到班级
    const teacherClass = data.teacherClasses.find(tc => tc.teacherId === teacherId);
    if (teacherClass) {
      if (!teacherClass.classIds.includes(classId)) {
        teacherClass.classIds.push(classId);
      }
    } else {
      data.teacherClasses.push({
        teacherId,
        classIds: [classId]
      });
    }
    
    console.log(`已创建班级: 计算机科学2023级${classId}班，分配教师ID: ${teacherId}`);
  }
}

// 生成考场数据
function generateExamRooms() {
  const examRoomCount = 3; // 生成3个考场
  
  // 找到所有教师
  const teacherIds = data.users
    .filter(user => user.role === 'teacher')
    .map(teacher => teacher.id);
  
  if (teacherIds.length === 0) {
    console.warn('没有教师账号，将使用固定ID');
  }
  
  console.log(`生成${examRoomCount}个考场`);
  
  // 清空现有考场
  data.examRooms = [];
  
  for (let i = 0; i < examRoomCount; i++) {
    const examRoomId = i + 1;
    const teacherId = teacherIds.length > 0 ? teacherIds[i % teacherIds.length] : 2; // 默认使用ID 2
    
    data.examRooms.push({
      id: examRoomId,
      name: `${['数据结构', '计算机原理', 'C++程序设计'][i % 3]}期末考试`,
      location: `教学楼${String.fromCharCode(65 + i)}${(201 + i).toString()}`,
      capacity: 40 + i * 10,
      rows: 5 + Math.floor(i / 2),
      columns: 8 + i,
      examDate: `2023-12-${25 + i}`,
      examTime: `${9 + i * 2}:00-${11 + i * 2}:00`,
      teacherId
    });
    
    console.log(`已创建考场: ${data.examRooms[i].name}`);
    
    // 关联考场和班级
    const classIds = [];
    const classCount = Math.min(data.classes.length, 2);
    for (let j = 0; j < classCount; j++) {
      classIds.push(1 + (i + j) % data.classes.length);
    }
    
    data.examRoomClasses[examRoomId] = classIds;
    console.log(`已关联考场${examRoomId}到班级: ${classIds.join(', ')}`);
  }
}

// 确保管理员账号存在
function ensureAdminExists() {
  const adminExists = data.users.some(user => user.role === 'admin');
  
  if (!adminExists) {
    console.log('添加管理员账号...');
    data.users.push({
      id: 1,
      username: 'admin',
      password: 'admin23',
      role: 'admin',
      name: '系统管理员'
    });
  } else {
    console.log('管理员账号已存在');
  }
}

// 生成所有测试数据
function generateAllData() {
  console.log('====== 开始生成测试数据 ======');
  
  ensureAdminExists();
  generateTeachers();
  generateClasses();
  generateExamRooms();
  generateStudents();
  
  // 保存数据到文件
  fs.writeFileSync(DATA_FILE_PATH, JSON.stringify(data, null, 2));
  console.log(`数据已保存到: ${DATA_FILE_PATH}`);
  
  console.log('\n====== 测试数据生成完成 ======');
  console.log(`用户总数: ${data.users.length}`);
  console.log(`教师数: ${data.users.filter(u => u.role === 'teacher').length}`);
  console.log(`学生数: ${data.users.filter(u => u.role === 'student').length}`);
  console.log(`班级数: ${data.classes.length}`);
  console.log(`考场数: ${data.examRooms.length}`);
}

// 执行数据生成
generateAllData(); 