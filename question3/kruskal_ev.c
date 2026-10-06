#include <stdio.h>
#include <stdlib.h>

#define V 7
#define MAX_EDGES 32

typedef struct {
    int u, v, cost;
} Edge;

static const char NAMES[V] = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};

/* ---------- Union-Find (disjoint sets) ---------- */
static int parent[V], rnk[V];

static void uf_init(void) {
    for (int i = 0; i < V; i++) {
        parent[i] = i;
        rnk[i] = 0;
    }
}

static int uf_find(int x) {
    if (parent[x] != x)
        parent[x] = uf_find(parent[x]);   /* path compression */
    return parent[x];
}

/* Returns 1 if the sets were different and are now merged, 0 if same set (cycle) */
static int uf_union(int a, int b) {
    int ra = uf_find(a), rb = uf_find(b);
    if (ra == rb) return 0;
    if (rnk[ra] < rnk[rb])      parent[ra] = rb;
    else if (rnk[ra] > rnk[rb]) parent[rb] = ra;
    else { parent[rb] = ra; rnk[ra]++; }
    return 1;
}

/* ---------- Graph helpers ---------- */
static int cmp_edges(const void *a, const void *b) {
    return ((const Edge *)a)->cost - ((const Edge *)b)->cost;
}

static void add_edge(int m[V][V], char a, char b, int cost) {
    int i = a - 'A', j = b - 'A';
    m[i][j] = cost;
    m[j][i] = cost;      /* undirected: keep the matrix symmetric */
}

static void print_matrix(int m[V][V]) {
    printf("Adjacency matrix (0 = no direct connection):\n     ");
    for (int j = 0; j < V; j++) printf("%4c", NAMES[j]);
    printf("\n");
    for (int i = 0; i < V; i++) {
        printf("  %c  ", NAMES[i]);
        for (int j = 0; j < V; j++) printf("%4d", m[i][j]);
        printf("\n");
    }
}

/* Collect each undirected edge once (upper triangle) from the matrix */
static int matrix_to_edges(int m[V][V], Edge edges[]) {
    int n = 0;
    for (int i = 0; i < V; i++)
        for (int j = i + 1; j < V; j++)
            if (m[i][j] != 0)
                edges[n++] = (Edge){i, j, m[i][j]};
    return n;
}

int main(void) {
    int adj[V][V] = {0};

    add_edge(adj, 'A', 'B', 6);
    add_edge(adj, 'A', 'D', 12);
    add_edge(adj, 'B', 'D', 5);
    add_edge(adj, 'B', 'C', 11);
    add_edge(adj, 'C', 'D', 17);
    add_edge(adj, 'C', 'G', 25);
    add_edge(adj, 'D', 'E', 22);
    add_edge(adj, 'D', 'F', 15);
    add_edge(adj, 'E', 'F', 10);
    add_edge(adj, 'F', 'G', 22);

    print_matrix(adj);

    Edge edges[MAX_EDGES];
    int ne = matrix_to_edges(adj, edges);
    qsort(edges, ne, sizeof(Edge), cmp_edges);

    printf("\nKruskal's algorithm:\n");
    uf_init();

    Edge mst[V - 1];
    int count = 0, total = 0;

    for (int i = 0; i < ne && count < V - 1; i++) {
        Edge e = edges[i];
        if (uf_union(e.u, e.v)) {
            mst[count++] = e;
            total += e.cost;
            printf("  Select  %c - %c : %2d\n", NAMES[e.u], NAMES[e.v], e.cost);
        } else {
            printf("  Reject  %c - %c : %2d (would create a cycle)\n",
                   NAMES[e.u], NAMES[e.v], e.cost);
        }
    }

    printf("\nSelected Connections:\n");
    for (int i = 0; i < count; i++)
        printf("  Station %c - Station %c : %d\n",
               NAMES[mst[i].u], NAMES[mst[i].v], mst[i].cost);

    printf("\nEdges in MST: %d (expected %d)\n", count, V - 1);
    printf("Total Installation Cost: %d thousand dollars\n", total);

    return 0;
}
