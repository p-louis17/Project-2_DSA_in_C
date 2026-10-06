#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define V 10                 /* data centers A..J */
#define MAX_EDGES 32
#define INF INT_MAX
#define NONE (-1)

typedef struct {
    int from, to, cost;
} Edge;

static const char NAMES[V] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};

/* ---------- Graph: directed edge list ---------- */
static Edge edges[MAX_EDGES];
static int n_edges = 0;

static void add_route(char from, char to, int cost) {
    edges[n_edges].from = from - 'A';
    edges[n_edges].to   = to - 'A';
    edges[n_edges].cost = cost;
    n_edges++;
}

static void build_network(void) {
    add_route('A', 'B',  6);
    add_route('A', 'D', 16);
    add_route('B', 'C',  6);
    add_route('B', 'D',  6);
    add_route('B', 'J',  7);
    add_route('C', 'G', -9);
    add_route('D', 'E',  7);
    add_route('D', 'J',  8);
    add_route('E', 'F', 10);
    add_route('E', 'I', -2);
    add_route('F', 'G',  4);
    add_route('F', 'I',  2);
    add_route('G', 'H', 13);
    add_route('I', 'F',  2);
    add_route('J', 'E',  3);
#ifdef TEST_NEG_CYCLE
    add_route('I', 'E', -3);   /* E -> I -> E has total cost -5 */
#endif
}

/* ---------- Input ---------- */
/* Returns node index for a valid data-center name, or -1 if invalid */
static int read_source(void) {
    char line[64];
    printf("Enter source data center: ");
    if (fgets(line, sizeof line, stdin) == NULL)
        return -1;

    line[strcspn(line, "\r\n")] = '\0';
    if (strlen(line) != 1)
        return -1;

    char c = (char)toupper((unsigned char)line[0]);
    if (c < 'A' || c > 'A' + V - 1)
        return -1;
    return c - 'A';
}

/* ---------- Bellman-Ford ---------- */
/* Returns 1 if a negative cycle reachable from src exists, else 0 */
static int bellman_ford(int src, int dist[], int pred[]) {
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        pred[i] = NONE;
    }
    dist[src] = 0;

    /* Relax every edge V-1 times; stop early if a pass changes nothing */
    for (int pass = 1; pass <= V - 1; pass++) {
        int changed = 0;
        for (int e = 0; e < n_edges; e++) {
            int u = edges[e].from, v = edges[e].to, w = edges[e].cost;
            /* Skip unreachable u so INF + w never overflows */
            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pred[v] = u;
                changed = 1;
            }
        }
        if (!changed) break;
    }

    /* V-th pass: any further improvement means a reachable negative cycle */
    for (int e = 0; e < n_edges; e++) {
        int u = edges[e].from, v = edges[e].to, w = edges[e].cost;
        if (dist[u] != INF && dist[u] + w < dist[v])
            return 1;
    }
    return 0;
}

/* ---------- Path reconstruction via predecessor array ---------- */
static void print_path(int dest, const int pred[]) {
    int stack[V], top = 0;
    for (int v = dest; v != NONE && top < V; v = pred[v])
        stack[top++] = v;

    for (int i = top - 1; i >= 0; i--)
        printf("%c%s", NAMES[stack[i]], i > 0 ? " -> " : "");
}

int main(void) {
    build_network();

    int src = read_source();
    if (src < 0) {
        printf("Invalid data center. Valid names are: ");
        for (int i = 0; i < V; i++)
            printf("%c%s", NAMES[i], i < V - 1 ? ", " : "\n");
        return 1;
    }

    int dist[V], pred[V];
    int neg_cycle = bellman_ford(src, dist, pred);

    printf("\nSource: %c\n\n", NAMES[src]);

    if (neg_cycle) {
        printf("Negative-weight cycle detected.\n");
        printf("Shortest-path results may be undefined.\n");
        return 2;
    }

    printf("No negative-weight cycle detected.\n\n");
    printf("%-12s %-14s %s\n", "Destination", "Shortest Cost", "Path");
    printf("%-12s %-14s %s\n", "-----------", "-------------", "----");

    for (int v = 0; v < V; v++) {
        if (v == src) continue;
        if (dist[v] == INF) {
            printf("%-12c %-14s %s\n", NAMES[v], "unreachable", "-");
        } else {
            printf("%-12c %-14d ", NAMES[v], dist[v]);
            print_path(v, pred);
            printf("\n");
        }
    }

    return 0;
}
