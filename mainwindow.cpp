#include "mainwindow.h"
#include <qfiledialog.h>
#include <qlayoutitem.h>
#include <qobject.h>
#include <qpixmap.h>
#include "ui_mainwindow.h"
#include "TCanvasWidget.h"
#include <QFileDialog>
#include <QSpacerItem>
#include "qprogressbar.h"
#include "dialogbaseline.h"
#include "dialogtrigger.h"
#include "dialogmisc.h"
#include "socketconfigurations.h"
#include "dialoghvcalculator.h"
#include "formdrawchannelwaves.h"
#include "formdraweventwaves.h"
#include "formdrawseveralwaves.h"
#include "formdrawtrack.h"

#include <iostream>
#include <cstdint>
#include <memory>
#include <sstream>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPrintDialog>
#include <QTextDocument>

#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>

#include <QDebug>
#include <QString>
#include "Client.h"
#include <QTimer>



#include <iostream>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , iProgress(0)
    , bDrawProgressConnected(false)
    , bDAProgressConnected(false)
    , bClientProgressConnected(false)




{
    time_t t = time(0);

    std::strftime(timeStr, sizeof(timeStr), "%Y%m%d%H%M%S", localtime(&t));
    ui->setupUi(this);



    // 假设你的清除按钮对象名为 clearButton
   // connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::clearMessagesAndResetProgress);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        QString message = DebugMessageQueue::instance().getNextMessage();
        if (!message.isEmpty())
        {
            ui->textBrowser_Log->append(message);
        }
    });
    timer->start(20);  // 每隔100毫秒检查一次消息队列
    // 其他初始化代码...

    layout = new QVBoxLayout(ui->frame);
    QWidget *logoWidget = new QWidget(ui->frame);
    logoWidget->setMaximumHeight(50);
    canvas = new TCanvasWidget(ui->frame);
    //progressBar = new QProgressBar(ui->centralwidget);
    QHBoxLayout* hLayout = new QHBoxLayout(logoWidget);
    QLabel *logoLabel = new QLabel(logoWidget);
    logoLabel->setMaximumHeight(50);
    logoLabel->setMaximumWidth(557);
    QCoreApplication::applicationDirPath();
    QPixmap *logoImage = new QPixmap("1732603530514.jpg"); // 图片路径
    logoLabel->setPixmap(*logoImage);
    logoLabel->setScaledContents(true);
    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    textBrowser = new QTextBrowser(logoWidget);
    textBrowser->append("Welcome to the Data Analysis Tool!");
    hLayout->addWidget(logoLabel);
    hLayout->addSpacerItem(spacer); // Add a spacer to the layout (pushes the textBrowser to the right
    hLayout->addWidget(textBrowser);
    layout->addWidget(logoWidget);
    layout->addWidget(canvas);
    sCurrentPath = std::filesystem::current_path().string() + "/";
    sDataFolder = sCurrentPath + "data/";
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    settings->beginGroup("DataFolder");
    sDataFolder = settings->value("dataFolder", QString::fromStdString(sDataFolder)).toString().toStdString();
    settings->endGroup();
    ui->progressBar->setValue(0);
}

MainWindow::~MainWindow()
{
    if (timer!= nullptr)
    {
        if (timer->isActive()) // 如果定时器还在运行，先停止它
        {
            timer->stop();
        }
        delete timer; // 释放定时器对象占用的内存
        timer = nullptr; // 将指针置为空，避免悬空指针
    }

   // delete baselineLogTextEdit;
   // baselineLogTextEdit = nullptr;
    delete ui;
    ui = nullptr;
}


void MainWindow::PrintOnStatusBar(std::string message)
{
    ui->statusBar->showMessage(QString::fromStdString(message));
}

void MainWindow::on_actionFolderOpen_triggered()
{
    QString fdName = QFileDialog::getExistingDirectory(this,
                                                       tr("Open Data Directory"), (sDataFolder).c_str(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!fdName.isEmpty())
    {
        sDataFolder = fdName.toStdString() + "/";
        textBrowser->append("Data folder: " + QString::fromStdString(sDataFolder) + " opened.");
    }
    settings->beginGroup("DataFolder");
    settings->setValue("dataFolder", QString::fromStdString(sDataFolder));
    settings->endGroup();
}

void MainWindow::on_actionFileSave_triggered()
{
    timer->stop();
    //QString fName = QFileDialog::getSaveFileName(this,
    //    tr("Save File"), (sDataFolder).c_str(), tr("Portable Document Format (*.pdf);; Images (*.png *.jpg);; All Files (*)"));
    if (draw == nullptr)
    {
        return;
    }
    draw->printCanvas();
    textBrowser->append("Figure saved.");

    PrintOnStatusBar("Figure has been saved with default name.");
    timer->start(20);
}
/*
void MainWindow::on_actionSocketConnect_triggered()
{
    std::cout << "IP: " << this->ip << std::endl;
    if (client != nullptr)
    {
        delete client;
        client = nullptr;
    }
    client = new Client(this->ip.c_str(), port);
    client->configDPU(sendFileNameVec, saveToFileName.c_str(), saveToLength);
    //client->close();
    delete client;
    client = nullptr;
    textBrowser->append("Data received!");
    PrintOnStatusBar("Data received!");
}
*/
void MainWindow::on_actionSocketConnect_triggered()
{
    std::cout << "IP: " << this->ip << std::endl;
    if (client!= nullptr)
    {
        delete client;
        client = nullptr;
    }
    client = new Client(this->ip.c_str(), port);
    if (client!= nullptr)
    {
        client->configDPU(sendFileNameVec, saveToFileName.c_str(), saveToLength);
        delete client;
        client = nullptr;
    }
    textBrowser->append("Data received!");
    PrintOnStatusBar("Data received!");
}
void MainWindow::on_actionSocketConfigurations_triggered()
{
    if (socketConfigurations != nullptr)
    {
        delete socketConfigurations;
        socketConfigurations = nullptr;
    }
    socketConfigurations = new SocketConfigurations(this);
    //socketConfigurations->setWindowModality(Qt::WindowModal);
    socketConfigurations->show();
    QObject::connect(socketConfigurations, &SocketConfigurations::socketConfigurationsAccepted, this, [=](std::string ip, int port, std::string saveToFileName, int64_t saveToLength, std::vector<std::string> sendFileNameVec) {
        if (settings == nullptr)
        {
            settings = new QSettings("settings.ini", QSettings::IniFormat);
        }
        settings->beginGroup("SocketConfigurations");
        settings->setValue("ip", QString::fromStdString(ip));
        settings->setValue("port", port);
        settings->setValue("saveToFileName", QString::fromStdString(saveToFileName));
        settings->setValue("saveToLength", static_cast<qint64>(saveToLength));

        settings->setValue("sendFileNameVec", QString::fromStdString(sendFileNameVec.at(0)));
        for (int i = 1; i < sendFileNameVec.size(); i++)
        {
            settings->setValue("sendFileNameVec", QString::fromStdString(sendFileNameVec.at(i)));
        }
        settings->endGroup();
        this->ip = ip;
        this->port = port;
        this->saveToFileName = saveToFileName;
        this->saveToLength = saveToLength * 1024*1024;
        this->sendFileNameVec = sendFileNameVec;
        PrintOnStatusBar("IP: " + this->ip);
        textBrowser->append(QString::fromStdString("IP: " + this->ip));
        PrintOnStatusBar("Port: " + std::to_string(this->port));
        textBrowser->append(QString::fromStdString("Port: " + std::to_string(this->port)));
        PrintOnStatusBar("Save to file name: " + this->saveToFileName);
        textBrowser->append(QString::fromStdString("Save to file name: " + this->saveToFileName));
        PrintOnStatusBar("Save to length: " + std::to_string(this->saveToLength));
        textBrowser->append(QString::fromStdString("Save to length: " + std::to_string(this->saveToLength)));
        //std::cout << "ip: " << this->ip << std::endl;
        //std::cout << "port: " << this->port << std::endl;
        //std::cout << "saveToFileName: " << this->saveToFileName << std::endl;
        //std::cout << "saveToLength: " << this->saveToLength << std::endl;
        for (int i = 0; i < this->sendFileNameVec.size(); i++)
        {
            //std::cout << "sendFileNameVec[" << i << "]: " << this->sendFileNameVec.at(i) << std::endl;
            PrintOnStatusBar("sendFileNameVec[" + std::to_string(i) + "]: " + this->sendFileNameVec.at(i));
            textBrowser->append(QString::fromStdString("sendFileNameVec[" + std::to_string(i) + "]: " + this->sendFileNameVec.at(i)));
        }
        //client = new Client(ip.c_str(), port);
        //client->configDPU(saveToFileName.c_str(), saveToLength);
        //client->sendFiles(sendFileNameVec);
    });
}
void MainWindow::on_actionBaseline_triggered()

{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;

    DialogBaseline* dialogBaseline = new DialogBaseline(this);
    //dialogBaseline->setWindowModality(Qt::WindowModal);
    dialogBaseline->show();

    QObject::connect(dialogBaseline, &DialogBaseline::baselineSetupAccepted, this, [&](std::string fileName, std::string filterFileName, int fileSize, int timeOut) {
        if (settings == nullptr)
        {
            settings = new QSettings("settings.ini", QSettings::IniFormat);
        }
        settings->beginGroup("BaselineAcquisition");
        settings->setValue("filterFileName", QString::fromStdString(filterFileName));
        settings->setValue("saveToLength", fileSize);
        settings->setValue("timeOut", timeOut);
        settings->endGroup();



        //std::cout << "IP: " << this->ip << std::endl;
        std::vector<std::string> vSendFileName = {"./cmd/1.dat", "./cmd/2.dat", "./cmd/3.dat", "./cmd/4_self.dat", "./cmd/5.dat", "./cmd/6.dat"};
        if (!filterFileName.empty())
        {
            vSendFileName.push_back(filterFileName);
        }
        vSendFileName.push_back("./cmd/7.dat");
        vSendFileName.push_back("./cmd/8.dat");
        // Create a new thread and run configDPU in that thread
        if (tClient != nullptr)
        {
            delete tClient;
            tClient = nullptr;
        }
        tClient = new std::thread([=]() {
            if (client != nullptr)
            {
                delete client;
                client = nullptr;
            }
            client = new Client("192.168.10.16", 4660);
            ReconnectSocket(SignalClient);
            if (!std::filesystem::exists((std::filesystem::path)sDataFolder))
            {
                std::filesystem::create_directory((std::filesystem::path)sDataFolder);
            }
            std::cout << "Client created, start configDPU" << std::endl;
            client->configDPU(vSendFileName, sDataFolder + fileName + ".dat", fileSize*1024*1024, (int)timeOut);
            // Optionally, you can emit a signal to indicate that the operation is done
        });
        tClient->detach(); // Detach the thread to run independently
        PrintOnStatusBar("Data receiving...");
        textBrowser->append("Data receiving...");
    });

   // QObject::connect(this, &MainWindow::baselineLogUpdated, this, &MainWindow::handleBaselineLog); // 连接信号和槽函数，确保日志信息能被正确处理和显示
    isReconnecting = false;
}

void MainWindow::on_actionTrigger_triggered()

{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;
    DialogTrigger* dialogTrigger = new DialogTrigger(this);
    //dialogTrigger->setWindowModality(Qt::WindowModal);
    dialogTrigger->show();
    QObject::connect(dialogTrigger, &DialogTrigger::triggerSetupAccepted, this, [&](std::string saveToFileName, std::string thresholdFileName, int fileSize, int timeOut) {
        if (settings == nullptr)
        {
            settings = new QSettings("settings.ini", QSettings::IniFormat);
        }
        settings->beginGroup("TriggerAcquisition");
        settings->setValue("thresholdFileName", QString::fromStdString(thresholdFileName));
        settings->setValue("saveToLength", fileSize);
        settings->setValue("timeOut", timeOut);
        settings->endGroup();
        std::vector<std::string> vSendFileName = {"./cmd/1.dat", "./cmd/2.dat", "./cmd/3.dat", "./cmd/4_hit.dat", "./cmd/5.dat", "./cmd/6.dat"};
        if (!thresholdFileName.empty())
        {
            for (const auto & entry : std::filesystem::directory_iterator(thresholdFileName))
            {
                if (entry.is_regular_file() && entry.path().extension() == ".dat")
                {
                    vSendFileName.push_back(entry.path().string());
                }
            }
        }
        vSendFileName.push_back("./cmd/7.dat");
        vSendFileName.push_back("./cmd/8.dat");
        // Create a new thread and run configDPU in that thread
        if (tClient != nullptr)
        {
            delete tClient;
            tClient = nullptr;
        }
        tClient = new std::thread([=]() {
            int dataReceiveHalt = 1;
            if (timeOut != 0)
                dataReceiveHalt = timeOut;
            while (dataReceiveHalt > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                if (client != nullptr)
                {
                    delete client;
                    client = nullptr;
                }
                client = new Client("192.168.10.16", 4660);
                ReconnectSocket(SignalClient);
                std::cout << "Client created, start configDPU" << std::endl;
                dataReceiveHalt = client->configDPU(vSendFileName, (sDataFolder + saveToFileName + ".dat").c_str(), (std::uint64_t)fileSize*1024*1024, (int)(timeOut == 0) ? 0 : dataReceiveHalt);
                // Optionally, you can emit a signal to indicate that the operation is done
            }
        });
        tClient->detach(); // Detach the thread to run independently
        //tClient->join(); // Wait for the thread to finish
        PrintOnStatusBar("Data receiving...");
        textBrowser->append("Data receiving...");
    });
isReconnecting = false;
}

void MainWindow::on_actionExtractDat2Root_triggered()
{ if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;
    sFileName = QFileDialog::getOpenFileName(this,
                                             tr("Open File"), (sDataFolder).c_str(), tr("Data Files (*.dat)")).toStdString();
    if (sFileName.empty())
    {
        return;
    }
    sFileNameWoSuffix = sFileName.substr(0, sFileName.find_last_of("."));
    if (dataAnalysis != nullptr)
    {
        delete dataAnalysis;
        dataAnalysis = nullptr;
    }
    if (draw != nullptr)
    {
        delete draw;
        draw = nullptr;
    }
    if (std::ifstream((sFileNameWoSuffix + ".root").c_str()).good())
    {
        dataAnalysis = new DataAnalysis();
        ReconnectSocket(SignalDataAnalysis);
        dataAnalysis->SetDataFileName(sFileName.c_str());
        dataAnalysis->ReadRootFile();
        this->draw = new Draw(canvas, (sFileNameWoSuffix + ".root").c_str());
        ReconnectSocket(SignalDraw);
        PrintOnStatusBar(sFileName + " Extracted!");
        textBrowser->append(QString::fromStdString(sFileName + " Extracted!"));
        return;
    }
    //std::cout << "sFileName: " << sFileName << std::endl;
    //std::cout << "sFileNameWoSuffix: " << sFileNameWoSuffix << std::endl;
    QElapsedTimer timer;
    timer.start();
    dataAnalysis = new DataAnalysis();
    ReconnectSocket(SignalDataAnalysis);
    dataAnalysis->SetDataFileName(sFileName.c_str());
    dataAnalysis->GetFileHandle();
    dataAnalysis->SetDataFormat(this->DataFormat);
    dataAnalysis->SetTestMode(this->testMode);
    dataAnalysis->Dat2Root();
    dataAnalysis->ReadRootFile();
    //int elapsed = timer.elapsed();
    //std::cout << "DataExtract spent: " << elapsed << " ms" << std::endl;
    //timer.restart();
    this->draw = new Draw(canvas, (sFileNameWoSuffix + ".root").c_str());
    ReconnectSocket(SignalDraw);
    int elapsed = timer.elapsed();
    std::cout << "Extract Data elapsed: " << elapsed << " ms" << std::endl;
    PrintOnStatusBar(sFileName + " Extracted!");
    textBrowser->append(QString::fromStdString(sFileName + " Extracted!"));
    isReconnecting = false;
}
void MainWindow::on_actionExtractWriteThresholdFile_triggered()
{
    if (dataAnalysis == nullptr)
    {
        return;
    }
    std::ostringstream ossThreshold;
    ossThreshold << sFileNameWoSuffix << "_Threshold.dat";
    dataAnalysis->SetDataFormat(this->DataFormat);
    ConfigData configData;
    configData.uTrigRiseStep = this->trigRiseStep;
    configData.uNSigmaCompres = this->trigThresholdNSigma;
    configData.uNHitChannel = this->thrsNHitChannel;
    configData.uTrigDelayCycle = this->trigDelayTime;
    configData.uHitWidthCycle = this->hitWidthCycle;
    configData.puIIRFilter = reinterpret_cast<uint32_t*>(this->IIRFilter);
    configData.uIIRFilterNum = this->IIRFilterNum;
    configData.puBaseline = reinterpret_cast<uint32_t*>(this->Baseline);
    configData.uBaselineNum = this->BaselineByteNum;
    dataAnalysis->WriteConfigFile(ossThreshold.str(), configData);
    PrintOnStatusBar("Threshold file saved to " + ossThreshold.str());
    textBrowser->append(QString::fromStdString("Threshold file saved to " + ossThreshold.str()));
}
void MainWindow::on_actionWriteFilterFile_triggered()
{
    if (dataAnalysis == nullptr)
    {
        return;
    }
    std::ostringstream ossFilter;
    ossFilter << sDataFolder << timeStr << "_Filter.dat";
    DataAnalysis::WriteFilterFile(ossFilter.str().c_str(), this->IIRFilter, int (this->IIRFilterNum), this->Baseline, int (this->BaselineByteNum));
    PrintOnStatusBar("Filter file saved to " + ossFilter.str());
    textBrowser->append(QString::fromStdString("Filter file saved to " + ossFilter.str()));
}
void MainWindow::on_actionExtractBaseline_triggered()
{
    if (dataAnalysis == nullptr)
    {
        return;
    }
    dataAnalysis->ExtractBaseline();
    PrintOnStatusBar("Baseline extracted!");
    textBrowser->append("Baseline extracted!");
}

void MainWindow::on_actionDrawSeveralWaves_triggered()
{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;

    if (frame != nullptr)
    {
        layout->removeWidget(frame);
        delete frame;
        frame = nullptr;
    }
    frame = new QFrame(this);
    frame->setFrameShadow(QFrame::Sunken);
    frame->setFrameShape(QFrame::Box);
    frame->setMaximumHeight(100);
    QHBoxLayout* hLayout = new QHBoxLayout(frame);
    FormDrawSeveralWaves* formDrawSeveralWaves = new FormDrawSeveralWaves(frame);
    hLayout->addWidget(formDrawSeveralWaves);
    layout->addWidget(frame);
    QObject::connect(formDrawSeveralWaves, &FormDrawSeveralWaves::signalDrawSeveralWaves, this, [&](int nBeg, int nFig) {
        std::string fName = sFileNameWoSuffix + "_severalWaves_" + std::to_string(nBeg) + "_" + std::to_string(nFig) + ".png";
        if (draw == nullptr)
        {
            draw = new Draw(canvas, (sFileNameWoSuffix + ".root").c_str());
            ReconnectSocket(SignalDraw);
        }
        draw->drawSeveralWaves(nBeg, nFig, (fName.c_str()));
        PrintOnStatusBar("Figure " + fName + " drawn!");
        textBrowser->append(QString::fromStdString("Figure " + fName + " drawn!"));
    });
    isReconnecting = false;
}
void MainWindow::on_actionDrawSigmaOfBaseline_triggered()
{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;
    if (std::ifstream((sFileNameWoSuffix + ".root").c_str()).good() == false)
    {
        PrintOnStatusBar("No root file found!");
        textBrowser->append("No root file found!");
        return;
    }
    if (frame != nullptr)
    {
        layout->removeWidget(frame);
        delete frame;
        frame = nullptr;
    }
    if (draw == nullptr)
    {
        draw = new Draw(canvas, (sFileNameWoSuffix + ".root").c_str());
        ReconnectSocket(SignalDraw);
    }
    draw->drawSigmaOfBaseline((sFileNameWoSuffix + "_sigmaOfBaseline.png").c_str());
    PrintOnStatusBar("Figure " + sFileNameWoSuffix + "_sigmaOfBaseline.png drawn!");
    textBrowser->append(QString::fromStdString("Figure " + sFileNameWoSuffix + "_sigmaOfBaseline.png drawn!"));
    isReconnecting = false;

}
void MainWindow::on_actionDrawChannelWaves_triggered()
{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;
    if (std::ifstream((sFileNameWoSuffix + ".root").c_str()).good() == false)
    {
        PrintOnStatusBar("No root file found!");
        textBrowser->append("No root file found!");
        return;
    }
    if (frame != nullptr)
    {
        layout->removeWidget(frame);
        delete frame;
        frame = nullptr;
    }
    frame = new QFrame(this);
    frame->setFrameShadow(QFrame::Sunken);
    frame->setFrameShape(QFrame::Box);
    frame->setMaximumHeight(150);
    QGridLayout* gLayout = new QGridLayout(frame);
    FormDrawChannelWaves* formDrawChannelWaves = new FormDrawChannelWaves(frame);
    gLayout->addWidget(formDrawChannelWaves);
    layout->addWidget(frame);
    QObject::connect(formDrawChannelWaves, &FormDrawChannelWaves::signalDrawChannelWaves, this, [&](int channel, int nBeg, int nFig) {
        std::string fName = sFileNameWoSuffix + "_channelWaves_" + std::to_string(channel) + "_" + std::to_string(nBeg) + "_" + std::to_string(nFig) + ".png";
        if (draw == nullptr)
        {
            draw = new Draw(canvas, (sFileNameWoSuffix + ".root").c_str());
            ReconnectSocket(SignalDraw);
        }
        draw->drawChannelWave(channel, nBeg, nFig, (fName).c_str());
        PrintOnStatusBar(fName + " drawn!");
        textBrowser->append(QString::fromStdString(fName + " drawn!"));
    });
    //draw->drawChannelWave(0, 0, 100, (sFileNameWoSuffix.append("_channelWaves.png")).c_str());
    isReconnecting = false;
}
void MainWindow::on_actionDrawEventWaves_triggered()
{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;
    if (std::ifstream((sFileNameWoSuffix + ".root").c_str()).good() == false)
    {
        PrintOnStatusBar("No root file found!");
        textBrowser->append("No root file found!");
        return;
    }
    if (frame != nullptr)
    {
        layout->removeWidget(frame);
        delete frame;
        frame = nullptr;
    }
    frame = new QFrame(this);
    frame->setFrameShadow(QFrame::Sunken);
    frame->setFrameShape(QFrame::Box);
    frame->setMaximumHeight(200);
    QGridLayout* gLayout = new QGridLayout(frame);
    FormDrawEventWaves* formDrawEventWaves = new FormDrawEventWaves(frame);
    gLayout->addWidget(formDrawEventWaves);
    layout->addWidget(frame);
    QObject::connect(formDrawEventWaves, &FormDrawEventWaves::signalDrawEventWaves, this, [&](int eventID, int rbChannelW) {
        std::string fName = sFileNameWoSuffix + "_eventWaves_" + std::to_string(eventID) + ".png";
        if (draw == nullptr)
        {
            draw = new Draw(canvas, (sFileNameWoSuffix + ".root").c_str());
            ReconnectSocket(SignalDraw);
        }
        draw->setChannelWho(rbChannelW);
        draw->drawEventWaves(eventID, (fName).c_str());
        PrintOnStatusBar(fName + " drawn!");
        textBrowser->append(QString::fromStdString(fName + " drawn!"));
    });
     isReconnecting = false;
}
void MainWindow::on_actionDrawMap_triggered()
{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;
    if (std::ifstream((sFileNameWoSuffix + ".root").c_str()).good() == false)
    {
        PrintOnStatusBar("No root file found!");
        textBrowser->append("No root file found!");
        return;
    }
    if (frame != nullptr)
    {
        layout->removeWidget(frame);
        delete frame;
        frame = nullptr;
    }
    if (draw == nullptr)
    {
        draw = new Draw(canvas, (sFileNameWoSuffix + ".root").c_str());
        ReconnectSocket(SignalDraw);
    }
    draw->drawMap((sFileNameWoSuffix + "_map.png").c_str());
    PrintOnStatusBar("Figure " + sFileNameWoSuffix + "_map.png drawn!");
    textBrowser->append(QString::fromStdString("Figure " + sFileNameWoSuffix + "_map.png drawn!"));
     isReconnecting = false;
}
void MainWindow::on_actionDrawTrack_triggered()
{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;
    if (std::ifstream((sFileNameWoSuffix + ".root").c_str()).good() == false)
    {
        PrintOnStatusBar("No root file found!");
        textBrowser->append("No root file found!");
        return;
    }
    if (frame != nullptr)
    {
        layout->removeWidget(frame);
        delete frame;
        frame = nullptr;
    }
    frame = new QFrame(this);
    frame->setFrameShadow(QFrame::Sunken);
    frame->setFrameShape(QFrame::Box);
    frame->setMaximumHeight(100);
    QHBoxLayout* hLayout;
    hLayout = new QHBoxLayout(frame);
    FormDrawTrack* formDrawTrack = new FormDrawTrack(frame);
    hLayout->addWidget(formDrawTrack);
    layout->addWidget(frame);
    QObject::connect(formDrawTrack, &FormDrawTrack::signalDrawTrack, this, [&](int eventID) {
        std::string fName = sFileNameWoSuffix + "_track_" + std::to_string(eventID) + ".png";
        if (draw == nullptr)
        {
            draw = new Draw(canvas, (sFileNameWoSuffix + ".root").c_str());
            ReconnectSocket(SignalDraw);
        }
        draw->drawTrack(eventID, fName.c_str());
        PrintOnStatusBar("Figure " + sFileNameWoSuffix + "_track.png drawn!");
        textBrowser->append(QString::fromStdString("Figure " + sFileNameWoSuffix + "_track.png drawn!"));
    });
     isReconnecting = false;
}
void MainWindow::on_actionDrawSpectrum_triggered()
{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;
    if (std::ifstream((sFileNameWoSuffix + ".root").c_str()).good() == false)
    {
        PrintOnStatusBar("No root file found!");
        textBrowser->append("No root file found!");
        return;
    }
    if (frame != nullptr)
    {
        layout->removeWidget(frame);
        delete frame;
        frame = nullptr;
    }
    if (draw == nullptr)
    {
        draw = new Draw(canvas, (sFileNameWoSuffix + ".root").c_str());
        ReconnectSocket(SignalDraw);
    }
    draw->drawSpectrum((sFileNameWoSuffix + "_spectrum.png").c_str());
    PrintOnStatusBar("Figure " + sFileNameWoSuffix + "_spectrum.png drawn!");
    textBrowser->append(QString::fromStdString("Figure " + sFileNameWoSuffix + "_spectrum.png drawn!"));
     isReconnecting = false;
}

void MainWindow::on_actionDrawTest_triggered()
{
    Draw::drawTest(canvas);
}

void MainWindow::on_actionMiscSetup_triggered()
{
    DialogMisc* dialogMisc = new DialogMisc(this);
    //dialogMisc->setWindowModality(Qt::WindowModal);
    dialogMisc->show();
    //QObject::connect(dialogMisc, &DialogMisc::miscSetupAccepted, this, [&](int testMode, int trigRiseStep, int trigThresholdNSigma, int thrsNHitChannel, int trigDelayTime, int hitWidthCycle, double tSamplePeriod, double crCap, double crRes1, double crRes2, double rc1Mag, double rc1Res, double rc1Cap, double rc2Mag, double rc2Res, double rc2Cap, double crOutBaseline, int DataFormat) {
    QObject::connect(dialogMisc, &DialogMisc::miscSetupAccepted, this, [&](MiscSetup miscSetup) {
        if (settings == nullptr)
        {
            settings = new QSettings("settings.ini", QSettings::IniFormat);
        }
        //std::cout << "Data Format: " << miscSetup.DataFormat << std::endl;
        settings->beginGroup("MiscSetup");
        settings->setValue("testMode", miscSetup.testMode);
        settings->setValue("Trig_Rise_Step", miscSetup.trigRiseStep);
        settings->setValue("Trig_Threshold_Number_of_Sigma", miscSetup.trigThresholdNSigma);
        settings->setValue("Threshold_Number_of_Hit_Channel", miscSetup.thrsNHitChannel);
        settings->setValue("Trig_Delay_Time", miscSetup.trigDelayTime);
        settings->setValue("Hit_Width_Cycle", miscSetup.hitWidthCycle);
        settings->setValue("Data_Format", miscSetup.DataFormat);
        settings->setValue("Sample_Period", miscSetup.tSamplePeriod);
        settings->setValue("CR_Filter_Cap", miscSetup.crCap);
        settings->setValue("CR_Filter_Res1", miscSetup.crRes1);
        settings->setValue("CR_Filter_Res2", miscSetup.crRes2);
        settings->setValue("RC1_Filter_Mag", miscSetup.rc1Mag);
        settings->setValue("RC1_Filter_Res", miscSetup.rc1Res);
        settings->setValue("RC1_Filter_Cap", miscSetup.rc1Cap);
        settings->setValue("RC2_Filter_Mag", miscSetup.rc2Mag);
        settings->setValue("RC2_Filter_Res", miscSetup.rc2Res);
        settings->setValue("RC2_Filter_Cap", miscSetup.rc2Cap);
        settings->setValue("CR_Filter_Output_Baseline", miscSetup.crOutBaseline);
        settings->endGroup();
        this->testMode = miscSetup.testMode;
        this->trigRiseStep = miscSetup.trigRiseStep;
        this->trigThresholdNSigma = miscSetup.trigThresholdNSigma;
        this->thrsNHitChannel = miscSetup.thrsNHitChannel;
        this->trigDelayTime = miscSetup.trigDelayTime;
        this->hitWidthCycle = miscSetup.hitWidthCycle;
        this->DataFormat = miscSetup.DataFormat;
        double CRParam [3] = {};
        double RC1Param [3] = {};
        double RC2Param [3] = {};
        if (miscSetup.tSamplePeriod == 0)
        {
            this->IIRFilterNum = 0;
            this->BaselineByteNum = 0;
        }
        DataAnalysis::CalIIRFilterCRRParameter(CRParam, miscSetup.tSamplePeriod, miscSetup.crRes1, miscSetup.crRes2, miscSetup.crCap);
        //DataAnalysis::calIIRFilterCRParameter(CRParam, tSamplePeriod, crRes1, crRes2, crCap);
        DataAnalysis::CalIIRFilterRCParameter(RC1Param, miscSetup.tSamplePeriod, miscSetup.rc1Mag, miscSetup.rc1Res, miscSetup.rc1Cap);
        DataAnalysis::CalIIRFilterRCParameter(RC2Param, miscSetup.tSamplePeriod, miscSetup.rc2Mag, miscSetup.rc2Res, miscSetup.rc2Cap);
        int IIRFilterPrecision = 10;
        DataAnalysis::Decimal2binary(CRParam[0], IIRFilterPrecision, this->IIRFilter[0]);
        DataAnalysis::Decimal2binary(CRParam[1], IIRFilterPrecision, this->IIRFilter[1]);
        DataAnalysis::Decimal2binary(CRParam[2], IIRFilterPrecision, this->IIRFilter[2]);
        DataAnalysis::Decimal2binary(RC1Param[0], IIRFilterPrecision, this->IIRFilter[3]);
        DataAnalysis::Decimal2binary(RC1Param[1], IIRFilterPrecision, this->IIRFilter[4]);
        DataAnalysis::Decimal2binary(RC1Param[2], IIRFilterPrecision, this->IIRFilter[5]);
        DataAnalysis::Decimal2binary(RC2Param[0], IIRFilterPrecision, this->IIRFilter[6]);
        DataAnalysis::Decimal2binary(RC2Param[1], IIRFilterPrecision, this->IIRFilter[7]);
        DataAnalysis::Decimal2binary(RC2Param[2], IIRFilterPrecision, this->IIRFilter[8]);
        int BaselineIntegerPrecision = 12;
        int BaselineDecimalPrecision = 12;
        //DataAnalysis::decimal2binary(crOutBaseline, BaselineIntegerPrecision, BaselineDecimalPrecision, this->Baseline[0]);
        this->Baseline[0] = miscSetup.crOutBaseline;
        std::cout << "crOutBaseline: " << miscSetup.crOutBaseline << std::endl;
        int BaselineTemp = 0;
        // TODO: precision problem
        //double crAdjustBaseline = miscSetup.crOutBaseline * (1 - CRParam[2]+std::pow(2, -(BaselineDecimalPrecision-1)));
        const int MAX10BITS = 0b0111111111;
        double crAdjustBaseline = miscSetup.crOutBaseline * ((MAX10BITS - this->IIRFilter[2] + 0b1) << 3);
        DataAnalysis::Decimal2binary(crAdjustBaseline, BaselineIntegerPrecision, BaselineDecimalPrecision, BaselineTemp);
        std::cout << "crAdjustBaseline: " << crAdjustBaseline << std::endl;
        this->Baseline[1] = BaselineTemp & 0xFFFF;
        this->Baseline[2] = (BaselineTemp & 0xFFFF0000) >> 16;
        std::cout << "Baseline[0]: " << this->Baseline[0] << std::endl;
        std::cout << "Baseline[1]: " << this->Baseline[1] << std::endl;
        std::cout << "Baseline[2]: " << this->Baseline[2] << std::endl;

        PrintOnStatusBar((this->testMode == 0) ? "Self mode" : "Hit mode");
        textBrowser->append((this->testMode == 0) ? "Self mode" : "Hit mode");
        std::cout << "Trigger rise step: " << miscSetup.trigRiseStep << std::endl;
        std::cout << "Trigger threshold n sigma: " << miscSetup.trigThresholdNSigma << std::endl;
        std::cout << "Threshold n hit channel: " << miscSetup.thrsNHitChannel << std::endl;
        std::cout << "Trigger delay time: " << miscSetup.trigDelayTime << std::endl;
        std::cout << "Hit width cycle: " << miscSetup.hitWidthCycle << std::endl;
        std::cout << "Data format: " << ((miscSetup.DataFormat == 0) ? "Waveform" : "TQ") << std::endl;
        std::cout << "Sample period: " << miscSetup.tSamplePeriod << std::endl;
        std::cout << "CR cap: " << miscSetup.crCap << std::endl;
        std::cout << "CR res1: " << miscSetup.crRes1 << std::endl;
        std::cout << "CR res2: " << miscSetup.crRes2 << std::endl;
        std::cout << "RC1 mag: " << miscSetup.rc1Mag << std::endl;
        std::cout << "RC1 res: " << miscSetup.rc1Res << std::endl;
        std::cout << "RC1 cap: " << miscSetup.rc1Cap << std::endl;
        std::cout << "RC2 mag: " << miscSetup.rc2Mag << std::endl;
        std::cout << "RC2 res: " << miscSetup.rc2Res << std::endl;
        std::cout << "RC2 cap: " << miscSetup.rc2Cap << std::endl;

        std::cout << "CRParam: " << CRParam[0] << " " << CRParam[1] << " " << CRParam[2] << std::endl;
        std::cout << "RC1Param: " << RC1Param[0] << " " << RC1Param[1] << " " << RC1Param[2] << std::endl;
        std::cout << "RC2Param: " << RC2Param[0] << " " << RC2Param[1] << " " << RC2Param[2] << std::endl;
        std::cout << "IIRFilter: " << this->IIRFilter[0] << " " << this->IIRFilter[1] << " " << this->IIRFilter[2] << " " << this->IIRFilter[3] << " " << this->IIRFilter[4] << " " << this->IIRFilter[5] << " " << this->IIRFilter[6] << " " << this->IIRFilter[7] << " " << this->IIRFilter[8] << std::endl;

    });
}
void MainWindow::on_actionHV_Calculator_triggered()
{
    DialogHVCalculator* dialogHVCalculator = new DialogHVCalculator(this);
    //dialogHVCalculator->setWindowModality(Qt::WindowModal);
    dialogHVCalculator->show();
    QObject::connect(dialogHVCalculator, &DialogHVCalculator::hvCalculatorAccepted, this, [&](HVCalculator hvCalculator) {
        if (settings == nullptr)
        {
            settings = new QSettings("settings.ini", QSettings::IniFormat);
        }
        //std::cout << "Data Format: " << miscSetup.DataFormat << std::endl;
        settings->beginGroup("HVCalculator");
        settings->setValue("Ea_Ed", hvCalculator.Ea_Ed);
        settings->setValue("Ea", hvCalculator.Ea);
        settings->setValue("Vm", hvCalculator.Vm);
        settings->setValue("Vfc", hvCalculator.Vfc);
        settings->setValue("Va", hvCalculator.Va);
        settings->setValue("La", hvCalculator.La);
        settings->setValue("Lfc", hvCalculator.Lfc);
        settings->setValue("Ld", hvCalculator.Ld);
        settings->endGroup();
        //settings->sync();
    });
}
/*
void MainWindow::ReconnectSocket(SignalSource signalSource)
{
    if (signalSource == SignalDraw)
    {
        if (bDAProgressConnected)
        {
            bDAProgressConnected = !QObject::disconnect(this->dataAnalysis, &DataAnalysis::progressChanged, ui->progressBar, &QProgressBar::setValue);
            bDrawProgressConnected = QObject::connect(this->draw, &Draw::progressChanged, ui->progressBar, &QProgressBar::setValue);
        }
        else if (bClientProgressConnected)
        {
            bClientProgressConnected = !QObject::disconnect(this->client, &Client::progressChanged, ui->progressBar, &QProgressBar::setValue);
            bDrawProgressConnected = QObject::connect(this->draw, &Draw::progressChanged, ui->progressBar, &QProgressBar::setValue);
        }
        else
        {
            bDrawProgressConnected = QObject::connect(this->draw, &Draw::progressChanged, ui->progressBar, &QProgressBar::setValue);
        }
    }
    else if (signalSource == SignalDataAnalysis)
    {
        if (bDrawProgressConnected)
        {
            bDrawProgressConnected = !QObject::disconnect(this->draw, &Draw::progressChanged, ui->progressBar, &QProgressBar::setValue);
            bDAProgressConnected = QObject::connect(this->dataAnalysis, &DataAnalysis::progressChanged, ui->progressBar, &QProgressBar::setValue);
        }
        else if (bClientProgressConnected)
        {
            bClientProgressConnected = !QObject::disconnect(this->client, &Client::progressChanged, ui->progressBar, &QProgressBar::setValue);
            bDAProgressConnected = QObject::connect(this->dataAnalysis, &DataAnalysis::progressChanged, ui->progressBar, &QProgressBar::setValue);
        }
        else
        {
            bDAProgressConnected = QObject::connect(this->dataAnalysis, &DataAnalysis::progressChanged, ui->progressBar, &QProgressBar::setValue);
        }
    }
    else if (signalSource == SignalClient)
    {
        if (bDrawProgressConnected)
        {
            bDrawProgressConnected = !QObject::disconnect(this->draw, &Draw::progressChanged, ui->progressBar, &QProgressBar::setValue);
            bClientProgressConnected = QObject::connect(this->client, &Client::progressChanged, ui->progressBar, &QProgressBar::setValue);
        }
        else if (bDAProgressConnected)
        {
            bDAProgressConnected = !QObject::disconnect(this->dataAnalysis, &DataAnalysis::progressChanged, ui->progressBar, &QProgressBar::setValue);
            bClientProgressConnected = QObject::connect(this->client, &Client::progressChanged, ui->progressBar, &QProgressBar::setValue);
        }
        else
        {
            bClientProgressConnected = QObject::connect(this->client, &Client::progressChanged, ui->progressBar, &QProgressBar::setValue);
        }
    }
}
*/
void MainWindow::ReconnectSocket(SignalSource signalSource)
{
    if (isReconnecting) return;  // 如果正在重连，直接返回，避免重复操作
    isReconnecting = true;

    bool prevDAConnected = bDAProgressConnected;
    bool prevDrawConnected = bDrawProgressConnected;
    bool prevClientConnected = bClientProgressConnected;
    // 先断开所有可能的连接
    if (bDAProgressConnected)
    {
        bDAProgressConnected =!QObject::disconnect(this->dataAnalysis, &DataAnalysis::progressChanged, ui->progressBar, &QProgressBar::setValue);
    }
    if (bDrawProgressConnected)
    {
        bDrawProgressConnected =!QObject::disconnect(this->draw, &Draw::progressChanged, ui->progressBar, &QProgressBar::setValue);
    }
    if (bClientProgressConnected)
    {
        bClientProgressConnected =!QObject::disconnect(this->client, &Client::progressChanged, ui->progressBar, &QProgressBar::setValue);
    }
    // 根据信号源进行正确的连接
    if (signalSource == SignalDraw)
    {
        bDrawProgressConnected = QObject::connect(this->draw, &Draw::progressChanged, ui->progressBar, &QProgressBar::setValue);
    }
    else if (signalSource == SignalDataAnalysis)
    {
        bDAProgressConnected = QObject::connect(this->dataAnalysis, &DataAnalysis::progressChanged, ui->progressBar, &QProgressBar::setValue);
    }
    else if (signalSource == SignalClient)
    {
        bClientProgressConnected = QObject::connect(this->client, &Client::progressChanged, ui->progressBar, &QProgressBar::setValue);
    }
    // 检查连接状态是否符合预期，如果不符合，可以输出错误信息或者采取其他措施
    if ((signalSource == SignalDraw && bDrawProgressConnected!= true) ||
        (signalSource == SignalDataAnalysis && bDAProgressConnected!= true) ||
        (signalSource == SignalClient && bClientProgressConnected!= true))
    {
        qDebug() << "Signal - Slot connection failed for " << (signalSource == SignalDraw? "SignalDraw" : (signalSource == SignalDataAnalysis? "SignalDataAnalysis" : "SignalClient"));
    }

    isReconnecting = false;
}
void MainWindow::on_widget_print_customContextMenuRequested(const QPoint &pos)
{

}
/*
void MainWindow::clearMessagesAndResetProgress()
{
    // 清除文本浏览器中的信息
    ui->textBrowser_Log->clear();
    // 将iProgress置为0
    iProgress = 0;

    // 如果iProgress的改变需要通知其他地方（比如有进度条等关联显示），可以在这里添加相应的信号发射代码
    // 例如，如果有对应的信号关联进度条更新，像下面这样发射信号（假设存在对应的信号定义和连接）
    // emit progressChanged(iProgress);
}
*/
/*

std::stringstream redirectedOutput;
QString terminalOutput;

void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    switch (type) {
    case QtDebugMsg:
    case QtWarningMsg:
    case QtCriticalMsg:
    case QtFatalMsg:
        terminalOutput += msg + "\n";
        break;



    default:
        // 对于非Qt消息类型相关的普通文本输出（比如重定向过来的printf输出），也添加到字符串流中
        redirectedOutput << msg.toStdString();
        terminalOutput += QString::fromStdString(redirectedOutput.str());  // 将字符串流中的内容添加到要显示的终端输出字符串里
        redirectedOutput.str("");  // 清空字符串流，准备收集下一次的内容
        break;


    }
}

QString getTerminalOutput()//获取终端信息
{

    // 保存原始的消息处理函数
    QtMessageHandler originalHandler = qInstallMessageHandler(nullptr);
    // 安装自定义的消息处理函数
    qInstallMessageHandler(customMessageHandler);

    // 获取收集到的输出内容
    QString result = terminalOutput;
    //terminalOutput.clear();

    // 恢复原始的消息处理函数
    //qInstallMessageHandler(originalHandler);

   // std::cout<<">> "<<result.toStdString()<<std::endl;
    return result;
}



void MainWindow::updateTerminalOutputDialog()
{
    QString newTerminalOutput = getTerminalOutput();
    if (!newTerminalOutput.isEmpty())
    {
        QDialog *dialog = findChild<QDialog *>("outTdialog");
        if (dialog)
        {
            QTextEdit *textEdit = dialog->findChild<QTextEdit *>("outTTextEdit");
            if (textEdit)
            {
                QString currentText = textEdit->toPlainText();
                int currentTextLength = currentText.length();
                QString currentTextEnd = currentText.right(newTerminalOutput.length());
                if (currentTextEnd!= newTerminalOutput)
                {
                    textEdit->setPlainText(currentText + newTerminalOutput);
                    QTextCursor cursor = textEdit->textCursor();
                    cursor.movePosition(QTextCursor::End);
                    textEdit->setTextCursor(cursor);
                    textEdit->ensureCursorVisible();
                }
            }
        }
    }
}


void MainWindow::on_printTerminalOutputAction_triggered()
{

    // 获取终端输出
    QString terminalOutput = getTerminalOutput();
    terminalOutput += QString::fromStdString(redirectedOutput.str());  // 合并重定向收集的内容

    QDialog* dialog = new QDialog(this);
    dialog->resize(350, 550);
    dialog->setWindowTitle("Terminal Output");
    dialog->setObjectName("outTdialog");

    QRect mainWindowRect = this->geometry();
    int mainWindowX = mainWindowRect.x();
    int mainWindowY = mainWindowRect.y();
    int mainWindowWidth = mainWindowRect.width();
    int mainWindowHeight = mainWindowRect.height();

    int dialogX = mainWindowRect.x() + mainWindowRect.width();
    int dialogY = mainWindowRect.y();
    dialog->move(dialogX, dialogY);
    dialog->setWindowTitle("Terminal Output");
    dialog->setObjectName("outTdialog");

    // 后续创建布局、添加部件等代码保持不变

    // 创建垂直布局
    QVBoxLayout* layout = new QVBoxLayout(dialog);

    // 创建文本编辑框用于显示终端输出内容
    QTextEdit* textEdit = new QTextEdit(dialog);
    textEdit->setObjectName("outTTextEdit");
    textEdit->setPlainText(terminalOutput);

    layout->addWidget(textEdit);


    // 创建关闭按钮
    QPushButton* closeButton = new QPushButton("Close", dialog);
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::close);

    layout->addWidget(closeButton);

    // 显示对话框（非阻塞方式）

    dialog->show();
    connect(dialog, &QDialog::finished, dialog, &QDialog::deleteLater);

}
*/
