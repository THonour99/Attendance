/**
 * 测试数据生成脚本 - 人脸识别排座系统
 * 此脚本用于生成测试数据，建立教师端和学生端之间的联系
 */

const fs = require('fs');
const path = require('path');
const bcrypt = require('bcrypt');

// 配置
const dataFolder = './data';
const usersFile = path.join(dataFolder, 'users.json');
const classesFile = path.join(dataFolder, 'classes.json');
const examRoomsFile = path.join(dataFolder, 'examRooms.json');
const studentsFile = path.join(dataFolder, 'students.json');
const attendanceFile = path.join(dataFolder, 'attendance.json');
const classStudentsFile = path.join(dataFolder, 'classStudents.json');
const examRoomClassesFile = path.join(dataFolder, 'examRoomClasses.json');
const examRoomSeatsFile = path.join(dataFolder, 'examRoomSeats.json');
const studentPhotosFile = path.join(dataFolder, 'studentPhotos.json');

// 确保数据目录存在
if (!fs.existsSync(dataFolder)) {
    fs.mkdirSync(dataFolder, { recursive: true });
    console.log(`创建数据文件夹: ${dataFolder}`);
}

// 生成测试用户数据
const generateUsers = () => {
    const saltRounds = 10;
    
    // 系统需要的三种角色用户
    const users = [
        {
            id: 1,
            username: 'admin',
            password: bcrypt.hashSync('admin23', saltRounds),
            role: 'admin',
            name: '系统管理员'
        },
        {
            id: 2,
            username: 'teacher',
            password: bcrypt.hashSync('teacher123', saltRounds),
            role: 'teacher',
            name: '张老师'
        },
        {
            id: 3,
            username: 'student',
            password: bcrypt.hashSync('student123', saltRounds),
            role: 'student',
            name: '王同学'
        }
    ];
    
    fs.writeFileSync(usersFile, JSON.stringify(users, null, 2));
    console.log(`生成用户数据: ${users.length}个用户`);
    return users;
};

// 生成测试班级数据
const generateClasses = () => {
    const classes = [
        {
            id: '1',
            className: '计算机科学与技术2023-1班',
            grade: '2023',
            major: '计算机科学与技术',
            teacherId: 2
        },
        {
            id: '2',
            className: '软件工程2023-1班',
            grade: '2023',
            major: '软件工程',
            teacherId: 2
        },
        {
            id: '3',
            className: '人工智能2023-1班',
            grade: '2023',
            major: '人工智能',
            teacherId: 2
        }
    ];
    
    fs.writeFileSync(classesFile, JSON.stringify(classes, null, 2));
    console.log(`生成班级数据: ${classes.length}个班级`);
    return classes;
};

// 生成测试考场数据
const generateExamRooms = () => {
    const examRooms = [
        {
            id: '1',
            name: '计算机原理期末考试',
            location: '教学楼A101',
            capacity: 60,
            rows: 6,
            columns: 10,
            examDate: '2023-12-25',
            examTime: '09:00-11:00',
            teacherId: 2
        },
        {
            id: '2',
            name: '数据结构期末考试',
            location: '教学楼B203',
            capacity: 45,
            rows: 5,
            columns: 9,
            examDate: '2023-12-26',
            examTime: '14:00-16:00',
            teacherId: 2
        }
    ];
    
    fs.writeFileSync(examRoomsFile, JSON.stringify(examRooms, null, 2));
    console.log(`生成考场数据: ${examRooms.length}个考场`);
    return examRooms;
};

// 生成测试学生数据
const generateStudents = () => {
    const students = [];
    const classCount = 3;
    const studentsPerClass = 20;
    
    for (let classIndex = 1; classIndex <= classCount; classIndex++) {
        for (let studentIndex = 1; studentIndex <= studentsPerClass; studentIndex++) {
            const studentId = `20231000${classIndex.toString().padStart(1, '0')}${studentIndex.toString().padStart(2, '0')}`;
            students.push({
                id: students.length + 1,
                studentId: studentId,
                name: `学生${classIndex}-${studentIndex}`,
                gender: studentIndex % 2 === 0 ? '男' : '女',
                userId: studentIndex === 1 && classIndex === 1 ? 3 : null // 第一个学生关联到测试学生账号
            });
        }
    }
    
    fs.writeFileSync(studentsFile, JSON.stringify(students, null, 2));
    console.log(`生成学生数据: ${students.length}个学生`);
    return students;
};

// 生成班级-学生关联数据
const generateClassStudents = (students) => {
    const classStudents = {};
    const classCount = 3;
    const studentsPerClass = 20;
    
    for (let classId = 1; classId <= classCount; classId++) {
        const classStudentIds = [];
        const startIdx = (classId - 1) * studentsPerClass;
        
        for (let i = 0; i < studentsPerClass; i++) {
            const studentIndex = startIdx + i;
            if (studentIndex < students.length) {
                classStudentIds.push(students[studentIndex].id);
            }
        }
        
        classStudents[classId.toString()] = classStudentIds;
    }
    
    fs.writeFileSync(classStudentsFile, JSON.stringify(classStudents, null, 2));
    console.log(`生成班级-学生关联数据: ${Object.keys(classStudents).length}个班级关联`);
    return classStudents;
};

// 生成考场-班级关联数据
const generateExamRoomClasses = () => {
    const examRoomClasses = {
        '1': ['1', '2'], // 考场1关联班级1和2
        '2': ['3']       // 考场2关联班级3
    };
    
    fs.writeFileSync(examRoomClassesFile, JSON.stringify(examRoomClasses, null, 2));
    console.log(`生成考场-班级关联数据: ${Object.keys(examRoomClasses).length}个考场关联`);
    return examRoomClasses;
};

// 生成考场座位安排
const generateExamRoomSeats = (examRooms, students, classStudents, examRoomClasses) => {
    const examRoomSeats = {};
    
    for (const examRoom of examRooms) {
        const examRoomId = examRoom.id;
        const classIds = examRoomClasses[examRoomId] || [];
        const seats = [];
        
        // 获取该考场所有班级的学生
        const examRoomStudents = [];
        for (const classId of classIds) {
            const studentIds = classStudents[classId] || [];
            for (const studentId of studentIds) {
                const student = students.find(s => s.id === studentId);
                if (student) {
                    examRoomStudents.push(student);
                }
            }
        }
        
        // 随机排座
        const shuffledStudents = [...examRoomStudents].sort(() => 0.5 - Math.random());
        
        // 生成座位
        let seatIndex = 0;
        for (let row = 1; row <= examRoom.rows; row++) {
            for (let column = 1; column <= examRoom.columns; column++) {
                const student = shuffledStudents[seatIndex];
                if (student && seatIndex < shuffledStudents.length) {
                    seats.push({
                        row: row,
                        column: column,
                        studentId: student.id,
                        name: student.name,
                        status: '已分配'
                    });
                    seatIndex++;
                } else {
                    // 没有更多学生，生成空座位
                    seats.push({
                        row: row,
                        column: column,
                        studentId: null,
                        name: null,
                        status: '空座位'
                    });
                }
            }
        }
        
        examRoomSeats[examRoomId] = seats;
    }
    
    fs.writeFileSync(examRoomSeatsFile, JSON.stringify(examRoomSeats, null, 2));
    console.log(`生成考场座位数据: ${Object.keys(examRoomSeats).length}个考场的座位安排`);
    return examRoomSeats;
};

// 生成学生照片数据
const generateStudentPhotos = (students) => {
    const studentPhotos = {};
    
    // 为第一个班级的学生生成照片状态
    for (let i = 0; i < 20; i++) {
        const studentId = students[i].id;
        
        // 不同状态的照片
        let status;
        if (i < 5) {
            status = 'uploaded'; // 已上传
        } else if (i < 10) {
            status = 'requested'; // 已请求上传
        } else {
            status = 'none'; // 未上传
        }
        
        studentPhotos[studentId] = {
            status: status,
            requestTime: status === 'requested' ? new Date().toISOString() : null,
            uploadTime: status === 'uploaded' ? new Date().toISOString() : null,
            photoPath: status === 'uploaded' ? `./photos/student_${studentId}.jpg` : null
        };
    }
    
    fs.writeFileSync(studentPhotosFile, JSON.stringify(studentPhotos, null, 2));
    console.log(`生成学生照片数据: ${Object.keys(studentPhotos).length}个学生的照片状态`);
    return studentPhotos;
};

// 生成考勤记录数据
const generateAttendance = (examRooms, students, examRoomClasses, classStudents) => {
    const attendance = [];
    let recordId = 1;
    
    for (const examRoom of examRooms) {
        const examRoomId = examRoom.id;
        const classIds = examRoomClasses[examRoomId] || [];
        
        // 获取该考场所有班级的学生
        const examRoomStudents = [];
        for (const classId of classIds) {
            const studentIds = classStudents[classId] || [];
            for (const studentId of studentIds) {
                const student = students.find(s => s.id === studentId);
                if (student) {
                    examRoomStudents.push(student);
                }
            }
        }
        
        // 为80%的学生生成考勤记录
        const attendanceCount = Math.floor(examRoomStudents.length * 0.8);
        for (let i = 0; i < attendanceCount; i++) {
            const student = examRoomStudents[i];
            if (student) {
                attendance.push({
                    id: recordId++,
                    studentId: student.id,
                    examRoomId: examRoomId,
                    time: new Date().toISOString(),
                    status: i < attendanceCount * 0.9 ? '正常' : '迟到'
                });
            }
        }
    }
    
    fs.writeFileSync(attendanceFile, JSON.stringify(attendance, null, 2));
    console.log(`生成考勤记录数据: ${attendance.length}条考勤记录`);
    return attendance;
};

// 执行数据生成
const generateAllData = () => {
    console.log('开始生成测试数据...');
    
    const users = generateUsers();
    const classes = generateClasses();
    const examRooms = generateExamRooms();
    const students = generateStudents();
    const classStudents = generateClassStudents(students);
    const examRoomClasses = generateExamRoomClasses();
    const examRoomSeats = generateExamRoomSeats(examRooms, students, classStudents, examRoomClasses);
    const studentPhotos = generateStudentPhotos(students);
    const attendance = generateAttendance(examRooms, students, examRoomClasses, classStudents);
    
    console.log('测试数据生成完成!');
    console.log('现在可以启动服务器使用测试数据。');
    console.log('\n测试账号信息:');
    console.log('- 管理员: admin/admin23');
    console.log('- 教师: teacher/teacher123');
    console.log('- 学生: student/student123');
};

// 执行数据生成
generateAllData(); 