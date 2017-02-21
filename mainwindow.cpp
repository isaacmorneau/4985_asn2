#include "networkthreader.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "statswindow.h"
#include "testset.h"
#include <QDialog>
#include <string>
#include <thread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stats(new StatsWindow)
{
    ui->setupUi(this);
    //to facilitate the hack for sending data to the window
    mainwindowptr = this;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete stats;
}

void MainWindow::messageAdd_slot(std::string s){
    //make text appened on a new line
    ui->textBrowserOutput->append(s.c_str());
}

void MainWindow::messageSet_slot(int percent){
    ui->progressBarL->setValue(percent);
}

void MainWindow::messageClear_slot(){
    ui->textBrowserOutput->clear();
}

void MainWindow::on_pushButtonStart_clicked()
{
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);

    //everything needs this so do it before starting them
    WSADATA wsaData;
    if(WSAStartup(0x0202,&wsaData) != 0){
        resultError("WSAStartup Failed.");
        return;
    }

    std::string dest;
    int port, size, number;

    if(ui->tabClientServer->currentIndex() == 0){//client
        port = ui->lineEditPortClient->text().toInt();
        dest = ui->lineEditDestination->text().toStdString();
        size = ui->lineEditPacketSize->text().toInt();
        //file or junk
        number = ui->lineEditPacketNum->text().toInt();
        std::thread client;
        if(ui->comboBoxProto->currentIndex() == 0)
            client = std::thread(clientTCP, dest, port, size, number);
        else
            client = std::thread(clientUDP, dest, port, size, number);
        client.detach();
    } else {//server
        port = ui->lineEditPortServer->text().toInt();

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

void MainWindow::on_pushButtonStop_clicked()
{
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
    if(!sharedInfo.running)
        return;
    sharedInfo.running= false;
    closesocket(sharedInfo.sharedSocket);
    //dont leave memory lying about
    if(sharedInfo.buffer != 0)
        free(sharedInfo.buffer);
    sharedInfo.buffer = 0;

    if(WSACleanup() != 0){
        resultError("WSACleanup Failed.");
    }
}

void MainWindow::on_checkBoxStats_clicked()
{
    if(ui->checkBoxStats->isChecked()) {
        QMetaObject().invokeMethod(stats, "updateData");
        stats->show();
    } else {
        stats->hide();
    }
}

void MainWindow::on_pushButtonResetTest_clicked()
{
   TestSet::getTestSets()->clear();
}

void MainWindow::on_pushButtonClientFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("All Files (*.*)"));
    ui->lineEditFileNameClient->setText(fileName);
}

void MainWindow::on_pushButtonServerFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("All Files (*.*)"));
    ui->lineEditFileNameServer->setText(fileName);
}
