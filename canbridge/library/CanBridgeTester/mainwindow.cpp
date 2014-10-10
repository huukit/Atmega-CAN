#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    LibCanBridge bridge;
    qDebug() << bridge.getLibraryVersionString();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
