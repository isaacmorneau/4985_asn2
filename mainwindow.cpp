#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "networkthreader.h"
#include <string>
#include <thread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonStart_clicked()
{
    /*
    std::string dest;
    int port, size, number;

    port = ui->lineEditPort->text().toInt();
    //client specific
    if(ui->comboBoxMode->currentIndex() == 0){
        dest = ui->lineEditDestination->text().toStdString();
        size = ui->lineEditPacketSize().toInt();
        number = ui->lineEditPacketNum().toInt();
        client(dest,port,size,number);
        return;
    }
    //start server
    server(port);
    */
}

void MainWindow::on_pushButton_clicked()
{
   std::thread worker(tryit,this);
   worker.join();
}

void MainWindow::messageHandler_slot(QString s){
    ui->textBrowserOutput->setText(s);
}
