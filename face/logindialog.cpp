#include "logindialog.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include "networkmanager.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("考勤端登录");
    setMinimumWidth(350);
    
    setupUI();
    
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginButtonClicked);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    // 创建表单控件
    QLabel *titleLabel = new QLabel("排座系统考勤端");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("请输入用户名");
    usernameEdit->setText("operator"); // 默认用户名
    
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setText("operator123"); // 默认密码
    
    serverUrlEdit = new QLineEdit(this);
    serverUrlEdit->setPlaceholderText("服务器地址");
    serverUrlEdit->setText("http://localhost:8080"); // 默认服务器地址
    
    loginButton = new QPushButton("登录", this);
    loginButton->setDefault(true);
    
    cancelButton = new QPushButton("取消", this);
    
    // 创建布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel);
    
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("用户名:", usernameEdit);
    formLayout->addRow("密码:", passwordEdit);
    formLayout->addRow("服务器:", serverUrlEdit);
    mainLayout->addLayout(formLayout);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    setLayout(mainLayout);
}

void LoginDialog::onLoginButtonClicked()
{
    username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    serverUrl = serverUrlEdit->text().trimmed();
    
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "登录失败", "用户名和密码不能为空");
        return;
    }
    
    if (serverUrl.isEmpty()) {
        QMessageBox::warning(this, "登录失败", "请输入服务器地址");
        return;
    }
    
    // 设置服务器地址
    NetworkManager::getInstance().setServerAddress(serverUrl);
    
    // 显示正在登录提示
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    // 尝试登录
    bool loginSuccess = NetworkManager::getInstance().login(username, password);
    
    // 恢复鼠标
    QApplication::restoreOverrideCursor();
    
    if (loginSuccess) {
        accept(); // 关闭登录对话框，返回接受结果
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误，或服务器连接失败");
    }
} 