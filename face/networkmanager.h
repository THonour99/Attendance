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

// 考场信息结构体
struct ExamRoom {
    int id;
    QString name;
    QString location;
    int capacity;
    QString examDate;
    QString examTime;
};

// 学生信息结构体
struct StudentInfo {
    int id;
    QString studentId; // 学号
    QString name;
    int seatNumber;
    QString attendanceStatus;
};

class NetworkManager : public QObject
{
    Q_OBJECT
    
public:
    static NetworkManager& getInstance();
    void setServerAddress(const QString &url) { serverUrl = url; }
    
    bool login(const QString &username, const QString &password);
    QVector<ExamRoom> getExamRooms();
    QVector<StudentInfo> getStudentsForExam(int examRoomId);
    bool uploadAttendanceRecord(int studentId, int examRoomId, const QDateTime &time);
    
signals:
    void networkError(const QString &errorMessage);
    
private:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();
    
    static NetworkManager *m_instance;
    
    QNetworkAccessManager *manager;
    QString serverUrl;
    QString authToken;
};

#endif // NETWORKMANAGER_H 