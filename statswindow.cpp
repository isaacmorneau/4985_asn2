#include "statswindow.h"
#include "ui_statswindow.h"

#include <QtCharts>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

QT_CHARTS_USE_NAMESPACE

StatsWindow::StatsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatsWindow),
    seriesPacket(new QBarSeries),
    seriesSize(new QBarSeries),
    chart(new QChart)
{
    ui->setupUi(this);

    QBarSet *setTotal = new QBarSet("Total Packets");
    QBarSet *setLost = new QBarSet("Packets Lost");
    QBarSet *setSize = new QBarSet("Amount Transfer");

    QStringList categories;

    //get actual test data here and build the sets out of it

    *setSize << 200 << 350 << 500 << 1000 << 460 << 210;
    *setTotal << 10 << 30 << 60 << 100 << 40 << 20;
    *setLost << 0 << 1 << 3 << 8 << 2 << 1;

    categories << "Test 1" << "Test 2" << "Test 3" << "Test 4" << "Test 5";

    chart->setTitle("Test Result Stats");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QValueAxis *axisYPacket = new QValueAxis;
    QValueAxis *axisYSize = new QValueAxis;
    QBarCategoryAxis *axisX = new QBarCategoryAxis;

    seriesSize->append(setSize);
    seriesPacket->append(setTotal);
    seriesPacket->append(setLost);

    //size first
    chart->addSeries(seriesSize);
    //then packets
    chart->addSeries(seriesPacket);

    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    axisYSize->setTitleText("Transfer Size");
    chart->addAxis(axisYSize, Qt::AlignLeft);
    axisYPacket->setTitleText("Number of Packets");
    chart->addAxis(axisYPacket, Qt::AlignRight);

    seriesSize->attachAxis(axisX);
    seriesSize->attachAxis(axisYSize);

    seriesPacket->attachAxis(axisX);
    seriesPacket->attachAxis(axisYPacket);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    setCentralWidget(chartView);
}

StatsWindow::~StatsWindow()
{
    delete ui;
    //dont delete the other QtChart pointers, they will be removed by qt
}
