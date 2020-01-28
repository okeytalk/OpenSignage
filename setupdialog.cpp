#include "setupdialog.h"
#include "ui_setupdialog.h"

#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QtDebug>
#include <QMessageBox>
#include <QProcess>

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);

    QFile file("config.ini");
    QString config[6];

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    for(int i=0; i<6; i++)
        config[i] = in.readLine();

    ui->comboBox_language->setCurrentIndex(config[0].toInt());

    qDebug() << "m_settings.language=" << config[0];
    qDebug() << "m_settings.available_maxbet=" << config[1];
    qDebug() << "m_settings.line_option=" << config[2];
    qDebug() << "m_settings.maxbet_kind=" << config[3];
    qDebug() << "m_settings.button_config=" << config[4];
    qDebug() << "m_settings.doubleup_game=" << config[5];
    file.close();

    saveButton = ui->saveButton;
    QObject::connect(saveButton, SIGNAL(clicked() ), SLOT( click_saveButton() ));
    defaultButton = ui->defaultButton;
    QObject::connect(defaultButton, SIGNAL(clicked() ), SLOT( click_defaultButton() ));
    exitButton = ui->exitButton;
    QObject::connect(exitButton, SIGNAL(clicked() ), SLOT( click_exitButton() ));

}

SetupDialog::~SetupDialog()
{
    delete ui;
}

SetupDialog::Settings SetupDialog::settings() const
{
    return m_settings;
}

void SetupDialog::click_saveButton()
{
    m_settings.language = ui->comboBox_language->currentIndex();
//        if (m_settings.parity > 0)
//            m_settings.parity++;
//        m_settings.baud = ui->baudCombo->currentText().toInt();
//        m_settings.dataBits = ui->dataBitsCombo->currentText().toInt();
//        m_settings.stopBits = ui->stopBitsCombo->currentText().toInt();
//        m_settings.responseTime = ui->timeoutSpinner->value();
//        m_settings.numberOfRetries = ui->retriesSpinner->value();



    //QString str;
    //QString str2;
    //QTextCodec *codec = QTextCodec::codecForLocale();
    // //str = ui->textEdit_write->toPlainText();
    //str = StrToQString("하이,asdklfj,안녕");
    //str2 = codec->toUnicode(str.toLocal8Bit());

    QFile file("config.ini");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << m_settings.language << "\n";
    out << m_settings.available_maxbet << "\n";
    out << m_settings.line_option << "\n";
    out << m_settings.maxbet_kind << "\n";
    out << m_settings.button_config << "\n";
    out << m_settings.doubleup_game << "\n";

    //file.flush();
    file.close();

    QMessageBox msgBox;
    msgBox.setText("The setup has been saved.");
    msgBox.setInformativeText("Do you want to reboot your program?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    switch (ret) {
      case QMessageBox::Save:
          qDebug() << "mSave was clicked";
          QProcess::startDetached(QApplication::applicationFilePath());
          exit(12);
          break;
      case QMessageBox::Cancel:
        qDebug() << "Cancel was clicked";
          break;
      default:
          break;
    }

}

void SetupDialog::click_defaultButton()
{
    qDebug() << "click_defaultButton";
    ui->comboBox_language->setCurrentIndex(0);
}

void SetupDialog::click_exitButton()
{  
    qDebug() << "click_exitButton";
    hide();
}
