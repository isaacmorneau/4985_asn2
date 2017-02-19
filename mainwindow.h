#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

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
    //the stupid way that qt generates the files means
    //that if i try and make this an actual Ui::StatsWindow
    //the linker cannot complete the type
    //void *stats;
    void *stats;
};

#endif // MAINWINDOW_H
