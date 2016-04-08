#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <memory.h>

//const int EDGENUMBERS = 150;
const int EDGENUMBERS = 1;

SetNode *setNode[MAX_INCLUDING_SET];

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
    int topoArray[edge_num][4];
    int includingSet[MAX_INCLUDING_SET];
    int cntPass = 0;                    // record valid number of elements in includingSet.
    int sourceID = 0, destinationID = 0;
    memset(topoArray, 0, sizeof(topoArray));    // initialize to 0.
    memset(includingSet, 0, sizeof(includingSet));

    getTopoArray(edge_num, topo, topoArray);
    getDemand(demand, includingSet, sourceID, destinationID, cntPass);

    for (int i = cntPass; i < MAX_INCLUDING_SET; i++)
    {
        includingSet[i] = -1;
    }

    //================================================================================
    // CREAT ADJACENT LIST.
    EdgeNode* nodeArray[MAX_VERTEX_NUM];        // array containing the first element of each list. Index of the array is nodeID.
    memset(nodeArray, 0, sizeof(nodeArray));
    change2List(nodeArray, topoArray, edge_num);
#if (defined(PRINTDEBUG) && defined(ROUTEDEBUG))
    testChange2List(nodeArray);
#endif

/*    if (edge_num < EDGENUMBERS)
    {
#ifdef DFSDEBUG
        getDFS(nodeArray, includingSet, cntPass, sourceID, destinationID);
#endif // DFSDEBUG
    }
    else
    {
#ifdef GREEDYALGORITHM
        mainGreedyAlgorithm(nodeArray, includingSet, cntPass, sourceID, destinationID);
#endif // GREEDYALGORITHM
    }*/

    int path[MAX_VERTEX_NUM];
    int length ;
    memset(path, 0, sizeof(sizeof(int) * MAX_VERTEX_NUM));
    getDFS1(nodeArray, includingSet, cntPass, sourceID, destinationID);
    length = getDFS2(nodeArray, includingSet, int cntPass, destinationID, path);
    for(int i = 0; i < length; i++)
    {
        record_result(path[i]);
    }


    return;
}

//====================================================================================================
// FUNCTIONS
//====================================================================================================
void getTopoArray(int edge_num, char *topo[5000], int topoArray[][4] )
{
    int i, j, k;
    for (i = 0; i < edge_num; i++)          // extract data from topo to topoArray. (topo[i][j], topoArray[i][k])
    {
        k = 0;
        for (j = 0; (topo[i][j] >= '0' && topo[i][j] <= '9') || topo[i][j] == ','; j++) // end of line is not \n!
        {
            if (topo[i][j] != ',')
            {
                topoArray[i][k] = topoArray[i][k] * 10 + topo[i][j] - '0';
            }
            else
            {
                k++;
            }
        }
    }

    /*
    // check the correctness of topoArray. CHECKED!
    printf("topoArray is:\n");
    for(i = 0; i < edge_num; i++){
        for(j = 0; j < 4; j++)  printf("%d ",topoArray[i][j]);
        printf("\n");
    }
    */
    return;
}

//====================================================================================================
void getDemand(char *demand, int includingSet[MAX_INCLUDING_SET], int& sourceID, int& destinationID, int& cntPass)
{
    // Extract data from demand[] to sourceID, destinationID and includingSet[]. includingSet contains invalid info!!(0)
    int i, j;
    for (i = 0; demand[i] != ','; i++)
    {
        sourceID = sourceID * 10 + demand[i] - '0';
    }
    i++;
    for (     ; demand[i] != ','; i++)
    {
        destinationID = destinationID * 10 + demand[i] - '0';
    }
    i++;
    cntPass = 1;
    for (j = 0; (demand[i] >= '0' && demand[i] <= '9') || demand[i] == '|'; i++)
    {
        if (demand[i] != '|')
        {
            includingSet[j] = includingSet[j] * 10 + demand[i] - '0';
        }
        else
        {
            j++;
            cntPass++;
        }
    }

    /*
    // check the correctness of demand. CHECKED!
    printf("srcID:%d, destID:%d,\nIncludingSet:",sourceID,destinationID);
    for(i = 0; i < cntPass; i++) printf("%d ",includingSet[i]); printf("\n");
    */

    return;
}

//====================================================================================================

///转换成邻接链表存储
void change2List(EdgeNode *node[MAX_VERTEX_NUM], int topoArray[][4], int edge_num)
{
    EdgeNode *pNode;
    EdgeNode *pTemp;
    bool repetition; ///检查两点之间的多条边
    for (int i = 0; i < edge_num; i++)
    {
        repetition = false;
        pNode = node[topoArray[i][1]];
        if (pNode == NULL)
        {
            pNode = new EdgeNode();
            node[topoArray[i][1]] = pNode;
            pNode->nodeID = topoArray[i][2];
            pNode->linkID = topoArray[i][0];
            pNode->weight = topoArray[i][3];
            continue;
        }
        pTemp = pNode;
        while (pNode != NULL)
        {
            if (pNode->nodeID == topoArray[i][2])
            {
                repetition = true;
                break;
            }
            pTemp = pNode;
            pNode = pNode->next;
        }
        if (repetition)
        {
            if (pNode->weight > topoArray[i][3])
            {
                pNode->weight = topoArray[i][3];
                pNode->linkID = topoArray[i][0];
            }
        }
        else
        {
            pNode = new EdgeNode();
            pTemp->next = pNode;
            pNode->nodeID = topoArray[i][2];
            pNode->linkID = topoArray[i][0];
            pNode->weight = topoArray[i][3];
        }
    }
}

bool checkInDemand(int nodeDemand[MAX_INCLUDING_SET], int ID)
{
    for (int i = 0; i < MAX_INCLUDING_SET; i++)
    {
        if (nodeDemand[i] == ID)
        {
            return true;
        }
    }
    return false;
}

//从待选队列中选取一个最短点
//更新后getMinNode不再标记处理标志
int getMinNode(int dijkstraDistance[MAX_VERTEX_NUM], bool inStack[MAX_VERTEX_NUM])
{
    int cost = 15000;
    int ID = -1;
    for (int i = 0; i < MAX_VERTEX_NUM; i++)
    {
        //为零说明此点没有得到更新
        if (dijkstraDistance[i] == 0)
        {
            continue;
        }
        if (!inStack[i])
        {
            if (cost > dijkstraDistance[i])
            {
                cost = dijkstraDistance[i];
                ID = i;
            }
        }
    }
    return ID;
}

//将currentID对应的路径复制到nodeID中,同时将currentID添加到nodeID路径中
void copyRoute(int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM], int recordDepth[MAX_VERTEX_NUM], int currentID, int nodeID)
{
    memcpy(recordRoute[nodeID], recordRoute[currentID], sizeof(recordRoute[nodeID]));
    recordRoute[nodeID][recordDepth[currentID]] = currentID;
    recordDepth[nodeID] = recordDepth[currentID] + 1;
    return;
}

//获得路径序列的cost，同时获得路径的编号
int getCost(EdgeNode *node[MAX_VERTEX_NUM], int sequence[MAX_VERTEX_NUM], int route[MAX_VERTEX_NUM], int length)
{
    EdgeNode* pNode = NULL;
    int cost = 0;
    for (int i = 0; i < length - 1; i++)
    {
        pNode = node[sequence[i]];
        while (pNode != NULL && pNode->nodeID != sequence[i + 1])
        {
            pNode = pNode->next;
        }
        if (pNode != NULL)
        {
            cost += pNode->weight;
            route[i] = pNode->linkID;
        }
    }
    return cost;
}

#if (defined(PRINTDEBUG) && defined(ROUTEDEBUG))
void testChange2List(EdgeNode *node[MAX_VERTEX_NUM])
{
    EdgeNode *pNode;
    for (int i = 0; i < MAX_VERTEX_NUM; i++)
    {
        pNode = node[i];
        while (pNode != NULL)
        {
            printf("LinkID: %d,sourceID: %d, DestinationID: %d,cost: %d\n", pNode->linkID, i, pNode->nodeID, pNode->weight);
            pNode = pNode->next;
        }
    }
}
#endif

#ifdef DFSDEBUG
void getDFS(EdgeNode *node[MAX_VERTEX_NUM], int nodeDemand[MAX_INCLUDING_SET], int cntPass, int sourceID, int destinationID)
{
    bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
    memset(hasVisited, 0, sizeof(hasVisited));
    bool inStack[MAX_VERTEX_NUM];
    memset(inStack, 0, sizeof(inStack));
    int nodeStack[MAX_VERTEX_NUM];
    memset(nodeStack, 0, sizeof(nodeStack));
    int stackDepth = 0;
    nodeStack[stackDepth] = sourceID;
    stackDepth++;
    inStack[sourceID] = true;
    int tempID = sourceID;
    EdgeNode *pNode = node[sourceID];
    int numbers = 0;
    int recordRoute[MAX_VERTEX_NUM];
    memset(recordRoute, 0, sizeof(recordRoute));
    int tempRoute[MAX_VERTEX_NUM];
    memset(tempRoute, 0, sizeof(tempRoute));
    int length = 0;
    int cost = 150000;
    int tempCost = 0;
    int temp[MAX_VERTEX_NUM];
    memcpy(temp, nodeDemand, sizeof(temp));
    while (stackDepth > 0)
    {
        while (pNode != NULL)
        {
            if (!inStack[pNode->nodeID] && !hasVisited[tempID][pNode->nodeID])
            {
                break;
            }
            else
            {
                pNode = pNode->next;
            }
        }
        //如果pNode为空则选择退栈
        if (pNode == NULL)
        {
            stackDepth--;
            if (stackDepth > 0)
            {
                pNode = node[nodeStack[stackDepth - 1]];
                tempID = nodeStack[stackDepth - 1];
                if (checkInDemand(nodeDemand, nodeStack[stackDepth]))
                {
                    numbers--;
                }
                memset(hasVisited[nodeStack[stackDepth]], 0, sizeof(hasVisited[nodeStack[stackDepth]]));
                inStack[nodeStack[stackDepth]] = false;
            }
        }
        else  //入栈
        {
            nodeStack[stackDepth] = pNode->nodeID;
            hasVisited[tempID][pNode->nodeID] = true;
            inStack[pNode->nodeID] = true;
            if (checkInDemand(nodeDemand, pNode->nodeID))
            {
                numbers++;
            }
            stackDepth++;
            if (pNode->nodeID == destinationID)
            {
                if (numbers == cntPass)
                {
                    //检查路径权值
                    memset(tempRoute, 0, sizeof(tempRoute));
                    tempCost = getCost(node, nodeStack, tempRoute, stackDepth);
                    if (tempCost < cost)
                    {
                        cost = tempCost;
                        memcpy(recordRoute, tempRoute, sizeof(tempRoute));
                        length = stackDepth - 1;
#if (defined(PRINTDEBUG) && defined(DFSDEBUG))
                        printRoute(recordRoute, length, cost);
#endif
                    }
                }
                stackDepth--;
                if (checkInDemand(nodeDemand, nodeStack[stackDepth]))
                {
                    numbers--;
                }
                memset(hasVisited[nodeStack[stackDepth]], 0, sizeof(hasVisited[nodeStack[stackDepth]]));
                inStack[nodeStack[stackDepth]] = false;
            }
            pNode = node[nodeStack[stackDepth - 1]];
            tempID = nodeStack[stackDepth - 1];
        }
    }
    for (int i = 0; i < length; i++)
    {
        record_result(recordRoute[i]);
    }
}
#endif // DFSDEBUG

#if (defined(PRINTDEBUG) && defined(ROUTEDEBUG))
void printRoute(int route[MAX_VERTEX_NUM], int length, int value)
{
    printf("Cost = %d\n", value);
    for (int i = 0; i < length; i++)
    {
        printf("%d ", route[i]);
    }
    printf("\n");
}
#endif

//把与currentID相连且未使用过的顶点全部更新
void updateBaseOnCurrentNode(EdgeNode *pNode, int dijkstraDistance[MAX_VERTEX_NUM], int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM], int recordDepth[MAX_VERTEX_NUM], bool inStack[MAX_VERTEX_NUM], int currentID)
{
    int tempID = 0;
    while (pNode != NULL)
    {
        tempID = pNode->nodeID;
        //如果该点没有在路径中则可以更新，否则不更新
        if (!inStack[tempID])
        {
            //对于还没有更新距离点的更新方法
            if (dijkstraDistance[tempID] > 0)
            {
                if (dijkstraDistance[currentID] + pNode->weight < dijkstraDistance[tempID])
                {
                    dijkstraDistance[tempID] = dijkstraDistance[currentID] + pNode->weight;
                    //涉及到路径的存储方法
                    copyRoute(recordRoute, recordDepth, currentID, tempID);
                }
                else if (dijkstraDistance[currentID] + pNode->weight == dijkstraDistance[tempID])
                {
                    //相等的时候去涉及点数较大的边
                    if ((recordDepth[currentID] + 1) > recordDepth[tempID])
                    {
                        copyRoute(recordRoute, recordDepth, currentID, tempID);
                    }
                }
            }
            else
            {
                dijkstraDistance[tempID] = dijkstraDistance[currentID] + pNode->weight;
                copyRoute(recordRoute, recordDepth, currentID, tempID);
            }
        }
        pNode = pNode->next;
    }
    return;
}

bool getRoute2DestinationID(EdgeNode *node[MAX_VERTEX_NUM], bool inStack[MAX_VERTEX_NUM], int route[MAX_VERTEX_NUM], int &length, int currentID, int destinationID)
{
    EdgeNode *pNode = NULL;
    int recordRouteDes[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
    memset(recordRouteDes, 0, sizeof(recordRouteDes));
    int distance[MAX_VERTEX_NUM];
    memset(distance, 0, sizeof(distance));
    int recordDepth[MAX_VERTEX_NUM];
    memset(recordDepth, 0, sizeof(recordDepth));
    for (int i = 1; i < MAX_VERTEX_NUM; i++)
    {
        pNode = node[currentID];
        updateBaseOnCurrentNode(pNode, distance, recordRouteDes, recordDepth, inStack, currentID);
        currentID = getMinNode(distance, inStack);
        if (currentID > -1)
        {
            if (currentID == destinationID)
            {
                memcpy(route, recordRouteDes[currentID], sizeof(recordRouteDes[currentID]));
                length = recordDepth[currentID];
                route[length++] = destinationID;
                return true;
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}

//退栈操作--仅仅对栈进行操作
void popStack(int nodeStack[MAX_VERTEX_NUM], bool inStack[MAX_VERTEX_NUM], int &stackDepth, bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM], int nodeDemand[MAX_INCLUDING_SET], int sourceID)
{
    int tempID = nodeStack[stackDepth - 1];
    inStack[tempID] = false;
    memset(hasVisited[tempID], 0, sizeof(hasVisited[tempID]));
    stackDepth--;
    tempID = nodeStack[stackDepth - 1];
    while (!checkInDemand(nodeDemand, tempID) && tempID != sourceID)
    {
        stackDepth--;
        inStack[tempID] = false;
        tempID = nodeStack[stackDepth - 1];
    }
    return;
}

void pushStack(int nodeStack[MAX_VERTEX_NUM], bool inStack[MAX_VERTEX_NUM], int &stackDepth, bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM], int route[MAX_VERTEX_NUM], int length)
{
    for (int i = 1; i < length; i++)
    {
        nodeStack[stackDepth++] = route[i];
        inStack[route[i]] = true;
    }
    hasVisited[route[0]][route[length - 1]] = true;
    return;
}

//如果成功找到则返回true
bool getPoints1Dijkstra(EdgeNode *node[MAX_VERTEX_NUM], bool inStack[MAX_VERTEX_NUM], bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM], int route[MAX_VERTEX_NUM], int &routeLength, int sourceID, int nodeDemand[MAX_INCLUDING_SET])
{
    int tempDistacne[MAX_VERTEX_NUM];
    memset(tempDistacne, 0, sizeof(tempDistacne));
    int tempRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
    memset(tempRoute, 0, sizeof(tempRoute));
    int tempDepth[MAX_VERTEX_NUM];
    memset(tempDepth, 0, sizeof(tempDepth));
    bool tempInStack[MAX_VERTEX_NUM];
    memcpy(tempInStack, inStack, sizeof(tempInStack));
    int currentID = sourceID;
    EdgeNode *pNode = NULL;
    for (int i = 1; i < MAX_VERTEX_NUM; i++)
    {
        pNode = node[currentID];
        updateBaseOnCurrentNode(pNode, tempDistacne, tempRoute, tempDepth, tempInStack, currentID);
        currentID = getMinNode(tempDistacne, tempInStack);
        if (currentID > -1)
        {
            tempInStack[currentID] = true;
            if (checkInDemand(nodeDemand, currentID) && !hasVisited[sourceID][currentID])
            {
                //入栈之类的操作,在上层函数操作
                //将路径返回即可，格式为sourceID。。。currentID
                memcpy(route, tempRoute[currentID], sizeof(tempRoute[currentID]));
                routeLength = tempDepth[currentID];
                route[routeLength++] = currentID;
                return true;
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}

//从栈中确定有多少个必过点已经找到
int checkNumbersInStack(int nodeStack[MAX_VERTEX_NUM], int stackDepth, int nodeDamand[MAX_INCLUDING_SET])
{
    int numbers = 0;
    for (int i = 0; i < stackDepth; i++)
        if (checkInDemand(nodeDamand, nodeStack[i]))
        {
            numbers++;
        }
    return numbers;
}

#ifdef GREEDYALGORITHM
//此函数用来处理贪心算法
void mainGreedyAlgorithm(EdgeNode *node[MAX_VERTEX_NUM], int nodeDemand[MAX_INCLUDING_SET], int cntPass, int sourceID, int destinationID)
{
    bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
    memset(hasVisited, 0, sizeof(hasVisited));
    bool inStack[MAX_VERTEX_NUM];
    memset(inStack, 0, sizeof(inStack));
    int nodeStack[MAX_VERTEX_NUM];
    memset(nodeStack, 0, sizeof(nodeStack));
    int route[MAX_VERTEX_NUM];
    memset(route, 0, sizeof(route));
    int stackDepth = 0;
    nodeStack[stackDepth++] = sourceID;
    inStack[sourceID] = true;
    int numbers = 0;
    int tempRouteLength = 0;
    int currentID = sourceID;
    //当栈不空时，就继续查找
    while (stackDepth > 0)
    {
        //当必过点还没有全部找完时
        if (numbers < cntPass)
        {
            //如果从currentID能向后扩展到新的必过点，则进行的一系列操作
            if (getPoints1Dijkstra(node, inStack, hasVisited, route, tempRouteLength, currentID, nodeDemand))
            {
                pushStack(nodeStack, inStack, stackDepth, hasVisited, route, tempRouteLength);
                currentID = nodeStack[stackDepth - 1];
                numbers = checkNumbersInStack(nodeStack, stackDepth, nodeDemand);
            }
            //如果从currentID无法继续向后扩展，则退栈
            else
            {
                //如果栈中有必过点则退栈，否则就只有sourceID，显然永远无法继续扩展了
                if (stackDepth > 1)
                {
                    currentID = nodeStack[stackDepth - 1];
                    memset(hasVisited[currentID], 0, sizeof(hasVisited[currentID]));
                    popStack(nodeStack, inStack, stackDepth, hasVisited, nodeDemand, sourceID);
                    currentID = nodeStack[stackDepth - 1];
                    numbers--;
                }
                else
                {
                    stackDepth--;
                }
            }
        }
        //必过点找完后，从最后一个必过点找到终点的路径
        else
        {
            //如果找的到则说明，该路径是成功的
            if (getRoute2DestinationID(node, inStack, route, tempRouteLength, currentID, destinationID))
            {
                pushStack(nodeStack, inStack, stackDepth, hasVisited, route, tempRouteLength);
                currentID = nodeStack[stackDepth - 1];
                break;
            }
            //找不到的话，就退栈，继续找，直到栈为空
            else
            {
                currentID = nodeStack[stackDepth - 1];
                memset(hasVisited[currentID], 0, sizeof(hasVisited[currentID]));
                popStack(nodeStack, inStack, stackDepth, hasVisited, nodeDemand, sourceID);
                currentID = nodeStack[stackDepth - 1];
                numbers--;
            }
        }
    }
    getCost(node, nodeStack, route, stackDepth);
    for (int i = 0; i < stackDepth - 1; i++)
    {
        record_result(route[i]);
    }
    return;
}
#endif // GREEDYALGORITHM


/*
 *  cntPass 必过点个数
 *
 */
void getDFS1(EdgeNode *node[MAX_VERTEX_NUM], int includingSet[MAX_INCLUDING_SET], int cntPass, int sourceID, int destinationID)
{
    //const int MAX_SEARCH_DEPTH = 5;     // 基本功能实现后可添加迭代加深.
    bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
    memset(hasVisited, 0, sizeof(hasVisited));
    bool inStack[MAX_VERTEX_NUM];
    memset(inStack, 0, sizeof(inStack));
    int nodeStack[MAX_VERTEX_NUM];
    memset(nodeStack, 0, sizeof(nodeStack));
    int stackDepth = 0;
    nodeStack[stackDepth] = sourceID;
    stackDepth++;
    inStack[sourceID] = true;
    int tempID = sourceID;
    EdgeNode *pNode = node[sourceID];
    int numbers = 0;
    int recordRoute[MAX_VERTEX_NUM];
    memset(recordRoute, 0, sizeof(recordRoute));
    int tempRoute[MAX_VERTEX_NUM];
    memset(tempRoute, 0, sizeof(tempRoute));

    int length = 0;
    int tempCost = 0;
    int index = 0;
    int tempDepth = 0;
    SetNode * curSNode;
    while (index < cntPass)
    {
        //设置头结点
        setNode[index] = (SetNode *) malloc(sizeof(SetNode));
        setNode[index] ->startNode = includingSet[index];
        setNode[index] -> weight = 0;
        setNode[index] -> endNode = 0;
        setNode[index] ->length = 0;
        setNode[index] ->mark = false;
        setNode[index] -> next = NULL;
        curSNode = setNode[index];
        while (stackDepth > 0)
        {
            while (pNode != NULL)
            {
                if (!inStack[pNode->nodeID] && !hasVisited[tempID][pNode->nodeID])
                {
                    break;
                }
                else
                {
                    pNode = pNode->next;
                }
            }
            //如果pNode为空则选择退栈
            if (pNode == NULL)
            {
                stackDepth--;
                if (stackDepth > 0)
                {
                    pNode = node[nodeStack[stackDepth - 1]];
                    tempID = nodeStack[stackDepth - 1];
                    memset(hasVisited[nodeStack[stackDepth]], 0, sizeof(hasVisited[nodeStack[stackDepth]]));
                    inStack[nodeStack[stackDepth]] = false;
                }
            }
            else  //入栈
            {
                nodeStack[stackDepth] = pNode->nodeID;
                hasVisited[tempID][pNode->nodeID] = true;
                inStack[pNode->nodeID] = true;
                stackDepth++;
                //如果经过了必过点，或者终点，且深度小于MAX_SEARCH_DEPTH
                if ((checkInDemand(includingSet, pNode->nodeID) || pNode->nodeID == destinationID ) && stackDepth <= MAX_SEARCH_DEPTH ) // 遍历到另一个必过点,存储路径,回退寻找下一个必过点.
                {
                    //#!!!! 记录路径写到这里！！
                    memset(tempRoute, 0, sizeof(tempRoute));
                    tempCost = getCost(node, nodeStack, tempRoute, stackDepth);
                    memcpy(recordRoute, tempRoute, sizeof(tempRoute));
                    length = stackDepth - 1;
                    stackDepth--;
                    memset(hasVisited[nodeStack[stackDepth]], 0, sizeof(hasVisited[nodeStack[stackDepth]]));
                    inStack[nodeStack[stackDepth]] = false;
                    //记录结点
                    SetNode *tempSNode = (SetNode *)malloc(sizeof(SetNode));
                    tempSNode ->next = NULL;
                    tempSNode ->mark = false;
                    tempDepth = stackDepth;
                    tempSNode ->nodeList[tempDepth + 1] = nodeStack[stackDepth];
                    tempSNode ->length = 1;
                    while(tempDepth > 0)
                    {
                        tempSNode ->nodeList[tempDepth] = nodeStack[tempDepth];
                        tempSNode ->length++;
                        tempDepth --;
                    }
                    tempSNode ->endNode = pNode ->nodeID;
                    tempSNode ->weight = tempCost;
                    curSNode ->next = tempSNode;
                    curSNode = curSNode ->next;
                }
                pNode = node[nodeStack[stackDepth - 1]];
                tempID = nodeStack[stackDepth - 1];
            }
        }
        nodeStack[stackDepth] = includingSet[index];    // 更新下次进行遍历时的起点.
        index++;
    }
    return;
}


int getDFS2(EdgeNode *node[MAX_VERTEX_NUM], int includingSet[MAX_INCLUDING_SET], int cntPass, int destinationID, int *path)
{
    bool hasVisited[MAX_VERTEX_NUM];
    memset(hasVisited, 0, sizeof(hasVisited));

    int setId = 0;//从起点开始

    int stackDepth = 0;
    int nodeStack[MAX_INCLUDING_SET];
    memset(nodeStack, 0, sizeof(nodeStack));

    SetNode *shortPath = NULL;
    SetNode *curPath = NULL;
    while(1)
    {
        shortPath = setNode[setId] ->next;
        //找到通往下一个必过结点中权值最小的路径
        while(setNode[setId] ->next != NULL)
        {
            curPath = setNode[setId] ->next;
            //如果没有被访问过且权重较小
            if(!curPath ->mark && curPath->weight < shortPath ->weight)
            {
                shortPath = curPath;
            }
        }
        if(shortPath != NULL)
        {
            //判断冲突，不冲突的话表示找到了一条路径
            if(!CheckConf(shortPath, hasVisited))
            {
                shortPath ->mark = true;
                //把最短路径的信息复制到第一个结点中
                CopyToHead(setNode[setId], shortPath);
                //保存
                nodeStack[stackDepth++] = setId;
                //设置新的遍历起点
                setId = shortPath ->endNode;
                //找到
                if(setId == destinationID)
                {
                    break;
                }
            }
        }
        //如果没有适合的路径，则退栈
        else
        {
            //清除mark状态
            CleanState(setNode[setId]);
            setId = nodeStack[stackDepth--];
            //如果把第一个结点也出栈，说明没有结果
            if(stackDepth < 0)
            {
                return 0;
            }
        }
    }

    int pathlenth = GetPath(node, nodeStack, stackDepth, path);

    return pathlenth;
}

//检查冲突，有冲突返回true，无返回false
bool CheckConf(SetNode *path, bool hasVisited[MAX_VERTEX_NUM])
{
    int i ;
    for(i = 0; i < path ->length; i++)
    {
        if(true == hasVisited[path ->nodeList[i]] )
            return true;
        hasVisited[path ->nodeList[i]] = true;
    }
    return false;
}

void CopyToHead(SetNode *head, SetNode *path)
{
    int i;
    head ->mark= path ->mark;
    head ->endNode = path ->endNode;
    for(i = 0; i < path->length; i++)
    {
        head ->nodeList[i] = path ->nodeList[i];
    }
    head ->weight = path ->weight;
}

void CleanState(SetNode *node)
{
    node -> weight = 0;
    node -> endNode = 0;
    node ->length = 0;
    node ->mark = false;
    while(node ->next != NULL)
    {
        node = node ->next;
        node ->mark = false;
    }
}
//获得路径
int GetPath(EdgeNode *node[MAX_VERTEX_NUM], int nodeStack[MAX_INCLUDING_SET], int stackDepth, int *path)
{
    int i,j;
    int setId = 0;
    int length;
    int count = 0;

    int startId;
    int nodeId;
    int pathId;


    //依次读取栈中的关键结点
    for(i = 0; i < stackDepth; i++)
    {
        setId = nodeStack[i];
        length = setNode[setId] ->length;
        startId = setNode[setId] ->startNode;
        for(j = 0; j < length; j++)
        {
            nodeId = setNode[setId] ->nodeList[j];
            pathId = ConToPath(node, startId, nodeId);
            path[count++] = pathId;
            startId = nodeId;
        }
    }

    return count;
}

int ConToPath(EdgeNode *node[MAX_VERTEX_NUM],int startId, int nodeId)
{
    EdgeNode *cNode = node[startId];
    while(cNode != NULL)
    {
        if(cNode ->nodeID == nodeId)
            return cNode ->linkID;
        cNode = cNode ->next;
    }
    return 0;
}
