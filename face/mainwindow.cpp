#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentExamRoomId(-1)
    , isAttendanceMode(false)
{
    ui->setupUi(this);
    
    // 设置窗口标题
    setWindowTitle("人脸识别系统 - 基于OpenCV 4.6.0");
    
    // 初始化成员变量
    timer = new QTimer(this);
    model = cv::face::LBPHFaceRecognizer::create();
    capturing = false;
    captureCount = 0;
    
    // 连接信号和槽
    connect(timer, &QTimer::timeout, this, &MainWindow::processFrame);
    
    // 连接菜单项信号和槽
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::on_actionAbout_triggered);
    connect(ui->actionViewRecords, &QAction::triggered, this, &MainWindow::on_actionViewRecords_triggered);
    
    // 连接网络错误信号
    connect(&NetworkManager::getInstance(), &NetworkManager::networkError, 
            this, &MainWindow::onNetworkError);
    
    // 连接考勤模式切换信号
    connect(ui->attendanceModeCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        isAttendanceMode = checked;
        if (checked) {
            ui->logEdit->append("已开启自动考勤模式");
            ui->statusbar->showMessage("自动考勤模式已开启", 3000);
        } else {
            ui->logEdit->append("已关闭自动考勤模式");
            ui->statusbar->showMessage("自动考勤模式已关闭", 3000);
        }
    });
    
    // 初始化人脸检测器
    setupFaceDetector();
    
    // 初始化数据库
    initDatabase();
    
    // 加载已有人脸数据
    if (loadFaceData()) {
        ui->logEdit->append("已加载人脸数据");
        ui->statusbar->showMessage("人脸数据加载成功", 3000);
    } else {
        ui->logEdit->append("没有找到人脸数据或加载失败");
        ui->statusbar->showMessage("没有找到人脸数据", 3000);
    }
    
    // 启动摄像头
    capture.open(0);
    if (capture.isOpened()) {
        timer->start(30);
        ui->logEdit->append("摄像头已打开");
        ui->statusbar->showMessage("摄像头已打开", 3000);
    } else {
        ui->logEdit->append("无法打开摄像头");
        ui->statusbar->showMessage("无法打开摄像头，请检查设备", 3000);
        QMessageBox::warning(this, "错误", "无法打开摄像头，请检查设备连接！");
    }
    
    // 自动加载考场信息
    loadExamRooms();
}

MainWindow::~MainWindow()
{
    // 关闭摄像头和释放资源
    if (timer->isActive())
        timer->stop();
    
    if (capture.isOpened())
        capture.release();
        
    delete ui;
}

void MainWindow::setupFaceDetector()
{
    // 加载人脸分类器
    QString faceCascadePath = QCoreApplication::applicationDirPath() + 
                             "/haarcascade_frontalface_default.xml";
    
    // 如果没有找到级联分类器文件，尝试从OpenCV目录读取
    if (!QFile::exists(faceCascadePath)) {
        faceCascadePath = "D:/opencv/mingw-build-with-contrib/install/etc/haarcascades/haarcascade_frontalface_default.xml";
    }
    
    if (!faceCascade.load(faceCascadePath.toStdString())) {
        ui->logEdit->append("无法加载人脸检测模型: " + faceCascadePath);
    } else {
        ui->logEdit->append("已加载人脸检测模型");
    }
}

void MainWindow::initDatabase()
{
    // 创建数据目录
    QDir().mkpath(QCoreApplication::applicationDirPath() + "/data");
    
    // 初始化SQL数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/data/face_db.sqlite");
    
    if (!db.open()) {
        ui->logEdit->append("数据库打开失败: " + db.lastError().text());
        return;
    }
    
    // 创建用户表
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, user_id TEXT)");
    
    // 创建考勤表
    query.exec("CREATE TABLE IF NOT EXISTS attendance (id INTEGER PRIMARY KEY, user_id TEXT, name TEXT, time TEXT)");
}

bool MainWindow::loadFaceData()
{
    QString facesPath = QCoreApplication::applicationDirPath() + "/data/faces.xml";
    QString namesPath = QCoreApplication::applicationDirPath() + "/data/names.txt";
    
    if (!QFile::exists(facesPath) || !QFile::exists(namesPath)) {
        return false;
    }
    
    // 加载人脸识别模型
    try {
        model->read(facesPath.toStdString());
    } catch (const cv::Exception& e) {
        qDebug() << "加载人脸模型失败：" << e.what();
        return false;
    }
    
    // 加载名称数据
    QFile namesFile(namesPath);
    if (namesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&namesFile);
        faceNames.clear();
        faceLabels.clear();
        
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(",");
            if (parts.size() == 2) {
                int label = parts[0].toInt();
                QString name = parts[1];
                faceLabels.append(label);
                faceNames.append(name);
            }
        }
        
        namesFile.close();
        return true;
    }
    
    return false;
}

void MainWindow::saveFaceData()
{
    QString facesPath = QCoreApplication::applicationDirPath() + "/data/faces.xml";
    QString namesPath = QCoreApplication::applicationDirPath() + "/data/names.txt";
    
    // 保存人脸识别模型
    model->write(facesPath.toStdString());
    
    // 保存名称数据
    QFile namesFile(namesPath);
    if (namesFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&namesFile);
        for (int i = 0; i < faceLabels.size() && i < faceNames.size(); i++) {
            out << faceLabels[i] << "," << faceNames[i] << "\n";
        }
        namesFile.close();
    }
}

void MainWindow::trainModel()
{
    if (faceLabels.isEmpty()) {
        ui->logEdit->append("没有训练数据");
        return;
    }
    
    QDir dataDir(QCoreApplication::applicationDirPath() + "/data/faces");
    if (!dataDir.exists()) {
        ui->logEdit->append("没有找到人脸数据目录");
        return;
    }
    
    std::vector<cv::Mat> faces;
    std::vector<int> labels;
    
    // 遍历所有人脸图像
    for (int i = 0; i < faceLabels.size(); i++) {
        int label = faceLabels[i];
        QDir personDir(dataDir.absolutePath() + "/" + QString::number(label));
        
        if (!personDir.exists()) {
            continue;
        }
        
        QStringList filters;
        filters << "*.jpg" << "*.png";
        QFileInfoList fileList = personDir.entryInfoList(filters, QDir::Files);
        
        for (const QFileInfo &fileInfo : fileList) {
            cv::Mat faceImg = cv::imread(fileInfo.absoluteFilePath().toStdString(), 
                                         cv::IMREAD_GRAYSCALE);
            if (!faceImg.empty()) {
                faces.push_back(faceImg);
                labels.push_back(label);
            }
        }
    }
    
    if (faces.empty()) {
        ui->logEdit->append("没有找到人脸图像进行训练");
        return;
    }
    
    // 训练模型
    try {
        model->train(faces, labels);
        ui->logEdit->append("模型训练完成");
        saveFaceData();
    } catch (const cv::Exception& e) {
        ui->logEdit->append("训练失败: " + QString(e.what()));
    }
}

void MainWindow::processFrame()
{
    // 捕获一帧图像
    capture >> frame;
    
    if (frame.empty()) {
        return;
    }
    
    // 复制帧用于显示
    cv::Mat displayFrame = frame.clone();
    
    // 转换为灰度图
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    
    // 在灰度图上做直方图均衡化
    cv::equalizeHist(grayFrame, grayFrame);
    
    // 检测人脸
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayFrame, faces, 1.1, 5, 0, cv::Size(30, 30));
    
    // 绘制检测到的人脸
    for (const auto& face : faces) {
        cv::rectangle(displayFrame, face, cv::Scalar(0, 255, 0), 2);
        
        // 如果正在捕获人脸，保存此人脸
        if (capturing && !currentUser.isEmpty() && captureCount < 10) {
            cv::Mat faceROI = grayFrame(face).clone();
            
            // 调整大小为统一尺寸
            cv::Mat resizedFace;
            cv::resize(faceROI, resizedFace, cv::Size(100, 100));
            
            // 创建目录存储人脸
            int label = -1;
            int idx = faceNames.indexOf(currentUser);
            if (idx != -1) {
                label = faceLabels[idx];
            } else {
                label = faceLabels.isEmpty() ? 0 : (faceLabels.last() + 1);
                faceLabels.append(label);
                faceNames.append(currentUser);
            }
            
            QDir().mkpath(QCoreApplication::applicationDirPath() + 
                         "/data/faces/" + QString::number(label));
            
            // 保存人脸图像
            QString filename = QCoreApplication::applicationDirPath() + 
                              "/data/faces/" + QString::number(label) + 
                              "/face_" + QString::number(captureCount) + ".jpg";
            
            cv::imwrite(filename.toStdString(), resizedFace);
            
            captureCount++;
            ui->logEdit->append("已捕获第 " + QString::number(captureCount) + " 张人脸");
            
            // 如果已经捕获了足够的图像，停止捕获并训练模型
            if (captureCount >= 10) {
                capturing = false;
                trainModel();
                ui->logEdit->append("人脸采集完成");
            }
        }
        
        // 如果当前在识别模式，对人脸进行识别
        if (!capturing && !faces.empty()) {
            recognizeFace();
        }
    }
    
    // 将OpenCV图像转换为Qt图像并显示
    QImage qimg(displayFrame.data, displayFrame.cols, displayFrame.rows, 
                static_cast<int>(displayFrame.step), QImage::Format_BGR888);
    ui->cameraLabel->setPixmap(QPixmap::fromImage(qimg).scaled(
        ui->cameraLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::recognizeFace()
{
    if (frame.empty() || faceNames.isEmpty()) {
        return;
    }
    
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayFrame, faces, 1.1, 5, 0, cv::Size(30, 30));
    
    if (faces.empty()) {
        return;
    }
    
    // 选取最大的人脸
    auto maxFaceIt = std::max_element(faces.begin(), faces.end(),
        [](const cv::Rect& a, const cv::Rect& b) {
            return a.area() < b.area();
        });
    
    cv::Rect face = *maxFaceIt;
    
    // 裁剪人脸区域
    cv::Mat faceROI = grayFrame(face).clone();
    
    // 调整尺寸
    cv::Mat resizedFace;
    cv::resize(faceROI, resizedFace, cv::Size(100, 100));
    
    // 执行人脸识别
    int label = -1;
    double confidence = 0.0;
    
    try {
        model->predict(resizedFace, label, confidence);
    } catch (const cv::Exception &e) {
        ui->logEdit->append("识别错误: " + QString(e.what()));
        return;
    }
    
    // 如果置信度低于设定阈值，认为识别成功
    if (confidence < 85.0 && label >= 0 && label < faceNames.size()) {
        QString recognizedName = faceNames[label];
        
        // 获取对应的用户ID
        QSqlQuery query;
        query.prepare("SELECT user_id FROM users WHERE name = :name");
        query.bindValue(":name", recognizedName);
        
        if (query.exec() && query.next()) {
            QString userId = query.value(0).toString();
            
            // 显示识别结果
            cv::putText(frame, recognizedName.toStdString(), 
                       cv::Point(face.x, face.y - 10), 
                       cv::FONT_HERSHEY_SIMPLEX, 1.0, 
                       cv::Scalar(0, 255, 0), 2);
            
            ui->logEdit->append("识别到: " + recognizedName + " (ID: " + userId + 
                               "), 置信度: " + QString::number(confidence));
            
            // 如果在考勤模式下，处理考勤
            processStudentRecognition(recognizedName);
        }
    } else {
        ui->logEdit->append("未能识别人脸或置信度太低 (" + QString::number(confidence) + ")");
    }
}

void MainWindow::attendanceRecord(const QString &name)
{
    if (name.isEmpty()) {
        ui->logEdit->append("请先识别人脸");
        return;
    }
    
    // 获取用户ID
    QSqlQuery query;
    query.prepare("SELECT user_id FROM users WHERE name = :name");
    query.bindValue(":name", name);
    
    if (!query.exec() || !query.next()) {
        ui->logEdit->append("未找到用户: " + name);
        return;
    }
    
    QString userId = query.value(0).toString();
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    
    // 记录考勤
    query.prepare("INSERT INTO attendance (user_id, name, time) VALUES (:user_id, :name, :time)");
    query.bindValue(":user_id", userId);
    query.bindValue(":name", name);
    query.bindValue(":time", currentTime);
    
    if (query.exec()) {
        ui->logEdit->append(name + " 签到成功: " + currentTime);
    } else {
        ui->logEdit->append("签到失败: " + query.lastError().text());
    }
}

void MainWindow::on_registerButton_clicked()
{
    QString name = ui->nameEdit->text().trimmed();
    QString id = ui->idEdit->text().trimmed();
    
    if (name.isEmpty() || id.isEmpty()) {
        ui->logEdit->append("请输入姓名和ID");
        return;
    }
    
    // 检查ID是否已存在
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE user_id = :user_id");
    query.bindValue(":user_id", id);
    
    if (query.exec() && query.next()) {
        ui->logEdit->append("ID已存在，请使用其他ID");
        return;
    }
    
    // 插入新用户
    query.prepare("INSERT INTO users (name, user_id) VALUES (:name, :user_id)");
    query.bindValue(":name", name);
    query.bindValue(":user_id", id);
    
    if (query.exec()) {
        ui->logEdit->append("用户注册成功: " + name + " (ID: " + id + ")");
        currentUser = name;
    } else {
        ui->logEdit->append("用户注册失败: " + query.lastError().text());
    }
}

void MainWindow::on_captureButton_clicked()
{
    if (currentUser.isEmpty()) {
        ui->logEdit->append("请先注册用户信息");
        return;
    }
    
    capturing = true;
    captureCount = 0;
    ui->logEdit->append("开始采集 " + currentUser + " 的人脸数据，请面向摄像头");
}

void MainWindow::on_recognizeButton_clicked()
{
    if (faceNames.isEmpty()) {
        ui->logEdit->append("没有训练数据，请先采集人脸");
        return;
    }
    
    // 识别过程在processFrame中实现
    ui->logEdit->append("请面向摄像头进行识别");
}

void MainWindow::on_attendanceButton_clicked()
{
    if (frame.empty() || faceNames.isEmpty()) {
        ui->logEdit->append("请先进行人脸识别");
        return;
    }
    
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
    
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayFrame, faces, 1.1, 5, 0, cv::Size(30, 30));
    
    if (faces.empty()) {
        ui->logEdit->append("未检测到人脸");
        return;
    }
    
    // 选择最大的人脸
    auto maxFaceIt = std::max_element(faces.begin(), faces.end(),
        [](const cv::Rect& a, const cv::Rect& b) {
            return a.area() < b.area();
        });
    
    cv::Rect face = *maxFaceIt;
    cv::Mat faceROI = grayFrame(face).clone();
    cv::Mat resizedFace;
    cv::resize(faceROI, resizedFace, cv::Size(100, 100));
    
    // 执行人脸识别
    int label = -1;
    double confidence = 0.0;
    
    try {
        model->predict(resizedFace, label, confidence);
    } catch (const cv::Exception &e) {
        ui->logEdit->append("识别错误: " + QString(e.what()));
        return;
    }
    
    // 如果置信度低于设定阈值，认为识别成功
    if (confidence < 85.0 && label >= 0 && label < faceNames.size()) {
        QString recognizedName = faceNames[label];
        attendanceRecord(recognizedName);
    } else {
        ui->logEdit->append("无法识别，无法签到");
    }
}

// 新增菜单项槽函数实现
void MainWindow::on_actionExit_triggered()
{
    // 退出程序前确认
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "退出确认", 
                                  "确定要退出人脸识别系统吗？",
                                  QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QString aboutText = "人脸识别系统 v1.0\n\n"
                        "基于Qt 5.15.2和OpenCV 4.6.0\n"
                        "支持人脸注册、识别和考勤功能\n\n"
                        "© 2023 FaceRecognition";
    
    QMessageBox::about(this, "关于", aboutText);
}

void MainWindow::on_viewRecordsButton_clicked()
{
    showAttendanceRecords();
}

void MainWindow::on_actionViewRecords_triggered()
{
    showAttendanceRecords();
}

void MainWindow::showAttendanceRecords()
{
    // 创建一个新窗口显示考勤记录
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("考勤记录");
    dialog->setMinimumSize(600, 400);
    
    // 创建表格控件
    QTableWidget *tableWidget = new QTableWidget(dialog);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({"ID", "姓名", "用户ID", "签到时间"});
    
    // 设置表格样式
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 查询考勤记录
    QSqlQuery query;
    if (query.exec("SELECT a.id, a.name, a.user_id, a.time FROM attendance a ORDER BY a.time DESC")) {
        int row = 0;
        while (query.next()) {
            tableWidget->insertRow(row);
            
            tableWidget->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            tableWidget->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            tableWidget->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
            tableWidget->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
            
            row++;
        }
        
        ui->statusbar->showMessage("已加载 " + QString::number(row) + " 条考勤记录", 3000);
    } else {
        ui->statusbar->showMessage("加载考勤记录失败: " + query.lastError().text(), 3000);
    }
    
    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(tableWidget);
    
    // 添加导出按钮
    QPushButton *exportButton = new QPushButton("导出CSV", dialog);
    connect(exportButton, &QPushButton::clicked, [=]() {
        QString fileName = QFileDialog::getSaveFileName(
            dialog,
            "导出考勤记录",
            QCoreApplication::applicationDirPath() + "/attendance_export.csv",
            "CSV文件 (*.csv)");
            
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out.setCodec("UTF-8");
                
                // 写入表头
                out << "ID,姓名,用户ID,签到时间\n";
                
                // 写入数据
                for (int row = 0; row < tableWidget->rowCount(); ++row) {
                    out << tableWidget->item(row, 0)->text() << ","
                        << tableWidget->item(row, 1)->text() << ","
                        << tableWidget->item(row, 2)->text() << ","
                        << tableWidget->item(row, 3)->text() << "\n";
                }
                
                file.close();
                QMessageBox::information(dialog, "导出成功", "考勤记录已成功导出到: " + fileName);
            } else {
                QMessageBox::warning(dialog, "导出失败", "无法写入文件: " + fileName);
            }
        }
    });
    
    layout->addWidget(exportButton);
    
    // 显示对话框
    dialog->setLayout(layout);
    dialog->exec();
    
    // 对话框关闭后释放资源
    delete dialog;
}

void MainWindow::on_loadExamRoomsButton_clicked()
{
    loadExamRooms();
}

void MainWindow::on_loadStudentsButton_clicked()
{
    if (ui->examRoomSelector->currentIndex() >= 0) {
        currentExamRoomId = ui->examRoomSelector->currentData().toInt();
        loadStudentList(currentExamRoomId);
    } else {
        QMessageBox::warning(this, "错误", "请先选择考场");
    }
}

void MainWindow::on_examRoomSelector_currentIndexChanged(int index)
{
    if (index >= 0 && index < examRooms.size()) {
        const ExamRoom &room = examRooms[index];
        ui->examInfoLabel->setText(QString("考场位置：%1 | 时间：%2 %3 | 容量：%4")
                                  .arg(room.location)
                                  .arg(room.examDate)
                                  .arg(room.examTime)
                                  .arg(room.capacity));
    } else {
        ui->examInfoLabel->setText("考场位置：| 时间：| 容量：");
    }
}

void MainWindow::onNetworkError(const QString &errorMessage)
{
    ui->logEdit->append("网络错误: " + errorMessage);
    ui->statusbar->showMessage("网络错误，请检查连接", 5000);
}

void MainWindow::loadExamRooms()
{
    ui->examRoomSelector->clear();
    ui->statusbar->showMessage("正在加载考场信息...", 2000);
    
    // 从服务器获取考场信息
    examRooms = NetworkManager::getInstance().getExamRooms();
    
    if (examRooms.isEmpty()) {
        ui->logEdit->append("未能获取考场信息或当前没有可用考场");
        ui->statusbar->showMessage("未能获取考场信息", 3000);
        return;
    }
    
    for (const auto &room : examRooms) {
        ui->examRoomSelector->addItem(room.name, room.id);
    }
    
    ui->logEdit->append(QString("已加载 %1 个考场").arg(examRooms.size()));
}

void MainWindow::loadStudentList(int examRoomId)
{
    ui->studentListTable->setRowCount(0);
    ui->statusbar->showMessage("正在加载学生名单...", 2000);
    
    // 从服务器获取学生名单
    currentStudents = NetworkManager::getInstance().getStudentsForExam(examRoomId);
    
    if (currentStudents.isEmpty()) {
        ui->logEdit->append("未能获取学生名单或当前考场没有安排学生");
        ui->statusbar->showMessage("未能获取学生名单", 3000);
        return;
    }
    
    int row = 0;
    for (const auto &student : currentStudents) {
        ui->studentListTable->insertRow(row);
        ui->studentListTable->setItem(row, 0, new QTableWidgetItem(student.studentId));
        ui->studentListTable->setItem(row, 1, new QTableWidgetItem(student.name));
        ui->studentListTable->setItem(row, 2, new QTableWidgetItem(QString::number(student.seatNumber)));
        ui->studentListTable->setItem(row, 3, new QTableWidgetItem(student.attendanceStatus));
        
        // 设置不同状态的颜色
        if (student.attendanceStatus == "已到") {
            ui->studentListTable->item(row, 3)->setBackground(QColor(200, 255, 200)); // 绿色
        } else if (student.attendanceStatus == "迟到") {
            ui->studentListTable->item(row, 3)->setBackground(QColor(255, 255, 200)); // 黄色
        } else if (student.attendanceStatus == "缺席") {
            ui->studentListTable->item(row, 3)->setBackground(QColor(255, 200, 200)); // 红色
        }
        
        row++;
    }
    
    ui->logEdit->append(QString("已加载 %1 名学生").arg(currentStudents.size()));
}

void MainWindow::updateStudentAttendance(int studentRow, const QString &status)
{
    if (studentRow >= 0 && studentRow < ui->studentListTable->rowCount()) {
        ui->studentListTable->item(studentRow, 3)->setText(status);
        
        // 设置颜色
        if (status == "已到") {
            ui->studentListTable->item(studentRow, 3)->setBackground(QColor(200, 255, 200));
        } else if (status == "迟到") {
            ui->studentListTable->item(studentRow, 3)->setBackground(QColor(255, 255, 200));
        } else if (status == "缺席") {
            ui->studentListTable->item(studentRow, 3)->setBackground(QColor(255, 200, 200));
        }
        
        // 更新内存中的状态
        if (studentRow < currentStudents.size()) {
            currentStudents[studentRow].attendanceStatus = status;
        }
    }
}

void MainWindow::processStudentRecognition(const QString &recognizedName)
{
    if (!isAttendanceMode || currentExamRoomId < 0 || recognizedName.isEmpty()) {
        return;
    }
    
    // 在当前考场学生中查找匹配的学生
    int studentRow = -1;
    for (int i = 0; i < currentStudents.size(); i++) {
        if (currentStudents[i].name == recognizedName) {
            studentRow = i;
            break;
        }
    }
    
    if (studentRow >= 0) {
        // 检查当前状态
        QString currentStatus = currentStudents[studentRow].attendanceStatus;
        if (currentStatus != "已到" && currentStatus != "迟到") {
            // 根据当前时间判断是否迟到
            QDateTime now = QDateTime::currentDateTime();
            QDateTime examStartTime;
            
            if (!examRooms.isEmpty() && currentExamRoomId >= 0) {
                for (const auto &room : examRooms) {
                    if (room.id == currentExamRoomId) {
                        QString dateTimeStr = room.examDate + " " + room.examTime.split("-").first();
                        examStartTime = QDateTime::fromString(dateTimeStr, "yyyy-MM-dd hh:mm");
                        break;
                    }
                }
            }
            
            QString newStatus = "已到";
            if (examStartTime.isValid() && now > examStartTime.addSecs(15 * 60)) { // 15分钟后算迟到
                newStatus = "迟到";
            }
            
            // 更新学生状态
            updateStudentAttendance(studentRow, newStatus);
            
            // 上传考勤记录到服务器
            NetworkManager::getInstance().uploadAttendanceRecord(
                currentStudents[studentRow].id, 
                currentExamRoomId, 
                QDateTime::currentDateTime()
            );
            
            ui->logEdit->append(QString("学生 %1 (座位 %2) 考勤状态已更新为: %3")
                              .arg(recognizedName)
                              .arg(currentStudents[studentRow].seatNumber)
                              .arg(newStatus));
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 显示确认对话框
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, 
        "退出确认", 
        "确定要退出系统吗？",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        // 保存相关数据
        saveFaceData();
        event->accept();  // 允许窗口关闭
    } else {
        event->ignore();  // 取消关闭
    }
}
