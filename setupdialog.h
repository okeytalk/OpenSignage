#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
        int language = 1;
        int available_maxbet = 1;
        int line_option = 1;
        int maxbet_kind = 1;
        int button_config = 1;
        int doubleup_game = 1;
    };


    explicit SetupDialog(QWidget *parent = 0);
    ~SetupDialog();

    Settings settings() const;

private:
    Settings m_settings;
    Ui::SetupDialog *ui;
    QPushButton* saveButton;
    QPushButton* defaultButton;
    QPushButton* exitButton;

public slots:
    void click_saveButton();
    void click_defaultButton();
    void click_exitButton();

};

#endif // SETUPDIALOG_H
