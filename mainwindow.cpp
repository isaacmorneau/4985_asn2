#include "wrapper.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "statswindow.h"
#include "networkthreader.h"
#include "testset.h"
#include <QDialog>
#include <string>
#include <thread>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stats(new StatsWindow),
    table(new TableWindow)

{
    ui->setupUi(this);
    //to facilitate the hack for sending data to the window
    mainwindowptr = this;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete stats;
    delete table;
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
    bool usingFile = false;

    if(ui->tabClientServer->currentIndex() == 0){//client
        //vet ui
        if(ui->radioButtonFile->isChecked()){
            if(ui->lineEditFileNameClient->text().size() == 0){
                ui->textBrowserOutput->append("Please Select A File");
                return;
            } else {
                usingFile = true;
            }
        }

        //init
        if((port = ui->lineEditPortClient->text().toInt())<1){
            ui->textBrowserOutput->append("Port must be positive integer");
            return;
        }
        dest = ui->lineEditDestination->text().toStdString();
        size = ui->lineEditPacketSize->text().toInt();
        if(!usingFile && size < 1){
            ui->textBrowserOutput->append("Size must be positive integer");
            return;
        }

        number = ui->lineEditPacketNum->text().toInt();
        if(!usingFile && number < 1){
            ui->textBrowserOutput->append("Number must be positive integer");
            return;
        }

        std::thread client;
        if(ui->comboBoxProto->currentIndex() == 0)
            client = std::thread(clientTCP, dest, port, size, number,
                                 usingFile ? ui->lineEditFileNameClient->text().toStdString() : "");
        else
            client = std::thread(clientUDP, dest, port, size, number,
                                 usingFile ? ui->lineEditFileNameClient->text().toStdString() : "");
        client.detach();
    } else {//server
        //vet ui
        if(ui->radioButtonSave->isChecked()){
            if(ui->lineEditFileNameServer->text().size() == 0){
                ui->textBrowserOutput->append("Please Select A File");
                return;
            } else {
                usingFile = true;
            }
        }
        if((port = ui->lineEditPortServer->text().toInt())<1){
            ui->textBrowserOutput->append("Port must be positive integer");
            return;
        }

        size = ui->lineEditBuffer->text().toInt();
        if(!usingFile && size < 1){
            ui->textBrowserOutput->append("Size must be positive integer");
            return;
        }

        std::thread server;
        if(ui->comboBoxProto->currentIndex() == 0)
            server = std::thread(serverTCP, port, size,
                                 usingFile ? ui->lineEditFileNameServer->text().toStdString() : "");
        else
            server = std::thread(serverUDP, port, size,
                                 usingFile ? ui->lineEditFileNameServer->text().toStdString() : "");
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
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Open File"), "", tr("All Files (*.*)"));
    ui->lineEditFileNameServer->setText(fileName);
}

void MainWindow::on_checkBoxTable_clicked()
{
    if(ui->checkBoxTable->isChecked()) {
        QMetaObject().invokeMethod(table, "updateData");
        table->show();
    } else {
        table->hide();
    }
}
