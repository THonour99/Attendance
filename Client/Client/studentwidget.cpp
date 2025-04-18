#include "studentwidget.h"
#include "ui_studentwidget.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QUrl>
#include <QStandardItemModel>
#include <QDateTime>
#include <QHttpMultiPart>
#include <QBuffer>
#include <QLabel>
#include <QGridLayout>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QFileInfo>
#include <QIcon>
#include <QPixmap>
#include <QNetworkAccessManager>

// 服务器地址
const QString STUDENT_SERVER_URL = "http://localhost:8080";

StudentWidget::StudentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StudentWidget),
    networkManager(new QNetworkAccessManager(this)),
    notificationTimer(new QTimer(this)),
    hasRetryLoadSeat(false),
    currentPhotoUrl("")
{
    ui->setupUi(this);
    setupUI();
    
    // 连接网络错误信号
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        QString endpoint = reply->url().toString();
        
        if (endpoint.contains("/attendance/records")) {
            onAttendanceDataReceived(reply);
        } else if (endpoint.contains("/student/classes")) {
            onClassDataReceived(reply);
        } else if (endpoint.contains("/student/uploadPhoto")) {
            onPhotoUploadResponse(reply);
        } else if (endpoint.contains("/student/seat")) {
            onSeatInfoReceived(reply);
        } else if (endpoint.contains("/student/exam-info")) {
            onExamInfoReceived(reply);
        } else if (endpoint.contains("/notifications")) {
            // 不在这里处理通知，而是在专门的槽函数中处理
            reply->deleteLater();
        } else {
            // 处理其他未知响应
            reply->deleteLater();
        }
    });
    
    // 连接按钮信号
    connect(ui->refreshButton, &QPushButton::clicked, this, &StudentWidget::onRefreshButtonClicked);
    connect(ui->uploadPhotoButton, &QPushButton::clicked, this, &StudentWidget::onUploadPhotoButtonClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &StudentWidget::onLogoutButtonClicked);
    connect(ui->notificationButton, &QPushButton::clicked, this, &StudentWidget::onNotificationButtonClicked);
    
    // 连接通知定时器信号
    connect(notificationTimer, &QTimer::timeout, this, &StudentWidget::checkNotifications);
    
    // 加载用户信息和座位信息
    loadUserInfo();
    loadSeatInfo();
    checkNotifications();
}

StudentWidget::~StudentWidget()
{
    // 清理资源
    delete ui;
}

void StudentWidget::setUserInfo(const QString& name, const QString& studentId, const QString& token)
{
    this->userName = name;
    this->studentId = studentId;
    this->userToken = token;
    
    ui->nameLabel->setText("姓名: " + name);
    ui->studentIdLabel->setText("学号: " + studentId);
    
    // 加载学生数据
    loadAttendanceRecords();
    loadClassInfo();
    loadSeatInfo();
    loadExamInfo();
    
    // 启动通知系统
    setupNotificationSystem();
}

void StudentWidget::setupUI()
{
    // 设置考勤表格模型
    QStandardItemModel *attendanceModel = new QStandardItemModel(this);
    attendanceModel->setHorizontalHeaderLabels(QStringList() << "考场" << "日期" << "状态" << "签到时间");
    ui->attendanceTableView->setModel(attendanceModel);
    ui->attendanceTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 设置班级信息表格模型
    QStandardItemModel *classModel = new QStandardItemModel(this);
    classModel->setHorizontalHeaderLabels(QStringList() << "班级名称" << "教师" << "地点");
    ui->classTableView->setModel(classModel);
    ui->classTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 初始化照片状态
    updatePhotoStatus("未上传");
    
    // 隐藏通知面板，直到有通知
    ui->notificationsFrame->setVisible(false);
}

void StudentWidget::loadAttendanceRecords()
{
    if (userToken.isEmpty() || studentId.isEmpty()) {
        return;
    }
    
    // 准备考勤记录请求URL和参数
    QString url = STUDENT_SERVER_URL + "/attendance/records";
    
    // 创建一个QNetworkAccessManager实例用于发送请求
    QNetworkAccessManager *tempManager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 发送请求并处理响应
    QNetworkReply *reply = tempManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        tempManager->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "网络错误", "获取考勤记录失败: " + reply->errorString());
            return;
        }
        
        onAttendanceDataReceived(reply);
    });
}

void StudentWidget::loadClassInfo()
{
    if (userToken.isEmpty() || studentId.isEmpty()) {
        return;
    }
    
    // 准备班级信息请求URL和参数
    QString url = STUDENT_SERVER_URL + "/student/classes";
    
    // 创建一个QNetworkAccessManager实例用于发送请求
    QNetworkAccessManager *tempManager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 发送请求并处理响应
    QNetworkReply *reply = tempManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        tempManager->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "网络错误", "获取班级信息失败: " + reply->errorString());
            return;
        }
        
        onClassDataReceived(reply);
    });
}

void StudentWidget::loadSeatInfo()
{
    if (userToken.isEmpty() || studentId.isEmpty()) {
        return;
    }
    
    qDebug() << "正在请求学生座位信息...";
    
    // 准备座位信息请求URL和参数
    QString url = STUDENT_SERVER_URL + "/student/seat";
    
    // 创建一个QNetworkAccessManager实例用于发送请求
    QNetworkAccessManager *tempManager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 发送请求并处理响应
    QNetworkReply *reply = tempManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        tempManager->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "座位信息请求失败:" << reply->errorString() << "URL:" << url;
            return;
        }
        
        onSeatInfoReceived(reply);
    });
}

void StudentWidget::loadExamInfo()
{
    if (userToken.isEmpty() || studentId.isEmpty()) {
        return;
    }
    
    // 准备考场信息请求URL和参数
    QString url = STUDENT_SERVER_URL + "/student/exam-info";
    
    // 创建一个QNetworkAccessManager实例用于发送请求
    QNetworkAccessManager *tempManager = new QNetworkAccessManager(this);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 发送请求并处理响应
    QNetworkReply *reply = tempManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        tempManager->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "网络错误", "获取考场信息失败: " + reply->errorString());
            return;
        }
        
        onExamInfoReceived(reply);
    });
}

void StudentWidget::onAttendanceDataReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "网络错误", "获取考勤记录失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("data") && jsonObject["data"].isArray()) {
            QJsonArray attendanceRecords = jsonObject["data"].toArray();
            
            // 更新考勤表格
            QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->attendanceTableView->model());
            model->removeRows(0, model->rowCount());
            
            for (int i = 0; i < attendanceRecords.size(); ++i) {
                QJsonObject record = attendanceRecords[i].toObject();
                
                QList<QStandardItem*> rowItems;
                rowItems << new QStandardItem(record["examRoom"].toString());
                
                // 格式化日期
                QString dateStr = record["date"].toString();
                QDateTime date = QDateTime::fromString(dateStr, Qt::ISODate);
                rowItems << new QStandardItem(date.toString("yyyy-MM-dd"));
                
                // 设置状态
                QString status = record["status"].toString();
                QStandardItem *statusItem = new QStandardItem(status);
                if (status == "出席") {
                    statusItem->setForeground(QBrush(Qt::green));
                } else if (status == "缺席") {
                    statusItem->setForeground(QBrush(Qt::red));
                } else if (status == "迟到") {
                    statusItem->setForeground(QBrush(Qt::yellow));
                }
                rowItems << statusItem;
                
                // 签到时间
                if (record.contains("checkInTime")) {
                    QString checkInTime = record["checkInTime"].toString();
                    QDateTime time = QDateTime::fromString(checkInTime, Qt::ISODate);
                    rowItems << new QStandardItem(time.toString("HH:mm:ss"));
                } else {
                    rowItems << new QStandardItem("-");
                }
                
                model->appendRow(rowItems);
            }
        }
    } else {
        QString errorMessage = "获取考勤记录失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
    }
    
    reply->deleteLater();
}

void StudentWidget::onClassDataReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "网络错误", "获取班级信息失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("data") && jsonObject["data"].isArray()) {
            QJsonArray classesInfo = jsonObject["data"].toArray();
            
            // 更新班级表格
            QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->classTableView->model());
            model->removeRows(0, model->rowCount());
            
            for (int i = 0; i < classesInfo.size(); ++i) {
                QJsonObject classInfo = classesInfo[i].toObject();
                
                QList<QStandardItem*> rowItems;
                rowItems << new QStandardItem(classInfo["className"].toString());
                rowItems << new QStandardItem(classInfo["teacher"].toString());
                rowItems << new QStandardItem(classInfo["location"].toString());
                
                model->appendRow(rowItems);
            }
        }
    } else {
        QString errorMessage = "获取班级信息失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
    }
    
    reply->deleteLater();
}

void StudentWidget::onRefreshButtonClicked()
{
    // 重新加载数据
    loadAttendanceRecords();
    loadClassInfo();
    loadSeatInfo();
    loadExamInfo();
}

void StudentWidget::onLogoutButtonClicked()
{
    // 实现退出登录的逻辑
    int result = QMessageBox::question(this, "退出登录", "确定要退出登录吗？", 
                                      QMessageBox::Yes | QMessageBox::No);
    
    if (result == QMessageBox::Yes) {
        // 清除用户信息
        userToken.clear();
        userName.clear();
        studentId.clear();
        
        // 停止通知定时器
        notificationTimer->stop();
        
        // 通知父窗口退出登录
        emit logout();
        
        // 关闭窗口
        close();
    }
}

void StudentWidget::onNotificationButtonClicked()
{
    // 切换通知面板的可见性
    bool isVisible = ui->notificationsFrame->isVisible();
    ui->notificationsFrame->setVisible(!isVisible);
    
    // 如果显示通知面板，立即刷新通知数据
    if (!isVisible) {
        qDebug() << "正在刷新通知数据...";
        checkNotifications();
    } else {
        qDebug() << "隐藏通知面板";
    }
    
    // 更新按钮样式
    if (!isVisible) {
        ui->notificationButton->setStyleSheet("background-color: #2196F3; color: white;");
    } else {
        // 如果有未读通知，保持红色提示
        int unreadCount = ui->notificationButton->text().contains("(") ? 
            ui->notificationButton->text().section("(", 1, 1).section(")", 0, 0).toInt() : 0;
        
        if (unreadCount > 0) {
            ui->notificationButton->setStyleSheet("background-color: #FF5722; color: white;");
        } else {
            ui->notificationButton->setStyleSheet("");
        }
    }
}

void StudentWidget::onUploadPhotoButtonClicked()
{
    // 打开文件对话框选择照片
    QString fileName = QFileDialog::getOpenFileName(this,
        "选择学生照片", "", "图片文件 (*.png *.jpg *.jpeg)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // 加载图片并检查大小
    QPixmap photo(fileName);
    if (photo.isNull()) {
        QMessageBox::warning(this, "错误", "无法加载选择的图片");
        return;
    }
    
    // 检查文件大小（限制为2MB）
    QFileInfo fileInfo(fileName);
    if (fileInfo.size() > 2 * 1024 * 1024) {
        QMessageBox::warning(this, "错误", "图片大小超过2MB限制");
        return;
    }
    
    // 将图片转换为字节数组
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法读取选择的图片");
        return;
    }
    
    QByteArray imageData = file.readAll();
    file.close();
    
    // 显示上传中状态
    updatePhotoStatus("上传中...");
    
    // 准备上传照片请求
    QString url = STUDENT_SERVER_URL + "/student/uploadPhoto";
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 创建multipart/form-data请求
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    
    // 添加用户ID部分
    QHttpPart userIdPart;
    userIdPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"userId\""));
    userIdPart.setBody(studentId.toUtf8());
    multiPart->append(userIdPart);
    
    // 添加照片部分
    QHttpPart photoPart;
    QString fileExtension = QFileInfo(fileName).suffix();
    QString mimeType = "image/" + fileExtension.toLower();
    QString contentDisposition = QString("form-data; name=\"photo\"; filename=\"photo.%1\"").arg(fileExtension);
    photoPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(contentDisposition));
    photoPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType));
    photoPart.setBody(imageData);
    multiPart->append(photoPart);
    
    // 发送POST请求
    QNetworkReply *reply = networkManager->post(request, multiPart);
    multiPart->setParent(reply); // 设置父对象，确保自动删除
}

void StudentWidget::onPhotoUploadResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();

        qDebug() << "照片上传响应:" << QString(responseData);

        // 检查响应格式，服务器可能返回status字段或success字段
        bool success = false;
        QString message;
        QJsonObject data;

        // 标准化状态和消息
        if (jsonObject.contains("status")) {
            // 旧API格式
            success = (jsonObject["status"].toInt() == 200 || jsonObject["status"].toString() == "success");
            message = jsonObject["message"].toString();
            if (jsonObject.contains("data")) {
                data = jsonObject["data"].toObject();
            }
        } else if (jsonObject.contains("success")) {
            // 新API格式
            success = jsonObject["success"].toBool();
            message = jsonObject["message"].toString();
            if (jsonObject.contains("data")) {
                data = jsonObject["data"].toObject();
            }
        }

        if (success) {
            QString photoUrl = data["photoUrl"].toString();
            
            // 确保URL是完整的
            if (!photoUrl.isEmpty()) {
                if (!photoUrl.startsWith("http")) {
                    // 如果是相对路径，添加服务器基础URL
                    if (!photoUrl.startsWith("/")) {
                        photoUrl = "/" + photoUrl;
                    }
                    currentPhotoUrl = STUDENT_SERVER_URL + photoUrl;
                } else {
                    // 已经是完整URL
                    currentPhotoUrl = photoUrl;
                }
                qDebug() << "照片完整URL:" << currentPhotoUrl;
                
                // 直接显示成功图标
                QPixmap successIcon(":/images/success.png");
                if (!successIcon.isNull()) {
                    displayPhoto(successIcon);
                }
                
                // **更新状态为成功**
                updatePhotoStatus("照片上传成功！");
                QMessageBox::information(this, "成功", "照片上传成功");
                
                // 刷新数据以显示更新后的状态
                QTimer::singleShot(1000, this, &StudentWidget::loadClassInfo);
            } else {
                updatePhotoStatus("照片URL为空");
                QMessageBox::warning(this, "错误", "服务器返回成功，但照片URL为空");
            }
        } else {
            // **更新状态为上传失败**
            QString finalMessage = message.isEmpty() ? "未知错误" : message;
            updatePhotoStatus("照片上传失败: " + finalMessage);
            QMessageBox::warning(this, "错误", "照片上传失败: " + finalMessage);
        }
    } else {
        // **更新状态为网络错误**
        updatePhotoStatus("网络错误: " + reply->errorString());
        QMessageBox::warning(this, "网络错误", "照片上传失败: " + reply->errorString());
    }
    
    // 清理reply对象
    reply->deleteLater();
}

void StudentWidget::onSeatInfoReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        ui->seatInfoLabel->setText("座位信息: 获取失败 - " + reply->errorString());
        ui->examRoomInfoLabel->setText("考场: 获取失败");
        qDebug() << "座位信息请求失败:" << reply->errorString() << "URL:" << reply->url().toString();
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    qDebug() << "座位信息响应:" << QString(responseData);
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("data") && !jsonObject["data"].isNull() && jsonObject["data"].isObject()) {
            QJsonObject data = jsonObject["data"].toObject();
            qDebug() << "座位数据解析:" 
                     << "包含examRoom:" << data.contains("examRoom") 
                     << "包含seat:" << data.contains("seat");
            
            if (data.contains("examRoom") && data["examRoom"].isObject() && 
                data.contains("seat") && data["seat"].isObject()) {
                
                QJsonObject examRoomObj = data["examRoom"].toObject();
                QJsonObject seatObj = data["seat"].toObject();
                
                // 打印详细的座位和考场信息
                qDebug() << "考场信息:" << QJsonDocument(examRoomObj).toJson();
                qDebug() << "座位信息:" << QJsonDocument(seatObj).toJson();
                
                QString examRoomName = examRoomObj["name"].toString();
                QString examRoomLocation = examRoomObj["location"].toString();
                
                // 尝试多种方式获取座位号
                int seatNumber = 0;
                if (seatObj.contains("seatNumber")) {
                    seatNumber = seatObj["seatNumber"].toInt();
                } else if (seatObj.contains("seatId")) {
                    seatNumber = seatObj["seatId"].toInt();
                }
                
                QString seatInfo = QString("座位号: %1").arg(seatNumber);
                
                // 如果有行列信息，添加到显示中
                if (seatObj.contains("row") && seatObj.contains("column")) {
                    int row = seatObj["row"].toInt();
                    int column = seatObj["column"].toInt();
                    seatInfo += QString(" (行:%1 列:%2)").arg(row).arg(column);
                }
                
                ui->examRoomInfoLabel->setText(QString("考场: %1 (%2)").arg(examRoomName).arg(examRoomLocation));
                ui->seatInfoLabel->setText(seatInfo);
                qDebug() << "座位信息显示成功:" << examRoomName << seatInfo;
            } else {
                ui->examRoomInfoLabel->setText("考场: 数据格式错误");
                ui->seatInfoLabel->setText("座位号: 数据格式错误");
                qDebug() << "座位数据格式错误，无法正确解析examRoom或seat对象";
            }
        } else {
            ui->examRoomInfoLabel->setText("考场: 未分配");
            ui->seatInfoLabel->setText("座位号: 未分配");
            qDebug() << "未找到座位数据或数据为空";
            
            // 尝试重新加载座位信息 - 在无数据的情况下尝试一次
            if (!hasRetryLoadSeat) {
                hasRetryLoadSeat = true;
                QTimer::singleShot(1000, this, &StudentWidget::loadSeatInfo);
                qDebug() << "尝试重新加载座位信息...";
            }
        }
    } else {
        QString message = "获取失败";
        if (jsonObject.contains("message")) {
            message = jsonObject["message"].toString();
        }
        ui->examRoomInfoLabel->setText("考场: " + message);
        ui->seatInfoLabel->setText("座位号: " + message);
        qDebug() << "座位请求状态不成功:" << message;
    }
    
    reply->deleteLater();
}

void StudentWidget::onExamInfoReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        ui->examInfoLabel->setText("考试信息: 获取失败 - " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    qDebug() << "考试信息响应:" << jsonResponse.toJson();
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("data") && !jsonObject["data"].isNull() && jsonObject["data"].isObject()) {
            QJsonObject examData = jsonObject["data"].toObject();
            
            QString examName = examData["name"].toString();
            QString examDate = examData["examDate"].toString();
            QString examTime = examData["examTime"].toString();
            QString examLocation = examData["location"].toString();
            
            // 格式化信息
            QStringList infoLines;
            if (!examName.isEmpty()) infoLines << "考试: " + examName;
            if (!examDate.isEmpty()) {
                // 尝试格式化日期
                QDateTime dateTime = QDateTime::fromString(examDate, Qt::ISODate);
                if (dateTime.isValid()) {
                    infoLines << "日期: " + dateTime.toString("yyyy-MM-dd");
                } else {
                    infoLines << "日期: " + examDate;
                }
            }
            if (!examTime.isEmpty()) infoLines << "时间: " + examTime;
            if (!examLocation.isEmpty()) infoLines << "地点: " + examLocation;
            
            if (infoLines.isEmpty()) {
                ui->examInfoLabel->setText("考试信息: 信息不完整");
            } else {
                ui->examInfoLabel->setText(infoLines.join("\n"));
            }
        } else {
            ui->examInfoLabel->setText("考试信息: 未安排");
        }
    } else {
        QString message = "获取失败";
        if (jsonObject.contains("message")) {
            message = jsonObject["message"].toString();
        }
        ui->examInfoLabel->setText("考试信息: " + message);
    }
    
    reply->deleteLater();
}

void StudentWidget::updatePhotoStatus(const QString& status)
{
    if (!ui->photoStatusLabel) {
        qDebug() << "警告: photoStatusLabel不存在";
        return;
    }
    
    // 更新UI显示照片状态
    ui->photoStatusLabel->setText(status);
    
    // 根据状态修改标签样式
    QString style;
    
    if (status.contains("成功")) {
        style = "QLabel { color: green; font-weight: bold; }";
    } else if (status.contains("失败") || status.contains("错误")) {
        style = "QLabel { color: red; font-weight: bold; }";
    } else if (status.contains("请求")) {
        style = "QLabel { color: orange; font-weight: bold; }";
        // 启用上传照片按钮
        ui->uploadPhotoButton->setEnabled(true);
        ui->uploadPhotoButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
    } else if (status.contains("上传")) {
        style = "QLabel { color: blue; font-weight: bold; }";
    } else {
        style = "QLabel { color: black; }";
    }
    
    ui->photoStatusLabel->setStyleSheet(style);
    
    // 记录到控制台
    qDebug() << "照片状态更新为:" << status;
}

void StudentWidget::displayPhoto(const QPixmap& photo)
{
    // 调整照片大小以适应标签
    QPixmap scaledPhoto = photo.scaled(ui->photoPreviewLabel->size(), 
                                     Qt::KeepAspectRatio, 
                                     Qt::SmoothTransformation);
    
    // 设置预览
    ui->photoPreviewLabel->setPixmap(scaledPhoto);
    ui->photoPreviewLabel->setScaledContents(true);
}

// 设置通知系统
void StudentWidget::setupNotificationSystem()
{
    // 每分钟检查一次通知
    notificationTimer->start(60000);
    
    // 立即检查一次通知
    checkNotifications();
}

// 检查通知
void StudentWidget::checkNotifications()
{
    if (userToken.isEmpty() || studentId.isEmpty()) {
        return;
    }
    
    // 准备通知请求
    QString url = STUDENT_SERVER_URL + "/notifications";
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 发送GET请求
    QNetworkReply *reply = networkManager->get(request);
    
    // 处理响应
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "获取通知失败: " << reply->errorString();
            return;
        }
        
        // 解析响应数据
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        QJsonObject response = doc.object();
        
        qDebug() << "通知响应数据:" << QString(responseData);
        
        // **修正：检查响应状态，兼容 status: 200 和 status: "success"**
        bool success = false;
        if (response.contains("status")) {
            QJsonValue statusVal = response["status"];
            if (statusVal.isString()) {
                success = (statusVal.toString() == "success");
            } else if (statusVal.isDouble()) { // JSON numbers are often doubles
                success = (statusVal.toInt() == 200);
            }
        } else if (response.contains("success")) {
            success = response["success"].toBool();
        }
        
        if (success) {
            QJsonArray notifications;
            
            // 尝试不同的响应格式
            if (response.contains("notifications") && response["notifications"].isArray()) {
                notifications = response["notifications"].toArray();
            } else if (response.contains("data") && response["data"].isArray()) {
                notifications = response["data"].toArray();
            }
            
            qDebug() << "收到通知数量:" << notifications.size();
            
            // 更新通知UI
            updateNotificationUI(notifications);
            
            // 处理通知 (包括弹出提示框)
            processNotifications(notifications);
        } else {
            QString errorMsg = response.contains("message") ? response["message"].toString() : "未知错误或状态不成功";
            qDebug() << "获取通知失败(逻辑判断): " << errorMsg;
            // 可选：如果希望明确告知用户，可以取消下面的注释
            // QMessageBox::warning(this, "通知错误", "获取通知失败: " + errorMsg);
        }
    });
}

void StudentWidget::processNotifications(const QJsonArray &notifications)
{
    // **移除冗余的按钮更新逻辑** 
    // int unreadCount = 0; // 计数移到 updateNotificationUI 中完成

    // 遍历所有通知
    for (const QJsonValue &val : notifications) {
        QJsonObject notification = val.toObject();
        
        // 检查通知是否已读
        if (!notification["read"].toBool()) {
           // unreadCount++; // 不在此处计数
            
            // 根据通知类型处理通知
            QString type = notification["type"].toString();
            
            if (type == "PHOTO_REQUEST" || type == "photo_request") { // 兼容大小写
                // 处理照片请求通知
                onPhotoRequestNotificationReceived(notification);
            }
            // 可以根据需要添加其他类型的通知处理
        }
    }
    
    // **移除冗余的按钮更新**
    // if (unreadCount > 0) { ... } else { ... }
}

void StudentWidget::onPhotoRequestNotificationReceived(const QJsonObject &notification)
{
    // 当收到照片请求通知时，显示消息框提醒用户
    QString title = notification["title"].toString();
    QString content = notification["content"].toString();
    
    // 使用消息框显示通知
    QMessageBox::information(this, "照片请求", 
                            QString("%1\n\n%2").arg(title, content));
    
    // 可以在这里添加其他逻辑，例如自动打开上传照片的界面
    // 例如: onUploadPhotoButtonClicked();
}

void StudentWidget::updateUnreadNotificationsCount()
{
    // 此方法可能已经在updateNotificationUI中被处理，
    // 或者可以在这里添加额外的逻辑来计算未读通知
    
    // 如果需要立即检查通知数量，可以调用检查通知的方法
    checkNotifications();
}

void StudentWidget::loadUserInfo()
{
    // 如果已经有用户信息，直接显示
    if (!userName.isEmpty() && !studentId.isEmpty()) {
        ui->nameLabel->setText("姓名: " + userName);
        ui->studentIdLabel->setText("学号: " + studentId);
    } else {
        qDebug() << "用户信息未设置，无法加载";
    }
}

void StudentWidget::markNotificationAsRead(qint64 notificationId)
{
    if (userToken.isEmpty()) {
        qDebug() << "用户未登录，无法标记通知为已读";
        return;
    }
    
    // 准备标记通知请求
    QString url = STUDENT_SERVER_URL + "/notifications/read";
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    // 准备请求数据
    QJsonObject requestData;
    requestData["id"] = notificationId;
    
    // 发送POST请求（服务器需要POST方法）
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());
    
    // 处理响应
    connect(reply, &QNetworkReply::finished, this, [=]() {
        reply->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "标记通知为已读失败: " << reply->errorString();
            QMessageBox::warning(this, "错误", "标记通知为已读失败: " + reply->errorString());
            return;
        }
        
        // 解析响应数据
        QByteArray responseData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        QJsonObject response = doc.object();
        
        bool success = false;
        if (response.contains("status") && response["status"].toString() == "success") {
            success = true;
        } else if (response.contains("success")) {
            success = response["success"].toBool();
        }
        
        if (success) {
            qDebug() << "通知已标记为已读";
            // 刷新通知列表
            checkNotifications();
        } else {
            QString errorMsg = response["message"].toString();
            if (errorMsg.isEmpty()) {
                errorMsg = "未知错误";
            }
            qDebug() << "标记通知为已读失败: " << errorMsg;
            QMessageBox::warning(this, "错误", "标记通知为已读失败: " + errorMsg);
        }
    });
}

// 确保照片URL是完整的
QString StudentWidget::ensureFullPhotoUrl(const QString &photoUrl) {
    if (photoUrl.isEmpty()) {
        return "";
    }
    
    // 如果已经是完整URL，直接返回
    if (photoUrl.startsWith("http://") || photoUrl.startsWith("https://")) {
        return photoUrl;
    }
    
    // 确保URL以斜杠开头
    QString normalizedUrl = photoUrl;
    if (!normalizedUrl.startsWith("/")) {
        normalizedUrl = "/" + normalizedUrl;
    }
    
    // 服务器地址
    QString fullUrl = STUDENT_SERVER_URL + normalizedUrl;
    qDebug() << "原始照片URL:" << photoUrl << "，完整URL:" << fullUrl;
    return fullUrl;
}

void StudentWidget::updateNotificationUI(const QJsonArray &notifications)
{
    // 清除现有通知
    QLayout *layout = ui->notificationsLayout;
    while (layout->count() > 0) {
        QLayoutItem *item = layout->takeAt(0);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    // 如果没有通知，显示"暂无通知"消息
    if (notifications.isEmpty()) {
        QLabel *noNotificationsLabel = new QLabel("暂无通知", ui->notificationsScrollAreaContents);
        noNotificationsLabel->setAlignment(Qt::AlignCenter);
        noNotificationsLabel->setStyleSheet("color: gray; padding: 20px;");
        layout->addWidget(noNotificationsLabel);
        
        ui->notificationButton->setText("通知");
        ui->notificationButton->setStyleSheet("");
        
        ui->notificationsFrame->setVisible(false);
        return;
    }
    
    // 添加新通知
    int unreadCount = 0;
    
    for (const QJsonValue &val : notifications) {
        QJsonObject notification = val.toObject();
        bool isRead = notification["read"].toBool();
        
        if (!isRead) {
            unreadCount++;
        }
        
        // 创建通知框架
        QFrame *notificationFrame = new QFrame(ui->notificationsScrollAreaContents);
        notificationFrame->setFrameShape(QFrame::StyledPanel);
        notificationFrame->setStyleSheet(QString(
            "background-color: %1;"
            "border-radius: 5px;"
            "margin: 5px;"
            "padding: 8px;"
            "border: 1px solid %2;"
        ).arg(isRead ? "white" : "#E3F2FD", isRead ? "#DDDDDD" : "#2196F3"));
        
        QVBoxLayout *vLayout = new QVBoxLayout(notificationFrame);
        vLayout->setSpacing(8);
        vLayout->setContentsMargins(10, 10, 10, 10);
        
        // **修正：尝试从不同字段获取标题和内容**
        QString title = notification.contains("title") ? notification["title"].toString() : "通知"; // 默认标题
        QString content = notification.contains("message") ? notification["message"].toString() 
                          : (notification.contains("content") ? notification["content"].toString() : ""); // 优先用 message

        // 通知标题
        QLabel *titleLabel = new QLabel(title, notificationFrame);
        titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        vLayout->addWidget(titleLabel);
        
        // 通知内容
        QLabel *contentLabel = new QLabel(content, notificationFrame);
        contentLabel->setWordWrap(true);
        contentLabel->setStyleSheet("color: #333333; margin-top: 5px;");
        vLayout->addWidget(contentLabel);
        
        // 通知时间
        QDateTime timestamp = QDateTime::fromString(notification["createdAt"].toString(), Qt::ISODate);
        QLabel *timeLabel = new QLabel(timestamp.toString("yyyy-MM-dd hh:mm"), notificationFrame);
        timeLabel->setStyleSheet("color: gray; font-size: 10px; margin-top: 5px;");
        vLayout->addWidget(timeLabel);
        
        // 已读/未读状态操作按钮
        QHBoxLayout *statusLayout = new QHBoxLayout();
        statusLayout->addStretch();
        
        // 已读/标记为已读按钮
        QPushButton *markAsReadButton = new QPushButton(isRead ? "已读" : "标记为已读", notificationFrame);
        markAsReadButton->setStyleSheet(QString(
            "QPushButton {"
            "    background-color: %1;"
            "    color: %2;"
            "    border-radius: 3px;"
            "    padding: 5px 10px;"
            "    font-weight: bold;"
            "    border: none;"
            "}"
            "QPushButton:hover {"
            "    background-color: %3;"
            "}"
        ).arg(
            isRead ? "#EEEEEE" : "#2196F3",
            isRead ? "#999999" : "white",
            isRead ? "#DDDDDD" : "#1976D2"
        ));
        
        markAsReadButton->setCursor(Qt::PointingHandCursor);
        markAsReadButton->setMinimumWidth(100);
        
        // 如果通知未读，添加点击事件
        if (!isRead) {
            int notificationId = notification["id"].toInt();
            connect(markAsReadButton, &QPushButton::clicked, this, [=]() {
                markAsReadButton->setText("更新中...");
                markAsReadButton->setEnabled(false);
                
                // 标记通知为已读
                markNotificationAsRead(notificationId);
            });
        }
        
        statusLayout->addWidget(markAsReadButton);
        vLayout->addLayout(statusLayout);
        
        // 将通知添加到通知列表
        ui->notificationsLayout->addWidget(notificationFrame);
    }
    
    // 显示通知面板
    ui->notificationsFrame->setVisible(true);
    
    // 更新通知按钮显示未读数量
    if (unreadCount > 0) {
        ui->notificationButton->setText(QString("通知 (%1)").arg(unreadCount));
        ui->notificationButton->setStyleSheet("background-color: #FF5722; color: white;");
    } else {
        ui->notificationButton->setText("通知");
        ui->notificationButton->setStyleSheet("");
    }
} 
