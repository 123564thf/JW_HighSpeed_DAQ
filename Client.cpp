#include "Client.h"
#include <qDebug.h>
#include <qtmetamacros.h>
#include <winsock2.h>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <vector>
#include <iostream>
#include"mainwindow.h"

#include <sstream>

//Client::Client(const char* ip, int port)
   // : iProgress(0)

Client::Client(const char* ip, int port)

    : iProgress(0)//表示进度的iprogress为0
   // , sMessage("")
   // , sMessageTemp("")//两个用于储存消息的字符串为空字符串

{
    //qDebug()<<"client==>"<<" in\n";
   // tEmitSignal = new std::thread(&Client::EmitSignal, this);//创建一个新的线程 该线程执行emitsignal成员函数
    memset(sendBuffer, 0, sizeof(sendBuffer));
    memset(recvBuffer, 0, sizeof(recvBuffer));
    recvTimeOutFlag = 0;
    // 其他代码

    printf("Create client!\n");
    DebugMessageQueue::instance().addMessage("Create client!\n");
   // //qDebug()<<"Create client!\n";
    //emit signalDebug(QString("Create client!\n"));

    //emit signalClient({iProgress, "Create client!\n"});
    std::cout << "IP: " << ip << std::endl;
    DebugMessageQueue::instance().addMessage(QString("IP: ").arg(ip));

  // emit signalClient({iProgress, "IP:" + QString::fromStdString(ip)});

    ////qDebug()<<"IP: " << ip ;
    //emit signalDebug(QString("IP: ").arg(ip));
    std::cout << "Port: " << port << std::endl;
    DebugMessageQueue::instance().addMessage(QString("Port: ").arg(port));
   // emit signalClient({iProgress, "Port:" + QString::number(port)});
    //qDebug()<<"IP: " << port;
    WSADATA wsaData;//定义一个wsadata类型的变量
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)//如果不等于0说明WSAstartup函数执行失败
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
    DebugMessageQueue::instance().addMessage(QString("WSAStartup failed: ").arg(result));
        if (tEmitSignal!= nullptr)
        {
            delete tEmitSignal;
            tEmitSignal = nullptr;
        }
       // exit(EXIT_FAILURE);
      return;
    }
    saddr.sin_family = AF_INET;//存储网络地址相关信息的结构体
    inet_pton(AF_INET, ip, &saddr.sin_addr.s_addr); // server ip
    saddr.sin_port = htons(port);


    createSocket();
    connectServer();

    //communicate();
    //qDebug()<<"client==>"<<" out\n";

}
Client::~Client()
{
    if (tEmitSignal!= nullptr && tEmitSignal->joinable())
    {
        tEmitSignal->join();  // 先等待线程正常结束
    }
    delete tEmitSignal;
    tEmitSignal = nullptr;
   // memset(sendBuffer, 0, sizeof(sendBuffer));
   // memset(recvBuffer, 0, sizeof(recvBuffer));
    //TerminateThread(tEmitSignal->native_handle(), 0);
   // delete tEmitSignal;
    //tEmitSignal = nullptr;
    close();
    closesocket(fd);
    WSACleanup();
    // 清理其他资源，重置状态变量和缓冲区
    memset(sendBuffer, 0, sizeof(sendBuffer));
    memset(recvBuffer, 0, sizeof(recvBuffer));
    recvTimeOutFlag = 0;
}
/*
void Client::EmitSignal()
{
    while (true)
    {
        if (sMessage != sMessageTemp)
        {
            sMessageTemp = sMessage;
            emit signalClient({iProgress, sMessage.c_str()});
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        if (iProgress != 0)
        {
            emit signalClient({iProgress, ""});
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}*/
//用于设置客户端连接服务器的相关参数
void Client::SetParameters(const ParamClient& param)
{
    //std::cout << "IP: " << param.psIP << std::endl;
    //std::cout << "Port: " << param.piPort << std::endl;
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
       // emit signalClient({iProgress, "WSAStartup failed: " + QString::number(result)});
        //sMessage = "WSAStartup failed: " + std::to_string(result);
        //exit(EXIT_FAILURE);
        return ;
    }
    saddr.sin_family = AF_INET;
    inet_pton(AF_INET, param.psIP.c_str(), &saddr.sin_addr.s_addr); // server IP
    saddr.sin_port = htons(param.piPort);


}
void Client::createSocket()
{
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == INVALID_SOCKET)
    {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        DebugMessageQueue::instance().addMessage(QString("socket failed: %1").arg(WSAGetLastError()));
        // 进行额外的清理工作，如调用 WSACleanup 等
        WSACleanup();
        return;
    }
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) == SOCKET_ERROR)
    {
        std::cerr << "setsockopt SO_REUSEADDR failed: " << WSAGetLastError() << std::endl;
        DebugMessageQueue::instance().addMessage(QString("setsockopt SO_REUSEADDR failed: %1").arg(WSAGetLastError()));
        closesocket(fd);
        WSACleanup();
        return;
    }
    printf("Create socket sucessfully!\n");
    DebugMessageQueue::instance().addMessage("Create socket sucessfully!");
}
/*
void Client::createSocket()
{
    // 1. create socket
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == INVALID_SOCKET)
    {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
    DebugMessageQueue::instance().addMessage(QString("socket failed: %1").arg(WSAGetLastError()));
       // emit signalClient({iProgress,"socket failed" +  QString::number(WSAGetLastError())});
        //qDebug()<<"socket failed: " << WSAGetLastError();
       // exit(EXIT_FAILURE);
      return;
    }
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) == SOCKET_ERROR)
    {
        std::cerr << "setsockopt SO_REUSEADDR failed: " << WSAGetLastError() << std::endl;
        DebugMessageQueue::instance().addMessage(QString("setsockopt SO_REUSEADDR failed: %1").arg(WSAGetLastError()));
    }
    printf("Create socket sucessfully!\n");
    DebugMessageQueue::instance().addMessage("Create socket sucessfully!");

  //  printf("Create socket sucessfully!\n");
   // DebugMessageQueue::instance().addMessage("Create socket sucessfully!");
    //emit signalClient({iProgress, "Create socket sucessfully!\n"});
    //qDebug()<<"Create socket sucessfully!\n";
}
*/
void Client::connectServer()
{
    // 2. bind IP and port to socket
    ret = ::connect(fd, (struct sockaddr*)&saddr, sizeof(saddr));

    if (ret == SOCKET_ERROR)
    {
         std::cerr << "connect failed: " << WSAGetLastError() << std::endl;
    DebugMessageQueue::instance().addMessage(QString("connect failed: ").arg(WSAGetLastError()));

    // emit signalClient({iProgress, "connect failed: " + QString::number(WSAGetLastError())});
    //qDebug()<<"connect failed: " << WSAGetLastError();
        //exit(EXIT_FAILURE);
      return;
    }

    printf("Socket connect sucessfully!\n");
    DebugMessageQueue::instance().addMessage("Socket connect sucessfully!\n");
    // emit signalClient({iProgress, "Socket connect sucessfully!\n"});
   //qDebug()<<"Socket connect sucessfully!\n";
}
void Client::sendData(const char* data)
{
    ret = send(fd, data, static_cast<int>(strlen(data)+1), 0);
    if (ret == SOCKET_ERROR)
    {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        DebugMessageQueue::instance().addMessage(QString( "send failed: ").arg(WSAGetLastError()));
        // 尝试重新连接和发送
        connectServer();
        ret = send(fd, data, static_cast<int>(strlen(data)+1), 0);
        if (ret == SOCKET_ERROR) {
            std::cerr << "send failed after reconnection: " << WSAGetLastError() << std::endl;
            DebugMessageQueue::instance().addMessage(QString("send failed after reconnection: ").arg(WSAGetLastError()));
            return;
        }
    }
   // printf("Send data %s!\n", data);
    QString message = QString("Send data %1!").arg((data));
    DebugMessageQueue::instance().addMessage(message);
}
/*
void Client::sendData(const char* data)
{
    ret = send(fd, data, static_cast<int>(strlen(data)+1), 0);
    if (ret == SOCKET_ERROR)
    {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
    DebugMessageQueue::instance().addMessage(QString( "send failed: ").arg(WSAGetLastError()));
        //emit signalClient({iProgress, "send failed: " + QString::number(WSAGetLastError())});
        //qDebug()<< "send failed: " << WSAGetLastError();
       // exit(EXIT_FAILURE);
      return;
    }
    printf("Send data %s!\n", data);
    QString message = QString("Send data %1!").arg((data));
    DebugMessageQueue::instance().addMessage(message);



     //emit signalClient({iProgress, "Send data: " + QString::fromLocal8Bit(data)});
    //qDebug()<< "Send data %s!\n", data;
}

*/
/*void Client::sendData(const char* data, int length)
{
    ret = send(fd, data, length, 0);
    if (ret == SOCKET_ERROR)
    {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        //exit(EXIT_FAILURE);
        //emit signalClient({iProgress, "send failed: " + QString::number(WSAGetLastError())});
        //sMessage = "send failed: " + std::to_string(WSAGetLastError());
        return;
    }
    //printf("Send data %s!\n", data);
   // emit signalClient({iProgress, "Send data: " + QString::fromLocal8Bit(data)});
    //sMessage = ("Send data: " + QString::fromLocal8Bit(data)).toStdString();
}

*/


/*
void Client::sendFile(const char* fileName)
{


    //struct SockInfo* info = (struct SockInfo*) arg;
    std::ifstream ifs(fileName, std::ios::binary);
    if (!ifs)
    {
        std::cerr << "open failed: " << GetLastError() << std::endl;
        DebugMessageQueue::instance().addMessage(QString("open failed: ").arg( GetLastError()));
        // emit signalClient({iProgress, "open failed: " + QString::number(GetLastError())});
       //qDebug()<< "open failed: " << GetLastError();
       // exit(EXIT_FAILURE);
          return;
    }
    memset(sendBuffer, 0, sizeof(sendBuffer));
    //ifs.read(info->sendBuffer, sizeof(info->sendBuffer));
    //send(fd, (uint8_t*)info->sendBuffer, sizeof(info->sendBuffer), 0);

    while (ifs.read(sendBuffer, sizeof(sendBuffer)).gcount() > 0)
    {
        ret = send(fd, sendBuffer, static_cast<int>(ifs.gcount()), 0);
        if (ret == SOCKET_ERROR)
        {
            std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            DebugMessageQueue::instance().addMessage(QString( "send failed: ").arg(WSAGetLastError()));
           //  emit signalClient({iProgress, "send failed: " + QString::number(WSAGetLastError())});
          //qDebug()<< "send failed: " << WSAGetLastError();
            //exit(EXIT_FAILURE);
              return;
        }
    }
    ifs.close();
    printf("File %s sent!\n", fileName);
   // QString message = QString("File %s sent!\n").arg(QString::fromLocal8Bit(fileName));
    //DebugMessageQueue::instance().addMessage(message);
    //emit signalClient({iProgress, "File " + QString::fromStdString(fileName) + " sent!"});
    //qDebug()<<"File %s sent!\n", fileName;


}
*/
void Client::sendFile(const char* fileName)
{
    std::ifstream ifs(fileName, std::ios::binary);
    if (!ifs)
    {
        std::cerr << "open failed: " << GetLastError() << std::endl;
        DebugMessageQueue::instance().addMessage(QString("open failed: ").arg( GetLastError()));
        return;
    }
    try {
        memset(sendBuffer, 0, sizeof(sendBuffer));
        while (ifs.read(sendBuffer, sizeof(sendBuffer)).gcount() > 0)
        {
            ret = send(fd, sendBuffer, static_cast<int>(ifs.gcount()), 0);
            if (ret == SOCKET_ERROR)
            {
                std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                DebugMessageQueue::instance().addMessage(QString( "send failed: ").arg(WSAGetLastError()));
                return;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during file send: " << e.what() << std::endl;
    }
    ifs.close();
    //printf("File %s sent!\n", fileName);
   // QString message = QString("File %s sent!\n").arg(QString::fromLocal8Bit(fileName));
   // DebugMessageQueue::instance().addMessage(message);
}


/*
std::thread Client::sendFileAsync(const char* fileName)
{
    return std::thread([=] { sendFile(fileName); });
}*/
std::thread Client::sendFileAsync(const char* fileName)
{
    return std::thread([=]() {
        try {
            sendFile(fileName);
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in sendFileAsync: " << e.what() << std::endl;
        }
    });
}
void Client::receiveData(char* data, int64_t& length)
{
    //char buf[1024];
    len = recv(fd, data, static_cast<int>(length), 0);
    if (len == SOCKET_ERROR)
    {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        DebugMessageQueue::instance().addMessage(QString("recv failed: ").arg(WSAGetLastError()));
        // emit signalClient({iProgress, "recv failed: " + QString::number(WSAGetLastError())});
        //qDebug()<<"recv failed: " << WSAGetLastError();
        //exit(EXIT_FAILURE);
          return;
    }
    else if (len == 0)
    {
        printf("server disconnected\n");
         DebugMessageQueue::instance().addMessage("server disconnected\n");
         //emit signalClient({iProgress, "Server disconnected!"});
         //qDebug()<<"server disconnected\n";
        //exit(EXIT_FAILURE);
           return;
    }
    else
    {

        printf("From server %lld byte: %s\n", len, data);
        QString message = QString("From server %lld byte: %s\n").arg(data,len);
        DebugMessageQueue::instance().addMessage(message);

        //qDebug()<<"From server %lld byte: %s\n", len, data;
        //send(cfd, buf, len, 0);
    }
    //data = buf;
}

void Client::receiveFile(std::string fileName, int64_t length, int64_t timeOut)
{
    // 设置接收超时时间
    //struct timeval recvTimeOut;
    //recvTimeOut.tv_sec = 30;
    //recvTimeOut.tv_usec = 0;
    std::cout << "Start receive file: " << fileName << std::endl;
    QString message = QString("Start receive file: ").arg(QString::fromLocal8Bit(fileName));
    DebugMessageQueue::instance().addMessage(message);
   // emit signalClient({iProgress, "Start receive file: " + QString::fromStdString(fileName)});
   //qDebug()<<"Start receive file: " << fileName;

    DWORD recvTimeOut = static_cast<DWORD>(30); // 1 minute
    // 设置套接字选项，SO_RCVTIMEO 选项用于设置接收超时。
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeOut, sizeof(recvTimeOut)) == SOCKET_ERROR)
    {
        std::cerr << "setsockopt failed: " << WSAGetLastError() << std::endl;
        DebugMessageQueue::instance().addMessage(QString("socketopt failed: ").arg(WSAGetLastError()));
      //  emit signalClient({iProgress, "setsockopt failed: " + QString::number(WSAGetLastError())});
        //qDebug()<<"setsockopt failed: " << WSAGetLastError();
        //exit(EXIT_FAILURE);
          return;
    }
    //struct SockInfo* info = (struct SockInfo*) arg;
    std::ofstream ofs(fileName, std::ios::binary | std::ios::app);
    if (!ofs)
    {
        std::cerr << "Open failed: " << GetLastError() << std::endl;
        DebugMessageQueue::instance().addMessage(QString("Open failed: ").arg(GetLastError()));
       // emit signalClient({iProgress,"Open failed: " + QString::number(GetLastError())});
       //qDebug()<< "Open failed: " << GetLastError();
        //exit(EXIT_FAILURE);
          return;
    }
    int64_t remLen = length;
    int64_t recvLen = 0;
    memset(recvBuffer, 0, sizeof(recvBuffer));
    //std::vector<char> recvBuffer;  // 使用动态分配的缓冲区

    auto startTime = std::chrono::steady_clock::now(); // Record start time
    auto endTime = startTime + std::chrono::minutes(timeOut); // Calculate end time
    iProgress = 0;
    emit progressChanged(iProgress);

    while (remLen > 0 || std::chrono::steady_clock::now() < endTime)
    {
        recvTimeOutFlag = 0;
        int64_t n = recv(fd, recvBuffer, sizeof(recvBuffer), 0);
         //recvBuffer.resize(std::min(static_cast<size_t>(1024), static_cast<size_t>(remLen)));  // 根据剩余长度调整缓冲区大小
         //int64_t n = recv(fd, recvBuffer.data(), static_cast<int>(recvBuffer.size()), 0);
        if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                printf("Receive time out!\n");
                DebugMessageQueue::instance().addMessage("Receive time out!\n");

                //qDebug()<< "Receive time out!\n";
               // emit signalClient({iProgress,"Receive time out!\n"});



                recvTimeOutFlag = std::chrono::duration_cast<std::chrono::minutes>(endTime - std::chrono::steady_clock::now()).count();
                break;
            }
            else
            {

                fprintf(stderr, "Recv: %s (%d)\n", strerror(errno), errno);


                continue;
            }
        }
        else if (n == 0)
        //if (n == 0)
        {
            printf("server disconnected\n");
            DebugMessageQueue::instance().addMessage("server disconnected\n");
           // emit signalClient({iProgress,"server disconnected\n"});
            //qDebug()<< "server disconnected\n";
           //exit(EXIT_FAILURE);
              return;
        }
        else
        {
            ofs.write(recvBuffer, n);
            //ofs.write(recvBuffer.data(), n);
            remLen -= n;
            recvLen += n;
            //printf("received %l\n", n);
            //std::cout << "\rReceived " << n << std::flush;
        }
        if (remLen > 0)
        {
            std::cout << "\rSize Receive progress: " << (length-remLen)/(double)(length)*100. << "%: " << (double)recvLen/(1024*1024) << " MB / " << (double)length/(1024*1024) << " MB " << std::flush;

             //qDebug()<< "\rSize Receive progress: " << (length-remLen)/(double)(length)*100. << "%: " << (double)recvLen/(1024*1024) << " MB / " << (double)length/(1024*1024) << " MB ";
            iProgress = (length-remLen)/(double)(length)*100;
            emit progressChanged(iProgress);
        }
        else 
        {
            double spentTime = static_cast<double>((std::chrono::steady_clock::now() - startTime).count());
            double totalTime = static_cast<double>((endTime - startTime).count());
            //std::cout << "\rTime Receive progress: " << static_cast<double>((std::chrono::steady_clock::now() - startTime).count()) / (double)(endTime - startTime).count() * 100. << "%: " << (double)recvLen/(1024*1024) << "MB";// << std::flush;
            std::cout << "\rTime Receive progress: " << spentTime / totalTime * 100. << "%: " << spentTime/(1e9) << " s / " << totalTime/(1e9) << " s: " << (double)recvLen/(1024*1024) << "MB \t" << std::flush;// << std::flush;
            //qDebug() << "\rTime Receive progress: " << spentTime / totalTime * 100. << "%: " << spentTime/(1e9) << " s / " << totalTime/(1e9) << " s: " << (double)recvLen/(1024*1024) << "MB \t" ;
            iProgress = spentTime / totalTime * 100;

            emit progressChanged(iProgress);
        }
        //std::cout.flush();
    }
    iProgress = 100;
    emit progressChanged(iProgress);
    std::cout << std::endl;
    ofs.close();
    if (recvTimeOutFlag == 0)
    {
        //printf("File %s received!\n", fileName.c_str());
         DebugMessageQueue::instance().addMessage(QString("File  received!\n").arg(fileName.c_str()));

        //emit signalClient({iProgress, "File %s received!\n" + QString::fromStdString(fileName.c_str())});
        //qDebug() <<"File %s received!\n", fileName.c_str();
    }
}

std::thread Client::receiveFileAsync(std::string fileName, int64_t length, int64_t timeOut)
{
    return std::thread([=] {receiveFile(fileName, length, timeOut); });
}

void Client::initForConnection()
{
    // 重置文件发送相关状态，假设sendBuffer是用于发送文件数据的缓冲区
    memset(sendBuffer, 0, sizeof(sendBuffer));
    // 如果涉及文件接收相关的缓冲区，也进行重置
    memset(recvBuffer, 0, sizeof(recvBuffer));
    iProgress = 0;
    emit progressChanged(iProgress);
}
int Client::configDPU(std::vector<std::string> sendFileNameVec, std::string receiveFileName, int64_t receiveLength, int64_t receiveTimeOut)
{
    initForConnection();  // 先进行初始化操作

    std::cout << "Current thread ID: " << std::this_thread::get_id() << std::endl;
    // 新增计数器变量，初始化为 0，用于记录发送文件的数量
    int fileCount = 0;

    std::cout << "SendFileName: " << sendFileNameVec[0] << std::endl;
   DebugMessageQueue::instance().addMessage(QString("SendFileName: ") + QString::fromStdString(sendFileNameVec[0]));
   ////qDebug() <<"SendFileName: " << sendFileNameVec[0];
   sendFile(sendFileNameVec[0].c_str());
    Sleep(500);

    std::cout << "SendFileName: " << sendFileNameVec[1] << std::endl;
    DebugMessageQueue::instance().addMessage(QString("SendFileName: ") + QString::fromStdString(sendFileNameVec[1]));
   ////qDebug() <<"SendFileName: " << sendFileNameVec[1];
    sendFile(sendFileNameVec[1].c_str());
    Sleep(500);

    std::cout << "SendFileName: " << sendFileNameVec[2] << std::endl;
    DebugMessageQueue::instance().addMessage(QString("SendFileName: ") + QString::fromStdString(sendFileNameVec[2]));
    ////qDebug() <<"SendFileName: " << sendFileNameVec[2];
    sendFile(sendFileNameVec[2].c_str());
    Sleep(500);

    std::cout << "SendFileName: " << sendFileNameVec[3] << std::endl;
    DebugMessageQueue::instance().addMessage(QString("SendFileName: ") + QString::fromStdString(sendFileNameVec[3]));
    ////qDebug() <<"SendFileName: " << sendFileNameVec[3];
    sendFile(sendFileNameVec[3].c_str());
    Sleep(500);

    std::cout << "SendFileName: " << sendFileNameVec[4] << std::endl;
    DebugMessageQueue::instance().addMessage(QString("SendFileName: ") + QString::fromStdString(sendFileNameVec[4]));
   ////qDebug() <<"SendFileName: " << sendFileNameVec[4];
    sendFile(sendFileNameVec[4].c_str());
    Sleep(500);

    std::cout << "SendFileName: " << sendFileNameVec[5] << std::endl;
    DebugMessageQueue::instance().addMessage(QString("SendFileName: ") + QString::fromStdString(sendFileNameVec[5]));
   ////qDebug() <<"SendFileName: " << sendFileNameVec[5];
    sendFile(sendFileNameVec[5].c_str());
    Sleep(500);

    for (int i = 6; i < (sendFileNameVec.size()-2); i++)
    {
        std::cout << "SendFileName: " << sendFileNameVec[i] << std::endl;
       // DebugMessageQueue::instance().addMessage(QString("SendFileName: ") + QString::fromStdString(sendFileNameVec[i]));
         ////qDebug() <<"SendFileName: " << sendFileNameVec[i];
        sendFile(sendFileNameVec[i].c_str());
        // 计数器加 1，表示发送了一个文件
        fileCount++;

        // 检查计数器是否是 500 的倍数
        if (fileCount % 500 == 0)
        {
            // 当发送文件数量达到 500 的倍数时，添加日志信息
            DebugMessageQueue::instance().addMessage(QString("Sent 500 files so far."));
        }

        Sleep(100);
    }

    std::cout << "ReceiveFileName: " << receiveFileName << std::endl;
    DebugMessageQueue::instance().addMessage(QString("ReceiveFileName: ") + QString::fromStdString(receiveFileName));
     ////qDebug() <<"ReceiveFileName: " << receiveFileName ;
    std::thread receive1 = receiveFileAsync(receiveFileName, receiveLength, receiveTimeOut);
    Sleep(500);

    std::cout << "SendFileName: " << sendFileNameVec[sendFileNameVec.size()-2] << std::endl;
    DebugMessageQueue::instance().addMessage(QString("SendFileName: ") + QString::fromStdString(sendFileNameVec[sendFileNameVec.size()-2]));
    ////qDebug() <<"SendFileName: " << sendFileNameVec[sendFileNameVec.size()-2] ;
    sendFile(sendFileNameVec[sendFileNameVec.size()-2].c_str());
    receive1.join();
    Sleep(500);

    //std::cout << "ReceiveFileName: " << receiveFileName << std::endl;
    //receiveFile(receiveFileName, receiveLength, receiveTimeOut);
    //Sleep(500);


    std::cout << "SendFileName: " << sendFileNameVec[sendFileNameVec.size()-1] << std::endl;
    ////qDebug() <<"SendFileName: " << sendFileNameVec[sendFileNameVec.size()-1] ;

    //debuge 调式信息
    ////qDebug()<<"SendFileName:"<< sendFileNameVec[sendFileNameVec.size()-1];
    Sleep(500);
    sendFile(sendFileNameVec[sendFileNameVec.size()-1].c_str());

    if (recvTimeOutFlag != 0)
    {
        return recvTimeOutFlag;
    }
    else
    {
        return 0;
    }




}

//void Client::communicate()
//{
//    int number = 1;
//    // 3. communicate
//    while (1)
//    {
//        // send 
//        char buf[1024];
//        sprintf(buf, "hello, server, %d... \n", number++);
//        send(fd, buf, strlen(buf)+1, 0);
//
//        // reveive
//        memset(buf, 0, sizeof(buf));
//        len = recv(fd, buf, sizeof(buf), 0);
//        if (len == -1)
//        {
//            perror("recv");
//            exit(EXIT_FAILURE);
//        }
//        else if (len == 0)
//        {
//            printf("server disconnected\n");
//            break;
//        }
//        else
//        {
//            printf("recv: %s\n", buf);
//            //send(cfd, buf, len, 0);
//        }   
//        Sleep(500);
//    }
//}
/*
void Client::close()
{    iProgress = 0;
    emit progressChanged(iProgress);

    // 先尝试关闭套接字
    if (fd!= INVALID_SOCKET)
    {
         closesocket(fd);
    }
    // 终止并释放线程相关资源
    if (tEmitSignal!= nullptr)
    {
        TerminateThread(tEmitSignal->native_handle(), 0);
        delete tEmitSignal;
        tEmitSignal = nullptr;
    }

    // 清理Windows Sockets相关资源
    WSACleanup();
  //  closesocket(fd);
}
*/
void Client::close()
{
    iProgress = 0;
    emit progressChanged(iProgress);

    // 先尝试关闭套接字
    if (fd!= INVALID_SOCKET)
    {
        closesocket(fd);
    }

    // 终止并释放线程相关资源，如果线程存在则正确终止并释放
    if (tEmitSignal!= nullptr)
    {
        TerminateThread(tEmitSignal->native_handle(), 0);
        delete tEmitSignal;
        tEmitSignal = nullptr;
    }

    // 清理Windows Sockets相关资源
    WSACleanup();

}
