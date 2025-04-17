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

private slots:
    void onRefreshButtonClicked();
    void onUploadPhotoButtonClicked();
    void onAttendanceDataReceived(QNetworkReply *reply);
    void onClassDataReceived(QNetworkReply *reply);
    void onPhotoUploadResponse(QNetworkReply *reply);
    void onSeatInfoReceived(QNetworkReply *reply);
    void onExamInfoReceived(QNetworkReply *reply);

private:
    Ui::StudentWidget *ui;
    QNetworkAccessManager *networkManager;
    QString userToken;
    QString userName;
    QString studentId;

    void setupUI();
    void loadAttendanceRecords();
    void loadClassInfo();
    void loadSeatInfo();
    void loadExamInfo();
    void updatePhotoStatus(const QString& status);
    void displayPhoto(const QPixmap& photo);
};

#endif // STUDENTWIDGET_H 