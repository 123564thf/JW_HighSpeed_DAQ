#include "dialogbaseline.h"
#include <qglobal.h>

#include "QFileDialog"
#include "../ui/ui_dialogbaseline.h"

DialogBaseline::DialogBaseline(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBaseline)
{
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    settings->beginGroup("BaselineAcquisition");
    saveToLength = settings->value("saveToLength", static_cast<qlonglong>(saveToLength)).toLongLong();
    timeOut = settings->value("timeOut", static_cast<qlonglong>(timeOut)).toLongLong();
    filterFileName = settings->value("filterFileName", "").toString().toStdString();
    settings->endGroup();
    time_t t = time(0);
    std::strftime(timeStr, sizeof(timeStr), "%Y%m%d%H%M%S", localtime(&t));
    saveToFileName = std::string(timeStr) + "_baseline";
    ui->setupUi(this);
    ui->lineEditSaveFileLength->setText(QString::number(saveToLength));
    ui->lineEditTimeOut->setText(QString::number(timeOut));
    ui->lineEditSaveTo->setText(QString::fromStdString(saveToFileName));
    ui->lineEditFilterFile->setText(QString::fromStdString(filterFileName));

}

DialogBaseline::~DialogBaseline()
{
    delete settings;
    delete ui;
}

void DialogBaseline::on_buttonBoxBaseline_accepted()
{
    emit baselineSetupAccepted(ui->lineEditSaveTo->text().toStdString(), ui->lineEditFilterFile->text().toStdString(), ui->lineEditSaveFileLength->text().toInt(), ui->lineEditTimeOut->text().toInt());
}

void DialogBaseline::on_lineEditSaveTo_editingFinished()
{
    saveToFileName = ui->lineEditSaveTo->text().toStdString();
}

void DialogBaseline::on_lineEditFilterFile_editingFinished()
{
    filterFileName = ui->lineEditFilterFile->text().toStdString();
}

void DialogBaseline::on_pushButtonOpenFilterFile_clicked()
{
    filterFileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "./", tr("Data Files (*.dat)")).toStdString();
    ui->lineEditFilterFile->setText(QString::fromStdString(filterFileName));
}

void DialogBaseline::on_lineEditSaveFileLength_editingFinished()
{
    saveToLength = ui->lineEditSaveFileLength->text().toLongLong();
    if (saveToLength != 0)
    {
        timeOut = 0;
        ui->lineEditTimeOut->setText(QString::number(timeOut));
    }
}

void DialogBaseline::on_lineEditTimeOut_editingFinished()
{
    timeOut = ui->lineEditTimeOut->text().toLongLong();
    if (timeOut != 0)
    {
        saveToLength = 0;
        ui->lineEditSaveFileLength->setText(QString::number(saveToLength));
    }
}
