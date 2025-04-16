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

        self.verticalLayout.addWidget(self.tabWidget)


        self.retranslateUi(StudentWidget)

        self.tabWidget.setCurrentIndex(0)


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
    # retranslateUi

