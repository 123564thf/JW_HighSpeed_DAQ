#include "dialoghvcalculator.h"
#include "../ui/ui_dialoghvcalculator.h"

DialogHVCalculator::DialogHVCalculator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHVCalculator)
{
    hvCalculator.Ea_Ed = 240;
    hvCalculator.Ea = 380;
    hvCalculator.Vm = .0;
    hvCalculator.Vfc = .0;
    hvCalculator.Va = .0;
    hvCalculator.La = 0.1;
    hvCalculator.Lfc = 51;
    hvCalculator.Ld = 53;
    ui->setupUi(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    settings->beginGroup("HVCalculator");
    hvCalculator.Ea_Ed = settings->value("Ea_Ed", hvCalculator.Ea_Ed).toDouble();
    hvCalculator.Ea = settings->value("Ea", hvCalculator.Ea).toDouble();
    hvCalculator.Vm = settings->value("Vm", hvCalculator.Vm).toDouble();
    hvCalculator.Vfc = settings->value("Vfc", hvCalculator.Vfc).toDouble();
    hvCalculator.Va = settings->value("Va", hvCalculator.Va).toDouble();
    hvCalculator.La = settings->value("La", hvCalculator.La).toDouble();
    hvCalculator.Lfc = settings->value("Lfc", hvCalculator.Lfc).toDouble();
    hvCalculator.Ld = settings->value("Ld", hvCalculator.Ld).toDouble();
    settings->endGroup();
    ui->doubleSpinBoxEa_Ed->setValue(hvCalculator.Ea_Ed);
    ui->doubleSpinBoxEa->setValue(hvCalculator.Ea);
    ui->doubleSpinBoxVm->setValue(hvCalculator.Vm);
    ui->doubleSpinBoxVfc->setValue(hvCalculator.Vfc);
    ui->doubleSpinBoxVa->setValue(hvCalculator.Va);
    ui->doubleSpinBoxLa->setValue(hvCalculator.La);
    ui->doubleSpinBoxLfc->setValue(hvCalculator.Lfc);
    ui->doubleSpinBoxLd->setValue(hvCalculator.Ld);
}

DialogHVCalculator::~DialogHVCalculator()
{   delete settings;
    delete ui;
}

void DialogHVCalculator::calculateHV()
{
    hvCalculator.Vm = hvCalculator.Ea / hvCalculator.La * hvCalculator.Ld / hvCalculator.Ea_Ed;
    hvCalculator.Vfc = hvCalculator.Ea / hvCalculator.La * hvCalculator.Lfc / hvCalculator.Ea_Ed;
    hvCalculator.Va = hvCalculator.Ea * (1 + 1 / hvCalculator.La * hvCalculator.Ld / hvCalculator.Ea_Ed);
    ui->doubleSpinBoxVm->setValue(hvCalculator.Vm);
    ui->doubleSpinBoxVfc->setValue(hvCalculator.Vfc);
    ui->doubleSpinBoxVa->setValue(hvCalculator.Va);
}

void DialogHVCalculator::calculatePmt()
{
    hvCalculator.Ea_Ed = (hvCalculator.Va - hvCalculator.Vm) / hvCalculator.La * hvCalculator.Lfc / hvCalculator.Vfc;
    hvCalculator.Ea = hvCalculator.Va - hvCalculator.Vm;
    ui->doubleSpinBoxEa_Ed->setValue(hvCalculator.Ea_Ed);
    ui->doubleSpinBoxEa->setValue(hvCalculator.Ea);
}

void DialogHVCalculator::calculateMF()
{
    hvCalculator.Vm = hvCalculator.Vfc / hvCalculator.Lfc * hvCalculator.Ld;
    ui->doubleSpinBoxVm->setValue(hvCalculator.Vm);
}

void DialogHVCalculator::calculateFM()
{
    hvCalculator.Vfc = hvCalculator.Vm / hvCalculator.Ld * hvCalculator.Lfc;
    ui->doubleSpinBoxVfc->setValue(hvCalculator.Vfc);
}

void DialogHVCalculator::on_doubleSpinBoxEa_Ed_editingFinished()
{
    hvCalculator.Ea_Ed = ui->doubleSpinBoxEa_Ed->value();
    calculateHV();
}

void DialogHVCalculator::on_doubleSpinBoxEa_editingFinished()
{
    hvCalculator.Ea = ui->doubleSpinBoxEa->value();
    calculateHV();
}

void DialogHVCalculator::on_doubleSpinBoxVm_editingFinished()
{
    hvCalculator.Vm = ui->doubleSpinBoxVm->value();
    calculateFM();
    calculatePmt();
}

void DialogHVCalculator::on_doubleSpinBoxVfc_editingFinished()
{
    hvCalculator.Vfc = ui->doubleSpinBoxVfc->value();
    calculateMF();
    calculatePmt();
}

void DialogHVCalculator::on_doubleSpinBoxVa_editingFinished()
{
    hvCalculator.Va = ui->doubleSpinBoxVa->value();
    calculatePmt();
}

void DialogHVCalculator::on_doubleSpinBoxLa_editingFinished()
{
    hvCalculator.La = ui->doubleSpinBoxLa->value();
    calculateHV();
}

void DialogHVCalculator::on_doubleSpinBoxLfc_editingFinished()
{
    hvCalculator.Lfc = ui->doubleSpinBoxLfc->value();
    calculateHV();
}

void DialogHVCalculator::on_doubleSpinBoxLd_editingFinished()
{
    hvCalculator.Ld = ui->doubleSpinBoxLd->value();
    calculateHV();
}

void DialogHVCalculator::on_buttonBox_accepted()
{
    emit hvCalculatorAccepted(hvCalculator);
    //settings->beginGroup("HVCalculator");
    //settings->setValue("Ea_Ed", Ea_Ed);
    //settings->setValue("Ea", Ea);
    //settings->setValue("Vm", Vm);
    //settings->setValue("Vfc", Vfc);
    //settings->setValue("Va", Va);
    //settings->setValue("La", La);
    //settings->setValue("Lfc", Lfc);
    //settings->setValue("Ld", Ld);
    //settings->endGroup();
    //settings->sync();
    //this->close();
}
