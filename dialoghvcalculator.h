#ifndef DIALOGHVCALCULATOR_H
#define DIALOGHVCALCULATOR_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class DialogHVCalculator;
}

struct HVCalculator
{
    double Ea_Ed;
    double Ea;
    double Vm;
    double Vfc;
    double Va;
    double La;
    double Lfc;
    double Ld;
};

class DialogHVCalculator : public QDialog
{
    Q_OBJECT

public:
    explicit DialogHVCalculator(QWidget *parent = nullptr);
    ~DialogHVCalculator();
    void calculateHV();
    void calculatePmt();
    void calculateMF();
    void calculateFM();

signals:
    void hvCalculatorAccepted(HVCalculator hvCalculator);

private slots:
    void on_doubleSpinBoxEa_Ed_editingFinished();
    void on_doubleSpinBoxEa_editingFinished();
    void on_doubleSpinBoxVm_editingFinished();
    void on_doubleSpinBoxVfc_editingFinished();
    void on_doubleSpinBoxVa_editingFinished();
    void on_doubleSpinBoxLa_editingFinished();
    void on_doubleSpinBoxLfc_editingFinished();
    void on_doubleSpinBoxLd_editingFinished();
    void on_buttonBox_accepted();

private:
    Ui::DialogHVCalculator *ui;
    QSettings* settings;
    HVCalculator hvCalculator;
    //double Ea_Ed = 240;
    //double Ea = 380;
    //double Vm = .0;
    //double Vfc = .0;
    //double Va = .0;
    //double La = 0.1;
    //double Lfc = 51;
    //double Ld = 53;
};

#endif // DIALOGHVCALCULATOR_H
