/**
 * 自动创建学生账号脚本
 * 
 * 此脚本自动登录管理员账号，然后创建多个学生账号
 */

const fetch = require('node-fetch');

// 服务器地址
const BASE_URL = 'http://localhost:8080';

// 管理员凭据
const ADMIN_CREDENTIALS = {
    username: 'admin',
    password: 'admin123'
};

// 要创建的学生账号列表
const STUDENTS_TO_CREATE = [
    { username: '2023100001', password: 'student123', name: '学生1', role: 'student' },
    { username: '2023100002', password: 'student123', name: '学生2', role: 'student' },
    { username: '2023200001', password: 'student123', name: '学生3', role: 'student' },
    { username: '2023200002', password: 'student123', name: '学生4', role: 'student' },
    { username: '2023300001', password: 'student123', name: '学生5', role: 'student' },
    { username: '2023300002', password: 'student123', name: '学生6', role: 'student' }
];

// 存储授权令牌
let authToken = '';

/**
 * 登录管理员账号获取令牌
 */
async function loginAsAdmin() {
    console.log('正在登录管理员账号...');
    
    try {
        const response = await fetch(`${BASE_URL}/auth/login`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(ADMIN_CREDENTIALS)
        });
        
        const data = await response.json();
        
        if (!response.ok || data.status !== 'success') {
            throw new Error(`登录失败: ${data.message || response.statusText}`);
        }
        
        authToken = data.token;
        console.log('管理员登录成功，已获取令牌');
        
        return true;
    } catch (error) {
        console.error('登录过程中出错:', error.message);
        return false;
    }
}

/**
 * 创建一个学生账号
 */
async function createStudent(student) {
    console.log(`创建学生账号: ${student.username} (${student.name})`);
    
    try {
        const response = await fetch(`${BASE_URL}/admin/users`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${authToken}`
            },
            body: JSON.stringify(student)
        });
        
        const data = await response.json();
        
        if (!response.ok || data.status !== 'success') {
            throw new Error(`添加学生失败: ${data.message || response.statusText}`);
        }
        
        console.log(`✓ 学生 ${student.name} (${student.username}) 创建成功`);
        return true;
    } catch (error) {
        console.error(`× 创建学生 ${student.username} 失败:`, error.message);
        return false;
    }
}

/**
 * 主函数
 */
async function main() {
    console.log('=== 开始创建学生账号 ===');
    
    // 登录获取令牌
    const loginSuccess = await loginAsAdmin();
    if (!loginSuccess) {
        console.error('管理员登录失败，无法继续');
        return;
    }
    
    let successCount = 0;
    let failCount = 0;
    
    // 依次创建每个学生账号
    for (const student of STUDENTS_TO_CREATE) {
        const success = await createStudent(student);
        if (success) {
            successCount++;
        } else {
            failCount++;
        }
    }
    
    // 输出总结
    console.log('\n=== 学生账号创建完成 ===');
    console.log(`成功: ${successCount}`);
    console.log(`失败: ${failCount}`);
    console.log('=========================');
}

// 运行主函数
main().catch(error => {
    console.error('程序执行出错:', error);
}); 