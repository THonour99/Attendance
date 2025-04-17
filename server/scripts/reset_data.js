const fs = require('fs');
const path = require('path');
const { exec } = require('child_process');

// 定义数据文件路径
const dataFilePath = path.join(__dirname, '..', 'data.json');

console.log('开始重置数据...');
console.log(`数据文件路径: ${dataFilePath}`);

// 检查文件是否存在
if (fs.existsSync(dataFilePath)) {
    console.log('找到现有数据文件，准备删除...');
    
    try {
        fs.unlinkSync(dataFilePath);
        console.log('成功删除现有数据文件');
    } catch (err) {
        console.error('删除数据文件时出错:', err);
        process.exit(1);
    }
} else {
    console.log('未找到现有数据文件，将直接生成新数据');
}

// 运行测试设置脚本
console.log('开始生成测试数据...');
exec('node scripts/test_setup.js', { cwd: path.join(__dirname, '..') }, (error, stdout, stderr) => {
    if (error) {
        console.error('生成测试数据时出错:', error);
        console.error(stderr);
        process.exit(1);
    }
    
    console.log('测试数据输出:');
    console.log(stdout);
    
    // 确认数据文件是否已创建
    if (fs.existsSync(dataFilePath)) {
        const stats = fs.statSync(dataFilePath);
        console.log(`数据文件已生成，大小: ${stats.size} 字节`);
        console.log('数据重置成功完成！');
    } else {
        console.error('错误：测试数据脚本运行后，数据文件仍未创建');
        process.exit(1);
    }
}); 