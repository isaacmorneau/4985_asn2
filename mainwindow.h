#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include "statswindow.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //accept data from network threads
    void messageAdd_slot(std::string);
    void messageSet_slot(int);
    void messageClear_slot();

    //accept events from ui elements
    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

    void on_checkBoxStats_clicked();

private:
    Ui::MainWindow *ui;
    //dont add Ui namespace, everything will break
    StatsWindow *stats;
};

#endif // MAINWINDOW_H
