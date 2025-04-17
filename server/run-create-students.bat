@echo off
echo 正在安装node-fetch包(如果尚未安装)...
call npm install node-fetch

echo.
echo 开始创建学生账号...
node create-students.js

echo.
echo 脚本执行完成，按任意键退出...
pause > nul 