#include "dialogmisc.h"
#include <qnamespace.h>
#include "../ui/ui_dialogmisc.h"

DialogMisc::DialogMisc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMisc)
{
    miscSetup.testMode = 0;
    miscSetup.trigRiseStep = 2;
    miscSetup.trigThresholdNSigma = 3;
    miscSetup.thrsNHitChannel = 2;
    miscSetup.trigDelayTime = 600;
    miscSetup.hitWidthCycle = 10;
    miscSetup.tSamplePeriod = 0.000000025;
    miscSetup.crCap = 0;
    miscSetup.crRes1 = 0;
    miscSetup.crRes2 = 0;
    miscSetup.rc1Mag = 1;
    miscSetup.rc1Res = 10000;
    miscSetup.rc1Cap = 0.0000000001;
    miscSetup.rc2Mag = 1;
    miscSetup.rc2Res = 10000;
    miscSetup.rc2Cap = 0.0000000001;
    miscSetup.crOutBaseline = 0;
    miscSetup.crAdjustBaseline = 0;
    miscSetup.DataFormat = 0;

    ui->setupUi(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    settings->beginGroup("MiscSetup");
    miscSetup.testMode = settings->value("testMode", miscSetup.testMode).toInt();
    miscSetup.trigRiseStep = settings->value("Trig_Rise_Step", miscSetup.trigRiseStep).toInt();
    miscSetup.trigThresholdNSigma = settings->value("Trig_Threshold_Number_of_Sigma", miscSetup.trigThresholdNSigma).toInt();
    miscSetup.thrsNHitChannel = settings->value("Threshold_Number_of_Hit_Channel", miscSetup.thrsNHitChannel).toInt();
    miscSetup.trigDelayTime = settings->value("Trig_Delay_Time", miscSetup.trigDelayTime).toInt();
    miscSetup.hitWidthCycle = settings->value("Hit_Width_Cycle", miscSetup.hitWidthCycle).toInt();
    miscSetup.tSamplePeriod = settings->value("Sample_Period", miscSetup.tSamplePeriod).toDouble();
    miscSetup.DataFormat = settings->value("Data_Format", miscSetup.DataFormat).toInt();
    miscSetup.crCap = settings->value("CR_Filter_Cap", miscSetup.crCap).toDouble();
    miscSetup.crRes1 = settings->value("CR_Filter_Res1", miscSetup.crRes1).toDouble();
    miscSetup.crRes2 = settings->value("CR_Filter_Res2", miscSetup.crRes2).toDouble();
    miscSetup.rc1Mag = settings->value("RC1_Filter_Mag", miscSetup.rc1Mag).toDouble();
    miscSetup.rc1Res = settings->value("RC1_Filter_Res", miscSetup.rc1Res).toDouble();
    miscSetup.rc1Cap = settings->value("RC1_Filter_Cap", miscSetup.rc1Cap).toDouble();
    miscSetup.rc2Mag = settings->value("RC2_Filter_Mag", miscSetup.rc2Mag).toDouble();
    miscSetup.rc2Res = settings->value("RC2_Filter_Res", miscSetup.rc2Res).toDouble();
    miscSetup.rc2Cap = settings->value("RC2_Filter_Cap", miscSetup.rc2Cap).toDouble();
    miscSetup.crOutBaseline = settings->value("CR_Filter_Output_Baseline", miscSetup.crOutBaseline).toDouble();
    settings->endGroup();
    ui->radioButtonSelfMode->setChecked(miscSetup.testMode == 0);
    ui->radioButtonHitMode->setChecked(miscSetup.testMode == 1);
    ui->radioButtonWaveform->setChecked(miscSetup.DataFormat == 0);
    ui->radioButtonTQ->setChecked(miscSetup.DataFormat == 1);
    ui->spinBoxThrsTrig->setValue(miscSetup.trigRiseStep);
    ui->spinBoxThrsNSgm->setValue(miscSetup.trigThresholdNSigma);
    ui->spinBoxThrsHitCN->setValue(miscSetup.thrsNHitChannel);
    ui->spinBoxTrigDelayTime->setValue(miscSetup.trigDelayTime);
    ui->spinBoxHitWidthCycle->setValue(miscSetup.hitWidthCycle);
    ui->lineEditSampPeriod->setText(QString::number(miscSetup.tSamplePeriod));
    ui->lineEditCRC->setText(QString::number(miscSetup.crCap));
    ui->lineEditCRR1->setText(QString::number(miscSetup.crRes1));
    ui->lineEditCRR2->setText(QString::number(miscSetup.crRes2));
    ui->lineEditRC1M->setText(QString::number(miscSetup.rc1Mag));
    ui->lineEditRC1R->setText(QString::number(miscSetup.rc1Res));
    ui->lineEditRC1C->setText(QString::number(miscSetup.rc1Cap));
    ui->lineEditRC2M->setText(QString::number(miscSetup.rc2Mag));
    ui->lineEditRC2R->setText(QString::number(miscSetup.rc2Res));
    ui->lineEditRC2C->setText(QString::number(miscSetup.rc2Cap));
    ui->lineEditCROBase->setText(QString::number(miscSetup.crOutBaseline));
}

DialogMisc::~DialogMisc()
{   delete settings;
    delete ui;
}

void DialogMisc::on_radioButtonSelfMode_clicked()
{
    miscSetup.testMode = 0;
    ui->radioButtonSelfMode->setChecked(true);
    ui->radioButtonHitMode->setChecked(false);
}

void DialogMisc::on_radioButtonHitMode_clicked()
{
    miscSetup.testMode = 1;
    ui->radioButtonHitMode->setChecked(true);
    ui->radioButtonSelfMode->setChecked(false);
}

void DialogMisc::on_radioButtonWaveform_toggled(bool checked)
{
    if (checked)
    {
        miscSetup.DataFormat = 0;
    }
}

void DialogMisc::on_radioButtonTQ_toggled(bool checked)
{
    if (checked)
    {
        miscSetup.DataFormat = 1;
    }
}

void DialogMisc::on_spinBoxThrsTrig_valueChanged(int arg1)
{
    miscSetup.trigRiseStep = arg1;
}

void DialogMisc::on_spinBoxThrsNSgm_valueChanged(int arg1)
{
    miscSetup.trigThresholdNSigma = arg1;
}

void DialogMisc::on_spinBoxThrsHitCN_valueChanged(int arg1)
{
    miscSetup.thrsNHitChannel = arg1;
}

void DialogMisc::on_spinBoxTrigDelayTime_valueChanged(int arg1)
{
    miscSetup.trigDelayTime = arg1;
}

void DialogMisc::on_spinBoxHitWidthCycle_valueChanged(int arg1)
{
    miscSetup.hitWidthCycle = arg1;
}

void DialogMisc::on_lineEditSampPeriod_textChanged(const QString &arg1)
{
    miscSetup.tSamplePeriod = arg1.toDouble();
}

void DialogMisc::on_lineEditCRC_textChanged(const QString &arg1)
{
    miscSetup.crCap = arg1.toDouble();
}

void DialogMisc::on_lineEditCRR1_textChanged(const QString &arg1)
{
    miscSetup.crRes1 = arg1.toDouble();
}

void DialogMisc::on_lineEditCRR2_textChanged(const QString &arg1)
{
    miscSetup.crRes2 = arg1.toDouble();
}

void DialogMisc::on_lineEditRC1M_textChanged(const QString &arg1)
{
    miscSetup.rc1Mag = arg1.toDouble();
}

void DialogMisc::on_lineEditRC1R_textChanged(const QString &arg1)
{
    miscSetup.rc1Res = arg1.toDouble();
}

void DialogMisc::on_lineEditRC1C_textChanged(const QString &arg1)
{
    miscSetup.rc1Cap = arg1.toDouble();
}

void DialogMisc::on_lineEditRC2M_textChanged(const QString &arg1)
{
    miscSetup.rc2Mag = arg1.toDouble();
}

void DialogMisc::on_lineEditRC2R_textChanged(const QString &arg1)
{
    miscSetup.rc2Res = arg1.toDouble();
}

void DialogMisc::on_lineEditRC2C_textChanged(const QString &arg1)
{
    miscSetup.rc2Cap = arg1.toDouble();
}

void DialogMisc::on_lineEditCROBase_textChanged(const QString &arg1)
{
    miscSetup.crOutBaseline = arg1.toDouble();
}

void DialogMisc::on_lineEditCRABase_textChanged(const QString &arg1)
{
    miscSetup.crAdjustBaseline = arg1.toDouble();
}

void DialogMisc::on_buttonBoxMisc_accepted()
{
    //emit miscSetupAccepted(testMode, trigRiseStep, trigThresholdNSigma, thrsNHitChannel, trigDelayTime, hitWidthCycle, tSamplePeriod, crCap, crRes1, crRes2, rc1Mag, rc1Res, rc1Cap, rc2Mag, rc2Res, rc2Cap, crOutBaseline, crAdjustBaseline, DataFormat);
    emit miscSetupAccepted(miscSetup);
}
