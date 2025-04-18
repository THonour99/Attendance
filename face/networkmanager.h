#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QVector>
#include <QUrl>
#include <QMessageBox>
#include <QHttpMultiPart>

// 考场信息结构体
struct ExamRoom {
    int id;
    QString name;
    QString location;
    int capacity;
    QString examDate;
    QString examTime;
    QString teacherId;  // 新增：负责教师ID
};

// 学生信息结构体
struct StudentInfo {
    int id;
    QString studentId; // 学号
    QString name;
    int seatNumber;
    QString attendanceStatus;
};

// 回调函数类型定义
typedef std::function<void(QJsonObject)> NetworkCallback;

class NetworkManager : public QObject
{
    Q_OBJECT
    
public:
    static NetworkManager& getInstance();
    void setServerAddress(const QString &url) { serverUrl = url; }
    
    bool login(const QString &username, const QString &password);
    QVector<ExamRoom> getExamRooms();  // 移除teacherId参数
    QVector<StudentInfo> getStudentsForExam(int examRoomId);
    bool uploadAttendanceRecord(int studentId, int examRoomId, const QDateTime &time);
    
    // 获取当前登录用户名
    QString getLoggedInUsername() const { return loggedInUsername; }
    QString getLoggedInUserId() const { return loggedInUserId; }
    
    // 获取学生通知
    void getNotifications(NetworkCallback callback);
    
    // 标记通知为已读
    void markNotificationAsRead(int notificationId, NetworkCallback callback);
    
    // 上传学生照片
    void uploadStudentPhoto(const QByteArray& photoData, NetworkCallback callback);
    
    // 上传考勤记录（带回调）
    void uploadAttendanceRecord(int examRoomId, const QString& studentId, bool isPresent, const QString& photoUrl, NetworkCallback callback);
    
    // 辅助函数：确保照片URL是完整的
    QString ensureFullPhotoUrl(const QString &photoUrl);
    
    // 发送消息功能
    void sendMessage(int examRoomId, const QString &message, NetworkCallback callback);
    
    // 获取考场聊天记录
    void getChatMessages(int examRoomId, NetworkCallback callback);
    
signals:
    void networkError(const QString &errorMessage);
    
private:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();
    
    // 获取用户信息
    void getUserInfo(const QString &username);
    
    // 发送更新通知
    void sendUpdateNotification(int examRoomId);
    
    static NetworkManager *m_instance;
    
    QNetworkAccessManager *manager;
    QString serverUrl;
    QString authToken;
    QString loggedInUsername;
    QString loggedInUserId;
};

#endif // NETWORKMANAGER_H 