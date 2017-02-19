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
    ui(new Ui::StatsWindow)
{
    ui->setupUi(this);

    QBarSet *set0 = new QBarSet("Total Packets");
    QBarSet *set1 = new QBarSet("Packets Lost");
    QBarSet *set2 = new QBarSet("Amount Transfer");

    QStringList categories;

    //get actual test data here and build the sets out of it

    //size
    *set2 << 200 << 350 << 500 << 1000 << 460 << 210;
    //packets
    *set0 << 10 << 30 << 60 << 100 << 40 << 20;
    *set1 << 0 << 1 << 3 << 8 << 2 << 1;


    categories << "Test 1" << "Test 2" << "Test 3" << "Test 4" << "Test 5";

    QBarSeries *series0 = new QBarSeries;
    QBarSeries *series1 = new QBarSeries;
    QChart *chart = new QChart();
    chart->setTitle("Test Result Stats");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QValueAxis *axisY0 = new QValueAxis;
    QValueAxis *axisY1 = new QValueAxis;
    QBarCategoryAxis *axisX = new QBarCategoryAxis;

    series1->append(set2);
    series0->append(set0);
    series0->append(set1);

    //size first
    chart->addSeries(series1);
    //then packets
    chart->addSeries(series0);

    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    axisY1->setTitleText("Transfer Size");
    chart->addAxis(axisY1, Qt::AlignLeft);
    axisY0->setTitleText("Number of Packets");
    chart->addAxis(axisY0, Qt::AlignRight);

    series1->attachAxis(axisX);
    series1->attachAxis(axisY1);

    series0->attachAxis(axisX);
    series0->attachAxis(axisY0);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    setCentralWidget(chartView);
}

StatsWindow::~StatsWindow()
{
    delete ui;
}
