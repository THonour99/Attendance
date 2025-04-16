#include "teacherwidget.h"
#include "ui_teacherwidget.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QUrl>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QDebug>

// 服务器地址
const QString TEACHER_SERVER_URL = "http://localhost:8080";

TeacherWidget::TeacherWidget(const QJsonObject &userInfo, const QString &token, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeacherWidget),
    userInfo(userInfo),
    token(token),
    networkManager(new QNetworkAccessManager(this)),
    classesModel(new QStandardItemModel(this)),
    studentsModel(new QStandardItemModel(this)),
    examRoomsModel(new QStandardItemModel(this)),
    attendanceModel(new QStandardItemModel(this))
{
    ui->setupUi(this);
    setupUI();
    
    // 连接网络响应信号 - 修改处理逻辑，区分HTTP方法
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        QString endpoint = reply->request().url().path();
        QNetworkAccessManager::Operation operation = reply->operation();
        
        // 只对GET请求使用全局处理
        if (operation == QNetworkAccessManager::GetOperation) {
            if (endpoint.contains("/teacher/classes") && !endpoint.contains("students")) {
                onClassesDataReceived(reply);
            } else if (endpoint.contains("/teacher/classes") && endpoint.contains("students")) {
                onStudentsDataReceived(reply);
            } else if (endpoint.contains("/teacher/examrooms") && !endpoint.contains("attendance")) {
                onExamRoomsDataReceived(reply);
            } else if (endpoint.contains("/teacher/examrooms") && endpoint.contains("attendance")) {
                onExamAttendanceDataReceived(reply);
            }
        }
        // 其他操作（POST/PUT/DELETE）由各自的专门回调处理
    });
    
    // 连接班级管理相关信号
    connect(ui->classesListView, &QListView::clicked, this, &TeacherWidget::onClassSelected);
    connect(ui->addClassButton, &QPushButton::clicked, this, &TeacherWidget::onAddClassClicked);
    connect(ui->addStudentButton, &QPushButton::clicked, this, &TeacherWidget::onAddStudentToClassClicked);
    connect(ui->removeStudentButton, &QPushButton::clicked, this, &TeacherWidget::onRemoveStudentFromClassClicked);
    
    // 连接考场管理相关信号
    connect(ui->examRoomsListView, &QListView::clicked, this, &TeacherWidget::onExamRoomSelected);
    connect(ui->addExamRoomButton, &QPushButton::clicked, this, &TeacherWidget::onAddExamRoomClicked);
    connect(ui->exportAttendanceButton, &QPushButton::clicked, this, &TeacherWidget::onExportAttendanceClicked);
    
    // 连接刷新按钮
    connect(ui->refreshButton, &QPushButton::clicked, this, &TeacherWidget::onRefreshButtonClicked);
    
    // 加载初始数据
    loadClassesData();
    loadExamRoomsData();
}

TeacherWidget::~TeacherWidget()
{
    delete ui;
}

void TeacherWidget::setupUI()
{
    // 设置教师信息
    if (userInfo.contains("username")) {
        ui->nameLabel->setText("姓名: " + userInfo["username"].toString());
    }
    
    // 设置班级列表模型
    classesModel->setHorizontalHeaderLabels(QStringList() << "班级名称");
    ui->classesListView->setModel(classesModel);
    ui->classesListView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->classesListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置学生表格模型
    studentsModel->setHorizontalHeaderLabels(QStringList() << "姓名" << "学号" << "班级");
    ui->studentsTableView->setModel(studentsModel);
    ui->studentsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 设置考场列表模型
    examRoomsModel->setHorizontalHeaderLabels(QStringList() << "考场名称");
    ui->examRoomsListView->setModel(examRoomsModel);
    ui->examRoomsListView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->examRoomsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置考勤表格模型
    attendanceModel->setHorizontalHeaderLabels(QStringList() << "姓名" << "学号" << "状态" << "签到时间");
    ui->attendanceTableView->setModel(attendanceModel);
    ui->attendanceTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 确保连接信号槽
    connect(ui->classesListView, &QListView::clicked, this, &TeacherWidget::onClassSelected);
    connect(ui->examRoomsListView, &QListView::clicked, this, &TeacherWidget::onExamRoomSelected);
}

void TeacherWidget::loadClassesData()
{
    // 准备班级数据请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/classes");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 发送请求
    networkManager->get(request);
}

void TeacherWidget::loadExamRoomsData()
{
    // 准备考场数据请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/examrooms");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 发送请求
    networkManager->get(request);
}

void TeacherWidget::loadStudentsInClass(const QString &classId)
{
    if (classId.isEmpty()) return;
    
    // 准备班级学生请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/classes/" + classId + "/students");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 保存当前班级ID
    currentClassId = classId;
    
    // 发送请求
    networkManager->get(request);
}

void TeacherWidget::loadExamAttendance(const QString &examRoomId)
{
    if (examRoomId.isEmpty()) return;
    
    // 准备考场考勤请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/examrooms/" + examRoomId + "/attendance");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 保存当前考场ID
    currentExamRoomId = examRoomId;
    
    // 发送请求
    networkManager->get(request);
}

void TeacherWidget::onClassesDataReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "网络错误", "获取班级数据失败: " + reply->errorString());
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
            QJsonArray classesData = jsonObject["data"].toArray();
            
            // 更新班级列表
            classesModel->removeRows(0, classesModel->rowCount());
            
            for (int i = 0; i < classesData.size(); ++i) {
                QJsonObject classInfo = classesData[i].toObject();
                QStandardItem *item = new QStandardItem(classInfo["className"].toString());
                item->setData(classInfo["id"].toString(), Qt::UserRole); // 存储班级ID
                classesModel->appendRow(item);
            }
            
            // 自动选择第一个班级(如果有)
            if (classesModel->rowCount() > 0) {
                QModelIndex firstIndex = classesModel->index(0, 0);
                ui->classesListView->setCurrentIndex(firstIndex);
                onClassSelected(firstIndex);
            }
        }
    } else {
        QString errorMessage = "获取班级数据失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
    }
    
    reply->deleteLater();
}

void TeacherWidget::onStudentsDataReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "网络错误", "获取学生数据失败: " + reply->errorString());
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
            QJsonArray studentsData = jsonObject["data"].toArray();
            
            // 更新学生表格
            studentsModel->removeRows(0, studentsModel->rowCount());
            
            for (int i = 0; i < studentsData.size(); ++i) {
                QJsonObject studentInfo = studentsData[i].toObject();
                
                QList<QStandardItem*> rowItems;
                rowItems << new QStandardItem(studentInfo["name"].toString());
                rowItems << new QStandardItem(studentInfo["studentId"].toString());
                rowItems << new QStandardItem(studentInfo["className"].toString());
                
                // 存储学生ID用于后续操作
                rowItems[0]->setData(studentInfo["id"].toString(), Qt::UserRole);
                
                studentsModel->appendRow(rowItems);
            }
        }
    } else {
        QString errorMessage = "获取学生数据失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
    }
    
    reply->deleteLater();
}

void TeacherWidget::onExamRoomsDataReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "网络错误", "获取考场数据失败: " + reply->errorString());
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
            QJsonArray examRoomsData = jsonObject["data"].toArray();
            
            // 更新考场列表
            examRoomsModel->removeRows(0, examRoomsModel->rowCount());
            
            for (int i = 0; i < examRoomsData.size(); ++i) {
                QJsonObject examRoomInfo = examRoomsData[i].toObject();
                QStandardItem *item = new QStandardItem(examRoomInfo["name"].toString());
                item->setData(examRoomInfo["id"].toString(), Qt::UserRole); // 存储考场ID
                examRoomsModel->appendRow(item);
            }
            
            // 自动选择第一个考场(如果有)
            if (examRoomsModel->rowCount() > 0) {
                QModelIndex firstIndex = examRoomsModel->index(0, 0);
                ui->examRoomsListView->setCurrentIndex(firstIndex);
                onExamRoomSelected(firstIndex);
            }
        }
    } else {
        QString errorMessage = "获取考场数据失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
    }
    
    reply->deleteLater();
}

void TeacherWidget::onExamAttendanceDataReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "获取考勤数据失败: " << reply->errorString();
        qDebug() << "请求URL: " << reply->request().url().toString();
        QMessageBox::warning(this, "网络错误", "获取考勤数据失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    qDebug() << "考勤数据响应: " << QString(responseData);
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("data") && jsonObject["data"].isArray()) {
            QJsonArray attendanceData = jsonObject["data"].toArray();
            
            qDebug() << "获取到" << attendanceData.size() << "条考勤记录";
            
            // 更新考勤表格
            attendanceModel->removeRows(0, attendanceModel->rowCount());
            
            for (int i = 0; i < attendanceData.size(); ++i) {
                QJsonObject record = attendanceData[i].toObject();
                
                QList<QStandardItem*> rowItems;
                rowItems << new QStandardItem(record["studentName"].toString());
                rowItems << new QStandardItem(record["studentId"].toString());
                
                // 设置状态
                QString status = record["status"].toString();
                QStandardItem *statusItem = new QStandardItem(status);
                if (status == "已到") {
                    statusItem->setForeground(QBrush(Qt::green));
                } else if (status == "未到") {
                    statusItem->setForeground(QBrush(Qt::red));
                } else if (status == "迟到") {
                    statusItem->setForeground(QBrush(Qt::yellow));
                }
                rowItems << statusItem;
                
                // 签到时间
                if (record.contains("checkInTime") && !record["checkInTime"].isNull()) {
                    QString checkInTime = record["checkInTime"].toString();
                    QDateTime time = QDateTime::fromString(checkInTime, Qt::ISODate);
                    rowItems << new QStandardItem(time.isValid() ? time.toString("HH:mm:ss") : checkInTime);
                } else {
                    rowItems << new QStandardItem("-");
                }
                
                attendanceModel->appendRow(rowItems);
            }
        } else {
            qDebug() << "考勤数据格式错误: data字段不是数组或不存在";
        }
    } else {
        QString errorMessage = "获取考勤数据失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        qDebug() << "获取考勤数据失败: " << errorMessage;
        QMessageBox::warning(this, "错误", errorMessage);
    }
    
    reply->deleteLater();
}

void TeacherWidget::onClassSelected(const QModelIndex &index)
{
    // 获取选中班级的ID
    QString classId = classesModel->data(index, Qt::UserRole).toString();
    if (!classId.isEmpty()) {
        qDebug() << "选中的班级ID:" << classId;
        loadStudentsInClass(classId);
    } else {
        // 尝试使用索引获取
        classId = QString::number(index.row() + 1); // 假设ID从1开始
        qDebug() << "使用索引作为班级ID:" << classId;
        loadStudentsInClass(classId);
    }
}

void TeacherWidget::onExamRoomSelected(const QModelIndex &index)
{
    // 获取选中考场的ID
    QString examRoomId = examRoomsModel->data(index, Qt::UserRole).toString();
    if (!examRoomId.isEmpty()) {
        qDebug() << "选中考场ID:" << examRoomId << ", 正在加载考勤信息...";
        loadExamAttendance(examRoomId);
    } else {
        qDebug() << "考场ID为空，尝试使用索引作为ID";
        // 尝试使用索引作为ID
        examRoomId = QString::number(index.row() + 1);
        qDebug() << "使用索引生成的考场ID:" << examRoomId;
        loadExamAttendance(examRoomId);
    }
}

void TeacherWidget::onAddClassClicked()
{
    bool ok;
    QString className = QInputDialog::getText(this, "添加班级", "请输入班级名称:", QLineEdit::Normal, "", &ok);
    
    if (ok && !className.isEmpty()) {
        // 准备添加班级请求
        QUrl url(TEACHER_SERVER_URL + "/teacher/classes");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 构建添加班级数据
        QJsonObject classData;
        classData["className"] = className;
        QJsonDocument doc(classData);
        
        // 发送请求
        QNetworkReply *reply = networkManager->post(request, doc.toJson());
        
        // 处理响应
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "添加班级失败: " + reply->errorString());
                reply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", "班级添加成功");
                loadClassesData(); // 刷新班级列表
            } else {
                QString errorMessage = "添加班级失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            reply->deleteLater();
        });
    }
}

void TeacherWidget::onAddStudentToClassClicked()
{
    if (currentClassId.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择一个班级");
        return;
    }
    
    bool ok;
    QString studentId = QInputDialog::getText(this, "添加学生", "请输入学生学号:", QLineEdit::Normal, "", &ok);
    
    if (ok && !studentId.isEmpty()) {
        // 准备添加学生到班级请求
        QUrl url(TEACHER_SERVER_URL + "/teacher/classes/" + currentClassId + "/students");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 构建添加学生数据
        QJsonObject studentData;
        studentData["studentId"] = studentId;
        QJsonDocument doc(studentData);
        
        // 发送请求
        QNetworkReply *reply = networkManager->post(request, doc.toJson());
        
        // 处理响应
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "添加学生失败: " + reply->errorString());
                reply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", "学生添加成功");
                loadStudentsInClass(currentClassId); // 刷新学生列表
            } else {
                QString errorMessage = "添加学生失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            reply->deleteLater();
        });
    }
}

void TeacherWidget::onRemoveStudentFromClassClicked()
{
    // 获取选中的学生
    QModelIndex index = ui->studentsTableView->currentIndex();
    if (!index.isValid() || currentClassId.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要移除的学生");
        return;
    }
    
    // 获取学生ID
    QString studentId = studentsModel->data(studentsModel->index(index.row(), 0), Qt::UserRole).toString();
    
    // 确认删除
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认", "确定要从班级中移除该学生吗?",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // 准备移除学生请求
        QUrl url(TEACHER_SERVER_URL + "/teacher/classes/" + currentClassId + "/students/" + studentId);
        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 发送请求
        QNetworkReply *deleteReply = networkManager->deleteResource(request);
        
        // 处理响应
        connect(deleteReply, &QNetworkReply::finished, this, [this, deleteReply]() {
            if (deleteReply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "移除学生失败: " + deleteReply->errorString());
                deleteReply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = deleteReply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", "学生已从班级移除");
                loadStudentsInClass(currentClassId); // 刷新学生列表
            } else {
                QString errorMessage = "移除学生失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            deleteReply->deleteLater();
        });
    }
}

void TeacherWidget::onAddExamRoomClicked()
{
    bool ok;
    QString examRoomName = QInputDialog::getText(this, "添加考场", "请输入考场名称:", QLineEdit::Normal, "", &ok);
    
    if (ok && !examRoomName.isEmpty()) {
        // 准备添加考场请求
        QUrl url(TEACHER_SERVER_URL + "/teacher/examrooms");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 构建添加考场数据
        QJsonObject examRoomData;
        examRoomData["name"] = examRoomName;
        examRoomData["date"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        QJsonDocument doc(examRoomData);
        
        // 发送请求
        QNetworkReply *reply = networkManager->post(request, doc.toJson());
        
        // 处理响应
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "添加考场失败: " + reply->errorString());
                reply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", "考场添加成功");
                loadExamRoomsData(); // 刷新考场列表
            } else {
                QString errorMessage = "添加考场失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            reply->deleteLater();
        });
    }
}

void TeacherWidget::onExportAttendanceClicked()
{
    if (currentExamRoomId.isEmpty() || attendanceModel->rowCount() == 0) {
        QMessageBox::warning(this, "警告", "没有可导出的考勤数据");
        return;
    }
    
    // 获取保存文件路径
    QString fileName = QFileDialog::getSaveFileName(this, "导出考勤数据", "", "CSV文件 (*.csv)");
    if (fileName.isEmpty()) return;
    
    // 打开文件
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法创建文件: " + fileName);
        return;
    }
    
    QTextStream out(&file);
    
    // 写入表头
    out << "姓名,学号,状态,签到时间\n";
    
    // 写入数据
    for (int row = 0; row < attendanceModel->rowCount(); ++row) {
        QString name = attendanceModel->item(row, 0)->text();
        QString id = attendanceModel->item(row, 1)->text();
        QString status = attendanceModel->item(row, 2)->text();
        QString time = attendanceModel->item(row, 3)->text();
        
        out << QString("%1,%2,%3,%4\n").arg(name, id, status, time);
    }
    
    file.close();
    QMessageBox::information(this, "成功", "考勤数据已导出到: " + fileName);
}

void TeacherWidget::onRefreshButtonClicked()
{
    // 刷新所有数据
    loadClassesData();
    loadExamRoomsData();
    
    // 如果有选中的班级或考场，也刷新其相关数据
    if (!currentClassId.isEmpty()) {
        loadStudentsInClass(currentClassId);
    }
    
    if (!currentExamRoomId.isEmpty()) {
        loadExamAttendance(currentExamRoomId);
    }
} 