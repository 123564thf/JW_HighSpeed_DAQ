#include "formdraweventwaves.h"
#include "../ui/ui_formdraweventwaves.h"

FormDrawEventWaves::FormDrawEventWaves(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDrawEventWaves)
{
    ui->setupUi(this);
    ui->spinBoxEventID->setValue(eventID);
}

FormDrawEventWaves::~FormDrawEventWaves()
{
    delete ui;
}

void FormDrawEventWaves::on_spinBoxEventID_valueChanged(int arg1)
{
    eventID = arg1;
    emit signalDrawEventWaves(eventID, rbChannelW);
}

//void DialogDrawEventWaves::on_spinBoxMaxY_valueChanged(int arg1)
//{
//    yMaxValue = arg1;
//    emit signalDrawEventWaves(eventID, yMaxValue);
//}


void FormDrawEventWaves::on_radioButtonX_toggled(bool checked)
{
    if (checked)
    {
        rbChannelW = 1;
    }
    emit signalDrawEventWaves(eventID, rbChannelW);
}

void FormDrawEventWaves::on_radioButtonY_toggled(bool checked)
{
    if (checked)
    {
        rbChannelW = 2;
    }
    emit signalDrawEventWaves(eventID, rbChannelW);
}

void FormDrawEventWaves::on_radioButtonA_toggled(bool checked)
{
    if (checked)
    {
        rbChannelW = 3;
    }
    emit signalDrawEventWaves(eventID, rbChannelW);
}

void FormDrawEventWaves::on_radioButtonAll_toggled(bool checked)
{
    if (checked)
    {
        rbChannelW = 0;
    }
    emit signalDrawEventWaves(eventID, rbChannelW);
}
