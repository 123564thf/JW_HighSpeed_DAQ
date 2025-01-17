#ifndef FORMDRAWCHANNELWAVES_H
#define FORMDRAWCHANNELWAVES_H

#include <QWidget>

namespace Ui {
class FormDrawChannelWaves;
}

class FormDrawChannelWaves : public QWidget
{
    Q_OBJECT

public:
    explicit FormDrawChannelWaves(QWidget *parent = nullptr);
    ~FormDrawChannelWaves();


signals:
    void signalDrawChannelWaves(int channel, int nBeg, int nFig);


private slots:
    void on_spinBoxChannel_valueChanged(int arg1);
    void on_spinBoxChannelNBeg_valueChanged(int arg1);
    void on_spinBoxChannelNFig_valueChanged(int arg1);

private:
    Ui::FormDrawChannelWaves *ui;
    int channel = 0;
    int nBeg = 0;
    int nFig = 1;
};

#endif // FORMDRAWCHANNELWAVES_H
