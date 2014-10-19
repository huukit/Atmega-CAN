#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <string>
#include <QMessageBox>
#include <QDate>
#include <time.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->combSpeed->addItem("250k");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleIncomingMessage(int mcount){
    qDebug() << "Has messages, count: " << mcount;
    time_t drivertime;
    uint32_t devtime, id;
    uint8_t rtr, datalen, candata[8];

    while(mcount-- > 0){
        canBridge.getMessage(drivertime, devtime, id, rtr, datalen, candata);
        QString entry;
        entry.append(QDateTime::fromTime_t(drivertime).toString());
        entry.append(QString(" - %1 - 0x%2 - %3B - ").arg(devtime).arg(id, 0, 16).arg(datalen));
        for(int i = 0; i < datalen - 1; i++){
            entry.append(QString("%1:").arg(candata[i], 2, 16, QChar('0')).toUpper());
        }
        if(datalen > 0)entry.append(QString("%1").arg(candata[datalen -1], 2, 16, QChar('0')).toUpper());
        ui->textIncoming->append(entry);
    }
}

void MainWindow::on_butConnect_clicked()
{
    int32_t success = canBridge.init(canBridgeDefinitions::bus250k);
    if(success != 0){
        qDebug() << "Error: " << success;
    }
    ui->butConnect->setEnabled(false);
    ui->butDisconnect->setEnabled(true);
    connect(&canBridge, SIGNAL(hasMessage(int)), this, SLOT(handleIncomingMessage(int)));
}

void MainWindow::on_butDisconnect_clicked()
{
    canBridge.close();
    ui->butConnect->setEnabled(true);
    ui->butDisconnect->setEnabled(false);
    disconnect(&canBridge, SIGNAL(hasMessage(int)), this, SLOT(handleIncomingMessage(int)));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox box;
    box.setText(QString::fromStdString(canBridge.getLibraryVersionString()));
    box.exec();
}

void MainWindow::on_butSend_clicked()
{
    uint8_t bufferdata[] = "12345678";
    canBridge.sendMessage(0x0F0, 0, 8, bufferdata);
    count++;
    ui->lineCount->setText(QString::number(count));
}

void MainWindow::on_checkFlood_toggled(bool checked)
{
    if(checked){
        ftimer = new QTimer(this);
        ftimer->setInterval(1);
        connect(ftimer, SIGNAL(timeout()), this, SLOT(on_butSend_clicked()));
        ftimer->start();
        count = 0;
    }
    else
        ftimer->stop();
}

void MainWindow::on_actionClose_triggered()
{
    this->close();
}

void MainWindow::on_butClear_clicked()
{
    ui->textIncoming->clear();
}
