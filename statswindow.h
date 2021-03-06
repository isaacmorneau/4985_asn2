/**
 * Program: 4981 Assignment 2
 *
 * Source File: statswindow.cpp
 *
 * Class: StatsWindow
 *
 * Functions:
 *     void updateData();
 *
 * Date: 2017/02/15
 *
 * Designer: Isaac Morneau; A00958405
 *
 * Programmer: Isaac Morneau; A00958405
 *
 * Notes: This window displays the bar charts for the current test data
 */
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
    *     explicit StatsWindow(
    * 		QWidget *parent = 0 	- used only by qt when widget has a parent
    * 		);
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
    * 		void
    *
    * Notes: creates the bar charts out of the data in the TestSets it displays total size, packets sent and packets lost
    */
    void updateData();

private:
    Ui::StatsWindow *ui;
};

#endif // STATSWINDOW_H
