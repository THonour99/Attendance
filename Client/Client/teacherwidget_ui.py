# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'teacherwidget.ui'
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
    QListView, QPushButton, QSizePolicy, QSpacerItem,
    QTabWidget, QTableView, QVBoxLayout, QWidget)

class Ui_TeacherWidget(object):
    def setupUi(self, TeacherWidget):
        if not TeacherWidget.objectName():
            TeacherWidget.setObjectName(u"TeacherWidget")
        TeacherWidget.resize(900, 650)
        self.verticalLayout = QVBoxLayout(TeacherWidget)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.titleLabel = QLabel(TeacherWidget)
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

        self.nameLabel = QLabel(TeacherWidget)
        self.nameLabel.setObjectName(u"nameLabel")

        self.infoLayout.addWidget(self.nameLabel)

        self.horizontalSpacer_2 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.infoLayout.addItem(self.horizontalSpacer_2)

        self.refreshButton = QPushButton(TeacherWidget)
        self.refreshButton.setObjectName(u"refreshButton")

        self.infoLayout.addWidget(self.refreshButton)


        self.verticalLayout.addLayout(self.infoLayout)

        self.tabWidget = QTabWidget(TeacherWidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.classTab = QWidget()
        self.classTab.setObjectName(u"classTab")
        self.horizontalLayout = QHBoxLayout(self.classTab)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.classesLayout = QVBoxLayout()
        self.classesLayout.setObjectName(u"classesLayout")
        self.classesLabel = QLabel(self.classTab)
        self.classesLabel.setObjectName(u"classesLabel")
        font1 = QFont()
        font1.setBold(True)
        self.classesLabel.setFont(font1)

        self.classesLayout.addWidget(self.classesLabel)

        self.classesListView = QListView(self.classTab)
        self.classesListView.setObjectName(u"classesListView")

        self.classesLayout.addWidget(self.classesListView)

        self.addClassButton = QPushButton(self.classTab)
        self.addClassButton.setObjectName(u"addClassButton")

        self.classesLayout.addWidget(self.addClassButton)


        self.horizontalLayout.addLayout(self.classesLayout)

        self.studentsLayout = QVBoxLayout()
        self.studentsLayout.setObjectName(u"studentsLayout")
        self.studentsLabel = QLabel(self.classTab)
        self.studentsLabel.setObjectName(u"studentsLabel")
        self.studentsLabel.setFont(font1)

        self.studentsLayout.addWidget(self.studentsLabel)

        self.studentsTableView = QTableView(self.classTab)
        self.studentsTableView.setObjectName(u"studentsTableView")

        self.studentsLayout.addWidget(self.studentsTableView)

        self.studentButtonsLayout = QHBoxLayout()
        self.studentButtonsLayout.setObjectName(u"studentButtonsLayout")
        self.addStudentButton = QPushButton(self.classTab)
        self.addStudentButton.setObjectName(u"addStudentButton")

        self.studentButtonsLayout.addWidget(self.addStudentButton)

        self.removeStudentButton = QPushButton(self.classTab)
        self.removeStudentButton.setObjectName(u"removeStudentButton")

        self.studentButtonsLayout.addWidget(self.removeStudentButton)


        self.studentsLayout.addLayout(self.studentButtonsLayout)


        self.horizontalLayout.addLayout(self.studentsLayout)

        self.tabWidget.addTab(self.classTab, "")
        self.examTab = QWidget()
        self.examTab.setObjectName(u"examTab")
        self.horizontalLayout_2 = QHBoxLayout(self.examTab)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.examRoomsLayout = QVBoxLayout()
        self.examRoomsLayout.setObjectName(u"examRoomsLayout")
        self.examRoomsLabel = QLabel(self.examTab)
        self.examRoomsLabel.setObjectName(u"examRoomsLabel")
        self.examRoomsLabel.setFont(font1)

        self.examRoomsLayout.addWidget(self.examRoomsLabel)

        self.examRoomsListView = QListView(self.examTab)
        self.examRoomsListView.setObjectName(u"examRoomsListView")

        self.examRoomsLayout.addWidget(self.examRoomsListView)

        self.addExamRoomButton = QPushButton(self.examTab)
        self.addExamRoomButton.setObjectName(u"addExamRoomButton")

        self.examRoomsLayout.addWidget(self.addExamRoomButton)


        self.horizontalLayout_2.addLayout(self.examRoomsLayout)

        self.attendanceLayout = QVBoxLayout()
        self.attendanceLayout.setObjectName(u"attendanceLayout")
        self.attendanceLabel = QLabel(self.examTab)
        self.attendanceLabel.setObjectName(u"attendanceLabel")
        self.attendanceLabel.setFont(font1)

        self.attendanceLayout.addWidget(self.attendanceLabel)

        self.attendanceTableView = QTableView(self.examTab)
        self.attendanceTableView.setObjectName(u"attendanceTableView")

        self.attendanceLayout.addWidget(self.attendanceTableView)

        self.exportAttendanceButton = QPushButton(self.examTab)
        self.exportAttendanceButton.setObjectName(u"exportAttendanceButton")

        self.attendanceLayout.addWidget(self.exportAttendanceButton)


        self.horizontalLayout_2.addLayout(self.attendanceLayout)

        self.tabWidget.addTab(self.examTab, "")

        self.verticalLayout.addWidget(self.tabWidget)


        self.retranslateUi(TeacherWidget)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(TeacherWidget)
    # setupUi

    def retranslateUi(self, TeacherWidget):
        TeacherWidget.setWindowTitle(QCoreApplication.translate("TeacherWidget", u"\u6559\u5e08\u754c\u9762", None))
        self.titleLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u6559\u5e08\u7ba1\u7406\u9762\u677f", None))
        self.nameLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u59d3\u540d: ", None))
        self.refreshButton.setText(QCoreApplication.translate("TeacherWidget", u"\u5237\u65b0\u6570\u636e", None))
        self.classesLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u73ed\u7ea7\u5217\u8868", None))
        self.addClassButton.setText(QCoreApplication.translate("TeacherWidget", u"\u6dfb\u52a0\u73ed\u7ea7", None))
        self.studentsLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u73ed\u7ea7\u5b66\u751f", None))
        self.addStudentButton.setText(QCoreApplication.translate("TeacherWidget", u"\u6dfb\u52a0\u5b66\u751f", None))
        self.removeStudentButton.setText(QCoreApplication.translate("TeacherWidget", u"\u79fb\u9664\u5b66\u751f", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.classTab), QCoreApplication.translate("TeacherWidget", u"\u73ed\u7ea7\u7ba1\u7406", None))
        self.examRoomsLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u8003\u573a\u5217\u8868", None))
        self.addExamRoomButton.setText(QCoreApplication.translate("TeacherWidget", u"\u6dfb\u52a0\u8003\u573a", None))
        self.attendanceLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u8003\u52e4\u60c5\u51b5", None))
        self.exportAttendanceButton.setText(QCoreApplication.translate("TeacherWidget", u"\u5bfc\u51fa\u8003\u52e4\u6570\u636e", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.examTab), QCoreApplication.translate("TeacherWidget", u"\u8003\u573a\u7ba1\u7406", None))
    # retranslateUi

