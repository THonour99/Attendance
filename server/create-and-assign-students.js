/**
 * 自动创建学生账号并分配班级脚本
 * 
 * 此脚本自动登录管理员账号，创建多个学生账号，然后将学生分配到相应班级
 */

const fetch = require('node-fetch');

// 服务器地址
const BASE_URL = 'http://localhost:8080';

// 管理员凭据
const ADMIN_CREDENTIALS = {
    username: 'admin',
    password: 'admin123'
};

// 要创建的学生账号列表及其班级分配
const STUDENTS_TO_CREATE = [
    { username: '2023100001', password: 'student123', name: '学生1', role: 'student', classId: 1 },
    { username: '2023100002', password: 'student123', name: '学生2', role: 'student', classId: 1 },
    { username: '2023200001', password: 'student123', name: '学生3', role: 'student', classId: 2 },
    { username: '2023200002', password: 'student123', name: '学生4', role: 'student', classId: 2 },
    { username: '2023300001', password: 'student123', name: '学生5', role: 'student', classId: 3 },
    { username: '2023300002', password: 'student123', name: '学生6', role: 'student', classId: 3 }
];

// 存储授权令牌
let authToken = '';
// 存储创建的学生ID
const createdStudents = new Map();

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
            // 只发送API需要的字段
            body: JSON.stringify({
                username: student.username,
                password: student.password,
                name: student.name,
                role: student.role
            })
        });
        
        const data = await response.json();
        
        if (!response.ok || data.status !== 'success') {
            throw new Error(`添加学生失败: ${data.message || response.statusText}`);
        }
        
        console.log(`✓ 学生 ${student.name} (${student.username}) 创建成功，ID: ${data.data.id}`);
        
        // 保存创建的学生ID，用于后续分配班级
        createdStudents.set(student.username, {
            id: data.data.id,
            name: student.name,
            classId: student.classId
        });
        
        return true;
    } catch (error) {
        console.error(`× 创建学生 ${student.username} 失败:`, error.message);
        return false;
    }
}

/**
 * 以教师身份登录
 */
async function loginAsTeacher() {
    console.log('\n正在登录教师账号...');
    
    try {
        const response = await fetch(`${BASE_URL}/auth/login`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                username: 'teacher',
                password: 'teacher123'
            })
        });
        
        const data = await response.json();
        
        if (!response.ok || data.status !== 'success') {
            throw new Error(`教师登录失败: ${data.message || response.statusText}`);
        }
        
        authToken = data.token;
        console.log('教师登录成功，已获取令牌');
        
        return true;
    } catch (error) {
        console.error('教师登录过程中出错:', error.message);
        return false;
    }
}

/**
 * 将学生添加到班级
 */
async function addStudentToClass(studentInfo) {
    const { id, name, classId } = studentInfo;
    
    console.log(`将学生 ${name} 添加到班级 ${classId}...`);
    
    try {
        const response = await fetch(`${BASE_URL}/teacher/classes/${classId}/students`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': `Bearer ${authToken}`
            },
            body: JSON.stringify({
                studentId: id.toString(),
                name: name
            })
        });
        
        const data = await response.json();
        
        if (!response.ok || data.status !== 'success') {
            throw new Error(`添加学生到班级失败: ${data.message || response.statusText}`);
        }
        
        console.log(`✓ 学生 ${name} 成功添加到班级 ${classId}`);
        return true;
    } catch (error) {
        console.error(`× 添加学生 ${name} 到班级 ${classId} 失败:`, error.message);
        return false;
    }
}

/**
 * 将所有学生分配到各自的班级
 */
async function assignStudentsToClasses() {
    console.log('\n=== 开始将学生分配到班级 ===');
    
    // 登录教师账号
    const loginSuccess = await loginAsTeacher();
    if (!loginSuccess) {
        console.error('教师登录失败，无法分配班级');
        return false;
    }
    
    let successCount = 0;
    let failCount = 0;
    
    // 遍历所有创建的学生，将他们添加到对应班级
    for (const [username, studentInfo] of createdStudents.entries()) {
        const success = await addStudentToClass(studentInfo);
        if (success) {
            successCount++;
        } else {
            failCount++;
        }
    }
    
    console.log('\n=== 学生班级分配完成 ===');
    console.log(`成功: ${successCount}`);
    console.log(`失败: ${failCount}`);
    
    return successCount > 0;
}

/**
 * 主函数
 */
async function main() {
    console.log('=== 开始创建学生账号并分配班级 ===');
    
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
    
    // 输出学生创建总结
    console.log('\n=== 学生账号创建完成 ===');
    console.log(`成功: ${successCount}`);
    console.log(`失败: ${failCount}`);
    
    // 如果有学生成功创建，则进行班级分配
    if (successCount > 0) {
        await assignStudentsToClasses();
    }
    
    console.log('\n=== 整个过程已完成 ===');
}

// 运行主函数
main().catch(error => {
    console.error('程序执行出错:', error);
}); 