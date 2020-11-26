#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <basetsd.h>
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
    numSensor=0;
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

void MainWindow::calculate()
{
    int N=numSensor;
    int M=1<<(N-1);
    int D[N][N];
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            D[i][j]=distance()
        }
    }
    int dp[N][M];
    for(int i=0;i<N;i++){
        for(int j=0;j<M;j++){
            dp[i][j]=INT32_MAX;
        }
    }
    for(int i=0;i<N;i++){
        dp[i][0]=D[i][0];
    }
    for(int i=1;i<M;i++){
        for(int j=1;j<N;j++){
            if((1<<(j-1))&i)
                continue;
            for(int k=1;k<N;k++){

                if(!((1<<(k-1))&i))
                    continue;
                int  tmp =D[j][k]+dp[k][(~(1<<(k-1)))&i];
                if(tmp<dp[j][i]){
                    dp[j][i]=tmp;
                }
            }
        }
    }
}
