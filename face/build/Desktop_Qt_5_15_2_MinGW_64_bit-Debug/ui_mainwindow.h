/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionExit;
    QAction *actionAbout;
    QAction *actionViewRecords;
    QAction *actionSync;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *leftLayout;
    QLabel *cameraLabel;
    QTextEdit *logEdit;
    QTabWidget *tabWidget;
    QWidget *examRoomTab;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *examRoomGroup;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *examRoomLabel;
    QComboBox *examRoomSelector;
    QPushButton *loadExamRoomsButton;
    QLabel *examInfoLabel;
    QPushButton *loadStudentsButton;
    QTableWidget *studentListTable;
    QCheckBox *attendanceModeCheckBox;
    QWidget *registerTab;
    QVBoxLayout *verticalLayout;
    QGroupBox *registerGroup;
    QFormLayout *formLayout;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QLabel *idLabel;
    QLineEdit *idEdit;
    QPushButton *registerButton;
    QGroupBox *faceGroup;
    QVBoxLayout *verticalLayout_2;
    QPushButton *captureButton;
    QPushButton *recognizeButton;
    QPushButton *attendanceButton;
    QPushButton *viewRecordsButton;
    QSpacerItem *verticalSpacer;
    QMenuBar *menubar;
    QMenu *menu;
    QMenu *menu_2;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1200, 800);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionViewRecords = new QAction(MainWindow);
        actionViewRecords->setObjectName(QString::fromUtf8("actionViewRecords"));
        actionSync = new QAction(MainWindow);
        actionSync->setObjectName(QString::fromUtf8("actionSync"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout_2 = new QHBoxLayout(centralwidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        leftLayout = new QVBoxLayout();
        leftLayout->setObjectName(QString::fromUtf8("leftLayout"));
        cameraLabel = new QLabel(centralwidget);
        cameraLabel->setObjectName(QString::fromUtf8("cameraLabel"));
        cameraLabel->setMinimumSize(QSize(640, 480));
        cameraLabel->setFrameShape(QFrame::Box);
        cameraLabel->setAlignment(Qt::AlignCenter);

        leftLayout->addWidget(cameraLabel);

        logEdit = new QTextEdit(centralwidget);
        logEdit->setObjectName(QString::fromUtf8("logEdit"));
        logEdit->setMinimumSize(QSize(0, 150));
        logEdit->setReadOnly(true);

        leftLayout->addWidget(logEdit);


        horizontalLayout_2->addLayout(leftLayout);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setMinimumSize(QSize(400, 0));
        examRoomTab = new QWidget();
        examRoomTab->setObjectName(QString::fromUtf8("examRoomTab"));
        verticalLayout_3 = new QVBoxLayout(examRoomTab);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        examRoomGroup = new QGroupBox(examRoomTab);
        examRoomGroup->setObjectName(QString::fromUtf8("examRoomGroup"));
        verticalLayout_4 = new QVBoxLayout(examRoomGroup);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        examRoomLabel = new QLabel(examRoomGroup);
        examRoomLabel->setObjectName(QString::fromUtf8("examRoomLabel"));

        horizontalLayout_3->addWidget(examRoomLabel);

        examRoomSelector = new QComboBox(examRoomGroup);
        examRoomSelector->setObjectName(QString::fromUtf8("examRoomSelector"));
        examRoomSelector->setMinimumSize(QSize(150, 0));

        horizontalLayout_3->addWidget(examRoomSelector);

        loadExamRoomsButton = new QPushButton(examRoomGroup);
        loadExamRoomsButton->setObjectName(QString::fromUtf8("loadExamRoomsButton"));

        horizontalLayout_3->addWidget(loadExamRoomsButton);


        verticalLayout_4->addLayout(horizontalLayout_3);

        examInfoLabel = new QLabel(examRoomGroup);
        examInfoLabel->setObjectName(QString::fromUtf8("examInfoLabel"));

        verticalLayout_4->addWidget(examInfoLabel);

        loadStudentsButton = new QPushButton(examRoomGroup);
        loadStudentsButton->setObjectName(QString::fromUtf8("loadStudentsButton"));

        verticalLayout_4->addWidget(loadStudentsButton);


        verticalLayout_3->addWidget(examRoomGroup);

        studentListTable = new QTableWidget(examRoomTab);
        if (studentListTable->columnCount() < 4)
            studentListTable->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        studentListTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        studentListTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        studentListTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        studentListTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        studentListTable->setObjectName(QString::fromUtf8("studentListTable"));
        studentListTable->setAlternatingRowColors(true);
        studentListTable->setSelectionMode(QAbstractItemView::SingleSelection);
        studentListTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        studentListTable->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_3->addWidget(studentListTable);

        attendanceModeCheckBox = new QCheckBox(examRoomTab);
        attendanceModeCheckBox->setObjectName(QString::fromUtf8("attendanceModeCheckBox"));

        verticalLayout_3->addWidget(attendanceModeCheckBox);

        tabWidget->addTab(examRoomTab, QString());
        registerTab = new QWidget();
        registerTab->setObjectName(QString::fromUtf8("registerTab"));
        verticalLayout = new QVBoxLayout(registerTab);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        registerGroup = new QGroupBox(registerTab);
        registerGroup->setObjectName(QString::fromUtf8("registerGroup"));
        formLayout = new QFormLayout(registerGroup);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        nameLabel = new QLabel(registerGroup);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, nameLabel);

        nameEdit = new QLineEdit(registerGroup);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, nameEdit);

        idLabel = new QLabel(registerGroup);
        idLabel->setObjectName(QString::fromUtf8("idLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, idLabel);

        idEdit = new QLineEdit(registerGroup);
        idEdit->setObjectName(QString::fromUtf8("idEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, idEdit);

        registerButton = new QPushButton(registerGroup);
        registerButton->setObjectName(QString::fromUtf8("registerButton"));
        registerButton->setMinimumSize(QSize(0, 30));
        QIcon icon(QIcon::fromTheme(QString::fromUtf8("user-add")));
        registerButton->setIcon(icon);

        formLayout->setWidget(2, QFormLayout::FieldRole, registerButton);


        verticalLayout->addWidget(registerGroup);

        faceGroup = new QGroupBox(registerTab);
        faceGroup->setObjectName(QString::fromUtf8("faceGroup"));
        verticalLayout_2 = new QVBoxLayout(faceGroup);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        captureButton = new QPushButton(faceGroup);
        captureButton->setObjectName(QString::fromUtf8("captureButton"));
        captureButton->setMinimumSize(QSize(0, 40));
        QIcon icon1(QIcon::fromTheme(QString::fromUtf8("camera")));
        captureButton->setIcon(icon1);

        verticalLayout_2->addWidget(captureButton);

        recognizeButton = new QPushButton(faceGroup);
        recognizeButton->setObjectName(QString::fromUtf8("recognizeButton"));
        recognizeButton->setMinimumSize(QSize(0, 40));
        QIcon icon2(QIcon::fromTheme(QString::fromUtf8("face-detection")));
        recognizeButton->setIcon(icon2);

        verticalLayout_2->addWidget(recognizeButton);

        attendanceButton = new QPushButton(faceGroup);
        attendanceButton->setObjectName(QString::fromUtf8("attendanceButton"));
        attendanceButton->setMinimumSize(QSize(0, 40));
        QIcon icon3(QIcon::fromTheme(QString::fromUtf8("check")));
        attendanceButton->setIcon(icon3);

        verticalLayout_2->addWidget(attendanceButton);

        viewRecordsButton = new QPushButton(faceGroup);
        viewRecordsButton->setObjectName(QString::fromUtf8("viewRecordsButton"));
        viewRecordsButton->setMinimumSize(QSize(0, 40));
        QIcon icon4(QIcon::fromTheme(QString::fromUtf8("document-open")));
        viewRecordsButton->setIcon(icon4);

        verticalLayout_2->addWidget(viewRecordsButton);


        verticalLayout->addWidget(faceGroup);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        tabWidget->addTab(registerTab, QString());

        horizontalLayout_2->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1200, 21));
        menu = new QMenu(menubar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_2 = new QMenu(menubar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menu->menuAction());
        menubar->addAction(menu_2->menuAction());
        menu->addAction(actionSync);
        menu->addAction(actionViewRecords);
        menu->addSeparator();
        menu->addAction(actionExit);
        menu_2->addAction(actionAbout);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\344\272\272\350\204\270\350\257\206\345\210\253\346\216\222\345\272\247\347\263\273\347\273\237 - \350\200\203\345\213\244\347\253\257", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "\345\205\263\344\272\216", nullptr));
        actionViewRecords->setText(QCoreApplication::translate("MainWindow", "\346\237\245\347\234\213\350\200\203\345\213\244\350\256\260\345\275\225", nullptr));
#if QT_CONFIG(shortcut)
        actionViewRecords->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSync->setText(QCoreApplication::translate("MainWindow", "\345\220\214\346\255\245\346\234\215\345\212\241\345\231\250\346\225\260\346\215\256", nullptr));
#if QT_CONFIG(shortcut)
        actionSync->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        cameraLabel->setText(QCoreApplication::translate("MainWindow", "\346\221\204\345\203\217\345\244\264\351\242\204\350\247\210", nullptr));
        logEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\347\263\273\347\273\237\346\227\245\345\277\227", nullptr));
        examRoomGroup->setTitle(QCoreApplication::translate("MainWindow", "\350\200\203\345\234\272\344\277\241\346\201\257", nullptr));
        examRoomLabel->setText(QCoreApplication::translate("MainWindow", "\351\200\211\346\213\251\350\200\203\345\234\272\357\274\232", nullptr));
        loadExamRoomsButton->setText(QCoreApplication::translate("MainWindow", "\345\210\267\346\226\260\350\200\203\345\234\272", nullptr));
        examInfoLabel->setText(QCoreApplication::translate("MainWindow", "\350\200\203\345\234\272\344\275\215\347\275\256\357\274\232| \346\227\266\351\227\264\357\274\232| \345\256\271\351\207\217\357\274\232", nullptr));
        loadStudentsButton->setText(QCoreApplication::translate("MainWindow", "\345\212\240\350\275\275\345\255\246\347\224\237\345\220\215\345\215\225", nullptr));
        QTableWidgetItem *___qtablewidgetitem = studentListTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\345\255\246\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = studentListTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = studentListTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "\345\272\247\344\275\215\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = studentListTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "\350\200\203\345\213\244\347\212\266\346\200\201", nullptr));
        attendanceModeCheckBox->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\220\257\350\207\252\345\212\250\350\200\203\345\213\244\346\250\241\345\274\217", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(examRoomTab), QCoreApplication::translate("MainWindow", "\350\200\203\345\234\272\347\256\241\347\220\206", nullptr));
        registerGroup->setTitle(QCoreApplication::translate("MainWindow", "\347\224\250\346\210\267\346\263\250\345\206\214", nullptr));
        nameLabel->setText(QCoreApplication::translate("MainWindow", "\345\247\223\345\220\215\357\274\232", nullptr));
        idLabel->setText(QCoreApplication::translate("MainWindow", "ID\357\274\232", nullptr));
        registerButton->setText(QCoreApplication::translate("MainWindow", "\346\263\250\345\206\214\344\277\241\346\201\257", nullptr));
        faceGroup->setTitle(QCoreApplication::translate("MainWindow", "\344\272\272\350\204\270\346\223\215\344\275\234", nullptr));
        captureButton->setText(QCoreApplication::translate("MainWindow", "\351\207\207\351\233\206\344\272\272\350\204\270", nullptr));
        recognizeButton->setText(QCoreApplication::translate("MainWindow", "\350\257\206\345\210\253\344\272\272\350\204\270", nullptr));
        attendanceButton->setText(QCoreApplication::translate("MainWindow", "\347\255\276\345\210\260", nullptr));
        viewRecordsButton->setText(QCoreApplication::translate("MainWindow", "\346\237\245\347\234\213\350\200\203\345\213\244\350\256\260\345\275\225", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(registerTab), QCoreApplication::translate("MainWindow", "\344\272\272\350\204\270\346\263\250\345\206\214", nullptr));
        menu->setTitle(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266", nullptr));
        menu_2->setTitle(QCoreApplication::translate("MainWindow", "\345\270\256\345\212\251", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
