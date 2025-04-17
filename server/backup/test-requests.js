const fetch = require('node-fetch');

const BASE_URL = 'http://localhost:8080';
let token = '';

async function login() {
    console.log('测试登录接口...');
    const response = await fetch(`${BASE_URL}/auth/login`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({
            username: 'operator',
            password: 'operator123'
        })
    });
    
    const data = await response.json();
    console.log('登录结果:', data);
    
    if (data.token) {
        token = data.token;
        console.log('获取令牌成功:', token);
        return true;
    }
    
    return false;
}

async function getExamRooms() {
    console.log('\n测试获取考场信息接口...');
    const response = await fetch(`${BASE_URL}/teacher/examrooms`, {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${token}`
        }
    });
    
    const data = await response.json();
    console.log('考场信息:', data);
    
    return data.data || [];
}

async function getStudentsForExam(examRoomId) {
    console.log(`\n测试获取考场ID=${examRoomId}的学生名单接口...`);
    const response = await fetch(`${BASE_URL}/attendance/students`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify({
            examRoomId
        })
    });
    
    const data = await response.json();
    console.log(`考场ID=${examRoomId}的学生名单:`, data);
    
    return data.data || [];
}

async function recordAttendance(studentId, examRoomId) {
    console.log(`\n测试记录考勤接口 (学生ID=${studentId}, 考场ID=${examRoomId})...`);
    const response = await fetch(`${BASE_URL}/attendance/record`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify({
            studentId,
            examRoomId,
            time: new Date().toISOString()
        })
    });
    
    const data = await response.json();
    console.log('考勤记录结果:', data);
    
    return data;
}

async function getAttendanceRecords() {
    console.log('\n测试获取考勤记录接口...');
    const response = await fetch(`${BASE_URL}/attendance/records`, {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${token}`
        }
    });
    
    const data = await response.json();
    console.log('考勤记录:', data);
    
    return data.data || [];
}

async function runTests() {
    try {
        // 测试登录
        const loggedIn = await login();
        if (!loggedIn) {
            console.error('登录失败，测试终止');
            return;
        }
        
        // 测试获取考场信息
        const examRooms = await getExamRooms();
        if (examRooms.length === 0) {
            console.error('未获取到考场信息，测试终止');
            return;
        }
        
        // 选择第一个考场
        const examRoomId = examRooms[0].id;
        
        // 测试获取学生名单
        const students = await getStudentsForExam(examRoomId);
        if (students.length === 0) {
            console.error('未获取到学生名单，测试终止');
            return;
        }
        
        // 选择前两个学生进行考勤
        for (let i = 0; i < Math.min(2, students.length); i++) {
            await recordAttendance(students[i].id, examRoomId);
        }
        
        // 获取考勤记录
        await getAttendanceRecords();
        
        console.log('\n所有测试完成!');
    } catch (error) {
        console.error('测试过程中出错:', error);
    }
}

runTests(); 