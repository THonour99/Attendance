# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'adminwidget.ui'
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
from PySide6.QtWidgets import (QApplication, QGroupBox, QHBoxLayout, QHeaderView,
    QLabel, QPushButton, QSizePolicy, QSpacerItem,
    QTabWidget, QTableView, QVBoxLayout, QWidget)

class Ui_AdminWidget(object):
    def setupUi(self, AdminWidget):
        if not AdminWidget.objectName():
            AdminWidget.setObjectName(u"AdminWidget")
        AdminWidget.resize(850, 600)
        self.verticalLayout = QVBoxLayout(AdminWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.titleLabel = QLabel(AdminWidget)
        self.titleLabel.setObjectName(u"titleLabel")
        font = QFont()
        font.setPointSize(14)
        font.setBold(True)
        self.titleLabel.setFont(font)
        self.titleLabel.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.titleLabel)

        self.infoLayout = QHBoxLayout()
        self.infoLayout.setObjectName(u"infoLayout")
        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.infoLayout.addItem(self.horizontalSpacer)

        self.nameLabel = QLabel(AdminWidget)
        self.nameLabel.setObjectName(u"nameLabel")

        self.infoLayout.addWidget(self.nameLabel)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.infoLayout.addItem(self.horizontalSpacer_2)

        self.refreshButton = QPushButton(AdminWidget)
        self.refreshButton.setObjectName(u"refreshButton")

        self.infoLayout.addWidget(self.refreshButton)


        self.verticalLayout.addLayout(self.infoLayout)

        self.tabWidget = QTabWidget(AdminWidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.userTab = QWidget()
        self.userTab.setObjectName(u"userTab")
        self.verticalLayout_2 = QVBoxLayout(self.userTab)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.usersTableView = QTableView(self.userTab)
        self.usersTableView.setObjectName(u"usersTableView")

        self.verticalLayout_2.addWidget(self.usersTableView)

        self.userButtonsLayout = QHBoxLayout()
        self.userButtonsLayout.setObjectName(u"userButtonsLayout")
        self.addUserButton = QPushButton(self.userTab)
        self.addUserButton.setObjectName(u"addUserButton")

        self.userButtonsLayout.addWidget(self.addUserButton)

        self.editUserButton = QPushButton(self.userTab)
        self.editUserButton.setObjectName(u"editUserButton")

        self.userButtonsLayout.addWidget(self.editUserButton)

        self.deleteUserButton = QPushButton(self.userTab)
        self.deleteUserButton.setObjectName(u"deleteUserButton")

        self.userButtonsLayout.addWidget(self.deleteUserButton)


        self.verticalLayout_2.addLayout(self.userButtonsLayout)

        self.tabWidget.addTab(self.userTab, "")
        self.databaseTab = QWidget()
        self.databaseTab.setObjectName(u"databaseTab")
        self.verticalLayout_3 = QVBoxLayout(self.databaseTab)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_3.addItem(self.verticalSpacer)

        self.databaseGroupBox = QGroupBox(self.databaseTab)
        self.databaseGroupBox.setObjectName(u"databaseGroupBox")
        self.verticalLayout_4 = QVBoxLayout(self.databaseGroupBox)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.backupLabel = QLabel(self.databaseGroupBox)
        self.backupLabel.setObjectName(u"backupLabel")
        self.backupLabel.setWordWrap(True)

        self.verticalLayout_4.addWidget(self.backupLabel)

        self.backupButton = QPushButton(self.databaseGroupBox)
        self.backupButton.setObjectName(u"backupButton")

        self.verticalLayout_4.addWidget(self.backupButton)

        self.verticalSpacer_3 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_4.addItem(self.verticalSpacer_3)

        self.restoreLabel = QLabel(self.databaseGroupBox)
        self.restoreLabel.setObjectName(u"restoreLabel")
        self.restoreLabel.setWordWrap(True)

        self.verticalLayout_4.addWidget(self.restoreLabel)

        self.restoreButton = QPushButton(self.databaseGroupBox)
        self.restoreButton.setObjectName(u"restoreButton")

        self.verticalLayout_4.addWidget(self.restoreButton)


        self.verticalLayout_3.addWidget(self.databaseGroupBox)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout_3.addItem(self.verticalSpacer_2)

        self.tabWidget.addTab(self.databaseTab, "")

        self.verticalLayout.addWidget(self.tabWidget)


        self.retranslateUi(AdminWidget)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(AdminWidget)
    # setupUi

    def retranslateUi(self, AdminWidget):
        AdminWidget.setWindowTitle(QCoreApplication.translate("AdminWidget", u"\u7ba1\u7406\u5458\u754c\u9762", None))
        self.titleLabel.setText(QCoreApplication.translate("AdminWidget", u"\u7cfb\u7edf\u7ba1\u7406\u9762\u677f", None))
        self.nameLabel.setText(QCoreApplication.translate("AdminWidget", u"\u7ba1\u7406\u5458: ", None))
        self.refreshButton.setText(QCoreApplication.translate("AdminWidget", u"\u5237\u65b0\u6570\u636e", None))
        self.addUserButton.setText(QCoreApplication.translate("AdminWidget", u"\u6dfb\u52a0\u7528\u6237", None))
        self.editUserButton.setText(QCoreApplication.translate("AdminWidget", u"\u7f16\u8f91\u7528\u6237", None))
        self.deleteUserButton.setText(QCoreApplication.translate("AdminWidget", u"\u5220\u9664\u7528\u6237", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.userTab), QCoreApplication.translate("AdminWidget", u"\u7528\u6237\u7ba1\u7406", None))
        self.databaseGroupBox.setTitle(QCoreApplication.translate("AdminWidget", u"\u6570\u636e\u5e93\u64cd\u4f5c", None))
        self.backupLabel.setText(QCoreApplication.translate("AdminWidget", u"\u5907\u4efd\u6570\u636e\u5e93\u53ef\u4ee5\u4fdd\u5b58\u5f53\u524d\u7cfb\u7edf\u7684\u6240\u6709\u6570\u636e\u4e3aSQL\u6587\u4ef6\uff0c\u7528\u4e8e\u7cfb\u7edf\u8fc1\u79fb\u6216\u6570\u636e\u4fdd\u62a4\u3002", None))
        self.backupButton.setText(QCoreApplication.translate("AdminWidget", u"\u5907\u4efd\u6570\u636e\u5e93", None))
        self.restoreLabel.setText(QCoreApplication.translate("AdminWidget", u"\u6062\u590d\u6570\u636e\u5e93\u5c06\u4f7f\u7528\u5907\u4efd\u6587\u4ef6\u8986\u76d6\u5f53\u524d\u6240\u6709\u6570\u636e\uff0c\u6b64\u64cd\u4f5c\u4e0d\u53ef\u64a4\u9500\uff0c\u8bf7\u8c28\u614e\u64cd\u4f5c\u3002", None))
        self.restoreButton.setText(QCoreApplication.translate("AdminWidget", u"\u6062\u590d\u6570\u636e\u5e93", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.databaseTab), QCoreApplication.translate("AdminWidget", u"\u6570\u636e\u5e93\u7ba1\u7406", None))
    # retranslateUi

