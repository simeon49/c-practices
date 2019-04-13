#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// 边表节点
typedef struct EdgeNode {
    int index;          // 邻接点域, 存储该项点的对应下标
    int weight;         // 权重
    struct EdgeNode *next;
} EdgeNode;

typedef struct VertexNode {
    int in_degree;
    char data;
    EdgeNode *first_edge;
} VertexNode;

#define MAX_VEX 100
typedef struct Graph {
    VertexNode vertex_list[MAX_VEX];
    int num_vertex;
    int num_edgs;
} Graph;
Graph g_graph;

void add_edge_into_vertex(VertexNode *vertex, int index, int weight)
{
    EdgeNode *edge = (EdgeNode *)malloc(sizeof(EdgeNode));
    assert(edge != NULL);
    edge->index = index;
    edge->weight = weight;
    edge->next = vertex->first_edge;
    vertex->first_edge = edge;
    g_graph.vertex_list[index].in_degree ++;
}

void graph_init()
{
    // 参考 <大话数据结构.pdf> 309页 图7-9-4
    g_graph.num_vertex = 10;
    g_graph.num_edgs = 13;
    int i;
    for (i = 0; i < g_graph.num_vertex; i ++) {
        g_graph.vertex_list[i].in_degree = 0;
        g_graph.vertex_list[i].data = '0' + i;
        g_graph.vertex_list[i].first_edge = NULL;
    }
    add_edge_into_vertex(&g_graph.vertex_list[0],1,3);
    add_edge_into_vertex(&g_graph.vertex_list[0],2,4);
    add_edge_into_vertex(&g_graph.vertex_list[1],3,5);
    add_edge_into_vertex(&g_graph.vertex_list[1],4,6);
    add_edge_into_vertex(&g_graph.vertex_list[2],3,8);
    add_edge_into_vertex(&g_graph.vertex_list[2],5,7);
    add_edge_into_vertex(&g_graph.vertex_list[3],4,3);
    add_edge_into_vertex(&g_graph.vertex_list[4],6,9);
    add_edge_into_vertex(&g_graph.vertex_list[4],7,4);
    add_edge_into_vertex(&g_graph.vertex_list[5],7,6);
    add_edge_into_vertex(&g_graph.vertex_list[6],9,2);
    add_edge_into_vertex(&g_graph.vertex_list[7],8,5);
    add_edge_into_vertex(&g_graph.vertex_list[8],9,3);
    for (i = 0; i < g_graph.num_vertex; i ++) {
        printf("index:%d,data='%c': ", i, g_graph.vertex_list[i].data);
        EdgeNode *edge = g_graph.vertex_list[i].first_edge;
        while (edge != NULL) {
            printf("->['%c',%dw] ", 
                g_graph.vertex_list[edge->index].data, edge->weight);
            edge = edge->next;
        }
        printf("\n");
    }
}

void graph_destroy()
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

int topological_sort(int *stack2, int *etv)
{
    int *stack;
    int top = 0;
    int i;
    stack = (int *)malloc(g_graph.num_vertex * sizeof(int));
    assert(stack != NULL);
    for (i = 0; i < g_graph.num_vertex; i ++) {
        if (g_graph.vertex_list[i].in_degree == 0)
            stack[top ++] = i;
    }
    int top2 = 0;
    for (i = 0; i < g_graph.num_vertex; i ++)
        etv[i] = 0;
    int count = 0;
    while (top != 0) {
        int index = stack[--top];
        EdgeNode *edge = g_graph.vertex_list[index].first_edge;
        while (edge != NULL) {
            int k = edge->index;
            g_graph.vertex_list[k].in_degree --;
            if (g_graph.vertex_list[k].in_degree == 0) {
                stack[top ++] = k;
            }
            if (etv[k] < etv[index] + edge->weight) {
                etv[k] = etv[index] + edge->weight;
            }
            edge = edge->next;
        }
        stack2[top2 ++] = index;
        count ++;
    }
    free(stack);
    if (count != g_graph.num_vertex) {
        printf("Error:  图中存在环路 不是AOV网\n");
        return -1;
    } else {
        printf("Stack2 (total:%d): \n", top2);
        for (i = 0; i < top2; i ++) {
            printf(" %d", stack2[i]);
        }
        printf("\n最早出发时间:\n");
        for (i = 0; i < g_graph.num_vertex; i ++) {
            printf(" ['%c'](%d) ", '0'+i, etv[i]);
        }
        printf("\n");
        return top2;
    }
}

void cratical_path()
{
    int i;
    int *etv, *ltv;
    int *stack2;
    int top2;
    stack2 = (int *)malloc(g_graph.num_vertex * sizeof(int));
    assert(stack2 != NULL);
    etv = (int *)malloc(g_graph.num_vertex * sizeof(int));
    assert(etv != NULL);
    ltv = (int *)malloc(g_graph.num_vertex * sizeof(int));
    assert(ltv != NULL);
    top2 = topological_sort(stack2, etv);
    if (top2 == -1) {
        printf("Error: topological_sort fail.\n");
        goto err;
    }
    for (i = 0; i < g_graph.num_vertex; i ++) {
        ltv[i] = etv[stack2[top2 - 1]];
    }
    while (top2 != 0) {
        int index = stack2[--top2];
        EdgeNode *edge = g_graph.vertex_list[index].first_edge;
        while (edge != NULL) {
            int k = edge->index;
            if (ltv[k] - edge->weight < ltv[index])
                ltv[index] = ltv[k] - edge->weight;
            edge = edge->next;
        }
    }
    printf("最晚出发时间:\n");
    for (i = 0; i < g_graph.num_vertex; i ++) {
        printf(" ['%c'](%d) ", '0'+i, ltv[i]);
    }
    printf("\n\n关键路径为:\n");
    for (i = 0; i < g_graph.num_vertex; i ++) {
        EdgeNode *edge = g_graph.vertex_list[i].first_edge;
        while (edge != NULL) {
            int k = edge->index;
            int ete = etv[i];
            int lte = ltv[k] - edge->weight;
            if (ete == lte) 
                printf("<v%c, v%c> weight:%d   ",
                g_graph.vertex_list[i].data, g_graph.vertex_list[k].data,
                edge->weight);
            edge = edge->next;   
        }
        printf("\n");
    }
    
err:
    free(ltv);
    free(etv);
    free(stack2);
}

int main()
{
    graph_init();
    cratical_path();
    graph_destroy();
    return 0;
}

