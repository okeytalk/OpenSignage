#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QBoxLayout>
#include <QGridLayout>
#include <QTimer>
#include <QString>
#include "setupdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPushButton* pushbutton[12];
    QPushButton* extrabutton[4];

    int setupCnt;
    int quitCnt;
    QTimer *timer1;
    unsigned int gpio_input_value[15];
    unsigned int pre_gpio_input_value[15];

public slots:
    void click_0();
    void click_1();
    void click_2();
    void click_3();
    void click_4();
    void click_5();
    void click_6();
    void click_7();
    void click_8();
    void click_9();
    void click_10();
    void click_11();
    void click_topleft();
    void click_topright();
    void click_bottomleft();
    void click_bottomright();
    void ShowSetupDialog();

    void Timer1_Slot();
};

#endif // MAINWINDOW_H
