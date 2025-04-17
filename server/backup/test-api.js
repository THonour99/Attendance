const fetch = require('node-fetch');

// 测试配置
const BASE_URL = 'http://localhost:8080';
let authToken = '';

// 辅助函数：打印响应
async function printResponse(response) {
    const contentType = response.headers.get('content-type');
    if (contentType && contentType.includes('application/json')) {
        const data = await response.json();
        console.log(JSON.stringify(data, null, 2));
        return data;
    } else {
        const text = await response.text();
        console.log(text);
        return text;
    }
}

// 测试登录
async function testLogin() {
    console.log('\n测试登录接口...');
    const response = await fetch(`${BASE_URL}/auth/login`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
            username: 'operator',
            password: 'operator123'
        })
    });
    
    console.log('状态码:', response.status);
    const data = await printResponse(response);
    
    if (data.token) {
        authToken = data.token;
        console.log('获取到认证令牌');
        return true;
    }
    return false;
}

// 测试获取考场列表
async function testGetExamRooms() {
    console.log('\n测试获取考场列表...');
    const response = await fetch(`${BASE_URL}/teacher/examrooms`, {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${authToken}`
        }
    });
    
    console.log('状态码:', response.status);
    await printResponse(response);
}

// 主测试函数
async function runTests() {
    try {
        console.log('开始API测试...');
        
        // 测试1：登录
        const loginSuccess = await testLogin();
        if (!loginSuccess) {
            console.log('登录失败，测试终止');
            return;
        }
        
        // 测试2：获取考场列表
        await testGetExamRooms();
        
        console.log('\n测试完成');
    } catch (error) {
        console.error('测试出错:', error);
    }
}

// 运行测试
runTests(); 