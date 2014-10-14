#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "libcanbridge.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_butConnect_clicked();

    void on_butDisconnect_clicked();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    LibCanBridge canBridge;
};

#endif // MAINWINDOW_H
