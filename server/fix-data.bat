@echo off
echo 开始修复并生成测试数据...
echo.

rem 先停止服务器
echo 正在停止服务器...
taskkill /f /im node.exe >nul 2>nul

rem 检查是否已安装Node.js
where node >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo 错误: 未找到Node.js，请安装Node.js后再运行此脚本
    pause
    exit /b 1
)

rem 运行修复的数据生成脚本
echo 正在生成测试数据...
node fix-data-generate.js

echo.
echo 数据生成完成！
echo 现在你可以用以下账号进行测试:
echo.
echo 管理员账号: admin / admin23
echo 教师账号: teacher1 / teacher123 (还有teacher2-teacher5)
echo 学生账号: 20230101 / student123 (格式为: 班级前缀+学号)
echo.
echo 按任意键重新启动服务器...
pause

rem 重启服务器
echo 正在启动服务器...
start node server.js

echo 服务器已启动，现在可以进行测试了！
echo 按任意键退出...
pause 