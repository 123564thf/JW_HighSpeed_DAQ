#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTextBrowser>
#include <QMainWindow>
#include "Draw.h"
#include "TCanvasWidget.h"
#include "QVBoxLayout"
#include "QFrame.h"
#include "DataAnalysis.h"
#include "QSettings"
#include "Client.h"
#include "socketconfigurations.h"
#include <thread>
#include <QtPrintSupport>
#include <QtWidgets>
#include <QSettings>
#include "ui_mainwindow.h"
#include <queue>  // 添加这行来引入queue相关的定义

#include <QTextEdit>  // 引入头文件用于文本编辑框，用于显示日志信息
#include <QStringList> // 用于存储日志信息的列表

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void PrintOnStatusBar(std::string message);
    //void displayOutput(const QString newOutput);





//signals:
   // void baselineLogUpdated(const QString& logMessage); // 新增信号，用于在有新的baseline日志信息时发出通知



private:
    Ui::MainWindow *ui;
    std::string sCurrentPath;
    std::string sDataFolder = "";
    QVBoxLayout *layout = nullptr;
    TCanvasWidget* canvas = nullptr;
    QTextBrowser* textBrowser = nullptr;
    int iProgress = 0;
    bool bDrawProgressConnected = false;
    bool bDAProgressConnected = false;
    bool bClientProgressConnected = false;
    char timeStr[64];
    QFrame* frame = nullptr;
    DataAnalysis* dataAnalysis = nullptr;
    Client* client = nullptr;
    Draw* draw = nullptr;
    SocketConfigurations* socketConfigurations = nullptr;

    QSettings* settings = nullptr;
    std::thread* tClient = nullptr;
    std::string sFileName = "";
    std::string sFileNameWoSuffix = "";
    std::string ip;
    int port;
    std::string saveToFileName;
    int64_t saveToLength;
    std::vector<std::string> sendFileNameVec;
    int testMode = 0;
    int trigRiseStep = 2;
    int trigThresholdNSigma = 3;
    int thrsNHitChannel = 2;
    int trigDelayTime = 600;
    int hitWidthCycle = 10;
    int DataFormat = 0;
    #define IIRFILTERNUM 9
    int IIRFilter[IIRFILTERNUM] = {};
    int IIRFilterNum = IIRFILTERNUM;
    #define BASELINEBYTENUM 3
    int Baseline[BASELINEBYTENUM] = {};
    int BaselineByteNum = BASELINEBYTENUM;
    int rbChannelW = 0;
    enum SignalSource { SignalDataAnalysis, SignalDraw, SignalClient };
    void ReconnectSocket(SignalSource signalSource);
    QTimer *timer;
    bool isReconnecting = false;






private slots:
    void on_actionFolderOpen_triggered();
    void on_actionFileSave_triggered();

    void on_actionSocketConnect_triggered();
    void on_actionSocketConfigurations_triggered();
    void on_actionBaseline_triggered();
    void on_actionTrigger_triggered();

    void on_actionExtractDat2Root_triggered();
    void on_actionExtractWriteThresholdFile_triggered();
    void on_actionWriteFilterFile_triggered();
    void on_actionExtractBaseline_triggered();
    //void on_actionExtractGetWholeEvent_triggered();

    void on_actionDrawSeveralWaves_triggered();
    void on_actionDrawSigmaOfBaseline_triggered();
    void on_actionDrawChannelWaves_triggered();
    void on_actionDrawEventWaves_triggered();
    void on_actionDrawMap_triggered();
    void on_actionDrawTrack_triggered();
    void on_actionDrawSpectrum_triggered();
    void on_actionDrawTest_triggered();

    void on_actionMiscSetup_triggered();
    void on_actionHV_Calculator_triggered();

    //void on_actionprint_wriggered();
    void on_widget_print_customContextMenuRequested(const QPoint &pos);
    //void clearMessagesAndResetProgress();清除信息的槽函数



};
class DebugMessageQueue
{
public:
    static DebugMessageQueue& instance()
    {
        static DebugMessageQueue instance;
        return instance;
    }

    void addMessage(const QString& message)
    {
         std::lock_guard<std::mutex> guard(mutex);  // 使用互斥锁保护队列操作
        messageQueue.push(message);
    }

    QString getNextMessage()
    {
        if (!messageQueue.empty())
        {
            QString message = messageQueue.front();
            messageQueue.pop();
            return message;
        }
        return QString();
    }

private:
    std::queue<QString> messageQueue;
    std::mutex mutex;
    DebugMessageQueue() {}
    DebugMessageQueue(const DebugMessageQueue&) = delete;
    DebugMessageQueue& operator=(const DebugMessageQueue&) = delete;
};
#endif // MAINWINDOW_H
