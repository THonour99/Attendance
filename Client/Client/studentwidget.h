#ifndef STUDENTWIDGET_H
#define STUDENTWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Ui {
class StudentWidget;
}

class StudentWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StudentWidget(const QJsonObject &userInfo, const QString &token, QWidget *parent = nullptr);
    ~StudentWidget();

private slots:
    void onAttendanceDataReceived(QNetworkReply *reply);
    void onClassInfoReceived(QNetworkReply *reply);
    void onRefreshButtonClicked();

private:
    Ui::StudentWidget *ui;
    QJsonObject userInfo;
    QString token;
    QNetworkAccessManager *networkManager;

    void loadAttendanceRecords();
    void loadClassInfo();
    void setupUI();
};

#endif // STUDENTWIDGET_H 