#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define V 7

static const char NAMES[V] = {'A', 'B', 'C', 'D', 'E', 'F', 'G'};

/* ---------- Graph: adjacency matrix (0 = no direct link) ---------- */
static int adj[V][V];

static void add_link(char a, char b, int ms) {
    int i = a - 'A', j = b - 'A';
    adj[i][j] = ms;
    adj[j][i] = ms;          /* undirected */
}

static void build_network(void) {
    add_link('A', 'B', 6);
    add_link('A', 'D', 12);
    add_link('B', 'D', 5);
    add_link('B', 'C', 11);
    add_link('C', 'D', 17);
    add_link('C', 'G', 25);
    add_link('D', 'E', 22);
    add_link('D', 'F', 15);
    add_link('E', 'F', 10);
    add_link('F', 'G', 22);
}

/* ---------- Queue (array-based, circular) ---------- */
typedef struct {
    int items[V];
    int front, rear, count;
} Queue;

static void q_init(Queue *q)           { q->front = 0; q->rear = 0; q->count = 0; }
static int  q_empty(const Queue *q)    { return q->count == 0; }

static void q_enqueue(Queue *q, int x) {
    q->items[q->rear] = x;
    q->rear = (q->rear + 1) % V;
    q->count++;
}

static int q_dequeue(Queue *q) {
    int x = q->items[q->front];
    q->front = (q->front + 1) % V;
    q->count--;
    return x;
}

/* ---------- Input ---------- */
/* Returns the node index for a valid gateway name, or -1 if invalid */
static int read_gateway(void) {
    char line[64];
    printf("Enter starting gateway: ");
    if (fgets(line, sizeof line, stdin) == NULL)
        return -1;

    line[strcspn(line, "\r\n")] = '\0';

    /* Must be exactly one letter, A-G (case-insensitive) */
    if (strlen(line) != 1)
        return -1;
    char c = (char)toupper((unsigned char)line[0]);
    if (c < 'A' || c > 'A' + V - 1)
        return -1;
    return c - 'A';
}

/* ---------- BFS ---------- */
int main(void) {
    build_network();

    int start = read_gateway();
    if (start < 0) {
        printf("Invalid gateway. Valid gateways are: ");
        for (int i = 0; i < V; i++)
            printf("%c%s", NAMES[i], i < V - 1 ? ", " : "\n");
        return 1;
    }

    int visited[V] = {0};
    int hops[V] = {0};
    int order[V], n_order = 0;

    Queue q;
    q_init(&q);
    visited[start] = 1;
    q_enqueue(&q, start);

    while (!q_empty(&q)) {
        int u = q_dequeue(&q);
        order[n_order++] = u;
        for (int v = 0; v < V; v++) {
            if (adj[u][v] != 0 && !visited[v]) {
                visited[v] = 1;
                hops[v] = hops[u] + 1;
                q_enqueue(&q, v);
            }
        }
    }

    /* Task 2: BFS order and one-hop neighbours */
    printf("\nBFS traversal from %c:\n", NAMES[start]);
    for (int i = 0; i < n_order; i++)
        printf("  %c (hops from %c: %d)\n", NAMES[order[i]], NAMES[start], hops[order[i]]);

    printf("\nDirectly connected to %c (in BFS discovery order):\n", NAMES[start]);
    int direct_count = 0;
    for (int i = 0; i < n_order; i++) {
        int g = order[i];
        if (hops[g] == 1) {
            printf("  %c : %d ms\n", NAMES[g], adj[start][g]);
            direct_count++;
        }
    }

    if (direct_count == 0) {
        printf("  (none)\n");
        return 0;
    }

    /* Task 3: highest transfer time among direct links */
    int max_ms = 0;
    for (int i = 0; i < n_order; i++)
        if (hops[order[i]] == 1 && adj[start][order[i]] > max_ms)
            max_ms = adj[start][order[i]];

    printf("\nHighest data-transfer time among direct links:\n");
    for (int i = 0; i < n_order; i++)
        if (hops[order[i]] == 1 && adj[start][order[i]] == max_ms)
            printf("  Gateway %c : %d ms\n", NAMES[order[i]], max_ms);

    return 0;
}
