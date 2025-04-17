/********************************************************************************
** Form generated from reading UI file 'studentwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STUDENTWIDGET_H
#define UI_STUDENTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StudentWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QHBoxLayout *infoLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *nameLabel;
    QSpacerItem *horizontalSpacer_3;
    QLabel *studentIdLabel;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *refreshButton;
    QTabWidget *tabWidget;
    QWidget *attendanceTab;
    QVBoxLayout *verticalLayout_2;
    QTableView *attendanceTableView;
    QWidget *classTab;
    QVBoxLayout *verticalLayout_3;
    QTableView *classTableView;
    QWidget *photoTab;
    QVBoxLayout *verticalLayout_4;
    QLabel *photoStatusLabel;
    QPushButton *uploadPhotoButton;
    QLabel *photoPreviewLabel;
    QWidget *seatTab;
    QVBoxLayout *verticalLayout_5;
    QLabel *examRoomInfoLabel;
    QLabel *seatInfoLabel;
    QWidget *examInfoTab;
    QVBoxLayout *verticalLayout_6;
    QLabel *examInfoLabel;

    void setupUi(QWidget *StudentWidget)
    {
        if (StudentWidget->objectName().isEmpty())
            StudentWidget->setObjectName(QString::fromUtf8("StudentWidget"));
        StudentWidget->resize(800, 600);
        verticalLayout = new QVBoxLayout(StudentWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(StudentWidget);
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

        nameLabel = new QLabel(StudentWidget);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        infoLayout->addWidget(nameLabel);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        infoLayout->addItem(horizontalSpacer_3);

        studentIdLabel = new QLabel(StudentWidget);
        studentIdLabel->setObjectName(QString::fromUtf8("studentIdLabel"));

        infoLayout->addWidget(studentIdLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        infoLayout->addItem(horizontalSpacer_2);

        refreshButton = new QPushButton(StudentWidget);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));

        infoLayout->addWidget(refreshButton);


        verticalLayout->addLayout(infoLayout);

        tabWidget = new QTabWidget(StudentWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        attendanceTab = new QWidget();
        attendanceTab->setObjectName(QString::fromUtf8("attendanceTab"));
        verticalLayout_2 = new QVBoxLayout(attendanceTab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        attendanceTableView = new QTableView(attendanceTab);
        attendanceTableView->setObjectName(QString::fromUtf8("attendanceTableView"));

        verticalLayout_2->addWidget(attendanceTableView);

        tabWidget->addTab(attendanceTab, QString());
        classTab = new QWidget();
        classTab->setObjectName(QString::fromUtf8("classTab"));
        verticalLayout_3 = new QVBoxLayout(classTab);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        classTableView = new QTableView(classTab);
        classTableView->setObjectName(QString::fromUtf8("classTableView"));

        verticalLayout_3->addWidget(classTableView);

        tabWidget->addTab(classTab, QString());
        photoTab = new QWidget();
        photoTab->setObjectName(QString::fromUtf8("photoTab"));
        verticalLayout_4 = new QVBoxLayout(photoTab);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        photoStatusLabel = new QLabel(photoTab);
        photoStatusLabel->setObjectName(QString::fromUtf8("photoStatusLabel"));

        verticalLayout_4->addWidget(photoStatusLabel);

        uploadPhotoButton = new QPushButton(photoTab);
        uploadPhotoButton->setObjectName(QString::fromUtf8("uploadPhotoButton"));

        verticalLayout_4->addWidget(uploadPhotoButton);

        photoPreviewLabel = new QLabel(photoTab);
        photoPreviewLabel->setObjectName(QString::fromUtf8("photoPreviewLabel"));
        photoPreviewLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(photoPreviewLabel);

        tabWidget->addTab(photoTab, QString());
        seatTab = new QWidget();
        seatTab->setObjectName(QString::fromUtf8("seatTab"));
        verticalLayout_5 = new QVBoxLayout(seatTab);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        examRoomInfoLabel = new QLabel(seatTab);
        examRoomInfoLabel->setObjectName(QString::fromUtf8("examRoomInfoLabel"));

        verticalLayout_5->addWidget(examRoomInfoLabel);

        seatInfoLabel = new QLabel(seatTab);
        seatInfoLabel->setObjectName(QString::fromUtf8("seatInfoLabel"));

        verticalLayout_5->addWidget(seatInfoLabel);

        tabWidget->addTab(seatTab, QString());
        examInfoTab = new QWidget();
        examInfoTab->setObjectName(QString::fromUtf8("examInfoTab"));
        verticalLayout_6 = new QVBoxLayout(examInfoTab);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        examInfoLabel = new QLabel(examInfoTab);
        examInfoLabel->setObjectName(QString::fromUtf8("examInfoLabel"));

        verticalLayout_6->addWidget(examInfoLabel);

        tabWidget->addTab(examInfoTab, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(StudentWidget);

        tabWidget->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(StudentWidget);
    } // setupUi

    void retranslateUi(QWidget *StudentWidget)
    {
        StudentWidget->setWindowTitle(QCoreApplication::translate("StudentWidget", "\345\255\246\347\224\237\347\225\214\351\235\242", nullptr));
        titleLabel->setText(QCoreApplication::translate("StudentWidget", "\345\255\246\347\224\237\344\277\241\346\201\257\351\235\242\346\235\277", nullptr));
        nameLabel->setText(QCoreApplication::translate("StudentWidget", "\345\247\223\345\220\215: ", nullptr));
        studentIdLabel->setText(QCoreApplication::translate("StudentWidget", "\345\255\246\345\217\267: ", nullptr));
        refreshButton->setText(QCoreApplication::translate("StudentWidget", "\345\210\267\346\226\260\346\225\260\346\215\256", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(attendanceTab), QCoreApplication::translate("StudentWidget", "\350\200\203\345\213\244\350\256\260\345\275\225", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(classTab), QCoreApplication::translate("StudentWidget", "\347\217\255\347\272\247\344\277\241\346\201\257", nullptr));
        photoStatusLabel->setText(QCoreApplication::translate("StudentWidget", "\347\205\247\347\211\207\347\212\266\346\200\201: \346\234\252\344\270\212\344\274\240", nullptr));
        uploadPhotoButton->setText(QCoreApplication::translate("StudentWidget", "\344\270\212\344\274\240\347\205\247\347\211\207", nullptr));
        photoPreviewLabel->setText(QCoreApplication::translate("StudentWidget", "\347\205\247\347\211\207\351\242\204\350\247\210", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(photoTab), QCoreApplication::translate("StudentWidget", "\347\205\247\347\211\207\347\256\241\347\220\206", nullptr));
        examRoomInfoLabel->setText(QCoreApplication::translate("StudentWidget", "\350\200\203\345\234\272\344\277\241\346\201\257", nullptr));
        seatInfoLabel->setText(QCoreApplication::translate("StudentWidget", "\345\272\247\344\275\215\344\277\241\346\201\257", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(seatTab), QCoreApplication::translate("StudentWidget", "\345\272\247\344\275\215\344\277\241\346\201\257", nullptr));
        examInfoLabel->setText(QCoreApplication::translate("StudentWidget", "\350\200\203\345\234\272\344\277\241\346\201\257", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(examInfoTab), QCoreApplication::translate("StudentWidget", "\350\200\203\345\234\272\344\277\241\346\201\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StudentWidget: public Ui_StudentWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STUDENTWIDGET_H
