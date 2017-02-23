#ifndef TABLEWINDOW_H
#define TABLEWINDOW_H

#include <QMainWindow>

namespace Ui {
class TableWindow;
}

class TableWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
    * Function:TableWindow
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     explicit TableWindow(
    * 		QWidget *parent = 0 	- used only by qt when widget has a parent
    * 		);
    *
    *
    * Notes: calls parents to intialize the QT window
    */
    explicit TableWindow(QWidget *parent = 0);
    /**
    * Function:~TableWindow
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     ~TableWindow();
    *
    *
    * Notes: deletes ui elements saved
    */
    ~TableWindow();
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
    * Notes: builds the charts to display out of the data in the TestSet instance, with a row per test
    * it displays all saved data, incling sent, lost, size, protocol, time taken
    */
    void updateData();
private:
    Ui::TableWindow *ui;
};

#endif // TABLEWINDOW_H
