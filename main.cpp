#include "mainwindow.h"
#include <QApplication>
#include <string>
int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //tell qt that if something is called string that it is one
    qRegisterMetaType<std::string>("std::string");
    return a.exec();
}
