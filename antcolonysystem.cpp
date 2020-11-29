#include "antcolonysystem.h"
#include<math.h>
#include "mainwindow.h"
#include"acsant.h"
#include<iostream>
#include<sstream>
AntColonySystem::AntColonySystem(MainWindow* window)
{
  alpha = 1,beta = 4, rou = 0.1, q0 = 0.01;
  localMax = 5000,globalMax = 10;
  M = N = window->numSensor;

  allDistance =new double*[N];
  for(int i=0;i<N;i++){
      allDistance[i] = new double[N];
  }
  info =new double*[N];
  for(int i=0;i<N;i++){
      info[i] = new double[N];
  }
  visible =new double*[N];
  for(int i=0;i<N;i++){
     visible[i] = new double[N];
  }
  mainWindow = window;
}

AntColonySystem::~AntColonySystem()
{
    for(int i=0;i<N;i++){
        delete[] allDistance[i];
    }
    delete[] allDistance;
    for(int i=0;i<N;i++){
        delete[] info[i];
    }
    delete[] info;
    for(int i=0;i<N;i++){
        delete[] visible[i];
    }
    delete[] visible;
}

double AntColonySystem::Transition(int i, int j)
{
    if (i != j)
        {

            return (pow(info[i][j], alpha) * pow(visible[i][j], beta));
        }
        else
        {
            return 0.0;
    }
}


void AntColonySystem::InitParameter(double value)
{
    //初始化路径上的信息素强度
       for (int i = 0; i < N; i++)
       {
           for (int j = 0; j < N; j++)
           {
               info[i][j] = value;
               info[j][i] = value;
               if (i != j)
               {
                   visible[i][j] = 1.0 / allDistance[i][j];
                   visible[j][i] = visible[i][j];
               }
           }
       }
}

void AntColonySystem::UpdateGlobalPathRule(int *bestTour, int globalBestLength)
{
    int n=N-1;
    if(mainWindow->strIndex==mainWindow->endIndex)
        n++;
    for (int i = 0; i < n; i++)
        {
            int row = *(bestTour + 2 * i);
            int col = *(bestTour + 2 * i + 1);
            info[row][col] = (1.0 - rou) * info[row][col] + rou * (1.0 / globalBestLength);
            info[col][row] = info[row][col];
    }
}

double AntColonySystem::CalAdjacentDistance()
{
    double sum = 0.0;
    int visitedNode[N];
    for (int j = 0; j <N; j++)
    {
        visitedNode[j] = 1;
    }
    visitedNode[mainWindow->strIndex] = 0;
    visitedNode[mainWindow->endIndex] = 0;
    int currentNode = mainWindow->strIndex;
    int nextNode;
    do
    {
        nextNode = ChooseNextNode(currentNode, visitedNode);
        if (nextNode >= 0)
        {
            sum += allDistance[currentNode][nextNode];
            currentNode = nextNode;
            visitedNode[currentNode] = 0;
        }
    } while (nextNode >= 0);
    sum += allDistance[currentNode][mainWindow->endIndex];
    return sum;
}

double AntColonySystem::calculateSumOfDistance(int **tour)
{
    double sum = 0;
    int n=N-1;
    if(mainWindow->strIndex==mainWindow->endIndex)
        n++;
    for (int i = 0; i< n; i++)
    {
        int row = tour[i][0];
        int col = tour[i][1];
        sum += allDistance[row][col];
    }
    return sum;
}
int AntColonySystem::ChooseNextNode(int currentNode, int visitedNode[])
{
    int nextNode = -1;
    double shortDistance = 0.0;
    for (int i = 0; i < N; i++)
    {
        //去掉已走过的节点,从剩下节点中选择距离最近的节点
        if (1 == visitedNode[i])
        {
            if (shortDistance == 0.0)
            {
                shortDistance = allDistance[currentNode][i];
                nextNode = i;
            }
            if (shortDistance < allDistance[currentNode][i])
            {
                nextNode = i;
            }
        }
    }
    return nextNode;
}


void AntColonySystem::ACO()
{
    srand(time(0));
    double time,start;
    std::string sout;
    start = clock();
    QString output ;
    time = (clock()-start)/CLOCKS_PER_SEC;
    output = QString::number(time,'f',6);
    output+="：正在初始化数据...\n";
    mainWindow->setTextBrowser(output);

    int nodes[N][2];
    int num=0;
    for(int i=0;i<8;i++){
        for(int j=0;j<12;j++){
            if(mainWindow->graph[i][j]==true){
                nodes[num][0]=i;
                nodes[num][1]=j;
                if(i==mainWindow->strRow&&j==mainWindow->strCol)
                    mainWindow->strIndex=num;
                if(i==mainWindow->endRow&&j==mainWindow->endCol)
                    mainWindow->endIndex=num;
                num++;
            }
        }
    }

    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            allDistance[i][j]=mainWindow->distance(nodes[i][0],nodes[i][1],nodes[j][0],nodes[j][1]);
        }
    }

    //全局最优路径
    int globalTour[N][2];
    //全局最优长度
    double globalBestLength = 0.0;

    time = (clock()-start)/CLOCKS_PER_SEC;
    output = QString::number(time,'f',6);
    output+="：正在计算...\n";
    mainWindow->setTextBrowser(output);
    for(int i=0;i<globalMax;i++){

        ACSAnt* ant = new ACSAnt(this,mainWindow->strIndex,mainWindow->endIndex);
        //随机选择一个节点计算由最近邻方法得到的一个长度
        Lnn = CalAdjacentDistance();
        //各条路径上初始化的信息素强度
        double initInfo = 1 / ( N*Lnn);
        this->InitParameter(initInfo);

        //最优路径
        int localTour[N][2];
        //最优长度
        double localBestLength = 0.0;

        for (int j = 0; j < localMax; j++)
        {

            int** tourPath = ant->Search();
            double tourLength = calculateSumOfDistance(tourPath);


            //更新局部最优
            if (tourLength < localBestLength || abs(localBestLength - 0.0) < 0.000001)
            {
                for (int m = 0; m< N; m++)
                {
                    localTour[m][0] = tourPath[m][0];
                    localTour[m][1] = tourPath[m][1];
                }
                localBestLength = tourLength;
            }
            this->UpdateGlobalPathRule(*localTour, localBestLength);
        }

        //更新全局最优
        if (localBestLength < globalBestLength || abs(globalBestLength - 0.0) < 0.000001)
        {
            for (int m = 0; m< N; m++)
            {
                globalTour[m][0] = localTour[m][0];
                globalTour[m][1] = localTour[m][1];
            }
            globalBestLength = localBestLength;
        }
        delete ant;
    }

    time = (clock()-start)/CLOCKS_PER_SEC;
    output = QString::number(time,'f',6);
    output+="：计算完毕...\n";
    output+= "路径长度为："+QString::number(globalBestLength*mainWindow->dis);
    output+= "\n遍历用时："+QString::number(globalBestLength*mainWindow->dis/mainWindow->speed,'f',2);
    output+= "\n遍历路径如下：\n";
    mainWindow->setTextBrowser(output);
    int n = N-1;
    if(mainWindow->strIndex==mainWindow->endIndex)
        n++;
    for (int m = 0; m< n; m++)
        {
            char x;
            x = nodes[globalTour[m][0]][0]+65;
            output = "("+QString(x)+","+QString::number(nodes[globalTour[m][0]][1]+1)+")->";
            mainWindow->setTextBrowser(output);
        }
    char x;
    x = nodes[globalTour[n-1][1]][0]+65;
    output = "("+QString(x)+","+QString::number(nodes[globalTour[n-1][1]][1]+1)+")\n";
    mainWindow->setTextBrowser(output);
}
