#ifndef DIALOGMISC_H
#define DIALOGMISC_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class DialogMisc;
}

struct MiscSetup
{
    int testMode;
    int trigRiseStep;
    int trigThresholdNSigma;
    int thrsNHitChannel;
    int trigDelayTime;
    int hitWidthCycle;
    double tSamplePeriod;
    double crCap;
    double crRes1;
    double crRes2;
    double rc1Mag;
    double rc1Res;
    double rc1Cap;
    double rc2Mag;
    double rc2Res;
    double rc2Cap;
    double crOutBaseline;
    double crAdjustBaseline;
    int DataFormat;
};

class DialogMisc : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMisc(QWidget *parent = nullptr);
    ~DialogMisc();

signals:
    //void miscSetupAccepted(int testMode, int trigRiseStep, int trigThresholdNSigma, int thrsNHitChannel, int trigDelayTime, int hitWidthCycle, double tSamplePeriod, double crCap, double crRes1, double crRes2, double rc1Mag, double rc1Res, double rc1Cap, double rc2Mag, double rc2Res, double rc2Cap, double crOutBaseline, double crAdjustBaseline, int DataFormat);
    void miscSetupAccepted(MiscSetup miscSetup);

private slots:
    void on_radioButtonSelfMode_clicked();
    void on_radioButtonHitMode_clicked();
    void on_radioButtonWaveform_toggled(bool checked);
    void on_radioButtonTQ_toggled(bool checked);

    void on_spinBoxThrsTrig_valueChanged(int arg1);
    void on_spinBoxThrsNSgm_valueChanged(int arg1);
    void on_spinBoxThrsHitCN_valueChanged(int arg1);
    void on_spinBoxTrigDelayTime_valueChanged(int arg1);
    void on_spinBoxHitWidthCycle_valueChanged(int arg1);

    void on_lineEditSampPeriod_textChanged(const QString &arg1);
    void on_lineEditCRC_textChanged(const QString &arg1);
    void on_lineEditCRR1_textChanged(const QString &arg1);
    void on_lineEditCRR2_textChanged(const QString &arg1);
    void on_lineEditRC1M_textChanged(const QString &arg1);
    void on_lineEditRC1R_textChanged(const QString &arg1);
    void on_lineEditRC1C_textChanged(const QString &arg1);
    void on_lineEditRC2M_textChanged(const QString &arg1);
    void on_lineEditRC2R_textChanged(const QString &arg1);
    void on_lineEditRC2C_textChanged(const QString &arg1);
    void on_lineEditCROBase_textChanged(const QString &arg1);
    void on_lineEditCRABase_textChanged(const QString &arg1);

    void on_buttonBoxMisc_accepted();

private:
    Ui::DialogMisc *ui;
    QSettings* settings;
    //int testMode = 0;
    //int trigRiseStep = 2;
    //int trigThresholdNSigma = 3;
    //int thrsNHitChannel = 2;
    //int trigDelayTime = 600;
    //int hitWidthCycle = 10;
    //int DataFormat = 0;
    //double tSamplePeriod = 0.000000025;
    //double crCap = 0;
    //double crRes1 = 0;
    //double crRes2 = 0;
    //double rc1Mag = 1;
    //double rc1Res = 10000;
    //double rc1Cap = 0.0000000001;
    //double rc2Mag = 1;
    //double rc2Res = 10000;
    //double rc2Cap = 0.0000000001;
    //double crOutBaseline = 0;
    //double crAdjustBaseline = 0;
    MiscSetup miscSetup;
    //{
        //.testMode = 0,
        //.trigRiseStep = 2,
        //.trigThresholdNSigma = 3,
        //.thrsNHitChannel = 2,
        //.trigDelayTime = 600,
        //.hitWidthCycle = 10,
        //.tSamplePeriod = 0.000000025,
        //.crCap = 0,
        //.crRes1 = 0,
        //.crRes2 = 0,
        //.rc1Mag = 1,
        //.rc1Res = 10000,
        //.rc1Cap = 0.0000000001,
        //.rc2Mag = 1,
        //.rc2Res = 10000,
        //.rc2Cap = 0.0000000001,
        //.crOutBaseline = 0,
        //.crAdjustBaseline = 0,
        //.DataFormat = 0
    //};
};

#endif // DIALOGMISC_H
