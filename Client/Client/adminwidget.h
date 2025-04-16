#ifndef ADMINWIDGET_H
#define ADMINWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStandardItemModel>
#include <QTimer>

namespace Ui {
class AdminWidget;
}

class AdminWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AdminWidget(const QJsonObject &userInfo, const QString &token, QWidget *parent = nullptr);
    ~AdminWidget();

private slots:
    // 用户管理
    void onUsersDataReceived(QNetworkReply *reply);
    void onAddUserClicked();
    void onEditUserClicked();
    void onDeleteUserClicked();
    
    // 数据库管理
    void onBackupDatabaseClicked();
    void onRestoreDatabaseClicked();
    
    // 刷新数据
    void onRefreshButtonClicked();
    void refreshToken();
    void onTokenRefreshed(QNetworkReply *reply);

private:
    Ui::AdminWidget *ui;
    QJsonObject userInfo;
    QString token;
    QNetworkAccessManager *networkManager;
    
    QStandardItemModel *usersModel;
    QTimer *tokenRefreshTimer;

    void setupUI();
    void loadUsersData();
};

#endif // ADMINWIDGET_H 