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

// 服务器地址
const QString STUDENT_SERVER_URL = "http://localhost:8080";

StudentWidget::StudentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StudentWidget),
    networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setupUI();
    
    // 连接网络响应信号
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        QString endpoint = reply->request().url().path();
        if (endpoint.contains("/attendance/records")) {
            onAttendanceDataReceived(reply);
        } else if (endpoint.contains("/student/classes")) {
            onClassDataReceived(reply);
        } else if (endpoint.contains("/student/photo/upload")) {
            onPhotoUploadResponse(reply);
        } else if (endpoint.contains("/student/seat")) {
            onSeatInfoReceived(reply);
        } else if (endpoint.contains("/student/examroom/seat")) {
            onSeatInfoReceived(reply); // 也处理旧端点的响应
        } else if (endpoint.contains("/student/exam-info")) {
            onExamInfoReceived(reply);
        }
    });
    
    // 连接按钮信号
    connect(ui->refreshButton, &QPushButton::clicked, this, &StudentWidget::onRefreshButtonClicked);
    connect(ui->uploadPhotoButton, &QPushButton::clicked, this, &StudentWidget::onUploadPhotoButtonClicked);
}

StudentWidget::~StudentWidget()
{
    delete ui;
}

void StudentWidget::setUserInfo(const QString& name, const QString& id, const QString& token)
{
    this->userName = name;
    this->studentId = id;
    this->userToken = token;
    
    // 设置学生信息标签
    ui->nameLabel->setText("姓名: " + name);
    ui->studentIdLabel->setText("学号: " + id);
    
    // 加载初始数据
    loadAttendanceRecords();
    loadClassInfo();
    loadSeatInfo();
    loadExamInfo();
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
}

void StudentWidget::loadAttendanceRecords()
{
    if (userToken.isEmpty() || studentId.isEmpty()) {
        return;
    }
    
    // 准备考勤记录请求
    QUrl url(STUDENT_SERVER_URL + "/attendance/records");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 发送请求
    networkManager->get(request);
}

void StudentWidget::loadClassInfo()
{
    if (userToken.isEmpty() || studentId.isEmpty()) {
        return;
    }
    
    // 准备班级信息请求
    QUrl url(STUDENT_SERVER_URL + "/student/classes");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 发送请求
    networkManager->get(request);
}

void StudentWidget::loadSeatInfo()
{
    if (userToken.isEmpty() || studentId.isEmpty()) {
        return;
    }
    
    qDebug() << "正在请求学生座位信息...";
    
    // 修改为使用正确的座位信息API端点
    QUrl url(STUDENT_SERVER_URL + "/student/seat");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 发送请求
    QNetworkReply* reply = networkManager->get(request);
    
    // 添加调试信息
    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "座位信息请求失败:" << reply->errorString() << "URL:" << url.toString();
        } else {
            qDebug() << "座位信息请求成功，等待响应处理...";
        }
    });
}

void StudentWidget::loadExamInfo()
{
    if (userToken.isEmpty() || studentId.isEmpty()) {
        return;
    }
    
    // 准备考场信息请求
    QUrl url(STUDENT_SERVER_URL + "/student/exam-info");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 发送请求
    networkManager->get(request);
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

void StudentWidget::onUploadPhotoButtonClicked()
{
    // 检查用户是否已登录
    if (userToken.isEmpty() || studentId.isEmpty()) {
        QMessageBox::warning(this, "错误", "请先登录后再上传照片");
        return;
    }

    // 打开文件选择对话框
    QString fileName = QFileDialog::getOpenFileName(this,
        "选择照片", "", "图片文件 (*.png *.jpg *.jpeg)");

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开所选照片文件");
        return;
    }

    QByteArray imageData = file.readAll();
    file.close();

    // 检查文件大小是否过大
    if (imageData.size() > 5 * 1024 * 1024) { // 限制为5MB
        QMessageBox::warning(this, "错误", "照片文件过大，请选择小于5MB的文件");
        return;
    }

    // 显示照片预览
    QPixmap photo;
    if (photo.loadFromData(imageData)) {
        displayPhoto(photo);
    } else {
        QMessageBox::warning(this, "错误", "无法解析所选文件为图片");
        return;
    }

    // 准备网络请求
    QUrl url(STUDENT_SERVER_URL + "/student/photo/upload");
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Bearer " + userToken.toUtf8());
    
    // 将图片数据编码为Base64
    QString base64Image = QString::fromLatin1(imageData.toBase64());
    
    // 创建JSON对象
    QJsonObject jsonObject;
    jsonObject["photo"] = base64Image;
    
    QJsonDocument jsonDoc(jsonObject);
    
    // 显示上传进度
    updatePhotoStatus("正在上传照片...");
    
    // 发送网络请求
    QNetworkReply* reply = networkManager->post(request, jsonDoc.toJson());
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject["status"].toString() == "success") {
                updatePhotoStatus("照片已上传");
                QMessageBox::information(this, "成功", "照片上传成功");
            } else {
                QString errorMessage = "上传失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                updatePhotoStatus("上传失败");
                QMessageBox::warning(this, "错误", errorMessage);
            }
        } else {
            updatePhotoStatus("上传失败");
            QMessageBox::warning(this, "网络错误", "上传照片时发生错误: " + reply->errorString());
        }
        
        reply->deleteLater();
    });
}

void StudentWidget::onPhotoUploadResponse(QNetworkReply *reply)
{
    // 处理上传结果
    if (reply->error() != QNetworkReply::NoError) {
        updatePhotoStatus("上传失败");
        QMessageBox::warning(this, "网络错误", "照片上传失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        updatePhotoStatus("已上传");
        QMessageBox::information(this, "成功", "照片上传成功");
    } else {
        updatePhotoStatus("上传失败");
        QString errorMessage = "照片上传失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
    }
    
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
    ui->photoStatusLabel->setText("照片状态: " + status);
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