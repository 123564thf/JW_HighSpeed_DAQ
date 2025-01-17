#include "formdrawseveralwaves.h"
#include "../ui/ui_formdrawseveralwaves.h"

FormDrawSeveralWaves::FormDrawSeveralWaves(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDrawSeveralWaves)
{
    ui->setupUi(this);
    ui->spinBoxNBegin->setValue(nBegin);
    ui->spinBoxNFigure->setValue(nFigure);
}

FormDrawSeveralWaves::~FormDrawSeveralWaves()
{
    delete ui;
}

void FormDrawSeveralWaves::on_spinBoxNBegin_valueChanged(int arg1)
{
    nBegin = arg1;
    emit signalDrawSeveralWaves(nBegin, nFigure);
}

void FormDrawSeveralWaves::on_spinBoxNFigure_valueChanged(int arg1)
{
    nFigure = arg1;
    emit signalDrawSeveralWaves(nBegin, nFigure);
}
