#include "formdrawtrack.h"
#include "../ui/ui_formdrawtrack.h"

FormDrawTrack::FormDrawTrack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormDrawTrack)
{
    ui->setupUi(this);
    ui->spinBoxEventID->setValue(eventID);
}

FormDrawTrack::~FormDrawTrack()
{
    delete ui;
}

void FormDrawTrack::on_spinBoxEventID_valueChanged(int arg1)
{
    eventID = arg1;
    emit signalDrawTrack(eventID);
}