#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int bool;
               
#define true    1
#define false   0

#define MAX_VES 100     //最大顶点数

#define INFINITY    65535
#define INF     INFINITY
typedef struct {
    char vertexs[MAX_VES];    // 顶点
    int edges[MAX_VES][MAX_VES];   //边
    int num_vertexs;    // 顶点总数
} MGraph;

MGraph g_grap = {
    {'0', '1', '2', '3', '4', '5', '6', '7', '8'},
    {
        //0    1    2    3    4    5    6    7    8
        {  0,  10, INF, INF, INF,  11, INF, INF, INF},  // 0
        { 10,   0,  18, INF, INF, INF,  16, INF,  12},  // 1
        {INF,  18,   0,  22, INF, INF, INF, INF,   8},  // 2
        {INF, INF,  22,   0,  20, INF, INF,  16,  21},  // 3
        {INF, INF, INF,  20,   0,  26, INF,   7, INF},  // 4
        { 11, INF, INF, INF,  26,   0,  17, INF, INF},  // 5
        {INF,  16, INF, INF, INF,  17,   0,  19, INF},  // 6
        {INF, INF, INF,  16,   7, INF,  19,   0, INF},  // 7
        {INF,  12,   8,  21, INF, INF, INF, INF,   0},  // 8
    },
    9
};

// 普利姆 prim 最小树生成法
void mini_span_tree_prim(MGraph grap)
{
    int adjves[MAX_VES] = {0};
    int lowcost[MAX_VES] = {0};

    int i;
    // 从下标为4的点开始
    for (i = 0; i < grap.num_vertexs; i ++) {
        lowcost[i] = grap.edges[4][i];
    }
    adjves[0] = 4;
    int index;
    for (index = 1; index < grap.num_vertexs; i ++) {
        int min_cost = INFINITY;
        int k;
        for (i = 0; i < grap.num_vertexs; i ++) {
            if (lowcost[i] != 0 && lowcost[i] < min_cost) {
                min_cost = lowcost[i];
                k = i;
            }
        }
        for (i = 0; i < grap.num_vertexs; i ++) {
            if (lowcost[i] != 0 && grap.edges[k][i] < lowcost[i]) {
                lowcost[i] = grap.edges[k][i];
            }
        }
        adjves[index] = k;
        lowcost[k] = 0;
        index ++;
    }
    printf("after prim span:\n");
    for (i = 0; i < grap.num_vertexs; i ++) {
        printf(" %c", grap.vertexs[adjves[i]]);
    }
    printf("\n");
}

int main()
{
    int i;
    mini_span_tree_prim(g_grap);
    return 0;
}

