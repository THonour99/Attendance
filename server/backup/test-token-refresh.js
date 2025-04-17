const fetch = require('node-fetch');
const fs = require('fs');
const path = require('path');

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
            username: 'admin',
            password: 'admin123'
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

// 测试令牌刷新
async function testTokenRefresh() {
    console.log('\n测试令牌刷新...');
    const response = await fetch(`${BASE_URL}/auth/refresh`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${authToken}`
        },
        body: JSON.stringify({})
    });
    
    console.log('状态码:', response.status);
    const data = await printResponse(response);
    
    if (data.token) {
        authToken = data.token;
        console.log('令牌刷新成功，获取到新令牌');
        return true;
    }
    return false;
}

// 测试添加用户
async function testAddUser() {
    console.log('\n测试添加用户...');
    
    // 创建一个随机用户名
    const username = `testuser_${Math.floor(Math.random() * 1000)}`;
    
    const response = await fetch(`${BASE_URL}/admin/users`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': `Bearer ${authToken}`
        },
        body: JSON.stringify({
            username: username,
            password: 'test123',
            name: '测试用户',
            role: 'student'
        })
    });
    
    console.log('状态码:', response.status);
    const data = await printResponse(response);
    
    if (data.status === 'success') {
        console.log(`成功添加用户: ${username}`);
        return data.data.id;
    }
    return null;
}

// 检查数据库文件
function checkDatabaseFile() {
    const dbFile = path.join(__dirname, 'data', 'database.json');
    console.log('\n检查数据库文件...');
    
    if (fs.existsSync(dbFile)) {
        const data = JSON.parse(fs.readFileSync(dbFile, 'utf8'));
        console.log(`数据库文件存在，最后更新时间: ${data.lastUpdate}`);
        console.log(`用户数量: ${data.users.length}`);
        return true;
    } else {
        console.log('数据库文件不存在');
        return false;
    }
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
        
        // 测试2：添加用户
        const userId = await testAddUser();
        if (!userId) {
            console.log('添加用户失败，测试终止');
            return;
        }
        
        // 测试3：令牌刷新
        const refreshSuccess = await testTokenRefresh();
        if (!refreshSuccess) {
            console.log('令牌刷新失败');
        }
        
        // 测试4：检查数据库文件
        const dbExists = checkDatabaseFile();
        if (!dbExists) {
            console.log('数据库持久化测试失败');
        }
        
        console.log('\n测试完成');
    } catch (error) {
        console.error('测试出错:', error);
    }
}

// 运行测试
runTests(); 