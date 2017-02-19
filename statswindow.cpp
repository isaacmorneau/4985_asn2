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

    QBarSet *set0 = new QBarSet("Number Of Packets");
    QBarSet *set1 = new QBarSet("Packets Lost");
    QStringList categories;

    *set0 << 10 << 30 << 60 << 100 << 40 << 20;
    *set1 << 0 << 1 << 3 << 8 << 2 << 1;

    categories << "Test 1 100B" << "Test 2 200B" << "Test 3 600B" << "Test 4 1K" << "Test 5 200B";

    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Statitistics for Transfers");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

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
