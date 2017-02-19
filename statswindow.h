#ifndef STATSWINDOW_H
#define STATSWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <QtCharts/QBarSeries>

namespace Ui {
class StatsWindow;
}

class StatsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StatsWindow(QWidget *parent = 0);
    ~StatsWindow();

private:
    Ui::StatsWindow *ui;
    QtCharts::QBarSeries *seriesPacket;
    QtCharts::QBarSeries *seriesSize;
    QtCharts::QChart *chart;
};

#endif // STATSWINDOW_H
