@echo off
echo 正在切换到纯Qt实现方式（不使用Windows API）...

cd Client

if not exist main_alt.cpp (
    echo 错误：找不到main_alt.cpp文件
    exit /b 1
)

if not exist mainwindow_alt.cpp (
    echo 错误：找不到mainwindow_alt.cpp文件
    exit /b 1
)

echo 备份当前文件...
copy main.cpp main.cpp.bak
copy mainwindow.cpp mainwindow.cpp.bak

echo 替换为纯Qt实现...
copy /y main_alt.cpp main.cpp
copy /y mainwindow_alt.cpp mainwindow.cpp

echo 完成！
echo 请重新编译项目。
pause 