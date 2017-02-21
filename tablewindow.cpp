#include "tablewindow.h"
#include "ui_tablewindow.h"
#include "testset.h"
#include <tuple>
#include <QTableWidget>
#include <string>

TableWindow::TableWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TableWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowTitleHint);
    updateData();
}

TableWindow::~TableWindow()
{
    delete ui;
}

void TableWindow::updateData(){
    auto test = TestSet::getTestSets();
    int size = test->size();
    ui->tableWidget->setRowCount(size);
    //test + sent + lost + size + protocol == 4
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Packets Sent"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Packets Lost"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Transfer Size"));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("Protocol"));
    if(!size)
        return;
    for(int i = 0; i < size; ++i){
        auto t = test->at(i);
        ui->tableWidget->setVerticalHeaderItem(i, new QTableWidgetItem("Test " + QString::number(i+1)));
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(std::get<0>(t))));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(std::get<1>(t))));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(std::get<2>(t))));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(std::get<3>(t).c_str()));
    }
}
