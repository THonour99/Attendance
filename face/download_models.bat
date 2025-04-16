@echo off
echo 正在下载人脸检测模型...

mkdir data 2>nul

curl -L https://raw.githubusercontent.com/opencv/opencv/master/data/haarcascades/haarcascade_frontalface_default.xml -o haarcascade_frontalface_default.xml

echo 下载完成！
pause 