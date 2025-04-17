/********************************************************************************
** Form generated from reading UI file 'adminwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADMINWIDGET_H
#define UI_ADMINWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
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

class Ui_AdminWidget
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
    QWidget *userTab;
    QVBoxLayout *verticalLayout_2;
    QTableView *usersTableView;
    QHBoxLayout *userButtonsLayout;
    QPushButton *addUserButton;
    QPushButton *editUserButton;
    QPushButton *deleteUserButton;
    QWidget *databaseTab;
    QVBoxLayout *verticalLayout_3;
    QSpacerItem *verticalSpacer;
    QGroupBox *databaseGroupBox;
    QVBoxLayout *verticalLayout_4;
    QLabel *backupLabel;
    QPushButton *backupButton;
    QSpacerItem *verticalSpacer_3;
    QLabel *restoreLabel;
    QPushButton *restoreButton;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *AdminWidget)
    {
        if (AdminWidget->objectName().isEmpty())
            AdminWidget->setObjectName(QString::fromUtf8("AdminWidget"));
        AdminWidget->resize(850, 600);
        verticalLayout = new QVBoxLayout(AdminWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(AdminWidget);
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

        nameLabel = new QLabel(AdminWidget);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        infoLayout->addWidget(nameLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        infoLayout->addItem(horizontalSpacer_2);

        refreshButton = new QPushButton(AdminWidget);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));

        infoLayout->addWidget(refreshButton);


        verticalLayout->addLayout(infoLayout);

        tabWidget = new QTabWidget(AdminWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        userTab = new QWidget();
        userTab->setObjectName(QString::fromUtf8("userTab"));
        verticalLayout_2 = new QVBoxLayout(userTab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        usersTableView = new QTableView(userTab);
        usersTableView->setObjectName(QString::fromUtf8("usersTableView"));

        verticalLayout_2->addWidget(usersTableView);

        userButtonsLayout = new QHBoxLayout();
        userButtonsLayout->setObjectName(QString::fromUtf8("userButtonsLayout"));
        addUserButton = new QPushButton(userTab);
        addUserButton->setObjectName(QString::fromUtf8("addUserButton"));

        userButtonsLayout->addWidget(addUserButton);

        editUserButton = new QPushButton(userTab);
        editUserButton->setObjectName(QString::fromUtf8("editUserButton"));

        userButtonsLayout->addWidget(editUserButton);

        deleteUserButton = new QPushButton(userTab);
        deleteUserButton->setObjectName(QString::fromUtf8("deleteUserButton"));

        userButtonsLayout->addWidget(deleteUserButton);


        verticalLayout_2->addLayout(userButtonsLayout);

        tabWidget->addTab(userTab, QString());
        databaseTab = new QWidget();
        databaseTab->setObjectName(QString::fromUtf8("databaseTab"));
        verticalLayout_3 = new QVBoxLayout(databaseTab);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        databaseGroupBox = new QGroupBox(databaseTab);
        databaseGroupBox->setObjectName(QString::fromUtf8("databaseGroupBox"));
        verticalLayout_4 = new QVBoxLayout(databaseGroupBox);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        backupLabel = new QLabel(databaseGroupBox);
        backupLabel->setObjectName(QString::fromUtf8("backupLabel"));
        backupLabel->setWordWrap(true);

        verticalLayout_4->addWidget(backupLabel);

        backupButton = new QPushButton(databaseGroupBox);
        backupButton->setObjectName(QString::fromUtf8("backupButton"));

        verticalLayout_4->addWidget(backupButton);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_3);

        restoreLabel = new QLabel(databaseGroupBox);
        restoreLabel->setObjectName(QString::fromUtf8("restoreLabel"));
        restoreLabel->setWordWrap(true);

        verticalLayout_4->addWidget(restoreLabel);

        restoreButton = new QPushButton(databaseGroupBox);
        restoreButton->setObjectName(QString::fromUtf8("restoreButton"));

        verticalLayout_4->addWidget(restoreButton);


        verticalLayout_3->addWidget(databaseGroupBox);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        tabWidget->addTab(databaseTab, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(AdminWidget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(AdminWidget);
    } // setupUi

    void retranslateUi(QWidget *AdminWidget)
    {
        AdminWidget->setWindowTitle(QCoreApplication::translate("AdminWidget", "\347\256\241\347\220\206\345\221\230\347\225\214\351\235\242", nullptr));
        titleLabel->setText(QCoreApplication::translate("AdminWidget", "\347\263\273\347\273\237\347\256\241\347\220\206\351\235\242\346\235\277", nullptr));
        nameLabel->setText(QCoreApplication::translate("AdminWidget", "\347\256\241\347\220\206\345\221\230: ", nullptr));
        refreshButton->setText(QCoreApplication::translate("AdminWidget", "\345\210\267\346\226\260\346\225\260\346\215\256", nullptr));
        addUserButton->setText(QCoreApplication::translate("AdminWidget", "\346\267\273\345\212\240\347\224\250\346\210\267", nullptr));
        editUserButton->setText(QCoreApplication::translate("AdminWidget", "\347\274\226\350\276\221\347\224\250\346\210\267", nullptr));
        deleteUserButton->setText(QCoreApplication::translate("AdminWidget", "\345\210\240\351\231\244\347\224\250\346\210\267", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(userTab), QCoreApplication::translate("AdminWidget", "\347\224\250\346\210\267\347\256\241\347\220\206", nullptr));
        databaseGroupBox->setTitle(QCoreApplication::translate("AdminWidget", "\346\225\260\346\215\256\345\272\223\346\223\215\344\275\234", nullptr));
        backupLabel->setText(QCoreApplication::translate("AdminWidget", "\345\244\207\344\273\275\346\225\260\346\215\256\345\272\223\345\217\257\344\273\245\344\277\235\345\255\230\345\275\223\345\211\215\347\263\273\347\273\237\347\232\204\346\211\200\346\234\211\346\225\260\346\215\256\344\270\272SQL\346\226\207\344\273\266\357\274\214\347\224\250\344\272\216\347\263\273\347\273\237\350\277\201\347\247\273\346\210\226\346\225\260\346\215\256\344\277\235\346\212\244\343\200\202", nullptr));
        backupButton->setText(QCoreApplication::translate("AdminWidget", "\345\244\207\344\273\275\346\225\260\346\215\256\345\272\223", nullptr));
        restoreLabel->setText(QCoreApplication::translate("AdminWidget", "\346\201\242\345\244\215\346\225\260\346\215\256\345\272\223\345\260\206\344\275\277\347\224\250\345\244\207\344\273\275\346\226\207\344\273\266\350\246\206\347\233\226\345\275\223\345\211\215\346\211\200\346\234\211\346\225\260\346\215\256\357\274\214\346\255\244\346\223\215\344\275\234\344\270\215\345\217\257\346\222\244\351\224\200\357\274\214\350\257\267\350\260\250\346\205\216\346\223\215\344\275\234\343\200\202", nullptr));
        restoreButton->setText(QCoreApplication::translate("AdminWidget", "\346\201\242\345\244\215\346\225\260\346\215\256\345\272\223", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(databaseTab), QCoreApplication::translate("AdminWidget", "\346\225\260\346\215\256\345\272\223\347\256\241\347\220\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AdminWidget: public Ui_AdminWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADMINWIDGET_H
