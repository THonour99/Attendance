#ifndef TEACHERWIDGET_H
#define TEACHERWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QAxObject>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QListWidget>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QTimer>
#include <QGridLayout>

namespace Ui {
class TeacherWidget;
}

class TeacherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherWidget(const QJsonObject &userInfo, const QString &token, QWidget *parent = nullptr);
    ~TeacherWidget();

private slots:
    // 班级管理
    void onClassesDataReceived(QNetworkReply *reply);
    void onStudentsDataReceived(QNetworkReply *reply);
    void onClassSelected(const QModelIndex &index);
    void onAddClassClicked();
    void onAddStudentToClassClicked();
    void onRemoveStudentFromClassClicked();
    
    // 考场管理
    void onExamRoomsDataReceived(QNetworkReply *reply);
    void onExamRoomSelected(const QModelIndex &index);
    void onExamAttendanceDataReceived(QNetworkReply *reply);
    void onAddExamRoomClicked();
    void onExportAttendanceClicked();
    
    // 排座功能
    void onManageSeatsClicked();
    void onLinkClassesClicked();
    void onLinkClassButtonClicked();
    void onClassesLinkedReceived(QNetworkReply *reply);
    void onGenerateSeatsClicked();
    void onSeatArrangementTypeChanged(int index);
    void onExportSeatsClicked();
    
    // 照片管理
    void onViewStudentPhotoClicked();
    void onStudentPhotoReceived(QNetworkReply *reply);
    void onRequestStudentPhotosClicked();
    void onViewStudentPhotosClicked();
    
    // 刷新数据
    void onRefreshButtonClicked();

private:
    Ui::TeacherWidget *ui;
    QJsonObject userInfo;
    QString token;
    QNetworkAccessManager *networkManager;
    
    QStandardItemModel *classesModel;
    QStandardItemModel *studentsModel;
    QStandardItemModel *examRoomsModel;
    QStandardItemModel *attendanceModel;
    QStandardItemModel *linkedClassesModel;
    QStandardItemModel *seatsModel;
    
    QString currentClassId;
    QString currentExamRoomId;
    QString currentStudentId;

    void setupUI();
    void loadClassesData();
    void loadExamRoomsData();
    void loadStudentsInClass(const QString &classId);
    void loadExamAttendance(const QString &examRoomId);
    void loadLinkedClasses(const QString &examRoomId);
    void loadExamSeats(const QString &examRoomId);
    
    // 导出辅助函数
    bool exportToExcel(const QString &fileName, QStandardItemModel *model, const QString &sheetName);
    bool exportToCSV(const QString &fileName, QStandardItemModel *model);
};

#endif // TEACHERWIDGET_H 