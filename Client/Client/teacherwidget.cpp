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
#include <QStandardItemModel>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QGroupBox>
#include <QFormLayout>
#include <QRadioButton>
#include <QSpinBox>
#include <QAxObject>
#include <QSet> // Ensure QSet is included

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
    seatsModel(new QStandardItemModel(this)),
    photoRefreshTimer(new QTimer(this)),
    repliesBeingProcessed() // ** 初始化 QSet **
{
    ui->setupUi(this);
    setupUI();
    
    // 连接网络响应信号 - 修改处理逻辑，区分HTTP方法
    connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        if (!reply) {
            qDebug() << "[Global Handler] Received null reply, ignoring.";
            return;
        }

        // 检查 reply 是否已被标记为正在处理
        if (repliesBeingProcessed.contains(reply)) {
             qDebug() << "[Global Handler] Reply for" << reply->request().url().path() << "is already being processed, ignoring.";
             // 不在此处 deleteLater，由具体处理函数负责
             return;
        }
        
        if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::RemoteHostClosedError) {
            qDebug() << "[Global Handler] Reply aborted or host closed, deleting.";
            reply->deleteLater();
            return;
        }

        QString endpoint = reply->request().url().path();
        QNetworkAccessManager::Operation operation = reply->operation();
        bool handled = false; 

        if (operation == QNetworkAccessManager::GetOperation) {
             // **标记 reply 正在被处理**
             repliesBeingProcessed.insert(reply);
             qDebug() << "[Global Handler] Processing GET reply for:" << endpoint;

            if (endpoint.contains("/teacher/classes") && !endpoint.contains("/students")) {
                onClassesDataReceived(reply);
                handled = true; 
            } else if (endpoint.contains("/teacher/classes") && endpoint.contains("/students")) {
                onStudentsDataReceived(reply);
                handled = true; 
            } else if (endpoint.contains("/teacher/examrooms") && !endpoint.contains("/attendance") && !endpoint.contains("/seats") && !endpoint.contains("/classes")) {
                onExamRoomsDataReceived(reply);
                handled = true; 
            } else if (endpoint.contains("/teacher/examrooms") && endpoint.contains("/attendance")) {
                onExamAttendanceDataReceived(reply);
                handled = true; 
            } else if (endpoint.contains("/teacher/student") && endpoint.contains("/photo")) {
                onStudentPhotoReceived(reply);
                handled = true; 
            } else if (endpoint.contains("/teacher/examrooms") && endpoint.contains("/classes")) {
                 onClassesLinkedReceived(reply); // 处理获取关联班级的响应
                 handled = true;
            } else if (endpoint.contains("/teacher/examrooms") && endpoint.contains("/seats")) {
                // 座位获取通常在此触发，但我们应确保在 onExamRoomSelected 中处理
                // loadExamSeats(currentExamRoomId); // 不在此处直接调用
                 qDebug() << "[Global Handler] GET /seats endpoint detected, assuming handled by specific logic.";
                 handled = true; // 标记为已处理，避免下面删除
            } else if (endpoint.contains("/teacher/students/photos")) {
                 // 处理获取班级所有照片状态的响应
                 onViewStudentPhotosFinished(reply); // 需要一个处理函数
                 handled = true;
            }
             // 如果标记了处理，则从 set 中移除
            if (handled) {
                 repliesBeingProcessed.remove(reply);
            }
        } else { 
            qDebug() << "[Global Handler] Ignoring non-GET operation for endpoint:" << endpoint;
            handled = true; // 假设 POST/PUT/DELETE 由其他 connect 处理
        }

        // 如果 GET 请求未被上述任何 if/else if 处理，则删除
        if (operation == QNetworkAccessManager::GetOperation && !handled && reply) {
            qDebug() << "[Global Handler] Unhandled GET reply for" << endpoint << ", deleting.";
             repliesBeingProcessed.remove(reply); // 确保从 set 中移除
            reply->deleteLater();
        }
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
    
    // 设置照片刷新定时器
    setupClassPhotoRefresh();
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
                
                // --- 修正开始 ---
                QString originalName = student["name"].toString();
                QString studentName = originalName; // 默认使用原始名称
                QString studentClassName = student["className"].toString(); // 从 className 字段获取

                // 如果姓名包含 "-"，尝试分离班级和姓名
                if (originalName.contains("-")) {
                    QStringList parts = originalName.split("-");
                    if (parts.size() >= 2) {
                         // 如果 className 字段为空，则使用分离出的班级名
                         if (studentClassName.isEmpty()) {
                             studentClassName = parts[0].trimmed();
                         }
                         // 使用分离出的学生姓名
                        studentName = parts[1].trimmed(); 
                    }
                }
                
                QStandardItem *nameItem = new QStandardItem(studentName); // 使用处理后的姓名
                nameItem->setTextAlignment(Qt::AlignCenter);
                if (student.contains("id")) {
                    nameItem->setData(student["id"].toVariant(), Qt::UserRole);
                    qDebug() << "[StudentData] Setting UserRole for" << studentName << "to ID:" << student["id"].toVariant();
                } else {
                    qDebug() << "[StudentData] Warning: Student object missing 'id' field for" << studentName;
                }
                
                QStandardItem *idItem = new QStandardItem(student["studentId"].toString()); // 学号列
                idItem->setTextAlignment(Qt::AlignCenter);
                
                QStandardItem *classItem = new QStandardItem(studentClassName); // 使用获取或分离出的班级名
                classItem->setTextAlignment(Qt::AlignCenter);
                
                // 添加所有项到行数据
                row << nameItem << idItem << classItem; 
                // --- 修正结束 ---
                
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
    QDialog dialog(this);
    dialog.setWindowTitle("添加班级");
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    QHBoxLayout* nameLayout = new QHBoxLayout();
    QLabel* nameLabel = new QLabel("班级名称:", &dialog);
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    
    layout->addLayout(nameLayout);
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString className = nameEdit->text().trimmed();
        if (className.isEmpty()) {
            showMessage("班级名称不能为空", true);
            return;
        }
        
        QJsonObject requestData;
        requestData["name"] = className;
        
        QNetworkRequest request(QUrl("http://localhost:3000/api/classes"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        QJsonDocument doc(requestData);
        QByteArray data = doc.toJson();
        
        QNetworkReply* reply = networkManager->post(request, data);
        connect(reply, &QNetworkReply::finished, [this, reply]() {
            QJsonObject response = parseResponse(reply);
            if (!response.isEmpty()) {
                showMessage("班级添加成功");
                loadClassesData();
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
    qDebug() << "[ViewPhoto] Current index valid?" << index.isValid();

    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择一个学生");
        return;
    }

    qDebug() << "[ViewPhoto] Selected row:" << index.row() << "column:" << index.column();

    // **显式获取第0列的索引**
    QModelIndex nameIndex = studentsModel->index(index.row(), 0);
    if (!nameIndex.isValid()) {
         QMessageBox::warning(this, "错误", "无法获取姓名单元格的索引");
         qDebug() << "[ViewPhoto] Failed to get valid index for column 0.";
         return;
    }
    qDebug() << "[ViewPhoto] Index for column 0 (name) valid?" << nameIndex.isValid();


    // 获取学生ID
    QVariant idData = studentsModel->data(nameIndex, Qt::UserRole);
    currentStudentId = idData.toString();
    qDebug() << "[ViewPhoto] Attempting to get student ID from UserRole (column 0). Is data valid?" << idData.isValid() << "Value:" << currentStudentId;


    QString studentName = studentsModel->data(nameIndex).toString();
    qDebug() << "[ViewPhoto] Student Name from model:" << studentName;

    // **添加检查：确保 currentStudentId 不为空**
    if (currentStudentId.isEmpty()) {
        // 添加更详细的错误提示
        QMessageBox::warning(this, "错误", QString("无法获取学生 '%1' 的ID (行: %2)。请确保学生数据已正确加载。").arg(studentName).arg(index.row()));
        qDebug() << "[ViewPhoto] Failed to get Student ID for row:" << index.row() << "Name:" << studentName;
        
        // 尝试打印该行所有列的数据和UserRole以供调试
        for (int col = 0; col < studentsModel->columnCount(); ++col) {
            QModelIndex checkIndex = studentsModel->index(index.row(), col);
            if (checkIndex.isValid()) {
                 qDebug() << "  [Debug] Col:" << col << "Display Data:" << studentsModel->data(checkIndex, Qt::DisplayRole).toString()
                          << "UserRole Data:" << studentsModel->data(checkIndex, Qt::UserRole).toString();
            }
        }
        return;
    }

    qDebug() << "[ViewPhoto] Successfully retrieved Student ID:" << currentStudentId << "Name:" << studentName;

    // 准备请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/student/" + currentStudentId + "/photo");
    qDebug() << "[ViewPhoto] Requesting photo URL:" << url.toString();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 显示加载中消息
    // QMessageBox::information(this, "加载中", "正在获取学生照片..."); // 改为非阻塞
    qDebug() << "[ViewPhoto] Sending request to get student photo URL...";
    
    // 发送请求
    QNetworkReply *reply = networkManager->get(request);
    // 连接处理函数
     connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onStudentPhotoReceived(reply);
    });
}

// 检查服务器返回的照片URL，确保它是完整URL
QString TeacherWidget::ensureFullPhotoUrl(const QString &photoUrl)
{
    if (photoUrl.isEmpty()) {
        qDebug() << "警告: 收到空的照片URL";
        return QString();
    }
    
    // 调试输出原始URL
    qDebug() << "原始照片URL:" << photoUrl;
    
    QString fullUrl;
    
    if (photoUrl.startsWith("http")) {
        // 已经是完整URL
        fullUrl = photoUrl;
    } else if (photoUrl.startsWith("/uploads/")) {
        // 服务器上的uploads目录，需要拼接完整URL
        // 使用正确的服务器地址，确保端口号一致
        // 注意：这里使用硬编码的服务器地址和端口，确保与服务器匹配
        fullUrl = "http://localhost:8080" + photoUrl;
        qDebug() << "处理uploads路径，生成URL:" << fullUrl;
    } else if (photoUrl.startsWith("/")) {
        // 其他相对路径，以/开头
        fullUrl = TEACHER_SERVER_URL + photoUrl;
    } else {
        // 相对路径，不以/开头
        fullUrl = TEACHER_SERVER_URL + "/" + photoUrl;
    }
    
    qDebug() << "构建的完整照片URL:" << fullUrl;
    
    // 额外检查URL是否有效
    if (!QUrl(fullUrl).isValid()) {
        qDebug() << "警告: 生成的URL无效:" << fullUrl;
        return QString();
    }
    
    return fullUrl;
}

void TeacherWidget::onStudentPhotoReceived(QNetworkReply *reply)
{
    // **在处理开始时检查并标记**
    if (!reply || repliesBeingProcessed.contains(reply)) {
         // 如果reply无效或已在处理中，则忽略
         qDebug() << "[onStudentPhotoReceived] Ignoring invalid or already processed reply.";
         if (reply) repliesBeingProcessed.remove(reply); // 确保移除
         // 不在此处调用 deleteLater，因为可能已被删除或将由其他地方删除
         return; 
    }
    // repliesBeingProcessed.insert(reply); // 移到全局处理中标记
    qDebug() << "[onStudentPhotoReceived] Processing reply for:" << reply->request().url().path();

    // 检查网络错误
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(this, "网络错误", "获取学生照片URL失败: " + reply->errorString());
        repliesBeingProcessed.remove(reply); // **处理完毕，移除标记**
        reply->deleteLater();
        return;
    }
    
    // 解析响应数据
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObject = jsonResponse.object();
    
    qDebug() << "[onStudentPhotoReceived] 学生照片URL响应:" << QString(responseData);
    
    // 检查响应状态
    if (jsonObject.contains("status") && jsonObject["status"].toInt() == 200) {
        if (jsonObject.contains("data")) {
            // 获取照片URL
            QString photoUrl = jsonObject["data"].toString();
            
            if (photoUrl.isEmpty()) {
                QMessageBox::warning(this, "错误", "服务器返回的照片URL为空");
                 repliesBeingProcessed.remove(reply); // **处理完毕，移除标记**
                reply->deleteLater();
                return;
            }
            
             // **创建对话框显示照片 (在 lambda 之外创建，确保作用域)**
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

            // 确保photoUrl是完整的URL
            QString fullPhotoUrl = ensureFullPhotoUrl(photoUrl);
            
            if (fullPhotoUrl.isEmpty()) {
                titleLabel->setText("无效的照片URL");
                repliesBeingProcessed.remove(reply); // **处理完毕，移除标记**
                reply->deleteLater(); // 删除第一个请求的 reply
                dialog.exec(); // 显示带有错误消息的对话框
                return;
            }
            
            qDebug() << "[onStudentPhotoReceived] 请求实际照片的完整URL:" << fullPhotoUrl;
            
            // 发起另一个请求获取实际照片
            // **使用 dialog 作为 parent，确保 photoManager 生命周期与对话框一致**
            QNetworkAccessManager *photoManager = new QNetworkAccessManager(&dialog);
            QNetworkRequest photoRequest;
            photoRequest.setUrl(QUrl(fullPhotoUrl));
            photoRequest.setRawHeader("Authorization", "Bearer " + token.toUtf8());
            
            QNetworkReply *photoReply = photoManager->get(photoRequest);
             // **连接第二个请求的 finished 信号**
            connect(photoReply, &QNetworkReply::finished, this, [=]() {
                qDebug() << "[onStudentPhotoReceived lambda] Second request finished. Error:" << photoReply->errorString();
                if (photoReply->error() == QNetworkReply::NoError) {
                    QByteArray imgData = photoReply->readAll();
                    QPixmap pixmap;
                    if (pixmap.loadFromData(imgData)) {
                        imageLabel->setPixmap(pixmap.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                        titleLabel->setText("学生照片");
                    } else {
                        titleLabel->setText("照片格式无效");
                         qDebug() << "[onStudentPhotoReceived lambda] Failed to load pixmap from data.";
                    }
                } else {
                    titleLabel->setText("加载照片失败: " + photoReply->errorString());
                    qDebug() << "[onStudentPhotoReceived lambda] 照片加载错误:" << photoReply->errorString();
                }
                photoReply->deleteLater(); // **删除第二个请求的 reply**
            });
            
            repliesBeingProcessed.remove(reply); // **移除第一个 reply 的标记**
            reply->deleteLater(); // **删除第一个请求的 reply**
            dialog.exec(); // 显示对话框，加载图片

        } else {
            QMessageBox::warning(this, "错误", "照片数据不存在");
            repliesBeingProcessed.remove(reply); // **处理完毕，移除标记**
            reply->deleteLater();
        }
    } else {
        QString errorMessage = "获取照片URL失败";
        if (jsonObject.contains("message")) {
            errorMessage = jsonObject["message"].toString();
        }
        QMessageBox::warning(this, "错误", errorMessage);
        repliesBeingProcessed.remove(reply); // **处理完毕，移除标记**
        reply->deleteLater();
    }
    // **移除函数末尾多余的 deleteLater**
    // reply->deleteLater(); 
}

// **添加 onViewStudentPhotosFinished 处理函数（如果需要）**
void TeacherWidget::onViewStudentPhotosFinished(QNetworkReply *reply) {
     if (!reply) return;
     qDebug() << "[onViewStudentPhotosFinished] Processing reply for photos list.";
     // 这里添加处理 /teacher/students/photos 响应的逻辑
     // ...
      repliesBeingProcessed.remove(reply); // 处理完毕，移除标记
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
    
    qDebug() << "正在请求班级" << currentClassId << "的学生上传照片";
    
    // 准备请求数据
    QJsonObject requestData;
    requestData["classId"] = currentClassId;
    
    // 创建网络请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/requestPhotos");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 打印请求详情
    qDebug() << "请求URL:" << url.toString();
    qDebug() << "请求数据:" << QJsonDocument(requestData).toJson();
    
    // 发送请求
    QNetworkReply *reply = networkManager->post(request, QJsonDocument(requestData).toJson());
    
    // 处理响应
    connect(reply, &QNetworkReply::finished, this, [=]() {
        // 检查网络错误
        if (reply->error() != QNetworkReply::NoError) {
            QString errorString = reply->errorString();
            qDebug() << "照片请求失败:" << errorString;
            QMessageBox::warning(this, "网络错误", "请求失败: " + errorString);
            reply->deleteLater();
            return;
        }
        
        // 解析响应数据
        QByteArray responseData = reply->readAll();
        qDebug() << "照片请求响应:" << QString(responseData);
        
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();
        
        // 检查响应状态
        if (jsonObject["status"].toInt() == 200) {
            QJsonObject data = jsonObject["data"].toObject();
            int totalStudents = data["totalStudents"].toInt();
            int updatedStudents = data["updatedStudents"].toInt();
            
            qDebug() << "照片请求成功 - 总学生:" << totalStudents << "更新学生:" << updatedStudents;
            
            QMessageBox::information(this, "成功", 
                QString("照片请求已发送给班级中的%1名学生。\n%2名学生将收到通知。\n学生登录后将收到提醒。")
                .arg(totalStudents)
                .arg(updatedStudents));
                
            // 立即刷新照片状态
            refreshClassPhotosStatus();
        } else {
            QString errorMessage = "请求失败";
            if (jsonObject.contains("message")) {
                errorMessage = jsonObject["message"].toString();
            }
            qDebug() << "照片请求失败:" << errorMessage;
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
        qDebug() << "获取班级照片响应:" << QString(responseData);
        
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
                
                // 确保photoUrl是完整的URL
                QString fullPhotoUrl = ensureFullPhotoUrl(photoUrl);
                
                if (fullPhotoUrl.isEmpty()) {
                    photoLabel->setText("照片URL无效");
                    continue;
                }
                
                qDebug() << "请求班级照片的完整URL:" << fullPhotoUrl;
                
                // 加载照片
                QNetworkAccessManager *photoManager = new QNetworkAccessManager(photoLabel);
                QNetworkRequest photoRequest;
                photoRequest.setUrl(QUrl(fullPhotoUrl));
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
                        qDebug() << "班级照片加载错误:" << photoReply->errorString();
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

// 设置照片刷新定时器
void TeacherWidget::setupClassPhotoRefresh()
{
    // 连接定时器到刷新方法
    connect(photoRefreshTimer, &QTimer::timeout, this, &TeacherWidget::refreshClassPhotosStatus);
    
    // 禁用自动刷新，改为手动刷新
    // photoRefreshTimer->start(60000);
    
    // 输出日志，表明不再自动刷新照片状态
    qDebug() << "照片状态刷新已设置为手动模式，不会自动弹出照片对话框";
}

// 刷新班级照片状态
void TeacherWidget::refreshClassPhotosStatus()
{
    qDebug() << "Checking class photos status...";
    QString currentClassId = ui->classesListView->currentIndex().data(Qt::UserRole).toString();
    
    if (!currentClassId.isEmpty() && !token.isEmpty()) {
        QUrl url(TEACHER_SERVER_URL + "/teacher/students/photos?classId=" + currentClassId);
        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        QNetworkReply* reply = networkManager->get(request);
        
        connect(reply, &QNetworkReply::finished, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray responseData = reply->readAll();
                QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);
                QJsonObject responseObj = responseDoc.object();
                
                // 检查照片状态变化，如有必要更新UI
                if (responseObj["status"].toInt() == 200 && responseObj.contains("data")) {
                    QJsonArray studentsData = responseObj["data"].toArray();
                    int uploadedCount = 0;
                    int requestedCount = 0;
                    
                    for (const QJsonValue &val : studentsData) {
                        QJsonObject student = val.toObject();
                        QString status = student["photoStatus"].toString();
                        
                        if (status == "uploaded") {
                            uploadedCount++;
                        } else if (status == "requested") {
                            requestedCount++;
                        }
                    }
                    
                    // 更新状态标签 - 使用studentsLabel代替不存在的photoStatusLabel
                    QString originalText = ui->studentsLabel->text();
                    ui->studentsLabel->setText(
                        QString("班级学生 - 照片状态: %1/%2 已上传, %3/%4 等待上传")
                        .arg(uploadedCount)
                        .arg(studentsData.size())
                        .arg(requestedCount)
                        .arg(studentsData.size())
                    );
                }
            }
            
            reply->deleteLater();
        });
    }
}

void TeacherWidget::onLogoutButtonClicked()
{
    // 确认退出
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认", 
        "确定要退出登录吗?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // 发送退出登录信号
        emit this->close();
    }
}

void TeacherWidget::onRefreshClassesButtonClicked()
{
    // 刷新班级数据
    loadClassesData();
    
    // 如果当前有选中的班级，也刷新学生列表
    if (!currentClassId.isEmpty()) {
        loadStudentsInClass(currentClassId);
    }
}

void TeacherWidget::onAddClassButtonClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加班级");
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    QHBoxLayout* nameLayout = new QHBoxLayout();
    QLabel* nameLabel = new QLabel("班级名称:", &dialog);
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    
    layout->addLayout(nameLayout);
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString className = nameEdit->text().trimmed();
        if (className.isEmpty()) {
            showMessage("班级名称不能为空", true);
            return;
        }
        
        QJsonObject requestData;
        requestData["name"] = className;
        
        QNetworkRequest request(QUrl("http://localhost:3000/api/classes"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        QJsonDocument doc(requestData);
        QByteArray data = doc.toJson();
        
        QNetworkReply* reply = networkManager->post(request, data);
        connect(reply, &QNetworkReply::finished, [this, reply]() {
            QJsonObject response = parseResponse(reply);
            if (!response.isEmpty()) {
                showMessage("班级添加成功");
                loadClassesData();
            }
            reply->deleteLater();
        });
    }
}

void TeacherWidget::onEditClassButtonClicked()
{
    // 获取当前选中的班级
    QModelIndex index = ui->classesListView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择一个班级");
        return;
    }
    
    QString classId = classesModel->data(index, Qt::UserRole).toString();
    QString currentClassName = classesModel->data(index).toString();
    
    // 弹出对话框编辑班级名称
    bool ok;
    QString newClassName = QInputDialog::getText(this, "编辑班级", 
        "请输入新的班级名称:", QLineEdit::Normal, currentClassName, &ok);
    
    if (ok && !newClassName.isEmpty() && newClassName != currentClassName) {
        // 准备编辑班级请求
        QUrl url(TEACHER_SERVER_URL + "/teacher/classes/" + classId);
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 构建请求数据
        QJsonObject classData;
        classData["className"] = newClassName;
        QJsonDocument doc(classData);
        
        // 发送请求
        QNetworkReply *reply = networkManager->put(request, doc.toJson());
        
        // 处理响应
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "编辑班级失败: " + reply->errorString());
                reply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", "班级编辑成功");
                loadClassesData(); // 刷新班级列表
            } else {
                QString errorMessage = "编辑班级失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            reply->deleteLater();
        });
    }
}

void TeacherWidget::onDeleteClassButtonClicked()
{
    // 获取当前选中的班级
    QModelIndex index = ui->classesListView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择一个班级");
        return;
    }
    
    QString classId = classesModel->data(index, Qt::UserRole).toString();
    QString className = classesModel->data(index).toString();
    
    // 确认删除
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认", 
        QString("确定要删除班级 %1 吗?").arg(className),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // 准备删除班级请求
        QUrl url(TEACHER_SERVER_URL + "/teacher/classes/" + classId);
        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 发送请求
        QNetworkReply *deleteReply = networkManager->deleteResource(request);
        
        // 处理响应
        connect(deleteReply, &QNetworkReply::finished, this, [this, deleteReply, className]() {
            if (deleteReply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "删除班级失败: " + deleteReply->errorString());
                deleteReply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = deleteReply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", QString("班级 %1 已删除").arg(className));
                loadClassesData(); // 刷新班级列表
                
                // 清空学生列表
                studentsModel->removeRows(0, studentsModel->rowCount());
            } else {
                QString errorMessage = "删除班级失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            deleteReply->deleteLater();
        });
    }
}

void TeacherWidget::onStudentDataReceived(QNetworkReply* reply)
{
    // 这个方法可能是冗余的，功能已由onStudentsDataReceived实现
    // 为了兼容性保留，简单地调用现有方法
    onStudentsDataReceived(reply);
}

void TeacherWidget::onAddStudentButtonClicked()
{
    // 这个方法可能是冗余的，功能已由onAddStudentToClassClicked实现
    // 为了兼容性保留，简单地调用现有方法
    onAddStudentToClassClicked();
}

void TeacherWidget::onEditStudentButtonClicked()
{
    // 获取当前选中的学生
    QModelIndex index = ui->studentsTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择一个学生");
        return;
    }
    
    int row = index.row();
    QString studentId = studentsModel->item(row, 0)->data(Qt::UserRole).toString();
    QString studentName = studentsModel->item(row, 0)->text();
    QString studentNumber = studentsModel->item(row, 1)->text();
    
    // 创建编辑对话框
    QDialog dialog(this);
    dialog.setWindowTitle("编辑学生信息");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    
    // 创建表单
    QFormLayout *formLayout = new QFormLayout();
    QLineEdit *nameEdit = new QLineEdit(studentName, &dialog);
    QLineEdit *idEdit = new QLineEdit(studentNumber, &dialog);
    idEdit->setReadOnly(true); // 学号不允许修改
    
    formLayout->addRow("姓名:", nameEdit);
    formLayout->addRow("学号:", idEdit);
    layout->addLayout(formLayout);
    
    // 添加确定和取消按钮
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttonBox);
    
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    
    // 获取编辑后的值
    QString newName = nameEdit->text();
    
    if (newName.isEmpty()) {
        QMessageBox::warning(this, "警告", "学生姓名不能为空");
        return;
    }
    
    // 准备网络请求
    QUrl url(TEACHER_SERVER_URL + "/teacher/students/" + studentId);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
    
    // 构建请求数据
    QJsonObject studentData;
    studentData["name"] = newName;
    QJsonDocument doc(studentData);
    
    // 发送请求
    QNetworkReply *reply = networkManager->put(request, doc.toJson());
    
    // 处理响应
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            QMessageBox::warning(this, "网络错误", "编辑学生信息失败: " + reply->errorString());
            reply->deleteLater();
            return;
        }
        
        // 解析响应
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonObject jsonObject = jsonResponse.object();
        
        if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
            QMessageBox::information(this, "成功", "学生信息编辑成功");
            
            // 刷新学生列表
            if (!currentClassId.isEmpty()) {
                loadStudentsInClass(currentClassId);
            }
        } else {
            QString errorMessage = "编辑学生信息失败";
            if (jsonObject.contains("message")) {
                errorMessage = jsonObject["message"].toString();
            }
            QMessageBox::warning(this, "错误", errorMessage);
        }
        
        reply->deleteLater();
    });
}

void TeacherWidget::onDeleteStudentButtonClicked()
{
    // 获取当前选中的学生
    QModelIndex index = ui->studentsTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择一个学生");
        return;
    }
    
    int row = index.row();
    QString studentId = studentsModel->item(row, 0)->data(Qt::UserRole).toString();
    QString studentName = studentsModel->item(row, 0)->text();
    
    // 确认删除
    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认", 
        QString("确定要删除学生 %1 吗?").arg(studentName),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // 准备删除学生请求
        QUrl url(TEACHER_SERVER_URL + "/teacher/students/" + studentId);
        QNetworkRequest request(url);
        request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
        
        // 发送请求
        QNetworkReply *deleteReply = networkManager->deleteResource(request);
        
        // 处理响应
        connect(deleteReply, &QNetworkReply::finished, this, [this, deleteReply, studentName]() {
            if (deleteReply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "网络错误", "删除学生失败: " + deleteReply->errorString());
                deleteReply->deleteLater();
                return;
            }
            
            // 解析响应
            QByteArray responseData = deleteReply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObject = jsonResponse.object();
            
            if (jsonObject.contains("status") && jsonObject["status"].toString() == "success") {
                QMessageBox::information(this, "成功", QString("学生 %1 已删除").arg(studentName));
                
                // 刷新学生列表
                if (!currentClassId.isEmpty()) {
                    loadStudentsInClass(currentClassId);
                }
            } else {
                QString errorMessage = "删除学生失败";
                if (jsonObject.contains("message")) {
                    errorMessage = jsonObject["message"].toString();
                }
                QMessageBox::warning(this, "错误", errorMessage);
            }
            
            deleteReply->deleteLater();
        });
    }
}

void TeacherWidget::onExportButtonClicked()
{
    // 导出当前表格数据（根据当前标签页选择不同的导出数据）
    int currentTab = ui->tabWidget->currentIndex();
    
    QStandardItemModel *modelToExport = nullptr;
    QString defaultFileName = "";
    QString sheetName = "";
    
    // 根据当前标签页选择要导出的数据
    if (currentTab == 0) { // 班级管理
        modelToExport = studentsModel;
        defaultFileName = "学生列表.xlsx";
        sheetName = "学生列表";
        
        if (modelToExport->rowCount() == 0) {
            QMessageBox::warning(this, "警告", "没有学生数据可导出");
            return;
        }
    } else if (currentTab == 1) { // 考勤管理
        modelToExport = attendanceModel;
        defaultFileName = "考勤记录.xlsx";
        sheetName = "考勤记录";
        
        if (modelToExport->rowCount() == 0) {
            QMessageBox::warning(this, "警告", "没有考勤数据可导出");
            return;
        }
    } else if (currentTab == 2) { // 座位管理
        modelToExport = seatsModel;
        defaultFileName = "座位安排.xlsx";
        sheetName = "座位安排";
        
        if (modelToExport->rowCount() == 0) {
            QMessageBox::warning(this, "警告", "没有座位数据可导出");
            return;
        }
    } else {
        QMessageBox::warning(this, "警告", "当前页面没有可导出的数据");
        return;
    }
    
    // 获取保存文件路径
    QString fileName = QFileDialog::getSaveFileName(this, "导出数据", 
                                                 defaultFileName,
                                                 "Excel 文件 (*.xlsx);;CSV 文件 (*.csv)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    bool success = false;
    
    // 根据文件类型选择导出方式
    if (fileName.endsWith(".xlsx", Qt::CaseInsensitive)) {
        success = exportToExcel(fileName, modelToExport, sheetName);
    } else if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        success = exportToCSV(fileName, modelToExport);
    } else {
        if (!fileName.contains(".")) {
            fileName += ".xlsx";
            success = exportToExcel(fileName, modelToExport, sheetName);
        } else {
            QMessageBox::warning(this, "警告", "不支持的文件格式");
            return;
        }
    }
    
    if (success) {
        QMessageBox::information(this, "成功", "数据导出成功");
    } else {
        QMessageBox::critical(this, "错误", "数据导出失败");
    }
}

void TeacherWidget::onSearchButtonClicked()
{
    // 使用输入对话框替代不存在的搜索框控件
    bool ok;
    QString searchText = QInputDialog::getText(this, "搜索", "请输入搜索内容:", QLineEdit::Normal, "", &ok);
    
    if (!ok || searchText.isEmpty()) {
        return;
    }
    
    // 根据当前标签页选择搜索范围
    int currentTab = ui->tabWidget->currentIndex();
    QStandardItemModel *modelToSearch = nullptr;
    
    if (currentTab == 0) { // 班级管理
        modelToSearch = studentsModel;
    } else if (currentTab == 1) { // 考勤管理
        modelToSearch = attendanceModel;
    } else if (currentTab == 2) { // 座位管理
        modelToSearch = seatsModel;
    } else {
        QMessageBox::warning(this, "警告", "当前页面不支持搜索");
        return;
    }
    
    // 执行搜索
    bool found = false;
    for (int row = 0; row < modelToSearch->rowCount(); ++row) {
        for (int col = 0; col < modelToSearch->columnCount(); ++col) {
            QModelIndex index = modelToSearch->index(row, col);
            QString cellData = modelToSearch->data(index).toString();
            
            if (cellData.contains(searchText, Qt::CaseInsensitive)) {
                // 找到匹配项，选中该行
                if (currentTab == 0) {
                    ui->studentsTableView->selectRow(row);
                    ui->studentsTableView->scrollTo(index);
                } else if (currentTab == 1) {
                    ui->attendanceTableView->selectRow(row);
                    ui->attendanceTableView->scrollTo(index);
                } else if (currentTab == 2) {
                    ui->seatsTableView->selectRow(row);
                    ui->seatsTableView->scrollTo(index);
                }
                
                found = true;
                break;
            }
        }
        
        if (found) break;
    }
    
    if (!found) {
        QMessageBox::information(this, "搜索结果", "未找到匹配项");
    }
}

void TeacherWidget::onSearchTextChanged(const QString& text)
{
    // 由于移除了搜索框控件，这个方法可能不再需要
    // 为了保持代码完整性，保留此方法但简化逻辑
    if (text.isEmpty()) {
        // 根据当前标签页操作
        int currentTab = ui->tabWidget->currentIndex();
        
        if (currentTab == 0 && !currentClassId.isEmpty()) { // 班级管理
            loadStudentsInClass(currentClassId);
        } else if (currentTab == 1 && !currentExamRoomId.isEmpty()) { // 考勤管理
            loadExamAttendance(currentExamRoomId);
        } else if (currentTab == 2 && !currentExamRoomId.isEmpty()) { // 座位管理
            loadExamSeats(currentExamRoomId);
        }
    }
}

void TeacherWidget::setUserInfo(const QString& username, const QString& token, const QString& userId)
{
    this->username = username;
    this->token = token;
    this->userId = userId;
    
    // 更新UI显示
    ui->nameLabel->setText("姓名: " + username);
}

void TeacherWidget::showMessage(const QString& message, bool isError)
{
    if (isError) {
        QMessageBox::warning(this, "错误", message);
    } else {
        QMessageBox::information(this, "提示", message);
    }
}

QJsonObject TeacherWidget::parseResponse(QNetworkReply* reply, bool showError)
{
    QJsonObject result;
    
    if (reply->error() != QNetworkReply::NoError) {
        if (showError) {
            QMessageBox::warning(this, "网络错误", "请求失败: " + reply->errorString());
        }
        return result;
    }
    
    QByteArray responseData = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
    
    if (jsonResponse.isObject()) {
        result = jsonResponse.object();
        
        // 检查错误消息
        if (result.contains("error") && showError) {
            QMessageBox::warning(this, "错误", result["error"].toString());
            return QJsonObject(); // 返回空对象表示错误
        }
    } else if (showError) {
        QMessageBox::warning(this, "错误", "无效的响应格式");
    }
    
    return result;
} 