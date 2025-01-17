#ifndef DIALOGBASELINE_H
#define DIALOGBASELINE_H

#include <QDialog>
#include <QSettings>
#include <cstdint>

namespace Ui {
class DialogBaseline;
}

class DialogBaseline : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBaseline(QWidget *parent = nullptr);
    ~DialogBaseline();

signals:
    void baselineSetupAccepted(std::string fileName, std::string filterFileName, int fileSize, int timeOut);

private slots:
    void on_buttonBoxBaseline_accepted();
    void on_lineEditSaveTo_editingFinished();
    void on_lineEditFilterFile_editingFinished();
    void on_pushButtonOpenFilterFile_clicked();
    void on_lineEditSaveFileLength_editingFinished();
    void on_lineEditTimeOut_editingFinished();

private:
    Ui::DialogBaseline *ui;
    QSettings *settings = nullptr;
    char timeStr[64];
    std::string saveToFileName;
    std::string filterFileName = "";
    int64_t saveToLength = 10;
    int64_t timeOut = 0;

};

#endif // DIALOGBASELINE_H
