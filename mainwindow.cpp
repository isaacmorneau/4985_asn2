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
    std::string dest;
    int port, size, number;

    port = ui->lineEditPort->text().toInt();
    //client specific
    if(ui->comboBoxMode->currentIndex() == 0){
        dest = ui->lineEditDestination->text().toStdString();
        size = ui->lineEditPacketSize->text().toInt();
        number = ui->lineEditPacketNum->text().toInt();
        std::thread client;
        if(ui->comboBoxProto->currentIndex() == 0)
            client = std::thread(clientTCP, this, dest, port, size, number);
        else
            client = std::thread(clientUDP, this, dest, port, size, number);
        client.detach();
    } else {
        //start server
        std::thread server;
        if(ui->comboBoxProto->currentIndex() == 0)
            server = std::thread(serverTCP,this, port);
        else
            server = std::thread(serverUDP,this, port);
        server.detach();
    }
}

void MainWindow::messageAdd_slot(std::string s){
    //make text appened on a new line
    ui->textBrowserOutput->append(s.c_str());
    ui->textBrowserOutput->append("\n");
}

void MainWindow::messageClear_slot(){
    ui->textBrowserOutput->clear();
}
