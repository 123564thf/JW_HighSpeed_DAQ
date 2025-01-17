#include "mainwindow.h"

#include <random>
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "data_struct_cint.h"
#include <QApplication>


void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
