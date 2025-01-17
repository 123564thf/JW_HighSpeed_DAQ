#ifndef FORMDRAWEVENTWAVES_H
#define FORMDRAWEVENTWAVES_H

#include <QWidget>

namespace Ui {
class FormDrawEventWaves;
}

class FormDrawEventWaves : public QWidget
{
    Q_OBJECT

public:
    explicit FormDrawEventWaves(QWidget *parent = nullptr);
    ~FormDrawEventWaves();

signals:
    void signalDrawEventWaves(int eventID, int rbChannelW);

private slots:
    void on_spinBoxEventID_valueChanged(int arg1);
    //void on_spinBoxMaxY_valueChanged(int arg1);


    void on_radioButtonX_toggled(bool checked);
    void on_radioButtonY_toggled(bool checked);
    void on_radioButtonA_toggled(bool checked);
    void on_radioButtonAll_toggled(bool checked);

private:
    Ui::FormDrawEventWaves *ui;
    int eventID = 0;
    int rbChannelW = 0;
};

#endif // FORMDRAWEVENTWAVES_H
