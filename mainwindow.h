#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void sensorClicked(bool checked);

    void on_btnClr_clicked();

    void on_inputDis_valueChanged(int arg1);

    void on_inputSpd_valueChanged(int arg1);


    void on_startRow_currentIndexChanged(int index);

    void on_startCol_currentIndexChanged(int index);

    void on_endRow_currentIndexChanged(int index);

    void on_endCol_currentIndexChanged(int index);

    void on_btnStr_clicked();

private:
    Ui::MainWindow *ui;
    bool** graph;
    int numSensor;
    int dis;
    int speed;
    int strRow;
    int strCol;
    int endRow;
    int endCol;

    void calculate();
    int distance(int a_x,int a_y,int b_x,int b_y);
    void setStrEnd();
    void mySwitch(int& res,int tar,int** nodes);
};
#endif // MAINWINDOW_H
