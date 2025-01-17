#include "formdrawchannelwaves.h"
#include "../ui/ui_formdrawchannelwaves.h"

FormDrawChannelWaves::FormDrawChannelWaves(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDrawChannelWaves)
{
    ui->setupUi(this);
    ui->spinBoxChannel->setRange(0, 127);
    ui->spinBoxChannel->setValue(channel);
    ui->spinBoxChannelNBeg->setValue(nBeg);
    ui->spinBoxChannelNFig->setValue(nFig);
}

FormDrawChannelWaves::~FormDrawChannelWaves()
{
    delete ui;
}

void FormDrawChannelWaves::on_spinBoxChannel_valueChanged(int arg1)
{
    channel = arg1;
    emit signalDrawChannelWaves(channel, nBeg, nFig);
}

void FormDrawChannelWaves::on_spinBoxChannelNBeg_valueChanged(int arg1)
{
    nBeg = arg1;
    emit signalDrawChannelWaves(channel, nBeg, nFig);
}

void FormDrawChannelWaves::on_spinBoxChannelNFig_valueChanged(int arg1)
{
    nFig = arg1;
    emit signalDrawChannelWaves(channel, nBeg, nFig);
}
