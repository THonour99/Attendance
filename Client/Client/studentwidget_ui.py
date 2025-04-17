# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'studentwidget.ui'
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
from PySide6.QtWidgets import (QApplication, QHBoxLayout, QHeaderView, QLabel,
    QPushButton, QSizePolicy, QSpacerItem, QTabWidget,
    QTableView, QVBoxLayout, QWidget)

class Ui_StudentWidget(object):
    def setupUi(self, StudentWidget):
        if not StudentWidget.objectName():
            StudentWidget.setObjectName(u"StudentWidget")
        StudentWidget.resize(800, 600)
        self.verticalLayout = QVBoxLayout(StudentWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.titleLabel = QLabel(StudentWidget)
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

        self.nameLabel = QLabel(StudentWidget)
        self.nameLabel.setObjectName(u"nameLabel")

        self.infoLayout.addWidget(self.nameLabel)

        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.infoLayout.addItem(self.horizontalSpacer_3)

        self.studentIdLabel = QLabel(StudentWidget)
        self.studentIdLabel.setObjectName(u"studentIdLabel")

        self.infoLayout.addWidget(self.studentIdLabel)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.infoLayout.addItem(self.horizontalSpacer_2)

        self.refreshButton = QPushButton(StudentWidget)
        self.refreshButton.setObjectName(u"refreshButton")

        self.infoLayout.addWidget(self.refreshButton)


        self.verticalLayout.addLayout(self.infoLayout)

        self.tabWidget = QTabWidget(StudentWidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.attendanceTab = QWidget()
        self.attendanceTab.setObjectName(u"attendanceTab")
        self.verticalLayout_2 = QVBoxLayout(self.attendanceTab)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.attendanceTableView = QTableView(self.attendanceTab)
        self.attendanceTableView.setObjectName(u"attendanceTableView")

        self.verticalLayout_2.addWidget(self.attendanceTableView)

        self.tabWidget.addTab(self.attendanceTab, "")
        self.classTab = QWidget()
        self.classTab.setObjectName(u"classTab")
        self.verticalLayout_3 = QVBoxLayout(self.classTab)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.classTableView = QTableView(self.classTab)
        self.classTableView.setObjectName(u"classTableView")

        self.verticalLayout_3.addWidget(self.classTableView)

        self.tabWidget.addTab(self.classTab, "")
        self.photoTab = QWidget()
        self.photoTab.setObjectName(u"photoTab")
        self.verticalLayout_4 = QVBoxLayout(self.photoTab)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.photoStatusLabel = QLabel(self.photoTab)
        self.photoStatusLabel.setObjectName(u"photoStatusLabel")

        self.verticalLayout_4.addWidget(self.photoStatusLabel)

        self.uploadPhotoButton = QPushButton(self.photoTab)
        self.uploadPhotoButton.setObjectName(u"uploadPhotoButton")

        self.verticalLayout_4.addWidget(self.uploadPhotoButton)

        self.photoPreviewLabel = QLabel(self.photoTab)
        self.photoPreviewLabel.setObjectName(u"photoPreviewLabel")
        self.photoPreviewLabel.setAlignment(Qt.AlignCenter)

        self.verticalLayout_4.addWidget(self.photoPreviewLabel)

        self.tabWidget.addTab(self.photoTab, "")
        self.seatTab = QWidget()
        self.seatTab.setObjectName(u"seatTab")
        self.verticalLayout_5 = QVBoxLayout(self.seatTab)
        self.verticalLayout_5.setObjectName(u"verticalLayout_5")
        self.examRoomInfoLabel = QLabel(self.seatTab)
        self.examRoomInfoLabel.setObjectName(u"examRoomInfoLabel")

        self.verticalLayout_5.addWidget(self.examRoomInfoLabel)

        self.seatInfoLabel = QLabel(self.seatTab)
        self.seatInfoLabel.setObjectName(u"seatInfoLabel")

        self.verticalLayout_5.addWidget(self.seatInfoLabel)

        self.tabWidget.addTab(self.seatTab, "")
        self.examInfoTab = QWidget()
        self.examInfoTab.setObjectName(u"examInfoTab")
        self.verticalLayout_6 = QVBoxLayout(self.examInfoTab)
        self.verticalLayout_6.setObjectName(u"verticalLayout_6")
        self.examInfoLabel = QLabel(self.examInfoTab)
        self.examInfoLabel.setObjectName(u"examInfoLabel")

        self.verticalLayout_6.addWidget(self.examInfoLabel)

        self.tabWidget.addTab(self.examInfoTab, "")

        self.verticalLayout.addWidget(self.tabWidget)


        self.retranslateUi(StudentWidget)

        self.tabWidget.setCurrentIndex(4)


        QMetaObject.connectSlotsByName(StudentWidget)
    # setupUi

    def retranslateUi(self, StudentWidget):
        StudentWidget.setWindowTitle(QCoreApplication.translate("StudentWidget", u"\u5b66\u751f\u754c\u9762", None))
        self.titleLabel.setText(QCoreApplication.translate("StudentWidget", u"\u5b66\u751f\u4fe1\u606f\u9762\u677f", None))
        self.nameLabel.setText(QCoreApplication.translate("StudentWidget", u"\u59d3\u540d: ", None))
        self.studentIdLabel.setText(QCoreApplication.translate("StudentWidget", u"\u5b66\u53f7: ", None))
        self.refreshButton.setText(QCoreApplication.translate("StudentWidget", u"\u5237\u65b0\u6570\u636e", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.attendanceTab), QCoreApplication.translate("StudentWidget", u"\u8003\u52e4\u8bb0\u5f55", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.classTab), QCoreApplication.translate("StudentWidget", u"\u73ed\u7ea7\u4fe1\u606f", None))
        self.photoStatusLabel.setText(QCoreApplication.translate("StudentWidget", u"\u7167\u7247\u72b6\u6001: \u672a\u4e0a\u4f20", None))
        self.uploadPhotoButton.setText(QCoreApplication.translate("StudentWidget", u"\u4e0a\u4f20\u7167\u7247", None))
        self.photoPreviewLabel.setText(QCoreApplication.translate("StudentWidget", u"\u7167\u7247\u9884\u89c8", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.photoTab), QCoreApplication.translate("StudentWidget", u"\u7167\u7247\u7ba1\u7406", None))
        self.examRoomInfoLabel.setText(QCoreApplication.translate("StudentWidget", u"\u8003\u573a\u4fe1\u606f", None))
        self.seatInfoLabel.setText(QCoreApplication.translate("StudentWidget", u"\u5ea7\u4f4d\u4fe1\u606f", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.seatTab), QCoreApplication.translate("StudentWidget", u"\u5ea7\u4f4d\u4fe1\u606f", None))
        self.examInfoLabel.setText(QCoreApplication.translate("StudentWidget", u"\u8003\u573a\u4fe1\u606f", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.examInfoTab), QCoreApplication.translate("StudentWidget", u"\u8003\u573a\u4fe1\u606f", None))
    # retranslateUi

