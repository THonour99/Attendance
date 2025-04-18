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
    
    // 增加: 初始化状态更新定时器
    statusUpdateTimer = new QTimer(this);
    statusUpdateTimer->setInterval(10000); // 每10秒更新一次
    connect(statusUpdateTimer, &QTimer::timeout, this, &MainWindow::updateAttendanceStatus);
    
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
            // 启动状态更新定时器
            statusUpdateTimer->start();
        } else {
            ui->logEdit->append("已关闭自动考勤模式");
            ui->statusbar->showMessage("自动考勤模式已关闭", 3000);
            // 停止状态更新定时器
            statusUpdateTimer->stop();
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
    
    // 初始化考场列表UI
    setupExamRoomUI();
}

MainWindow::~MainWindow()
{
    // 关闭摄像头和释放资源
    if (timer->isActive())
        timer->stop();
    
    if (statusUpdateTimer->isActive())
        statusUpdateTimer->stop();
    
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
                // 增强：对训练图像进行预处理以提高训练质量
                cv::equalizeHist(faceImg, faceImg);
                
                // 增强：添加标准化步骤
                cv::Mat normalizedFace;
                cv::normalize(faceImg, normalizedFace, 0, 255, cv::NORM_MINMAX);
                
                faces.push_back(normalizedFace);
                labels.push_back(label);
                
                // 增强：通过小角度旋转增加训练样本（数据增强）
                cv::Mat rotatedFace;
                cv::Point2f center(faceImg.cols/2.0, faceImg.rows/2.0);
                
                // 添加+5度旋转的样本
                cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, 5, 1.0);
                cv::warpAffine(faceImg, rotatedFace, rotationMatrix, faceImg.size());
                cv::normalize(rotatedFace, rotatedFace, 0, 255, cv::NORM_MINMAX);
                faces.push_back(rotatedFace);
                labels.push_back(label);
                
                // 添加-5度旋转的样本
                rotationMatrix = cv::getRotationMatrix2D(center, -5, 1.0);
                cv::warpAffine(faceImg, rotatedFace, rotationMatrix, faceImg.size());
                cv::normalize(rotatedFace, rotatedFace, 0, 255, cv::NORM_MINMAX);
                faces.push_back(rotatedFace);
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
        // 增强：设置更优的LBPH参数
        model = cv::face::LBPHFaceRecognizer::create(
            8,    // radius
            8,    // neighbors
            8,    // grid_x
            8,    // grid_y
            80.0  // threshold
        );
        
        model->train(faces, labels);
        ui->logEdit->append("模型训练完成，共" + QString::number(faces.size()) + "个样本");
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
    
    // 增强预处理：应用直方图均衡化提高对比度
    cv::equalizeHist(grayFrame, grayFrame);
    
    // 增强：降噪处理
    cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);
    
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
    
    // 增强：应用额外的图像标准化
    cv::normalize(resizedFace, resizedFace, 0, 255, cv::NORM_MINMAX);
    
    // 执行人脸识别
    int label = -1;
    double confidence = 0.0;
    
    try {
        model->predict(resizedFace, label, confidence);
    } catch (const cv::Exception &e) {
        ui->logEdit->append("识别错误: " + QString(e.what()));
        return;
    }
    
    // 增强：多次识别取平均结果以提高准确性
    if (confidence < 90.0 && label >= 0 && label < faceNames.size()) {
        // 进行二次确认，连续3次识别并取平均置信度
        int confirmedLabel = label;
        double totalConfidence = confidence;
        int confirmCount = 1;
        
        // 尝试额外两次识别进行确认
        for (int i = 0; i < 2; i++) {
            // 稍微调整处理参数模拟不同条件
            cv::Mat confirmedFace = faceROI.clone();
            // 稍微改变亮度
            confirmedFace = confirmedFace * (0.95 + i * 0.1);
            cv::resize(confirmedFace, confirmedFace, cv::Size(100, 100));
            cv::normalize(confirmedFace, confirmedFace, 0, 255, cv::NORM_MINMAX);
            
            int tempLabel = -1;
            double tempConfidence = 0.0;
            
            try {
                model->predict(confirmedFace, tempLabel, tempConfidence);
                if (tempLabel == label) {
                    // 标签一致，累加置信度
                    totalConfidence += tempConfidence;
                    confirmCount++;
                }
            } catch (const cv::Exception &) {
                // 忽略确认过程中的错误
                continue;
            }
        }
        
        // 计算平均置信度
        double avgConfidence = totalConfidence / confirmCount;
        
        // 如果多次识别结果一致且平均置信度符合要求，则识别成功
        if (confirmCount >= 2 && avgConfidence < 85.0) {
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
                                   "), 置信度: " + QString::number(avgConfidence) + 
                                   ", 确认次数: " + QString::number(confirmCount));
                
                // 如果在考勤模式下，处理考勤
                processStudentRecognition(recognizedName);
            }
        } else {
            ui->logEdit->append("人脸验证未通过 (置信度: " + QString::number(avgConfidence) + 
                           ", 确认次数: " + QString::number(confirmCount) + ")");
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
        int selectedExamRoomId = ui->examRoomSelector->currentData().toInt();
        
        // 检查ID是否有效
        if (selectedExamRoomId <= 0) {
            QMessageBox::warning(this, "错误", "无效的考场ID");
            return;
        }
        
        // 更新当前选择的考场ID
        currentExamRoomId = selectedExamRoomId;
        
        // 尝试从考场列表中找到选定的考场
        ExamRoom selectedRoom;
        bool found = false;
        
        for (const auto &room : examRooms) {
            if (room.id == currentExamRoomId) {
                selectedRoom = room;
                found = true;
                break;
            }
        }
        
        if (!found) {
            QMessageBox::warning(this, "错误", "找不到所选考场的信息");
            return;
        }
        
        // 显示正在加载的消息
        ui->logEdit->append(QString("正在加载考场 %1 的学生名单...").arg(selectedRoom.name));
        
        // 加载学生名单
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
    qDebug() << "加载考场信息";
    
    try {
        // 获取考场信息
        QVector<ExamRoom> allExamRooms = NetworkManager::getInstance().getExamRooms();
        
        // 检查是否获取到考场
        if (allExamRooms.isEmpty()) {
            qDebug() << "警告: 未找到任何考场";
            QMessageBox::warning(this, "提示", "未找到任何考场信息");
            return;
        }
        
        // 使用所有考场
        QVector<ExamRoom> teacherExamRooms = allExamRooms;
        
        // 排序考场按日期和时间
        std::sort(teacherExamRooms.begin(), teacherExamRooms.end(), [](const ExamRoom &a, const ExamRoom &b) {
            if (a.examDate != b.examDate) {
                return a.examDate < b.examDate;
            }
            return a.examTime < b.examTime;
        });
        
        // 清除现有考场列表
        ui->examRoomSelector->clear();
        examRooms.clear();
        
        // 添加考场到下拉列表
        for (const ExamRoom &room : teacherExamRooms) {
            // 验证考场数据完整性
            if (room.id <= 0 || room.name.isEmpty()) {
                qDebug() << "警告: 跳过无效的考场数据，ID:" << room.id;
                continue;
            }
            
            QString displayText = QString("%1 - %2 (%3 %4)").arg(
                room.name.isEmpty() ? "未命名考场" : room.name,
                room.location.isEmpty() ? "未知地点" : room.location,
                room.examDate.isEmpty() ? "日期未定" : room.examDate,
                room.examTime.isEmpty() ? "时间未定" : room.examTime
            );
            ui->examRoomSelector->addItem(displayText, room.id);
            examRooms.append(room);
        }
        
        // 如果有考场，选择第一个但不立即加载学生名单
        if (!examRooms.isEmpty()) {
            ui->examRoomSelector->setCurrentIndex(0);
            currentExamRoomId = examRooms[0].id;
            
            // 显示成功提示
            ui->logEdit->append(QString("成功加载%1个考场").arg(examRooms.size()));
            
            // 不自动加载学生名单，等待用户点击加载按钮
            // loadStudentList(currentExamRoomId);
        } else {
            ui->logEdit->append("没有有效的考场数据");
            ui->statusbar->showMessage("没有有效的考场数据", 3000);
        }
    }
    catch (const std::exception& e) {
        ui->logEdit->append("加载考场信息时发生异常: " + QString(e.what()));
        ui->statusbar->showMessage("加载考场信息失败", 3000);
    }
    catch (...) {
        ui->logEdit->append("加载考场信息时发生未知异常");
        ui->statusbar->showMessage("加载考场列表失败", 3000);
    }
}

void MainWindow::loadStudentList(int examRoomId)
{
    // 检查考场ID是否有效
    if (examRoomId <= 0) {
        ui->logEdit->append("错误：无效的考场ID");
        ui->statusbar->showMessage("无效的考场ID，无法加载学生", 3000);
        return;
    }

    ui->studentListTable->setRowCount(0);
    ui->statusbar->showMessage("正在加载学生名单...", 2000);
    
    try {
        // 从服务器获取学生名单
        currentStudents = NetworkManager::getInstance().getStudentsForExam(examRoomId);
        
        if (currentStudents.isEmpty()) {
            ui->logEdit->append("未能获取学生名单或当前考场没有安排学生");
            ui->statusbar->showMessage("未能获取学生名单", 3000);
            return;
        }
        
        // 按座位号排序
        std::sort(currentStudents.begin(), currentStudents.end(), [](const StudentInfo &a, const StudentInfo &b) {
            return a.seatNumber < b.seatNumber;
        });
        
        // 清空表格并设置适当的行数以提高性能
        ui->studentListTable->clearContents();
        ui->studentListTable->setRowCount(currentStudents.size());
        
        int row = 0;
        for (const auto &student : currentStudents) {
            QTableWidgetItem* idItem = new QTableWidgetItem(student.studentId);
            QTableWidgetItem* nameItem = new QTableWidgetItem(student.name);
            QTableWidgetItem* seatItem = new QTableWidgetItem(QString::number(student.seatNumber));
            QTableWidgetItem* statusItem = new QTableWidgetItem(student.attendanceStatus);
            
            ui->studentListTable->setItem(row, 0, idItem);
            ui->studentListTable->setItem(row, 1, nameItem);
            ui->studentListTable->setItem(row, 2, seatItem);
            ui->studentListTable->setItem(row, 3, statusItem);
            
            // 设置不同状态的颜色
            if (student.attendanceStatus == "已到") {
                statusItem->setBackground(QColor(200, 255, 200)); // 绿色
            } else if (student.attendanceStatus == "迟到") {
                statusItem->setBackground(QColor(255, 255, 200)); // 黄色
            } else if (student.attendanceStatus == "缺席") {
                statusItem->setBackground(QColor(255, 200, 200)); // 红色
            }
            
            row++;
        }
        
        ui->logEdit->append(QString("已加载 %1 名学生").arg(currentStudents.size()));
        
        // 更新考勤统计
        updateAttendanceStats();
    }
    catch (const std::exception& e) {
        ui->logEdit->append("加载学生名单时发生异常: " + QString(e.what()));
        ui->statusbar->showMessage("加载学生名单失败", 3000);
    }
    catch (...) {
        ui->logEdit->append("加载学生名单时发生未知异常");
        ui->statusbar->showMessage("加载学生名单失败", 3000);
    }
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
    
    ui->logEdit->append("正在处理学生考勤: " + recognizedName);
    
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
        
        // 如果学生已经签到过，显示提示但不再更新状态
        if (currentStatus == "已到" || currentStatus == "迟到") {
            ui->logEdit->append(QString("学生 %1 (座位 %2) 已经签到过，状态为: %3")
                              .arg(recognizedName)
                              .arg(currentStudents[studentRow].seatNumber)
                              .arg(currentStatus));
            return;
        }
        
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
        bool success = NetworkManager::getInstance().uploadAttendanceRecord(
            currentStudents[studentRow].id, 
            currentExamRoomId, 
            QDateTime::currentDateTime()
        );
        
        if (success) {
            ui->logEdit->append(QString("学生 %1 (座位 %2) 考勤状态已更新为: %3")
                              .arg(recognizedName)
                              .arg(currentStudents[studentRow].seatNumber)
                              .arg(newStatus));
            
            // 更新考勤统计
            updateAttendanceStats();
            
            // 在摄像头预览上显示签到成功的文字
            showSuccessOverlay(recognizedName, currentStudents[studentRow].seatNumber, newStatus);
        } else {
            ui->logEdit->append("考勤记录上传失败，请重试或检查网络连接");
        }
    } else {
        ui->logEdit->append("警告: 识别到的学生 " + recognizedName + " 不在当前考场名单中");
    }
}

// 显示签到成功的覆盖图
void MainWindow::showSuccessOverlay(const QString &name, int seatNumber, const QString &status)
{
    // 创建一个临时的图像用于显示
    cv::Mat overlay = frame.clone();
    
    // 添加绿色透明背景
    cv::Mat colorOverlay(overlay.size(), CV_8UC3, cv::Scalar(0, 200, 0));
    cv::addWeighted(overlay, 0.7, colorOverlay, 0.3, 0, overlay);
    
    // 添加签到成功信息
    QString message = QString("%1 签到成功").arg(name);
    cv::putText(overlay, message.toStdString(), cv::Point(50, 100), 
               cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(255, 255, 255), 2);
    
    QString seatInfo = QString("座位号: %1").arg(seatNumber);
    cv::putText(overlay, seatInfo.toStdString(), cv::Point(50, 150), 
               cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(255, 255, 255), 2);
    
    QString statusInfo = QString("状态: %1").arg(status);
    cv::putText(overlay, statusInfo.toStdString(), cv::Point(50, 200), 
               cv::FONT_HERSHEY_SIMPLEX, 1.5, cv::Scalar(255, 255, 255), 2);
    
    QString timeInfo = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    cv::putText(overlay, timeInfo.toStdString(), cv::Point(50, 250), 
               cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
    
    // 显示覆盖图像
    QImage qimg(overlay.data, overlay.cols, overlay.rows, 
                static_cast<int>(overlay.step), QImage::Format_BGR888);
    ui->cameraLabel->setPixmap(QPixmap::fromImage(qimg).scaled(
        ui->cameraLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    
    // 创建一个定时器，2秒后恢复正常显示
    QTimer::singleShot(2000, this, [this]() {
        // 恢复摄像头的正常显示
        QImage qimg(frame.data, frame.cols, frame.rows, 
                   static_cast<int>(frame.step), QImage::Format_BGR888);
        ui->cameraLabel->setPixmap(QPixmap::fromImage(qimg).scaled(
            ui->cameraLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    });
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

// 实现考场列表UI初始化
void MainWindow::setupExamRoomUI()
{
    // 美化考场列表
    ui->examRoomSelector->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #ced4da; border-radius: 4px; }"
                                       "QComboBox::drop-down { border: 0px; }");
    
    // 美化学生列表表格
    ui->studentListTable->setAlternatingRowColors(true);
    ui->studentListTable->setStyleSheet("QTableView { border: 1px solid #ced4da; border-radius: 4px; }"
                                       "QTableView::item { padding: 4px; }"
                                       "QTableView::item:selected { background-color: #007bff; color: white; }");
    ui->studentListTable->horizontalHeader()->setStretchLastSection(true);
    ui->studentListTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->studentListTable->verticalHeader()->setVisible(false);
    
    // 美化按钮
    QString buttonStyle = "QPushButton { background-color: #007bff; color: white; padding: 6px 12px; "
                         "border-radius: 4px; font-weight: bold; }"
                         "QPushButton:hover { background-color: #0069d9; }"
                         "QPushButton:pressed { background-color: #0062cc; }";
    
    ui->loadExamRoomsButton->setStyleSheet(buttonStyle);
    ui->loadStudentsButton->setStyleSheet(buttonStyle);
    
    // 创建状态标签（直接添加到状态栏）
    attendanceStatsLabel = new QLabel("出勤: 0 | 迟到: 0 | 缺席: 0 | 总计: 0", this);
    attendanceStatsLabel->setStyleSheet("QLabel { padding: 5px; background-color: #f8f9fa; border: 1px solid #ced4da; border-radius: 4px; }");
    ui->statusbar->addPermanentWidget(attendanceStatsLabel);
    
    // 创建刷新按钮（直接添加到状态栏）
    QPushButton* refreshButton = new QPushButton("刷新考勤状态", this);
    refreshButton->setStyleSheet(buttonStyle);
    refreshButton->setMaximumHeight(24);  // 使按钮适合状态栏高度
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::updateAttendanceStatus);
    ui->statusbar->addPermanentWidget(refreshButton);
    
    // 创建导出按钮（直接添加到状态栏）
    QPushButton* exportButton = new QPushButton("导出考勤记录", this);
    exportButton->setStyleSheet(buttonStyle);
    exportButton->setMaximumHeight(24);  // 使按钮适合状态栏高度
    
    connect(exportButton, &QPushButton::clicked, this, [this]() {
        if (currentExamRoomId < 0 || currentStudents.isEmpty()) {
            QMessageBox::warning(this, "警告", "请先选择考场并加载学生名单");
            return;
        }
        
        QString fileName = QFileDialog::getSaveFileName(
            this,
            "导出考勤记录",
            QCoreApplication::applicationDirPath() + "/考勤记录_" + ui->examRoomSelector->currentText() + ".csv",
            "CSV文件 (*.csv)");
            
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out.setCodec("UTF-8");
                
                // 写入表头
                out << "学号,姓名,座位号,考勤状态,时间\n";
                
                // 写入数据
                QDateTime now = QDateTime::currentDateTime();
                for (const auto &student : currentStudents) {
                    out << student.studentId << ","
                        << student.name << ","
                        << student.seatNumber << ","
                        << student.attendanceStatus << ","
                        << now.toString("yyyy-MM-dd hh:mm:ss") << "\n";
                }
                
                file.close();
                QMessageBox::information(this, "导出成功", "考勤记录已成功导出到: " + fileName);
            } else {
                QMessageBox::warning(this, "导出失败", "无法写入文件: " + fileName);
            }
        }
    });
    ui->statusbar->addPermanentWidget(exportButton);
}

// 实现考勤状态实时更新机制
void MainWindow::updateAttendanceStatus()
{
    if (currentExamRoomId < 0 || currentStudents.isEmpty()) {
        return;
    }
    
    // 从服务器获取最新的考勤状态
    QVector<StudentInfo> updatedStudents = NetworkManager::getInstance().getStudentsForExam(currentExamRoomId);
    
    if (updatedStudents.isEmpty()) {
        ui->logEdit->append("获取考勤状态更新失败");
        return;
    }
    
    // 更新学生状态
    for (const auto &updatedStudent : updatedStudents) {
        for (int i = 0; i < currentStudents.size(); i++) {
            if (currentStudents[i].id == updatedStudent.id) {
                // 如果状态有变化，更新UI
                if (currentStudents[i].attendanceStatus != updatedStudent.attendanceStatus) {
                    currentStudents[i].attendanceStatus = updatedStudent.attendanceStatus;
                    updateStudentAttendance(i, updatedStudent.attendanceStatus);
                }
                break;
            }
        }
    }
    
    // 更新考勤统计
    updateAttendanceStats();
    
    ui->logEdit->append("考勤状态已更新: " + QDateTime::currentDateTime().toString("hh:mm:ss"));
}

// 更新考勤统计数据
void MainWindow::updateAttendanceStats()
{
    int present = 0;
    int late = 0;
    int absent = 0;
    
    for (const auto &student : currentStudents) {
        if (student.attendanceStatus == "已到") {
            present++;
        } else if (student.attendanceStatus == "迟到") {
            late++;
        } else if (student.attendanceStatus == "缺席") {
            absent++;
        }
    }
    
    int total = currentStudents.size();
    
    // 更新统计标签
    attendanceStatsLabel->setText(QString("出勤: %1 | 迟到: %2 | 缺席: %3 | 总计: %4")
                                 .arg(present).arg(late).arg(absent).arg(total));
}

