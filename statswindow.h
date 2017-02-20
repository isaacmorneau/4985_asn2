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
private slots:
    void updateData();

private:
    Ui::StatsWindow *ui;
};

#endif // STATSWINDOW_H
