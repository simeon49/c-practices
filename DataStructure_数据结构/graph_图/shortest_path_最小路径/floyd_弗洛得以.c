#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int bool;
               
#define true    1
#define false   0

#define MAX_VES 10     //最大顶点数

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
    int path[MAX_VES][MAX_VES];
    int edges[MAX_VES][MAX_VES];
    int num_vertexs;
} SortestPath;

void shortest_path_floyd(MGraph grap, SortestPath *sort_path)
{
    int i, j, k;
    sort_path->num_vertexs = grap.num_vertexs;
    for (i = 0; i < sort_path->num_vertexs; i ++) {
        for (j = 0; j < sort_path->num_vertexs; j ++) {
            sort_path->path[i][j] = j;
            sort_path->edges[i][j] = grap.edges[i][j];
        }
    }
    for (i = 0; i < sort_path->num_vertexs; i ++) {
        for (j = 0; j < sort_path->num_vertexs; j ++) {
            for (k = 0; k < sort_path->num_vertexs; k ++) {
                // 如果j点到i点的权加i点到k点的权 小于j点到k点的权
                if (sort_path->edges[j][i] + sort_path->edges[i][k] < sort_path->edges[j][k]) {
                    sort_path->edges[j][k] = sort_path->edges[j][i] + sort_path->edges[i][k];
                    sort_path->path[j][k] = sort_path->path[j][i];  // j点到k点 需要经过i点 ([j][i]为j点到i点的下一跳)
                }
            }
        }
    }
}

int main()
{
    SortestPath *sort_path = (SortestPath *)malloc(sizeof(SortestPath));
    assert(sort_path != NULL);
    shortest_path_floyd(g_grap, sort_path);
    printf("num: %d\n", sort_path->num_vertexs);
    printf("最短路径图:\n");
    int i, j;
    printf("   ");
    for (i = 0; i < sort_path->num_vertexs; i ++) {
        printf("[%d]", i);
    }
    for (i = 0; i < sort_path->num_vertexs; i ++) {
        printf("\n[%d]", i);
        for (j = 0; j < sort_path->num_vertexs; j ++) {
            printf(" %2d", sort_path->path[i][j]);
        }
    }
    printf("\n");
    printf("权值图:\n");
    printf("   ");
    for (i = 0; i < sort_path->num_vertexs; i ++) {
        printf("[%d]", i);
    }
    for (i = 0; i < sort_path->num_vertexs; i ++) {
        printf("\n[%d]", i);
        for (j = 0; j < sort_path->num_vertexs; j ++) {
            printf(" %2d", sort_path->edges[i][j]);
        }
    }
    printf("\n");
    free(sort_path);
    return 0;
}
