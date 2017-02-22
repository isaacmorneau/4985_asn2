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
    /**
    * Function:StatsWindow
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     explicit StatsWindow(QWidget *parent = 0);
    *
    * Return:
    *
    * Notes: creates the window initializing the partent QT window
    */
    explicit StatsWindow(QWidget *parent = 0);
    /**
    * Function:~StatsWindow
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     ~StatsWindow();
    *
    * Return:
    *
    * Notes: delete the saved ui elements
    */
    ~StatsWindow();
private slots:
    /**
    * Function:updateData
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void updateData();
    *
    * Return:
    *
    * Notes: creates the bar charts out of the data in the TestSets it displays total size, packets sent and packets lost
    */
    void updateData();

private:
    Ui::StatsWindow *ui;
};

#endif // STATSWINDOW_H
