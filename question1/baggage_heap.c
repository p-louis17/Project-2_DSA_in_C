#include <stdio.h>

#define MAX 32

typedef struct {
    char id;
    int  priority;
} Container;

static Container heap[MAX];
static int size = 0;

static void swap(Container *a, Container *b) {
    Container t = *a;   /* swaps ID and priority together */
    *a = *b;
    *b = t;
}

/* Sift down: used for build-heap and deletion */
static void heapify_down(int n, int i) {
    for (;;) {
        int l = 2 * i + 1, r = 2 * i + 2, largest = i;
        if (l < n && heap[l].priority > heap[largest].priority) largest = l;
        if (r < n && heap[r].priority > heap[largest].priority) largest = r;
        if (largest == i) return;
        swap(&heap[i], &heap[largest]);
        i = largest;
    }
}

/* Sift up: used for insertion */
static void heapify_up(int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap[p].priority >= heap[i].priority) return;
        swap(&heap[p], &heap[i]);
        i = p;
    }
}

static void build_heap(void) {
    for (int i = size / 2 - 1; i >= 0; i--)
        heapify_down(size, i);
}

static void insert(char id, int priority) {
    heap[size].id = id;
    heap[size].priority = priority;
    size++;
    heapify_up(size - 1);
}

/* Remove a container by ID: move last element into its slot, then restore order */
static int remove_by_id(char id) {
    int i;
    for (i = 0; i < size; i++)
        if (heap[i].id == id) break;
    if (i == size) return 0;

    heap[i] = heap[size - 1];
    size--;
    if (i < size) {
        heapify_down(size, i);
        heapify_up(i);
    }
    return 1;
}

static void print_heap(const char *title) {
    printf("\n=== %s ===\nArray: ", title);
    for (int i = 0; i < size; i++)
        printf("[%d]%c(%d) ", i, heap[i].id, heap[i].priority);
    printf("\nTree by level:\n");
    int level = 0, start = 0;
    while (start < size) {
        int end = start + (1 << level);
        if (end > size) end = size;
        printf("  Level %d: ", level);
        for (int i = start; i < end; i++)
            printf("%c(%d)  ", heap[i].id, heap[i].priority);
        printf("\n");
        start = end;
        level++;
    }
}

int main(void) {
    int P[] = {56, 23, 91, 34, 72, 48, 85, 17, 63, 79, 42};
    int n = sizeof(P) / sizeof(P[0]);

    for (int i = 0; i < n; i++) {
        heap[i].id = 'A' + i;
        heap[i].priority = P[i];
    }
    size = n;
    print_heap("Initial array (before heapify)");

    build_heap();
    print_heap("1. After Max-Heap construction");

    insert('X', 100);
    print_heap("2. After inserting X(100)");

    remove_by_id('X');
    print_heap("3. After removing X");

    return 0;
}
