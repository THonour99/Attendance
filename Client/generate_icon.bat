@echo off
echo 正在检查图标文件...

set ICON_PATH=Client\app_icon.ico
set DEFAULT_ICON=C:\Windows\System32\SHELL32.dll,22

if exist "%ICON_PATH%" (
    echo 图标文件已存在: %ICON_PATH%
) else (
    echo 图标文件不存在，将创建一个默认图标...
    echo 注意: 这将使用Windows系统图标作为临时图标
    
    :: 使用PowerShell从系统DLL提取图标
    powershell -Command "Add-Type -AssemblyName System.Drawing; Add-Type -AssemblyName System.Windows.Forms; $icon = [System.Drawing.Icon]::ExtractAssociatedIcon('C:\Windows\System32\SHELL32.dll'); $icon.ToBitmap().Save('Client\app_icon.bmp', [System.Drawing.Imaging.ImageFormat]::Bmp); Write-Host '创建了临时BMP图像，请使用图标编辑器转换为ICO格式'"
    
    echo.
    echo 请注意: 
    echo 1. 已创建临时图像 Client\app_icon.bmp
    echo 2. 您需要使用图标编辑器(如IcoFX或在线转换工具)将其转换为ICO格式
    echo 3. 保存为 Client\app_icon.ico
    echo.
    echo 或者您可以复制任何您喜欢的ICO文件到 Client\app_icon.ico
)

echo.
echo 请确保编译前，图标文件 Client\app_icon.ico 存在并且是有效的ICO格式。
echo 按任意键退出...
pause > nul 