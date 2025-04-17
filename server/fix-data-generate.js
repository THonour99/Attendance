/**
 * 修复版测试数据生成脚本 - 确保与服务器使用相同的数据文件路径
 * 同时处理班级分配，将学生和教师关联到班级
 */

const fs = require('fs');
const path = require('path');

// 使用与服务器相同的数据文件路径
const DATA_FILE_PATH = path.join(__dirname, 'data', 'database.json');

console.log(`将生成数据保存到: ${DATA_FILE_PATH}`);

// 确保data目录存在
const dataDir = path.join(__dirname, 'data');
if (!fs.existsSync(dataDir)) {
  fs.mkdirSync(dataDir, { recursive: true });
  console.log(`创建目录: ${dataDir}`);
}

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
let adminUsers = [];
try {
  if (fs.existsSync(DATA_FILE_PATH)) {
    const existingData = JSON.parse(fs.readFileSync(DATA_FILE_PATH, 'utf8'));
    console.log('已读取现有数据文件');
    
    // 仅保留管理员账号
    if (existingData.users && Array.isArray(existingData.users)) {
      adminUsers = existingData.users.filter(user => user.role === 'admin');
      if (adminUsers.length > 0) {
        console.log(`保留${adminUsers.length}个管理员账号`);
      }
    }
  }
} catch (error) {
  console.error('读取数据文件出错，将创建新数据:', error.message);
}

// 重置数据，但保留管理员账号
data = initialData;
data.users = adminUsers;

console.log('已清除旧数据（保留管理员账号）');

// 确保数据结构完整
data.classes = [];
data.examRooms = [];
data.teacherClasses = [];
data.studentClasses = [];
data.classStudents = {};
data.examRoomClasses = {};
data.examRoomSeats = {};
data.studentPhotos = {};
data.attendanceRecords = [];

// 序列化studentsMap
data.studentsMap = {};

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

// 生成教师账号
function generateTeachers() {
  const teacherCount = 5; // 生成5个教师账号
  const maxId = data.users.length > 0 ? Math.max(...data.users.map(u => u.id)) : 0;
  const startId = maxId + 1;

  console.log(`\n===== 生成教师账号 =====`);
  console.log(`将创建${teacherCount}个教师账号，起始ID: ${startId}`);
  
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
  
  return data.users.filter(user => user.role === 'teacher').map(teacher => teacher.id);
}

// 生成班级数据并分配教师
function generateClasses(teacherIds) {
  const classCount = 4; // 生成4个班级
  
  if (teacherIds.length === 0) {
    console.warn('没有教师账号，无法分配班级');
    return [];
  }
  
  console.log(`\n===== 生成班级并分配教师 =====`);
  console.log(`将创建${classCount}个班级，分配给${teacherIds.length}名教师`);
  
  // 记录教师分配的班级数量，用于平均分配
  const teacherClassCount = new Map();
  teacherIds.forEach(id => teacherClassCount.set(id, 0));
  
  const classIds = [];
  
  for (let i = 0; i < classCount; i++) {
    const classId = i + 1;
    classIds.push(classId);
    
    // 找出分配班级最少的教师
    let minClassCount = Infinity;
    let selectedTeacherId = teacherIds[0];
    
    for (const teacherId of teacherIds) {
      const count = teacherClassCount.get(teacherId);
      if (count < minClassCount) {
        minClassCount = count;
        selectedTeacherId = teacherId;
      }
    }
    
    // 更新教师班级计数
    teacherClassCount.set(selectedTeacherId, minClassCount + 1);
    
    // 创建班级
    const className = `计算机科学2023级${classId}班`;
    data.classes.push({
      id: classId,
      className: className,
      location: `教学楼${String.fromCharCode(65 + i)}${(101 + i).toString()}`,
      teacher: `教师${teacherIds.indexOf(selectedTeacherId) + 1}`
    });
    
    // 分配教师到班级
    const teacherClass = data.teacherClasses.find(tc => tc.teacherId === selectedTeacherId);
    if (teacherClass) {
      if (!teacherClass.classIds.includes(classId)) {
        teacherClass.classIds.push(classId);
      }
    } else {
      data.teacherClasses.push({
        teacherId: selectedTeacherId,
        classIds: [classId]
      });
    }
    
    // 初始化班级学生数组
    data.classStudents[classId] = [];
    
    console.log(`已创建班级: ${className}，分配给教师ID: ${selectedTeacherId} (教师${teacherIds.indexOf(selectedTeacherId) + 1})`);
  }
  
  // 打印教师-班级分配结果
  console.log("\n教师-班级分配情况:");
  data.teacherClasses.forEach(tc => {
    const teacherIndex = teacherIds.indexOf(tc.teacherId) + 1;
    const classNames = tc.classIds.map(classId => {
      const classObj = data.classes.find(c => c.id === classId);
      return classObj ? classObj.className : `未知班级(ID: ${classId})`;
    });
    console.log(`教师${teacherIndex} (ID: ${tc.teacherId}) => ${classNames.join(', ')}`);
  });
  
  return classIds;
}

// 生成考场数据
function generateExamRooms(teacherIds, classIds) {
  const examRoomCount = 3; // 生成3个考场
  
  if (teacherIds.length === 0) {
    console.warn('没有教师账号，将使用固定ID');
  }
  
  if (classIds.length === 0) {
    console.warn('没有班级数据，考场将没有关联班级');
  }
  
  console.log(`\n===== 生成考场并关联班级 =====`);
  console.log(`将创建${examRoomCount}个考场`);
  
  for (let i = 0; i < examRoomCount; i++) {
    const examRoomId = i + 1;
    const teacherId = teacherIds.length > 0 ? teacherIds[i % teacherIds.length] : 2; // 默认使用ID 2
    
    const examName = `${['数据结构', '计算机原理', 'C++程序设计'][i % 3]}期末考试`;
    data.examRooms.push({
      id: examRoomId,
      name: examName,
      location: `教学楼${String.fromCharCode(65 + i)}${(201 + i).toString()}`,
      capacity: 40 + i * 10,
      rows: 5 + Math.floor(i / 2),
      columns: 8 + i,
      examDate: `2023-12-${25 + i}`,
      examTime: `${9 + i * 2}:00-${11 + i * 2}:00`,
      teacherId
    });
    
    console.log(`已创建考场: ${examName}`);
    
    // 关联考场和班级，尽量平均分配
    const classIdsToAssign = [];
    const classesPerExamRoom = Math.max(1, Math.floor(classIds.length / examRoomCount));
    
    for (let j = 0; j < classesPerExamRoom; j++) {
      const classIndex = (i * classesPerExamRoom + j) % classIds.length;
      classIdsToAssign.push(classIds[classIndex]);
    }
    
    data.examRoomClasses[examRoomId] = classIdsToAssign;
    
    const classNames = classIdsToAssign.map(cid => {
      const classObj = data.classes.find(c => c.id === cid);
      return classObj ? classObj.className : `未知班级(ID: ${cid})`;
    });
    
    console.log(`已关联考场 ${examName} 到以下班级: ${classNames.join(', ')}`);
  }
  
  // 打印考场-班级关联情况
  console.log("\n考场-班级关联情况:");
  Object.entries(data.examRoomClasses).forEach(([examRoomId, classIds]) => {
    const examRoom = data.examRooms.find(er => er.id === parseInt(examRoomId));
    if (!examRoom) return;
    
    const classNames = classIds.map(classId => {
      const classObj = data.classes.find(c => c.id === classId);
      return classObj ? classObj.className : `未知班级(ID: ${classId})`;
    });
    
    console.log(`考场: ${examRoom.name} => ${classNames.join(', ')}`);
  });
}

// 生成学生账号并分配到班级
function generateStudents(classIds) {
  const studentsPerClass = 20; // 每个班级20个学生
  const classCount = classIds.length;
  
  if (classCount === 0) {
    console.warn('没有班级数据，无法生成学生');
    return;
  }
  
  const maxId = data.users.length > 0 ? Math.max(...data.users.map(u => u.id)) : 0;
  const startId = maxId + 1;
  
  console.log(`\n===== 生成学生并分配班级 =====`);
  console.log(`将为${classCount}个班级各创建${studentsPerClass}个学生，起始ID: ${startId}`);
  
  let studentId = startId;
  let totalStudents = 0;
  
  for (let i = 0; i < classCount; i++) {
    const classId = classIds[i];
    const classObj = data.classes.find(c => c.id === classId);
    if (!classObj) {
      console.warn(`班级ID ${classId} 不存在，跳过此班级的学生生成`);
      continue;
    }
    
    const className = classObj.className;
    const classIdStr = classId.toString();
    const classPrefix = `2023${classId.toString().padStart(2, '0')}`;
    
    console.log(`\n为班级 "${className}" 创建${studentsPerClass}个学生:`);
    
    // 初始化班级学生数组
    if (!data.classStudents[classIdStr]) {
      data.classStudents[classIdStr] = [];
    }
    
    for (let j = 0; j < studentsPerClass; j++) {
      const studentNumber = (j + 1).toString().padStart(2, '0');
      const studentCode = `${classPrefix}${studentNumber}`;
      const studentName = `学生${studentNumber}`;
      const fullStudentName = `${className}-${studentName}`;
      
      // 创建学生用户
      data.users.push({
        id: studentId,
        username: studentCode,
        password: 'student123',
        role: 'student',
        name: fullStudentName
      });
      
      // 添加学生到班级
      data.classStudents[classIdStr].push({
        id: studentId,
        studentId: studentCode,
        name: fullStudentName,
        gender: j % 2 === 0 ? '男' : '女',
        photoStatus: 'none'
      });
      
      // 添加学生班级关联
      data.studentClasses.push({
        studentId: studentId,
        classIds: [classId]
      });
      
      // 为考场分配学生
      // 查找包含此班级的考场
      for (const [examRoomId, erClassIds] of Object.entries(data.examRoomClasses)) {
        if (erClassIds.includes(classId)) {
          // 初始化考场学生数组
          if (!data.studentsMap[examRoomId]) {
            data.studentsMap[examRoomId] = [];
          }
          
          // 添加学生到考场
          data.studentsMap[examRoomId].push({
            id: studentId,
            studentId: studentCode,
            name: fullStudentName,
            seatNumber: data.studentsMap[examRoomId].length + 1, // 座位号从1开始递增
            attendanceStatus: '未到'
          });
        }
      }
      
      if ((j + 1) % 5 === 0 || j === studentsPerClass - 1) {
        console.log(`  已创建学生: ${studentCode} 到 ${studentCode - (j % 5)} (共${(j % 5) + 1}个)`);
      }
      
      studentId++;
      totalStudents++;
    }
  }
  
  console.log(`\n总共创建了 ${totalStudents} 个学生账号并分配到 ${classCount} 个班级`);
  
  // 打印班级-学生分配情况
  console.log("\n班级-学生分配情况统计:");
  Object.entries(data.classStudents).forEach(([classId, students]) => {
    const classObj = data.classes.find(c => c.id === parseInt(classId));
    if (!classObj) return;
    
    console.log(`班级: ${classObj.className} => ${students.length}名学生`);
  });
  
  // 打印考场-学生分配情况
  console.log("\n考场-学生分配情况统计:");
  Object.entries(data.studentsMap).forEach(([examRoomId, students]) => {
    const examRoom = data.examRooms.find(er => er.id === parseInt(examRoomId));
    if (!examRoom) return;
    
    console.log(`考场: ${examRoom.name} => ${students.length}名学生`);
  });
}

// 生成所有测试数据
function generateAllData() {
  console.log('====== 开始生成测试数据 ======');
  
  ensureAdminExists();
  const teacherIds = generateTeachers();
  const classIds = generateClasses(teacherIds);
  generateExamRooms(teacherIds, classIds);
  generateStudents(classIds);
  
  // 添加时间戳
  data.lastUpdate = new Date().toISOString();
  
  // 保存数据到文件
  fs.writeFileSync(DATA_FILE_PATH, JSON.stringify(data, null, 2));
  console.log(`\n数据已保存到: ${DATA_FILE_PATH}`);
  
  console.log('\n====== 测试数据生成完成 ======');
  console.log(`用户总数: ${data.users.length}`);
  console.log(`教师数: ${data.users.filter(u => u.role === 'teacher').length}`);
  console.log(`学生数: ${data.users.filter(u => u.role === 'student').length}`);
  console.log(`班级数: ${data.classes.length}`);
  console.log(`考场数: ${data.examRooms.length}`);
  
  console.log(`\n测试账号信息:`);
  console.log(`管理员: admin / admin23`);
  console.log(`教师: teacher1 / teacher123`);
  console.log(`学生: 20230101 / student123`);
}

// 执行数据生成
generateAllData(); 