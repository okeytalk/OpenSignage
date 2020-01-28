#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>

#include <QFile>

#ifdef RASPBERRY_PI
#include <wiringPi.h>
#else
#include <QDebug>
#endif

#define BUTTON_CNT              13  // 버튼갯수 (기본12개+setup button 1개)

#define BTN_RESOLRUTION_X       1920
#define BTN_RESOLRUTION_Y       720

#define BTN_SMALL_WIDTH         180
#define BTN_SMALL_HEIGHT        180
#define BTN_MIDDLE_WIDTH        250
#define BTN_MIDDLE_HEIGHT       250

#define BTN_X_MARGINE           90
#define BTN_X_PADDING           60

#define BTN_MIDDLE_X_MARGINE    30

#define BTN_DELAY   200

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent,Qt::FramelessWindowHint),    // remove title bar
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // read config.ini
    QString lang="en";
    QFile file("config.ini");
    QString config[6];

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    for(int i=0; i<6; i++)
        config[i] = in.readLine();

    qDebug() << "m_settings.language=" << config[0];
    qDebug() << "m_settings.available_maxbet=" << config[1];
    qDebug() << "m_settings.line_option=" << config[2];
    qDebug() << "m_settings.maxbet_kind=" << config[3];
    qDebug() << "m_settings.button_config=" << config[4];
    qDebug() << "m_settings.doubleup_game=" << config[5];

    if(config[0].toInt()==0) lang="en";
    else if(config[0].toInt()==1) lang="cn";
    else if(config[0].toInt()==2) lang="sp";
    else if(config[0].toInt()==3) lang="kr";
    qDebug() << "lang=" << lang;

//    file.flush();
    file.close();


    QPixmap bg(":/images/" + lang + "/bg.png");
    QPalette p(palette());
    p.setBrush(QPalette::Background, bg);
    setAutoFillBackground(true);
    setPalette(p);
    setGeometry(0,0,1920,720);

    setupCnt=0;
    quitCnt=0;

    for(int i=0; i<15; i++)
    {
        gpio_input_value[i]= 1;
        pre_gpio_input_value[i]=1;
    }        


#ifdef RASPBERRY_PI
    //    if(wiringPiSetup() == -1)
    //        return 1;        
        wiringPiSetup();
        for(int i=0; i<15; i++)
        {
            pinMode(i,INPUT);
            pullUpDnControl(i,PUD_UP);
        }
        // gpio 15,16 serial(txd,rxd)
        for(int i=17; i<32; i++)
        {
            pinMode(i,OUTPUT);
            digitalWrite(i,HIGH);
        }

        timer1 = new QTimer(this);
        connect(timer1,SIGNAL(timeout()),this, SLOT(Timer1_Slot()));
        timer1->start(200);

#else
    qDebug("RaspberryPi::RaspberryPi()");
#endif

    QBoxLayout *vboxlayout = new QVBoxLayout(this);
    QGridLayout *grid = new QGridLayout();

    QString num;
    int pos[12][4]={
            {47,89,262,261},{360,89,262,261},{672,89,262,261},{988,89,262,261},{1299,89,262,261},{1612,89,262,261},
            {47,371,262,261},{360,371,262,261},{672,371,262,261},{988,371,262,261},{1299,371,262,261},{1612,371,262,261}
    };    



    for(int i = 0; i < 12; i++) {
        num = QString::number(i);
        pushbutton[i] = new QPushButton("", this);
        pushbutton[i]->setStyleSheet("QPushButton{border-image: url(:/images/"+lang+"/btn"+num+"_normal.png);}\n"
                          "QPushButton:pressed{border-image: url(:/images/"+lang+"/btn"+num+"_press.png);}");

            pushbutton[i]->setGeometry(pos[i][0],pos[i][1],pos[i][2],pos[i][3]);
            pushbutton[i]->setMask(QRegion(QRect(0,0,pos[i][2],pos[i][3]),QRegion::Rectangle));
            grid->addWidget(pushbutton[i], i/6, i%6);

    };
    vboxlayout->addLayout(grid);

    QObject::connect(pushbutton[0], SIGNAL(clicked() ), SLOT( click_0() ));
    QObject::connect(pushbutton[1], SIGNAL(clicked() ), SLOT( click_1() ));
    QObject::connect(pushbutton[2], SIGNAL(clicked() ), SLOT( click_2() ));
    QObject::connect(pushbutton[3], SIGNAL(clicked() ), SLOT( click_3() ));
    QObject::connect(pushbutton[4], SIGNAL(clicked() ), SLOT( click_4() ));
    QObject::connect(pushbutton[5], SIGNAL(clicked() ), SLOT( click_5() ));
    QObject::connect(pushbutton[6], SIGNAL(clicked() ), SLOT( click_6() ));
    QObject::connect(pushbutton[7], SIGNAL(clicked() ), SLOT( click_7() ));
    QObject::connect(pushbutton[8], SIGNAL(clicked() ), SLOT( click_8() ));
    QObject::connect(pushbutton[9], SIGNAL(clicked() ), SLOT( click_9() ));
    QObject::connect(pushbutton[10], SIGNAL(clicked() ), SLOT( click_10() ));
    QObject::connect(pushbutton[11], SIGNAL(clicked() ), SLOT( click_11() ));

    int extra_button_pos[4][4]={
        {0,0,50,50},
        {1870,0,50,50},
        {0,670,50,50},
        {1870,670,50,50}
    };
    for(int i = 0; i < 4; i++) {
        extrabutton[i] = new QPushButton("", this);
        extrabutton[i]->setStyleSheet("QPushButton{border-image: url(:/images/"+lang+"/blink_normal.png);}\n"
                                "QPushButton:pressed{border-image: url(:/images/"+lang+"/blink_press.png);}");

            extrabutton[i]->setGeometry(extra_button_pos[i][0],extra_button_pos[i][1],extra_button_pos[i][2],extra_button_pos[i][3]);
            extrabutton[i]->setMask(QRegion(QRect(0,0,extra_button_pos[i][2],extra_button_pos[i][3]),QRegion::Rectangle));
    };
    QObject::connect(extrabutton[0], SIGNAL(clicked() ), SLOT( click_topleft() ));
    QObject::connect(extrabutton[1], SIGNAL(clicked() ), SLOT( click_topright() ));
    QObject::connect(extrabutton[2], SIGNAL(clicked() ), SLOT( click_bottomleft() ));
    QObject::connect(extrabutton[3], SIGNAL(clicked() ), SLOT( click_bottomright() ));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// MainWindow PINMAP
// L1 = Gpio29
// L2 = Gpio28
// L3 = Gpio25
// L4 = Gpio27
// L5 = Gpio24
// B1 = Gpio23
// B2 = Gpio26
// B3 = Gpio23
// B4 = Gpio22
// B5 = Gpio31
// MAXBET = Gpio30
// SPIN = Gpio11
// ATT = Gpio10
// DOUBLE = Gpio14
// TAKE = Gpio6
// CAS = Gpio13

void MainWindow::click_0()
{
#ifdef RASPBERRY_PI
    digitalWrite(10,LOW);
    qDebug("gpio10 = LOW");
    delay(BTN_DELAY);
    digitalWrite(10,HIGH);
    qDebug("gpio10 = HIGH");
#else
    qDebug("void MainWindow::click_0()");
#endif
}

void MainWindow::click_1()
{
#ifdef RASPBERRY_PI
    digitalWrite(29,LOW);
    qDebug("gpio29 = LOW");
    delay(BTN_DELAY);
    digitalWrite(29,HIGH);
    qDebug("gpio29 = HIGH");
#else
    qDebug("void MainWindow::click_1()");
#endif
}

void MainWindow::click_2()
{
#ifdef RASPBERRY_PI
    digitalWrite(28,LOW);
    qDebug("gpio28 = LOW");
    delay(BTN_DELAY);
    digitalWrite(28,HIGH);
    qDebug("gpio28 = HIGH");
#else
    qDebug("void MainWindow::click_2()");
#endif
}

void MainWindow::click_3()
{
#ifdef RASPBERRY_PI
    digitalWrite(25,LOW);
    qDebug("gpio25 = LOW");
    delay(BTN_DELAY);
    digitalWrite(25,HIGH);
    qDebug("gpio25 = HIGH");
#else
    qDebug("void MainWindow::click_3()");
#endif
}

void MainWindow::click_4()
{
#ifdef RASPBERRY_PI
    digitalWrite(27,LOW);
    qDebug("gpio26 = LOW");
    delay(BTN_DELAY);
    digitalWrite(27,HIGH);
    qDebug("gpio26 = HIGH");
#else
    qDebug("void MainWindow::click_4()");
#endif
}

void MainWindow::click_5()
{
#ifdef RASPBERRY_PI
    digitalWrite(24,LOW);
    qDebug("gpio24 = LOW");
    delay(BTN_DELAY);
    digitalWrite(24,HIGH);
    qDebug("gpio24 = HIGH");
#else
    qDebug("void MainWindow::click_5()");
#endif
}

void MainWindow::click_6()
{
#ifdef RASPBERRY_PI
    digitalWrite(13,LOW);
    qDebug("gpio13 = LOW");
    delay(BTN_DELAY);
    digitalWrite(13,HIGH);
    qDebug("gpio13 = HIGH");
#else
    qDebug("void MainWindow::click_6()");
#endif
}

void MainWindow::click_7()
{
#ifdef RASPBERRY_PI
    digitalWrite(23,LOW);
    qDebug("gpio23 = LOW");
    delay(BTN_DELAY);
    digitalWrite(23,HIGH);
    qDebug("gpio23 = HIGH");
#else
    qDebug("void MainWindow::click_7()");
#endif
}

void MainWindow::click_8()
{
#ifdef RASPBERRY_PI
    digitalWrite(26,LOW);
    qDebug("gpio26 = LOW");
    delay(BTN_DELAY);
    digitalWrite(26,HIGH);
    qDebug("gpio26 = HIGH");
#else
    qDebug("void MainWindow::click_8()");
#endif
}

void MainWindow::click_9()
{
#ifdef RASPBERRY_PI
    digitalWrite(22,LOW);
    qDebug("gpio22 = LOW");
    delay(BTN_DELAY);
    digitalWrite(22,HIGH);
    qDebug("gpio22 = HIGH");
#else
    qDebug("void MainWindow::click_9()");
#endif
}

void MainWindow::click_10()
{
#ifdef RASPBERRY_PI
    digitalWrite(21,LOW);
    qDebug("gpio21 = LOW");
    delay(BTN_DELAY);
    digitalWrite(21,HIGH);
    qDebug("gpio21 = HIGH");
#else
    qDebug("void MainWindow::click_10()");
#endif
}

void MainWindow::click_11()
{
#ifdef RASPBERRY_PI
    digitalWrite(31,LOW);
    qDebug("gpio31 = LOW");
    delay(BTN_DELAY);
    digitalWrite(31,HIGH);
    qDebug("gpio31 = HIGH");
#else
    qDebug("void MainWindow::click_11()");
#endif
}

void MainWindow::click_topleft()
{
    setupCnt++;
    qDebug("setupCnt = %d \r\n", setupCnt);
    if(setupCnt==1)
    {
        setupCnt=0;
        ShowSetupDialog();
    }
    // ShowSetupWindow();
}

void MainWindow::click_topright()
{
    qDebug("click_topright");
}

void MainWindow::click_bottomleft()
{
    qDebug("click_bottomleft");
}

void MainWindow::click_bottomright()
{
    quitCnt++;
    qDebug("quitCnt = %d \r\n", quitCnt);
    if(quitCnt==1)
    {
        quitCnt=0;
        this->close();
    }
}

void MainWindow::ShowSetupDialog()
{
// Modal
    SetupDialog dialog;
    dialog.setGeometry(480,0,960,720);
    dialog.exec();

// Modaless
//    SetupDialog dialog;
//    dialog.show();
//    dialog.raise();
//    dialog.activateWindow();
}


void MainWindow::Timer1_Slot()
{
#ifdef RASPBERRY_PI
    QString num;

    for(int i=0; i<15; i++)
    {
        gpio_input_value[i] = digitalRead(i);


        if(pre_gpio_input_value[i] != gpio_input_value[i])
        {
            num = QString::number(i);
            if(!gpio_input_value[i])
            {
                pushbutton[i]->setStyleSheet("QPushButton{border-image: url(:/images/btn"+num+"_normal_off.png);}\n"
                                  "QPushButton:pressed{border-image: url(:/images/btn"+num+"_press.png);}");
                qDebug("%d = normal_off", i);
            }
            else
            {
                pushbutton[i]->setStyleSheet("QPushButton{border-image: url(:/images/btn"+num+"_normal.png);}\n"
                                  "QPushButton:pressed{border-image: url(:/images/btn"+num+"_press.png);}");
                qDebug("%d = normal", i);
            }
            pre_gpio_input_value[i] = gpio_input_value[i];
        }

    }

    qDebug("0 gpio_input_value %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d", gpio_input_value[0],gpio_input_value[1],gpio_input_value[2], \
            gpio_input_value[3],gpio_input_value[4],gpio_input_value[5],gpio_input_value[6], gpio_input_value[7], \
            gpio_input_value[8],gpio_input_value[9],gpio_input_value[10], \
            gpio_input_value[11],gpio_input_value[12],gpio_input_value[13],gpio_input_value[14]);

    qDebug("1 gpio_input_value %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d", pre_gpio_input_value[0],pre_gpio_input_value[1],pre_gpio_input_value[2], \
            gpio_input_value[3],pre_gpio_input_value[4],pre_gpio_input_value[5],pre_gpio_input_value[6], pre_gpio_input_value[7], \
            pre_gpio_input_value[8],pre_gpio_input_value[9],pre_gpio_input_value[10], \
            pre_gpio_input_value[11],pre_gpio_input_value[12],pre_gpio_input_value[13],pre_gpio_input_value[14]);
#endif
}


