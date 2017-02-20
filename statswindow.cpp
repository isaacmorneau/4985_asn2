#include <QtCharts>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <string>

#include "statswindow.h"
#include "ui_statswindow.h"
#include "testset.h"

QT_CHARTS_USE_NAMESPACE

StatsWindow::StatsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatsWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowTitleHint);
}

void StatsWindow::updateData(){
    QValueAxis *axisYPacket = new QValueAxis;
    QValueAxis *axisYSize = new QValueAxis;
    QBarCategoryAxis *axisX = new QBarCategoryAxis;
    QtCharts::QBarSeries *seriesPacket = new QBarSeries;
    QtCharts::QBarSeries *seriesSize = new QBarSeries;
    QtCharts::QChart *chart = new QChart;

    QBarSet *setTotal = new QBarSet("Total Packets");
    QBarSet *setLost = new QBarSet("Packets Lost");
    QBarSet *setSize = new QBarSet("Amount Transfer");

    auto tests = TestSet::getTestSets();
    tests->extractSets(setTotal,setLost,setSize);

    seriesSize->append(setSize);
    seriesPacket->append(setTotal);
    seriesPacket->append(setLost);

    QStringList categories;
    int sz = tests->size();
    for(int i = 0; i < sz; ++i)
        categories.append(("Test " + std::to_string(i)).c_str());
    axisX->append(categories);

    chart->setTitle("Test Result Stats");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    //size first
    chart->addSeries(seriesSize);
    //then packets
    chart->addSeries(seriesPacket);

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
