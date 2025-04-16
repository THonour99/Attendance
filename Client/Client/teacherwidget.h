#ifndef TEACHERWIDGET_H
#define TEACHERWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardItemModel>

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
    
    QString currentClassId;
    QString currentExamRoomId;

    void setupUI();
    void loadClassesData();
    void loadExamRoomsData();
    void loadStudentsInClass(const QString &classId);
    void loadExamAttendance(const QString &examRoomId);
};

#endif // TEACHERWIDGET_H 