#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString getUsername() const { return username; }
    QString getServerUrl() const { return serverUrl; }

private slots:
    void onLoginButtonClicked();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *serverUrlEdit;
    QPushButton *loginButton;
    QPushButton *cancelButton;
    
    QString username;
    QString serverUrl;

    void setupUI();
};

#endif // LOGINDIALOG_H 