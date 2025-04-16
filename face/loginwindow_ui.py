# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'loginwindow.ui'
##
## Created by: Qt User Interface Compiler version 6.8.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QApplication, QComboBox, QFormLayout, QHBoxLayout,
    QLabel, QLineEdit, QMainWindow, QPushButton,
    QSizePolicy, QSpacerItem, QStatusBar, QVBoxLayout,
    QWidget)

class Ui_LoginWindow(object):
    def setupUi(self, LoginWindow):
        if not LoginWindow.objectName():
            LoginWindow.setObjectName(u"LoginWindow")
        LoginWindow.resize(400, 350)
        self.centralwidget = QWidget(LoginWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.verticalLayout = QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)

        self.titleLabel = QLabel(self.centralwidget)
        self.titleLabel.setObjectName(u"titleLabel")
        font = QFont()
        font.setPointSize(16)
        font.setBold(True)
        self.titleLabel.setFont(font)
        self.titleLabel.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.titleLabel)

        self.verticalSpacer_2 = QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_2)

        self.formLayout = QFormLayout()
        self.formLayout.setObjectName(u"formLayout")
        self.formLayout.setLabelAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)
        self.formLayout.setFormAlignment(Qt.AlignHCenter|Qt.AlignTop)
        self.serverLabel = QLabel(self.centralwidget)
        self.serverLabel.setObjectName(u"serverLabel")

        self.formLayout.setWidget(0, QFormLayout.LabelRole, self.serverLabel)

        self.serverEdit = QLineEdit(self.centralwidget)
        self.serverEdit.setObjectName(u"serverEdit")
        self.serverEdit.setMinimumSize(QSize(200, 0))

        self.formLayout.setWidget(0, QFormLayout.FieldRole, self.serverEdit)

        self.usernameLabel = QLabel(self.centralwidget)
        self.usernameLabel.setObjectName(u"usernameLabel")

        self.formLayout.setWidget(1, QFormLayout.LabelRole, self.usernameLabel)

        self.usernameEdit = QLineEdit(self.centralwidget)
        self.usernameEdit.setObjectName(u"usernameEdit")
        self.usernameEdit.setMinimumSize(QSize(200, 0))

        self.formLayout.setWidget(1, QFormLayout.FieldRole, self.usernameEdit)

        self.passwordLabel = QLabel(self.centralwidget)
        self.passwordLabel.setObjectName(u"passwordLabel")

        self.formLayout.setWidget(2, QFormLayout.LabelRole, self.passwordLabel)

        self.passwordEdit = QLineEdit(self.centralwidget)
        self.passwordEdit.setObjectName(u"passwordEdit")
        self.passwordEdit.setMinimumSize(QSize(200, 0))
        self.passwordEdit.setEchoMode(QLineEdit.Password)

        self.formLayout.setWidget(2, QFormLayout.FieldRole, self.passwordEdit)

        self.roleComboBox = QComboBox(self.centralwidget)
        self.roleComboBox.addItem("")
        self.roleComboBox.addItem("")
        self.roleComboBox.addItem("")
        self.roleComboBox.setObjectName(u"roleComboBox")
        self.roleComboBox.setMinimumSize(QSize(200, 0))

        self.formLayout.setWidget(3, QFormLayout.FieldRole, self.roleComboBox)

        self.roleLabel = QLabel(self.centralwidget)
        self.roleLabel.setObjectName(u"roleLabel")

        self.formLayout.setWidget(3, QFormLayout.LabelRole, self.roleLabel)


        self.verticalLayout.addLayout(self.formLayout)

        self.verticalSpacer_3 = QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_3)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.loginButton = QPushButton(self.centralwidget)
        self.loginButton.setObjectName(u"loginButton")
        self.loginButton.setMinimumSize(QSize(120, 35))

        self.horizontalLayout.addWidget(self.loginButton)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer_2)


        self.verticalLayout.addLayout(self.horizontalLayout)

        self.verticalSpacer_4 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_4)

        LoginWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QStatusBar(LoginWindow)
        self.statusbar.setObjectName(u"statusbar")
        LoginWindow.setStatusBar(self.statusbar)

        self.retranslateUi(LoginWindow)

        QMetaObject.connectSlotsByName(LoginWindow)
    # setupUi

    def retranslateUi(self, LoginWindow):
        LoginWindow.setWindowTitle(QCoreApplication.translate("LoginWindow", u"\u4eba\u8138\u8bc6\u522b\u6392\u5ea7\u7cfb\u7edf - \u767b\u5f55", None))
        self.titleLabel.setText(QCoreApplication.translate("LoginWindow", u"\u4eba\u8138\u8bc6\u522b\u6392\u5ea7\u7cfb\u7edf", None))
        self.serverLabel.setText(QCoreApplication.translate("LoginWindow", u"\u670d\u52a1\u5668\u5730\u5740\uff1a", None))
        self.serverEdit.setText(QCoreApplication.translate("LoginWindow", u"http://localhost:3000", None))
        self.usernameLabel.setText(QCoreApplication.translate("LoginWindow", u"\u7528\u6237\u540d\uff1a", None))
        self.passwordLabel.setText(QCoreApplication.translate("LoginWindow", u"\u5bc6\u7801\uff1a", None))
        self.roleComboBox.setItemText(0, QCoreApplication.translate("LoginWindow", u"\u5b66\u751f", None))
        self.roleComboBox.setItemText(1, QCoreApplication.translate("LoginWindow", u"\u8001\u5e08", None))
        self.roleComboBox.setItemText(2, QCoreApplication.translate("LoginWindow", u"\u7ba1\u7406\u5458", None))

        self.roleLabel.setText(QCoreApplication.translate("LoginWindow", u"\u89d2\u8272\uff1a", None))
        self.loginButton.setText(QCoreApplication.translate("LoginWindow", u"\u767b\u5f55", None))
    # retranslateUi

