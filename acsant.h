#ifndef ACSANT_H
#define ACSANT_H

#include"antcolonysystem.h"
class ACSAnt
{
protected:
    int startCity, cururentCity;//初始城市编号，当前城市编号
    int* allowed;//禁忌表
    int** tour;//当前路径
    int currentTourIndex;//当前路径索引，从0开始，存储蚂蚁经过城市的编号
public:
    ACSAnt(AntColonySystem* acs, int start);
    //开始搜索
    int** Search();
    //选择下一节点
    int Choose();
    //移动到下一节点
    void MoveToNextCity(int nextCity);
private:
    AntColonySystem* antColony;
};

#endif // ACSANT_H
