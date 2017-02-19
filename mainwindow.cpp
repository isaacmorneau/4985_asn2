#include "networkthreader.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "statswindow.h"
#include <string>
#include <thread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stats(0)
{
    ui->setupUi(this);
    //to facilitate the hack for sending data to the window
    mainwindowptr = this;
}

MainWindow::~MainWindow()
{
    delete ui;
    if(stats)
        delete static_cast<StatsWindow*>(stats);
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
            client = std::thread(clientTCP, dest, port, size, number);
        else
            client = std::thread(clientUDP, dest, port, size, number);
        client.detach();
    } else {
        size = ui->lineEditBuffer->text().toInt();
        //start server
        std::thread server;
        if(ui->comboBoxProto->currentIndex() == 0)
            server = std::thread(serverTCP, port, size);
        else
            server = std::thread(serverUDP, port, size);
        server.detach();
    }
}

void MainWindow::messageAdd_slot(std::string s){
    //make text appened on a new line
    ui->textBrowserOutput->append(s.c_str());
}

void MainWindow::messageSet_slot(int percent){
    ui->progressBarL->setValue(percent);
    ui->progressBarR->setValue(percent);
}

void MainWindow::messageClear_slot(){
    ui->textBrowserOutput->clear();
}

void MainWindow::on_pushButtonStop_clicked()
{
    if(!sharedInfo.running)
        return;
    sharedInfo.running= false;
    closesocket(sharedInfo.sharedSocket);
    //dont leave memory lying about
    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
    sharedInfo.buffer = 0;
    WSACleanup();
}

void MainWindow::on_checkBoxStats_clicked()
{
    if(!stats)
        stats = new StatsWindow();
    //it refuses to regcognize the type for use as a member
    StatsWindow *stat = static_cast<StatsWindow*>(stats);
    if(ui->checkBoxStats->isChecked()) {
        stat->show();
    } else {
        stat->hide();
    }
}
