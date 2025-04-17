const fs = require('fs');
const path = require('path');

// 定义数据文件路径
const DATA_FILE = path.join(__dirname, '..', 'data.json');

// 初始化数据结构
let data = {
  users: [],
  classes: [],
  teacherClasses: [],
  studentClasses: [],
  classStudents: {},
  examRooms: [],
  examRoomClasses: {},
  studentPhotos: {},
  examRoomSeats: {},
  photoRequests: {}
};

// 尝试从文件读取现有数据
try {
  const fileData = fs.readFileSync(DATA_FILE, 'utf8');
  data = JSON.parse(fileData);
  console.log('成功读取现有数据文件');
} catch (error) {
  console.log(`读取数据文件失败，创建新数据: ${error.message}`);
}

console.log('开始生成测试数据...');

// 生成管理员账号
function generateAdmin() {
  // 检查管理员是否已存在
  const adminExists = data.users.some(user => user.username === 'admin');
  if (!adminExists) {
    data.users.push({
      id: 1,
      username: 'admin',
      password: 'admin23',
      role: 'admin'
    });
    console.log('创建了管理员账号: admin/admin23');
  }
}

// 生成教师账号
function generateTeacher() {
  // 检查教师账号是否已存在
  const teacherExists = data.users.some(user => user.username === 'teacher');
  if (!teacherExists) {
    data.users.push({
      id: 2,
      username: 'teacher',
      password: 'teacher123',
      role: 'teacher',
      name: '测试教师'
    });
    console.log('创建了教师账号: teacher/teacher123');
  }
  return data.users.find(user => user.role === 'teacher');
}

// 生成学生账号
function generateStudents(count) {
  const startId = data.users.length > 0 ? Math.max(...data.users.map(u => u.id)) + 1 : 3;
  const students = [];
  
  for (let i = 0; i < count; i++) {
    const studentId = `2023${String(i + 1).padStart(3, '0')}`;
    
    // 检查学生是否已存在
    if (!data.users.some(user => user.username === studentId)) {
      const student = {
        id: startId + i,
        username: studentId,
        password: 'student123',
        role: 'student',
        name: `学生${i + 1}`
      };
      
      data.users.push(student);
      students.push(student);
      console.log(`创建了学生账号: ${studentId}/student123`);
    } else {
      students.push(data.users.find(user => user.username === studentId));
    }
  }
  
  return students;
}

// 生成班级
function generateClasses() {
  if (data.classes.length === 0) {
    data.classes = [
      { 
        id: 1, 
        className: '计算机科学2023级1班', 
        location: '教学楼A101' 
      },
      { 
        id: 2, 
        className: '计算机科学2023级2班', 
        location: '教学楼B102' 
      }
    ];
    console.log('创建了测试班级');
  }
  return data.classes;
}

// 生成考场
function generateExamRooms() {
  if (data.examRooms.length === 0) {
    data.examRooms = [
      {
        id: 1,
        name: '2023年秋季学期期末考试 - 教学楼A201',
        location: '教学楼A201',
        capacity: 60,
        examDate: '2023-12-25',
        examTime: '09:00-11:00'
      },
      {
        id: 2,
        name: '2023年秋季学期期末考试 - 教学楼B202',
        location: '教学楼B202',
        capacity: 40,
        examDate: '2023-12-26',
        examTime: '14:00-16:00'
      }
    ];
    console.log('创建了测试考场');
  }
  return data.examRooms;
}

// 关联教师到班级
function linkTeacherToClasses(teacher, classes) {
  const teacherId = teacher.id;
  
  // 检查教师是否已关联班级
  const existingLink = data.teacherClasses.find(tc => tc.teacherId === teacherId);
  
  if (existingLink) {
    // 更新关联
    existingLink.classIds = [...new Set([...existingLink.classIds, ...classes.map(c => c.id)])];
  } else {
    // 创建新关联
    data.teacherClasses.push({
      teacherId,
      classIds: classes.map(c => c.id)
    });
  }
  
  console.log(`关联教师 ${teacher.name} 到班级: ${classes.map(c => c.id).join(', ')}`);
}

// 分配学生到班级
function assignStudentsToClasses(students, classes) {
  // 平均分配学生到班级
  const studentsPerClass = Math.ceil(students.length / classes.length);
  
  classes.forEach((classObj, index) => {
    const start = index * studentsPerClass;
    const end = Math.min(start + studentsPerClass, students.length);
    const classStudents = students.slice(start, end);
    
    // 为学生创建班级关联
    classStudents.forEach(student => {
      const existingLink = data.studentClasses.find(sc => sc.studentId === student.id);
      
      if (existingLink) {
        if (!existingLink.classIds.includes(classObj.id)) {
          existingLink.classIds.push(classObj.id);
        }
      } else {
        data.studentClasses.push({
          studentId: student.id,
          classIds: [classObj.id]
        });
      }
    });
    
    // 为班级添加学生名单
    if (!data.classStudents[classObj.id]) {
      data.classStudents[classObj.id] = [];
    }
    
    // 添加学生到班级名单
    classStudents.forEach(student => {
      // 检查学生是否已经在名单中
      if (!data.classStudents[classObj.id].some(s => s.studentId === student.username)) {
        data.classStudents[classObj.id].push({
          id: student.id,
          name: student.name,
          studentId: student.username,
          className: classObj.className
        });
      }
    });
    
    console.log(`分配 ${classStudents.length} 名学生到班级 ${classObj.className}`);
  });
}

// 关联班级到考场
function linkClassesToExamRooms(classes, examRooms) {
  // 简单关联：第一个考场关联第一个班级，第二个考场关联第二个班级，以此类推
  const minLength = Math.min(classes.length, examRooms.length);
  
  for (let i = 0; i < minLength; i++) {
    const examRoomId = examRooms[i].id;
    const classId = classes[i].id;
    
    // 确保考场与班级关联结构存在
    if (!data.examRoomClasses[examRoomId]) {
      data.examRoomClasses[examRoomId] = [];
    }
    
    // 检查是否已存在关联
    if (!data.examRoomClasses[examRoomId].includes(classId)) {
      data.examRoomClasses[examRoomId].push(classId);
      console.log(`将班级 ${classes[i].className}(ID: ${classId}) 关联到考场 ${examRooms[i].name}(ID: ${examRoomId})`);
    }
  }
  
  // 添加额外关联：让第一个考场关联多个班级
  if (examRooms.length > 0 && classes.length > 1) {
    const examRoomId = examRooms[0].id;
    // 关联第二个班级到第一个考场
    if (classes.length > 1) {
      const classId = classes[1].id;
      if (!data.examRoomClasses[examRoomId].includes(classId)) {
        data.examRoomClasses[examRoomId].push(classId);
        console.log(`将额外班级 ${classes[1].className}(ID: ${classId}) 关联到考场 ${examRooms[0].name}(ID: ${examRoomId})`);
      }
    }
  }
}

// 生成座位安排
function generateSeats(examRooms) {
  examRooms.forEach(room => {
    // 检查考场是否已有座位安排
    if (!data.examRoomSeats[room.id]) {
      // 获取关联到此考场的班级
      const classIds = data.examRoomClasses[room.id] || [];
      
      if (classIds.length > 0) {
        // 获取这些班级的所有学生
        const students = [];
        classIds.forEach(classId => {
          const classStudents = data.classStudents[classId] || [];
          students.push(...classStudents.map(student => ({
            ...student,
            classId: classId // 添加班级ID便于后续处理
          })));
        });
        
        // 生成随机座位安排
        const seats = students.map((student, index) => {
          return {
            seatNumber: index + 1,
            studentId: student.studentId,
            studentName: student.name,
            classId: student.classId
          };
        });
        
        // 随机打乱座位顺序
        for (let i = seats.length - 1; i > 0; i--) {
          const j = Math.floor(Math.random() * (i + 1));
          [seats[i], seats[j]] = [seats[j], seats[i]];
          // 更新座位号
          seats[i].seatNumber = i + 1;
          seats[j].seatNumber = j + 1;
        }
        
        // 保存座位安排
        data.examRoomSeats[room.id] = {
          arrangementType: 'random',
          seats: seats,
          createdAt: new Date().toISOString()
        };
        
        console.log(`为考场 ${room.name}(ID: ${room.id}) 生成了${seats.length}个座位安排`);
      }
    }
  });
}

// 创建照片请求
function createPhotoRequests(teachers, classes) {
  // 为每个教师处理
  teachers.forEach(teacher => {
    // 获取教师关联的班级
    const teacherClassLink = data.teacherClasses.find(tc => tc.teacherId === teacher.id);
    
    if (teacherClassLink) {
      const teacherClassIds = teacherClassLink.classIds;
      
      // 为每个班级创建照片请求
      teacherClassIds.forEach(classId => {
        const classObj = classes.find(c => c.id === classId);
        
        if (classObj) {
          // 创建照片请求记录
          if (!data.photoRequests[classId]) {
            data.photoRequests[classId] = {
              requestedBy: teacher.id,
              requestedAt: new Date().toISOString(),
              status: 'requested'
            };
            
            console.log(`教师 ${teacher.name} 为班级 ${classObj.className} 创建了照片请求`);
          }
        }
      });
    }
  });
}

// 生成学生照片上传状态
function generateStudentPhotoStatus(students) {
  // 初始化照片状态数据结构
  if (!data.studentPhotos) {
    data.studentPhotos = {};
  }
  
  // 随机为30%的学生上传照片
  students.forEach(student => {
    if (Math.random() < 0.3) {  // 30%的概率上传照片
      data.studentPhotos[student.studentId] = {
        status: 'uploaded',
        uploadedAt: new Date().toISOString(),
        photoPath: `uploads/photos/${student.studentId}.jpg`  // 虚拟路径
      };
      console.log(`学生 ${student.name}(ID: ${student.studentId}) 已上传照片`);
    }
  });
}

// 运行测试数据生成
generateAdmin();
const teachers = [generateTeacher(), generateTeacher()];
const students = generateStudents(20);  // 增加到20名学生
const classes = generateClasses();
const examRooms = generateExamRooms();

// 关联教师到班级
teachers.forEach((teacher, index) => {
  // 确保有足够的班级可分配
  if (index < classes.length) {
    linkTeacherToClasses(teacher, [classes[index]]);
    console.log(`教师 ${teacher.name}(ID: ${teacher.id}) 关联到班级 ${classes[index].className}`);
  }
});

// 确保第一个教师关联到多个班级
if (teachers.length > 0 && classes.length > 1) {
  const existingLink = data.teacherClasses.find(tc => tc.teacherId === teachers[0].id);
  if (existingLink && !existingLink.classIds.includes(classes[1].id)) {
    existingLink.classIds.push(classes[1].id);
    console.log(`教师 ${teachers[0].name}(ID: ${teachers[0].id}) 额外关联到班级 ${classes[1].className}`);
  }
}

assignStudentsToClasses(students, classes);
linkClassesToExamRooms(classes, examRooms);
generateSeats(examRooms);
createPhotoRequests(teachers, classes);
generateStudentPhotoStatus(students);

// 为第一个考场创建一个考勤记录
if (examRooms.length > 0) {
  const examRoomId = examRooms[0].id;
  const seats = data.examRoomSeats[examRoomId]?.seats || [];
  
  if (seats.length > 0) {
    if (!data.attendance) {
      data.attendance = {};
    }
    
    const attendanceId = Date.now().toString();
    data.attendance[attendanceId] = {
      examRoomId: examRoomId,
      date: new Date().toISOString(),
      records: seats.map(seat => {
        // 随机80%的学生出勤，20%缺勤
        const isPresent = Math.random() < 0.8;
        return {
          studentId: seat.studentId,
          studentName: seat.studentName,
          seatNumber: seat.seatNumber,
          status: isPresent ? 'present' : 'absent',
          recordedAt: new Date().toISOString()
        };
      })
    };
    
    console.log(`为考场 ${examRooms[0].name}(ID: ${examRoomId}) 创建了一条出勤记录`);
  }
}

// 保存数据到文件
fs.writeFileSync(DATA_FILE, JSON.stringify(data, null, 2));
console.log(`测试数据生成完成，已保存到 ${DATA_FILE}`); 