/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPlainTextEdit *txtLog;
    QPushButton *btnStart;
    QPushButton *btnStop;
    QTableWidget *tblProcesses;
    QProgressBar *barProductos;
    QProgressBar *barRecursos;
    QLabel *lblProcesados;
    QLabel *lblRecursos;
    QFrame *line;
    QFrame *line_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1506, 851);
        MainWindow->setAutoFillBackground(false);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        txtLog = new QPlainTextEdit(centralwidget);
        txtLog->setObjectName(QString::fromUtf8("txtLog"));
        txtLog->setGeometry(QRect(140, 630, 291, 111));
        btnStart = new QPushButton(centralwidget);
        btnStart->setObjectName(QString::fromUtf8("btnStart"));
        btnStart->setEnabled(true);
        btnStart->setGeometry(QRect(1400, 640, 88, 26));
        btnStop = new QPushButton(centralwidget);
        btnStop->setObjectName(QString::fromUtf8("btnStop"));
        btnStop->setGeometry(QRect(1400, 740, 88, 26));
        tblProcesses = new QTableWidget(centralwidget);
        tblProcesses->setObjectName(QString::fromUtf8("tblProcesses"));
        tblProcesses->setGeometry(QRect(480, 631, 391, 111));
        barProductos = new QProgressBar(centralwidget);
        barProductos->setObjectName(QString::fromUtf8("barProductos"));
        barProductos->setGeometry(QRect(1160, 660, 131, 41));
        barProductos->setValue(24);
        barRecursos = new QProgressBar(centralwidget);
        barRecursos->setObjectName(QString::fromUtf8("barRecursos"));
        barRecursos->setGeometry(QRect(940, 660, 141, 41));
        barRecursos->setValue(24);
        lblProcesados = new QLabel(centralwidget);
        lblProcesados->setObjectName(QString::fromUtf8("lblProcesados"));
        lblProcesados->setGeometry(QRect(1180, 640, 66, 18));
        lblRecursos = new QLabel(centralwidget);
        lblRecursos->setObjectName(QString::fromUtf8("lblRecursos"));
        lblRecursos->setGeometry(QRect(970, 640, 66, 18));
        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(110, 630, 20, 111));
        line->setMouseTracking(false);
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(centralwidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(120, 610, 118, 3));
        line_2->setAutoFillBackground(true);
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1506, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnStart->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
        btnStop->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
        lblProcesados->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        lblRecursos->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
