#pragma once //预处理指令，确保文件在一个编译单元中只被包含一次避免重复定义

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif
#include <qobject.h>
#include <cstdint>
#include <thread>
#include <vector>//动态数组容器
#include <QThread>



#define NOCHAR 1024
QT_BEGIN_NAMESPACE
namespace Ui {
class Client;
}
QT_END_NAMESPACE
struct ClientSignal
{
    int iProgress;
    QString sMessage;
};

struct ParamClient
{
    int piPort;
    std::string psIP;
};


class Client : public QObject
{
    Q_OBJECT
private:
    // 1. create listening socket
    int fd;

    // 2. connect IP and port to socket
    struct sockaddr_in saddr;
    int ret;

    int64_t len;


    //std::vector<uint8_t> sendBuffer;
    char sendBuffer[NOCHAR];
    char recvBuffer[NOCHAR];
    int recvTimeOutFlag = 0;
    int iProgress;
    std::string sMessage;
    std::string sMessageTemp;
    std::thread *tEmitSignal;
    void EmitSignal();


public:
    Client() = default;
    Client(const char* ip, int port);

    ~Client();
   void SetParameters(const ParamClient& param);
    void createSocket();
    void connectServer();
    void sendData(const char* data);
    void sendFile(const char* fileName);
    std::thread sendFileAsync(const char* fileName);
    void receiveData(char* data, int64_t& length);
    void receiveFile(std::string fileName, int64_t length = 0, int64_t timeOut = 0);
    std::thread receiveFileAsync(std::string fileName, int64_t length = 0, int64_t timeOut = 0);
    int configDPU(std::vector<std::string> sendFileNameVec, std::string receiveFileName, int64_t receiveLength = 0, int64_t receiveTimeOut = 0);
    void close();
    void initForConnection();


signals:
    void progressChanged(int value);
signals:
   void printInfo(const QString& info);//定义一个信号来传递要打印的信息
signals:
    void signalClient(ClientSignal);
    void signalDebug(const QString& debugMessage);
};
