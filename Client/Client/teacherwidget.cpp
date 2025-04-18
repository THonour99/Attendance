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
#include <QListWidget>
#include <QCheckBox>
#include <QAxObject>
#include <QDir>
#include <QTimer>

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
    attendanceModel(new QStandardItemModel(this)),
    linkedClassesModel(new QStandardItemModel(this)),
    seatsModel(new QStandardItemModel(this))
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
            } else if (endpoint.contains("/teacher/examrooms") && !endpoint.contains("attendance") && !endpoint.contains("seats") && !endpoint.contains("classes")) {
                onExamRoomsDataReceived(reply);
            } else if (endpoint.contains("/teacher/examrooms") && endpoint.contains("attendance")) {
                onExamAttendanceDataReceived(reply);
            } else if (endpoint.contains("/teacher/student") && endpoint.contains("photo")) {
                onStudentPhotoReceived(reply);
            }
        }
        // 其他操作（POST/PUT/DELETE）由各自的专门回调处理
    });
    
    // 连接班级管理相关信号
    connect(ui->classesListView, &QListView::clicked, this, &TeacherWidget::onClassSelected);
    connect(ui->addClassButton, &QPushButton::clicked, this, &TeacherWidget::onAddClassClicked);
    connect(ui->addStudentButton, &QPushButton::clicked, this, &TeacherWidget::onAddStudentToClassClicked);
    connect(ui->removeStudentButton, &QPushButton::clicked, this, &TeacherWidget::onRemoveStudentFromClassClicked);
    connect(ui->viewStudentPhotoButton, &QPushButton::clicked, this, &TeacherWidget::onViewStudentPhotoClicked);
    connect(ui->requestPhotosButton, &QPushButton::clicked, this, &TeacherWidget::onRequestStudentPhotosClicked);
    connect(ui->viewAllPhotosButton, &QPushButton::clicked, this, &TeacherWidget::onViewStudentPhotosClicked);
    
    // 连接考场管理相关信号
    connect(ui->examRoomsListView, &QListView::clicked, this, &TeacherWidget::onExamRoomSelected);
    connect(ui->addExamRoomButton, &QPushButton::clicked, this, &TeacherWidget::onAddExamRoomClicked);
    connect(ui->exportAttendanceButton, &QPushButton::clicked, this, &TeacherWidget::onExportAttendanceClicked);
    connect(ui->manageSeatsButton, &QPushButton::clicked, this, &TeacherWidget::onManageSeatsClicked);
    
    // 连接座位安排相关信号
    connect(ui->linkClassesButton, &QPushButton::clicked, this, &TeacherWidget::onLinkClassButtonClicked);
    connect(ui->generateSeatsButton, &QPushButton::clicked, this, &TeacherWidget::onGenerateSeatsClicked);
    connect(ui->arrangementTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TeacherWidget::onSeatArrangementTypeChanged);
    connect(ui->exportSeatsButton, &QPushButton::clicked, this, &TeacherWidget::onExportSeatsClicked);
    
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
    
    // 设置座位表格模型
    seatsModel->setHorizontalHeaderLabels(QStringList() << "座位号" << "学号" << "学生姓名" << "班级" << "出勤状态");
    ui->seatsTableView->setModel(seatsModel);
    ui->seatsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 应用表格样式和对齐方式
    QString tableStyle = 
        "QTableView {"
        "  gridline-color: #d0d0d0;"  // 网格线颜色
        "  background-color: white;"  // 背景色
        "  alternate-background-color: #f7f7f7;"  // 交替行颜色
        "}"
        "QHeaderView::section {"
        "  background-color: #f0f0f0;"  // 表头背景色
        "  color: black;"  // 表头文字颜色
        "  padding: 5px;"  // 表头内边距
        "  border: 1px solid #c0c0c0;"  // 表头边框
        "  font-weight: bold;"  // 表头字体加粗
        "}";
    
    // 应用样式到所有表格
    ui->studentsTableView->setStyleSheet(tableStyle);
    ui->attendanceTableView->setStyleSheet(tableStyle);
    ui->seatsTableView->setStyleSheet(tableStyle);
    
    // 禁用行头（第一列的序号列）
    ui->seatsTableView->verticalHeader()->setVisible(false);
    
    // 启用交替行颜色
    ui->studentsTableView->setAlternatingRowColors(true);
    ui->attendanceTableView->setAlternatingRowColors(true);
    ui->seatsTableView->setAlternatingRowColors(true);
    
    // 设置选择模式
    ui->studentsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->attendanceTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->seatsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    
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
    
    // 调试输出获取到的班级数据
    qDebug() << "班级数据原始响应: " << QString(responseData);
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("data") && jsonObject["data"].isArray()) {
            QJsonArray classesData = jsonObject["data"].toArray();
            
            qDebug() << "获取到" << classesData.size() << "个班级";
            
            // 更新班级列表
            classesModel->removeRows(0, classesModel->rowCount());
            
            for (int i = 0; i < classesData.size(); ++i) {
                QJsonObject classInfo = classesData[i].toObject();
                
                // 获取班级信息
                QString className = classInfo["className"].toString();
                int classId = classInfo["id"].toInt();
                
                // 输出调试信息
                qDebug() << "加载班级:" << className << ", ID=" << classId;
                
                // 创建班级项
                QStandardItem *item = new QStandardItem(className);
                
                // 确保班级ID正确设置
                item->setData(QString::number(classId), Qt::UserRole);
                
                // 输出调试信息
                qDebug() << "设置班级ID=" << item->data(Qt::UserRole).toString();
                
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
    
    // 清空现有学生数据
    studentsModel->removeRows(0, studentsModel->rowCount());
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("data") && jsonObject["data"].isArray()) {
            QJsonArray students = jsonObject["data"].toArray();
            
            for (int i = 0; i < students.size(); ++i) {
                QJsonObject student = students[i].toObject();
                
                QList<QStandardItem*> row;
                
                // 创建并设置每个单元格
                QStandardItem *nameItem = new QStandardItem(student["name"].toString());
                nameItem->setTextAlignment(Qt::AlignCenter);
                nameItem->setData(student["id"].toString(), Qt::UserRole);
                
                QStandardItem *idItem = new QStandardItem(student["studentId"].toString());
                idItem->setTextAlignment(Qt::AlignCenter);
                
                QStandardItem *classItem = new QStandardItem(student["className"].toString());
                classItem->setTextAlignment(Qt::AlignCenter);
                
                // 添加行数据
                row << nameItem << idItem << classItem;
                studentsModel->appendRow(row);
            }
            
            // 设置表头对齐方式
            for (int i = 0; i < studentsModel->columnCount(); ++i) {
                studentsModel->setHeaderData(i, Qt::Horizontal, 
                                          studentsModel->headerData(i, Qt::Horizontal), 
                                          Qt::DisplayRole);
                studentsModel->setHeaderData(i, Qt::Horizontal, 
                                          Qt::AlignCenter, 
                                          Qt::TextAlignmentRole);
            }
            
            // 更新表格显示
            ui->studentsTableView->resizeColumnsToContents();
            ui->studentsTableView->update();
        } else {
            QMessageBox::information(this, "提示", "该班级没有学生");
        }
    } else {
        QString errorMessage = "获取学生列表失败";
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
        QMessageBox::warning(this, "网络错误", "获取考勤数据失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    // 清空现有考勤数据
    attendanceModel->removeRows(0, attendanceModel->rowCount());
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        if (jsonObject.contains("data") && jsonObject["data"].isArray()) {
            QJsonArray attendances = jsonObject["data"].toArray();
            
            for (int i = 0; i < attendances.size(); ++i) {
                QJsonObject attendanceRecord = attendances[i].toObject();
                
                // 创建并配置单元格
                QStandardItem *nameItem = new QStandardItem(attendanceRecord["studentName"].toString());
                nameItem->setTextAlignment(Qt::AlignCenter);
                
                QStandardItem *idItem = new QStandardItem(attendanceRecord["studentId"].toString());
                idItem->setTextAlignment(Qt::AlignCenter);
                
                QStandardItem *statusItem = new QStandardItem(attendanceRecord["status"].toString());
                statusItem->setTextAlignment(Qt::AlignCenter);
                
                // 处理时间
                QString timestamp = attendanceRecord["timestamp"].toString();
                QDateTime dateTime = QDateTime::fromString(timestamp, Qt::ISODate);
                QString formattedTime = dateTime.isValid() ? 
                    dateTime.toString("yyyy-MM-dd HH:mm:ss") : timestamp;
                
                QStandardItem *timeItem = new QStandardItem(formattedTime);
                timeItem->setTextAlignment(Qt::AlignCenter);
                
                // 根据考勤状态设置颜色
                QString status = attendanceRecord["status"].toString();
                if (status == "出勤") {
                    statusItem->setForeground(QBrush(QColor("#008000"))); // 绿色
                } else if (status == "缺勤") {
                    statusItem->setForeground(QBrush(QColor("#ff0000"))); // 红色
                } else if (status == "迟到") {
                    statusItem->setForeground(QBrush(QColor("#ffa500"))); // 橙色
                }
                
                // 添加行
                QList<QStandardItem*> row;
                row << nameItem << idItem << statusItem << timeItem;
                attendanceModel->appendRow(row);
            }
            
            // 设置表头对齐方式
            for (int i = 0; i < attendanceModel->columnCount(); ++i) {
                attendanceModel->setHeaderData(i, Qt::Horizontal, 
                                            attendanceModel->headerData(i, Qt::Horizontal), 
                                            Qt::DisplayRole);
                attendanceModel->setHeaderData(i, Qt::Horizontal, 
                                            Qt::AlignCenter, 
                                            Qt::TextAlignmentRole);
            }
            
            // 更新表格显示
            ui->attendanceTableView->resizeColumnsToContents();
            ui->attendanceTableView->update();
        } else {
            QMessageBox::information(this, "提示", "该考场没有考勤记录");
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
        
        // 保存当前考场ID并更新显示
        currentExamRoomId = examRoomId;
        QString examRoomName = examRoomsModel->data(index).toString();
        ui->currentExamRoomLabel->setText("当前考场: " + examRoomName);
        
        // 加载关联班级和座位信息
        loadLinkedClasses(examRoomId);
        loadExamSeats(examRoomId);
    } else {
        qDebug() << "考场ID为空，尝试使用索引作为ID";
        // 尝试使用索引作为ID
        examRoomId = QString::number(index.row() + 1);
        qDebug() << "使用索引生成的考场ID:" << examRoomId;
        loadExamAttendance(examRoomId);
        
        // 保存当前考场ID并更新显示
        currentExamRoomId = examRoomId;
        QString examRoomName = examRoomsModel->data(index).toString();
        ui->currentExamRoomLabel->setText("当前考场: " + examRoomName);
        
        // 加载关联班级和座位信息
        loadLinkedClasses(examRoomId);
        loadExamSeats(examRoomId);
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
    
    // 打印更多调试信息
    int row = index.row();
    int column = index.column();
    qDebug() << "选中学生行列: 行=" << row << ", 列=" << column;
    qDebug() << "当前选中班级ID:" << currentClassId;
    qDebug() << "学生表格模型行数:" << studentsModel->rowCount();
    
    // 确保行号有效
    if (row < 0 || row >= studentsModel->rowCount()) {
        QMessageBox::warning(this, "错误", "选择的行号无效");
        return;
    }
    
    // 获取学生数据
    QString studentName;
    QString studentIdStr;
    QVariant studentIdData;
    
    // 尝试多种方式获取学生ID
    if (studentsModel->item(row, 0) != nullptr) {
        studentName = studentsModel->item(row, 0)->text();
        studentIdData = studentsModel->item(row, 0)->data(Qt::UserRole);
    } else {
        QMessageBox::warning(this, "错误", "无法访问选中行的数据");
        return;
    }
    
    // 如果从UserRole获取不到ID，尝试从学号列获取
    if (!studentIdData.isValid() || studentIdData.toString().isEmpty()) {
        if (studentsModel->item(row, 1) != nullptr) {
            studentIdStr = studentsModel->item(row, 1)->text(); // 使用学号作为ID
            qDebug() << "使用学号作为ID:" << studentIdStr;
        } else {
            QMessageBox::warning(this, "错误", "无法获取学生学号");
            return;
        }
    } else {
        studentIdStr = studentIdData.toString();
        qDebug() << "从UserRole获取到学生ID:" << studentIdStr;
    }
    
    // 如果ID仍然为空，则无法继续
    if (studentIdStr.isEmpty()) {
        QMessageBox::warning(this, "错误", "无法获取学生ID，无法执行移除操作");
        return;
    }
    
    // 确认删除
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认", 
        QString("确定要将学生 %1 从班级中移除吗?").arg(studentName),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // 准备移除学生请求
        QUrl url(TEACHER_SERVER_URL + "/teacher/classes/" + currentClassId + "/students/" + studentIdStr);
        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        
        // 输出请求URL
        qDebug() << "移除学生请求URL:" << url.toString();
        
        // 发送请求
        QNetworkReply *deleteReply = networkManager->deleteResource(request);
        
        // 处理响应
        connect(deleteReply, &QNetworkReply::finished, this, [this, deleteReply, studentName]() {
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
                QMessageBox::information(this, "成功", QString("学生 %1 已从班级移除").arg(studentName));
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
    QString fileName = QFileDialog::getSaveFileName(this, "导出考勤数据", "", "Excel文件 (*.xlsx);;CSV文件 (*.csv)");
    if (fileName.isEmpty()) return;
    
    // 根据文件扩展名决定导出格式
    if (fileName.endsWith(".xlsx", Qt::CaseInsensitive)) {
        // 导出为Excel
        if (exportToExcel(fileName, attendanceModel, "考勤数据")) {
            QMessageBox::information(this, "成功", "考勤数据已导出到Excel: " + fileName);
        } else {
            QMessageBox::critical(this, "错误", "导出Excel失败");
        }
    } else {
        // 默认导出为CSV
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
        QMessageBox::information(this, "成功", "考勤数据已导出到CSV: " + fileName);
    }
}

void TeacherWidget::onManageSeatsClicked()
{
    // 检查是否选择了考场
    if (currentExamRoomId.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择一个考场");
        return;
    }
    
    // 切换到座位管理标签页
    ui->tabWidget->setCurrentWidget(ui->seatTab);
}

void TeacherWidget::onLinkClassButtonClicked()
{
    if (currentExamRoomId.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择考场");
        return;
    }
    
    // 创建班级选择对话框
    QDialog dialog(this);
    dialog.setWindowTitle("关联班级");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    // 创建班级列表
    QListWidget *classesListWidget = new QListWidget(&dialog);
    layout->addWidget(new QLabel("选择要关联的班级:"));
    layout->addWidget(classesListWidget);
    
    // 检查班级模型是否为空
    if (classesModel->rowCount() == 0) {
        QMessageBox::warning(this, "警告", "没有可用的班级");
        return;
    }
    
    // 加载全部班级并检查数据
    for (int i = 0; i < classesModel->rowCount(); ++i) {
        QString className = classesModel->item(i)->text();
        QVariant classIdVariant = classesModel->item(i)->data(Qt::UserRole);
        QString classId;
        
        if (classIdVariant.isValid()) {
            classId = classIdVariant.toString();
            qDebug() << "添加班级到列表: " << className << ", ID=" << classId;
        } else {
            classId = QString::number(i + 1); // 如果没有ID，使用索引+1作为ID
            qDebug() << "班级ID不存在，使用索引作为ID: " << classId;
        }
        
        QListWidgetItem *item = new QListWidgetItem(className);
        item->setData(Qt::UserRole, classId);
        item->setCheckState(Qt::Unchecked);
        classesListWidget->addItem(item);
    }
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    
    // 收集选中的班级并添加调试信息
    QJsonArray selectedClasses;
    qDebug() << "开始收集选中的班级:";
    qDebug() << "班级列表项数量:" << classesListWidget->count();
    
    for (int i = 0; i < classesListWidget->count(); ++i) {
        QListWidgetItem *item = classesListWidget->item(i);
        qDebug() << "检查项 #" << i << ": " << item->text() << ", 勾选状态:" << (item->checkState() == Qt::Checked ? "已选中" : "未选中");
        
        if (item->checkState() == Qt::Checked) {
            QString classId = item->data(Qt::UserRole).toString();
            qDebug() << "班级已选中，ID=" << classId;
            
            if (!classId.isEmpty()) {
                selectedClasses.append(classId);
                qDebug() << "添加班级ID到已选列表:" << classId;
            } else {
                qDebug() << "班级ID为空，不添加到列表";
            }
        }
    }
    
    qDebug() << "总共选中班级数:" << selectedClasses.size();
    
    if (selectedClasses.isEmpty()) {
        QMessageBox::information(this, "提示", "未选择任何班级");
        return;
    }
    
    // 准备网络请求数据
    QJsonObject requestData;
    requestData["classIds"] = selectedClasses;
    
    // 输出调试信息
    qDebug() << "关联班级请求数据:" << QJsonDocument(requestData).toJson();
    
    // 发送网络请求关联班级
    QUrl url(TEACHER_SERVER_URL + "/teacher/examrooms/" + currentExamRoomId + "/classes");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::critical(this, "错误", "网络请求失败: " + reply->errorString());
            return;
        }
        
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();
        
        if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
            QMessageBox::information(this, "成功", "班级关联成功");
            loadLinkedClasses(currentExamRoomId);
        } else {
            QString errorMessage = "班级关联失败";
            if (jsonObject.contains("message")) {
                errorMessage = jsonObject["message"].toString();
            }
            QMessageBox::warning(this, "错误", errorMessage);
        }
    });
}

void TeacherWidget::loadLinkedClasses(const QString &examRoomId)
{
    QNetworkRequest request(QUrl(TEACHER_SERVER_URL + "/teacher/examrooms/" + examRoomId + "/classes"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onClassesLinkedReceived(reply);
    });
}

void TeacherWidget::onClassesLinkedReceived(QNetworkReply *reply)
{
    reply->deleteLater();
    
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(this, "错误", "获取关联班级失败: " + reply->errorString());
        return;
    }
    
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
        // 清空现有数据
        ui->linkedClassesListWidget->clear();
        
        // 添加关联班级数据
        QJsonArray classes = jsonObject["data"].toArray();
        for (int i = 0; i < classes.size(); ++i) {
            QJsonObject classInfo = classes[i].toObject();
            
            QListWidgetItem *item = new QListWidgetItem(classInfo["className"].toString());
            item->setData(Qt::UserRole, classInfo["id"].toString());
            item->setCheckState(Qt::Unchecked);
            
            ui->linkedClassesListWidget->addItem(item);
        }
    } else {
        QString errorMessage = "获取关联班级失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
    }
}

void TeacherWidget::loadExamSeats(const QString &examRoomId)
{
    // 验证参数
    if (examRoomId.isEmpty()) {
        qDebug() << "错误: loadExamSeats接收到空的考场ID";
        return;
    }

    qDebug() << "开始加载考场" << examRoomId << "的座位数据";
    
    // 准备获取考场座位的请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/examrooms/" + examRoomId + "/seats");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 发送请求
    QNetworkReply *reply = networkManager->get(request);
    
    // 使用专门的处理函数处理回复
    connect(reply, &QNetworkReply::finished, this, [this, reply, examRoomId]() {
        reply->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::critical(this, "错误", "获取座位信息失败: " + reply->errorString());
            return;
        }
        
        // 读取响应数据
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();
        
        qDebug() << "接收到考场" << examRoomId << "的座位数据:" << QString(responseData.left(200)) << "...";
        
        // 清空现有座位数据
        seatsModel->removeRows(0, seatsModel->rowCount());
        
        // 设置表头
        QStringList headers;
        headers << "座位号" << "学号" << "学生姓名" << "班级" << "出勤状态";
        seatsModel->setHorizontalHeaderLabels(headers);
        
        if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
            // 解析座位数据
            if (jsonObject.contains("data")) {
                QJsonValue dataValue = jsonObject["data"];
                QJsonArray seats;
                
                if (dataValue.isArray()) {
                    seats = dataValue.toArray();
                    qDebug() << "获取到" << seats.size() << "个座位记录";
                } else {
                    qDebug() << "座位数据不是数组格式";
                    return;
                }
                
                // 处理每个座位数据
                for (int i = 0; i < seats.size(); ++i) {
                    QJsonObject seat = seats[i].toObject();
                    
                    QList<QStandardItem*> row;
                    
                    // 服务器返回的字段说明：
                    // seatId: 座位号
                    // studentId: 学生ID (内部系统ID)
                    // studentNumber: 学号 (显示用学号)
                    // studentName: 学生姓名
                    // className: 班级名称
                    
                    // 获取座位字段，提供默认值以防数据缺失
                    // 确保座位号始终有值
                    QString seatId;
                    if (seat.contains("seatId") && !seat["seatId"].toString().isEmpty()) {
                        seatId = seat["seatId"].toString();
                    } else {
                        seatId = QString::number(i+1);
                    }
                    
                    QString studentNumber = seat.contains("studentNumber") ? seat["studentNumber"].toString() : "";
                    QString originalStudentName = seat.contains("studentName") ? seat["studentName"].toString() : "";
                    QString className = seat.contains("className") ? seat["className"].toString() : "";
                    
                    // 从学生姓名中分离班级信息（如果名字中包含班级信息）
                    QString studentName = originalStudentName;
                    if (studentName.contains("-")) {
                        QStringList parts = studentName.split("-");
                        if (parts.size() >= 2) {
                            // 更新班级和学生姓名
                            if (className.isEmpty()) {
                                className = parts[0].trimmed();
                            }
                            studentName = parts[1].trimmed();
                        }
                    }
                    
                    // 创建并配置标准项目，设置文本对齐方式
                    QStandardItem *seatIdItem = new QStandardItem(seatId);
                    seatIdItem->setTextAlignment(Qt::AlignCenter);
                    
                    QStandardItem *studentNumberItem = new QStandardItem(studentNumber);
                    studentNumberItem->setTextAlignment(Qt::AlignCenter);
                    
                    QStandardItem *studentNameItem = new QStandardItem(studentName);
                    studentNameItem->setTextAlignment(Qt::AlignCenter);
                    
                    QStandardItem *classNameItem = new QStandardItem(className);
                    classNameItem->setTextAlignment(Qt::AlignCenter);
                    
                    // 添加数据到行
                    row << seatIdItem << studentNumberItem << studentNameItem << classNameItem;
                    
                    // 添加出勤状态
                    QStandardItem *statusItem = nullptr;
                    if (seat.contains("attendanceStatus")) {
                        statusItem = new QStandardItem(seat["attendanceStatus"].toString());
                    } else {
                        statusItem = new QStandardItem("未记录");
                    }
                    statusItem->setTextAlignment(Qt::AlignCenter);
                    row << statusItem;
                    
                    seatsModel->appendRow(row);
                }
                
                // 配置表格视图
                ui->seatsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                
                // 设置水平表头对齐方式为居中
                for (int i = 0; i < seatsModel->columnCount(); ++i) {
                    seatsModel->setHeaderData(i, Qt::Horizontal, seatsModel->headerData(i, Qt::Horizontal), Qt::DisplayRole);
                    seatsModel->setHeaderData(i, Qt::Horizontal, Qt::AlignCenter, Qt::TextAlignmentRole);
                }
                
                // 应用自定义样式
                ui->seatsTableView->setStyleSheet(
                    "QTableView {"
                    "  gridline-color: #d0d0d0;"  // 网格线颜色
                    "  background-color: white;"  // 背景色
                    "  alternate-background-color: #f7f7f7;"  // 交替行颜色
                    "}"
                    "QHeaderView::section {"
                    "  background-color: #f0f0f0;"  // 表头背景色
                    "  color: black;"  // 表头文字颜色
                    "  padding: 5px;"  // 表头内边距
                    "  border: 1px solid #c0c0c0;"  // 表头边框
                    "  font-weight: bold;"  // 表头字体加粗
                    "}"
                );
                
                // 禁用行头（第一列的序号列）
                ui->seatsTableView->verticalHeader()->setVisible(false);
                
                // 启用表格交替行颜色
                ui->seatsTableView->setAlternatingRowColors(true);
                
                // 设置表格选择模式
                ui->seatsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
                ui->seatsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
                
                ui->seatsTableView->update();
                qDebug() << "座位表更新完成，共" << seatsModel->rowCount() << "行数据";
            } else {
                qDebug() << "座位数据中没有data字段";
                QMessageBox::information(this, "提示", "没有座位数据");
            }
        } else {
            QString errorMessage = "获取座位数据失败";
            if (jsonObject.contains("message")) {
                errorMessage = jsonObject["message"].toString();
            }
            QMessageBox::warning(this, "错误", errorMessage);
        }
    });
}

void TeacherWidget::onGenerateSeatsClicked()
{
    if (currentExamRoomId.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择考场");
        return;
    }
    
    // 确认是否已关联班级
    if (ui->linkedClassesListWidget->count() == 0) {
        QMessageBox::warning(this, "警告", "请先关联班级到考场");
        return;
    }
    
    QStringList selectedClasses;
    for (int i = 0; i < ui->linkedClassesListWidget->count(); ++i) {
        QListWidgetItem *item = ui->linkedClassesListWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            selectedClasses.append(item->data(Qt::UserRole).toString());
        }
    }
    
    // 如果没有勾选班级，默认使用所有关联的班级
    if (selectedClasses.isEmpty()) {
        for (int i = 0; i < ui->linkedClassesListWidget->count(); ++i) {
            QListWidgetItem *item = ui->linkedClassesListWidget->item(i);
            QString classId = item->data(Qt::UserRole).toString();
            if (!classId.isEmpty()) {
                selectedClasses.append(classId);
                // 自动勾选这些班级
                item->setCheckState(Qt::Checked);
            }
        }
    }
    
    // 显示排座方式选择对话框
    QDialog dialog(this);
    dialog.setWindowTitle("选择排座方式");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    QRadioButton *randomButton = new QRadioButton("完全随机排座", &dialog);
    QRadioButton *classGroupButton = new QRadioButton("按班级分组", &dialog);
    QRadioButton *byGradeButton = new QRadioButton("按成绩排座", &dialog);
    
    randomButton->setChecked(true); // 默认选择随机排座
    
    layout->addWidget(randomButton);
    layout->addWidget(classGroupButton);
    layout->addWidget(byGradeButton);
    
    QLabel *seatsCountLabel = new QLabel("每排座位数：", &dialog);
    QSpinBox *seatsPerRowBox = new QSpinBox(&dialog);
    seatsPerRowBox->setRange(1, 20);
    seatsPerRowBox->setValue(6); // 默认每排6个座位
    
    QHBoxLayout *rowLayout = new QHBoxLayout();
    rowLayout->addWidget(seatsCountLabel);
    rowLayout->addWidget(seatsPerRowBox);
    layout->addLayout(rowLayout);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    
    int seatsPerRow = seatsPerRowBox->value();
    QString arrangementType;
    
    if (randomButton->isChecked()) {
        arrangementType = "random";
    } else if (classGroupButton->isChecked()) {
        arrangementType = "byClass";
    } else if (byGradeButton->isChecked()) {
        arrangementType = "byGrade";
    }
    
    // 准备网络请求数据
    QJsonObject requestData;
    requestData["arrangementType"] = arrangementType;
    requestData["seatsPerRow"] = seatsPerRow;
    
    // 输出调试信息
    qDebug() << "生成座位请求数据:" << QJsonDocument(requestData).toJson();
    
    // 显示进度对话框
    QMessageBox::information(this, "请稍候", "正在生成座位安排...");
    
    // 发送网络请求生成座位
    QUrl url(TEACHER_SERVER_URL + "/teacher/examrooms/" + currentExamRoomId + "/seats");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        // 安全地处理回复对象
        reply->deleteLater();
        
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::critical(this, "错误", "生成座位失败: " + reply->errorString());
            return;
        }
        
        // 读取响应数据
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();
        
        if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
            QMessageBox::information(this, "成功", "座位生成成功");
            
            // 加载座位数据 - 不传递已经读取过的reply对象
            loadExamSeats(currentExamRoomId);
        } else {
            QString errorMessage = "生成座位失败";
            if (jsonObject.contains("message")) {
                errorMessage = jsonObject["message"].toString();
            }
            QMessageBox::warning(this, "错误", errorMessage);
        }
    });
}

void TeacherWidget::onSeatArrangementTypeChanged(int index)
{
    // 根据选择的排座方式更新UI
    if (index == 3) { // 自定义排座
        QMessageBox::information(this, "提示", "自定义排座功能允许您手动编辑座位表");
    }
}

void TeacherWidget::onExportSeatsClicked()
{
    if (seatsModel->rowCount() == 0) {
        QMessageBox::warning(this, "警告", "没有座位数据可导出");
        return;
    }
    
    // 弹出文件选择对话框
    QString fileName = QFileDialog::getSaveFileName(this, "导出座位表", 
                                                  QDir::homePath() + "/座位表.xlsx",
                                                  "Excel 文件 (*.xlsx);;CSV 文件 (*.csv)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    bool success = false;
    
    // 根据文件类型选择导出方式
    if (fileName.endsWith(".xlsx", Qt::CaseInsensitive)) {
        success = exportToExcel(fileName, seatsModel, "座位表");
    } else if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        success = exportToCSV(fileName, seatsModel);
    } else {
        QMessageBox::warning(this, "警告", "不支持的文件格式");
        return;
    }
    
    if (success) {
        QMessageBox::information(this, "成功", "座位表导出成功");
    } else {
        QMessageBox::critical(this, "错误", "座位表导出失败");
    }
}

bool TeacherWidget::exportToCSV(const QString &fileName, QStandardItemModel *model)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件进行写入");
        return false;
    }
    
    QTextStream stream(&file);
    
    // 写入表头
    QStringList headers;
    for (int col = 0; col < model->columnCount(); ++col) {
        headers << model->headerData(col, Qt::Horizontal).toString();
    }
    stream << headers.join(",") << "\n";
    
    // 写入数据
    for (int row = 0; row < model->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < model->columnCount(); ++col) {
            QString data = model->data(model->index(row, col)).toString();
            // 如果数据中包含逗号，则用引号包围
            if (data.contains(",")) {
                data = "\"" + data + "\"";
            }
            rowData << data;
        }
        stream << rowData.join(",") << "\n";
    }
    
    file.close();
    return true;
}

void TeacherWidget::onViewStudentPhotoClicked()
{
    // 检查是否选择了学生
    QModelIndex index = ui->studentsTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择一个学生");
        return;
    }
    
    // 获取学生ID
    currentStudentId = studentsModel->data(studentsModel->index(index.row(), 0), Qt::UserRole).toString();
    QString studentName = studentsModel->data(studentsModel->index(index.row(), 0)).toString();
    
    // 准备请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/student/" + currentStudentId + "/photo");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 显示加载中消息
    QMessageBox::information(this, "加载中", "正在获取学生照片...");
    
    // 发送请求
    networkManager->get(request);
}

void TeacherWidget::onStudentPhotoReceived(QNetworkReply *reply)
{
    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "网络错误", "获取学生照片失败: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toInt() == 200) {
        if (jsonObject.contains("data")) {
            // 获取照片URL
            QString photoUrl = jsonObject["data"].toString();
            
            // 创建对话框显示照片
            QDialog dialog(this);
            dialog.setWindowTitle("学生照片");
            QVBoxLayout *layout = new QVBoxLayout(&dialog);
            
            QLabel *titleLabel = new QLabel("正在加载照片...");
            titleLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(titleLabel);
            
            QLabel *imageLabel = new QLabel();
            imageLabel->setMinimumSize(300, 300);
            imageLabel->setAlignment(Qt::AlignCenter);
            layout->addWidget(imageLabel);
            
            QPushButton *closeButton = new QPushButton("关闭");
            layout->addWidget(closeButton);
            
            connect(closeButton, &QPushButton::clicked, &dialog, &QDialog::accept);
            
            // 发起另一个请求获取实际照片
            QNetworkAccessManager *photoManager = new QNetworkAccessManager(&dialog);
            QNetworkRequest photoRequest(QUrl(TEACHER_SERVER_URL + photoUrl));
            photoRequest.setRawHeader("Authorization", "Bearer " + token.toUtf8());
            
            QNetworkReply *photoReply = photoManager->get(photoRequest);
            connect(photoReply, &QNetworkReply::finished, [=]() {
                if (photoReply->error() == QNetworkReply::NoError) {
                    QByteArray imgData = photoReply->readAll();
                    QPixmap pixmap;
                    if (pixmap.loadFromData(imgData)) {
                        imageLabel->setPixmap(pixmap.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        titleLabel->setText("学生照片");
                    } else {
                        titleLabel->setText("照片格式无效");
                    }
                } else {
                    titleLabel->setText("加载照片失败: " + photoReply->errorString());
                }
                photoReply->deleteLater();
            });
            
            dialog.exec();
        } else {
            QMessageBox::warning(this, "错误", "照片数据不存在");
        }
    } else {
        QString errorMessage = "获取照片失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
    }
    
    reply->deleteLater();
}

void TeacherWidget::onRequestStudentPhotosClicked()
{
    // 获取当前选中的班级
    if (currentClassId.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择一个班级");
        return;
    }
    
    // 确认请求照片
    QMessageBox::StandardButton confirm = QMessageBox::question(
        this, 
        "确认请求", 
        "确定要向该班级的所有学生请求上传照片吗？",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (confirm != QMessageBox::Yes) {
        return;
    }
    
    // 准备请求数据
    QJsonObject requestData;
    requestData["classId"] = currentClassId;
    
    // 创建网络请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/requestPhotos");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 发送请求
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());
    
    // 处理响应
    connect(reply, &QNetworkReply::finished, this, [=]() {
        // 检查网络错误
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "网络错误", "请求失败: " + reply->errorString());
            reply->deleteLater();
            return;
        }
        
        // 解析响应数据
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();
        
        // 检查响应状态
        if (jsonObject["status"].toInt() == 200) {
            QMessageBox::information(this, "成功", "照片请求已发送给所有学生");
        } else {
            QString errorMessage = "请求失败";
            if (jsonObject.contains("message")) {
                errorMessage = jsonObject["message"].toString();
            }
            QMessageBox::warning(this, "错误", errorMessage);
        }
        
        reply->deleteLater();
    });
}

void TeacherWidget::onViewStudentPhotosClicked()
{
    // 检查是否选择了班级
    if (currentClassId.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择一个班级");
        return;
    }
    
    // 准备请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/students/photos?classId=" + currentClassId);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 显示加载提示
    QMessageBox loadingMsg(QMessageBox::Information, "加载中", "正在获取学生照片数据...", QMessageBox::Cancel, this);
    loadingMsg.setStandardButtons(QMessageBox::NoButton);
    QTimer::singleShot(1000, &loadingMsg, &QMessageBox::accept);
    loadingMsg.exec();
    
    // 发送请求
    QNetworkReply *reply = networkManager->get(request);
    
    // 处理响应
    connect(reply, &QNetworkReply::finished, this, [=]() {
        // 检查网络错误
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "网络错误", "获取照片数据失败: " + reply->errorString());
            reply->deleteLater();
            return;
        }
        
        // 解析响应数据
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();
        
        // 检查响应状态
        if (jsonObject["status"].toInt() != 200) {
            QString errorMessage = "获取照片数据失败";
            if (jsonObject.contains("message")) {
                errorMessage = jsonObject["message"].toString();
            }
            QMessageBox::warning(this, "错误", errorMessage);
            reply->deleteLater();
            return;
        }
        
        // 获取学生数据
        QJsonArray studentsData = jsonObject["data"].toArray();
        if (studentsData.isEmpty()) {
            QMessageBox::information(this, "提示", "该班级暂无学生照片信息");
            reply->deleteLater();
            return;
        }
        
        // 创建照片查看对话框
        QDialog dialog(this);
        dialog.setWindowTitle("查看学生照片");
        dialog.setMinimumSize(800, 600);
        
        // 创建布局
        QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
        QScrollArea *scrollArea = new QScrollArea(&dialog);
        scrollArea->setWidgetResizable(true);
        mainLayout->addWidget(scrollArea);
        
        QWidget *scrollContent = new QWidget(scrollArea);
        scrollArea->setWidget(scrollContent);
        
        QGridLayout *gridLayout = new QGridLayout(scrollContent);
        gridLayout->setSpacing(20);
        
        // 设置照片网格
        const int columns = 3;
        int row = 0, col = 0;
        
        for (int i = 0; i < studentsData.size(); ++i) {
            QJsonObject student = studentsData[i].toObject();
            
            // 创建学生信息卡片
            QGroupBox *studentCard = new QGroupBox(scrollContent);
            QVBoxLayout *cardLayout = new QVBoxLayout(studentCard);
            
            // 学生基本信息
            QString name = student["name"].toString();
            QString studentId = student["studentId"].toString();
            QString photoStatus = student["photoStatus"].toString();
            QString photoUrl = student["photoUrl"].toString();
            
            // 学生姓名和学号
            QLabel *infoLabel = new QLabel(QString("<b>%1</b><br>学号: %2").arg(name, studentId));
            infoLabel->setAlignment(Qt::AlignCenter);
            cardLayout->addWidget(infoLabel);
            
            // 照片显示区域
            QLabel *photoLabel = new QLabel(studentCard);
            photoLabel->setFixedSize(200, 200);
            photoLabel->setAlignment(Qt::AlignCenter);
            photoLabel->setFrameShape(QFrame::Box);
            
            // 根据照片状态显示不同内容
            if (photoStatus == "uploaded" && !photoUrl.isEmpty()) {
                // 设置加载中提示
                photoLabel->setText("正在加载照片...");
                
                // 加载照片
                QNetworkAccessManager *photoManager = new QNetworkAccessManager(photoLabel);
                QNetworkRequest photoRequest(QUrl(TEACHER_SERVER_URL + photoUrl));
                photoRequest.setRawHeader("Authorization", "Bearer " + token.toUtf8());
                
                QNetworkReply *photoReply = photoManager->get(photoRequest);
                connect(photoReply, &QNetworkReply::finished, [photoReply, photoLabel]() {
                    if (photoReply->error() == QNetworkReply::NoError) {
                        QByteArray imgData = photoReply->readAll();
                        QPixmap pixmap;
                        if (pixmap.loadFromData(imgData)) {
                            photoLabel->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        } else {
                            photoLabel->setText("照片格式无效");
                        }
                    } else {
                        photoLabel->setText("网络错误: " + photoReply->errorString());
                    }
                    photoReply->deleteLater();
                });
            } else if (photoStatus == "requested") {
                photoLabel->setText("照片已请求，等待上传");
            } else {
                photoLabel->setText("暂无照片");
            }
            
            cardLayout->addWidget(photoLabel);
            
            // 添加到网格布局
            gridLayout->addWidget(studentCard, row, col);
            
            // 更新行列位置
            col++;
            if (col >= columns) {
                col = 0;
                row++;
            }
        }
        
        // 添加关闭按钮
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
        connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::accept);
        mainLayout->addWidget(buttonBox);
        
        // 显示对话框
        reply->deleteLater();
        dialog.exec();
    });
}

bool TeacherWidget::exportToExcel(const QString &fileName, QStandardItemModel *model, const QString &sheetName)
{
    // 创建Excel应用对象
    QAxObject excel("Excel.Application");
    if (excel.isNull()) {
        QMessageBox::critical(nullptr, "错误", "未能创建Excel应用，请确保安装了Microsoft Excel");
        return false;
    }
    
    excel.setProperty("Visible", false);
    
    // 创建工作簿
    QAxObject *workbooks = excel.querySubObject("Workbooks");
    workbooks->dynamicCall("Add");
    QAxObject *workbook = excel.querySubObject("ActiveWorkbook");
    
    // 获取工作表
    QAxObject *worksheets = workbook->querySubObject("Worksheets");
    QAxObject *worksheet = worksheets->querySubObject("Item(int)", 1);
    worksheet->setProperty("Name", sheetName);
    
    // 写入表头
    for (int i = 0; i < model->columnCount(); ++i) {
        QAxObject *cell = worksheet->querySubObject("Cells(int,int)", 1, i + 1);
        cell->setProperty("Value", model->headerData(i, Qt::Horizontal).toString());
        
        // 设置表头格式
        QAxObject *font = cell->querySubObject("Font");
        font->setProperty("Bold", true);
    }
    
    // 写入数据
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            QAxObject *cell = worksheet->querySubObject("Cells(int,int)", row + 2, col + 1);
            cell->setProperty("Value", model->data(model->index(row, col)).toString());
        }
    }
    
    // 自动调整列宽
    QAxObject *range = worksheet->querySubObject("UsedRange");
    range->dynamicCall("EntireColumn.AutoFit()");
    
    // 保存并关闭
    workbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(fileName));
    workbook->dynamicCall("Close()");
    excel.dynamicCall("Quit()");
    
    return true;
}

void TeacherWidget::onRefreshButtonClicked()
{
    // 根据当前标签页刷新不同数据
    int currentTab = ui->tabWidget->currentIndex();
    
    // 刷新所有数据
    loadClassesData();
    loadExamRoomsData();
    
    // 如果有选中的班级或考场，也刷新其相关数据
    if (!currentClassId.isEmpty()) {
        loadStudentsInClass(currentClassId);
    }
    
    if (!currentExamRoomId.isEmpty()) {
        loadExamAttendance(currentExamRoomId);
        
        // 如果在座位管理标签页，也刷新座位数据
        if (currentTab == 2) { // 座位管理标签页
            loadLinkedClasses(currentExamRoomId);
            loadExamSeats(currentExamRoomId);
        }
    }
}

void TeacherWidget::onLinkClassesClicked()
{
    // 这个函数可能是当初计划用于链接班级的功能，但后来被onLinkClassButtonClicked取代
    // 为了保持兼容性，我们将调用该函数
    onLinkClassButtonClicked();
} 