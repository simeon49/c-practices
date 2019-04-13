#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int start;
    int end;
    int weight;
} Edge;

#define MAX_VES 100
#define MAX_EDGES ((MAX_VES-1)*MAX_VES/2)
typedef struct {
    char vertexs[MAX_VES];  // 节点集合
    Edge edges[MAX_EDGES];  // 边集合
    int num_vertexs;        // 节点个数
    int num_edges;          // 边的个数
} MGraph;

MGraph g_graph = {
    {'0', '1', '2', '3', '4', '5', '6', '7', '8'},
    {
       //S, E,  W
        {0, 1, 10},
        {0, 5, 11},
        {1, 6, 16},
        {1, 8, 12},
        {1, 2, 18},
        {2, 8,  8},
        {2, 3, 22},
        {3, 8, 21},
        {3, 6, 24},
        {3, 7, 16},
        {3, 4, 20},
        {4, 7,  7},
        {4, 5, 26},
        {5, 6, 17},
        {6, 7, 19},
    },
    9,
    15,
};

int edge_cmp(const void *v1, const void *v2)
{
    Edge *edge1 = (Edge *)v1;
    Edge *edge2 = (Edge *)v2;
    if (edge1->weight > edge2->weight)
        return 1;
    else if (edge1->weight < edge2->weight)
        return -1;
    else
        return 0;
}

int find(int *parent, int f)
{
    while (parent[f] > 0)
        f = parent[f];
    return f;
}

void mini_span_tree_kruskal(MGraph graph)
{
    int parent[MAX_VES] = {0};
    int index;
    int count = graph.num_edges - 1;
    // 对图的边 按权重从小到大排序
    qsort(&graph.edges, graph.num_edges, sizeof(Edge), edge_cmp);
    /*
    int i;
    for (i = 0; i < graph.num_edges; i ++) {
        printf("[egde] start: %d, end: %d, weight: %d\n", 
            graph.edges[i].start, graph.edges[i].end,
            graph.edges[i].weight);
    }*/
    for (index = 0; index < graph.num_edges; index ++) {
        int n = find(parent, graph.edges[index].start);
        int m = find(parent, graph.edges[index].end);
        if (n != m) {
            printf("[egde] start: %d, end: %d, weight: %d\n", 
                graph.edges[index].start, graph.edges[index].end,
                graph.edges[index].weight);
            parent[n] = m;
            count --;
            if (count == 0)
                break;
        }
    }
}

int main()
{
    mini_span_tree_kruskal(g_graph);
    return 0;
}

