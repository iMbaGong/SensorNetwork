#ifndef ANTCOLONYSYSTEM_H
#define ANTCOLONYSYSTEM_H

#include"mainwindow.h"
class AntColonySystem
{
public:
    AntColonySystem(MainWindow* window);
    ~AntColonySystem();
    //计算当前节点到下一节点转移的概率
    double Transition(int i, int j);
    //局部更新规则
    void UpdateLocalPathRule(int i, int j);
    //初始化
    void InitParameter(double value);
    //全局信息素更新
    void UpdateGlobalPathRule(int* bestTour, int globalBestLength);
     //信息启发因子，期望启发式因子，全局信息素挥发参数，局部信息素挥发参数, 状态转移公式中的q0
    double alpha , beta , rou  , alpha1 ,  q0 ;
    int N ,M, NcMax;
    double Lnn;
    void ACO();
    int ChooseNextNode(int currentNode, int visitedNode[]);
    double CalAdjacentDistance();
    double calculateSumOfDistance(int** tour);
private:
    double** info;//节点之间的信息素强度
    double** visible;//节点之间的能见度
    double** allDistance;//由矩阵表示两两城市之间的距离
    MainWindow* mainWindow;

};
#endif // ANTCOLONYSYSTEM_H
