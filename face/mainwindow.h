#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QComboBox>
#include <QTableWidget>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <QTextStream>
#include <QDateTimeEdit>
#include <QCloseEvent>
#include "logindialog.h"
// #include "registerdialog.h"
#include "networkmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_registerButton_clicked();
    void on_captureButton_clicked();
    void on_recognizeButton_clicked();
    void on_attendanceButton_clicked();
    void on_viewRecordsButton_clicked();
    void on_actionViewRecords_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void processFrame();
    
    // 考勤端专用槽函数
    void on_loadExamRoomsButton_clicked();
    void on_loadStudentsButton_clicked();
    void on_examRoomSelector_currentIndexChanged(int index);
    void onNetworkError(const QString &errorMessage);

private:
    Ui::MainWindow *ui;
    QTimer *timer;
    cv::VideoCapture capture;
    cv::CascadeClassifier faceCascade;
    cv::Mat frame;
    cv::Ptr<cv::face::LBPHFaceRecognizer> model;
    
    QString currentUser;
    QVector<int> faceLabels;
    QVector<QString> faceNames;
    bool capturing;
    int captureCount;
    
    // 考勤端专用成员变量
    QComboBox *examRoomSelector;
    QTableWidget *studentListTable;
    QPushButton *loadExamRoomsButton;
    QPushButton *loadStudentsButton;
    QLabel *examInfoLabel;
    
    QVector<ExamRoom> examRooms;
    QVector<StudentInfo> currentStudents;
    int currentExamRoomId;
    bool isAttendanceMode;
    
    void initDatabase();
    bool loadFaceData();
    void saveFaceData();
    void trainModel();
    void setupFaceDetector();
    void recognizeFace();
    void attendanceRecord(const QString &name);
    void showAttendanceRecords();
    
    // 考勤端专用方法
    void setupExamRoomUI();
    void loadExamRooms();
    void loadStudentList(int examRoomId);
    void updateStudentAttendance(int studentRow, const QString &status);
    void processStudentRecognition(const QString &recognizedName);
    
    // 重写关闭事件
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
