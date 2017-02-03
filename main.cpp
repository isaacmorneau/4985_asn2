#include "mainwindow.h"
#include <QApplication>
#include <thread>

int startQT(int argc, char *argv[]){
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

void testing(){

}

int main(int argc, char *argv[]){
    int ret;
    std::thread worker(testing);
    ret = startQT(argc,argv);
    worker.join();
    return ret;
}
