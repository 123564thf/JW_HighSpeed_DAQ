#ifndef FORMDRAWTRACK_H
#define FORMDRAWTRACK_H

#include <QWidget>

namespace Ui {
class FormDrawTrack;
}

class FormDrawTrack : public QWidget
{
    Q_OBJECT

public:
    explicit FormDrawTrack(QWidget *parent = nullptr);
    ~FormDrawTrack();

private:
    Ui::FormDrawTrack *ui;
    int eventID = 0;

signals:
    void signalDrawTrack(int eventID);

private slots:
    void on_spinBoxEventID_valueChanged(int arg1);
};

#endif // FORMDRAWTRACK_H
