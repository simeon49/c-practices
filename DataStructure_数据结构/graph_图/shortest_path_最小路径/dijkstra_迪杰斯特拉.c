#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int bool;
               
#define true    1
#define false   0

#define MAX_VES 100     //最大顶点数

#define INFINITY    65535   // 无穷大数 不可到达
#define INF     INFINITY
typedef struct {
    char vertexs[MAX_VES];    // 顶点
    int edges[MAX_VES][MAX_VES];   //边
    int num_vertexs;    // 顶点总数
} MGraph;

// 数据来源于 <<大话数据结构.pdf>> page:290 图7-7-7
MGraph g_grap = {
    {'0', '1', '2', '3', '4', '5', '6', '7', '8'},
    {
        //0    1    2    3    4    5    6    7    8
        {  0,   1,   5, INF, INF, INF, INF, INF, INF},  // 0
        {  1,   0,   3,   7,   5, INF, INF, INF, INF},  // 1
        {  5,   3,   0, INF,   1,   7, INF, INF, INF},  // 2
        {INF,   7, INF,   0,   2, INF,   3, INF, INF},  // 3
        {INF,   5,   1,   2,   0,   3,   6,   9, INF},  // 4
        {INF, INF,   7, INF,   3,   0, INF,   5, INF},  // 5
        {INF, INF, INF,   3,   6, INF,   0,   2,   7},  // 6
        {INF, INF, INF, INF,   9,   5,   2,   0,   4},  // 7
        {INF, INF, INF, INF, INF, INF,   7,   4,   0},  // 8
    },
    9
};

typedef struct {
    int start;          // 最短路径的起始位置
    int path[MAX_VES];  // 从起始位置到各点的最小路径
    int weight[MAX_VES];// 从起始位置到各点的权值和
    int num_vertexs;
} SortestPath;

void shortest_path_dijkstra(MGraph grap, int start, SortestPath *sort_path)
{
    int i, j;
    int final[MAX_VES] = {0};   // final[v]=1 表示找到从start到v的最短路线
    sort_path->start = start;
    sort_path->num_vertexs = grap.num_vertexs;
    for (i = 0; i < MAX_VES; i ++) {
        sort_path->path[i] = start;
        sort_path->weight[i] = INF;
    }
    for (i = 0; i < grap.num_vertexs; i ++) {
        sort_path->weight[i] = grap.edges[start][i];
    }
    sort_path->path[start] = 0;
    sort_path->weight[start] = 0;
    final[start] = 1; 
    for (i = 1; i < grap.num_vertexs; i ++) {
        int index, min_weight = INFINITY;
        for (j = 0; j < grap.num_vertexs; j ++) {
            if (final[j] != 1 && sort_path->weight[j] < min_weight) {
                min_weight = sort_path->weight[j];
                index = j;
            }
        }
        final[index] = 1;   // 找到到index点的最近路线
        for (j = 0; j < grap.num_vertexs; j ++) {
            // 如果从index到j点的权加上start到index的权值和小于目前从start到j点的权值,
            // 则更新start到j的权值为前者,且记录到j点需要通过index
            if (final[j] != 1 && min_weight + grap.edges[index][j] < sort_path->weight[j]) {
                sort_path->weight[j] = min_weight + grap.edges[index][j];
                sort_path->path[j] = index;
            }
        }
    }
}

int main()
{
    int start = 0;
    SortestPath *sort_path = (SortestPath *)malloc(sizeof(SortestPath));
    assert(sort_path != NULL);
    shortest_path_dijkstra(g_grap, start, sort_path);

    int i;
    printf("从%d点开始到各点的最短路径为([下标]: (P)经过点 (w)权值):\n", start);
    for (i = 0; i < sort_path->num_vertexs; i ++) {
        if (sort_path->weight[i] == INFINITY)
            printf(" [%2d]: 不可到达\n", i);
        else {
            printf(" [%2d]: (p)%2d (w)%3d\n", i, sort_path->path[i], sort_path->weight[i]);
        }
    }
    int end = 8;
    printf("从%d到%d点的最小路径为:\n", start, end);
    if (sort_path->weight[end] == INFINITY) {
        printf("  不可达.\n");
    } else {
        int v = end; 
        int steps[MAX_VES] = {0};
        int count = 0;
        while (1) {
            steps[count] = v;
            v = sort_path->path[v];
            if (v == 0) {
                break;
            }
            count ++;
        }
        while (count >= 0) {
            printf("-> %d ", steps[count]);
            count --;
        }
        printf(" 权值和为: %d\n", sort_path->weight[end]);
    }
    free(sort_path);
    return 0;
}

