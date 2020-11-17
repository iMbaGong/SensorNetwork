#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    graph = new bool*[8];
    for(int i=0;i<8;i++){
        graph[i] = new bool[12];
    }

}

MainWindow::~MainWindow()
{
    delete ui;

    delete[] graph;
}



void MainWindow::on_radioButton_clicked(bool checked)
{
    if(checked)
        graph[0][0] = true;
    else
        graph[0][0] = false;
}
