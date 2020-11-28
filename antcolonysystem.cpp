#include "antcolonysystem.h"
#include<math.h>
#include "mainwindow.h"
#include"acsant.h"
#include<iostream>
#include<sstream>
AntColonySystem::AntColonySystem(MainWindow* window)
{
  alpha = 2,beta = 3, rou = 0.1, alpha1 = 0.1,  q0 = 0.01;
  NcMax = 500;
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

void AntColonySystem::UpdateLocalPathRule(int i, int j)
{
    info[i][j] = (1.0 - alpha1) * info[i][j] + alpha1 * (1.0 / (N*Lnn));
    info[j][i] = info[i][j];
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
    for (int i = 0; i < N; i++)
        {
            int row = *(bestTour + 2 * i);
            int col = *(bestTour + 2 * i + 1);
            info[row][col] = (1.0 - rou) * info[row][col] + rou * (1.0 / globalBestLength);
            info[col][row] = info[row][col];
    }
}

double AntColonySystem::CalAdjacentDistance(int node)
{
    double sum = 0.0;
    int visitedNode[N];
    for (int j = 0; j <N; j++)
    {
        visitedNode[j] = 1;
    }
    visitedNode[node] = 0;
    int currentNode = node;
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
    sum += allDistance[currentNode][node];
    return sum;
}

double AntColonySystem::calculateSumOfDistance(int **tour)
{
    double sum = 0;
        for (int i = 0; i< N; i++)
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
                num++;
            }
        }
    }
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            allDistance[i][j]=mainWindow->distance(nodes[i][0],nodes[i][1],nodes[j][0],nodes[j][1]);
        }
    }




    ACSAnt* ants[M];
    //蚂蚁均匀分布在城市上
    for (int k = 0; k < M; k++)
    {
        ants[k] = new ACSAnt(this, (int)(k%N));
    }

    //随机选择一个节点计算由最近邻方法得到的一个长度
    int node = rand() % N;
    Lnn = CalAdjacentDistance(node);

    //各条路径上初始化的信息素强度
    double initInfo = 1 / ( N*Lnn);
    this->InitParameter(initInfo);

    //全局最优路径
    int globalTour[N][2];
    //全局最优长度
    double globalBestLength = 0.0;

    time = (clock()-start)/CLOCKS_PER_SEC;
    output = QString::number(time,'f',6);
    output+="：正在计算...\n";
    mainWindow->setTextBrowser(output);
    for (int i = 0; i < NcMax; i++)
    {
        //局部最优路径
        int localTour[N][2];
        //局部最优长度
        double localBestLength = 0.0;
        //当前路径长度
        double tourLength=0;
        for (int j = 0; j < M; j++)
        {
            int** tourPath = ants[j]->Search();
            tourLength = calculateSumOfDistance(tourPath);
            //局部比较，并记录路径和长度
            if (tourLength < localBestLength || abs(localBestLength - 0.0) < 0.000001)
            {
                for (int m = 0; m< N; m++)
                {
                    int row = tourPath[m][0];
                    int col = tourPath[m][1];
                    localTour[m][0] = row;
                    localTour[m][1] = col;
                }
                localBestLength = tourLength;
            }
        }
        //全局比较，并记录路径和长度
        if (localBestLength < globalBestLength || abs(globalBestLength - 0.0) < 0.000001)
        {
            for (int m = 0; m< N; m++)
            {
                globalTour[m][0] = localTour[m][0];
                globalTour[m][1] = localTour[m][1];
            }
            globalBestLength = localBestLength;
        }
        this->UpdateGlobalPathRule(*globalTour, globalBestLength);
    }
    time = (clock()-start)/CLOCKS_PER_SEC;
    output = QString::number(time,'f',6);
    output+="：计算完毕...\n";
    for (int m = 0; m< N; m++)
        {
            output+=  QString::number(globalTour[m][0]) +".";
        }
    mainWindow->setTextBrowser(output);

}
