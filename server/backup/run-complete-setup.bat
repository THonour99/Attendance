@echo off
echo 自动化设置脚本 - 启动服务器和创建学生账号
echo ===================================================

echo 1. 安装所需的npm包...
call npm install node-fetch

echo.
echo 2. 启动服务器(后台运行)...
start cmd /k "node server.js"

echo.
echo 等待服务器启动... (5秒)
timeout /t 5 /nobreak > nul

echo.
echo 3. 创建学生账号并分配班级...
node create-and-assign-students.js

echo.
echo ===================================================
echo 设置完成!
echo 服务器已在另一个窗口中运行，请勿关闭该窗口。
echo 现在您可以使用学生账号登录系统。
echo ===================================================
echo.
echo 按任意键退出此窗口...
pause > nul 