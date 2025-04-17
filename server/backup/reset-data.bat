@echo off
echo 开始重置所有测试数据...
echo 注意: 这将删除所有班级、学生、考场等数据，但保留管理员账号
echo.

set /p confirm=确定要继续吗？(Y/N) 
if /i "%confirm%" neq "Y" goto :end

rem 检查是否已安装Node.js
where node >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo 错误: 未找到Node.js，请安装Node.js后再运行此脚本
    pause
    exit /b 1
)

rem 先停止服务器
echo 正在停止服务器...
taskkill /f /im node.exe >nul 2>nul

rem 运行数据重置脚本
echo.
echo 正在重置数据...
node reset-data.js

echo.
echo 数据重置完成！
echo 现在你可以:
echo  1. 运行 generate-test-data.bat 生成新的测试数据
echo  2. 或直接启动服务器使用默认管理员账号
echo.
echo 是否要立即生成测试数据？

set /p gendata=生成测试数据？(Y/N) 
if /i "%gendata%"=="Y" (
    echo.
    call generate-test-data.bat
) else (
    echo.
    echo 未生成测试数据，仅保留管理员账号
    echo 管理员账号: admin / admin23
    echo.
    echo 按任意键启动服务器...
    pause >nul
    start node server.js
)

:end
echo.
echo 操作已完成！
pause 