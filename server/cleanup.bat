@echo off
echo 正在整理Server文件夹...
echo.

rem 创建备份文件夹
echo 创建备份文件夹...
mkdir backup 2>nul

rem 移动不需要的文件到备份文件夹
echo 移动不需要的文件到备份文件夹...

rem 测试脚本和旧版数据生成脚本
move /Y generate-test-data-new.js backup\ 2>nul
move /Y generate-test-data.bat backup\ 2>nul
move /Y generate_test_data.js backup\ 2>nul
move /Y reset-data.js backup\ 2>nul
move /Y reset-data.bat backup\ 2>nul
move /Y create-students.js backup\ 2>nul
move /Y create-and-assign-students.js backup\ 2>nul
move /Y run-create-students.bat backup\ 2>nul
move /Y run-complete-setup.bat backup\ 2>nul
move /Y data.json backup\ 2>nul
move /Y test-api.js backup\ 2>nul
move /Y test-requests.js backup\ 2>nul
move /Y test-token-refresh.js backup\ 2>nul
move /Y generate_test_students.py backup\ 2>nul
move /Y generate-test-students.cpp backup\ 2>nul
move /Y client-patch.md backup\ 2>nul
move /Y client-patch-fix.cpp backup\ 2>nul
move /Y solution.md backup\ 2>nul
move /Y fix-client.md backup\ 2>nul
move /Y students.csv backup\ 2>nul

rem 对README-client.md进行特殊处理，如果存在则重命名并移动
if exist README-client.md (
    move /Y README-client.md backup\client-README.md 2>nul
)

echo.
echo 文件整理完成！
echo 不需要的文件已移动到backup文件夹
echo 您可以在确认系统正常运行后删除该文件夹
echo.
echo 保留的核心文件:
echo  * server.js - 服务器主程序
echo  * package.json - NPM依赖配置
echo  * fix-data-generate.js - 最新的数据生成脚本  
echo  * fix-data.bat - 最新的数据生成批处理文件
echo  * start-server.bat - 启动服务器的批处理文件
echo  * README.md - 项目说明文档
echo  * data/目录 - 数据库文件存储位置
echo  * uploads/目录 - 照片上传存储位置
echo.
echo 要恢复文件，您可以从backup文件夹中复制回来
echo.
echo 如果需要重新生成测试数据，请运行fix-data.bat文件
echo 如果仅需要启动服务器，请运行start-server.bat文件
echo.
pause 