/**
 * 数据重置脚本 - 清空所有测试数据，保留管理员账号
 * 运行方法: node reset-data.js
 */

const fs = require('fs');
const path = require('path');

// 数据文件路径
const DATA_FILE_PATH = './data.json';

console.log('====== 开始重置数据 ======');

// 初始化空的数据结构
let data = {
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

// 如果存在现有文件，检查管理员账号
try {
  if (fs.existsSync(DATA_FILE_PATH)) {
    console.log(`正在读取数据文件: ${DATA_FILE_PATH}`);
    const existingData = JSON.parse(fs.readFileSync(DATA_FILE_PATH, 'utf8'));
    
    // 保留管理员账号
    if (existingData.users && Array.isArray(existingData.users)) {
      const adminUsers = existingData.users.filter(user => user.role === 'admin');
      if (adminUsers.length > 0) {
        console.log(`保留${adminUsers.length}个管理员账号`);
        data.users = adminUsers;
      } else {
        console.log('未找到管理员账号，创建默认管理员');
        data.users.push({
          id: 1,
          username: 'admin',
          password: 'admin23',
          role: 'admin',
          name: '系统管理员'
        });
      }
    }
  } else {
    console.log('未找到现有数据文件，将创建初始数据结构');
    // 创建默认管理员账号
    data.users.push({
      id: 1,
      username: 'admin',
      password: 'admin23',
      role: 'admin',
      name: '系统管理员'
    });
  }
} catch (error) {
  console.error('读取数据文件出错:', error.message);
  console.log('创建默认管理员账号');
  data.users.push({
    id: 1,
    username: 'admin',
    password: 'admin23',
    role: 'admin',
    name: '系统管理员'
  });
}

// 保存重置后的数据
try {
  fs.writeFileSync(DATA_FILE_PATH, JSON.stringify(data, null, 2));
  console.log(`数据已重置并保存到: ${DATA_FILE_PATH}`);
  
  console.log('\n数据统计:');
  console.log(`用户: ${data.users.length}个 (仅保留管理员)`);
  console.log('班级: 0个');
  console.log('考场: 0个');
  console.log('学生: 0个');
} catch (error) {
  console.error('保存数据文件出错:', error.message);
}

console.log('\n====== 数据重置完成 ======');
console.log('现在你可以运行 generate-test-data-new.js 生成新的测试数据'); 