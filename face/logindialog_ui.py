# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'logindialog.ui'
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
from PySide6.QtWidgets import (QApplication, QDialog, QFormLayout, QHBoxLayout,
    QLabel, QLineEdit, QPushButton, QSizePolicy,
    QSpacerItem, QVBoxLayout, QWidget)

class Ui_LoginDialog(object):
    def setupUi(self, LoginDialog):
        if not LoginDialog.objectName():
            LoginDialog.setObjectName(u"LoginDialog")
        LoginDialog.resize(400, 300)
        self.verticalLayout = QVBoxLayout(LoginDialog)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.titleLabel = QLabel(LoginDialog)
        self.titleLabel.setObjectName(u"titleLabel")
        font = QFont()
        font.setPointSize(14)
        font.setBold(True)
        self.titleLabel.setFont(font)
        self.titleLabel.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.titleLabel)

        self.verticalSpacer = QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)

        self.formLayout = QFormLayout()
        self.formLayout.setObjectName(u"formLayout")
        self.serverLabel = QLabel(LoginDialog)
        self.serverLabel.setObjectName(u"serverLabel")

        self.formLayout.setWidget(0, QFormLayout.LabelRole, self.serverLabel)

        self.serverEdit = QLineEdit(LoginDialog)
        self.serverEdit.setObjectName(u"serverEdit")

        self.formLayout.setWidget(0, QFormLayout.FieldRole, self.serverEdit)

        self.usernameLabel = QLabel(LoginDialog)
        self.usernameLabel.setObjectName(u"usernameLabel")

        self.formLayout.setWidget(1, QFormLayout.LabelRole, self.usernameLabel)

        self.usernameEdit = QLineEdit(LoginDialog)
        self.usernameEdit.setObjectName(u"usernameEdit")

        self.formLayout.setWidget(1, QFormLayout.FieldRole, self.usernameEdit)

        self.passwordLabel = QLabel(LoginDialog)
        self.passwordLabel.setObjectName(u"passwordLabel")

        self.formLayout.setWidget(2, QFormLayout.LabelRole, self.passwordLabel)

        self.passwordEdit = QLineEdit(LoginDialog)
        self.passwordEdit.setObjectName(u"passwordEdit")
        self.passwordEdit.setEchoMode(QLineEdit.Password)

        self.formLayout.setWidget(2, QFormLayout.FieldRole, self.passwordEdit)


        self.verticalLayout.addLayout(self.formLayout)

        self.verticalSpacer_2 = QSpacerItem(20, 20, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer_2)

        self.horizontalLayout = QHBoxLayout()
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.horizontalLayout.addItem(self.horizontalSpacer)

        self.loginButton = QPushButton(LoginDialog)
        self.loginButton.setObjectName(u"loginButton")

        self.horizontalLayout.addWidget(self.loginButton)

        self.cancelButton = QPushButton(LoginDialog)
        self.cancelButton.setObjectName(u"cancelButton")

        self.horizontalLayout.addWidget(self.cancelButton)


        self.verticalLayout.addLayout(self.horizontalLayout)


        self.retranslateUi(LoginDialog)

        QMetaObject.connectSlotsByName(LoginDialog)
    # setupUi

    def retranslateUi(self, LoginDialog):
        LoginDialog.setWindowTitle(QCoreApplication.translate("LoginDialog", u"\u767b\u5f55", None))
        self.titleLabel.setText(QCoreApplication.translate("LoginDialog", u"\u4eba\u8138\u8bc6\u522b\u6392\u5ea7\u7cfb\u7edf - \u767b\u5f55", None))
        self.serverLabel.setText(QCoreApplication.translate("LoginDialog", u"\u670d\u52a1\u5668\u5730\u5740\uff1a", None))
        self.serverEdit.setText(QCoreApplication.translate("LoginDialog", u"http://localhost:3000", None))
        self.usernameLabel.setText(QCoreApplication.translate("LoginDialog", u"\u7528\u6237\u540d\uff1a", None))
        self.passwordLabel.setText(QCoreApplication.translate("LoginDialog", u"\u5bc6\u7801\uff1a", None))
        self.loginButton.setText(QCoreApplication.translate("LoginDialog", u"\u767b\u5f55", None))
        self.cancelButton.setText(QCoreApplication.translate("LoginDialog", u"\u53d6\u6d88", None))
    # retranslateUi

