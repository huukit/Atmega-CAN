#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <string>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_butConnect_clicked()
{
    int32_t success = canBridge.init(canBridgeDefinitions::bus250k);
    if(success != 0){
        qDebug() << "Error: " << success;
    }
    ui->butConnect->setEnabled(false);
    ui->butDisconnect->setEnabled(true);
}

void MainWindow::on_butDisconnect_clicked()
{
    canBridge.close();
    ui->butConnect->setEnabled(true);
    ui->butDisconnect->setEnabled(false);
}

void MainWindow::on_actionAbout_triggered()
{
    QString version = QString::fromStdString(canBridge.getLibraryVersionString());
    QMessageBox box;
    box.setText(version);
    box.exec();
}
