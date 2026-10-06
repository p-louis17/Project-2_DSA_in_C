#include <stdio.h>
#include <string.h>

#define MAX 32

typedef struct {
    char id[8];
    char name[32];
    int  priority;
} Patient;

typedef struct {
    Patient data[MAX];
    int size;
} Heap;

/* Swaps the whole record, so ID, name and priority always move together */
static void swap(Patient *a, Patient *b) {
    Patient t = *a;
    *a = *b;
    *b = t;
}

/* Sift down: used for build-heap, extraction and deletion */
static void heapify_down(Heap *h, int i) {
    for (;;) {
        int l = 2 * i + 1, r = 2 * i + 2, largest = i;
        if (l < h->size && h->data[l].priority > h->data[largest].priority) largest = l;
        if (r < h->size && h->data[r].priority > h->data[largest].priority) largest = r;
        if (largest == i) return;
        swap(&h->data[i], &h->data[largest]);
        i = largest;
    }
}

/* Sift up: used for insertion */
static void heapify_up(Heap *h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->data[p].priority >= h->data[i].priority) return;
        swap(&h->data[p], &h->data[i]);
        i = p;
    }
}

static void build_heap(Heap *h) {
    for (int i = h->size / 2 - 1; i >= 0; i--)
        heapify_down(h, i);
}

static int insert(Heap *h, const char *id, const char *name, int priority) {
    if (h->size >= MAX) return 0;
    Patient *p = &h->data[h->size];
    strncpy(p->id, id, sizeof p->id - 1);     p->id[sizeof p->id - 1] = '\0';
    strncpy(p->name, name, sizeof p->name - 1); p->name[sizeof p->name - 1] = '\0';
    p->priority = priority;
    h->size++;
    heapify_up(h, h->size - 1);
    return 1;
}

static int extract_max(Heap *h, Patient *out) {
    if (h->size == 0) return 0;
    *out = h->data[0];
    h->data[0] = h->data[h->size - 1];
    h->size--;
    if (h->size > 0) heapify_down(h, 0);
    return 1;
}

/* Delete any patient by ID: fill the hole with the last element, then fix order */
static int delete_by_id(Heap *h, const char *id) {
    int i;
    for (i = 0; i < h->size; i++)
        if (strcmp(h->data[i].id, id) == 0) break;
    if (i == h->size) return 0;

    h->data[i] = h->data[h->size - 1];
    h->size--;
    if (i < h->size) {
        heapify_down(h, i);
        heapify_up(h, i);
    }
    return 1;
}

static int is_max_heap(const Heap *h) {
    for (int i = 1; i < h->size; i++)
        if (h->data[(i - 1) / 2].priority < h->data[i].priority) return 0;
    return 1;
}

static void print_heap(const Heap *h, const char *title) {
    printf("\n=== %s ===\nArray:\n", title);
    for (int i = 0; i < h->size; i++)
        printf("  [%d] %s %-7s %d\n", i, h->data[i].id, h->data[i].name, h->data[i].priority);
    printf("Tree by level:\n");
    int level = 0, start = 0;
    while (start < h->size) {
        int end = start + (1 << level);
        if (end > h->size) end = h->size;
        printf("  Level %d: ", level);
        for (int i = start; i < end; i++)
            printf("%s %s(%d)  ", h->data[i].id, h->data[i].name, h->data[i].priority);
        printf("\n");
        start = end;
        level++;
    }
    printf("Max-Heap property valid: %s\n", is_max_heap(h) ? "yes" : "NO");
}

int main(void) {
    Heap h = { .size = 7 };
    Patient init[] = {
        {"P01", "Amina",  72}, {"P02", "Daniel", 45}, {"P03", "Eric",  91},
        {"P04", "Grace",  63}, {"P05", "Hassan", 88}, {"P06", "Irene", 54},
        {"P07", "Jean",   76}
    };
    for (int i = 0; i < 7; i++) h.data[i] = init[i];

    /* 1. Build */
    print_heap(&h, "Initial array (before heapify)");
    build_heap(&h);
    print_heap(&h, "1. After Max-Heap construction");

    /* 2. Treatment order: extract from a COPY so the live heap is kept */
    Heap copy = h;
    Patient p;
    printf("\n=== 2. Treatment order ===\n");
    while (extract_max(&copy, &p))
        printf("Patient %s (%s) - Priority %d\n", p.id, p.name, p.priority);

    /* 3. Insert Kofi */
    insert(&h, "P08", "Kofi", 98);
    print_heap(&h, "3. After inserting P08 Kofi (98)");

    /* 4. Delete P08 */
    delete_by_id(&h, "P08");
    print_heap(&h, "4. After removing P08");

    return 0;
}
