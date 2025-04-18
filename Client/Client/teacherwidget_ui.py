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
from PySide6.QtWidgets import (QApplication, QComboBox, QHBoxLayout, QHeaderView,
    QLabel, QListView, QListWidget, QListWidgetItem,
    QPushButton, QSizePolicy, QSpacerItem, QTabWidget,
    QTableView, QVBoxLayout, QWidget)

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

        self.viewStudentPhotoButton = QPushButton(self.classTab)
        self.viewStudentPhotoButton.setObjectName(u"viewStudentPhotoButton")

        self.studentButtonsLayout.addWidget(self.viewStudentPhotoButton)

        self.requestPhotosButton = QPushButton(self.classTab)
        self.requestPhotosButton.setObjectName(u"requestPhotosButton")

        self.studentButtonsLayout.addWidget(self.requestPhotosButton)

        self.viewAllPhotosButton = QPushButton(self.classTab)
        self.viewAllPhotosButton.setObjectName(u"viewAllPhotosButton")

        self.studentButtonsLayout.addWidget(self.viewAllPhotosButton)


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

        self.examRoomButtonsLayout = QHBoxLayout()
        self.examRoomButtonsLayout.setObjectName(u"examRoomButtonsLayout")
        self.addExamRoomButton = QPushButton(self.examTab)
        self.addExamRoomButton.setObjectName(u"addExamRoomButton")

        self.examRoomButtonsLayout.addWidget(self.addExamRoomButton)

        self.manageSeatsButton = QPushButton(self.examTab)
        self.manageSeatsButton.setObjectName(u"manageSeatsButton")

        self.examRoomButtonsLayout.addWidget(self.manageSeatsButton)


        self.examRoomsLayout.addLayout(self.examRoomButtonsLayout)


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
        self.seatTab = QWidget()
        self.seatTab.setObjectName(u"seatTab")
        self.verticalLayout_2 = QVBoxLayout(self.seatTab)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.seatTopLayout = QHBoxLayout()
        self.seatTopLayout.setObjectName(u"seatTopLayout")
        self.currentExamRoomLabel = QLabel(self.seatTab)
        self.currentExamRoomLabel.setObjectName(u"currentExamRoomLabel")
        self.currentExamRoomLabel.setFont(font1)

        self.seatTopLayout.addWidget(self.currentExamRoomLabel)

        self.horizontalSpacer_3 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.seatTopLayout.addItem(self.horizontalSpacer_3)

        self.linkClassesButton = QPushButton(self.seatTab)
        self.linkClassesButton.setObjectName(u"linkClassesButton")

        self.seatTopLayout.addWidget(self.linkClassesButton)


        self.verticalLayout_2.addLayout(self.seatTopLayout)

        self.seatsMainLayout = QHBoxLayout()
        self.seatsMainLayout.setObjectName(u"seatsMainLayout")
        self.linkedClassesLayout = QVBoxLayout()
        self.linkedClassesLayout.setObjectName(u"linkedClassesLayout")
        self.linkedClassesLabel = QLabel(self.seatTab)
        self.linkedClassesLabel.setObjectName(u"linkedClassesLabel")
        self.linkedClassesLabel.setFont(font1)

        self.linkedClassesLayout.addWidget(self.linkedClassesLabel)

        self.linkedClassesListWidget = QListWidget(self.seatTab)
        self.linkedClassesListWidget.setObjectName(u"linkedClassesListWidget")

        self.linkedClassesLayout.addWidget(self.linkedClassesListWidget)


        self.seatsMainLayout.addLayout(self.linkedClassesLayout)

        self.seatsLayout = QVBoxLayout()
        self.seatsLayout.setObjectName(u"seatsLayout")
        self.seatsLabel = QLabel(self.seatTab)
        self.seatsLabel.setObjectName(u"seatsLabel")
        self.seatsLabel.setFont(font1)

        self.seatsLayout.addWidget(self.seatsLabel)

        self.seatsTableView = QTableView(self.seatTab)
        self.seatsTableView.setObjectName(u"seatsTableView")

        self.seatsLayout.addWidget(self.seatsTableView)


        self.seatsMainLayout.addLayout(self.seatsLayout)


        self.verticalLayout_2.addLayout(self.seatsMainLayout)

        self.seatsOptionsLayout = QHBoxLayout()
        self.seatsOptionsLayout.setObjectName(u"seatsOptionsLayout")
        self.arrangementTypeLabel = QLabel(self.seatTab)
        self.arrangementTypeLabel.setObjectName(u"arrangementTypeLabel")

        self.seatsOptionsLayout.addWidget(self.arrangementTypeLabel)

        self.arrangementTypeComboBox = QComboBox(self.seatTab)
        self.arrangementTypeComboBox.addItem("")
        self.arrangementTypeComboBox.addItem("")
        self.arrangementTypeComboBox.addItem("")
        self.arrangementTypeComboBox.addItem("")
        self.arrangementTypeComboBox.setObjectName(u"arrangementTypeComboBox")

        self.seatsOptionsLayout.addWidget(self.arrangementTypeComboBox)

        self.horizontalSpacer_4 = QSpacerItem(40, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)

        self.seatsOptionsLayout.addItem(self.horizontalSpacer_4)

        self.generateSeatsButton = QPushButton(self.seatTab)
        self.generateSeatsButton.setObjectName(u"generateSeatsButton")

        self.seatsOptionsLayout.addWidget(self.generateSeatsButton)

        self.exportSeatsButton = QPushButton(self.seatTab)
        self.exportSeatsButton.setObjectName(u"exportSeatsButton")

        self.seatsOptionsLayout.addWidget(self.exportSeatsButton)


        self.verticalLayout_2.addLayout(self.seatsOptionsLayout)

        self.tabWidget.addTab(self.seatTab, "")

        self.verticalLayout.addWidget(self.tabWidget)


        self.retranslateUi(TeacherWidget)

        self.tabWidget.setCurrentIndex(2)


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
        self.viewStudentPhotoButton.setText(QCoreApplication.translate("TeacherWidget", u"\u67e5\u770b\u7167\u7247", None))
        self.requestPhotosButton.setText(QCoreApplication.translate("TeacherWidget", u"\u8bf7\u6c42\u7167\u7247", None))
        self.viewAllPhotosButton.setText(QCoreApplication.translate("TeacherWidget", u"\u67e5\u770b\u73ed\u7ea7\u7167\u7247", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.classTab), QCoreApplication.translate("TeacherWidget", u"\u73ed\u7ea7\u7ba1\u7406", None))
        self.examRoomsLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u8003\u573a\u5217\u8868", None))
        self.addExamRoomButton.setText(QCoreApplication.translate("TeacherWidget", u"\u6dfb\u52a0\u8003\u573a", None))
        self.manageSeatsButton.setText(QCoreApplication.translate("TeacherWidget", u"\u7ba1\u7406\u5ea7\u4f4d", None))
        self.attendanceLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u8003\u52e4\u60c5\u51b5", None))
        self.exportAttendanceButton.setText(QCoreApplication.translate("TeacherWidget", u"\u5bfc\u51fa\u8003\u52e4\u6570\u636e", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.examTab), QCoreApplication.translate("TeacherWidget", u"\u8003\u573a\u7ba1\u7406", None))
        self.currentExamRoomLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u5f53\u524d\u8003\u573a: \u672a\u9009\u62e9", None))
        self.linkClassesButton.setText(QCoreApplication.translate("TeacherWidget", u"\u5173\u8054\u73ed\u7ea7", None))
        self.linkedClassesLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u5173\u8054\u73ed\u7ea7", None))
        self.seatsLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u5ea7\u4f4d\u5b89\u6392", None))
        self.arrangementTypeLabel.setText(QCoreApplication.translate("TeacherWidget", u"\u6392\u5ea7\u65b9\u5f0f:", None))
        self.arrangementTypeComboBox.setItemText(0, QCoreApplication.translate("TeacherWidget", u"\u968f\u673a\u6392\u5ea7", None))
        self.arrangementTypeComboBox.setItemText(1, QCoreApplication.translate("TeacherWidget", u"\u6309\u73ed\u7ea7\u6392\u5ea7", None))
        self.arrangementTypeComboBox.setItemText(2, QCoreApplication.translate("TeacherWidget", u"\u6309\u6210\u7ee9\u6392\u5ea7", None))
        self.arrangementTypeComboBox.setItemText(3, QCoreApplication.translate("TeacherWidget", u"\u81ea\u5b9a\u4e49\u6392\u5ea7", None))

        self.generateSeatsButton.setText(QCoreApplication.translate("TeacherWidget", u"\u751f\u6210\u5ea7\u4f4d", None))
        self.exportSeatsButton.setText(QCoreApplication.translate("TeacherWidget", u"\u5bfc\u51fa\u5ea7\u4f4d\u8868", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.seatTab), QCoreApplication.translate("TeacherWidget", u"\u5ea7\u4f4d\u5b89\u6392", None))
    # retranslateUi

