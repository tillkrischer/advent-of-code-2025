#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int target;
    int buttons[100];
    int no_buttons;
} Machine;
Machine machines[200];
int no_machines = 0;

typedef struct {
    int items[1024];
    int head;
    int tail;
} Queue;

bool parse_file(const char *filename);
void parse_input(FILE *f);
void parse_machine(char *line, Machine *m);
void print_machine(const Machine *m);
void enqueue(Queue *q, int item);
int dequeue(Queue *q);
bool is_empty(Queue *q);
void bfs(Machine *m, int *parents);
int find_min_presses(Machine *m);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return 1;
    }

    if (!parse_file(argv[1])) {
        return 1;
    }

    // for (int i = 0; i < no_machines; i++) {
    //     print_machine(&machines[i]);
    // }

    int total_presses = 0;
    for (int i = 0; i < no_machines; i++) {
        int presses = find_min_presses(&machines[i]);
        total_presses += presses;
    }

    printf("Total presses: %d\n", total_presses);

    return 0;
}

int find_min_presses(Machine *m) {
    int dists[1024];
    memset(dists, -1, sizeof(dists));
    bfs(m, dists);
    return dists[m->target];
}

void bfs(Machine *m, int *dists) {
    Queue q = {0};
    enqueue(&q, 0);
    dists[0] = 0;
    while (!is_empty(&q)) {
        int state = dequeue(&q);
        if (state == m->target) {
            return;
        }
        for (int i = 0; i < m->no_buttons; i++) {
            int next_state = state ^ m->buttons[i];
            if (dists[next_state] == -1) {
                dists[next_state] = dists[state] + 1;
                enqueue(&q, next_state);
            }
        }
    }
}

void enqueue(Queue *q, int item) {
    q->items[q->tail] = item;
    q->tail = (q->tail + 1) % 1024;
}

int dequeue(Queue *q) {
    int item = q->items[q->head];
    q->head = (q->head + 1) % 1024;
    return item;
}

bool is_empty(Queue *q) { return q->head == q->tail; }

int target_to_int(const char *s) {
    int result = 0;
    int mask = 1;
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == '#') {
            result |= mask;
        }
        mask <<= 1;
    }
    return result;
}

int button_to_int(const char *s) {
    int result = 0;
    char *token;
    char *str = strdup(s);
    token = strtok(str, ",");
    while (token != NULL) {
        int bit = atoi(token);
        result |= (1 << bit);
        token = strtok(NULL, ",");
    }
    free(str);
    return result;
}

void parse_machine(char *line, Machine *m) {
    char item[32];
    int n;
    sscanf(line, "[%[.#]]%n", item, &n);
    m->target = target_to_int(item);
    line += n;

    while (sscanf(line, " (%[0-9,])%n", item, &n)) {
        m->buttons[m->no_buttons] = button_to_int(item);
        m->no_buttons++;
        line += n;
    }
}

void print_machine(const Machine *m) {
    printf("Target: %d\n", m->target);
    printf("Buttons:\n");
    for (int i = 0; i < m->no_buttons; i++) {
        printf("  %d\n", m->buttons[i]);
    }
}

void parse_input(FILE *f) {
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        parse_machine(line, &machines[no_machines]);
        no_machines++;
    }
    printf("Parsed %d machines\n", no_machines);
}

bool parse_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return false;
    }

    parse_input(f);

    if (ferror(f)) {
        perror("read error");
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}
