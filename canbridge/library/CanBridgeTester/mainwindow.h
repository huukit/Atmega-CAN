#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

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
    void on_butSend_clicked();
    void on_checkFlood_toggled(bool checked);

    void handleIncomingMessage(int mcount);

    void on_actionClose_triggered();

    void on_butClear_clicked();

private:
    Ui::MainWindow *ui;
    LibCanBridge canBridge;
    QTimer * ftimer;
    uint32_t count;
};

#endif // MAINWINDOW_H
