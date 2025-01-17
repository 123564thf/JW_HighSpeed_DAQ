#ifndef FORMDRAWSEVERALWAVES_H
#define FORMDRAWSEVERALWAVES_H

#include <QWidget>

namespace Ui {
class FormDrawSeveralWaves;
}

class FormDrawSeveralWaves : public QWidget
{
    Q_OBJECT

public:
    explicit FormDrawSeveralWaves(QWidget *parent = nullptr);
    ~FormDrawSeveralWaves();

signals:
    void signalDrawSeveralWaves(int nBegin, int nFigure);

private slots:
    void on_spinBoxNBegin_valueChanged(int arg1);
    void on_spinBoxNFigure_valueChanged(int arg1);

private:
    Ui::FormDrawSeveralWaves *ui;
    int nBegin = 0;
    int nFigure = 100;
};

#endif // FORMDRAWSEVERALWAVES_H
