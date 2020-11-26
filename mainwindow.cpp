#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include "QMessageBox"
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
    numSensor = strRow = strCol = endRow = endCol =0;
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
    strRow = index;
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
    if(!graph[strRow][strCol]||!graph[endRow][endCol]){
        QMessageBox msg(this);
        msg.setWindowTitle("Warning");
        msg.setText("设置的起点或终点没有被选中，若继续将为您自动选中，是否继续？");
        msg.setIcon(QMessageBox::Information);
        msg.setStandardButtons(QMessageBox::Ok | QMessageBox:: Cancel);
        if(msg.exec() == QMessageBox::Ok)
        {
           setStrEnd();
        }
        else
            return;
    }

    int N=numSensor;
    int M=1<<(N-1);
    int nodes[N][2];
    int num=0;
    int strIndex,endIndex;
    //将点放进数组以便建立图
    for(int i=0;i<8;i++){
        for(int j=0;j<12;j++){
            if(graph[i][j]==true){
                nodes[num][0]=i;
                nodes[num][1]=j;
                if(i==strRow&&j==strCol)
                    strIndex=num;
                if(i==endRow&&j==endCol)
                    endIndex=num;
                num++;
            }
        }
    }
    {//将起始点移到第一个
        int sw[2];
        sw[0]=nodes[0][0];sw[1]=nodes[0][1];
        nodes[0][0]=nodes[strIndex][0];nodes[0][1]=nodes[strIndex][1];
        nodes[strIndex][0]=sw[0];nodes[strIndex][1]=sw[1];
    }
    //计算边的权值
    int D[N][N];
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            D[i][j]=distance(nodes[i][0],nodes[i][1],nodes[j][0],nodes[j][1]);
        }
    }
    //初始化动态规划数组
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

int MainWindow::distance(int a_x, int a_y, int b_x, int b_y)
{
    return abs(a_x-b_x)>abs(a_y-b_y)?abs(a_x-b_x):abs(a_y-b_y);
}

void MainWindow::setStrEnd()
{
    std::stringstream out;
    std::string str;
    if(!graph[strRow][strCol]){
        graph[strRow][strCol]=true;
        int start = strRow*12+strCol;
        out<<"radioButton_";
        if(start>24)
         out<<0;
        out<<start+1;

        out>>str;
        ui->sensorNet->findChildren<QAbstractButton*>(QString::fromStdString(str))
                .last()->setChecked(true);
        numSensor++;
    }
    if(!graph[endRow][endCol]){
        graph[endRow][endCol]=true;
        int end = endRow*12+endCol;
        out.clear();
        out<<"radioButton_";
        if(end>24)
         out<<0;
        out<<end+1;
        out>>str;
        ui->sensorNet->findChildren<QAbstractButton*>(QString::fromStdString(str))
                .last()->setChecked(true);
        numSensor++;
    }
}

void MainWindow::on_btnStr_clicked()
{
    calculate();
}
