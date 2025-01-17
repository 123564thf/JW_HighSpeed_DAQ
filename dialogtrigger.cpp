#include "dialogtrigger.h"
#include <qfiledialog.h>
#include "../ui/ui_dialogtrigger.h"
#include <QFileDialog>

DialogTrigger::DialogTrigger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTrigger)
{
    time_t t = time(0);
    std::strftime(timeStr, sizeof(timeStr), "%Y%m%d%H%M%S", localtime(&t));
    saveToFileName = std::string(timeStr) + "_trigger";
    ui->setupUi(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    settings->beginGroup("TriggerAcquisition");
    saveToLength = settings->value("saveToLength", static_cast<qlonglong>(saveToLength)).toLongLong();
    timeOut = settings->value("timeOut", static_cast<qlonglong>(timeOut)).toLongLong();
    thresholdFileName = settings->value("thresholdFileName", "").toString().toStdString();
    settings->endGroup();
    ui->lineEditThresholdFile->setText(QString::fromStdString(thresholdFileName));
    ui->lineEditSaveFileLength->setText(QString::number(saveToLength));
    ui->lineEditTimeOut->setText(QString::number(timeOut));
    ui->lineEditSaveTo->setText(QString::fromStdString(saveToFileName));
}

DialogTrigger::~DialogTrigger()
{   delete settings;
    delete ui;
}

void DialogTrigger::on_buttonBoxTrigger_accepted()
{
    emit triggerSetupAccepted(saveToFileName, thresholdFileName, saveToLength, timeOut);
}

void DialogTrigger::on_lineEditSaveTo_editingFinished()
{
    saveToFileName = ui->lineEditSaveTo->text().toStdString();
}

void DialogTrigger::on_lineEditThresholdFile_editingFinished()
{
    thresholdFileName = ui->lineEditThresholdFile->text().toStdString();
}

void DialogTrigger::on_pushButtonOpenThresholdFile_clicked()
{
    //thresholdFileName = QFileDialog::getOpenFileName(this,
    //    tr("Open File"), "./", tr("Data Files (*.dat)")).toStdString();
    thresholdFileName = QFileDialog::getExistingDirectory(this,
        tr("Open Directory"), "./", QFileDialog::ShowDirsOnly).toStdString();
    ui->lineEditThresholdFile->setText(QString::fromStdString(thresholdFileName));
}

void DialogTrigger::on_lineEditSaveFileLength_editingFinished()
{
    saveToLength = ui->lineEditSaveFileLength->text().toLongLong();
    if (saveToLength != 0)
    {
        timeOut = 0;
        ui->lineEditTimeOut->setText(QString::number(timeOut));
    }
}

void DialogTrigger::on_lineEditTimeOut_editingFinished()
{
    timeOut = ui->lineEditTimeOut->text().toLongLong();
    if (timeOut != 0)
    {
        saveToLength = 0;
        ui->lineEditSaveFileLength->setText(QString::number(saveToLength));
    }
}
