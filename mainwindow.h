/**
 * Program: 4981 Assignment 2
 *
 * Source File: mainwindow.cpp
 *
 * Class: MainWindow
 *
 * Functions:
 *
 * Date: 2017/02/15
 *
 * Designer: Isaac Morneau; A00958405
 *
 * Programmer: Isaac Morneau; A00958405
 *
 * Notes: This is the primary window that has the controls for all sub windows for displaying stats
 * it is also what accepts the data from user input into fields to decide what type of networking
 * application to run such as server or client, tcp or udp.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "statswindow.h"
#include "tablewindow.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
    * Function:MainWindow
    *
    * Date: 2017/02/15
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface
    *     explicit MainWindow(
    *	 		QWidget *parent = 0 	- used only by qt when widget has a parent
    * 		);
    *
    * Notes: creates the window initializing the partent QT window
    * It also creates and saves both the table window and the stats window
    */
    explicit MainWindow(QWidget *parent = 0);
    /**
    * Function:~MainWindow
    *
    * Date: 2017/02/15
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *    ~MainWindow();
    *
    * Notes: deletes the saved UI elements
    * It also deletes both the table window and the stats window
    */
    ~MainWindow();

private slots:
    /**
    * Function:messageAdd_slot
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void messageAdd_slot(
    * 			td::string 		- the message to add to the message box
    * 		);
    *
    * Return:
    * 	void
    *
    * Notes:
    * Notes:triggered by the wrapper function resultAdd and appends the message specifed
    * to the message window.
    */
    void messageAdd_slot(std::string);
    /**
    * Function:messageSet_slot
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void messageSet_slot(
    * 			int 			- the percent to set the progress bar to
    * 		);
    *
    * Return:
    * 	void
    *
    * Notes:triggered by the wrapper function resultPercent and will set the progress bar to
    * the value specified
    */
    void messageSet_slot(int);
    /**
    * Function:messageClear_slot
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void messageClear_slot();
    *
    * Return:
    * 	void
    *
    * Notes:triggered by the wrapper function resultClear and will empty the message window
    */
    void messageClear_slot();

    /**
    * Function:on_pushButtonStart_clicked
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void on_pushButtonStart_clicked();
    *
    * Return:
    * 	void
    *
    * Notes: startup WSA session extract data from the UI for parameteres of what to run. Based on
    * elements it will select one of serverUDP serverTCP clientUDP clientTCP and run it in a throw
    * away thread with the options specified. it enables the stop button and disables itself
    */
    void on_pushButtonStart_clicked();

    /**
    * Function:on_pushButtonStop_clicked
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void on_pushButtonStop_clicked();
    *
    * Return:
    * 	void
    *
    * Notes: cleanup WSA session, close shared resources, and enable the start button, disabling it self.
    * the threads are not directly closed but they themselves notice the request and exit cleanly
    */
    void on_pushButtonStop_clicked();

    /**
    * Function:on_checkBoxStats_clicked
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void on_checkBoxStats_clicked();
    *
    * Return:
    * 	void
    *
    * Notes: This function triggers the stats window to update its table as well as show it when
    * being enabled. when being disabled it hides the table window
    */
    void on_checkBoxStats_clicked();

    /**
    * Function:on_pushButtonResetTest_clicked
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void on_pushButtonResetTest_clicked();
    *
    * Return:
    * 	void
    *
    * Notes: Triggers the internal TestSet to clear its saved data
    */
    void on_pushButtonResetTest_clicked();

    /**
    * Function:on_pushButtonClientFile_clicked
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void on_pushButtonClientFile_clicked();
    *
    * Return:
    * 	void
    *
    * Notes: Opens a Open File dialog using the system widget if avalible. The selected file
    * will be displayed in the text box below the button.
    */
    void on_pushButtonClientFile_clicked();

    /**
    * Function:on_pushButtonServerFile_clicked
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void on_pushButtonServerFile_clicked();
    *
    * Return:
    * 	void
    *
    * Notes: Opens a Save file dalog using the system widget if avalible. The selected file will
    * be displayed in the text box below the button.
    */
    void on_pushButtonServerFile_clicked();

    /**
    * Function:on_checkBoxTable_clicked
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void on_checkBoxTable_clicked();
    *
    * Return:
    * 	void
    *
    * Notes: This function triggers the table window to update its table as well as show it when
    * being enabled. when being disabled it hides the table window
    */
    void on_checkBoxTable_clicked();

    /**
    * Function:on_pushButtonApply_clicked
    *
    * Date: 2017/02/19
    *
    * Designer: Isaac Morneau; A00958405
    *
    * Programmer: Isaac Morneau; A00958405
    *
    * Interface:
    *     void on_pushButtonApply_clicked();
    *
    * Return:
    * 	void
    *
    * Notes: This function triggers the test to update its lost packets based on the client codes
    */
    void on_pushButtonApply_clicked();

private:
    Ui::MainWindow *ui;
    //dont add Ui namespace, everything will break
    StatsWindow *stats;
    TableWindow *table;
};

#endif // MAINWINDOW_H
