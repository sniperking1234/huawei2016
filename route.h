#ifndef __ROUTE_H__
#define __ROUTE_H__

//test

void search_route(char *graph[5000], int edge_num, char *condition);

const int MAX_VERTEX_NUM = 600;		// max vertex number in the graph
const int MAX_INCLUDING_SET = 50;	// max vertex number in the including set

#define ROUTEDEBUG

#define PRINTDEBUG

#define DFSDEBUG

//#define actualNodes MAX_VERTEX_NUM

#define GREEDYALGORITHM

typedef struct EdgeNode
{
	int linkID;
	int nodeID;
	int weight;
	EdgeNode *next;
} EdgeNode;

#define MAX_SEARCH_DEPTH 5
typedef struct SetNode
{
    int startNode;
    int weight;
    int endNode;
    int length;
    bool mark;
    int nodeList[MAX_SEARCH_DEPTH] = {0};
    SetNode *next;
}SetNode;

//未注释的为通用公共函数
void getTopoArray(int edge_num, char *topo[5000], int topoArray[][4]);

void getDemand(char *demand, int includingSet[MAX_INCLUDING_SET], int& sourceID, int& destinationID, int& cntPass);

void change2List(EdgeNode *node[MAX_VERTEX_NUM],int topoArray[][4],int edge_num);

bool checkInDemand(int nodeDemand[MAX_INCLUDING_SET],int ID);

int getCost(EdgeNode *node[MAX_VERTEX_NUM],int sequence[MAX_VERTEX_NUM],int route[MAX_VERTEX_NUM],int length);

void copyRoute(int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],int currentID,int nodeID);

int getMinNode(int dijkstraDistance[MAX_VERTEX_NUM],bool inStack[MAX_VERTEX_NUM]);

void updateBaseOnCurrentNode(EdgeNode *pNode,int dijkstraDistance[MAX_VERTEX_NUM],int recordRoute[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int recordDepth[MAX_VERTEX_NUM],bool inStack[MAX_VERTEX_NUM],int currentID);

bool getRoute2DestinationID(EdgeNode *node[MAX_VERTEX_NUM],bool inStack[MAX_VERTEX_NUM],int route[MAX_VERTEX_NUM],int &length,int &dijkstraDistance,int currentID,int destinationID);

void popStack(int nodeStack[MAX_VERTEX_NUM],bool inStack[MAX_VERTEX_NUM],int &stackDepth,bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int nodeDemand[MAX_INCLUDING_SET],int sourceID);

void pushStack(int nodeStack[MAX_VERTEX_NUM],bool inStack[MAX_VERTEX_NUM],int &stackDepth,bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int route[MAX_VERTEX_NUM],int length);

bool getPoints1Dijkstra(EdgeNode *node[MAX_VERTEX_NUM],bool inStack[MAX_VERTEX_NUM],bool hasVisited[MAX_VERTEX_NUM][MAX_VERTEX_NUM],int route[MAX_VERTEX_NUM],int &routeLength,int sourceID,int nodeDemand[MAX_INCLUDING_SET]);

int checkNumbersInStack(int nodeStack[MAX_VERTEX_NUM],int stackDepth,int nodeDamand[MAX_INCLUDING_SET]);

#if (defined(PRINTDEBUG) && defined(ROUTEDEBUG))
void testChange2List(EdgeNode *node[MAX_VERTEX_NUM]);
#endif

#ifdef DFSDEBUG
void getDFS(EdgeNode *node[MAX_VERTEX_NUM],int nodeDemand[MAX_INCLUDING_SET],int cntPass,int sourceID,int destinationID);
int getDFS2(EdgeNode *node[MAX_VERTEX_NUM], int includingSet[MAX_INCLUDING_SET], int cntPass, int destinationID, int *path);

bool CheckConf(SetNode *path, bool hasVisited[MAX_VERTEX_NUM]);
void CopyToHead(SetNode *head, SetNode *path);
void CleanState(SetNode *node);
void GetPath(EdgeNode *node[MAX_VERTEX_NUM], int nodeStack[MAX_INCLUDING_SET], int stackDepth, int *path);
int ConToPath(EdgeNode *node[MAX_VERTEX_NUM],int startId, int nodeId);
#endif // DFSDEBUG

#ifdef PRINTDEBUG
void printRoute(int route[MAX_VERTEX_NUM],int length,int value);
#endif // PRINTDEBUG

#ifdef GREEDYALGORITHM
void mainGreedyAlgorithm(EdgeNode *node[MAX_VERTEX_NUM],int nodeDemand[MAX_INCLUDING_SET],int cntPass,int sourceID,int destinationID);
#endif // GREEDYALGORITHM

#endif // __ROUTE_H__
