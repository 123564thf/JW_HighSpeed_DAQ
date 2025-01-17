#pragma once

#include <QDialog>
#include <QSettings>

namespace Ui {
class SocketConfigurations;
}

class SocketConfigurations : public QDialog
{
    Q_OBJECT

public:
    explicit SocketConfigurations(QWidget *parent = nullptr);
    ~SocketConfigurations();

    std::string getIP() { return ip; }
    int getPort() { return port; }
    std::string getSaveToFileName() { return saveToFileName; }
    int64_t getSaveToLength() { return saveToLength; }
    std::vector<std::string> getSaveToFileNameVec() { return sendFileNameVec; }

signals:
    void socketConfigurationsAccepted(std::string ip, int port, std::string saveToFileName, int64_t saveToLength, std::vector<std::string> sendFileNameVec);

private slots:
    void on_lineEditIPAddr_editingFinished();
    void on_lineEditPort_editingFinished();
    void on_lineEditSaveTo_editingFinished();
    void on_pushButtonOpenSaveTo_clicked();
    void on_lineEditSaveFileLength_editingFinished();
    void on_textEditSendFiles_textChanged();
    void on_pushButtonOpenSendFiles_clicked();
    void on_buttonBoxSocket_accepted();

private:
    Ui::SocketConfigurations *ui;
    QSettings* settings;
    std::string ip = "192.168.10.16";
    int port = 4660;
    std::string saveToFileName;
    int64_t saveToLength = 10;
   // std::vector<std::string> sendFileNameVec = {"./1.dat", "./2.dat", "./3.dat", "./4_hit.dat", "./5.dat", "./6.dat", "./7.dat", "./8.dat"};
    std::vector<std::string> sendFileNameVec = {"cmd/1.dat", "cmd/2.dat", "cmd/3.dat", "cmd/4_hit.dat", "cmd/5.dat", "cmd/6.dat", "cmd/7.dat", "cmd/8.dat"};

};
