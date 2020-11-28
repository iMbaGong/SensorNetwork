#include "acsant.h"
#include <random>
ACSAnt::ACSAnt(AntColonySystem* acs, int start)
{
    antColony = acs;
    startCity = start;

    allowed = new int[antColony->N];
    tour = new int*[antColony->N];
    for(int i=0;i<antColony->N;i++){
        tour[i] = new int[2];
    }

}

int **ACSAnt::Search()
{
    cururentCity = startCity;
        int toCity;
        currentTourIndex = 0;
        for (int i = 0; i <antColony->N; i++)
        {
            allowed[i] = 1;
        }
        allowed[cururentCity] = 0;
        int endCity;
        int count = 0;
        do
        {
            count++;
            endCity = cururentCity;
            toCity = Choose();
            if (toCity >= 0)
            {
                MoveToNextCity(toCity);
                antColony->UpdateLocalPathRule(endCity, toCity);
                cururentCity = toCity;
            }
        } while (toCity >= 0);
        MoveToNextCity(startCity);
        antColony->UpdateLocalPathRule(endCity, startCity);

        return tour;
}

int ACSAnt::Choose()
{
    int nextCity = -1;
        double q = rand() / (double)RAND_MAX;
        //如果 q <= q0,按先验知识，否则则按概率转移，
        if (q <= antColony->q0)
        {
            double probability = -1.0;//转移到下一节点的概率
            for (int i = 0; i <antColony->N; i++)
            {
                //去掉禁忌表中已走过的节点,从剩下节点中选择最大概率的可行节点
                if (1 == allowed[i])
                {
                    double prob = antColony->Transition(cururentCity, i);
                    if (prob  > probability)
                    {
                        nextCity = i;
                        probability = prob;
                    }
                }
            }
        }
        else
        {
            //按概率转移
            double p = rand() / (double)RAND_MAX;//生成一个随机数,用来判断落在哪个区间段
            double sum = 0.0;
            double probability = 0.0;//概率的区间点，p 落在哪个区间段，则该点是转移的方向
            //计算概率公式的分母的值
            for (int i = 0; i < antColony->N; i++)
            {
                if (1 == allowed[i])
                {
                    sum += antColony->Transition(cururentCity, i);
                }
            }
            for (int j = 0; j <antColony->N; j++)
            {
                if (1 == allowed[j] && sum > 0)
                {
                    probability += antColony->Transition(cururentCity, j) / sum;
                    if (probability >= p || (p > 0.9999 && probability > 0.9999))
                    {
                        nextCity = j;
                        break;
                    }
                }
            }
        }
        return nextCity;
}

void ACSAnt::MoveToNextCity(int nextCity)
{
    allowed[nextCity] = 0;
    tour[currentTourIndex][0] = cururentCity;
    tour[currentTourIndex][1] = nextCity;
    currentTourIndex++;
    cururentCity = nextCity;
}
