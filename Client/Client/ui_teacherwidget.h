/********************************************************************************
** Form generated from reading UI file 'teacherwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEACHERWIDGET_H
#define UI_TEACHERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TeacherWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QHBoxLayout *infoLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *nameLabel;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *refreshButton;
    QTabWidget *tabWidget;
    QWidget *classTab;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *classesLayout;
    QLabel *classesLabel;
    QListView *classesListView;
    QPushButton *addClassButton;
    QVBoxLayout *studentsLayout;
    QLabel *studentsLabel;
    QTableView *studentsTableView;
    QHBoxLayout *studentButtonsLayout;
    QPushButton *addStudentButton;
    QPushButton *removeStudentButton;
    QPushButton *viewStudentPhotoButton;
    QPushButton *requestPhotosButton;
    QPushButton *viewAllPhotosButton;
    QWidget *examTab;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *examRoomsLayout;
    QLabel *examRoomsLabel;
    QListView *examRoomsListView;
    QHBoxLayout *examRoomButtonsLayout;
    QPushButton *addExamRoomButton;
    QPushButton *manageSeatsButton;
    QVBoxLayout *attendanceLayout;
    QLabel *attendanceLabel;
    QTableView *attendanceTableView;
    QPushButton *exportAttendanceButton;
    QWidget *seatTab;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *seatTopLayout;
    QLabel *currentExamRoomLabel;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *linkClassesButton;
    QHBoxLayout *seatsMainLayout;
    QVBoxLayout *linkedClassesLayout;
    QLabel *linkedClassesLabel;
    QListWidget *linkedClassesListWidget;
    QVBoxLayout *seatsLayout;
    QLabel *seatsLabel;
    QTableView *seatsTableView;
    QHBoxLayout *seatsOptionsLayout;
    QLabel *arrangementTypeLabel;
    QComboBox *arrangementTypeComboBox;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *generateSeatsButton;
    QPushButton *exportSeatsButton;

    void setupUi(QWidget *TeacherWidget)
    {
        if (TeacherWidget->objectName().isEmpty())
            TeacherWidget->setObjectName(QString::fromUtf8("TeacherWidget"));
        TeacherWidget->resize(900, 650);
        verticalLayout = new QVBoxLayout(TeacherWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(TeacherWidget);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        infoLayout = new QHBoxLayout();
        infoLayout->setObjectName(QString::fromUtf8("infoLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        infoLayout->addItem(horizontalSpacer);

        nameLabel = new QLabel(TeacherWidget);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        infoLayout->addWidget(nameLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        infoLayout->addItem(horizontalSpacer_2);

        refreshButton = new QPushButton(TeacherWidget);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));

        infoLayout->addWidget(refreshButton);


        verticalLayout->addLayout(infoLayout);

        tabWidget = new QTabWidget(TeacherWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        classTab = new QWidget();
        classTab->setObjectName(QString::fromUtf8("classTab"));
        horizontalLayout = new QHBoxLayout(classTab);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        classesLayout = new QVBoxLayout();
        classesLayout->setObjectName(QString::fromUtf8("classesLayout"));
        classesLabel = new QLabel(classTab);
        classesLabel->setObjectName(QString::fromUtf8("classesLabel"));
        QFont font1;
        font1.setBold(true);
        classesLabel->setFont(font1);

        classesLayout->addWidget(classesLabel);

        classesListView = new QListView(classTab);
        classesListView->setObjectName(QString::fromUtf8("classesListView"));

        classesLayout->addWidget(classesListView);

        addClassButton = new QPushButton(classTab);
        addClassButton->setObjectName(QString::fromUtf8("addClassButton"));

        classesLayout->addWidget(addClassButton);


        horizontalLayout->addLayout(classesLayout);

        studentsLayout = new QVBoxLayout();
        studentsLayout->setObjectName(QString::fromUtf8("studentsLayout"));
        studentsLabel = new QLabel(classTab);
        studentsLabel->setObjectName(QString::fromUtf8("studentsLabel"));
        studentsLabel->setFont(font1);

        studentsLayout->addWidget(studentsLabel);

        studentsTableView = new QTableView(classTab);
        studentsTableView->setObjectName(QString::fromUtf8("studentsTableView"));

        studentsLayout->addWidget(studentsTableView);

        studentButtonsLayout = new QHBoxLayout();
        studentButtonsLayout->setObjectName(QString::fromUtf8("studentButtonsLayout"));
        addStudentButton = new QPushButton(classTab);
        addStudentButton->setObjectName(QString::fromUtf8("addStudentButton"));

        studentButtonsLayout->addWidget(addStudentButton);

        removeStudentButton = new QPushButton(classTab);
        removeStudentButton->setObjectName(QString::fromUtf8("removeStudentButton"));

        studentButtonsLayout->addWidget(removeStudentButton);

        viewStudentPhotoButton = new QPushButton(classTab);
        viewStudentPhotoButton->setObjectName(QString::fromUtf8("viewStudentPhotoButton"));

        studentButtonsLayout->addWidget(viewStudentPhotoButton);

        requestPhotosButton = new QPushButton(classTab);
        requestPhotosButton->setObjectName(QString::fromUtf8("requestPhotosButton"));

        studentButtonsLayout->addWidget(requestPhotosButton);

        viewAllPhotosButton = new QPushButton(classTab);
        viewAllPhotosButton->setObjectName(QString::fromUtf8("viewAllPhotosButton"));

        studentButtonsLayout->addWidget(viewAllPhotosButton);


        studentsLayout->addLayout(studentButtonsLayout);


        horizontalLayout->addLayout(studentsLayout);

        tabWidget->addTab(classTab, QString());
        examTab = new QWidget();
        examTab->setObjectName(QString::fromUtf8("examTab"));
        horizontalLayout_2 = new QHBoxLayout(examTab);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        examRoomsLayout = new QVBoxLayout();
        examRoomsLayout->setObjectName(QString::fromUtf8("examRoomsLayout"));
        examRoomsLabel = new QLabel(examTab);
        examRoomsLabel->setObjectName(QString::fromUtf8("examRoomsLabel"));
        examRoomsLabel->setFont(font1);

        examRoomsLayout->addWidget(examRoomsLabel);

        examRoomsListView = new QListView(examTab);
        examRoomsListView->setObjectName(QString::fromUtf8("examRoomsListView"));

        examRoomsLayout->addWidget(examRoomsListView);

        examRoomButtonsLayout = new QHBoxLayout();
        examRoomButtonsLayout->setObjectName(QString::fromUtf8("examRoomButtonsLayout"));
        addExamRoomButton = new QPushButton(examTab);
        addExamRoomButton->setObjectName(QString::fromUtf8("addExamRoomButton"));

        examRoomButtonsLayout->addWidget(addExamRoomButton);

        manageSeatsButton = new QPushButton(examTab);
        manageSeatsButton->setObjectName(QString::fromUtf8("manageSeatsButton"));

        examRoomButtonsLayout->addWidget(manageSeatsButton);


        examRoomsLayout->addLayout(examRoomButtonsLayout);


        horizontalLayout_2->addLayout(examRoomsLayout);

        attendanceLayout = new QVBoxLayout();
        attendanceLayout->setObjectName(QString::fromUtf8("attendanceLayout"));
        attendanceLabel = new QLabel(examTab);
        attendanceLabel->setObjectName(QString::fromUtf8("attendanceLabel"));
        attendanceLabel->setFont(font1);

        attendanceLayout->addWidget(attendanceLabel);

        attendanceTableView = new QTableView(examTab);
        attendanceTableView->setObjectName(QString::fromUtf8("attendanceTableView"));

        attendanceLayout->addWidget(attendanceTableView);

        exportAttendanceButton = new QPushButton(examTab);
        exportAttendanceButton->setObjectName(QString::fromUtf8("exportAttendanceButton"));

        attendanceLayout->addWidget(exportAttendanceButton);


        horizontalLayout_2->addLayout(attendanceLayout);

        tabWidget->addTab(examTab, QString());
        seatTab = new QWidget();
        seatTab->setObjectName(QString::fromUtf8("seatTab"));
        verticalLayout_2 = new QVBoxLayout(seatTab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        seatTopLayout = new QHBoxLayout();
        seatTopLayout->setObjectName(QString::fromUtf8("seatTopLayout"));
        currentExamRoomLabel = new QLabel(seatTab);
        currentExamRoomLabel->setObjectName(QString::fromUtf8("currentExamRoomLabel"));
        currentExamRoomLabel->setFont(font1);

        seatTopLayout->addWidget(currentExamRoomLabel);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        seatTopLayout->addItem(horizontalSpacer_3);

        linkClassesButton = new QPushButton(seatTab);
        linkClassesButton->setObjectName(QString::fromUtf8("linkClassesButton"));

        seatTopLayout->addWidget(linkClassesButton);


        verticalLayout_2->addLayout(seatTopLayout);

        seatsMainLayout = new QHBoxLayout();
        seatsMainLayout->setObjectName(QString::fromUtf8("seatsMainLayout"));
        linkedClassesLayout = new QVBoxLayout();
        linkedClassesLayout->setObjectName(QString::fromUtf8("linkedClassesLayout"));
        linkedClassesLabel = new QLabel(seatTab);
        linkedClassesLabel->setObjectName(QString::fromUtf8("linkedClassesLabel"));
        linkedClassesLabel->setFont(font1);

        linkedClassesLayout->addWidget(linkedClassesLabel);

        linkedClassesListWidget = new QListWidget(seatTab);
        linkedClassesListWidget->setObjectName(QString::fromUtf8("linkedClassesListWidget"));

        linkedClassesLayout->addWidget(linkedClassesListWidget);


        seatsMainLayout->addLayout(linkedClassesLayout);

        seatsLayout = new QVBoxLayout();
        seatsLayout->setObjectName(QString::fromUtf8("seatsLayout"));
        seatsLabel = new QLabel(seatTab);
        seatsLabel->setObjectName(QString::fromUtf8("seatsLabel"));
        seatsLabel->setFont(font1);

        seatsLayout->addWidget(seatsLabel);

        seatsTableView = new QTableView(seatTab);
        seatsTableView->setObjectName(QString::fromUtf8("seatsTableView"));

        seatsLayout->addWidget(seatsTableView);


        seatsMainLayout->addLayout(seatsLayout);


        verticalLayout_2->addLayout(seatsMainLayout);

        seatsOptionsLayout = new QHBoxLayout();
        seatsOptionsLayout->setObjectName(QString::fromUtf8("seatsOptionsLayout"));
        arrangementTypeLabel = new QLabel(seatTab);
        arrangementTypeLabel->setObjectName(QString::fromUtf8("arrangementTypeLabel"));

        seatsOptionsLayout->addWidget(arrangementTypeLabel);

        arrangementTypeComboBox = new QComboBox(seatTab);
        arrangementTypeComboBox->addItem(QString());
        arrangementTypeComboBox->addItem(QString());
        arrangementTypeComboBox->addItem(QString());
        arrangementTypeComboBox->addItem(QString());
        arrangementTypeComboBox->setObjectName(QString::fromUtf8("arrangementTypeComboBox"));

        seatsOptionsLayout->addWidget(arrangementTypeComboBox);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        seatsOptionsLayout->addItem(horizontalSpacer_4);

        generateSeatsButton = new QPushButton(seatTab);
        generateSeatsButton->setObjectName(QString::fromUtf8("generateSeatsButton"));

        seatsOptionsLayout->addWidget(generateSeatsButton);

        exportSeatsButton = new QPushButton(seatTab);
        exportSeatsButton->setObjectName(QString::fromUtf8("exportSeatsButton"));

        seatsOptionsLayout->addWidget(exportSeatsButton);


        verticalLayout_2->addLayout(seatsOptionsLayout);

        tabWidget->addTab(seatTab, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(TeacherWidget);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(TeacherWidget);
    } // setupUi

    void retranslateUi(QWidget *TeacherWidget)
    {
        TeacherWidget->setWindowTitle(QCoreApplication::translate("TeacherWidget", "\346\225\231\345\270\210\347\225\214\351\235\242", nullptr));
        titleLabel->setText(QCoreApplication::translate("TeacherWidget", "\346\225\231\345\270\210\347\256\241\347\220\206\351\235\242\346\235\277", nullptr));
        nameLabel->setText(QCoreApplication::translate("TeacherWidget", "\345\247\223\345\220\215: ", nullptr));
        refreshButton->setText(QCoreApplication::translate("TeacherWidget", "\345\210\267\346\226\260\346\225\260\346\215\256", nullptr));
        classesLabel->setText(QCoreApplication::translate("TeacherWidget", "\347\217\255\347\272\247\345\210\227\350\241\250", nullptr));
        addClassButton->setText(QCoreApplication::translate("TeacherWidget", "\346\267\273\345\212\240\347\217\255\347\272\247", nullptr));
        studentsLabel->setText(QCoreApplication::translate("TeacherWidget", "\347\217\255\347\272\247\345\255\246\347\224\237", nullptr));
        addStudentButton->setText(QCoreApplication::translate("TeacherWidget", "\346\267\273\345\212\240\345\255\246\347\224\237", nullptr));
        removeStudentButton->setText(QCoreApplication::translate("TeacherWidget", "\347\247\273\351\231\244\345\255\246\347\224\237", nullptr));
        viewStudentPhotoButton->setText(QCoreApplication::translate("TeacherWidget", "\346\237\245\347\234\213\347\205\247\347\211\207", nullptr));
        requestPhotosButton->setText(QCoreApplication::translate("TeacherWidget", "\350\257\267\346\261\202\347\205\247\347\211\207", nullptr));
        viewAllPhotosButton->setText(QCoreApplication::translate("TeacherWidget", "\346\237\245\347\234\213\347\217\255\347\272\247\347\205\247\347\211\207", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(classTab), QCoreApplication::translate("TeacherWidget", "\347\217\255\347\272\247\347\256\241\347\220\206", nullptr));
        examRoomsLabel->setText(QCoreApplication::translate("TeacherWidget", "\350\200\203\345\234\272\345\210\227\350\241\250", nullptr));
        addExamRoomButton->setText(QCoreApplication::translate("TeacherWidget", "\346\267\273\345\212\240\350\200\203\345\234\272", nullptr));
        manageSeatsButton->setText(QCoreApplication::translate("TeacherWidget", "\347\256\241\347\220\206\345\272\247\344\275\215", nullptr));
        attendanceLabel->setText(QCoreApplication::translate("TeacherWidget", "\350\200\203\345\213\244\346\203\205\345\206\265", nullptr));
        exportAttendanceButton->setText(QCoreApplication::translate("TeacherWidget", "\345\257\274\345\207\272\350\200\203\345\213\244\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(examTab), QCoreApplication::translate("TeacherWidget", "\350\200\203\345\234\272\347\256\241\347\220\206", nullptr));
        currentExamRoomLabel->setText(QCoreApplication::translate("TeacherWidget", "\345\275\223\345\211\215\350\200\203\345\234\272: \346\234\252\351\200\211\346\213\251", nullptr));
        linkClassesButton->setText(QCoreApplication::translate("TeacherWidget", "\345\205\263\350\201\224\347\217\255\347\272\247", nullptr));
        linkedClassesLabel->setText(QCoreApplication::translate("TeacherWidget", "\345\205\263\350\201\224\347\217\255\347\272\247", nullptr));
        seatsLabel->setText(QCoreApplication::translate("TeacherWidget", "\345\272\247\344\275\215\345\256\211\346\216\222", nullptr));
        arrangementTypeLabel->setText(QCoreApplication::translate("TeacherWidget", "\346\216\222\345\272\247\346\226\271\345\274\217:", nullptr));
        arrangementTypeComboBox->setItemText(0, QCoreApplication::translate("TeacherWidget", "\351\232\217\346\234\272\346\216\222\345\272\247", nullptr));
        arrangementTypeComboBox->setItemText(1, QCoreApplication::translate("TeacherWidget", "\346\214\211\347\217\255\347\272\247\346\216\222\345\272\247", nullptr));
        arrangementTypeComboBox->setItemText(2, QCoreApplication::translate("TeacherWidget", "\346\214\211\346\210\220\347\273\251\346\216\222\345\272\247", nullptr));
        arrangementTypeComboBox->setItemText(3, QCoreApplication::translate("TeacherWidget", "\350\207\252\345\256\232\344\271\211\346\216\222\345\272\247", nullptr));

        generateSeatsButton->setText(QCoreApplication::translate("TeacherWidget", "\347\224\237\346\210\220\345\272\247\344\275\215", nullptr));
        exportSeatsButton->setText(QCoreApplication::translate("TeacherWidget", "\345\257\274\345\207\272\345\272\247\344\275\215\350\241\250", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(seatTab), QCoreApplication::translate("TeacherWidget", "\345\272\247\344\275\215\345\256\211\346\216\222", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TeacherWidget: public Ui_TeacherWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEACHERWIDGET_H
