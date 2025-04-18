@echo off
echo 正在切换到简化版实现方式...

cd Client

if not exist main_simple.cpp (
    echo 错误：找不到main_simple.cpp文件
    exit /b 1
)

echo 备份当前文件...
copy main.cpp main.cpp.bak

echo 替换为简化版实现...
copy /y main_simple.cpp main.cpp

echo 完成！
echo 请重新编译项目。
pause 