/********************************************************************************
** Form generated from reading UI file 'MainWindowfNHVIa.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MAINWINDOWFNHVIA_H
#define MAINWINDOWFNHVIA_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTreeWidget *treeWidget;
    QLabel *label;
    QGroupBox *groupBox;
    QLineEdit *bindIpLineEdit;
    QLabel *label_2;
    QLineEdit *bindPortLineEdit;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *retransIpLineEdit;
    QLabel *label_5;
    QLineEdit *retransPortLineEdit;
    QPushButton *saveSettingPushButton;
    QLabel *label_6;
    QLineEdit *diffLineEdit;
    QListWidget *listWidget;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(988, 785);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        treeWidget = new QTreeWidget(centralwidget);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setGeometry(QRect(30, 70, 341, 681));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(30, 30, 131, 31));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(400, 40, 561, 341));
        bindIpLineEdit = new QLineEdit(groupBox);
        bindIpLineEdit->setObjectName(QString::fromUtf8("bindIpLineEdit"));
        bindIpLineEdit->setGeometry(QRect(80, 30, 141, 31));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 40, 72, 15));
        bindPortLineEdit = new QLineEdit(groupBox);
        bindPortLineEdit->setObjectName(QString::fromUtf8("bindPortLineEdit"));
        bindPortLineEdit->setGeometry(QRect(320, 30, 141, 31));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(240, 40, 72, 15));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 90, 72, 15));
        retransIpLineEdit = new QLineEdit(groupBox);
        retransIpLineEdit->setObjectName(QString::fromUtf8("retransIpLineEdit"));
        retransIpLineEdit->setGeometry(QRect(80, 80, 141, 31));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(240, 90, 72, 15));
        retransPortLineEdit = new QLineEdit(groupBox);
        retransPortLineEdit->setObjectName(QString::fromUtf8("retransPortLineEdit"));
        retransPortLineEdit->setGeometry(QRect(320, 80, 141, 31));
        saveSettingPushButton = new QPushButton(groupBox);
        saveSettingPushButton->setObjectName(QString::fromUtf8("saveSettingPushButton"));
        saveSettingPushButton->setGeometry(QRect(370, 290, 93, 28));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 140, 51, 16));
        diffLineEdit = new QLineEdit(groupBox);
        diffLineEdit->setObjectName(QString::fromUtf8("diffLineEdit"));
        diffLineEdit->setGeometry(QRect(80, 130, 141, 31));
        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(400, 410, 561, 341));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "POWPServer", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "\345\200\274", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "\345\257\271\350\261\241\345\220\215\345\255\227", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\345\256\242\346\210\267\347\253\257\345\210\227\350\241\250:", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "\345\205\250\345\261\200\350\256\276\347\275\256:", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\347\233\221\345\220\254IP:", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\347\233\221\345\220\254\347\253\257\345\217\243:", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\350\275\254\345\217\221IP:", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\350\275\254\345\217\221\347\253\257\345\217\243:", nullptr));
        saveSettingPushButton->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\350\256\276\347\275\256", nullptr));
        label_6->setText(QCoreApplication::translate("MainWindow", "\351\232\276\345\272\246:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MAINWINDOWFNHVIA_H
