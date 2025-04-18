#ifndef STUDENTWIDGET_H
#define STUDENTWIDGET_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QFileDialog>
#include <QPixmap>
#include <QTimer>
#include <QHttpMultiPart>

namespace Ui {
class StudentWidget;
}

class StudentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StudentWidget(QWidget *parent = nullptr);
    ~StudentWidget();

    void setUserInfo(const QString& name, const QString& studentId, const QString& token);

signals:
    void logout(); // 添加登出信号

private slots:
    void onRefreshButtonClicked();
    void onUploadPhotoButtonClicked();
    void onAttendanceDataReceived(QNetworkReply *reply);
    void onClassDataReceived(QNetworkReply *reply);
    void onPhotoUploadResponse(QNetworkReply *reply);
    void onSeatInfoReceived(QNetworkReply *reply);
    void onExamInfoReceived(QNetworkReply *reply);
    void onLogoutButtonClicked();
    void onNotificationButtonClicked();
    
    // 通知系统相关槽
    void checkNotifications();
    void processNotifications(const QJsonArray &notifications);
    void onPhotoRequestNotificationReceived(const QJsonObject &notification);
    void markNotificationAsRead(qint64 notificationId);
    void updateNotificationUI(const QJsonArray &notifications);
    
    void updateUnreadNotificationsCount();

private:
    Ui::StudentWidget *ui;
    QNetworkAccessManager *networkManager;
    QString userToken;
    QString userName;
    QString studentId;
    bool hasRetryLoadSeat = false; // 座位信息重试标志
    QString currentPhotoUrl;       // 当前照片URL
    
    // 通知系统相关
    QTimer *notificationTimer;
    void setupNotificationSystem();

    void setupUI();
    void loadAttendanceRecords();
    void loadClassInfo();
    void loadSeatInfo();
    void loadExamInfo();
    void loadUserInfo();
    void updatePhotoStatus(const QString& status);
    void displayPhoto(const QPixmap& photo);
    
    // 工具函数，确保照片URL是完整的
    QString ensureFullPhotoUrl(const QString &photoUrl);
};

#endif // STUDENTWIDGET_H 