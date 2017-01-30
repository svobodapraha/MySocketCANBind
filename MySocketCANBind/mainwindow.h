#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "receivecanframes.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void onCanMessageReceived(int iCounter, XMC_LMOCan_t *ReceivedCanMsg);
public slots:
    void onCanMessageReceivedFromQThread(int iCounter);
private slots:
    void on_BtnRead_clicked();

private:
    Ui::MainWindow *ui;
};

extern MainWindow *pMainWindow;

#endif // MAINWINDOW_H
