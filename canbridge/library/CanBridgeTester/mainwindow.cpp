#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <string>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    LibCanBridge bridge;
    std::string version = bridge.getLibraryVersionString();
    qDebug() << QString::fromStdString(version);
    //bridge.init(250);
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
