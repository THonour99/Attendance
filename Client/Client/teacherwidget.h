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
#include <QFormLayout>
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
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QLineEdit>
#include <QSet>

namespace Ui {
class TeacherWidget;
}

class TeacherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherWidget(const QJsonObject &userInfo, const QString &token, QWidget *parent = nullptr);
    ~TeacherWidget();

    void setUserInfo(const QString& username, const QString& token, const QString& userId);

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
    void onViewStudentPhotosFinished(QNetworkReply *reply);
    
    // 刷新数据
    void onRefreshButtonClicked();

    void onLogoutButtonClicked();
    void onRefreshClassesButtonClicked();
    void onAddClassButtonClicked();
    void onEditClassButtonClicked();
    void onDeleteClassButtonClicked();
    void onStudentDataReceived(QNetworkReply* reply);
    void onAddStudentButtonClicked();
    void onEditStudentButtonClicked();
    void onDeleteStudentButtonClicked();
    void onExportButtonClicked();
    void onSearchButtonClicked();
    void onSearchTextChanged(const QString& text);
    
    // 照片刷新相关
    void setupClassPhotoRefresh();
    void refreshClassPhotosStatus();

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
    QString username;
    QString userId;
    QTimer *photoRefreshTimer;
    QSet<QNetworkReply*> repliesBeingProcessed;

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

    void initUI();
    void loadClasses();
    void loadStudentsForClass(const QString& classId);
    void clearStudentTable();
    void showMessage(const QString& message, bool isError = false);
    QJsonObject parseResponse(QNetworkReply* reply, bool showError = true);

    QStandardItemModel* generateAttendanceModel(QJsonArray attendanceData);
    void showSelectedStudentList(QStandardItemModel* model);
    QString ensureFullPhotoUrl(const QString &photoUrl);
};

#endif // TEACHERWIDGET_H 