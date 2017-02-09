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
    void messageAdd_slot(std::string);
    void messageSet_slot(int);
    void messageClear_slot();

    void on_pushButtonStart_clicked();

    void on_pushButtonStop_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
