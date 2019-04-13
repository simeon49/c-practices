#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// 边表节点
typedef struct EdgeNode {
    int index;      // 邻接点域, 存储该项点的对应下标
    //int weight;   // 网图的权值
    struct EdgeNode *next;
} EdgeNode;

// 顶点节点
typedef struct VertexNode {
    int in_degree;  // 入度;
    char data;
    EdgeNode *first_edge;
} VertexNode;

#define MAX_VEX 100
typedef struct Graph {
    VertexNode vertex_list[MAX_VEX];
    int num_vertex; // 顶点数
    int num_edges;  // 边数
} Graph;

Graph g_graph;
void add_edge_into_vertex(VertexNode *vertex, int index)
{
    EdgeNode *edge = (EdgeNode *)malloc(sizeof(EdgeNode));
    assert(edge != NULL);
    edge->index = index;
    edge->next = vertex->first_edge;
    vertex->first_edge = edge;
    g_graph.vertex_list[index].in_degree ++;
}

void graph_init()
{
    // 参考<拓扑图.jpg>
    int i;
    g_graph.num_vertex = 6;
    g_graph.num_edges = 5;
    for (i = 0; i < g_graph.num_vertex; i ++) {
        g_graph.vertex_list[i].in_degree = 0;
        g_graph.vertex_list[i].data = 'a' + i;
        g_graph.vertex_list[i].first_edge = NULL;
    }
    add_edge_into_vertex(&g_graph.vertex_list[0], 2);
    add_edge_into_vertex(&g_graph.vertex_list[0], 1);
    add_edge_into_vertex(&g_graph.vertex_list[1], 3);
    add_edge_into_vertex(&g_graph.vertex_list[4], 5);
    add_edge_into_vertex(&g_graph.vertex_list[5], 1);
    for (i = 0; i < g_graph.num_vertex; i ++) {
        printf("index:%d,data='%c': ", i, g_graph.vertex_list[i].data);
        EdgeNode *edge = g_graph.vertex_list[i].first_edge;
        while (edge != NULL) {
            printf("->[(%d),'%c'] ", 
                edge->index, g_graph.vertex_list[edge->index].data);
            edge = edge->next;
        }
        printf("\n");
    }
}

void grap_destroy()
{
    int i;
    for (i = 0; i < g_graph.num_vertex; i ++) {
        EdgeNode *next, *edge = g_graph.vertex_list[i].first_edge;
        while (edge != NULL) {
            next = edge->next;
            free(edge);
            edge = next;
        }
    }
}

void topological_sort()
{
    int stack[MAX_VEX];
    int top = 0;
    int i;
    int count = 0;
    // 将所有入度为0的顶点 压入栈中
    for (i = 0; i < g_graph.num_vertex; i ++) {
        if (g_graph.vertex_list[i].in_degree == 0) {
            stack[top ++] = i;
            count ++;
        }
    }
    while (top != 0) {
        int index = stack[-- top];
        printf(" --> %c", g_graph.vertex_list[index].data);
        EdgeNode *edge = g_graph.vertex_list[index].first_edge;
        while (edge != NULL) {
            int k = edge->index;
            g_graph.vertex_list[k].in_degree --;
            if (g_graph.vertex_list[k].in_degree == 0) {
                stack[top ++] = k;
                count ++;
            }
            edge = edge->next;
        }
    }
    if (count != g_graph.num_vertex) {
        printf("  图中存在环路 不是AOV网\n");
    } else {
        printf("  OK\n");
    }
}

int main()
{
    graph_init();
    topological_sort();
    grap_destroy();
    return 0;
}
