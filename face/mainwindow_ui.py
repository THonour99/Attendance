# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'mainwindow.ui'
##
## Created by: Qt User Interface Compiler version 6.8.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QAbstractItemView, QApplication, QCheckBox, QComboBox,
    QFormLayout, QFrame, QGroupBox, QHBoxLayout,
    QHeaderView, QLabel, QLineEdit, QMainWindow,
    QMenu, QMenuBar, QPushButton, QSizePolicy,
    QSpacerItem, QStatusBar, QTabWidget, QTableWidget,
    QTableWidgetItem, QTextEdit, QVBoxLayout, QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(1200, 800)
        self.actionExit = QAction(MainWindow)
        self.actionExit.setObjectName(u"actionExit")
        self.actionAbout = QAction(MainWindow)
        self.actionAbout.setObjectName(u"actionAbout")
        self.actionViewRecords = QAction(MainWindow)
        self.actionViewRecords.setObjectName(u"actionViewRecords")
        self.actionSync = QAction(MainWindow)
        self.actionSync.setObjectName(u"actionSync")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.horizontalLayout_2 = QHBoxLayout(self.centralwidget)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.leftLayout = QVBoxLayout()
        self.leftLayout.setObjectName(u"leftLayout")
        self.cameraLabel = QLabel(self.centralwidget)
        self.cameraLabel.setObjectName(u"cameraLabel")
        self.cameraLabel.setMinimumSize(QSize(640, 480))
        self.cameraLabel.setFrameShape(QFrame.Box)
        self.cameraLabel.setAlignment(Qt.AlignCenter)

        self.leftLayout.addWidget(self.cameraLabel)

        self.logEdit = QTextEdit(self.centralwidget)
        self.logEdit.setObjectName(u"logEdit")
        self.logEdit.setMinimumSize(QSize(0, 150))
        self.logEdit.setReadOnly(True)

        self.leftLayout.addWidget(self.logEdit)


        self.horizontalLayout_2.addLayout(self.leftLayout)

        self.tabWidget = QTabWidget(self.centralwidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.tabWidget.setMinimumSize(QSize(400, 0))
        self.examRoomTab = QWidget()
        self.examRoomTab.setObjectName(u"examRoomTab")
        self.verticalLayout_3 = QVBoxLayout(self.examRoomTab)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.examRoomGroup = QGroupBox(self.examRoomTab)
        self.examRoomGroup.setObjectName(u"examRoomGroup")
        self.verticalLayout_4 = QVBoxLayout(self.examRoomGroup)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.horizontalLayout_3 = QHBoxLayout()
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.examRoomLabel = QLabel(self.examRoomGroup)
        self.examRoomLabel.setObjectName(u"examRoomLabel")

        self.horizontalLayout_3.addWidget(self.examRoomLabel)

        self.examRoomSelector = QComboBox(self.examRoomGroup)
        self.examRoomSelector.setObjectName(u"examRoomSelector")
        self.examRoomSelector.setMinimumSize(QSize(150, 0))

        self.horizontalLayout_3.addWidget(self.examRoomSelector)

        self.loadExamRoomsButton = QPushButton(self.examRoomGroup)
        self.loadExamRoomsButton.setObjectName(u"loadExamRoomsButton")

        self.horizontalLayout_3.addWidget(self.loadExamRoomsButton)


        self.verticalLayout_4.addLayout(self.horizontalLayout_3)

        self.examInfoLabel = QLabel(self.examRoomGroup)
        self.examInfoLabel.setObjectName(u"examInfoLabel")

        self.verticalLayout_4.addWidget(self.examInfoLabel)

        self.loadStudentsButton = QPushButton(self.examRoomGroup)
        self.loadStudentsButton.setObjectName(u"loadStudentsButton")

        self.verticalLayout_4.addWidget(self.loadStudentsButton)


        self.verticalLayout_3.addWidget(self.examRoomGroup)

        self.studentListTable = QTableWidget(self.examRoomTab)
        if (self.studentListTable.columnCount() < 4):
            self.studentListTable.setColumnCount(4)
        __qtablewidgetitem = QTableWidgetItem()
        self.studentListTable.setHorizontalHeaderItem(0, __qtablewidgetitem)
        __qtablewidgetitem1 = QTableWidgetItem()
        self.studentListTable.setHorizontalHeaderItem(1, __qtablewidgetitem1)
        __qtablewidgetitem2 = QTableWidgetItem()
        self.studentListTable.setHorizontalHeaderItem(2, __qtablewidgetitem2)
        __qtablewidgetitem3 = QTableWidgetItem()
        self.studentListTable.setHorizontalHeaderItem(3, __qtablewidgetitem3)
        self.studentListTable.setObjectName(u"studentListTable")
        self.studentListTable.setAlternatingRowColors(True)
        self.studentListTable.setSelectionMode(QAbstractItemView.SingleSelection)
        self.studentListTable.setSelectionBehavior(QAbstractItemView.SelectRows)
        self.studentListTable.horizontalHeader().setStretchLastSection(True)

        self.verticalLayout_3.addWidget(self.studentListTable)

        self.attendanceModeCheckBox = QCheckBox(self.examRoomTab)
        self.attendanceModeCheckBox.setObjectName(u"attendanceModeCheckBox")

        self.verticalLayout_3.addWidget(self.attendanceModeCheckBox)

        self.tabWidget.addTab(self.examRoomTab, "")
        self.registerTab = QWidget()
        self.registerTab.setObjectName(u"registerTab")
        self.verticalLayout = QVBoxLayout(self.registerTab)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.registerGroup = QGroupBox(self.registerTab)
        self.registerGroup.setObjectName(u"registerGroup")
        self.formLayout = QFormLayout(self.registerGroup)
        self.formLayout.setObjectName(u"formLayout")
        self.nameLabel = QLabel(self.registerGroup)
        self.nameLabel.setObjectName(u"nameLabel")

        self.formLayout.setWidget(0, QFormLayout.LabelRole, self.nameLabel)

        self.nameEdit = QLineEdit(self.registerGroup)
        self.nameEdit.setObjectName(u"nameEdit")

        self.formLayout.setWidget(0, QFormLayout.FieldRole, self.nameEdit)

        self.idLabel = QLabel(self.registerGroup)
        self.idLabel.setObjectName(u"idLabel")

        self.formLayout.setWidget(1, QFormLayout.LabelRole, self.idLabel)

        self.idEdit = QLineEdit(self.registerGroup)
        self.idEdit.setObjectName(u"idEdit")

        self.formLayout.setWidget(1, QFormLayout.FieldRole, self.idEdit)

        self.registerButton = QPushButton(self.registerGroup)
        self.registerButton.setObjectName(u"registerButton")
        self.registerButton.setMinimumSize(QSize(0, 30))
        icon = QIcon(QIcon.fromTheme(u"user-add"))
        self.registerButton.setIcon(icon)

        self.formLayout.setWidget(2, QFormLayout.FieldRole, self.registerButton)


        self.verticalLayout.addWidget(self.registerGroup)

        self.faceGroup = QGroupBox(self.registerTab)
        self.faceGroup.setObjectName(u"faceGroup")
        self.verticalLayout_2 = QVBoxLayout(self.faceGroup)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.captureButton = QPushButton(self.faceGroup)
        self.captureButton.setObjectName(u"captureButton")
        self.captureButton.setMinimumSize(QSize(0, 40))
        icon1 = QIcon(QIcon.fromTheme(u"camera"))
        self.captureButton.setIcon(icon1)

        self.verticalLayout_2.addWidget(self.captureButton)

        self.recognizeButton = QPushButton(self.faceGroup)
        self.recognizeButton.setObjectName(u"recognizeButton")
        self.recognizeButton.setMinimumSize(QSize(0, 40))
        icon2 = QIcon(QIcon.fromTheme(u"face-detection"))
        self.recognizeButton.setIcon(icon2)

        self.verticalLayout_2.addWidget(self.recognizeButton)

        self.attendanceButton = QPushButton(self.faceGroup)
        self.attendanceButton.setObjectName(u"attendanceButton")
        self.attendanceButton.setMinimumSize(QSize(0, 40))
        icon3 = QIcon(QIcon.fromTheme(u"check"))
        self.attendanceButton.setIcon(icon3)

        self.verticalLayout_2.addWidget(self.attendanceButton)

        self.viewRecordsButton = QPushButton(self.faceGroup)
        self.viewRecordsButton.setObjectName(u"viewRecordsButton")
        self.viewRecordsButton.setMinimumSize(QSize(0, 40))
        icon4 = QIcon(QIcon.fromTheme(u"document-open"))
        self.viewRecordsButton.setIcon(icon4)

        self.verticalLayout_2.addWidget(self.viewRecordsButton)


        self.verticalLayout.addWidget(self.faceGroup)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Policy.Minimum, QSizePolicy.Policy.Expanding)

        self.verticalLayout.addItem(self.verticalSpacer)

        self.tabWidget.addTab(self.registerTab, "")

        self.horizontalLayout_2.addWidget(self.tabWidget)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 1200, 21))
        self.menu = QMenu(self.menubar)
        self.menu.setObjectName(u"menu")
        self.menu_2 = QMenu(self.menubar)
        self.menu_2.setObjectName(u"menu_2")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menu.menuAction())
        self.menubar.addAction(self.menu_2.menuAction())
        self.menu.addAction(self.actionSync)
        self.menu.addAction(self.actionViewRecords)
        self.menu.addSeparator()
        self.menu.addAction(self.actionExit)
        self.menu_2.addAction(self.actionAbout)

        self.retranslateUi(MainWindow)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"\u4eba\u8138\u8bc6\u522b\u6392\u5ea7\u7cfb\u7edf - \u8003\u52e4\u7aef", None))
        self.actionExit.setText(QCoreApplication.translate("MainWindow", u"\u9000\u51fa", None))
        self.actionAbout.setText(QCoreApplication.translate("MainWindow", u"\u5173\u4e8e", None))
        self.actionViewRecords.setText(QCoreApplication.translate("MainWindow", u"\u67e5\u770b\u8003\u52e4\u8bb0\u5f55", None))
#if QT_CONFIG(shortcut)
        self.actionViewRecords.setShortcut(QCoreApplication.translate("MainWindow", u"Ctrl+R", None))
#endif // QT_CONFIG(shortcut)
        self.actionSync.setText(QCoreApplication.translate("MainWindow", u"\u540c\u6b65\u670d\u52a1\u5668\u6570\u636e", None))
#if QT_CONFIG(shortcut)
        self.actionSync.setShortcut(QCoreApplication.translate("MainWindow", u"Ctrl+S", None))
#endif // QT_CONFIG(shortcut)
        self.cameraLabel.setText(QCoreApplication.translate("MainWindow", u"\u6444\u50cf\u5934\u9884\u89c8", None))
        self.logEdit.setPlaceholderText(QCoreApplication.translate("MainWindow", u"\u7cfb\u7edf\u65e5\u5fd7", None))
        self.examRoomGroup.setTitle(QCoreApplication.translate("MainWindow", u"\u8003\u573a\u4fe1\u606f", None))
        self.examRoomLabel.setText(QCoreApplication.translate("MainWindow", u"\u9009\u62e9\u8003\u573a\uff1a", None))
        self.loadExamRoomsButton.setText(QCoreApplication.translate("MainWindow", u"\u5237\u65b0\u8003\u573a", None))
        self.examInfoLabel.setText(QCoreApplication.translate("MainWindow", u"\u8003\u573a\u4f4d\u7f6e\uff1a| \u65f6\u95f4\uff1a| \u5bb9\u91cf\uff1a", None))
        self.loadStudentsButton.setText(QCoreApplication.translate("MainWindow", u"\u52a0\u8f7d\u5b66\u751f\u540d\u5355", None))
        ___qtablewidgetitem = self.studentListTable.horizontalHeaderItem(0)
        ___qtablewidgetitem.setText(QCoreApplication.translate("MainWindow", u"\u5b66\u53f7", None));
        ___qtablewidgetitem1 = self.studentListTable.horizontalHeaderItem(1)
        ___qtablewidgetitem1.setText(QCoreApplication.translate("MainWindow", u"\u59d3\u540d", None));
        ___qtablewidgetitem2 = self.studentListTable.horizontalHeaderItem(2)
        ___qtablewidgetitem2.setText(QCoreApplication.translate("MainWindow", u"\u5ea7\u4f4d\u53f7", None));
        ___qtablewidgetitem3 = self.studentListTable.horizontalHeaderItem(3)
        ___qtablewidgetitem3.setText(QCoreApplication.translate("MainWindow", u"\u8003\u52e4\u72b6\u6001", None));
        self.attendanceModeCheckBox.setText(QCoreApplication.translate("MainWindow", u"\u5f00\u542f\u81ea\u52a8\u8003\u52e4\u6a21\u5f0f", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.examRoomTab), QCoreApplication.translate("MainWindow", u"\u8003\u573a\u7ba1\u7406", None))
        self.registerGroup.setTitle(QCoreApplication.translate("MainWindow", u"\u7528\u6237\u6ce8\u518c", None))
        self.nameLabel.setText(QCoreApplication.translate("MainWindow", u"\u59d3\u540d\uff1a", None))
        self.idLabel.setText(QCoreApplication.translate("MainWindow", u"ID\uff1a", None))
        self.registerButton.setText(QCoreApplication.translate("MainWindow", u"\u6ce8\u518c\u4fe1\u606f", None))
        self.faceGroup.setTitle(QCoreApplication.translate("MainWindow", u"\u4eba\u8138\u64cd\u4f5c", None))
        self.captureButton.setText(QCoreApplication.translate("MainWindow", u"\u91c7\u96c6\u4eba\u8138", None))
        self.recognizeButton.setText(QCoreApplication.translate("MainWindow", u"\u8bc6\u522b\u4eba\u8138", None))
        self.attendanceButton.setText(QCoreApplication.translate("MainWindow", u"\u7b7e\u5230", None))
        self.viewRecordsButton.setText(QCoreApplication.translate("MainWindow", u"\u67e5\u770b\u8003\u52e4\u8bb0\u5f55", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.registerTab), QCoreApplication.translate("MainWindow", u"\u4eba\u8138\u6ce8\u518c", None))
        self.menu.setTitle(QCoreApplication.translate("MainWindow", u"\u6587\u4ef6", None))
        self.menu_2.setTitle(QCoreApplication.translate("MainWindow", u"\u5e2e\u52a9", None))
    # retranslateUi

