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
        for(int j=0;j<12;j++){
            graph[i][j] = false;
        }
    }
    numSensor = strRol = strCol = endRow = endCol =0;
    dis = speed = 100;
    QObject* object;
    QString name;
    foreach(object,ui->sensorNet->children()){
        QAbstractButton* btn = dynamic_cast<QAbstractButton*>(object);
        name = object->objectName();
        if(name.startsWith("radioButton")){
            QObject::connect(btn,SIGNAL(clicked(bool)),this,SLOT(sensorClicked(bool)));
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete[] graph;
}

void MainWindow::sensorClicked(bool checked)
{
    QString name = sender()->objectName();
    QString indexS = name.split("_")[1];
    int index = indexS.toInt()-1;
    int rol = index/12;
    int col = index%12;
    graph[rol][col] = checked;
    if(checked)numSensor++;
    else numSensor--;
}





void MainWindow::on_btnClr_clicked()
{
    for(int i=0;i<8;i++){
        for(int j=0;j<12;j++){
            graph[i][j]=false;
        }
    }
    QObject* object;
    QString name;
    foreach(object,ui->sensorNet->children()){
        name = object->objectName();
        if(name.startsWith("radioButton")){
           dynamic_cast<QAbstractButton*>(object)->setChecked(false);
        }
    }
}

void MainWindow::on_inputDis_valueChanged(int arg1)
{
    dis = arg1;
}

void MainWindow::on_inputSpd_valueChanged(int arg1)
{
    speed = arg1;
}

void MainWindow::on_startRow_currentIndexChanged(int index)
{
    strRol = index;
}

void MainWindow::on_startCol_currentIndexChanged(int index)
{
    strCol = index;
}

void MainWindow::on_endRow_currentIndexChanged(int index)
{
    endRow = index;
}

void MainWindow::on_endCol_currentIndexChanged(int index)
{
    endCol = index;
}
