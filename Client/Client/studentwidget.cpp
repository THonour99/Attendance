#include "studentwidget.h"
#include "ui_studentwidget.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QUrl>
#include <QStandardItemModel>
#include <QDateTime>

// 服务器地址
const QString STUDENT_SERVER_URL = "http://localhost:8080";

StudentWidget::StudentWidget(const QJsonObject &userInfo, const QString &token, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StudentWidget),
    userInfo(userInfo),
    token(token),
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
            onClassInfoReceived(reply);
        }
    });
    
    // 连接刷新按钮
    connect(ui->refreshButton, &QPushButton::clicked, this, &StudentWidget::onRefreshButtonClicked);
    
    // 加载初始数据
    loadAttendanceRecords();
    loadClassInfo();
}

StudentWidget::~StudentWidget()
{
    delete ui;
}

void StudentWidget::setupUI()
{
    // 设置学生信息
    if (userInfo.contains("username")) {
        ui->nameLabel->setText("姓名: " + userInfo["username"].toString());
    }
    
    // 对于学生用户，服务器可能没有提供学号信息，目前客户端显示为空
    // 默认显示用户名作为学号
    if (userInfo.contains("studentId")) {
        ui->studentIdLabel->setText("学号: " + userInfo["studentId"].toString());
    } else if (userInfo.contains("username")) {
        ui->studentIdLabel->setText("学号: " + userInfo["username"].toString());
    }
    
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
}

void StudentWidget::loadAttendanceRecords()
{
    // 准备考勤记录请求
    QUrl url(STUDENT_SERVER_URL + "/attendance/records");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 发送请求
    networkManager->get(request);
}

void StudentWidget::loadClassInfo()
{
    // 使用学生ID获取班级信息
    QString studentId;
    if (userInfo.contains("id")) {
        studentId = userInfo["id"].toString();
    } else {
        QMessageBox::warning(this, "错误", "无法获取学生ID");
        return;
    }
    
    // 准备班级信息请求
    QUrl url(STUDENT_SERVER_URL + "/student/classes");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
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

void StudentWidget::onClassInfoReceived(QNetworkReply *reply)
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
} 