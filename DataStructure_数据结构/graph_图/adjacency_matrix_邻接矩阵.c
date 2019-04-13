#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef char VertexType;
typedef int EdgeType;
typedef int bool;

#define true    1
#define false   0

#define MAX_VES 100     //最大顶点数
typedef struct {
    int queue[MAX_VES];
    int top;
    int tail;
} Queue;

Queue g_queue;
void queue_init(void)
{
    int i;
    g_queue.top = 0;
    g_queue.tail = 0;
}

bool queue_push(int index)
{
    if ((g_queue.tail + 1) % MAX_VES == g_queue.top) {
        printf("[queue] push error, queue is full.\n");
        return false;
    }
    g_queue.queue[g_queue.tail] = index;
    g_queue.tail = (g_queue.tail + 1) % MAX_VES;
    return true;
}

bool queue_get(int *p_index)
{
    if (g_queue.top == g_queue.tail) {
        //printf("[queue] get warn, queue is empty.\n");
        return false;
    }
    *p_index = g_queue.queue[g_queue.top];
    return true;
}

bool queue_pop(void)
{
    if (g_queue.top == g_queue.tail) {
        printf("[queue] get warn, queue is empty.\n");
        return false;
    }
    g_queue.top = (g_queue.top + 1) % MAX_VES;
    return true;
}


typedef struct {
    VertexType vertexs[MAX_VES];    // 顶点
    EdgeType edges[MAX_VES][MAX_VES];   //边
    int num_vertexs;    // 顶点总数
    int num_edges;      // 边总数
} MGraph;


MGraph *mgraph_init(void)
{
    MGraph *graph = (MGraph *)malloc(sizeof(MGraph));
    assert(graph != NULL);
    bzero(graph, sizeof(MGraph));
    // 创建一个无向图 图为<<大话数据结构.pdf 267页 图7-5-2>>
    int i;
    //   创建顶点
    graph->num_vertexs = 9;
    for (i = 0; i < graph->num_vertexs; i ++) {
        graph->vertexs[i] = 'A' + i;
    }
    graph->edges[0][1] = 1;
    graph->edges[0][5] = 1;
    graph->edges[1][0] = 1;
    graph->edges[1][2] = 1;
    graph->edges[1][6] = 1;
    graph->edges[1][8] = 1;
    graph->edges[2][1] = 1;
    graph->edges[2][3] = 1;
    graph->edges[2][8] = 1;
    graph->edges[3][2] = 1;
    graph->edges[3][4] = 1;
    graph->edges[3][6] = 1;
    graph->edges[3][7] = 1;
    graph->edges[3][8] = 1;
    graph->edges[4][3] = 1;
    graph->edges[4][5] = 1;
    graph->edges[4][7] = 1;
    graph->edges[5][0] = 1;
    graph->edges[5][4] = 1;
    graph->edges[5][6] = 1;
    graph->edges[6][1] = 1;
    graph->edges[6][3] = 1;
    graph->edges[6][5] = 1;
    graph->edges[6][7] = 1;
    graph->edges[7][3] = 1;
    graph->edges[7][4] = 1;
    graph->edges[7][6] = 1;
    graph->edges[8][1] = 1;
    graph->edges[8][2] = 1;
    graph->edges[8][3] = 1;
    graph->num_edges = 15;
    return graph;
}

void mgraph_destroy(MGraph *graph)
{
    free(graph);
}

bool _visited[MAX_VES];

void _mgraph_dfs(MGraph *graph, int index)
{
    int i;
    _visited[index] = true;
    printf(" %c", graph->vertexs[index]);
    for (i = 0; i < graph->num_vertexs; i ++) {
        if (graph->edges[index][i] == 1 && _visited[i] == false) {
            _mgraph_dfs(graph, i);
        }
    }
}

// 深度优先遍历
void mgraph_depth_first_traversal(MGraph *graph)
{
    int i;
    for (i = 0; i < graph->num_vertexs; i ++) {
        _visited[i] = false;
    }
    _mgraph_dfs(graph, 0);
    printf("\n");
}

// 广度优先遍历
void mgraph_breadth_first_traversal(MGraph *graph)
{
    int i;
    for (i = 0; i < graph->num_vertexs; i ++) {
        _visited[i] = false;
    }
    queue_init();
    queue_push(0);
    _visited[0] = true;
    while (1) {
        int index;
        if (!queue_get(&index)) {
            break;
        }
        //printf("-----%d\n", index);
        for (i = 0; i < graph->num_vertexs; i ++) {
            if (graph->edges[index][i] == 1 && _visited[i] == false) {
                queue_push(i);
                _visited[i] = true;
            }
        }
        printf(" %c", graph->vertexs[index]);
        queue_pop();
    }
    printf("\n");
}

int main()
{
    MGraph *graph = mgraph_init();
    printf("图的深度遍历:\n");
    mgraph_depth_first_traversal(graph);
    printf("图的广度遍历:\n");
    mgraph_breadth_first_traversal(graph);
    mgraph_destroy(graph);
    return 0;
}

