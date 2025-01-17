#include "socketconfigurations.h"
#include "../ui/ui_socketconfigurations.h"
#include <QFileDialog>
#include <iostream>
#include <string>
#include <QString>


SocketConfigurations::SocketConfigurations(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SocketConfigurations)
{
    ui->setupUi(this);
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    settings->beginGroup("SocketConfigurations");
    ip = settings->value("ip", ip.c_str()).toString().toStdString();
    port = settings->value("port", port).toInt();
    saveToFileName = settings->value("saveToFileName", saveToFileName.c_str()).toString().toStdString();
    saveToLength = settings->value("saveToLength", static_cast<qlonglong>(saveToLength)).toLongLong();
    sendFileNameVec.push_back(settings->value("sendFileNameVec", sendFileNameVec.at(0).c_str()).toString().toStdString());    
    ui->lineEditIPAddr->setText(QString::fromStdString(ip));
    ui->lineEditPort->setText(QString::number(port));
    ui->lineEditSaveFileLength->setText(QString::number(saveToLength));
    ui->textEditSendFiles->setText(QString("%1\n%2\n%3\n%4\n%5\n%6\n%7\n%8").arg(QString::fromStdString(sendFileNameVec.at(0))).arg(QString::fromStdString(sendFileNameVec.at(1))).arg(QString::fromStdString(sendFileNameVec.at(2))).arg(QString::fromStdString(sendFileNameVec.at(3))).arg(QString::fromStdString(sendFileNameVec.at(4))).arg(QString::fromStdString(sendFileNameVec.at(5))).arg(QString::fromStdString(sendFileNameVec.at(6))).arg(QString::fromStdString(sendFileNameVec.at(7))));
    ui->lineEditSaveTo->setText(QString::fromStdString(saveToFileName));
    //std::cout << "init_ip: " << ip << std::endl;
    //std::cout << "init_port: " << port << std::endl;
}

SocketConfigurations::~SocketConfigurations()
{
    delete settings;
    delete ui;
}

void SocketConfigurations::on_lineEditIPAddr_editingFinished()
{
    ip = ui->lineEditIPAddr->text().toStdString();
    std::cout << "ip: " << ip << std::endl;
}

void SocketConfigurations::on_lineEditPort_editingFinished()
{
    port = ui->lineEditPort->text().toInt();
    std::cout << "port: " << port << std::endl;
}

void SocketConfigurations::on_lineEditSaveTo_editingFinished()
{
    saveToFileName = ui->lineEditSaveTo->text().toStdString();
}

void SocketConfigurations::on_pushButtonOpenSaveTo_clicked()
{
    saveToFileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "./", tr("Data Files (*.dat)")).toStdString();
    std::cout << "saveToFileName: " << saveToFileName << std::endl;
    ui->lineEditSaveTo->setText(QString::fromStdString(saveToFileName));
}

void SocketConfigurations::on_lineEditSaveFileLength_editingFinished()
{
    saveToLength = ui->lineEditSaveFileLength->text().toLongLong();
    std::cout << "saveToLength: " << saveToLength << std::endl;
}

void SocketConfigurations::on_textEditSendFiles_textChanged()
{
    sendFileNameVec.clear();
    QStringList fileNames = ui->textEditSendFiles->toPlainText().split("\n");
    for (int i = 0; i < fileNames.size(); i++)
    {
        sendFileNameVec.push_back(fileNames.at(i).toStdString());
        std::cout << "sendFileNameVec[" << i << "]: " << sendFileNameVec.at(i) << std::endl;
    }
}

void SocketConfigurations::on_pushButtonOpenSendFiles_clicked()
{
    sendFileNameVec.clear();

    ui->textEditSendFiles->clear();
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Open File"), "./", tr("Data Files (*.dat)"));
    for (int i = 0; i < fileNames.size(); i++)
    {
        sendFileNameVec.push_back(fileNames.at(i).toStdString());
        ui->textEditSendFiles->append(fileNames.at(i));
        //ui->textEditSendFiles->append("\n");
        std::cout << "sendFileNameVec[" << i << "]: " << sendFileNameVec.at(i) << std::endl;
    }
}





void SocketConfigurations::on_buttonBoxSocket_accepted()
{
    emit socketConfigurationsAccepted(ip, port, saveToFileName, saveToLength, sendFileNameVec);
}
