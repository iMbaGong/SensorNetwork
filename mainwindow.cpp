#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <sstream>
#include "QMessageBox"
#include <time.h>
#include"antcolonysystem.h"
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
    isFirst=true;
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
    isFirst=true;
    ui->textBrowser->clear();
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

void MainWindow::DP()
{


    double time,start;
    std::string sout;
    start = clock();
    QString output ;
    time = (clock()-start)/CLOCKS_PER_SEC;
    output = QString::number(time,'f',6);
    output+="：正在初始化数据...\n";
    ui->textBrowser->insertPlainText(output);

    int N=numSensor,M;
    if(strRow==endRow&&strCol==endCol){
        M=1<<(N-1);
    }
    else{
        M=1<<(N-2);
    }
    int** nodes=new int*[N];
    for(int i=0;i<N;i++){
        nodes[i] = new int[2];
    }
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

    if(endIndex==strIndex){
        //将结束点移到开始
        mySwitch(endIndex,0,nodes);
        strIndex=0;
    }
    else{
        //将结束点移到最后
        mySwitch(endIndex,N-1,nodes);
        //将起始点移到第一个
        mySwitch(strIndex,0,nodes);
    }


    //计算边的权值
    int D[N][N];
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            D[i][j]=distance(nodes[i][0],nodes[i][1],nodes[j][0],nodes[j][1]);
        }
    }
    //保存路径
    std::vector<int>* path[N][M];
    for(int i=0;i<N;i++){
        for(int j=0;j<M;j++){
            path[i][j]=new std::vector<int>();
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
        dp[i][0]=D[i][endIndex];
        path[i][0]->push_back(i);
    }
    time = (clock()-start)/CLOCKS_PER_SEC;
    output = QString::number(time,'f',6);
    output+="：开始计算...\n";
    ui->textBrowser->insertPlainText(output);


    //开始计算
    for(int i=1;i<M;i++){
        for(int j=0;j<N;j++){
            if(j!=0&&((1<<(j-1))&i))
                continue;
            for(int k=1;k<N;k++){
                if(!((1<<(k-1))&i))
                    continue;
                int  tmp =D[j][k]+dp[k][(~(1<<(k-1)))&i];
                if(tmp<dp[j][i]){
                    dp[j][i]=tmp;
                    delete path[j][i];
                    path[j][i]=new std::vector<int>(*path[k][(~(1<<(k-1)))&i]);
                    path[j][i]->push_back(j);
                }
            }
        }
    }
    time = (clock()-start)/CLOCKS_PER_SEC;
    output = QString::number(time,'f',6);
    output+="：计算完毕...\n结果如下：\n";
    ui->textBrowser->insertPlainText(output);
    //输出结果

    while(path[0][M-1]->size()){
        char x;
        x = nodes[path[0][M-1]->back()][0]+65;
        output = "("+QString(x)+","+QString::number(nodes[path[0][M-1]->back()][1]+1)+")->";
        path[0][M-1]->pop_back();
        ui->textBrowser->insertPlainText(output);
    }

    char x;
    x = nodes[endIndex][0]+65;
    output = "("+QString(x)+","+QString::number(nodes[endIndex][1]+1)+")\n";
    ui->textBrowser->insertPlainText(output);
    ui->textBrowser->moveCursor(QTextCursor::End);
    delete[] nodes;
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

void MainWindow::mySwitch(int& res, int tar,int** nodes)
{
    int sw[2];
    sw[0]=nodes[tar][0];sw[1]=nodes[tar][1];
    nodes[tar][0]=nodes[res][0];nodes[tar][1]=nodes[res][1];
    nodes[res][0]=sw[0];nodes[res][1]=sw[1];
    res = tar;
}

void MainWindow::setTextBrowser(QString input)
{
    ui->textBrowser->insertPlainText(input);
}




void MainWindow::on_btnStr_clicked()
{
    if(isFirst){
        isFirst=false;
    }
    else
        ui->textBrowser->insertPlainText(QString::fromStdString("\n/***************HISTORY***************/\n\n"));
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
    //DP();
    AntColonySystem* acs = new AntColonySystem(this);
    acs->ACO();
}
