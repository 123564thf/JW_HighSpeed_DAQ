#ifndef DIALOGTRIGGER_H
#define DIALOGTRIGGER_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class DialogTrigger;
}

class DialogTrigger : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTrigger(QWidget *parent = nullptr);
    ~DialogTrigger();

signals:
    void triggerSetupAccepted(std::string fileName, std::string thresholdFileName, int fileSize, int timeOut);

private slots:
    void on_buttonBoxTrigger_accepted();
    void on_lineEditSaveTo_editingFinished();
    void on_lineEditThresholdFile_editingFinished();
    void on_pushButtonOpenThresholdFile_clicked();
    void on_lineEditSaveFileLength_editingFinished();
    void on_lineEditTimeOut_editingFinished();

private:
    Ui::DialogTrigger *ui;
    QSettings* settings;
    char timeStr[64];
    std::string saveToFileName;
    std::string thresholdFileName = "";
    int saveToLength = 10;
    int timeOut = 0;
};

#endif // DIALOGTRIGGER_H
